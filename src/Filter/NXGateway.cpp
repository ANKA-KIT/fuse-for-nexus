/*
 * NXGateway.cpp
 *
 *  Created on: May 2, 2013
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

#include "NXGateway.h"

pninx::nxfile NXGateway::_nxfile;

NXGateway::NXGateway() {
}

NXGateway::~NXGateway() 
{
	if(_nxfile.is_valid())
		_nxfile.close();
}

/**
 * 	\brief Gets NXObject from NeXus file
 * 	\param [in] nxpath : full path of NXObject to be returned.
 * 	\return NXObject
 * 	\throw NXFSException if nxobject is not found
 */
pninx::nxobject NXGateway::getNXObjectByPath(const char* nxpath)
{
	if( !_nxfile.is_valid() )
		throw NXFSException("NXGateway: Nexus file is not valid");
	return _nxfile[nxpath];
}

/**
 *	\brief Opens NeXus file.
 *	\param [in] nxfile_path : full path to locally stored NeXus file.
 *	\throw NXFSException if there is some troubles while opening NeXus file.
 */
void NXGateway::load_file(const char* nxfile_path)
{
	if(_nxfile.is_valid())
		_nxfile.close();


	if( strlen(nxfile_path) > 0 )
	{
		try
		{
			_nxfile = pninx::nxfile::open_file(nxfile_path, true);
		}catch (...) {
			throw NXFSException("cannot open NeXus file");
		}
	}
	else
	{
		throw NXFSException("NeXus file path is empty");
	}
}

