// =================================================================================================
// Copyright 2008 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
//
// =================================================================================================

/*
*
* a little set of functions for logging info, warnings, errors either to
* stdout or a file (this is no class on purpose, to avoid having to pass
* a particular instance on and on...)
*
* you can call the (static) log funcitons right away (resulting in stdout)
* or create one (1!, singleton) instance first telling the outfile
*  recommendation: do that as a scoped variable not using new, since ~destruction == proper file closure...
* (following exception-safe RAII-philosophy http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization )
*
*/

#ifdef WIN32
	#pragma warning ( disable : 4996 )	// fopen was declared deprecated...
#endif

#include <stdio.h>
#include <stdarg.h>

#include "samples/source/common/globals.h"
#include "samples/source/common/Log.h"
using namespace std;

//static variables ////////////////////////////////////////////////////////////////////
FILE* Log::logfile=NULL;
bool Log::noErrorsOrWarnings=true;
bool Log::mute=false;
bool Log::muteWarnings=false;

// strings for skipped test information
std::string Log::skippedTestsAll;
std::string Log::skippedTestsWin;
std::string Log::skippedTestsMac;
std::string Log::skippedTestsUnix;
//////////////////////////////////////////////////////////////////////////////////////

/* standard log, use filename=NULL or filename="" or do not construct at all
 * for logging to stdout only
 * (otherwise stuff gets logged to stdout *and* to the logfile specified)
 */
Log::Log(const char* filename)
{
	if (Log::logfile) { //enfore singleton
		printf("tried to create two logs (forbidden, singleton)");
		throw LOG_Exception("tried to create two logs (forbidden, singleton)");
	}
	if (!filename)
		logfile=NULL; //OLD: logfile=stdout;
	else if (!strcmp(filename,""))
		logfile=NULL; //empty string
	else {
		logfile=fopen(filename,"wt");
		if (!logfile) {
			printf("could not open output file %s for writing",filename); //do in addition to make sure it's output
			throw LOG_Exception("could not open output file for writing");
		}
	}
}

Log::~Log()
{
	if(logfile) {
		//info("logfile properly closed.");
		fclose(logfile);
		logfile=NULL; //BUGSOURCE: null out such that any possible following traces do not go to invalid file...
	}
}

/*
* outputs to file and standardout
* (Note: it would be wunderbar to group the essential output into one function,
*  regretably that type for  "..."-paramter forwarding doesn't go down well with logging MACROS (__DATE etc...)
*  thus this may be the best solution
*/

void Log::trace(const char* format, ...) {
	if (Log::mute) return;
	va_list args; va_start(args, format);
	//stdout
	vprintf( format, args);
	printf( "\n");
	//file
	if(logfile) {
		vfprintf( logfile, format, args);
		fprintf( logfile, "\n");
	}
	va_end(args);
}

void Log::info(const char* format, ...) {
	// experience from TagTree, not encountered in Log.
	//note: format and ... are somehow "used up", i.e. dumping them
	//      via vsprintf _and_ via printf brought up errors on Mac (only)
	//      i.e. %d %X stuff looking odd (roughly like signed vs unsigned...)
	//      buffer reuse as in (2) is fine, just dont use format/... twice.

	if (Log::mute) return;
	va_list args; va_start(args, format);
	//stdout
	vprintf( format, args);
	printf( "\n");
	//file
	if(logfile) {
		vfprintf( logfile, format, args);
		fprintf( logfile, "\n");
	}
	va_end(args);
}

void Log::important(const char* format, ...) {
	va_list args; va_start(args, format);
	//stdout
	vprintf( format, args);
	printf( "\n");
	//file
	if(logfile) {
		vfprintf( logfile, format, args);
		fprintf( logfile, "\n");
	}
	va_end(args);
}


void Log::infoNoLF(const char* format, ...) {
	if (Log::mute) return;
	va_list args; va_start(args, format);
	//stdout
	vprintf( format, args);
	//file
	if(logfile) {
		vfprintf( logfile, format, args);
	}
	va_end(args);
}

void Log::warn(const char* format, ...){
	if (Log::muteWarnings) return;  //bogus warnings or neither output nor do they 'count'

	noErrorsOrWarnings=false; //too bad...
	va_list args; va_start(args, format);
	//stdout
	printf( "warning:" );
	vprintf( format, args);
	printf( "\n");
	//file
	if(logfile) {
		fprintf( logfile, "warning:");
		vfprintf( logfile, format, args);
		fprintf( logfile, "\n");
	}
	va_end(args);
}

void Log::error(const char* format, ...){
	noErrorsOrWarnings=false; //too bad...
	va_list args; va_start(args, format);
		//since many errors in test are caused by intend in testing
		//(so you don't want to see them), do not output directly here,
		//but leave that to a particular dump-Function
		//(which should be part of any catch(LOG_Exception))
		char buffer [XMPQE_MAX_ERROR_LENGTH];  //let's hope that fits (see .h file)
		vsnprintf( buffer, XMPQE_MAX_ERROR_LENGTH, format, args);
		//if not existing on sme unix consider going back to (minimally less secure vsprintf (w/o the n))
	va_end(args);
	throw LOG_Exception(buffer);
	//may throw exception ONLY, no exit()-call, for cppunit's sake
}


///////////////////////////////////////////////////////////////////////////////
//convenience overloads

void Log::info(std::string s1)
{
	if (Log::mute) return;
	info(s1.c_str());
}

void Log::important(std::string s1)
{
	info(s1.c_str());
}

void Log::error( std::string s1 )
{
	error( s1.c_str() );
}

///////////////////////////////////////////////////////////////////////////////
// skipped tests
void Log::printSkippedTest(const char* filename)
{
	std::string completeString = "<html><head><title>Skipped Tests:</title><head><body>";
	if (!skippedTestsAll.empty())
		completeString.append("<br><br><table border=\"1\"><caption>all Plattforms:</caption><tr><th>Path to Test</th><th>comment</th></tr>"+skippedTestsAll + "</table>");
	if (!skippedTestsWin.empty())
		completeString.append("<br><br><table border=\"1\"><caption>Windows:</caption><tr><th>Path to Test</th><th>comment</th></tr>"+skippedTestsWin+ "</table>");
	if (!skippedTestsMac.empty())
		completeString.append("<br><br><table border=\"1\"><caption>Macintosh:</caption><tr><th>Path to Test</th><th>comment</th></tr>"+skippedTestsMac+ "</table>");
	if (!skippedTestsUnix.empty())
		completeString.append("<br><br><table border=\"1\"><caption>UNIX:</caption><tr><th>Path to Test</th><th>comment</th></tr>"+skippedTestsUnix+ "</table>");

	completeString.append("</body></html>");
	// print to console if mute is off
	/*if (!Log::mute)
		printf(completeString.c_str());*/

	//print result to file
	FILE* outputFile=fopen(filename,"wt");
	if (!outputFile) {
		printf("could not open output file %s for writing",filename); //do in addition to make sure it's output
		throw LOG_Exception("could not open output file for writing");
	}
	fprintf( outputFile, "%s", completeString.c_str());
	if(outputFile) {
		fclose(outputFile);
	}

}

/////////////////////////////////////////////////////////////////////////////////
// mute vs. verbose

void Log::setMute()
{
	Log::mute = true;
}

void Log::setVerbose()
{
	Log::mute = false;
}

void Log::setMuteWarnings()
{
	Log::muteWarnings = true;
}

void Log::setVerboseWarnings()
{
	Log::muteWarnings = false;
}
