/*
 * FSTree.cpp
 *
 *  Created on: Mar 15, 2013
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

#include "FSTree.h"

std::map<std::string, FSObject>* FSTree::_nodes_ptr;

/**
 *	\brief to access Nodes map and prevent static initializing troubles.
 */
std::map<std::string, FSObject>& FSTree::Nodes()
{
	if(_nodes_ptr == NULL)
	{
		_nodes_ptr = new std::map<std::string, FSObject>();
	}
	return *_nodes_ptr;
}

/**
 *	\brief Constructor of FSTree.
 *
 *	Creates root node "/".
 */
FSTree::FSTree() {
	FSObject obj;
	obj.name = "/";
	obj.fullpath = "/";
	try{
		Nodes().insert( std::pair<std::string, FSObject> ("/", obj) );
	}catch (...) {
		fprintf(stderr, "Error occurred: \n");
		abort();
	}

}

/**
 *	Destructor of FSTree. Frees memory.
 *
 */
FSTree::~FSTree() {
	if(this->Nodes().size() > 0)
		this->Nodes().clear();
	this->_nodes_ptr = NULL;
}


/**
 *	Debug method to look through Nodes.
 */
void FSTree::lookup()
{
#ifdef _DEBUG_MODE_
	ErrorLog::log_write("\nFSTree.lookup: run; \n" );
	for (std::map<std::string, FSObject>::iterator it = Nodes().begin(); it != Nodes().end(); ++it)
	{
		ErrorLog::log_write("%s: b-%p (%d) \n", it->second.fullpath.c_str(), it->second.rule, it->second.rule->_type);
	}
#endif
}

/**
 *	\brief Inserts new FSObject in tree.
 *	\param [in] new_node : new node to be inserted.
 *	\param [in] path : the path where the \a new_node will be inserted
 *	\return 0 if success, anything else - error.
 *
 *	Inserts new FSObject in the virtual filesystem tree. Adds new child to \a new_node 's parent FSObject.
 */
int FSTree::insert(FSObject& new_node, const std::string path)
{
	std::string path_str = path;
	if(path == "/")
		path_str += new_node.name;
	else
		path_str += "/" + new_node.name;

	new_node.fullpath = path_str;
	//todo: handle exception if there is no parent with such path?!
	auto it = Nodes().find( path.c_str() );
	if(it != Nodes().end())
		it->second.addChild(new_node.name.c_str());
	else
		return -1;

	Nodes().insert(std::pair<std::string, FSObject>(path_str, new_node));
	return 0;
}

/**
 *	\brief Inserts new FSObject in tree.
 *	\param [in] new_node : new node to be inserted.
 *	\param [in] parent : the parent FSObject of \a new_node.
 *	\return 0 if success, anything else - error.
 *
 *	Inserts new FSObject in the virtual filesystem tree. Adds new child to \a parent.
 */
int FSTree::insert(FSObject& new_node, FSObject& parent)
{
	return insert(new_node, parent.fullpath);
}

/**
 *	\brief It looks for FSObject specified by \a path.
 *	\param [in] path : full path of FSObject one want to have.
 *	\return reference to a match.
 *	\throw NXFSException if if can't find a FSObject.
 */
FSObject& FSTree::find(const char* path)
{
	auto fsobj_it = Nodes().find(path);
	if( fsobj_it != Nodes().end() )
		return fsobj_it->second;
	else
	{
		std::string err_msg =  "Cannot find object ";
		err_msg += path;
		throw NXFSException( err_msg );
	}
}

/**
 *	\brief It looks for FSObject specified by \a path.
 *	\param [in] path : full path of FSObject one want to have.
 *	\return reference to a match.
 *	\throw NXFSException if if can't find a FSObject.
 *
 *	Operator to make things easier. Calls find(const char*).
 */
FSObject& FSTree::operator [](const char* path)
{
	return find(path);
}


/**
 *	\brief Sets the FSObject#rule of FSObject specified by \a path.
 *	\param [in] path : path of FSObject.
 *	\param [in] behavior : Rule to be set.
 *	\return 0 if success, anything else - fail.
 */
int FSTree::setRule( const char* path, Rule* behavior )
{
	auto fsobj_it = Nodes().find( path );
	if(fsobj_it != Nodes().end() )
		fsobj_it->second.rule = behavior;
	else
		return -1;//todo log error
	return 0;
}

/**
 *	\brief Sets the FSObject#rule of FSObject specified by \a path.
 *	\param [in] fsobj : FSObject.
 *	\param [in] behavior : Rule to be set.
 *	\return 0 if success, anything else - fail.
 */
int FSTree::setRule( FSObject& fsobj, Rule* behavior )
{
	auto fsobj_it = Nodes().find( fsobj.fullpath.c_str() );
	if(fsobj_it != Nodes().end() )
		fsobj_it->second.rule = behavior;
	else
		return -1;//todo log error
	return 0;
}

/**
 *	\brief Changes the FSObject#name and FSObject#fullname of object specified by \a path.
 *	\param [in] path : full path of FSObject.
 *	\param [in] newname : new name of FSObject. Without path.
 *
 *	Takes the FSObject specified by \a path and changes it's name to \a newname.
 */
void FSTree::changeFSObjectName(const char* path, const char* newname)
{
	auto fsobj_it = Nodes().find(path);
	if( fsobj_it != Nodes().end() )
	{
		std::string parent_fullpath;
		std::string old_name;
		FSObject fsobj_copy = fsobj_it->second;

		Nodes().erase(fsobj_it);
		old_name = fsobj_it->second.name;
		fsobj_copy.name = newname;

		std::string fullpath = fsobj_copy.fullpath;
		auto namestart_pos = (fullpath.find_last_of("/")+1);
		size_t len = fullpath.length() - namestart_pos;

		if(namestart_pos != std::string::npos)
		{
			fullpath = fullpath.erase( namestart_pos, len );
			parent_fullpath = fullpath;
			parent_fullpath.erase( parent_fullpath.end()-1, parent_fullpath.end() );
			fullpath += newname;
		}
		//todo: else return nothing?

		fsobj_copy.fullpath = fullpath;
		Nodes().insert( std::pair<std::string, FSObject>( fullpath, fsobj_copy ) );

		Nodes().find(parent_fullpath)->second.renameChild(old_name.c_str(), newname);
	}
	//todo: else return an error
}

/**
 *	\brief Takes the last added element of FSTree.
 */
FSObject& FSTree::last()
{
	auto it = Nodes().end();
	it--;
	return it->second;
}
