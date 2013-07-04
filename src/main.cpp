/*
 * main.cpp
 *
 *  Created on: Feb 13, 2013
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



#include "FuseProvider.h"

int main(int argc, char **argv) {
	FuseProvider *FP = new FuseProvider(argc, argv);

	return 0;
}



/**
 *	\mainpage FUSE for NeXus files.
 * 	\section intro_sec Introduction
 *
 *	This is brief documentation describes the source code of application.
 *
 *
 * 	\section copyright_sec Copyright
 *  NXFS. FUSE for NeXus files with NeXus data filtering based on rules stored in xml file.\n
 *  Copyright (C) 2013 Karlsruher Institut für Technologie (KIT)\n
 *\n
 *  This program is free software: you can redistribute it and/or modify\n
 *  it under the terms of the GNU General Public License as published by\n
 *  the Free Software Foundation, either version 3 of the License, or\n
 *  (at your option) any later version.\n
 *\n
 *  This program is distributed in the hope that it will be useful,\n
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n
 *  GNU General Public License for more details.\n
 *\n
 *  You should have received a copy of the GNU General Public License\n
 *  along with this program. If not, see http://www.gnu.org/licenses/.\n
 */
