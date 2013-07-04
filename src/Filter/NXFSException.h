/*
 * NXFSException.h
 *
 *  Created on: Apr 18, 2013
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

#ifndef NXFSEXCEPTION_H_
#define NXFSEXCEPTION_H_

#include <exception>
#include <iostream>
#include <stdio.h>
#include <string.h>

/**
 *	Class is designed to catch the internal errors in application
 */
class NXFSException: public std::exception {
private:
	std::string _msg;
public:
	/**
	 *	Constructor of NXFSException.
	 */
	NXFSException(std::string msg) : _msg(msg) {};
	virtual ~NXFSException() throw() {};
	/**
	 *	Returns the explanation of error.
	 */
	const char* what() const throw() {return _msg.c_str();};
};

#endif /* NXFSEXCEPTION_H_ */
