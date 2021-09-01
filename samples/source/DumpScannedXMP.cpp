// =================================================================================================
// Copyright 2002 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

/**
* Scans a data file to find all embedded XMP Packets, without using the smart handlers. If a packet is found, 
* serializes the XMP and writes it to log file.
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

#include "public/include/XMP.hpp"
#include "public/include/XMP.incl_cpp"


#include "XMPFiles/source/FormatSupport/XMPScanner.hpp"
#include "XMPFiles/source/FormatSupport/XMPScanner.cpp"


using namespace std;

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
ProcessPacket ( const char * fileName,
				FILE *       inFile,
			    size_t		 offset,
			    size_t       length )
{
	std::string xmlString;
	xmlString.append ( length, ' ' );
	fseek ( inFile, (long)offset, SEEK_SET );
	fread ( (void*)xmlString.data(), 1, length, inFile );
	
	char title [1000];
	
	sprintf ( title, "// Dumping raw input for \"%s\" (%zu..%zu)", fileName, offset, (offset + length - 1) );
	printf ( "// " );
	for ( size_t i = 3; i < strlen(title); ++i ) printf ( "=" );
	printf ( "\n\n%s\n\n%.*s\n\n", title, (int)length, xmlString.c_str() );
	fflush ( stdout );
	
	SXMPMeta xmpObj;
	try {
		xmpObj.ParseFromBuffer ( xmlString.c_str(), (XMP_StringLen)length );
	} catch ( ... ) {
		printf ( "## Parse failed\n\n" );
		return;
	}
	
	xmpObj.DumpObject ( DumpCallback, stdout );
	fflush ( stdout );
	
	string xmpString;
	xmpObj.SerializeToBuffer ( &xmpString, kXMP_OmitPacketWrapper );
	printf ( "\nPretty serialization, %zu bytes :\n\n%s\n", xmpString.size(), xmpString.c_str() );
	fflush ( stdout );

	xmpObj.SerializeToBuffer ( &xmpString, (kXMP_OmitPacketWrapper | kXMP_UseCompactFormat) );
	printf ( "Compact serialization, %zu bytes :\n\n%s\n", xmpString.size(), xmpString.c_str() );
	fflush ( stdout );

}	// ProcessPacket

// =================================================================================================

static void
ProcessFile ( const char * fileName  )
{
	FILE * inFile;
	size_t fileLen, readCount;
	size_t snipCount;
	char buffer [64*1024];

	// ---------------------------------------------------------------------
	// Use the scanner to find all of the packets then process each of them.
	
	inFile = fopen ( fileName, "rb" );
	if ( inFile == 0 ) {
		printf ( "Can't open \"%s\"\n", fileName );
		return;
	}
	
	fseek ( inFile, 0, SEEK_END );
	fileLen = ftell ( inFile );	// ! Only handles up to 2GB files.
	fseek ( inFile, 0, SEEK_SET );

	XMPScanner scanner ( fileLen );
	
	for ( size_t filePos = 0; true; filePos += readCount ) {
		readCount = fread ( buffer, 1, sizeof(buffer), inFile );
		if ( readCount == 0 ) break;
		scanner.Scan ( buffer, filePos, readCount );
	}
	
	snipCount = scanner.GetSnipCount();
	
	XMPScanner::SnipInfoVector snips (snipCount);
	scanner.Report ( snips );

	size_t packetCount = 0;
	for ( size_t s = 0; s < snipCount; ++s ) {
		if ( snips[s].fState == XMPScanner::eValidPacketSnip ) {
			++packetCount;
			ProcessPacket ( fileName, inFile, (size_t)snips[s].fOffset, (size_t)snips[s].fLength );
		}
	}
	if ( packetCount == 0 ) printf ( "   No packets found\n" );

}	// ProcessFile

// =================================================================================================

extern "C" int
main ( int argc, const char * argv [] )
{

	if ( ! SXMPMeta::Initialize() ) {
		printf ( "## SXMPMeta::Initialize failed!\n" );
		return -1;
	}	

	if ( argc != 2 ) // 2 := command and 1 parameter
	{
		printf ("usage: DumpScannedXMP (filename)\n");
		return 0;
	}

	for ( int i = 1; i < argc; ++i ) ProcessFile ( argv[i] );
		
	SXMPMeta::Terminate();
	return 0;

}
