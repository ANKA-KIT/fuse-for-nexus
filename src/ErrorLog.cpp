/*
 * XMLErrorLog.cpp
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

#include "ErrorLog.h"

/**
 * file descriptor of log file.
 */
FILE* ErrorLog::logfile;
/**
 *	The default path where log file should be stored.
 */
const char* ErrorLog::logfile_path = "/tmp/NXFS.log";
/**
 *	The state of application and of class. If it is FUSEon mode it takes the file descriptor from NXFS_DATA
 */
ApplicationState ErrorLog::_state = ApplicationState::FUSEoff;

/**
 * Constructor ErrorLog.
 * Defines a state of a ApplicationState. And opens log file.
 */
ErrorLog::ErrorLog() {
	_state = ApplicationState::FUSEoff;
	log_open();
}

/**
 * Destructor ErrorLog.
 * Closes the log file.
 */
ErrorLog::~ErrorLog() {
	log_close();
}

/**
 *	Opens ErrorLog::logfile defined by ErrorLog#logfile_path.
 *	Writes current date in time in log file.
 */
void ErrorLog::log_open()
{
	logfile = fopen(logfile_path, "a+");
	_state = ApplicationState::FUSEoff;
	time_t t = time(0);
	struct tm * now = localtime( & t );

	log_write("\n--------------- NXFS run (%02d:%02d:%02d %02d.%02d.%02d %s) ---------------\n",
			now->tm_hour, now->tm_min, now->tm_sec, (now->tm_year+1900), now->tm_mon, now->tm_mday, now->tm_zone);
}

/**
 * Opens if not opened and returns FILE* to logfile.\n
 * \return fd to logfile.
 */
FILE* ErrorLog::log_fuse_open()
{
	if(logfile == NULL)
		log_open();
	return logfile;
}

/**
 * Changes ApplicationState to FUSE mode.
 */
void ErrorLog::changeStateToFUSEmode()
{
	setState(ApplicationState::FUSEon);
}

/**
 * Changes ApplicationState to application mode.
 */
void ErrorLog::changeStateToDefaultMode()
{
	setState(ApplicationState::FUSEoff);
}

/**
 * Sets the ApplicationState to passed state.\n
 * \param [in] state is a ApplicationState.
 */
void ErrorLog::setState(ApplicationState state)
{
	_state = state;
}

/**
 * Closes ErrorLog#logfile.
 */
void ErrorLog::log_close()
{
	fclose(logfile);
}

/**
 * \brief Writes formated string to a log file.
 *
 * \param [in] format : formated c-like string.\n
 * \param [in] ... : other arguments, such as variables needed to complete formated string.
 */
void ErrorLog::log_write(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	if(_state == ApplicationState::FUSEon)
		log_fuse_write(format, ap);
	else
		log_default_write(format, ap);
}

/**
 * \brief Writes formated string to a log file.
 *
 * Used only if ApplicationState in \link ApplicationState::FUSEoff FUSEoff \endlink mode.\n
 * \param [in] format : formated c-like string.\n
 * \param [in] ap : variable list.
 */
void ErrorLog::log_default_write(const char* format, va_list ap)
{
	// set logfile to line buffering
	setvbuf(logfile, NULL, _IOLBF, 0);

	vfprintf(logfile, format, ap);
}

/**
 * \brief Writes formated string to a log file.
 *
 * Used only if ApplicationState in \link ApplicationState::FUSEon FUSEon \endlink mode.\n
 * \param [in] format : formated c-like string.\n
 * \param [in] ap : variable list.\n
 */
void ErrorLog::log_fuse_write(const char* format, va_list ap)
{
	// set logfile to line buffering
	setvbuf(NXFS_DATA->logFile, NULL, _IOLBF, 0);

	vfprintf(NXFS_DATA->logFile, format, ap);
}

/**
 * \brief Writes error message to log file.
 *
 * \param [in] msg : Message to be displayed that explains the problem and consequences.\n
 * \param [in] xml_node_path : xml element path which cause the error.\n
 * \param [in] nxobject_path : NeXus object path which cause the error.\n
 */
void ErrorLog::log_xml_error_msg(const char* msg, const  char* xml_node_path, const  char* nxobject_path)
{
	log_write("\n=======================\n");
	log_write("Error in XML file: %s\n", msg);
	log_write("XML node path: %s\n", xml_node_path);
	log_write("NeXus node path: %s\n", nxobject_path);
	log_write("=======================\n");
}
