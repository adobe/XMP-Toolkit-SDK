// =================================================================================================
// Copyright 2008 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
//
// =================================================================================================

#ifndef __Log_h__
#define __Log_h__ 1

#include "public/include/XMP_Environment.h"

#include <stdexcept> // (xcode needs stdexcept to accept class declaration below )
#include <string>


#if XMP_WinBuild
	#pragma warning ( disable : 4996 )	// Consider using _snprintf_s instead.
	#define snprintf _snprintf
	//#define assertMsg(msg,c)																				\
	//if ( ! (c) ) {																							\
	//	Log::error ( "- assert that failed: %s\n- message: %s\n- " __FILE__ ":%u", #c, std::string( msg ).c_str(), __LINE__ );		\
	//	/* Log::error throws the exception which either fails the testcase resp. terminates dumpfile */	\
	//	/* important to do it this way such that both testrunner and standalone use is possible */			\
	//}
#endif 

//8K hopefully does for any type of error message
#define XMPQE_MAX_ERROR_LENGTH 8*1048

class LOG_Exception : public std::runtime_error {
public:
	LOG_Exception(const char* errorMsg) : std::runtime_error(errorMsg) { }
};

class Log {
	public:
		Log(const char* filename);
		~Log(void);

		// trace is identical to info, except that it can be turned of (soon)
		// ==> use trace for any non-crucial information 
		// i.e. bits and pieces in a test
		// do not use for parts of known-good-output, essential summaries, etc...
		static void trace(const char* format, ...);

		// be aware of bug1741056, feeding 64bit numbers might not output correctly
		static void info(const char* format, ...);
		static void infoNoLF(const char* format, ...);
		static void warn(const char* format, ...);
		static void error(const char* format, ...);
		static bool noErrorsOrWarnings; //set to true, if any warnings occured

		//identical to info, but immune to "mute"
		// be aware of bug1741056, feeding 64bit numbers might not output correctly
		static void important(const char* format, ...);

		//convenience overloads:
		static void info(std::string s1);
		static void important(std::string s1);
		static void error( std::string s1 );

		// mute vs. verbose
		static void setMute();
		static void setVerbose();

		static void setMuteWarnings();
		static void setVerboseWarnings();

		static bool mute;
		// NB: public on intend, such that i.e. copy commands can decide to
		// send their output to nirvana accordingly on mute==true;
		// (getter/setter on a static boolean var is fairly pointless)

		// strings to store the skipped test for different plattforms
		static std::string skippedTestsAll;
		static std::string skippedTestsWin;
		static std::string skippedTestsMac;
		static std::string skippedTestsUnix;

		// function to print the skipped tests on console and into a log file
		static void printSkippedTest(const char* filename);

	private:
		static bool singletonInstanciated;
		static FILE* logfile;
		
		//should only be temporarily used, i.e. on selftests that
		//inevitable yield (during selftet: bogus) warnings
		static bool muteWarnings;

		
};

#endif
