/*
 * XMLReader.cpp
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

#define TIXML_USE_STL
#include "XMLFile.h"
#include "../ErrorLog.h"
#include <vector>
#include <exception>

XMLFile::XMLFile( const char* filepath ) : xml_errors({
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
})
{
	pugi::xml_parse_result result = _doc.load_file( filepath );

	if( result.status != pugi::xml_parse_status::status_ok )
		throw NXFSException( xml_errors.find(result.status)->second );
}

XMLFile::~XMLFile() {}

/**
 *	\brief Loads xml file.
 *	\param [in] filepath : full path to xml file.
 *	\return True if success, False if fail
 *	\throw NXFSException if there are any errors while opening xml file.
 */
bool XMLFile::load_file( const char* filepath )
{
	pugi::xml_parse_result result = _doc.load_file( filepath );

	if( result.status != pugi::xml_parse_status::status_ok)
	{
		throw NXFSException( xml_errors.find(result.status)->second );
		return false;
	}

	return true;
}

/**
 *	\brief Gets xml element from xml file.
 *	\param [in] path : absolute element path within xml file.
 */
pugi::xml_node& XMLFile::operator[]( const char* path )
{
	pugi::xml_node node;
	std::string str_path = path;

	if(str_path[0] == '/')
	{
		size_t pos;
		node =  _doc.first_child();
		while( str_path.length() > 0 )
		{
			pos = str_path.find ( "/" );
			if ( pos == std::string::npos ) pos = str_path.length();
			if ( pos == 0 )
			{
				str_path = str_path.substr( 1, str_path.length()-1 );
				continue;
			}
			//getting node name
			std::string node_str = str_path.substr( 0, pos );
			//cutting path - excluding handled node
			str_path = str_path.substr( pos, str_path.length()-( pos ) );

			node = node.child( node_str.c_str() );
		}
	}
	//may return NULL or empty node
	return node;
}

/**
 *	\brief Gets first match of specific rule.
 *	\param [in] nxpath : path of NXObject of rule that needs to be found.
 *	\return matching xml element if success, or empty xml element if fail.
 */
pugi::xml_node XMLFile::getFirstSpecificRule(std::string nxpath)
{
	//todo think about case when there is no entry with this parameter

	pugi::xml_node begin = _doc.first_child().child( "specific_rules" ).child( "object" );//todo think how to define this group
	pugi::xml_node match = begin;

	while( nxpath == match.child( "path" ).value() or match == 0 )
	{
		match = match.next_sibling();
	}

	if( strcmp( nxpath.c_str(), match.child( "path" ).value() ) == 0 )
		return match;
	else
		return pugi::xml_node();

	//jic
	return pugi::xml_node();
}

/**
 *	\brief Gets first match of default rule.
 *	\param [in] rule_name : the name of default rule that needs to be found.
 *	\return matching xml element if success, or empty xml element if fail.
 */
pugi::xml_node XMLFile::getFirstDefaultRule( std::string rule_name )
{
	//todo think about case if there is no such default rule
	std::cout << this->_doc.first_child().path( '.' ) << std::endl;
	pugi::xml_node begin = _doc.first_child().child( "default_rules" );

	std::cout << begin.name() << std::endl;

	pugi::xml_node match = begin;

	while( rule_name == match.value() or match == 0 )
	{
		match = match.next_sibling();
	}
	if( strcmp( rule_name.c_str(), match.value() ) == 0 )
	{
		return match;
	}
	else
	{
		return pugi::xml_node();
	}

	//jic
	return pugi::xml_node();
}

/**
 *	\brief Fetches default rule specified by \a rule_name.
 *	\param [in] rule_name : the name of rule to be fetched.
 *	\return the map, where key is the name of the xml element, value is the value of the xml element.
 */
std::map<std::string, std::string> XMLFile::fetchDefaultRule( std::string rule_name )
{
	std::map<std::string, std::string> output;
	std::string rule_path = ".filters.default_rules."+rule_name;
	auto first_parameter = _doc.first_element_by_path( rule_path.c_str(), '.' ).first_child();

	for ( pugi::xml_node option = first_parameter; option; option = option.next_sibling() )
	{
	    output.insert( std::pair<std::string, std::string>( option.name(), option.child_value() ) );
	}

	return output;
}

/**
 *	\brief Gets root node of xml document.
 */
const pugi::xml_node XMLFile::root_node() const
{
	return this->_doc.first_child();
}

/**
 *	\brief Finds the default rule specified by \a rule_name
 *	\param [in] rule_name : the default rule name that needs to be found
 *	\return True if success, False if fail.
 */
bool XMLFile::findDefaultRule( std::string rule_name )
{
	std::string rule_path = ".filters.default_rules."+rule_name;
	auto match = _doc.first_element_by_path( rule_path.c_str(), '.' );

	return !( match.empty() );
}

/**
 *	\brief Finds the specific rule
 *	\param [in] nxobject_path : the full path of NXObject.
 *	\return The mode of specific rule if success, empty string if fail.
 */
std::string XMLFile::findSpecificRule( std::string nxobject_path )
{
	pugi::xml_node match;
	for(match = this->_doc.first_child().child("specific_rules").first_child() ; match ; match = match.next_sibling() )
	{
		std::string str = match.child( "path" ).child_value();
		if( strcmp( str.c_str(), nxobject_path.c_str() ) == 0 )
			break;
	}
	std::string output;

	if(!match.empty())
	{
		output = match.child("mode").child_value();
		if(output.empty())
			ErrorLog::log_xml_error_msg("Cannot find mandatory element mode. Specific rule won't be implemented",
					match.path().c_str(), nxobject_path.c_str());
	}

	return output;
}

/**
 *	\brief Gets xml elements in map.
 *	\param [in] start : first xml element to start.
 *	\param [out] map : the fetched xml elements stored.
 *	\param [in] prev_node_name : the name of previous xml element.
 *
 *	Browses through xml elements and gets all of them in \a map. If the xml
 *	element has subelements it goes deeper, and elements fetched from
 *	subelement will have the prefix of parent element's name.
 *	For example:
 *	\code
 *	<parent>
 *		<child>value</child>
 *	</parent>
 *	\endcode
 *	Will cause only one pair in \a map. This pair will be \c &lt;parent_child, value&gt;
 */
void XMLFile::recursiveFetchParameter(pugi::xml_node start, std::map<std::string, std::string>& map, const char* prev_node_name )
{
	for ( pugi::xml_node parameter = start ; parameter; parameter = parameter.next_sibling() )
	{
		std::string key;
		std::string value;

		if( strlen( parameter.first_child().name() ) > 0 )
		{
			std::string node_str = prev_node_name;
			node_str += parameter.name();
			node_str += "_";
			recursiveFetchParameter( parameter.first_child(), map, node_str.c_str() );
		}
		else
		{
			key += prev_node_name;
			key += parameter.name();
			value = parameter.child_value();
			map.insert( std::pair<std::string, std::string>( key, value ) );
		}

	}
}


/**
 *	\brief Fetches the specific rule specified by \a nxobject_path.
 *	\param [in] nxobject_path : The full path of NXObject.
 *	\return the map, where key is the name of the xml element, value is the value of the xml element.
 *
 *	Also it gets options of the specified default rule, but overwrites default
 *	options with specific options if there is redefined option in specific rule.
 */
std::map<std::string, std::string> XMLFile::fetchSpecificRule( std::string nxobject_path )
{
	std::map<std::string, std::string> output;

	auto first_nxrule = _doc.first_element_by_path( ".filters.specific_rules", '.' );
	std::string mode;
	pugi::xml_node match;

	// looking for certain xml_node for nxobject
	for ( pugi::xml_node object = first_nxrule.first_child(); object; object= object.next_sibling() )
	{
		if ( strcmp( object.child("path").child_value(), nxobject_path.c_str() ) == 0 )
		{
			mode = object.child("mode").child_value();
			match = object;
			break;
		}
	}

	// if there is errors in xml file
	if(match.empty())
		return output;

	// fetching data from current xml_node
	recursiveFetchParameter(match.child( mode.c_str() ).first_child(), output, "");

	//getting data from default rule
	std::string default_rule_node_path = ".filters.default_rules.";
	default_rule_node_path += mode;
	pugi::xml_node def_rule_node =  _doc.first_element_by_path( default_rule_node_path.c_str() , '.' );
	if( !def_rule_node.empty() )
	{
		auto def_rule_options = fetchDefaultRule(mode);
		output.insert( def_rule_options.begin(), def_rule_options.end() );
	}

	return output;
}







