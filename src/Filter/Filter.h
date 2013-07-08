/*
 * Filter.h
 *
 *  Created on: Mar 20, 2013
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

#ifndef FILTER_H_
#define FILTER_H_

#include <stdio.h>
#include <pni/core/types.hpp>
#include <pni/io/nx/nx.hpp>
#include <errno.h>
#include "FSTree.h"
#include "XMLFile.h"
#include "enums.h"
#include "ImageRule.h"
#include "TableRule.h"
#include "NXGateway.h"
#include "NXFSCache.h"
#include "../config.h"

namespace pninx=pni::io::nx::h5;

/**
 *	It manages the other components, such as FSTree (Filter#_nxtree), XMLFile (Filter#_xmlfile), NXGateway (Filter#_nxgate), NXFSCache (Filter#_cache).
 */
class Filter {
private:
	static FSTree _nxtree;
	static XMLFile _xmlfile;
	static NXGateway _nxgate;
	static NXFSCache* _cache;

	void addRootGroup( pninx::nxgroup& nxgroup );
	void addGroup( pninx::nxgroup nxgroup, FSObject& fsparent );

	Rule* createHardcodedBehavior( pninx::nxobject &nxobject );
	void tryCreateDefaultBehavior( Rule* &behaviour, pninx::nxobject &nxobject );
	void tryCreateSpecificBehavior(Rule* &behaviour, pninx::nxobject& nxobject, FSObject& fsobj);
	void createBehavior( FSObject &fsobj, pninx::nxobject &nxobject );
	void createSubFiles(Rule* behaviour, pninx::nxobject& nxobj,  FSObject& parent);

	static FSObject& fsobjectAt( const char* path );

	std::string _xml_path; /*!< Stores the NeXus file path. */
	std::string _nx_path; /*!< Stores the XML file path. */
public:
	void createTree();
	Filter( );
	Filter( const char* nxfile_path, const char* xmlfile_path );
	virtual ~Filter( );

	void reopenNXFile();
	static void usage();

	//FUSE functions
	static FSType getattr( const char* path );
	static std::string read( const char* path );
	static std::vector<std::string> readdir( const char* path );
	static size_t size( const char* path );
};

/**
 *	These objects are passed to FUSE as a private data.
 */
struct nxfs_state
{
	FILE* logFile; /*!< file descriptor for log file. Used by ErrorLog. */
	Filter* myFilter; /*!< Filter that initialized on FuseProvider start. */
};

#endif /* FILTER_H_ */
