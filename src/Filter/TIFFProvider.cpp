/*
 * TIFFProvider.cpp
 *
 *  Created on: Feb 18, 2013
 *  Author: Egor Iurchenko <egor.iurchenko@kit.edu> (Karlsruher Institut für Technologie)
 *  NXFS. FUSE for NeXus files with NeXus data filtering based on rules stored in xml file.
 *  Copyright (C) 2013 Karlsruher Institut für Technologie (KIT)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see http://www.gnu.org/licenses/.
 */

#include "TIFFProvider.h"
#include <iostream>
#include <sstream>
#include <cstring>

/**
 * TIFFProvider constructor
 */
TIFFProvider::TIFFProvider() {}

/**
 * TIFFProvider destructor
 */
TIFFProvider::~TIFFProvider() {}

/**
 * \brief Writes TIFF image with predefined settings.
 *
 * \param [in] into : pointer to TIFF to write into.
 * \param [in] data : 2D array of values. Value should be the same bit as \a bit passed.
 * \param [in] p_width : width of image.
 * \param [in] p_height : height of image.
 * \param [in] bit : number of bits per sample. Should be multiple of 8.
 * \param [in] photometric : photometric according to TIFF 6.0 specification (http://partners.adobe.com/public/developer/en/tiff/TIFF6.pdf).
 */
void TIFFProvider::WriteTIFF(TIFF* into, void *data, int p_width, int p_height, uint bit, char photometric)
{
	uint multiplier = bit/8;
	char SamplesPerPixel = 1;
	if(photometric == PHOTOMETRIC_RGB)
	{
		multiplier = 3;//*=3 if there is more than 8 bits
		SamplesPerPixel = 3;
		bit = 8;//is it crucial?
	}
	TIFFSetField(into, TIFFTAG_IMAGEWIDTH, p_width);
	TIFFSetField(into, TIFFTAG_IMAGELENGTH, p_height);
	TIFFSetField(into, TIFFTAG_BITSPERSAMPLE, bit);
	TIFFSetField(into, TIFFTAG_SAMPLESPERPIXEL, SamplesPerPixel);
	TIFFSetField(into, TIFFTAG_ROWSPERSTRIP, p_width);
	TIFFSetField(into, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT );

	//TIFFSetField(into, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE); // compression gzip
	TIFFSetField(into, TIFFTAG_PHOTOMETRIC, photometric);
	TIFFSetField(into, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG); // ?

	TIFFSetField(into, TIFFTAG_XRESOLUTION, 150.0);
	TIFFSetField(into, TIFFTAG_YRESOLUTION, 150.0);
	TIFFSetField(into, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

	if(photometric == PHOTOMETRIC_PALETTE)
	{
		// fill color tables
		ushort* redTable = new ushort[1 << bit];
		ushort* greenTable = new ushort[1 << bit];
		ushort* blueTable = new ushort[1 << bit];
		for (int i = 0; i < 3; ++i)
		{
			redTable[i] = (ushort)(100 * i);
			greenTable[i] = (ushort)(150 * i);
			blueTable[i] = (ushort)(200 * i);
		}
		TIFFSetField(into, TIFFTAG_COLORMAP, redTable, greenTable, blueTable);
		//todo:hope it works, i'll test it later
	}

	TIFFWriteEncodedStrip(into, 0, data, p_width*p_height*multiplier);

	TIFFClose(into);
}

/**
 *	\brief Creates TIFF in memory.
 *
 *	\param [in] name : the name of TIFF.
 *	\param [out] os : it should be ostream. As the result it will contain the TIFF image.
 *	\return pointer to current TIFF
 */
TIFF* TIFFProvider::StreamOpen(const char* name, void* os)
{
	os = (std::ostream*)os;
	// If os is either a ostrstream or ostringstream, and has no data
	// written to it yet, then tellp() will return -1 which will break us.
	// We workaround this by writing out a dummy character and
	// then seek back to the beginning.
	if( !((std::ostream*)os)->fail()
			&& (int)((std::ostream*)os)->tellp() < 0 ) {
			*((std::ostream*)os) << '\0';
			((std::ostream*)os)->seekp(0);
	}

	// NB: We don't support mapped files with streams so add 'm'
	return _tiffStreamOpen(name, "wm", os);
}


tsize_t
TIFFProvider::_osReadProc(thandle_t, tdata_t, tsize_t)
{
        return 0;
}

tsize_t
TIFFProvider::_osWriteProc(thandle_t fd, tdata_t buf, tsize_t size)
{
        tiffos_data     *data = (tiffos_data *)fd;
        std::ostream         *os = data->myOS;
        int             pos = os->tellp();

        os->write((const char *)buf, size);

        return ((int)os->tellp()) - pos;
}

toff_t
TIFFProvider::_osSeekProc(thandle_t fd, toff_t off, int whence)
{
        tiffos_data     *data = (tiffos_data *)fd;
        std::ostream *os = data->myOS;

        // if the stream has already failed, don't do anything
        if( os->fail() )
                return os->tellp();

        switch(whence) {
        case SEEK_SET:
            os->seekp(data->myStreamStartPos + off, std::ios::beg);
                break;
        case SEEK_CUR:
                os->seekp(off, std::ios::cur);
                break;
        case SEEK_END:
                os->seekp(off, std::ios::end);
                break;
        }

        // Attempt to workaround problems with seeking past the end of the
        // stream.  ofstream doesn't have a problem with this but
        // ostrstream/ostringstream does. In that situation, add intermediate
        // '\0' characters.
        if( os->fail() ) {
#ifdef __VMS
                int             old_state;
#else
                std::ios::iostate    old_state;
#endif
                toff_t          origin=0;

                old_state = os->rdstate();
                // reset the fail bit or else tellp() won't work below
                os->clear(os->rdstate() & ~std::ios::failbit);
                switch( whence ) {
                        case SEEK_SET:
                                origin = data->myStreamStartPos;
                                break;
                        case SEEK_CUR:
                                origin = os->tellp();
                                break;
                        case SEEK_END:
                                os->seekp(0, std::ios::end);
                                origin = os->tellp();
                                break;
                }
                // restore original stream state
                os->clear(old_state);

                // only do something if desired seek position is valid
                if( origin + off > data->myStreamStartPos ) {
                        toff_t  num_fill;

                        // clear the fail bit
                        os->clear(os->rdstate() & ~std::ios::failbit);

                        // extend the stream to the expected size
                        os->seekp(0, std::ios::end);
                        num_fill = origin + off - (toff_t)os->tellp();
                        for( toff_t i = 0; i < num_fill; i++ )
                                os->put('\0');

                        // retry the seek
                        os->seekp(origin + off, std::ios::beg);
                }
        }

        return os->tellp();
}


toff_t
TIFFProvider::_osSizeProc(thandle_t fd)
{
        tiffos_data     *data = (tiffos_data *)fd;
        std::ostream         *os = data->myOS;
        toff_t          pos = os->tellp();
        toff_t          len;

        os->seekp(0, std::ios::end);
        len = os->tellp();
        os->seekp(pos);

        return len;
}

int
TIFFProvider::_osCloseProc(thandle_t fd)
{
        // Our stream was not allocated by us, so it shouldn't be closed by us.
        delete (tiffos_data *)fd;
        return 0;
}

int
TIFFProvider::_DummyMapProc(thandle_t , tdata_t* , toff_t* )
{
        return (0);
}

void
TIFFProvider::_DummyUnmapProc(thandle_t , tdata_t , toff_t )
{
}


TIFF*
TIFFProvider::_tiffStreamOpen(const char* name, const char* mode, void *fd)
{
        TIFF*   tif;
        //open stream for writing only
        if( strchr(mode, 'w') ) {
        		tiffos_data     *private_data = new tiffos_data;
                private_data->myOS = (std::ostream *)fd;
                private_data->myStreamStartPos = private_data->myOS->tellp();

                // Open for writing.
                tif = TIFFClientOpen(name, mode,
                                (thandle_t) private_data,
                                _osReadProc, _osWriteProc,
                                _osSeekProc, _osCloseProc,
                                _osSizeProc,
                                _DummyMapProc, _DummyUnmapProc);
        }

        return (tif);
}

