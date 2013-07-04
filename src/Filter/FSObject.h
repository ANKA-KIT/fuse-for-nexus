/*
 * FSObject.h
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

#ifndef FSOBJECT_H_
#define FSOBJECT_H_

#include <stdio.h>
#include <vector>
#include <iostream>
#include <string.h>
#include <memory>

#include "Rule.h"
#include "enums.h"
#include "NXGateway.h"

/**
 *	Defines an entry in FSTree. Represents file/folder within virtual filesystem.
 */
class FSObject {
private:
	std::string _nxobjectpath; /*!< Absolute path of NXObject within NXFile. */
	std::vector<std::string> children; /*!< List of files/folders located in this FSObject. */
	FSType _type; /*!< deprecated. */
public:
	FSObject();
	FSObject(const char* name);
	virtual ~FSObject();

	std::string name;
	std::string fullpath;
	int addChild(std::string fullpath);
	void renameChild(const char* child_name, const char* new_name);
	void setNXObjectPath(std::string path);

	Rule* rule; /*!< The Rule defines how this FSObject will be represented. */

	//fuse methods
	virtual std::string read();
	virtual std::vector<std::string> readdir();
	virtual FSType getattr();
	virtual size_t size();
};

#endif /* FSOBJECT_H_ */
