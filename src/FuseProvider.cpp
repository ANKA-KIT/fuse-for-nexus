/*
 * FuseProvider.cpp
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

// ./progName [FUSE options] dummy.h5 xml_file mountdir
//argv[argc-1] = mountdir = argv[3]
//argv[argc-2] = nxfile  = argv[2]
//argv[argc-3] = xmlfile   = argv[1]
//argv[argc-4] = progName = argv[0]

/**
 * 	\brief Constructor FuseProvider.
 *	\param [in] argc : number of passed arguments
 *	\param [in] argv : arguments
 *
 *	It checks the passed arguments, aborts if any mandatory arguments missing. Initializes FUSE with Filter and runs it.\n
 *	Arguments should be the following:\n
 *	1) &lt;programName&gt; -h : prints the help about mount options and FUSE options.\n
 *	2) &lt;programName&gt; [FUSE and mount options] [XML file path] &lt;NeXus file path&gt; &lt;mount directory&gt;.\n
 *	The order of arguments makes sense, do not change it.
 */
FuseProvider::FuseProvider(int argc, char** argv) {
	std::string _xmlfile_path = "";
	std::string _nxfile_path;

	if(argc == 2 && argv[argc-1][0] == '-' && argv[argc-1][1] == 'h' )
	{
		fuse_main( argc, argv, &fs_operations, NULL );

		return;
	}

	if(argc < 3)
		Filter::usage();

	if(argc < 4 && argv[1][0] == '-')
		Filter::usage();

	if(argv[argc-1][0] != '-' && argv[argc-2][0] != '-' && argv[argc-3][0] != '-' && argc > 3)
	{
		//so there is at least 4 argument, one of them is xmlfile path
		//xmlfile path should be argv[argc-3], so let's take it
		char* xmlfile_path_ptr = realpath( argv[argc-3], NULL );
		if(xmlfile_path_ptr != NULL)
			_xmlfile_path = xmlfile_path_ptr;
		else
		{
			fprintf(stderr, "XML file was not found");
			abort();
		}
		argv[argc-3] = argv[argc-2];
		argv[argc-2] = argv[argc-1];
		argv[argc-1] = NULL;
		argc--;
	}

	//check if there is 3 arguments passed. They should be:
	//argv[0] = progName
	// [ argv[1] = "-d" ]
	//argv[argc-2] = nxfile
	//argv[argc-1] = mountdir

	//argc = 3
	//argv[argc-2] = argv[3-2] = argv[1] = nxfile
	char* nxfile_path_ptr = realpath(argv[argc-2], NULL);
	if(nxfile_path_ptr != NULL)
		_nxfile_path = nxfile_path_ptr;
	else
	{
		fprintf(stderr, "NeXus file was not found");
		abort();
	}

	argv[argc-2] = argv[argc-1];
	argv[argc-1] = NULL;
	argc--;

	//private data initialization
	_fs_data.logFile = ErrorLog::log_fuse_open();//FuseProvider::log.log_fuse_open();
	_fs_data.myFilter = new Filter(_nxfile_path.c_str(), _xmlfile_path.c_str());
	_fs_data.myFilter->createTree();

	/*
	 * initializing
	 * binding the fuse functions to nfs_fuse functions
	 */
	fs_operations.open = fs_open;
	fs_operations.getattr = fs_getattr;
	fs_operations.read = fs_read;
	fs_operations.readdir = fs_readdir;
	fs_operations.init = fs_init;
	fs_operations.opendir = fs_opendir;
	fs_operations.releasedir = fs_releasedir;
	fs_operations.access = fs_access;
	fs_operations.fgetattr = fs_fgetattr;

	fprintf( stderr, "\nFUSE starting. Mounting in %s \n", argv[argc-1] );
	int fuse_stat;

	ErrorLog::changeStateToFUSEmode();
	fuse_stat = fuse_main( argc, argv, &fs_operations, reinterpret_cast<void*>( &_fs_data ) );

	fprintf( stderr, "fuse returned %d \n", fuse_stat );
	fprintf( stderr, "%s \n", strerror( fuse_stat ) );
}

/**\brief destructor FuseProvider.
 *
 */
FuseProvider::~FuseProvider() {
	//fsFilter->~Filter();
}

/*void fs_destroy (void *private_data)
{

}*/

/**\brief checks the access to object
 *
 * \param [in] path : path of filesystem object that user trying to access
 * \param [in] mask : mask of filesystem object that user trying to access
 * \return 0 if success, -ERRNO if fail.
 */
int FuseProvider::fs_access (const char *path, int mask)
{
	return 0;
}


/**\brief Releases memory after directory open
 *
 */
int FuseProvider::fs_releasedir(const char* path, struct fuse_file_info* fi)
{
	return 0;
}

/**\brief Initializing of fuse.
 *	\param [in] conn : struct fuse_conn_info contains FUSE settings.
 *	\return NXFS_DATA : private data structure, contains pointer to Filter, and pointer to FILE for logging.
 *
 *  Passing filter as a private data to FUSE.
 */
void* FuseProvider::fs_init(struct fuse_conn_info *conn)
{
    return NXFS_DATA;
}

/**\brief Gets the filesystem structure in memory.
 *
 *	\param [in] path : path of folder opened by user.
 *	\param [in] buf : buffer that used by filler.
 *	\param [in] filler : function that fills metadata about new fsobjects in FUSE.
 *	\param [in] offset : offset from function start. Not used in this implementation.
 *	\param [in] fi : file info about opened folder.
 *	\return 0 if success.
 */
int FuseProvider::fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
	       struct fuse_file_info *fi)
{
	int retStat = 0;

	retStat = filler(buf, ".", NULL, 0);
	retStat = filler(buf, "..", NULL, 0);

	std::vector<std::string> folderContent;

	try{
		folderContent = NXFS_DATA->myFilter->readdir(path);
	}catch(...)
	{
		return retStat;
	}

	for(std::string &curObject : folderContent)
	{
		retStat = filler(buf, curObject.c_str(), NULL, 0);
	}

	return retStat;
}

/**\brief Checks the access to file.
 *
 *	\param [in] path : path of folder opened by user.
 *	\param [in] fi : file info about opened filesystem object.
 *	\return 0 if success.
 */
int FuseProvider::fs_open(const char *path, struct fuse_file_info *fi)
{
	return 0;
}

/**\brief Checks the access to folder.
 *
 *	\param [in] path : path of folder opened by user.
 *	\param [in] fi : file info about opened filesystem object.
 *	\return 0 if success.
 */
int FuseProvider::fs_opendir(const char* path, struct fuse_file_info *fi)
{
	return 0;
}


/**\brief Gets the file content in memory.
 *
 * 	Reads a file part specified by the offset from file begin and the size of information read.
 *	\param [in] path : path of file opened by user.
 *	\param [in] buf : write buffer.
 *	\param [in] size : size of maximum block to read.
 *	\param [in] offset : offset from the file begin. Due to file is read by blocks.
 *	\param [in] fi : file info about opened filesystem object.
 *	\return number of bytes read.
 */
int FuseProvider::fs_read(const char *path, char *buf, size_t size,
			off_t offset, struct fuse_file_info *fi)
{
	int retFileSize = 0;
	STARTNACCESS;
	FSType type;
	try{
		type = NXFS_DATA->myFilter->getattr(path);
	}catch (NXFSException&) {
		return 0;
	}

	if(type == FSType::FILE)
	{
		std::string outputdata;
		try{
			outputdata = NXFS_DATA->myFilter->read(path);
		}catch (...) {
			return 0;
		}

		size_t len = outputdata.length();

		if(offset < len)
		{
			if(offset + size > len)
				size = len - offset;
			std::copy( ( outputdata.data() + offset ), ( outputdata.data() + offset ) + size, buf );
		}

		retFileSize = size;
	}
	else
		retFileSize = -ENOENT;
	ENDNACCESS;

	return retFileSize;
}


/**\brief Get attributes of each file/folder.
 *
 *	Writing metadata for file system objects.
 *	\param [in] path : path of file/folder accessed by user.
 *	\param [out] statbuf : buffer that contains metadata of file/folder accessed by user.
 *	\return 0 if success, -ERRNO if fail.
 */
int FuseProvider::fs_getattr(const char *path, struct stat *statbuf)
{
	//cache results
	//statbuf->keep_caching or something like that
	int retStat = 0;
	memset(statbuf, 0, sizeof(struct stat)); //cleaning memory

	if( strcmp(path, "/") == 0 )
	{
		statbuf->st_mode = S_IFDIR | 0444;
		statbuf->st_nlink = 3;
		//statbuf->st_atim;//last access
	}else
	{
		FSType fstype = FSType::NONE;
		try{
			fstype = NXFS_DATA->myFilter->getattr(path);
		}
		catch (NXFSException&) {
			std::string path_str = path;
			size_t pos = path_str.find_last_of("/");
			if(pos != std::string::npos)
			{
				pos++;
				int len = path_str.length();
				std::string command = path_str.substr(pos, (len-pos));
				if( command == "fuse_reload")
					NXFS_DATA->myFilter->reopenNXFile();
			}

			return -ENOENT;
		}

		switch(fstype)
		{
			case FSType::FILE:
				statbuf->st_mode = S_IFREG | 0444;
				statbuf->st_nlink = 1;
				statbuf->st_size = NXFS_DATA->myFilter->size(path);
				break;
			case FSType::FOLDER:
				statbuf->st_mode = S_IFDIR | 0444;
				statbuf->st_nlink = 2;
				break;
			default:
				retStat = -ENOENT;
				break;
		};

	}
	return retStat;
}

/**\brief Get attributes of each file/folder when file is opened.
 *
 *	Writing metadata for file system objects.
 *	\param [in] path : path of file/folder opened by user.
 *	\param [out] statbuf : buffer that contains metadata of file/folder accessed by user.
 *	\param [in] fi : file info about opened filesystem object.
 *	\return 0 if success, -ERRNO if fail.
 */
int FuseProvider::fs_fgetattr (const char *path, struct stat *statbuf, struct fuse_file_info *fi)
{
	return fs_getattr(path, statbuf);
}

