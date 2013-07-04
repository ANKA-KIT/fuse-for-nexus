/*
 * XMLReader.h
 *
 *  Created on: Mar 12, 2013
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

#ifndef XMLREADER_H_
#define XMLREADER_H_

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <memory>

#include "NXFSException.h"
#include "pugixml-src/pugixml.hpp"

class XMLFile {
private:
	pugi::xml_document _doc; /*!< The xml file stored. */
	pugi::xml_node getFirstSpecificRule( std::string nxpath );
	pugi::xml_node getFirstDefaultRule( std::string rule_name );
	void recursiveFetchParameter(pugi::xml_node start, std::map<std::string, std::string>& map, const char* prev_node_name );

	const std::map<pugi::xml_parse_status, const char*> xml_errors; /*!< The explanation of possible errors. */

public:
	const pugi::xml_node root_node() const;
	XMLFile() : xml_errors({
		{pugi::xml_parse_status::status_file_not_found, "XMLFile: file was not found" },
		{pugi::xml_parse_status::status_io_error , "XMLFile: Error reading from file/stream" },
		{pugi::xml_parse_status::status_out_of_memory , "XMLFile: Could not allocate memory" },
		{pugi::xml_parse_status::status_internal_error , "XMLFile: Internal error occurred while opening xml file" },
		{pugi::xml_parse_status::status_unrecognized_tag , "XMLFile: Parser could not determine tag type" },
		{pugi::xml_parse_status::status_bad_pi , "XMLFile: Parsing error occurred while parsing document declaration/processing instruction" },
		{pugi::xml_parse_status::status_io_error , "XMLFile: Error reading from file/stream" },
		{pugi::xml_parse_status::status_bad_comment , "XMLFile: Parsing error occurred while parsing comment" },
		{pugi::xml_parse_status::status_bad_cdata , "XMLFile: Parsing error occurred while parsing CDATA section" },
		{pugi::xml_parse_status::status_bad_doctype , "XMLFile: Parsing error occurred while parsing document type declaration" },
		{pugi::xml_parse_status::status_bad_pcdata , "XMLFile: Parsing error occurred while parsing PCDATA section" },
		{pugi::xml_parse_status::status_bad_start_element , "XMLFile: Parsing error occurred while parsing start element tag" },
		{pugi::xml_parse_status::status_bad_attribute , "XMLFile: Parsing error occurred while parsing element attribute" },
		{pugi::xml_parse_status::status_bad_end_element , "XMLFile: Parsing error occurred while parsing end element tag" },
		{pugi::xml_parse_status::status_end_element_mismatch , "XMLFile: There was a mismatch of start-end tags" }
	}) {};
	XMLFile( const char* filepath );
	virtual ~XMLFile();
	bool load_file( const char* filepath );

	pugi::xml_node& operator[]( const char* path );
	std::map<std::string, std::string> fetchDefaultRule( std::string rule_name );
	std::map<std::string, std::string> fetchSpecificRule( std::string nxobject_path );

	bool findDefaultRule( std::string rule_name );
	std::string findSpecificRule( std::string rule_name );

};

#endif /* XMLREADER_H_ */
