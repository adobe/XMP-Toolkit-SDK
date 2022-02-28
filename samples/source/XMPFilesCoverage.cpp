// =================================================================================================
// Copyright 2002 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

/**
* Demonstrates syntax and usage by exercising most of the API functions of XMPFiles Toolkit SDK component, 
* using a sample XMP Packet that contains all of the different property and attribute types.
*/

#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include <cerrno>
#include <ctime>

#define TXMP_STRING_TYPE std::string
#define XMP_INCLUDE_XMPFILES 1
#include "public/include/XMP.hpp"
#include "public/include/XMP.incl_cpp"

//#define ENABLE_XMP_CPP_INTERFACE 1;

using namespace std;

#if WIN_ENV
	#pragma warning ( disable : 4100 )	// ignore unused variable
	#pragma warning ( disable : 4127 )	// conditional expression is constant
	#pragma warning ( disable : 4505 )	// unreferenced local function has been removed
	#pragma warning ( disable : 4996 )	// '...' was declared deprecated
#endif

// -------------------------------------------------------------------------------------------------

FILE * sLogFile = 0;

// -------------------------------------------------------------------------------------------------

static void WriteMinorLabel ( FILE * log, const char * title )
{

	fprintf ( log, "\n// " );
	for ( size_t i = 0; i < strlen(title); ++i ) fprintf ( log, "-" );
	fprintf ( log, "--\n// %s :\n\n", title );

}	// WriteMinorLabel

// -------------------------------------------------------------------------------------------------

static XMP_Status DumpToFile ( void * refCon, XMP_StringPtr outStr, XMP_StringLen outLen )
{
	XMP_Status	status	= 0; 
	size_t		count;
	FILE *		outFile	= static_cast < FILE * > ( refCon );
	
	count = fwrite ( outStr, 1, outLen, outFile );
	if ( count != outLen ) status = errno;
	return status;
	
}	// DumpToFile

// -------------------------------------------------------------------------------------------------

static XMP_Status DumpToString ( void * refCon, XMP_StringPtr outStr, XMP_StringLen outLen )
{
	std::string * fullStr = static_cast < std::string * > ( refCon );
	
	fullStr->append ( outStr, outLen );
	return 0;
	
}	// DumpToString

// -------------------------------------------------------------------------------------------------

#define DumpOneFormat(fmt)													\
	format = kXMP_ ## fmt ## File;											\
	flags = 0;																\
	ok = SXMPFiles::GetFormatInfo ( format, &flags );						\
	fprintf ( sLogFile, "kXMP_" #fmt "File = %.8X, %s, flags = 0x%X\n",		\
			  format, (ok ? "smart" : "dumb"), flags );

static void DumpHandlerInfo()
{

	WriteMinorLabel ( sLogFile, "Dump file format constants and handler flags" );

	bool ok;
	XMP_FileFormat format;
	XMP_OptionBits flags;

	DumpOneFormat ( PDF );
	DumpOneFormat ( PostScript );
	DumpOneFormat ( EPS );

	DumpOneFormat ( JPEG );
	DumpOneFormat ( JPEG2K );
	DumpOneFormat ( TIFF );
	DumpOneFormat ( GIF );
	DumpOneFormat ( PNG );

	DumpOneFormat ( MOV );
	DumpOneFormat ( AVI );
	DumpOneFormat ( CIN );
	DumpOneFormat ( WAV );
	DumpOneFormat ( MP3 );
	DumpOneFormat ( SES );
	DumpOneFormat ( CEL );
	DumpOneFormat ( MPEG );
	DumpOneFormat ( MPEG2 );
	DumpOneFormat ( WMAV );

	DumpOneFormat ( HTML );
	DumpOneFormat ( XML );
	DumpOneFormat ( Text );

	DumpOneFormat ( Photoshop );
	DumpOneFormat ( Illustrator );
	DumpOneFormat ( InDesign );
	DumpOneFormat ( AEProject );
	DumpOneFormat ( AEFilterPreset );
	DumpOneFormat ( EncoreProject );
	DumpOneFormat ( PremiereProject );
	DumpOneFormat ( PremiereTitle );

	DumpOneFormat ( Unknown );

}	// DumpHandlerInfo

// -------------------------------------------------------------------------------------------------

static void OpenTestFile ( const char * fileName, XMP_OptionBits rwMode, SXMPMeta* xmpMeta, SXMPFiles* xmpFile )
{
	bool ok;

	XMP_FileFormat format;
	XMP_OptionBits openFlags, handlerFlags;
	XMP_PacketInfo xmpPacket;
	
	bool isUpdate = ((rwMode & kXMPFiles_OpenForUpdate) != 0);
	
	static const char * charForms[] = { "UTF-8", "unknown char form", "UTF-16BE", "UTF-16LE", "UTF-32BE", "UTF-32LE" };

	XMP_OptionBits smartFlags = rwMode | kXMPFiles_OpenUseSmartHandler;
	XMP_OptionBits scanFlags  = rwMode | kXMPFiles_OpenUsePacketScanning;
	
	ok = xmpFile->OpenFile ( fileName, kXMP_UnknownFile, smartFlags );
	if ( ! ok ) {
		fprintf ( sLogFile, "Failed to get a smart handler\n" );
		ok = xmpFile->OpenFile ( fileName, kXMP_UnknownFile, scanFlags );
		if ( ! ok ) return;
	}

	ok = xmpFile->GetFileInfo ( 0, &openFlags, &format, &handlerFlags );
	if ( ! ok ) return;

	fprintf ( sLogFile, "File info : format = %.8X, handler flags = 0x%X, open flags = 0x%X (%s)\n",
			  format, handlerFlags, openFlags, (isUpdate ? "update" : "read-only") );

	ok = xmpFile->GetXMP ( xmpMeta, 0, &xmpPacket );
	if ( ! ok ) {
		fprintf ( sLogFile, "No XMP\n" );
		return;
	}

	fprintf ( sLogFile, "XMP packet info : file offset = %lld, length = %d, pad = %d",
			  xmpPacket.offset, xmpPacket.length, xmpPacket.padSize );
	fprintf ( sLogFile, ", %s", ((xmpPacket.charForm > 5) ? "bad char form" : charForms[xmpPacket.charForm]) );
	fprintf ( sLogFile, ", %s\n", (xmpPacket.writeable ? "writeable" : "read-only") );
	
	fprintf ( sLogFile, "\n" );

	// Remove extaneous properties to make the dump smaller.
	SXMPUtils::RemoveProperties ( xmpMeta, kXMP_NS_XMP, "Thumbnails", true );
	SXMPUtils::RemoveProperties ( xmpMeta, kXMP_NS_XMP, "PageInfo", true );
	SXMPUtils::RemoveProperties ( xmpMeta, "http://ns.adobe.com/xap/1.0/t/pg/", 0, true );
	SXMPUtils::RemoveProperties ( xmpMeta, "http://ns.adobe.com/xap/1.0/mm/", 0, true );
	#if 1
		SXMPUtils::RemoveProperties ( xmpMeta, "http://ns.adobe.com/camera-raw-settings/1.0/", 0, true );
		SXMPUtils::RemoveProperties ( xmpMeta, "http://ns.adobe.com/tiff/1.0/", 0, true );
		SXMPUtils::RemoveProperties ( xmpMeta, "http://ns.adobe.com/exif/1.0/", 0, true );
		SXMPUtils::RemoveProperties ( xmpMeta, "http://ns.adobe.com/exif/1.0/aux/", 0, true );
		SXMPUtils::RemoveProperties ( xmpMeta, "http://ns.adobe.com/photoshop/1.0/", 0, true );
		SXMPUtils::RemoveProperties ( xmpMeta, "http://ns.adobe.com/pdf/1.3/", 0, true );
	#endif

}	// OpenTestFile

// -------------------------------------------------------------------------------------------------

#ifndef OnlyReadOnly
	#define OnlyReadOnly 0
#endif

static void TestOneFile ( const char * fileName )
{
	char buffer [1000];
	SXMPMeta  xmpMeta;	
	SXMPFiles xmpFile;
	XMP_PacketInfo xmpPacket;
	std::string    roDump, rwDump;
	
	sprintf ( buffer, "Testing %s", fileName );
	WriteMinorLabel ( sLogFile, buffer );
	
	OpenTestFile ( fileName, kXMPFiles_OpenForRead, &xmpMeta, &xmpFile );
	xmpMeta.DumpObject ( DumpToString, &roDump );
	xmpFile.CloseFile();

	if ( OnlyReadOnly ) {
		fprintf ( sLogFile, "Initial XMP from %s\n", fileName );
		xmpMeta.DumpObject ( DumpToFile, sLogFile );
		return;
	}
	
	OpenTestFile ( fileName, kXMPFiles_OpenForUpdate, &xmpMeta, &xmpFile );
	xmpMeta.DumpObject ( DumpToString, &rwDump );
	if ( roDump != rwDump ) {
		fprintf ( sLogFile, "** Initial read-only and update XMP don't match! **\n\n" );
		fprintf ( sLogFile, "Read-only XMP\n%s\nUpdate XMP\n%s\n", roDump.c_str(), rwDump.c_str() );
	}

	fprintf ( sLogFile, "Initial XMP from %s\n", fileName );
	xmpMeta.DumpObject ( DumpToFile, sLogFile );
	
	XMP_DateTime now;
	SXMPUtils::CurrentDateTime ( &now );
	std::string nowStr;
	SXMPUtils::ConvertFromDate ( now, &nowStr );
	if ( xmpMeta.CountArrayItems ( kXMP_NS_XMP, "XMPFileStamps" ) >= 9 ) {
		xmpMeta.DeleteProperty ( kXMP_NS_XMP, "XMPFileStamps" );
	}
	xmpMeta.AppendArrayItem ( kXMP_NS_XMP, "XMPFileStamps", kXMP_PropArrayIsOrdered, "" );
	xmpMeta.SetProperty ( kXMP_NS_XMP, "XMPFileStamps[last()]", nowStr.c_str() );
	
	nowStr.insert ( 0, "Updating dc:description at " );
	xmpMeta.SetLocalizedText ( kXMP_NS_DC, "description", "", "x-default", nowStr.c_str() );

	xmpFile.PutXMP ( xmpMeta );
	XMP_OptionBits closeOptions = 0;
	XMP_OptionBits capabilities = 0;
	XMP_FileFormat fileFormat = 0;
	xmpFile.GetFileInfo( NULL, NULL, &fileFormat, NULL);
	SXMPFiles::GetFormatInfo( fileFormat, &capabilities);

	if ( (xmpMeta.CountArrayItems ( kXMP_NS_XMP, "XMPFileStamps" ) & 1) == 0
			&& ( capabilities & kXMPFiles_AllowsSafeUpdate ) ) 
		closeOptions |= kXMPFiles_UpdateSafely;

	xmpFile.CloseFile ( closeOptions );

	fprintf ( sLogFile, "\n" );
	OpenTestFile ( fileName, kXMPFiles_OpenForRead, &xmpMeta, &xmpFile );
	fprintf ( sLogFile, "Modified XMP from %s\n", fileName );
	xmpMeta.DumpObject ( DumpToFile, sLogFile );
	xmpFile.CloseFile();

	fprintf ( sLogFile, "\nDone testing %s\n", fileName );

}	// TestOneFile

// -------------------------------------------------------------------------------------------------

extern "C" int main ( int argc, const char * argv[] )
{
	int result = 0;

	char   logName[256];
	int    nameLen = (int) strlen ( argv[0] );
	if ( (nameLen >= 4) && (strcmp ( argv[0]+nameLen-4, ".exe" ) == 0) ) nameLen -= 4;
	memcpy ( logName, argv[0], nameLen );
	memcpy ( &logName[nameLen], "Log.txt", 8 );	// Include final null.
	sLogFile = fopen ( logName, "wb" );

	time_t now = time(0);
	fprintf ( sLogFile, "XMPFilesCoverage starting %s", ctime(&now) );

	XMP_VersionInfo coreVersion, filesVersion;
	SXMPMeta::GetVersionInfo ( &coreVersion );
	SXMPFiles::GetVersionInfo ( &filesVersion );
	fprintf ( sLogFile, "Version :\n   %s\n   %s\n", coreVersion.message, filesVersion.message );

	try {

		if ( ! SXMPMeta::Initialize() ) {
			fprintf ( sLogFile, "## XMPMeta::Initialize failed!\n" );
			return -1;
		}
		XMP_OptionBits options = 0;
		#if UNIX_ENV
			options |= kXMPFiles_ServerMode;
		#endif
		if ( ! SXMPFiles::Initialize ( options ) ) {
			fprintf ( sLogFile, "## SXMPFiles::Initialize failed!\n" );
			return -1;
		}
		
		DumpHandlerInfo();
	
		TestOneFile ( "../../../../testfiles/BlueSquare.ai" );
		TestOneFile ( "../../../../testfiles/BlueSquare.eps" );
		TestOneFile ( "../../../../testfiles/BlueSquare.indd" );
		TestOneFile ( "../../../../testfiles/BlueSquare.jpg" );
		TestOneFile ( "../../../../testfiles/BlueSquare.pdf" );
		TestOneFile ( "../../../../testfiles/BlueSquare.psd" );
		TestOneFile ( "../../../../testfiles/BlueSquare.tif" );
		TestOneFile ( "../../../../testfiles/BlueSquare.avi" );
		TestOneFile ( "../../../../testfiles/BlueSquare.mov" );
		TestOneFile ( "../../../../testfiles/BlueSquare.mp3" );
		TestOneFile ( "../../../../testfiles/BlueSquare.wav" );
		TestOneFile ( "../../../../testfiles/BlueSquare.png" );

	} catch ( XMP_Error & excep ) {

		fprintf ( sLogFile, "\nCaught XMP_Error %d : %s\n", excep.GetID(), excep.GetErrMsg() );
		result = -2;

	} catch ( ... ) {

		fprintf ( sLogFile, "## Caught unknown exception\n" );
		result = -3;

	}

	SXMPFiles::Terminate();
	SXMPMeta::Terminate();

	now = time(0);
	fprintf ( sLogFile, "\nXMPFilesCoverage finished %s", ctime(&now) );
	fprintf ( sLogFile,	"Final status = %d\n", result );
	fclose ( sLogFile );
	
	printf( "\nresults have been logged into %s\n", logName );
	
	return result;

}	// main
