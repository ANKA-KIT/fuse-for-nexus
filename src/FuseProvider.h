/*
 * FuseProvider.h
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

#ifndef FUSEPROVIDER_H_
#define FUSEPROVIDER_H_
#define FUSE_USE_VERSION 26

#include <fcntl.h>
#include <fuse.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <pni/nx/NX.hpp>
#include <pni/utils/Types.hpp>
#include "limits.h"

#include "Filter/Filter.h"
#include "ErrorLog.h"

static pthread_mutex_t access_nexus = PTHREAD_MUTEX_INITIALIZER;

static void start_naccess() {
	pthread_mutex_lock(&access_nexus);
}

static void end_naccess() {
	pthread_mutex_unlock(&access_nexus);
}

#define STARTNACCESS    start_naccess()
#define ENDNACCESS      end_naccess()

/**
 * FuseProvider is intended to handle FUSE events by passing parameters to Filter.
 */
class FuseProvider {
private:
	//std::string _nxfile_path;
	//std::string _xmlfile_path;
	struct nxfs_state _fs_data;/*!< This container will be in FUSE as a private data. */

	struct fuse_operations fs_operations; /*!< The list of functions passed to FUSE. */
	static void* fs_init(struct fuse_conn_info *conn);
	static int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
		       struct fuse_file_info *fi);
	static int fs_open(const char *path, struct fuse_file_info *fi);
	static int fs_opendir(const char* path, struct fuse_file_info *fi);
	static int fs_read(const char *path, char *buf, size_t size,
			off_t offset, struct fuse_file_info *fi);
	static int fs_getattr(const char *path, struct stat *statbuf);
	static int fs_fgetattr (const char *path, struct stat *statbuf, struct fuse_file_info *fi);
	static int fs_releasedir(const char* path, struct fuse_file_info* fi);
	static int fs_access (const char *path, int mask);

public:

	FuseProvider(int argc, char** argv);
	virtual ~FuseProvider();
};

#endif /* FUSEPROVIDER_H_ */
