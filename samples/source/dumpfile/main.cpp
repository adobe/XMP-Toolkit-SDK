// =================================================================================================
// Copyright 2003 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================
//
// 
// Dumpfile is a dev utility to understand, and to a certain degree validate file structures, with 
// specific focus on metadata related aspects, naturally. Dumpfile is NOT meant to extract
// valid metadata from files. Dumpfile is NOT meant to be a production tool.
// NO reconciliation, NO XMP logic, NO attention to encoding matters, etc.
// 
// Dumpfile gives developers a view on files just like a hex editor, 
// slightly more readible but in no way more sophisticated.

//sanity check platform/endianess
#include "samples/source/common/globals.h"
#if XMP_WinBuild
	#pragma warning ( disable : 4267 )	// suppress string conversion warning
	//	#pragma warning ( disable : 1234 )	// say what you do here
#endif

//only define in one non-public-source, non-header(.cpp) place

const int DUMPFILEVERSION=2;
#define EXENAME "dumpfile"

//standard stuff
#include <stdexcept>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <vector>
#include <sstream>

//XMPCore (only!)
#define TXMP_STRING_TYPE std::string
#include "public/include/XMP.hpp"			//NB: no XMP.incl_cpp here on purpose, gets compiled in main...
#include "public/include/XMP.incl_cpp"		//include in EXACTLY one source file (i.e. main, in Action gets you trouble...)
#include "public/include/XMP_Const.h"
#include "source/XML_Node.cpp"

//utils
#include "samples/source/common/Log.h"
#include "samples/source/common/LargeFileAccess.hpp"
#include "samples/source/common/DumpFile.h"
using namespace std;

void printUsageShort() {
	Log::info("%s -version",EXENAME);
	Log::info("    Print version information for this utility");
	Log::info("");
	Log::info("%s -help",EXENAME);
	Log::info("    Print descriptions of all available switches, including examples");
	Log::info("");
	Log::info("%s [ -help | -version | [-keys|-tree|-list] [-nocomments] <path> ]",EXENAME);
	Log::info("");
	Log::info("Copyright 2008 Adobe Systems Incorporated. All Rights Reserved.");
	Log::info("");
}

void printUsageLong() {
	Log::info("%s -version",EXENAME);
	Log::info("    Print version information for this utility");
	Log::info("");
	Log::info("%s -help",EXENAME);
	Log::info("    Print descriptions of all available switches, including examples");
	Log::info("");
	Log::info("%s [ -help | -version | [-keys|-tree|-list] [-nocomments] <path> ]",EXENAME);
	Log::info("");
	Log::info("Copyright 2008 Adobe Systems Incorporated. All Rights Reserved.");
	Log::info("NOTICE:  Adobe permits you to use, modify, and distribute this file in");
	Log::info("         accordance with the terms of the Adobe license agreement");
	Log::info("         accompanying it.");
	Log::info("");
	Log::info("Switches:");
	Log::info("-help    Prints usage information for the command.");
	Log::info("");
	Log::info("-version Prints version information for this tool, and for the version");
	Log::info("         of XMPCore to which it is statically linked. (NB: Dumpfile does");
	Log::info("         not use XMPFiles.");
	Log::info("");
	Log::info("-tree    Shows the tree structure of the file. (default)");
	Log::info("");
	Log::info("-keys    Shows only the key-value nodes found in the file, as a list with no");
	Log::info("         hierarchical structure, in alphabetical order by key.");
	Log::info("");
	Log::info("-list    Shows only the key-value nodes found in the file, as a list with no");
	Log::info("         hierarchical structure, in the order of parsing.");
	Log::info("");
	Log::info("-nocomments   Supresses the comment portion of key-value nodes.");
	Log::info("");
	
	Log::info("");
	Log::info("Examples:");
	Log::info("%s -keys Sample.jpg",EXENAME);
	Log::info("");
	Log::info("dumpfile returns 0 if there are no errors. Note: Warnings and errors");
	Log::info("are printed as part of the stdout output, not to stderr.");
	Log::info("");
}

int
main( int argc, char* argv[] )
{
	//if both not flagged will default to tree-only (below)
	bool treeFlag=false;
	bool keysFlag=false;
	bool listFlag=false;
	bool commentsFlag=true;	// true <=> do not supress comments (supressing them is better for kgo)

	//NB: not logging to file since mostly pointless...
	int returnValue=0; //assume all will be good

	int i=1; //define outside scope to access in catch clause
	try {
		if ( ! SXMPMeta::Initialize( ) )
		{
			Log::info( "SXMPMeta::Initialize failed!" );
			return -1;
		}

		//no parameters? print usage.
		if ( argc == 1 )
		{
			printUsageShort();
			return 0;
		}

		//one parameter? check...
		if (
			!strcmp("-help",argv[1]) ||
			!strcmp("--help",argv[1]) ||
			!strcmp("-h",argv[1])     ||
			!strcmp("-?",argv[1])     ||
			!strcmp("/?",argv[1]) )
		{
			printUsageLong();
			return 0;
		} 
		else if ( !strcmp("-version",argv[1]) ) 
		{
			XMP_VersionInfo coreVersion;
			SXMPMeta::GetVersionInfo ( &coreVersion );
			Log::info("using XMPCore %s", coreVersion.message );
			Log::info("dumpfile Version %d", DUMPFILEVERSION );
			return 0;
		}

		//multiple parameters? all but last must be switches!
		while (i < argc-1) //while not last parameter
		{
			if ( !strcmp("-keys",argv[i]) ) 
				keysFlag=true;
			else if ( !strcmp("-tree",argv[i]) ) 
				treeFlag=true;
			else if ( !strcmp("-list",argv[i]) ) 
				listFlag=true;
			else if ( !strcmp("-nocomments",argv[i]) ) 
				commentsFlag=false;
			else if (argv[i][0] == '-') //yet-another-switch?
			{
				Log::info("unknown switch %s",argv[i]);
				printUsageShort();
				return -1;
			}
			else
			{
				Log::info("only one image at a time: %s",argv[i]);
				printUsageShort();
				return -1;
			}
			i++;
		} //while
	
		if (!treeFlag && !keysFlag && !listFlag) //if nothing request default to "-tree"
			treeFlag=true;

		TagTree tree;
		std::string errMsg;
		try  //parsing failures on incorrect files should still lead to partial dump
		{
			DumpFile::Scan(argv[i],tree);
		}
		catch (DumpFileException& d)
		{
			tree.popAllNodes();	
			returnValue = -9; //earmark but keep going
			errMsg=d.what_dumpfile_reason();
		}

		if (treeFlag)
			tree.dumpTree(commentsFlag);
		if (treeFlag && keysFlag) //separator if needed
			Log::info("------------------------------------------------------------------------");
		if (keysFlag)
			tree.dumpTagMap();
		if ( (treeFlag || keysFlag) && listFlag) //separator if needed
			Log::info("------------------------------------------------------------------------");
		if (listFlag)
			tree.dumpTagList();	

		//order appears more logical to print after the dumping happended:
		if (returnValue != 0) {
			Log::info("parsing failed. \n%s",errMsg.c_str());
		}

		SXMPMeta::Terminate();
	}	
	catch (XMP_Error& x ) {
		Log::info("XMPError on file %s:\n%s", argv[i] , x.GetErrMsg() );
		return -3;
	}
	catch (LOG_Exception& q) { 
		//also used by assert which are very self-explanatory
		//neutral wording for standalone app
		Log::info("INTERNAL parsing error\n- file: %s\n%s", argv[i] , q.what() );
		return -4;
	}
	catch ( std::exception &e )
	{
		Log::info("std:exception on file %s:\n%s", argv[i] , e.what() );
		return -5;
	}
	catch (...)
	{
		Log::info("unknown (...) exception on file %s", argv[i] );
		return -6;		
	}

	if (returnValue==0)
		Log::info("OK");
	else
		Log::info("ERROR(s) encountered");
	return returnValue;
}
