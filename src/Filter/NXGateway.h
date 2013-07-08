/*
 * NXGateway.h
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

#ifndef NXGATEWAY_H_
#define NXGATEWAY_H_

#include <pni/io/nx/nx.hpp>
#include "NXFSException.h"

namespace pninx=pni::io::nx::h5;

class NXGateway {
private:
	static pninx::nxfile _nxfile;
public:
	NXGateway();
	virtual ~NXGateway();
	static void load_file(const char* nxfile_path);

	static pninx::nxobject getNXObjectByPath(const char* nxpath);
};

#endif /* NXGATEWAY_H_ */
