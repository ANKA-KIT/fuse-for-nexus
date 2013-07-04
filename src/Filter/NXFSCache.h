/*
 * NXFSCache.h
 *
 *  Created on: May 13, 2013
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

#ifndef NXFSCACHE_H_
#define NXFSCACHE_H_

#include <stdio.h>
#include "FSTree.h"
#include <unistd.h>

/**
 *	Class is designed to store in memory some file contents to increase performance of system.
 */
class NXFSCache {
private:
	static std::map<std::string, std::string> _memcache;
	FSTree& _nxfstree; /*!< Reference to FSTree created on application start. */

	std::string getCacheOutput(const char* fspath);
	bool getOutput(const char* fspath, std::string& output);
	void cacheFree(size_t size);

	size_t getFreeSystemMemory();

public:
	/**
	 *	\brief Constructor of NXFSCache.
	 */
	NXFSCache(FSTree& nxfstree) : _nxfstree(nxfstree) {};
	virtual ~NXFSCache();

	//FUSE function
	std::string read(const char* fspath);
};

#endif /* NXFSCACHE_H_ */
