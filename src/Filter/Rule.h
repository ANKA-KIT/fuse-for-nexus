/*
 * Rule.h
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

#ifndef RULE_H_
#define RULE_H_
#include "enums.h"
#include <pni/io/nx/nx.hpp>
#include <stdio.h>
#include <map>
#include <iostream>
#include <vector>
#include <memory>
#include <utility>
#include <set>

namespace pninx=pni::io::nx::h5;
using namespace pni::core;

/**
 *	Structure helps to pass data about subfiles from any Rule to Filter.
 */
struct subFiles
{
	const char* error_msg; /*!< If there is error this string will contain the explanation of error. */
	size_t num; /*!< The number of files needed to be created. */
	const char* extension; /*!< The extension of files. */

	/**
	 *	\brief Checks the validity of structure.
	 *	\return True if valid or false if there is an error.
	 *
	 *	In case of error there is \a error_msg that explains the reason of error.
	 */
	bool isValid()	{	return ( !( strlen( error_msg ) > 0 ) ); }
};


class Rule {
private:
	/**
	 *	Typedef pointer to function to store functions in a map.
	 */
	typedef std::string (Rule::*readNXField_t) (pninx::nxfield& nxfield);
	/**
	 *	This map is designed to call correct template function, due to the fact that we know the type we need to pass into template in runtime only.
	 */
	const std::map<type_id_t, readNXField_t> ruleReadField;

	std::string readNXFieldInt8 (pninx::nxfield& nxfield);
	std::string readNXFieldInt16 (pninx::nxfield& nxfield);
	std::string readNXFieldInt32 (pninx::nxfield& nxfield);
	std::string readNXFieldUInt8 (pninx::nxfield& nxfield);
	std::string readNXFieldUInt16 (pninx::nxfield& nxfield);
	std::string readNXFieldUInt32 (pninx::nxfield& nxfield);
	std::string readNXFieldFloat32 (pninx::nxfield& nxfield);
	std::string readNXFieldFloat64 (pninx::nxfield& nxfield);
	std::string readNXFieldFloat128 (pninx::nxfield& nxfield);
	std::string readNXFieldComplex32 (pninx::nxfield& nxfield);
	std::string readNXFieldComplex64 (pninx::nxfield& nxfield);
	std::string readNXFieldComplex128 (pninx::nxfield& nxfield);
	std::string readNXFieldString (pninx::nxfield& nxfield);
	std::string readNXFieldBinary (pninx::nxfield& nxfield);
	std::string readNXFieldBoolean (pninx::nxfield& nxfield);

	template<typename T>
	std::string writeDimension(darray<T>& data, shape_t shape, size_t rank, size_t depth=0)//,size_t offset=0)
	{
		std::ostringstream stream;
		for(size_t f=0;f<shape[depth];f++)//0, 1
		{
			size_t _offset=1;
			for(size_t k=depth+1;k<rank;k++)
				_offset *= shape[k];
			_offset *= f;
			if(depth == rank-2)
			{
				//size_t s_offset = f*shape[depth+1];
				for(size_t t=0;t<shape[depth+1];t++)
					stream << data.at(_offset+t) << " ";
			}
			else
			{
				stream << writeDimension<T>(data, shape, rank, depth+1);
			}
			stream << "\n";
		}

		return stream.str();
	}

	template<typename T>
	std::string readNXFieldRule(pninx::nxfield& nxfield)
	{
		shape_t nxfield_shape = nxfield.shape<shape_t>();
		darray<T> data( nxfield_shape );
		nxfield.read(data);
		std::ostringstream stream;
		size_t rank = nxfield.rank();
		if(rank > 1)
		{
			stream << writeDimension<T>(data, nxfield_shape, rank);
		}
		else
		{
			size_t overall = nxfield_shape[0];
			for(size_t k=0;k<overall;k++)
				stream << data.at(k) << " ";
		}

		return stream.str();
	}


protected:
	std::map<std::string, std::string> options; /*!< The list of options fetched from XML file. */
	const std::map<type_id_t, size_t> types_size; /*!<
	 The list of max length of string representation of types used in pninx library.\n
	 e.g. ostingstream(std::numeric_limits<int8_t>::min()).str().length() == 7 ;\n
	 that means that biggest integer stored in int8_t variable can be printed out as 7 characters.\n
	 This map is used to calculate the size of file, before read the content of file.
	 */
	std::set<std::string> required_options; /*!< The list of mandatory options for Rule. If there are some mandatory options missing Rule won't be applied */
	RuleType type; /*!< Type of rule. */
	void correctOptions();

public:

	Rule();
	virtual ~Rule();
	void addOption(std::string key, std::string value);
	void setOptions(std::map<std::string, std::string> input);

	virtual subFiles createSubFiles(pninx::nxobject &nxobject);
	virtual void changeFSType(FSType type);
	std::string getOptionValue(const char* option_name);
	bool isValidOptions();

	//methods for FUSE
	virtual FSType getattr(pninx::nxobject &nxobject);
	//virtual std::vector<std::string> readdir(pninx::NXObject &nxobject);
	virtual std::string read(pninx::nxobject &nxobject);
	virtual size_t size(pninx::nxobject &nxobject);
};

#endif /* RULE_H_ */

