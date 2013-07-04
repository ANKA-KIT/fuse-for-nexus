/*
 * NXFSCache.cpp
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

#include "NXFSCache.h"

/**
 *	The list of file contents. Key is a full file path.
 */
std::map<std::string, std::string> NXFSCache::_memcache;

NXFSCache::~NXFSCache() {
	if(_memcache.size() > 0)
		_memcache.clear();
}

/**
 *	\brief Gets the file content specified by \a path.
 *	\param [in] fspath : full path of FSObject that has to be readed.
 *	\return The content of file.
 *
 *	It gets the file content in memory if there was no such file content, and returns it.
 */
std::string NXFSCache::read(const char* fspath)
{
	std::string output;
	if( !getOutput(fspath, output) )
	{
		output = getCacheOutput(fspath);
	}

	return output;
}

/**
 *	\brief Tries to get file content from memory.
 *	\param [in] fspath : full path to FSObject that has to be read.
 *	\param [out] output : FSObject content.
 *	\return True if there is such file content in memory, otherwise false.
 */
bool NXFSCache::getOutput(const char* fspath, std::string& output)
{
	auto it = _memcache.find( fspath );

	if( it != _memcache.end() )
	{
		output = it->second;
		return true;
	}
	else
	{
		return false;
	}

}

/**
 *	\brief Caches the file content in memory, and returns it.
 *	\param [in] fspath : full path to FSObject that has to be read.
 *	\return The file content.
 */
std::string NXFSCache::getCacheOutput(const char* fspath)
{
	std::string output;
	//checks available memory
	size_t avail_mem = getFreeSystemMemory();

	//checks size of file supposed to read
	size_t sz = _nxfstree.find(fspath).size();

	//todo: to limit number of members in cache?!
	if(sz < avail_mem)
	{
		//todo: handle exception?
		output = _nxfstree.find(fspath).read();
		_memcache.insert(std::pair<std::string, std::string> (fspath, output) );
	}
	else
	{
		cacheFree(sz);
		output = _nxfstree.find(fspath).read();
		_memcache.insert(std::pair<std::string, std::string> (fspath, output) );
	}

	return output;
}


/**
 *	Deletes cached file contents while don't have enough memory.
 *	\param [in] size : An amount of memory to be freed.
 */
void NXFSCache::cacheFree(size_t size)
{
	long int mem_left = size;
//todo add counter if there are no memory on machine just don't give a fuck and abort.
	while(mem_left > 50)//magic number
	{
		mem_left -= _memcache.begin()->second.length();
		_memcache.erase( _memcache.begin() );
	}
}

/**
 *	\brief Checks how much memory there is in system.
 *	\return Amount of available memory in system.
 */
size_t NXFSCache::getFreeSystemMemory()
{
	long pages_avail = sysconf(_SC_AVPHYS_PAGES);
    //long pages_total = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);

    return pages_avail * page_size;
}
