/*
 * XMLErrorLog.h
 *
 *  Created on: Jun 3, 2013
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

#ifndef XMLERRORLOG_H_
#define XMLERRORLOG_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <fuse.h>
#include <ctime>
#include "Filter/Filter.h"

/**
 * Defines the application state whether FUSE running or not.\n
 * It affects the way how log file should be written.
 */
enum class ApplicationState {
	FUSEoff, /*!< Defines that application working in normal mode. */
	FUSEon /*!< Defines that application working in FUSE mode. */
};

/**
 * ErrorLog is designed for logging errors and other system messages.
 */
class ErrorLog {
private:
/**
*  The macro to take the private FUSE data easily.
*/
#define NXFS_DATA ( ( struct nxfs_state * ) fuse_get_context()->private_data )
	static void setState(ApplicationState state);
	static const char* logfile_path;
	static FILE* logfile;
	static ApplicationState _state;
	static void log_open();
	static void log_close();

	static void log_fuse_write(const char* format, va_list ap);
	static void log_default_write(const char* format, va_list ap);

public:
	ErrorLog();

	static void log_write(const char* format, ...);
	static void log_xml_error_msg(const char* msg, const char* xml_node_path, const char* nxobject_path);
	static void changeStateToFUSEmode();
	static void changeStateToDefaultMode();
	static FILE* log_fuse_open();

	virtual ~ErrorLog();
};

#endif /* XMLERRORLOG_H_ */
