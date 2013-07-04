/*
 * ImageRule.cpp
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

#include "ImageRule.h"
#include "enums.h"

//std::map<std::string, char> ImageRule::photometric_values;

/**
 *	Constructor of ImageRule
 */
ImageRule::ImageRule() : photometric_values({
		 {"PHOTOMETRIC_MINISBLACK", PHOTOMETRIC_MINISBLACK},
		 {"PHOTOMETRIC_MINISWHITE", PHOTOMETRIC_MINISWHITE},
		 {"PHOTOMETRIC_RGB", PHOTOMETRIC_RGB},
		 {"PHOTOMETRIC_PALETTE", PHOTOMETRIC_PALETTE},
		 {"MINISBLACK", PHOTOMETRIC_MINISBLACK},
		 {"MINISWHITE", PHOTOMETRIC_MINISWHITE},
		 {"PALETTE", PHOTOMETRIC_PALETTE},
		 {"RGB", PHOTOMETRIC_RGB},
		 {"MASK", PHOTOMETRIC_MASK},
		 {"1", PHOTOMETRIC_MINISBLACK},
		 {"0", PHOTOMETRIC_MINISWHITE},
		 {"3", PHOTOMETRIC_PALETTE},
		 {"2", PHOTOMETRIC_RGB},
		 {"4", PHOTOMETRIC_MASK}
		}), imageRuleReadField({
		 {TypeID::UINT8, &ImageRule::readNXFieldUInt8},
		 {TypeID::UINT16, &ImageRule::readNXFieldUInt16},
		 {TypeID::UINT32, &ImageRule::readNXFieldUInt32},
		 {TypeID::FLOAT32, &ImageRule::readNXFieldFloat32},
		 {TypeID::FLOAT64, &ImageRule::readNXFieldFloat64},
		 {TypeID::FLOAT128, &ImageRule::readNXFieldFloat128},
		 {TypeID::INT8, &ImageRule::readNXFieldInt8},
		 {TypeID::INT16, &ImageRule::readNXFieldInt16},
		 {TypeID::INT32, &ImageRule::readNXFieldInt32},
		 {TypeID::COMPLEX32, &ImageRule::readNXFieldComplex32},
		 {TypeID::COMPLEX64, &ImageRule::readNXFieldComplex64},
		 {TypeID::COMPLEX128, &ImageRule::readNXFieldComplex128}
		})
{
	type = RuleType::IMAGE;
}

/**
 *	 Destructor of ImageRule
 */
ImageRule::~ImageRule() {

}


/**
 * 	@name Read NXField functions
 *  Read the data from NXField in a correct way.
 */
///@{
/**
 *	Pointer to this function is located in ImageRule#imageRuleReadField map,
 *	and it is called only via ImageRule#imageRuleReadField map. The reason why this functions were created is the following:\n
 *	To get data from \a nxfield we need to know the type of data stored. But we have only TypeID of data type. \n
 *	So this function calls readNXFieldImageRule<T>(pninx::NXField&) with right template parameter.
 */
std::string ImageRule::readNXFieldUInt8 (pninx::NXField& nxfield)
{
	return readNXFieldImageRule<UInt8>(nxfield);
}

std::string ImageRule::readNXFieldUInt16 (pninx::NXField& nxfield)
{
	return readNXFieldImageRule<UInt16>(nxfield);
}

std::string ImageRule::readNXFieldUInt32 (pninx::NXField& nxfield)
{
	return readNXFieldImageRule<UInt32>(nxfield);
}

std::string ImageRule::readNXFieldInt8 (pninx::NXField& nxfield)
{
	return readNXFieldImageRule<Int8>(nxfield);
}

std::string ImageRule::readNXFieldInt16 (pninx::NXField& nxfield)
{
	return readNXFieldImageRule<Int16>(nxfield);
}

std::string ImageRule::readNXFieldInt32 (pninx::NXField& nxfield)
{
	return readNXFieldImageRule<Int32>(nxfield);
}

std::string ImageRule::readNXFieldFloat32 (pninx::NXField& nxfield)
{
	return readNXFieldImageRule<Float32>(nxfield);
}

std::string ImageRule::readNXFieldFloat64 (pninx::NXField& nxfield)
{
	return readNXFieldImageRule<Float64>(nxfield);
}

std::string ImageRule::readNXFieldFloat128 (pninx::NXField& nxfield)
{
	return readNXFieldImageRule<Float128>(nxfield);
}

std::string ImageRule::readNXFieldComplex32 (pninx::NXField& nxfield)
{
	return readNXFieldImageRule<Complex32>(nxfield);
}

std::string ImageRule::readNXFieldComplex64 (pninx::NXField& nxfield)
{
	return readNXFieldImageRule<Complex64>(nxfield);
}

std::string ImageRule::readNXFieldComplex128 (pninx::NXField& nxfield)
{
	return readNXFieldImageRule<Complex128>(nxfield);
}
///@}

/**
 *	\brief Reads the data from NeXus objects and returns the representation of this data.
 *	\param [in] nxobject : the NeXus object to be represented.
 *	\return A representation that is ready to stored as a file.
 *
 *	Takes the data from NeXus object in accordance with the value type of stored information.\n
 *	Handles the data in accordance with ImageRule#options.
 */
std::string ImageRule::read(pninx::NXObject &nxobject)
{
	std::string output;

	if(nxobject.object_type() == pni::nx::NXObjectType::NXFIELD)
	{
		auto readField_iterator = imageRuleReadField.find( ( (pninx::NXField) nxobject ).type_id() );
		if(readField_iterator != imageRuleReadField.end() )
		{
			getImageRuleContent_t read_func = readField_iterator->second;
			pninx::NXField nxfield = (pninx::NXField) nxobject;
			output = (this->*read_func)( nxfield );
		}else
			output = "An error occurred, see log file \n";//todo log error
	}
	else
	{
		throw new NXFSException("ImageRule: nxobject is not a NXFIELD");
	}

	return output;
}

/*std::vector<std::string> ImageRule::readdir(pninx::NXObject &nxobject)
{
	std::vector<std::string> output;
	return output;
}*/

/**
 *	\brief Gets size.
 *	\param [in] nxobject : NeXus object to get size from.
 *	\return Size of representaion of \a nxobject.
 *
 */
size_t ImageRule::size(pninx::NXObject &nxobject)
{
	shape_t volume = ( (pninx::NXField) nxobject ).shape<shape_t>();
	uint8_t bit = 32;
	auto bit_it = this->options.find("bit");
	if( bit_it != this->options.end() )
	{
		std::istringstream( bit_it->second.c_str() ) >> bit;
	}
	size_t multiplier = bit/8;
	size_t sz = volume[1]*volume[2]*multiplier + 186;//magic number, about 186 bytes is "system" TIFF information

	return sz;
}

/**
 *	\brief Gets filesystem type of representation.
 *	\param [in] nxobject : NeXus object to get type from.
 *	\return FSType of representaion of \a nxobject.
 */
FSType ImageRule::getattr(pninx::NXObject &nxobject)
{
	return FSType::FILE;
}

/**
 *	\brief Facilitates the creation of subfiles.
 *	\param [in] nxobject : NeXus object which has to be a folder with subfiles.
 *	\return Number of subfiles, and extension for them.
 */
subFiles ImageRule::createSubFiles(pninx::NXObject &nxobject)
{
	subFiles output;

	if(nxobject.object_type() == pni::nx::NXObjectType::NXFIELD)
	{
		pninx::NXField nxfield = (pninx::NXField)nxobject;

		int rank = nxfield.rank();
		if(rank == 3)
		{
			size_t entityNum = nxfield.shape<shape_t>()[0];
			output.num = entityNum;
			output.error_msg = "";
			output.extension = this->options.find("extension")->second.c_str();
		}
		else
		{
			//todo log error
			output.num = 0;
			output.error_msg = "NXFIELD rank is not appropriate";
		}
	}
	else
	{
		//todo log error
		output.num = 0;
		std::string err_msg = "nxobject";
		err_msg += nxobject.path();
		err_msg += " is not NXFIELD";
		output.error_msg = err_msg.c_str();
	}

	return output;
}

