// =================================================================================================
// Copyright 2005 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

/**
* A command-line tool for performing basic XMP actions such as get, put and dump. Can be used for testing 
* and scripting automation.
*/
#ifdef WIN32
	#pragma warning ( disable : 4267 )	// suppress string conversion warning
#endif

#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>

#include "samples/source/common/globals.h"
#include "samples/source/common/Log.h"
#include "samples/source/common/LargeFileAccess.hpp"
//some global constants / sanity checking


#define EXENAME "xmpcommand"

//XMP related
#define TXMP_STRING_TYPE std::string
#define XMP_INCLUDE_XMPFILES 1
#include "public/include/XMP.hpp"
#include "public/include/XMP.incl_cpp"		//include in EXACTLY one source file (i.e. main, in Action gets you trouble...)

//QE related
#include "samples/source/xmpcommand/Actions.h"
// -help output
#include "samples/source/xmpcommand/PrintUsage.h"


namespace XMPQE {
	void InitDependencies()
	{
		if ( ! SXMPMeta::Initialize() )
			Log::error( "XMPMeta::Initialize failed!" );

		XMP_OptionBits options = 0;
		#if UNIX_ENV
			options |= kXMPFiles_ServerMode;
		#endif
		
		if ( ! SXMPFiles::Initialize ( options ) )
			Log::error( "XMPFiles::Initialize failed!" );
	}

	void ShutdownDependencies()
	{
			SXMPFiles::Terminate();
			SXMPMeta::Terminate();
	}
} //namespace XMPQE

using namespace XMPQE;

int main ( int argc, const char * argv[] )
{
	Actions action; //create Action (to fill up with parameters below...)

	// ============================================================================================
	// ============================================================================================
	// ============================================================================================
	try { // on try for all

		//clean parameters (this is not elegant but might avoid some macPPC-side bus error bugs whatsoever
		action.outfile="";
		action.switch_safe=false;
		action.switch_smart=false;
		action.switch_scan=false;
		action.switch_nocheck=false;
		action.switch_compact=false;
		action.mediafile="";
		action.actiontype=Actions::NONE;
		action.xmpsnippet="";

		////////////////////////////////////////////
		// parameter verification
			if ( argc == 1 ) { //aka no parameters, just command itself.
				XMPQE::PrintUsageShort(EXENAME);
				return 0;
			} else if ( argc == 2 ) //single-paramter-call?
			{
				if ( !strcmp("help",argv[1])  ||  //only "help" and "version" permitted, ...
					!strcmp("-help",argv[1]) ||
					!strcmp("--help",argv[1]) ||
					!strcmp("-h",argv[1])     ||
					!strcmp("-?",argv[1])     ||
					!strcmp("/?",argv[1]) ) {
						XMPQE::PrintUsageLong(EXENAME);
						return 0;
				} 
				else if ( !strcmp("-version",argv[1]) || !strcmp("version",argv[1]) ) 
				{
					action.actiontype=Actions::VERSION;
				}		
				else 
				{ //..thus fail anything else
					Log::error("unknown parameter %s",argv[1]);
				}

			}
			else //multi-argument parameters
			{
				int i=1; // "for (int i=1;i<argc;i++)" ....

				while (true) {
					if (argv[i][0] != '-') {
						break; //apparently done parsing switches
					}
					if ( !strcmp("-safe",argv[i])) {
						action.switch_safe=true;
					}
					else if ( !strcmp("-smart",argv[i])) {
						action.switch_smart=true;
					}
					else if ( !strcmp("-nocheck",argv[i])) {
						action.switch_nocheck=true;
					}
					else if ( !strcmp("-compact",argv[i])) {
						action.switch_compact=true;
					}
					else if ( !strcmp("-scan",argv[i])) {
						action.switch_scan=true;
					} else if ( !strcmp("-out",argv[i])) {
						i++;  //ok, so gimme that file
						if (i>=argc) Log::error("missing output parameter, etc.");
						action.outfile=argv[i];
					} else
						Log::error("unknown switch %s",argv[i]);
					i++;
					if (i>=argc) Log::error("missing action parameter, etc.");
				}

				//check mutually exclusive
				if (action.switch_scan && action.switch_smart)
					Log::error("use either -smart or -scan");

				if ( !strcmp("info",argv[i])) {
					action.actiontype= Actions::INFO;
				} else if ( !strcmp("put",argv[i]) || !strcmp("inject",argv[i]) ) {
					action.actiontype= Actions::PUT;
				} else if ( !strcmp("get",argv[i]) || !strcmp("extract",argv[i]) ) {
					action.actiontype= Actions::GET;
				} else if ( !strcmp("dump",argv[i])) {
					action.actiontype= Actions::DUMP;
				} else
					Log::error("_unknown action %s",argv[i]);
				i++;

				//only inject needs an in-file parameter really...
				if ( action.actiontype== Actions::PUT) {
					if (i>=argc) Log::error("missing <xmpfile> parameter");
					action.xmpsnippet=argv[i];
					i++;		
				}

				//last argument must be mediafile
				if (i>=argc) Log::error("missing mediafile parameter");
				action.mediafile=argv[i];

				i++;
				if (i!=argc) Log::error("too many parameters.");
		}

		//Init ===========================================================================
		InitDependencies(); 

		//Do Action //////////////////
			Log log(action.outfile.c_str()); //will start logging to file (if filename given) or not ("")
			action.doAction();

		// Shutdown /////////////////////////////////////////////
		ShutdownDependencies();
	
	}
	catch (XMP_Error& e ) {
		Log::info("Unexpected XMP_Error %s\n", e.GetErrMsg()); //throw no further, no Log::error.. !
		return -2;
	}
	catch (LOG_Exception& q) {
		Log::info("LOG_Exception:%s\n",q.what());
		return -3;
	}
	catch (std::runtime_error& p) {
		Log::info("caught std::runtime_error exception: %s\n",p.what());
		return -4;
	}
	catch (...) {
		Log::info("unidentified error on action execution\n");
		return -5;
	}

	// ============================================================================================
	// ============================================================================================
	// ============================================================================================
		
	if (Log::noErrorsOrWarnings)
		return 0;
	//else
	Log::info("%s finished with warnings",EXENAME);
	return 10;
}

