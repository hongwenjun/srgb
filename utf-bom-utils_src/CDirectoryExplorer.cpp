/* +---------------------------------------------------------------------------+
   |          The Mobile Robot Programming Toolkit (MRPT) C++ library          |
   |                                                                           |
   |                   http://mrpt.sourceforge.net/                            |
   |                                                                           |
   |   Copyright (C) 2005-2009  University of Malaga                           |
   |                                                                           |
   |    This software was written by the Machine Perception and Intelligent    |
   |      Robotics Lab, University of Malaga (Spain).                          |
   |    Contact: Jose-Luis Blanco  <jlblanco@ctima.uma.es>                     |
   |                                                                           |
   |  This file is part of the MRPT project.                                   |
   |                                                                           |
   |     MRPT is free software: you can redistribute it and/or modify          |
   |     it under the terms of the GNU General Public License as published by  |
   |     the Free Software Foundation, either version 3 of the License, or     |
   |     (at your option) any later version.                                   |
   |                                                                           |
   |   MRPT is distributed in the hope that it will be useful,                 |
   |     but WITHOUT ANY WARRANTY; without even the implied warranty of        |
   |     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         |
   |     GNU General Public License for more details.                          |
   |                                                                           |
   |     You should have received a copy of the GNU General Public License     |
   |     along with MRPT.  If not, see <http://www.gnu.org/licenses/>.         |
   |                                                                           |
   +---------------------------------------------------------------------------+ */


#include "CDirectoryExplorer.h"

#include <stdexcept>
#include <string.h>

#define MRPT_TRY_START
#define MRPT_TRY_END
#define THROW_EXCEPTION(s) { throw std::runtime_error(s); }
#define THROW_EXCEPTION_CUSTOM_MSG1(msg,args) { throw std::runtime_error(msg); }

#if defined(_WIN32) || defined(WIN32)
	#define MRPT_OS_WINDOWS

	typedef unsigned __int64 uint64_t;

        #ifdef _MSC_VER
        	#include <sys/utime.h>
        #endif
	#include <io.h>
	#include <windows.h>
	#include <direct.h>
#else
	#define MRPT_OS_LINUX

	#include <sys/types.h>
	#include <dirent.h>
	#include <unistd.h>
	#include <time.h>
	#include <utime.h>
	#include <unistd.h>
	#include <errno.h>

	//typedef unsigned __int64 uint64_t;
#endif

#include <queue>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>


using namespace mrpt::system;
using namespace std;

/*---------------------------------------------------------------
		explore
 ---------------------------------------------------------------*/
void CDirectoryExplorer::explore(
	const string 		&path,
	const unsigned long 	in_mask,
	TFileInfoList  		&outList )
{
	MRPT_TRY_START

	unsigned long mask = in_mask;

	outList.clear();

	// The path terminated in "/" or "\\"
	string 			searchPath( path );


#ifdef MRPT_OS_WINDOWS
	// ====================
	// WINDOWS VERSION
	// ====================
	WIN32_FIND_DATAA		f;
	TFileInfo		newEntry;

	string searchPath_mask=searchPath;

	HANDLE h = FindFirstFileA( searchPath_mask.c_str(), &f);
	if (h==INVALID_HANDLE_VALUE)
		return; // No files.

	// Include the FILE_ATTRIB_ARCHIVE flag for files:
	if (mask & FILE_ATTRIB_ARCHIVE) mask |= FILE_ATTRIBUTE_NORMAL;
	do
	{
		if ( (mask & f.dwFileAttributes)!=0  )	// Passes the user masks:
		{
			// File name:
			newEntry.name = string(f.cFileName);

			// Complete file path:
			//newEntry.wholePath = searchPath;
			newEntry.wholePath = newEntry.name;

			// File size:
			newEntry.fileSize = ((uint64_t)f.nFileSizeLow) + (((uint64_t)f.nFileSizeHigh) << 32);

			// File times:
			struct stat statDat;
			if (stat( newEntry.wholePath.c_str(),&statDat ))
			{
				FindClose(h);
				THROW_EXCEPTION_CUSTOM_MSG1("Cannot get stat for file: '%s'",newEntry.wholePath.c_str())
			}

			newEntry.modTime    = statDat.st_mtime;
			newEntry.accessTime = statDat.st_atime;

			// Flags:
			newEntry.isDir = 0!=(statDat.st_mode &_S_IFDIR);
			newEntry.isSymLink = false; // (We donnot look for this in Windows, by now...)


			// Save:
			outList.push_back( newEntry );
		}
	} while(FindNextFileA(h, &f));

	FindClose(h); // Ignore possible errors..

	// Done
#else
	// ====================
	// LINUX VERSION
	// ====================
	TFileInfo		newEntry;

	newEntry.wholePath = searchPath;

	struct stat statDat, lstatDat;
	if (!stat( newEntry.wholePath.c_str(),&statDat ))
		outList.push_back( newEntry );

	return;

//	struct dirent 		*ent;
//
//	DIR *dir = opendir( searchPath.c_str() );
//	if (!dir)
//		THROW_EXCEPTION("Error starting exploration! (does path exist?)");
//
//
//	while((ent = readdir(dir)) != NULL)
//	{
//		if ( strcmp(ent->d_name,".") && strcmp(ent->d_name,"..") )
//		{
//			// File name:
//			newEntry.name = string(ent->d_name);
//
//			// Complete file path:
//			newEntry.wholePath = searchPath;
//			newEntry.wholePath += newEntry.name;
//
//			// File times:
//			struct stat statDat, lstatDat;
//			if (stat( newEntry.wholePath.c_str(),&statDat ))
//			{
//				closedir(dir);
//				THROW_EXCEPTION_CUSTOM_MSG1("Cannot get stat for file: '%s'",newEntry.wholePath.c_str())
//			}
//
//			newEntry.modTime    = statDat.st_mtime;
//			newEntry.accessTime = statDat.st_atime;
//
//			// Flags:
//			newEntry.isDir = S_ISDIR(statDat.st_mode);
//
//			if ( ( (mask & FILE_ATTRIB_ARCHIVE)!=0 && !newEntry.isDir ) ||
//			     ( (mask & FILE_ATTRIB_DIRECTORY)!=0 && newEntry.isDir ) )
//			{
//				// File size:
//				newEntry.fileSize = (intmax_t)statDat.st_size;
//
//				// Is it a symbolic link?? Need to call "lstat":
//				if (!lstat( newEntry.wholePath.c_str(),&lstatDat ))
//				{
//					newEntry.isSymLink = S_ISLNK(lstatDat.st_mode);
//				}
//				else	newEntry.isSymLink = false;
//
//				// Save:
//				outList.push_back( newEntry );
//			}
//		}
//	}
//
//	closedir(dir);

	// Done
#endif

	MRPT_TRY_END
}


// Auxiliary function to order by name, ascending
bool cmpFileEntriesName_Asc(const CDirectoryExplorer::TFileInfo &a, const CDirectoryExplorer::TFileInfo &b)
{
    return a.wholePath < b.wholePath;
}
bool cmpFileEntriesName_Desc(const CDirectoryExplorer::TFileInfo &a, const CDirectoryExplorer::TFileInfo &b)
{
    return a.wholePath < b.wholePath;
}

/*---------------------------------------------------------------
		sortByName
 ---------------------------------------------------------------*/
void CDirectoryExplorer::sortByName( TFileInfoList &lstFiles, bool ascendingOrder )
{
    std::sort(lstFiles.begin(),lstFiles.end(), ascendingOrder ? cmpFileEntriesName_Asc : cmpFileEntriesName_Desc );
}

