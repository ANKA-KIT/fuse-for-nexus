/*
 * TableRule.h
 *
 *  Created on: May 14, 2013
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

#ifndef TABLERULE_H_
#define TABLERULE_H_

#include "enums.h"
#include "Rule.h"
#include "NXGateway.h"
#include <pni/nx/NX.hpp>
#include <pni/utils/Types.hpp>
#include "NXFSException.h"
#include <sstream>
#include <tiffio.h>
#include <algorithm>

/*#define readNXField(typeID, Type) std::vector<std::string> TableRule::readNXField (pninx::NXField& nxfield) \
{\
	return readColumnContent<#Type>(nxfield);\
}*/

/**
 *	Class handles the representation of the NeXus data and creates the Table from it.
 */
class TableRule : public Rule {
private:
	/**
	 *	Typedef pointer to function to store functions in a map.
	 */
	typedef std::vector<std::string> (TableRule::*getTableRuleContent_t) (pninx::NXField& nxfield, const int precision);
	/**
	 *	This map is designed to call correct template function, due to the fact that we know the type we need to pass into template in runtime only.
	 */
	const std::map<TypeID, getTableRuleContent_t> tableRuleReadField;

	std::vector<std::string> readNXFieldInt8 (pninx::NXField& nxfield, const int precision);
	std::vector<std::string> readNXFieldInt16 (pninx::NXField& nxfield, const int precision);
	std::vector<std::string> readNXFieldInt32 (pninx::NXField& nxfield, const int precision);
	std::vector<std::string> readNXFieldUInt8 (pninx::NXField& nxfield, const int precision);
	std::vector<std::string> readNXFieldUInt16 (pninx::NXField& nxfield, const int precision);
	std::vector<std::string> readNXFieldUInt32 (pninx::NXField& nxfield, const int precision);
	std::vector<std::string> readNXFieldFloat32 (pninx::NXField& nxfield, const int precision);
	std::vector<std::string> readNXFieldFloat64 (pninx::NXField& nxfield, const int precision);
	std::vector<std::string> readNXFieldFloat128 (pninx::NXField& nxfield, const int precision);
	std::vector<std::string> readNXFieldComplex32 (pninx::NXField& nxfield, const int precision);
	std::vector<std::string> readNXFieldComplex64 (pninx::NXField& nxfield, const int precision);
	std::vector<std::string> readNXFieldComplex128 (pninx::NXField& nxfield, const int precision);
	std::vector<std::string> readNXFieldString (pninx::NXField& nxfield, const int precision);
	std::vector<std::string> readNXFieldBinary (pninx::NXField& nxfield, const int precision);
	std::vector<std::string> readNXFieldBoolean (pninx::NXField& nxfield, const int precision);

	std::vector<std::string> readNXFieldAsVector(pninx::NXField &nxfield, const int precision);
	std::string getTableNXGroupContent(pninx::NXGroup& nxgroup);
	std::vector<std::string> getColumnContent(pninx::NXGroup& nxgroup, const char* path);
	void setColumnPosition(size_t column_num, size_t* order, std::vector<size_t>& columnsWithoutOrder);
	size_t getSizeOfColumn(pninx::NXGroup& nxgroup, const char* path);
	void validateColumnsOrder(size_t* order, size_t column_count, std::vector<size_t>& columnsWithoutOrder);
	std::vector<std::string> getTitles(size_t column_count, const char* nxgroup_path);
	std::string writeTableToStream(size_t column_count, size_t* columns_order,
			std::vector<std::string>& column_titles, std::vector<std::vector<std::string>> table_data, size_t max_column_length);
	size_t getSizeOfNXGroupTable(pninx::NXGroup& nxgroup);
	std::string getTableNXFieldContent(pninx::NXField& nxfield);
	std::string writeTitlesToStream(size_t column_count, size_t* columns_order, std::vector<std::string>& column_titles);
	std::string writeDataToStream(size_t column_count, size_t* columns_order,
			std::vector<std::vector<std::string>> table_data, size_t max_column_length);
	std::vector<std::vector<std::string>> divideVectorIntoTable(std::vector<std::string>& data_vector, size_t width, size_t heigth);
	/** The string that will separate values in table. Should be fetched from XML file.
		If there is no such option in XML file it is "," by default. */
	static char* separator;

	/**
	 *	\brief Gets NeXus field content as a vector of strings.
	 *	\param [in] nxfield : The NeXus field to be read.
	 *	\param [in] precision : The decimal precision to be used to format floating-point values.
	 *	\return Each value in NeXus filed
	 */
	template<typename T>
	std::vector<std::string> readColumnContent(pninx::NXField& nxfield, const int precision)
	{
		std::vector<std::string> output;

		shape_t nxfield_shape = nxfield.shape<shape_t>();
		DArray<T> data( nxfield_shape );
		nxfield.read(data);

		size_t rank = nxfield.rank();
		size_t overall = 1;

		for(size_t k=0;k<rank;++k)
			overall *= nxfield_shape[k];

		for(size_t i=0;i<overall;i++)
		{
			std::ostringstream str_tmp;
			str_tmp.precision(precision);
			str_tmp << data.at(i);
			output.push_back( str_tmp.str() );
		}

		return output;
	}

public:
	TableRule();
	virtual ~TableRule();

	//fuse methods
	virtual FSType getattr(pninx::NXObject &nxobject);
	//virtual std::vector<std::string> readdir(pninx::NXObject &nxobject);
	virtual std::string read(pninx::NXObject &nxobject);
	virtual size_t size(pninx::NXObject &nxobject);
};

#endif /* TABLERULE_H_ */
