/*
 * FSObject.cpp
 *
 *  Created on: Mar 19, 2013
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

#include "FSObject.h"

/**
 * Default constructor of FSObject
 */
FSObject::FSObject() {
	rule = NULL;
	_type = FSType::NONE;
}

/**
 * \brief Constructor of FSObject.
 *
 * \param [in] in_name : name of FSObject
 */
FSObject::FSObject(const char* in_name) : name(in_name)
{
	rule = NULL;
	_type = FSType::NONE;
}

/**
 * Destructor of FSObject.
 */
FSObject::~FSObject() {
	this->rule = NULL;
}

/**
 *	\brief Adds new entry in children vector.
 *
 */
int FSObject::addChild(std::string object_name)
{
	children.push_back(object_name);
	return 0;
}

/**
 *	\brief Gets the content of FUSE file.
 *	\return content of file.
 */
std::string FSObject::read()
{
	std::string output = "";
	if(this->rule != NULL)
	{
		auto nx = NXGateway::getNXObjectByPath( this->_nxobjectpath.c_str() );
		output = this->rule->read( nx );
	}
	else
		output = "Behavior not implemented";
	return output;
}

/**
 *	\brief Gets the children of FSObject.
 *	\return children vector
 */
std::vector<std::string> FSObject::readdir()
{
	return children;
}

/**
 *	\brief Gets the type of FSObject.
 *
 *	\return FSType#FILE or FSType#FOLDER if success, FSType#NONE if fail.
 */
FSType FSObject::getattr()
{
	FSType ret = FSType::NONE;
	if(this->rule != NULL)
	{
		pninx::NXObject nx = NXGateway::getNXObjectByPath( this->_nxobjectpath.c_str() );
		try
		{
			//the truth is out there
			ret = this->rule->getattr( nx );
		}
		catch (NXFSException& e) {
			//todo handle exception
		}
	}
	return ret;
}

/**
 *	\brief Gets the size of FSObject.
 *
 *	\return size of file in bytes.
 */
size_t FSObject::size()
{
	size_t sz = 0;
	if(this->rule != NULL)
	{
		auto nx = NXGateway::getNXObjectByPath( this->_nxobjectpath.c_str() );
		sz = this->rule->size( nx );
	}
	return sz;
}

/**
 * \brief Sets NeXus object path
 *
 * \param [in] path : NeXus object path
 */
void FSObject::setNXObjectPath(std::string path)
{
	_nxobjectpath = path;
}

/**
 * \brief renames child specified by \a child_name with a \a new_name
 *
 * \param [in] child_name : Name of child to be renamed.
 * \param [in] new_name : New name of child.
 */
void FSObject::renameChild(const char* child_name, const char* new_name)
{
	for(auto it = children.begin(); it!= children.end();it++)
		if(*it == child_name)
			*it = new_name;
}
