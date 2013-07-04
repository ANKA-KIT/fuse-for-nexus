/*
 * Rule.cpp
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

#include "Rule.h"
#include "../ErrorLog.h"

Rule::Rule() : ruleReadField({
	{TypeID::UINT8, &Rule::readNXFieldUInt8},
	{TypeID::UINT16, &Rule::readNXFieldUInt16},
	{TypeID::UINT32, &Rule::readNXFieldUInt32},
	{TypeID::INT8, &Rule::readNXFieldInt8},
	{TypeID::INT16, &Rule::readNXFieldInt16},
	{TypeID::INT32, &Rule::readNXFieldInt32},
	{TypeID::FLOAT32, &Rule::readNXFieldFloat32},
	{TypeID::FLOAT64, &Rule::readNXFieldFloat64},
	{TypeID::FLOAT128, &Rule::readNXFieldFloat128},
	{TypeID::COMPLEX32, &Rule::readNXFieldComplex32},
	{TypeID::COMPLEX64, &Rule::readNXFieldComplex64},
	{TypeID::COMPLEX128, &Rule::readNXFieldComplex128},
	{TypeID::STRING, &Rule::readNXFieldString},
	{TypeID::BINARY, &Rule::readNXFieldBinary},
	{TypeID::BOOL, &Rule::readNXFieldBoolean},
	{TypeID::NONE, &Rule::readNXFieldBinary}
}), types_size({
	{TypeID::UINT8, 7},
	{TypeID::INT8, 4},
	{TypeID::UINT16, 18},
	{TypeID::INT16, 13},
	{TypeID::UINT32, 41},
	{TypeID::INT32, 30},
	{TypeID::FLOAT32, 52},
	{TypeID::FLOAT64, 64},
	{TypeID::FLOAT128, 77},
	{TypeID::COMPLEX32, 82},
	{TypeID::COMPLEX64, 87},
	{TypeID::COMPLEX128, 92},
	{TypeID::STRING, 2048},
	{TypeID::BINARY, 8},
	{TypeID::BOOL, 1},
	{TypeID::NONE, 8}
})
{
	type = RuleType::PLAINDATA;
	required_options.insert("fsobject_type");
}

Rule::~Rule() {}


/**
 * 	@name Read NXField functions
 *  Read the data from NXField in a correct way.
 */
///@{
/**
 *	Pointer to this function is located in Rule#ruleReadField map,
 *	and it is called only via Rule#ruleReadField map. The reason why this functions were created is the following:\n
 *	To get data from \a nxfield we need to know the type of data stored. But we have only TypeID of data type. \n
 *	So this function calls readNXFieldRule<T>(pninx::NXField&) with right template parameter.
 */
std::string Rule::readNXFieldInt8 (pninx::NXField& nxfield)
{
	return readNXFieldRule<Int8>(nxfield);
}

std::string Rule::readNXFieldInt16 (pninx::NXField& nxfield)
{
	return readNXFieldRule<Int16>(nxfield);
}

std::string Rule::readNXFieldInt32 (pninx::NXField& nxfield)
{
	return readNXFieldRule<Int32>(nxfield);
}

std::string Rule::readNXFieldUInt8 (pninx::NXField& nxfield)
{
	return readNXFieldRule<UInt8>(nxfield);
}

std::string Rule::readNXFieldUInt16 (pninx::NXField& nxfield)
{
	return readNXFieldRule<UInt16>(nxfield);
}

std::string Rule::readNXFieldUInt32 (pninx::NXField& nxfield)
{
	return readNXFieldRule<UInt32>(nxfield);
}

std::string Rule::readNXFieldFloat32 (pninx::NXField& nxfield)
{
	return readNXFieldRule<Float32>(nxfield);
}

std::string Rule::readNXFieldFloat64 (pninx::NXField& nxfield)
{
	return readNXFieldRule<Float64>(nxfield);
}

std::string Rule::readNXFieldFloat128 (pninx::NXField& nxfield)
{
	return readNXFieldRule<Float128>(nxfield);
}

std::string Rule::readNXFieldComplex32 (pninx::NXField& nxfield)
{
	return readNXFieldRule<Complex32>(nxfield);
}
std::string Rule::readNXFieldComplex64 (pninx::NXField& nxfield)
{
	return readNXFieldRule<Complex64>(nxfield);
}
std::string Rule::readNXFieldComplex128 (pninx::NXField& nxfield)
{
	return readNXFieldRule<Complex128>(nxfield);
}

std::string Rule::readNXFieldString (pninx::NXField& nxfield)
{
	return readNXFieldRule<String>(nxfield);
}

std::string Rule::readNXFieldBinary (pninx::NXField& nxfield)
{
	return readNXFieldRule<Binary>(nxfield);
}

std::string Rule::readNXFieldBoolean (pninx::NXField& nxfield)
{
	return readNXFieldRule<Bool>(nxfield);
}
///@}

/**
 *	\brief Adds an option to Rule#options list.
 *	\param [in] key : the name of option.
 *	\param [in] value : the value of option.
 */
void Rule::addOption(std::string key, std::string value)
{
	this->options.insert(std::pair<std::string, std::string> (key, value));
}

/**
 *	\brief Reads the representation of \a nxobject passed.
 *	\param [in] nxobject : NeXus object that has to be represented.
 *	\return File content.
 *
 *	The representation of NeXus object depends on it's value type and on Rule#options that are set.
 */
std::string Rule::read(pninx::NXObject& nxobject)
{
	std::string str;
	if(nxobject.object_type() == pni::nx::NXObjectType::NXFIELD)
	{
		auto readField_iterator = ruleReadField.find( ( ( pninx::NXField) nxobject ).type_id() );
		if(readField_iterator != ruleReadField.end() )
		{
			readNXField_t read_func = readField_iterator->second;
			pninx::NXField nxfield = (pninx::NXField) nxobject;
			str = (this->*read_func)( nxfield );
		}else
		{
			str = "An error occurred, see log file \n";
			ErrorLog::log_write("NXObject %s has unknown array value type: %d", nxobject.path().c_str(), ( ( pninx::NXField) nxobject ).type_id() );
		}
	}
	return str;
}

/**
 *	\brief Gets the type of file system object.
 *	\param [in] nxobject : NeXus object that has to be represented as an object of filesytem.
 *	\return The type of filesystem object.
 */
FSType Rule::getattr(pninx::NXObject& nxobject)
{
	std::map<std::string,std::string>::iterator option_fstype = options.find("fsobject_type");

	if(option_fstype != options.end())
	{
		if( option_fstype->second == "FOLDER" ) return FSType::FOLDER;
		else if( option_fstype->second == "FILE" )   return FSType::FILE;
	}
	else
	{
		if( nxobject.object_type() == pni::nx::NXObjectType::NXGROUP )
			return FSType::FOLDER;
		else
			return FSType::FILE;
	}
	return FSType::NONE;
}

/*std::vector<std::string> Rule::readdir(pninx::NXObject& nxobject)
{
	std::vector<std::string> output;

	if(nxobject.object_type() == pni::nx::NXObjectType::NXGROUP)
	{
		for(pninx::NXObject& obj : (pninx::NXGroup&)nxobject)
		{
			output.push_back(obj.name());
		}
	}

	return output;
}*/

/**
 *	\brief Gets size of file content.
 *	\param [in] nxobject : The NeXus object that has to be represented.
 *	\return The size of representation.
 */
size_t Rule::size(pninx::NXObject& nxobject)
{
	size_t sz =0;
	if(nxobject.object_type() == pni::nx::NXObjectType::NXFIELD)
	{
		sz = ((pninx::NXField)nxobject).size();

		//we trying to guess how much characters may be in file
		//so we check what type of values and how many entities of these values we have
		//then just multiply amount of entities on max number of characters for this value type
		auto type = ((pninx::NXField)nxobject).type_id();
		auto entity_size_it = types_size.find(type);
		if(entity_size_it != types_size.end())
		{
			sz += sz*entity_size_it->second;
		}
	}
	return sz;
}

/**
 *	\brief Adds or removes the extension as a mandatory option, in accordance with the fsobject_type
 */
void Rule::correctOptions()
{
	auto it = options.find("fsobject_type");
	if(it != options.end())
	{
		if(it->second == "FILE")
		{
			required_options.insert("extension");
		}
		else
		{
			auto pos = required_options.find("extension");
			if(pos != required_options.end() )
				required_options.erase(pos);
		}

	}
}

/**
 *	\brief Sets Rule#options with another %map.
 *	\param [in] input : the options needed to be set.
 */
void Rule::setOptions(std::map<std::string, std::string> input)
{
	this->options = std::move(input);
	correctOptions();
}

/**
 *	\brief Gets the amount of files to be created and extension of them.
 *	\param [in] nxobject : NeXus object.
 *	\return valid or not subFile structure.
 */
subFiles Rule::createSubFiles(pninx::NXObject &nxobject)
{
	subFiles output;
	std::string err_msg = "Cannot create subfiles for ";
	err_msg += nxobject.path();
	output.error_msg = err_msg.c_str();
	output.num = 0;
	return output;
}


/**
 *	\brief Changes the entry in Rule#options.
 *	\param [in] type : The FSType to be set.
 *
 *	Checks the items in Rule#options, if there is option "fsobject_type" it changes it in accordance with \a type,\n
 *	if there is no such options it inserts it with value as passed parameter \a type.
 */
void Rule::changeFSType(FSType type)
{
	std::string fstype_str;
	if(type == FSType::FOLDER)
	{
		auto pos = required_options.find("extension");
		if(pos != required_options.end() )
			required_options.erase(pos);
		fstype_str = "FOLDER";
	}
	else
	{
		required_options.insert("extension");
		fstype_str = "FILE";
	}

	if( this->options.find("fsobject_type") == this->options.end() )
	{
		this->addOption( "fsobject_type", fstype_str );
	}
	else
	{
		this->options.find( "fsobject_type" )->second = fstype_str;
	}

}


/**
 *	\brief Gets option value by option name.
 *	\param [in] option_name : Option name (key).
 *	\return The value of option found or empty string if not found.
 */
std::string Rule::getOptionValue(const char* option_name)
{
	auto it = options.find(option_name);
	if( it != options.end() )
	{
		return it->second;
	}
	else
	{
		//todo log this error
		std::string empty;
		return empty;
	}
}

/**
 *	\brief Checks if there are all mandatory options in Rule#options list.
 *	\return True if all mandatory options found, else False.
 */
bool Rule::isValidOptions()
{
	for(auto it=required_options.begin();it!=required_options.end();it++)
		if(options.find(*it) == options.end())
			return false;
	return true;
}
