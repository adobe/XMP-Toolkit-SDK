// =================================================================================================
// Copyright 2002 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

/**
* Uses the XMPFiles component API to find the main XMP Packet for a data file, serializes the XMP, and writes 
* it to a human-readable log file. This is preferred over "dumb" packet scanning.
*/

#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <ctime>

#include <cstdlib>
#include <stdexcept>
#include <cerrno>

//#define ENABLE_XMP_CPP_INTERFACE 1; 

#if XMP_WinBuild
	#pragma warning ( disable : 4127 )	// conditional expression is constant
	#pragma warning ( disable : 4996 )	// '...' was declared deprecated
#endif

#define TXMP_STRING_TYPE	std::string
#define XMP_INCLUDE_XMPFILES 1
#include "public/include/XMP.hpp"
#include "public/include/XMP.incl_cpp"

using namespace std;

static FILE * sLogFile = stdout;

// =================================================================================================

static void WriteMinorLabel ( FILE * log, const char * title )
{

	fprintf ( log, "\n// " );
	for ( size_t i = 0; i < strlen(title); ++i ) fprintf ( log, "-" );
	fprintf ( log, "--\n// %s :\n\n", title );
	fflush ( log );

}	// WriteMinorLabel

// =================================================================================================

static XMP_Status DumpCallback ( void * refCon, XMP_StringPtr outStr, XMP_StringLen outLen )
{
	XMP_Status	status	= 0; 
	size_t		count;
	FILE *		outFile	= static_cast < FILE * > ( refCon );
	
	count = fwrite ( outStr, 1, outLen, outFile );
	if ( count != outLen ) status = errno;
	return status;
	
}	// DumpCallback

// =================================================================================================

static void
ProcessFile ( const char * fileName  )
{
	bool ok;
	char buffer [1000];

	SXMPMeta  xmpMeta;	
	SXMPFiles xmpFile;
	XMP_FileFormat format;
	XMP_OptionBits openFlags, handlerFlags;
	XMP_PacketInfo xmpPacket;
	
	sprintf ( buffer, "Dumping main XMP for %s", fileName );
	WriteMinorLabel ( sLogFile, buffer );
	
	xmpFile.OpenFile ( fileName, kXMP_UnknownFile, kXMPFiles_OpenForRead );
	ok = xmpFile.GetFileInfo ( 0, &openFlags, &format, &handlerFlags );
	if ( ! ok ) return;

	fprintf ( sLogFile, "File info : format = %.8X, handler flags = %.8X\n", format, handlerFlags );
	fflush ( sLogFile );

	ok = xmpFile.GetXMP ( &xmpMeta, 0, &xmpPacket );
	if ( ! ok ) return;

	XMP_Int32 offset = (XMP_Int32)xmpPacket.offset;
	XMP_Int32 length = xmpPacket.length;
	fprintf ( sLogFile, "Packet info : offset = %d, length = %d\n", offset, length );
	fflush ( sLogFile );

	fprintf ( sLogFile, "\nInitial XMP from %s\n", fileName );
	xmpMeta.DumpObject ( DumpCallback, sLogFile );
	
	xmpFile.CloseFile();

}	// ProcessFile

// =================================================================================================

int
main ( int argc, const char * argv [] )
{

	if ( ! SXMPMeta::Initialize() ) {
		printf ( "## SXMPMeta::Initialize failed!\n" );
		return -1;
	}	
	
	XMP_OptionBits options = 0;
	#if UNIX_ENV
		options |= kXMPFiles_ServerMode;
	#endif
		
	if ( ! SXMPFiles::Initialize ( options ) ) {
	
		printf ( "## SXMPFiles::Initialize failed!\n" );
		return -1;
	}

	for ( int i = 1; i < argc; ++i ) ProcessFile ( argv[i] );
		
	SXMPFiles::Terminate();
	SXMPMeta::Terminate();
	
	return 0;
}
