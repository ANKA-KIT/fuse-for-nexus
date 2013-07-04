/*
 * ImageRule.h
 *
 *  Created on: Mar 18, 2013
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

#ifndef IMAGERULE_H_
#define IMAGERULE_H_

#include <stdio.h>
#include <iostream>

#include "Rule.h"
#include "enums.h"
#include "TIFFProvider.h"
#include "NXFSException.h"
#include "NXGateway.h"
#include <pni/nx/NX.hpp>
#include <pni/utils/Types.hpp>

/**
 *	Class handles the representation of the NeXus data and creates TIFFs from it.
 */
class ImageRule: public Rule {
private:
	/**
	 *	Typedef pointer to function to store functions in a map.
	 */
	typedef std::string (ImageRule::*getImageRuleContent_t) (pninx::NXField& nxfield);

	const std::map<std::string, char> photometric_values; /*!< list of supported photometric values from XML file. Key is an expected value from XML file, and value is correct value that will be written in TIFF tags. */
	/**
	 *	This map is designed to call correct template function, due to the fact that we know the type we need to pass into template in runtime only.
	 */
	const std::map<TypeID, getImageRuleContent_t> imageRuleReadField;


	/**
	 * 	\brief Gets the TIFF representation of NXField.
	 *	\param [in] nxfield : the NeXus Field that has to be represented as a TIFF file.
	 *	\return The content of file that can be stored as a TIFF.
	 *
	 *	The template function which cause such a non readable solution with map of functions.\n
	 *	It gets the NeXus Field data in accordance with type that passed as a template parameter.
	 *	Then it gets options how the TIFF file should be displayed (bit, photometric...).
	 *	And writes the TIFF file into memory.
	 */
	template<typename T>
	std::string readNXFieldImageRule (pninx::NXField& nxfield)
	{
		int bit = 32;
		int colormetric = PHOTOMETRIC_MINISBLACK;
		uint part_num = 0;

		auto bit_it = this->options.find("bit");
		if( bit_it != this->options.end() )
		{
			std::istringstream(bit_it->second.c_str()) >> bit;
			if(bit < 8)
			{
				bit = 32;
				/*ErrorLog::log_xml_error_msg("Amount of bits cannot be less than 8. The default value 32 bits will be set up.",
						"unknown", nxfield.path().c_str());*/
			}
		}

		auto photo_option_it = this->options.find("colormetric");
		if( photo_option_it != this->options.end() )
		{
			auto photo_it = photometric_values.find(photo_option_it->second);
			if( photo_it != photometric_values.end() )
				colormetric = photo_it->second;
		}//todo: log else case

		auto part_num_it = this->options.find("part_number");
		if( part_num_it != this->options.end() )
			part_num = atoi( part_num_it->second.c_str() );
		//todo substitute with ostringstream
		//convert str to int
		/*else
			int a =6;//todo: write error in log file*/

		// getting the file content from nexus field
		size_t width = nxfield.shape<shape_t>()[1];
		size_t height = nxfield.shape<shape_t>()[2];

		DArray<T> data( shape_t{ width, height } );
		nxfield( part_num ,Slice( 0, width ), Slice( 0, height ) ).read( data );

		// rotate array %)
		/*T content[width][height];
			for(size_t i=0;i<width;i++)
				for(size_t j=0;j<height;j++)
					content[i][j] = data[j*1024+i];*/

		std::ostringstream outputTIFF;

		TIFF *input = TIFFProvider::StreamOpen("mem_TIFF", &outputTIFF);
		TIFFProvider::WriteTIFF(input, (void*)( data.storage().ptr() ) , width, height, bit, colormetric);

		return outputTIFF.str();
	}

	virtual std::string readNXFieldInt8 (pninx::NXField& nxfield);
	virtual std::string readNXFieldInt16 (pninx::NXField& nxfield);
	virtual std::string readNXFieldInt32 (pninx::NXField& nxfield);
	virtual std::string readNXFieldUInt8 (pninx::NXField& nxfield);
	virtual std::string readNXFieldUInt16 (pninx::NXField& nxfield);
	virtual std::string readNXFieldUInt32 (pninx::NXField& nxfield);
	virtual std::string readNXFieldFloat32 (pninx::NXField& nxfield);
	virtual std::string readNXFieldFloat64 (pninx::NXField& nxfield);
	virtual std::string readNXFieldFloat128 (pninx::NXField& nxfield);
	virtual std::string readNXFieldComplex32 (pninx::NXField& nxfield);
	virtual std::string readNXFieldComplex64 (pninx::NXField& nxfield);
	virtual std::string readNXFieldComplex128 (pninx::NXField& nxfield);

public:
	ImageRule();
	virtual ~ImageRule();

	virtual subFiles createSubFiles(pninx::NXObject &nxobject);


	//fuse methods
	virtual FSType getattr(pninx::NXObject &nxobject);
	//virtual std::vector<std::string> readdir(pninx::NXObject &nxobject);
	virtual std::string read(pninx::NXObject &nxobject);
	virtual size_t size(pninx::NXObject &nxobject);
};

#endif /* IMAGERULE_H_ */
