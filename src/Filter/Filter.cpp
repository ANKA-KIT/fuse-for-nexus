/*
 * Filter.cpp
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

#include "Filter.h"

#include <stdlib.h>
#include "../ErrorLog.h"

XMLFile Filter::_xmlfile;
NXGateway Filter::_nxgate;
FSTree Filter::_nxtree;
NXFSCache* Filter::_cache;

/**
 * Constructor Filter
 */
Filter::Filter() {_cache = new NXFSCache(_nxtree);}

/**
 * \brief Constructor Filter.
 *
 * Aborts if passed parameters are not valid.
 * \param [in] nxfile_path : path to NeXus file to be opened
 * \param [in] xmlfile_path : path to XML file to be opened
 *
 */
Filter::Filter(const char* nxfile_path, const char* xmlfile_path)
{
	_nx_path = nxfile_path;
	_xml_path = xmlfile_path;
	_cache = new NXFSCache(_nxtree);

	if(_xml_path.empty())
	{
		char* real_path_out;
		real_path_out = realpath( RULES_XMLFILE_PATH, NULL );
		if(real_path_out != NULL)
			_xml_path = real_path_out;
		else
		{
			fprintf(stderr, "Having troubles while getting default xmlfile (%s): %s \n", RULES_XMLFILE_PATH, strerror(errno));
			usage();
			abort();
		}
	}

	if(_xml_path.empty())
	{
		fprintf(stderr, "Having some troubles with xmlfile (path: %s) \n", _xml_path.c_str());
		usage();
		abort();
	}

	try{
		_xmlfile.load_file(_xml_path.c_str());
	}catch (NXFSException& e) {
		fprintf(stderr, "Having troubles while opening passed xml file (%s): %s\n", _xml_path.c_str(), e.what() );
		usage();
		abort();
	}

	try
	{
		_nxgate.load_file( _nx_path.c_str() );
	}catch (NXFSException& e) {
		fprintf(stderr, "Having troubles while opening passed NeXus file (%s): %s\n", _nx_path.c_str(), e.what() );
		usage();
		abort();
	}

}

/**
 * \brief Prints out in stderr the usage of application.
 *
 */
void Filter::usage()
{
	fprintf(stderr, "usage:  NXFS 1.[FUSE and mount options] 2.[xmlFile] 3. <rootNexusFile>  4. <mountPoint>\nDo not change the order of these arguments.\n");
	abort();
}

/**
 * destructor Filter
 */
Filter::~Filter() {
	_nxgate.~NXGateway();
	_nxtree.~FSTree();
	_xmlfile.~XMLFile();
	_cache->~NXFSCache();
}

/**
 * \brief Creates and inserts FSObject for all NXObjects in NXGroup.
 *
 * Recursive function.
 * \param [in] nxgroup : NeXus group
 * \param [in] fsparent : parent FSObject folder
 */
void Filter::addGroup(pninx::NXGroup nxgroup, FSObject& fsparent)
{
	for(pninx::NXObject &nxobject : nxgroup)
	{
		FSObject fsobj;

		fsobj.name = nxobject.name();

		fsobj.fullpath = fsparent.fullpath;
		fsobj.fullpath += "/";
		fsobj.fullpath += fsobj.name;

		//todo: if exception
		if(_nxtree.insert(fsobj, fsparent) != 0)
		{
			//todo log error
			return;
		}

		createBehavior(fsobj, nxobject);
		fsobj = _nxtree.last();

		//todo think how to check further browsing
		if( nxobject.object_type() == pni::nx::NXObjectType::NXGROUP )
		{
			addGroup(nxobject, fsobj);
		}
	}
}

/**
 * \brief Creates and inserts FSObject for all NXObjects in root NXGroup.
 * \param [in] nxgroup : NeXus group
 */
void Filter::addRootGroup(pninx::NXGroup& nxgroup)
{
	for(pninx::NXObject &nxobject : nxgroup)
	{
		FSObject fsobj;

		fsobj.name = nxobject.name();
		fsobj.fullpath = "/";
		// todo: think how to substitude it
		fsobj.fullpath += fsobj.name;

		std::string root_str = "/";
		if(_nxtree.insert(fsobj, root_str) != 0)
		{
			ErrorLog::log_write("An internal error occurred. Cannot create filtesystem.");
			return;
		}
		createBehavior(fsobj, nxobject);

		//if( fsobj.Behaviour->getattr(nxobject) == FSType::FOLDER )
		//todo: think how to do it better. if there is some group that should be displayed as a file
		if( nxobject.object_type() == pni::nx::NXObjectType::NXGROUP )
		{
			addGroup(nxobject, fsobj);
		}
	}
}

/**
 * \brief Creates and inserts files into folder
 * \param [in] nxobj : NeXus object that has has to be displayed as a folder with subfiles.
 * \param [in] parent : parent FSObject of files that will be created.
 * \param [in] behaviour : Rule to be applied to files.
 */
void Filter::createSubFiles(Rule* behaviour, pninx::NXObject& nxobj,  FSObject& parent)
{
	/*
	 * algorythm:
	 * 1. get the shape of nxobject data
	 * 2. check that shape is 3d, if not then do not apply, and log error
	 * 3. for each slice of nxobject ( slice(i,width,height) ) make fsobject
	 * 	3.1. fsobject rule is passed rule (behaviour)
	 * 	3.2. fsobject options should have the following option: <"part", i>
	 * 	3.3. add just created fsobject to fstree
	 * 4. return
	 */
	//todo: check if there is several rule types

	ImageRule* myRule = reinterpret_cast<ImageRule*>(behaviour);
	subFiles data = myRule->createSubFiles( nxobj );
	if( data.isValid() )
	{
		size_t entityNum = data.num;

		for( size_t i=0; i<entityNum ;i++ )
		{
			FSObject curFSobj;
			std::string num = static_cast<std::ostringstream*>( &(std::ostringstream() << i) )->str();
			curFSobj.name = num + data.extension;
			curFSobj.fullpath = parent.fullpath + "/" + curFSobj.name;
			curFSobj.setNXObjectPath(nxobj.path());
			curFSobj.rule = myRule;
			curFSobj.rule->changeFSType( FSType::FILE );
			curFSobj.rule->addOption( "part_number", num );

			//todo: log error
			if(_nxtree.insert( curFSobj, parent ) != 0)
				continue;
		}
	}
}

/**
 * \brief Creates hardcoded Rule.
 *
 * If nxobject object type is NXFIELD, it creates Rule with <fsobject_type = FILE> option.\n
 * If nxobject object type is NXGROUP, it creates Rule with <fsobject_type = FOLDER> option.
 *
 * \param [in] nxobject : NeXus object to which Rule will be applied to.
 * \return Created Rule.
 */
Rule* Filter::createHardcodedBehavior(pninx::NXObject &nxobject)
{
	Rule* behavior = new Rule();

	//default hardcoded rule
	FSType obj_type = (nxobject.object_type() == pni::nx::NXObjectType::NXFIELD) ? FSType::FILE : FSType::FOLDER;
	std::string obj_type_str = (obj_type == FSType::FILE) ? "FILE" : "FOLDER";
	behavior->addOption("fsobject_type", obj_type_str);

	return behavior;
}


/**
 * \brief Trying to create default Rule for NeXus Object.
 *
 * Checks the XML file for default rule. If such rule found it replaces passed Rule with new one.\n
 *
 * \param [in] nxobject : NeXus object to which Rule will be applied to.
 * \param [out] behavior : if default Rule found, then behavior is default Rule, else it's the same.
 */
void Filter::tryCreateDefaultBehavior(Rule* &behavior, pninx::NXObject &nxobject)
{
	std::string rule_name;
	for( auto attr = nxobject.attr_begin(); attr != nxobject.attr_end(); attr++ )
	{
		rule_name = attr->name();
		bool match = _xmlfile.findDefaultRule( rule_name );

		if(match)
		{
			Rule* found = new Rule();
			found->setOptions( _xmlfile.fetchDefaultRule(rule_name) );
			if(found->isValidOptions())
			{
				delete behavior;
				behavior = found;
			}
		}
	}
}


/**
 * \brief Trying to create specific Rule for NeXus Object.
 *
 * Checks the XML file for specific rule. If such rule found it replaces passed Rule with new one.\n
 * Some changes in fsobject may be, it depends on Rule.
 *
 * \param [in] nxobject : NeXus object to which Rule will be applied to.
 * \param [out] behavior : if specific Rule found, then behavior is default Rule, else it's the same.
 * \param [out] fsobj : may be changed, e.g. added subfiles for this FSObject, it depends on Rule implementation.
 */
void Filter::tryCreateSpecificBehavior(Rule* &behavior, pninx::NXObject& nxobject, FSObject& fsobj)
{
	std::string specificRuleName;
	if( !( ( specificRuleName = _xmlfile.findSpecificRule( nxobject.path().c_str() ) ).empty() ) )
	{
		if(specificRuleName == "image")
		{
			//get this specific rule from xml file
			ImageRule* imageRule = new ImageRule();
			imageRule->setOptions( _xmlfile.fetchSpecificRule( nxobject.path().c_str() ) );
			if(imageRule->isValidOptions())
			{
				delete behavior;
				behavior = new Rule();
				behavior->addOption("fsobject_type", "FOLDER");
				createSubFiles(imageRule, nxobject, fsobj);
			}
			else
			{
				ErrorLog::log_xml_error_msg("There are some mandatory options missing", "unknown", nxobject.path().c_str());
				return;
			}
		}
		else
		{
			Rule* rule;
			if(specificRuleName == "table_csv")
			{
				rule = new TableRule();
				rule->setOptions( _xmlfile.fetchSpecificRule( nxobject.path().c_str() ) );
				if(!rule->isValidOptions())
				{
					ErrorLog::log_xml_error_msg("There are some mandatory options missing", "unknown", nxobject.path().c_str());
					return;
				}
			}
			else
			{
				rule = new Rule();
				rule->setOptions( _xmlfile.fetchSpecificRule( nxobject.path().c_str() ) );
				if(!rule->isValidOptions())
				{
					ErrorLog::log_xml_error_msg("There are some mandatory options missing", "unknown", nxobject.path().c_str());
					return;
				}
			}
			delete behavior;
			behavior = rule;
		}
	}
}

/**
 *	\brief Creates Rule for NeXus object in accordance with Rules in XML file.
 *
 *	\param [out] fsobj : changes name and fullpath if there are such options in fsobj.rule.
 *	\param [in] nxobject : NXObject the Rule created for.
 */
void Filter::createBehavior(FSObject &fsobj, pninx::NXObject &nxobject)
{
	Rule* behavior = createHardcodedBehavior(nxobject);
	tryCreateDefaultBehavior(behavior, nxobject);
	tryCreateSpecificBehavior(behavior, nxobject, fsobj);

	_nxtree[fsobj.fullpath.c_str()].rule = behavior;
	_nxtree[fsobj.fullpath.c_str()].setNXObjectPath(nxobject.path());

	std::string ext_str = _nxtree[fsobj.fullpath.c_str()].rule->getOptionValue("extension");
	if( !ext_str.empty() )
	{
		std::string new_filename = fsobj.name;
		new_filename += ext_str;
		_nxtree.changeFSObjectName(fsobj.fullpath.c_str(), new_filename.c_str());
	}
}

//todo add description
/**
 *	\brief Browses through NeXus file and creates filesystem object (FSObject) for each NeXus object.
 *
 */
void Filter::createTree()
{
	pninx::NXGroup root_group = _nxgate.getNXObjectByPath("/");

	Rule* behaviuor = new Rule();
	behaviuor->addOption("fsobject_type", "FOLDER");
	_nxtree["/"].rule = behaviuor;

	addRootGroup( root_group );
}

/**
 *	\brief Gets FSObject specified by path.
 *
 *	\param [in] path : fullpath of FSObject.
 *	\return FSObject specified by path or empty FSObject.
 */
FSObject& Filter::fsobjectAt( const char* path )
{
	try{
		return _nxtree.find(path);
	}catch (NXFSException& e) {
		//todo: substitute exception to null return
		std::string err_msg = "Error appears: Filter can't find object due to: ";
		err_msg +=  e.what();
		throw NXFSException( err_msg );
	}
	FSObject output;
	return output;
}

/**
 * \brief Gets FSType of file/folder specified by path.
 *
 * \param [in] path : fullpath of FSObject.
 * \return FSType of file/folder.
 */
FSType Filter::getattr( const char* path )
{
	FSType type = FSType::NONE;
	try{
		type = fsobjectAt( path ).getattr();
	}catch (NXFSException& e) {
		//todo: substitute exception to null return
		std::string err_msg = "Error appears: Filter can't get attributes due to: ";
		err_msg +=  e.what();
		throw NXFSException( err_msg );
	}

	return type;
}

/**
 * \brief Gets content of file specified by path.
 *
 * \param [in] path : fullpath of FSObject.
 * \return content of file as string.
 */
std::string Filter::read( const char* path )
{
	std::string output = _cache->read(path);
	//return fsobjectAt( path ).read();
	return output;
}

/**
 * \brief Gets list of file/folder names which stored in folder specified by path.
 *
 * \param [in] path : fullpath of FSObject.
 * \return list of file/folder names
 */
std::vector<std::string> Filter::readdir( const char* path )
{
	return fsobjectAt( path ).readdir();
}

/**
 * \brief Gets size of file specified by path
 *
 * \param [in] path : fullpath of FSObject.
 * \return size of file in bytes
 */
size_t Filter::size( const char* path )
{
	return fsobjectAt( path ).size();
}

/**
 * \brief reopens the NeXus file.
 *
 *	It reopens the NeXus file in order to achieve new data from it if it was updated.\n
 *	If any error occurred it will abort.
 */
void Filter::reopenNXFile()
{
	try
	{
		this->_nxgate.load_file(this->_nx_path.c_str());
		fprintf(stderr, "NXFS: NeXus file reopened\n");
	}catch (NXFSException& e) {
		fprintf(stdout, "The error occurred while reopening NeXus file (%s): %s", e.what(), this->_nx_path.c_str() );
		abort();
	}
}
