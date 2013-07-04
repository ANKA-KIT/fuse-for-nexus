/*
 * FSTree.h
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

#ifndef FSTREE_H_
#define FSTREE_H_

#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <string.h>

#include "FSObject.h"

/**
 * Class intended to manage the virtual filesystem tree based on FSObject
 */
class FSTree {
private:
	static std::map<std::string, FSObject>* _nodes_ptr; /*!< The list of files/folders. */
	static std::map<std::string, FSObject>& Nodes();

public:
	FSTree();
	virtual ~FSTree();
	static void lookup();
	static FSObject& find(const char* path);

	static int insert(FSObject& new_node, const std::string path);
	static int insert(FSObject& new_node, FSObject& parent);

	static int setRule( FSObject& fsobj, Rule* behavior );
	static int setRule( const char* path, Rule* behavior );
	static FSObject& last();

	static void changeFSObjectName(const char* path, const char* newname);

	FSObject& operator[](const char *path);
};

#endif /* FSTREE_H_ */
