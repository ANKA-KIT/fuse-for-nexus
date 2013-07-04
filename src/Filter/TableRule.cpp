/*
 * TableRule.cpp
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

#include "TableRule.h"
#include "../ErrorLog.h"

char* TableRule::separator;

/**
 *	Default constructor of TableRule
 */
TableRule::TableRule() : tableRuleReadField({
	{TypeID::UINT8, &TableRule::readNXFieldUInt8},
	{TypeID::UINT16, &TableRule::readNXFieldUInt16},
	{TypeID::UINT32, &TableRule::readNXFieldUInt32},
	{TypeID::INT8, &TableRule::readNXFieldInt8},
	{TypeID::INT16, &TableRule::readNXFieldInt16},
	{TypeID::INT32, &TableRule::readNXFieldInt32},
	{TypeID::FLOAT32, &TableRule::readNXFieldFloat32},
	{TypeID::FLOAT64, &TableRule::readNXFieldFloat64},
	{TypeID::FLOAT128, &TableRule::readNXFieldFloat128},
	{TypeID::COMPLEX32, &TableRule::readNXFieldComplex32},
	{TypeID::COMPLEX64, &TableRule::readNXFieldComplex64},
	{TypeID::COMPLEX128, &TableRule::readNXFieldComplex128},
	{TypeID::STRING, &TableRule::readNXFieldString},
	{TypeID::BINARY, &TableRule::readNXFieldBinary},
	{TypeID::BOOL, &TableRule::readNXFieldBoolean},
	{TypeID::NONE, &TableRule::readNXFieldBinary}
})
{
	type = RuleType::TABLE2D;
}

/**
 *	Default destructor of TableRule
 */
TableRule::~TableRule() {

}

/**
 * 	@name Read NXField functions
 *  Read the data from NXField in a correct way.
 */
///@{
/**
 *	Pointer to this function is located in TableRule#ruleReadField map,
 *	and it is called only via TableRule#ruleReadField map. The reason why this functions were created is the following:\n
 *	To get data from \a nxfield we need to know the type of data stored. But we have only TypeID of data type. \n
 *	So this function calls readColumnContent<T>(pninx::NXField&, const int) with right template parameter.
 */
std::vector<std::string> TableRule::readNXFieldInt8 (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<Int8>(nxfield, precision);
}
std::vector<std::string> TableRule::readNXFieldInt16 (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<Int16>(nxfield, precision);
}
std::vector<std::string> TableRule::readNXFieldInt32 (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<Int32>(nxfield, precision);
}
std::vector<std::string> TableRule::readNXFieldUInt8 (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<UInt8>(nxfield, precision);
}
std::vector<std::string> TableRule::readNXFieldUInt16 (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<UInt16>(nxfield, precision);
}
std::vector<std::string> TableRule::readNXFieldUInt32 (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<UInt32>(nxfield, precision);
}
std::vector<std::string> TableRule::readNXFieldFloat32 (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<Float32>(nxfield, precision);
}
std::vector<std::string> TableRule::readNXFieldFloat64 (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<Float64>(nxfield, precision);
}
std::vector<std::string> TableRule::readNXFieldFloat128 (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<Float128>(nxfield, precision);
}
std::vector<std::string> TableRule::readNXFieldComplex32 (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<Complex32>(nxfield, precision);
}
std::vector<std::string> TableRule::readNXFieldComplex64 (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<Complex64>(nxfield, precision);
}
std::vector<std::string> TableRule::readNXFieldComplex128 (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<Complex128>(nxfield, precision);
}
std::vector<std::string> TableRule::readNXFieldString (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<String>(nxfield, precision);
}
std::vector<std::string> TableRule::readNXFieldBinary (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<Binary>(nxfield, precision);
}
std::vector<std::string> TableRule::readNXFieldBoolean (pninx::NXField& nxfield, const int precision)
{
	return readColumnContent<Bool>(nxfield, precision);
}
///@}

FSType TableRule::getattr(pninx::NXObject &nxobject)
{
	return FSType::FILE;
}

/*std::vector<std::string> TableRule::readdir(pninx::NXObject &nxobject)
{
	std::vector<std::string> output;
	return output;
}*/
/**
 *	\brief Gets NeXus field data as a vector of strings, where each item from nxfield data is one string in vector.
 *	\param [in] nxfield : NeXus field that has to be read.
 *	\param [in] precision : The decimal precision to be used to format floating-point values.
 *	\return NeXus field data as a vector of strings.
 */
std::vector<std::string> TableRule::readNXFieldAsVector(pninx::NXField &nxfield, const int precision)
{
	std::vector<std::string> output;

	auto readField_iterator = tableRuleReadField.find( nxfield.type_id() );
	if(readField_iterator != tableRuleReadField.end() )
	{
		getTableRuleContent_t read_func = readField_iterator->second;
		output = (this->*read_func)( nxfield, precision );
	}else
	{
		std::string err_str = "An error occurred, unknown type. See log file \n";
		output.push_back(err_str);
	}

	return output;
}

/**
 *	\brief Divides passed vector into vector of vectors.
 *	\param [in] data_vector : The vector that has to be divided.
 *	\param [in] width : Number of columns in output.
 *	\param [in] heigth : Number of rows in output.
 *	\return The data stored in table (vector of vector).
 */
std::vector<std::vector<std::string>> TableRule::divideVectorIntoTable(std::vector<std::string>& data_vector, size_t width, size_t heigth)
{
	std::vector<std::vector<std::string>> data_table;
	size_t offset= 0;
	for(size_t i=0;i<width;i++)
	{
		//offset = i*height;
		offset = heigth;
		std::vector<std::string> tmp(data_vector.begin(), data_vector.begin()+heigth);
		data_vector.erase(data_vector.begin(), data_vector.begin()+offset);
		data_table.push_back(tmp);
	}
	return data_table;
}

/**
 *	\brief Gets representation of NXField as a Table.
 *	\param [in] nxfield : NeXus field that has to be represented.
 *	\return String representation, ready to be stored as a file content.
 */
std::string TableRule::getTableNXFieldContent(pninx::NXField& nxfield)
{
	size_t nxfield_rank = nxfield.rank();
	shape_t nxfield_shape = nxfield.shape<shape_t>();

	if(nxfield_rank > 2)
	{
		ErrorLog::log_write("NXField %s has more than 2 dimensions. Cannot represent more than 2D array, so representing it as an 2D array\n", nxfield.path().c_str());
	}

	int precision = 2;
	std::string value_format = this->getOptionValue("precision");
	if(value_format.empty())
		ErrorLog::log_xml_error_msg("Cannot find precision", "unknown", nxfield.path().c_str() );
	else
	{
		std::istringstream(value_format) >> precision;
		if(precision < 0)
			precision =0;
	}

	std::vector<std::string> nxfield_content = readNXFieldAsVector(nxfield, precision );

	size_t column_count;
	if(nxfield_rank >=2)
		column_count = nxfield_shape[0];
	else
		column_count = 1;
	//todo: ask what is the best way?
	ErrorLog::log_write("NXField representation will omit the column_count parameter from XML file");
	size_t max_lenght = nxfield_shape[1];

	std::vector<std::vector<std::string>> output = divideVectorIntoTable(nxfield_content, column_count, max_lenght);

	std::vector<size_t> unOrderedColumns;
	size_t* columns_order;
	columns_order = new size_t[column_count];
	std::fill_n(columns_order, column_count, -1);

	for(size_t i=0;i<column_count;i++)
	{
		setColumnPosition(i, columns_order, unOrderedColumns);
	}
	validateColumnsOrder(columns_order, column_count, unOrderedColumns);

	std::vector<std::string> column_titles = getTitles(column_count, nxfield.path().c_str());
	std::string data = writeTableToStream(column_count, columns_order, column_titles, output, max_lenght);

	delete columns_order;

	return data;
}

/**
 * 	\brief Gets file content representation of NeXus object.
 *
 * 	This function provides the representation of NXObject data based on XML options.
 *  It is called when FUSE called read function.
 */
std::string TableRule::read(pninx::NXObject &nxobject)
{
	std::string output;
	auto sep_it = this->options.find("separator");
	if( sep_it != this->options.end() )
		separator = const_cast<char*>( sep_it->second.c_str() );
	else
	{
		ErrorLog::log_xml_error_msg("Separator option not found. Default separator ',' will be used",
				"/specific_rules/object/table_csv/separator", nxobject.path().c_str() );
		separator = const_cast<char*>(",");
	}

	if(nxobject.object_type() == pni::nx::NXObjectType::NXFIELD)
	{
		pninx::NXField nxfield = (pninx::NXField) nxobject;
		output = getTableNXFieldContent(nxfield);
	}
	else
	{
		if(nxobject.object_type() == pni::nx::NXObjectType::NXGROUP)
		{
			pninx::NXGroup nxgroup = (pninx::NXGroup)nxobject;
			output = getTableNXGroupContent( nxgroup );
		}
		else
		{
			ErrorLog::log_write("ERROR: NXObject %s is unknown type - %d.\n", nxobject.path().c_str(), nxobject.object_type() );
			output = "An error occurred, NXObject is unknown type. See log file \n";
		}
	}
	return output;
}

/** This function provides the size of file.
 *  It is called when FUSE called getattr function.
 */
size_t TableRule::size(pninx::NXObject &nxobject)
{
	size_t output=0;
	if(nxobject.object_type() == pni::nx::NXObjectType::NXFIELD)
	{
		output = ( ( pninx::NXField ) nxobject).size();
		//todo think how many characters there will be
		output *= 2;
	}
	else
	{
		pninx::NXGroup nxgroup = (pninx::NXGroup)nxobject;
		output = getSizeOfNXGroupTable(nxgroup);
	}
	return output;
}

/**
 *	\brief Gets size of file if NeXus object is a NXGROUP.
 *	\param [in] nxgroup : NeXus group that has to be represented.
 *	\return The size of file in bytes that will require to store the representation of this NeXus group.
 */
size_t TableRule::getSizeOfNXGroupTable(pninx::NXGroup& nxgroup)
{
	size_t output = 0;
	size_t column_count = 0;

	auto column_count_str = getOptionValue("column_count");
	if(!column_count_str.empty())
	{
		long long int tmp;
		std::istringstream(column_count_str.c_str()) >> tmp;
		if(tmp < 0)
			ErrorLog::log_xml_error_msg("Column count can't be negative number. Representation as a table aborted.",
					"/specific_rules/object/table_csv/column_count", nxgroup.path().c_str() );
		else
			std::istringstream(column_count_str.c_str()) >> column_count;
	}
	else
		ErrorLog::log_xml_error_msg("Cannot find column count, the representation as a table is aborted",
				"/specific_rules/object/table_csv/column_count", nxgroup.path().c_str() );

	//get content of each column
	for(size_t i=0;i<column_count;i++)
	{
		std::vector<std::string> current_column_content;

		std::string column_content_path = "column";
		column_content_path += static_cast<std::ostringstream*>( &(std::ostringstream() << (i+1)) )->str();
		column_content_path += "_content_path";

		std::string content_path = getOptionValue( column_content_path.c_str() );
		if( !content_path.empty() )
		{
			std::string nxcolumn_path = content_path.erase(0, 2);
			output += getSizeOfColumn(nxgroup, nxcolumn_path.c_str());
		}
		else
		{
			ErrorLog::log_xml_error_msg("Cannot find column content path, this column would not be represented",
						"/specific_rules/object/table_csv/column/content/path", nxgroup.path().c_str() );
		}
	}
	return output;
}

/**
 *	\brief Gets the size of one column within the Table representation of NXGroup.
 *	\param [in] nxgroup : NeXus group.
 *	\param [in] path : relative path of NXField within the NXGroup.
 *	\return The size in bytes that will require to store the column in file.
 */
size_t TableRule::getSizeOfColumn(pninx::NXGroup& nxgroup, const char* path)
{
	size_t output = 0;
	pninx::NXObject nxcolumn;
	try{
		nxcolumn = nxgroup[path];
	}catch (Exception& e) {
		return 0;
	}
	if(nxcolumn.object_type() == pni::nx::NXObjectType::NXFIELD)
		output = ( (pninx::NXField) nxcolumn).size();
	else
		output = 0;
	//todo think how many characters there will be
	size_t multiplier = this->types_size.find(( (pninx::NXField) nxcolumn).type_id())->second;
	output *= multiplier;
	return output;
}

/**
 *	\brief Gets column content within the Table representation of NXGroup.
 *	\param [in] nxgroup : NeXus group.
 *	\param [in] path : relative path of NXField that should be represented as a column.
 *	\return The data from NXField as a vector of strings, where each data item from NXField is one string in vector.
 */
std::vector<std::string> TableRule::getColumnContent(pninx::NXGroup& nxgroup, const char* path)
{
	std::vector<std::string> output;
	pninx::NXObject nxcolumn;
	try{
		nxcolumn = nxgroup[path];
	}catch (Exception& e) {
		std::string err_output = "Wrong column path - ";
		err_output += path;
		ErrorLog::log_xml_error_msg( err_output.c_str(),
						"undefined column", nxgroup.path().c_str() );
		err_output = "A nxerror occurred, ";
		err_output += path;
		err_output += " NXObject cannot be found";
		output.push_back(err_output);
		return output;
	}

	int precision = 0;
	std::string value_format = this->getOptionValue("precision");
	if(value_format.empty())
		ErrorLog::log_xml_error_msg("Cannot find precision", "unknown", nxgroup.path().c_str() );
	else
	{
		std::istringstream(value_format) >> precision;
		if(precision < 0)
			precision = 0;
	}

	if( nxcolumn.object_type() == pni::nx::NXObjectType::NXFIELD )
	{
		pninx::NXField nxfield = ( (pninx::NXField) nxcolumn );
		output = readNXFieldAsVector( nxfield, precision );
	}
	else
	{
		std::string err_output = "error occurred, unknown object type of "+nxcolumn.path();
	}

	return output;
}

/**
 *	\brief Defines column position in \a order array.
 *	\param [in] column_num : The number of column to proceed.
 *	\param [out] order : array of columns. Where the index of array is an order of column,
 *	and value at indexed position is the column number.
 *	\param [out] columnsWithoutOrder : vector of column numbers.
 *
 *	Checks the TableRule#options for column_order option for certain \a column_num passed.
 *	If there is such column order entry, puts the found column order in the \a order array,
 *	where the index of array is an order of the column, and value at indexed position is the column number.
 *	(e.g. order[position] = column_num).
 */
void TableRule::setColumnPosition(size_t column_num, size_t* order, std::vector<size_t>& columnsWithoutOrder)
{
	std::string column_order = "column_order_column";
	column_order += static_cast<std::ostringstream*>( &(std::ostringstream() << (column_num+1)) )->str();

	std::string column_order_num = getOptionValue( column_order.c_str() );
	if( !column_order_num.empty() )
	{
		//getting column number from xml file, therefore column number we get starts from 1
		//size_t number;
		long int tmp;
		std::istringstream(column_order_num.c_str()) >> tmp;
		if(tmp < 1)
		{
			ErrorLog::log_xml_error_msg("Order of column cannot be negative number or zero. This order will be omitted.",
					column_order.c_str(), "unknown");
			columnsWithoutOrder.push_back(column_num);
			return;
		}
		order[tmp-1] = column_num;//.push_back( number - 1 );
	}
	else
	{
		columnsWithoutOrder.push_back(column_num);
		std::string xml_path = "/specific_rules/object/table_csv/column_order/column";
		xml_path += static_cast<std::ostringstream*>( &(std::ostringstream() << (column_num+1)) )->str();
		ErrorLog::log_xml_error_msg("Cannot find order of the column",
				xml_path.c_str(), "");
	}
}

/**
 *	\brief Gets titles of columns.
 *	\param [in] column_count : Total amount of columns.
 *	\param [in] nxgroup_path : the path of NXGroup that is proceeding.
 *	This param is used only in case of error logging.
 *	\return The vector of column titles.
 *
 *	Checks the TableRule#options for a title for each column,
 *	if title found it puts the title in vector, if there is no title for
 *	column it puts "Untitled" string in vector.
 */
std::vector<std::string> TableRule::getTitles(size_t column_count, const char* nxgroup_path)
{
	std::vector<std::string> column_titles;
	for(size_t i=0;i<column_count;i++)
	{
		std::string column_title = "column";
		column_title += static_cast<std::ostringstream*>( &(std::ostringstream() << (i+1)) )->str();
		column_title += "_title";

		auto title_it = this->options.find(column_title);
		if(title_it != this->options.end())
		{
			column_titles.push_back(title_it->second);
		}
		else
		{
			column_titles.push_back("Untitled");
			ErrorLog::log_xml_error_msg("Cannot find column title, the column title would not be represented.",
					"/specific_rules/object/table_csv/column/title", nxgroup_path );
		}
	}
	return column_titles;
}

/**
 *	Checks the order array and if there is -1 in order it replaces it with value from \a columnsWithoutOrder vector.
 */
void TableRule::validateColumnsOrder(size_t* order, size_t column_count, std::vector<size_t>& columnsWithoutOrder)
{
	size_t index=0;
	for(size_t i=0;i<column_count;i++)
	{
		if(order[i] == size_t(-1))
		{
			order[i] = columnsWithoutOrder.at(index);
			index++;
		}
	}
}

/**
 *	\brief Gets table representation of NXGroup.
 *	\param [in] nxgroup : NeXus group that has to be represented.
 *	\return the representation that ready to be stored as a file content.
 */
std::string TableRule::getTableNXGroupContent(pninx::NXGroup& nxgroup)
{
	std::vector<std::vector<std::string>> output;
	//defaults
	size_t column_count = 0;
	// columns as a numbers filled in order that they will be represented
	size_t* columns_order;
	std::vector<size_t> unOrderedColumns;
	size_t max_lenght=0;

	//check how much columns
	auto column_count_str = getOptionValue("column_count");
	if(!column_count_str.empty())
	{
		long long int tmp;
		std::istringstream(column_count_str.c_str()) >> tmp;
		if(tmp < 0)
			ErrorLog::log_xml_error_msg("Column count can't be negative number. Representation as a table aborted.",
					"/specific_rules/object/table_csv/column_count", nxgroup.path().c_str() );
		else
			std::istringstream(column_count_str.c_str()) >> column_count;
	}
	else
		ErrorLog::log_xml_error_msg("Cannot find column count, the representation as a table is aborted",
				"/specific_rules/object/table_csv/column_count", nxgroup.path().c_str() );

	columns_order = new size_t[column_count];
	std::fill_n(columns_order, column_count, -1);

	//get content of each column
	for(size_t i=0;i<column_count;i++)
	{
		setColumnPosition(i, columns_order, unOrderedColumns);
		std::vector<std::string> current_column_content;

		std::string column_content_path = "column";
		column_content_path += static_cast<std::ostringstream*>( &(std::ostringstream() << (i+1) ) )->str();
		column_content_path += "_content_path";

		std::string content_path = getOptionValue( column_content_path.c_str() );

		if( !content_path.empty() )
		{
			std::string nxcolumn_path = content_path.erase(0, 2);
			current_column_content = getColumnContent( nxgroup, nxcolumn_path.c_str() );

			if( max_lenght < current_column_content.size() )
				max_lenght = current_column_content.size();

			output.push_back(current_column_content);
		}
		else
		{
			std::vector<std::string> empty;
			output.push_back(empty);
			ErrorLog::log_xml_error_msg("Cannot find column content path, this column would not be represented",
							"/specific_rules/object/table_csv/column/content/path", nxgroup.path().c_str() );
		}
	}//end for loop

	validateColumnsOrder(columns_order, column_count, unOrderedColumns);

	std::vector<std::string> column_titles = getTitles(column_count, nxgroup.path().c_str());
	std::string data = writeTableToStream(column_count, columns_order, column_titles, output, max_lenght);

	delete columns_order;

	return data;
}

/**
 *	\brief Writes column titles to stream.
 *	\param [in] column_count : Total amount of columns.
 *	\param [in] columns_order : Array of column order.
 *	\param [in] column_titles : Vector of column titles.
 *	\return string with column titles written.
 */
std::string TableRule::writeTitlesToStream(size_t column_count, size_t* columns_order, std::vector<std::string>& column_titles)
{
	std::ostringstream stream;
	for(size_t k=0;k<column_count;k++)
	{
		size_t col_order_index = columns_order[k];
		if(k == column_count-1)
			stream << column_titles[col_order_index] << std::endl;
		else
			stream << column_titles[col_order_index] << separator;
	}
	return stream.str();
}

/**
 *	\brief Writes table data to stream.
 *	\param [in] column_count : Total amount of columns.
 *	\param [in] columns_order : Array of column order.
 *	\param [in] table_data : table data to be written.
 *	\param [in] max_column_length : The longest column length.
 *	\return the table written to string. Without headers.
 */
std::string TableRule::writeDataToStream(size_t column_count, size_t* columns_order,
		std::vector<std::vector<std::string>> table_data, size_t max_column_length)
{
	std::ostringstream stream;
	for(size_t i=0;i<max_column_length;i++)
	{
		for(size_t j=0;j<column_count;j++)
		{
			std::ostringstream tmp;

			size_t col_order_index = columns_order[j];
			try{
				if(j%(column_count-1)==0 && j!=0)
					tmp << table_data[col_order_index][i];
				else
					tmp << table_data[col_order_index][i] << separator;
			}catch (...) {
				tmp << '\0';
			}

			stream << tmp.str();
		}
		stream << std::endl;
	}
	return stream.str();
}


/**
 *	\brief Writes entire table.
 *	\param [in] column_count : Total amount of columns.
 *	\param [in] columns_order : Array of column order.
 *	\param [in] column_titles : Vector of column titles.
 *	\param [in] table_data : table data to be written.
 *	\param [in] max_column_length : The longest column length.
 *	\return Table with headers written to string and ready to be represented as a file content.
 */
std::string TableRule::writeTableToStream(size_t column_count, size_t* columns_order,
		std::vector<std::string>& column_titles, std::vector<std::vector<std::string>> table_data, size_t max_column_length)
{
	//write to stream
	std::ostringstream stream;
	stream << writeTitlesToStream(column_count, columns_order, column_titles);
	stream << writeDataToStream(column_count, columns_order, table_data, max_column_length);

	return stream.str();
}
