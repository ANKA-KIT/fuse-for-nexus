/*
 * TIFFProvider.h
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

#ifndef TIFFPROVIDER_H_
#define TIFFPROVIDER_H_

#include <sstream>
#include <tiffio.h>
#include <stdio.h>

/**
 *	Class used for store some required information while writing TIFF in stream.
 */
class tiffos_data
{
  public:
	std::ostream *myOS; /*!< The stream used to write TIFF into */
	long    myStreamStartPos;
};

class TIFFProvider {
private:
	static TIFF*
	_tiffStreamOpen(const char* name, const char* mode, void *fd);
	static void	_DummyUnmapProc(thandle_t , tdata_t , toff_t );
	static int	_DummyMapProc(thandle_t , tdata_t* , toff_t* );
	static int	_osCloseProc(thandle_t fd);
	static toff_t _osSizeProc(thandle_t fd);
	static toff_t _osSeekProc(thandle_t fd, toff_t off, int whence);
	static tsize_t	_osWriteProc(thandle_t fd, tdata_t buf, tsize_t size);
	static tsize_t	_osReadProc(thandle_t, tdata_t, tsize_t);
	//tiffos_data *private_data;

public:
	TIFFProvider();
	virtual ~TIFFProvider();
	static TIFF* StreamOpen(const char* name, void*);
	static void WriteTIFF(TIFF* into, void *data, int p_width, int p_height, uint bit, char photometric);

};

#endif /* TIFFPROVIDER_H_ */
