// =================================================================================================
//
// A thorough test for UTF-16 and UTF-32 serialization and parsing. It assumes the basic Unicode
// conversion functions are working - they have their own exhaustive test.
//
// =================================================================================================

#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>

#include <cstdlib>
#include <cerrno>
#include <stdexcept>
#include <cassert>

#define TXMP_STRING_TYPE	std::string
#include "XMP.hpp"
#include "XMP.incl_cpp"

#include "source/EndianUtils.hpp"
#include "source/UnicodeConversions.hpp"
#include "source/UnicodeConversions.cpp"

//#define ENABLE_XMP_CPP_INTERFACE 1;

using namespace std;

#if WIN_ENV
	#pragma warning ( disable : 4701 )	// local variable may be used without having been initialized
#endif

// =================================================================================================

#define IncludeUTF32 0	// *** UTF-32 parsing isn't working at the moment, Expat seems to not handle it.

#define kCodePointCount 0x110000

UTF8Unit  sU8  [kCodePointCount*4 + 8];
UTF16Unit sU16 [kCodePointCount*2 + 4];
UTF32Unit sU32 [kCodePointCount + 2];

static FILE * sLogFile;

static const char * kNS1 = "ns:test1/";

static const char * kSimpleRDF =
	"<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>"
	"  <rdf:Description rdf:about='Test:kSimpleRDF/' xmlns:ns1='ns:test1/' xmlns:ns2='ns:test2/'>"
	""
	"    <ns1:SimpleProp>Simple value</ns1:SimpleProp>"
	""
	"    <ns1:ArrayProp>"
	"      <rdf:Bag>"
	"        <rdf:li>Item1 value</rdf:li>"
	"        <rdf:li>Item2 value</rdf:li>"
	"      </rdf:Bag>"
	"    </ns1:ArrayProp>"
	""
	"    <ns1:StructProp rdf:parseType='Resource'>"
	"      <ns2:Field1>Field1 value</ns2:Field1>"
	"      <ns2:Field2>Field2 value</ns2:Field2>"
	"    </ns1:StructProp>"
	""
	"    <ns1:QualProp rdf:parseType='Resource'>"
	"      <rdf:value>Prop value</rdf:value>"
	"      <ns2:Qual>Qual value</ns2:Qual>"
	"    </ns1:QualProp>"
	""
	"    <ns1:AltTextProp>"
	"      <rdf:Alt>"
	"        <rdf:li xml:lang='x-one'>x-one value</rdf:li>"
	"        <rdf:li xml:lang='x-two'>x-two value</rdf:li>"
	"      </rdf:Alt>"
	"    </ns1:AltTextProp>"
	""
	"    <ns1:ArrayOfStructProp>"
	"      <rdf:Bag>"
	"        <rdf:li rdf:parseType='Resource'>"
	"          <ns2:Field1>Item-1</ns2:Field1>"
	"          <ns2:Field2>Field 1.2 value</ns2:Field2>"
	"        </rdf:li>"
	"        <rdf:li rdf:parseType='Resource'>"
	"          <ns2:Field1>Item-2</ns2:Field1>"
	"          <ns2:Field2>Field 2.2 value</ns2:Field2>"
	"        </rdf:li>"
	"      </rdf:Bag>"
	"    </ns1:ArrayOfStructProp>"
	""
	"  </rdf:Description>"
	"</rdf:RDF>";

// =================================================================================================

static XMP_Status DumpToString ( void * refCon, XMP_StringPtr outStr, XMP_StringLen outLen )
{
	std::string * dumpString = static_cast < std::string * > ( refCon );
	dumpString->append ( outStr, outLen );
	return 0;
}

// =================================================================================================

static XMP_Status DumpToFile ( void * refCon, XMP_StringPtr outStr, XMP_StringLen outLen )
{
	FILE * outFile	= static_cast < FILE * > ( refCon );
	fwrite ( outStr, 1, outLen, outFile );
	return 0;
}

// =================================================================================================

static void PrintXMPErrorInfo ( const XMP_Error & excep, const char * title )
{
	XMP_Int32 id = excep.GetID();
	const char * message = excep.GetErrMsg();
	fprintf ( sLogFile, "%s\n", title );
	fprintf ( sLogFile, "   #%d : %s\n", id, message );
}

// =================================================================================================

static void FullUnicodeParse ( FILE * log, const char * encoding, size_t bufferSize,
                               const std::string & packet, const std::string & fullUnicode )
{
	if ( bufferSize > sizeof(sU32) ) {
		fprintf ( log, "#ERROR: FullUnicodeParse buffer overrun for %s, %d byte buffers\n", encoding, bufferSize );
		return;
	}

	SXMPMeta meta;
	try {
		memset ( sU32, -1, sizeof(sU32) );
		for ( size_t i = 0; i < packet.size(); i += bufferSize ) {
			size_t count = bufferSize;
			if ( count > (packet.size() - i) ) count = packet.size() - i;
			memcpy ( sU32, &packet[i], count );
			meta.ParseFromBuffer ( XMP_StringPtr(sU32), count, kXMP_ParseMoreBuffers );
		}
		meta.ParseFromBuffer ( XMP_StringPtr(sU32), 0 );
	} catch ( XMP_Error& excep ) {
		char message [200];
		sprintf ( message, "#ERROR: Full Unicode parsing error for %s, %d byte buffers", encoding, bufferSize );
		PrintXMPErrorInfo ( excep, message );
		return;
	}
	
	std::string value;
	bool found = meta.GetProperty ( kNS1, "FullUnicode", &value, 0 );
	if ( (! found) || (value != fullUnicode) ) fprintf ( log, "#ERROR: Failed to get full Unicode value for %s, %d byte buffers\n", encoding, bufferSize );
	
}	// FullUnicodeParse

// =================================================================================================

static void DoTest ( FILE * log )
{
	SXMPMeta meta;
	size_t u8Count, u32Count;
	SXMPMeta meta8, meta16b, meta16l, meta32b, meta32l;
	std::string u8Packet, u16bPacket, u16lPacket, u32bPacket, u32lPacket;

	InitializeUnicodeConversions();

	// ---------------------------------------------------------------------------------------------

	fprintf ( log, "// ------------------------------------------------\n" );
	fprintf ( log, "// Test basic serialization and parsing using ASCII\n\n" );
	
	// ----------------------------------------------------
	// Create basic ASCII packets in each of the encodings.
	
	meta.ParseFromBuffer ( kSimpleRDF, kXMP_UseNullTermination );
	
	meta.SerializeToBuffer ( &u8Packet,   (kXMP_OmitPacketWrapper | kXMP_EncodeUTF8) );
	meta.SerializeToBuffer ( &u16bPacket, (kXMP_OmitPacketWrapper | kXMP_EncodeUTF16Big) );
	meta.SerializeToBuffer ( &u16lPacket, (kXMP_OmitPacketWrapper | kXMP_EncodeUTF16Little) );
	meta.SerializeToBuffer ( &u32bPacket, (kXMP_OmitPacketWrapper | kXMP_EncodeUTF32Big) );
	meta.SerializeToBuffer ( &u32lPacket, (kXMP_OmitPacketWrapper | kXMP_EncodeUTF32Little) );
	
	#if 0
		FILE* dump;
		dump = fopen ( "u8Packet.txt", "w" );
		fwrite ( u8Packet.c_str(), 1, u8Packet.size(), dump );
		fclose ( dump );
		dump = fopen ( "u16bPacket.txt", "w" );
		fwrite ( u16bPacket.c_str(), 1, u16bPacket.size(), dump );
		fclose ( dump );
		dump = fopen ( "u16lPacket.txt", "w" );
		fwrite ( u16lPacket.c_str(), 1, u16lPacket.size(), dump );
		fclose ( dump );
		dump = fopen ( "u32bPacket.txt", "w" );
		fwrite ( u32bPacket.c_str(), 1, u32bPacket.size(), dump );
		fclose ( dump );
		dump = fopen ( "u32lPacket.txt", "w" );
		fwrite ( u32lPacket.c_str(), 1, u32lPacket.size(), dump );
		fclose ( dump );
	#endif
	
	// Verify the character form. The conversion functions are tested separately.
	
	const char * ptr;
	
	ptr = u8Packet.c_str();
	fprintf ( log, "UTF-8    : %d : %.2X %.2X  \"%.10s...\"\n", u8Packet.size(), *ptr, *(ptr+1), ptr );
	
	ptr = u16bPacket.c_str();
	fprintf ( log, "UTF-16BE : %d : %.2X %.2X %.2X\n", u16bPacket.size(), *ptr, *(ptr+1), *(ptr+2) );
	ptr = u16lPacket.c_str();
	fprintf ( log, "UTF-16LE : %d : %.2X %.2X %.2X\n", u16lPacket.size(), *ptr, *(ptr+1), *(ptr+2) );
	
	ptr = u32bPacket.c_str();
	fprintf ( log, "UTF-32BE : %d : %.2X %.2X %.2X %.2X %.2X\n", u32bPacket.size(), *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4) );
	ptr = u32lPacket.c_str();
	fprintf ( log, "UTF-32LE : %d : %.2X %.2X %.2X %.2X %.2X\n", u32lPacket.size(), *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4) );
	
	fprintf ( log, "\nBasic serialization tests done\n" );
	
	// -------------------------------------------------
	// Verify round trip reparsing of the basic packets.
	
	std::string origDump, rtDump;
	
	 meta.DumpObject ( DumpToString, &origDump );
	fprintf ( log, "Original dump\n%s\n", origDump.c_str() );

	try {
		meta8.ParseFromBuffer   ( u8Packet.c_str(), u8Packet.size() );
		meta16b.ParseFromBuffer ( u16bPacket.c_str(), u16bPacket.size() );
		meta16l.ParseFromBuffer ( u16lPacket.c_str(), u16lPacket.size() );
		meta32b.ParseFromBuffer ( u32bPacket.c_str(), u32bPacket.size() );
		meta32l.ParseFromBuffer ( u32lPacket.c_str(), u32lPacket.size() );
	} catch ( XMP_Error& excep ) {
		PrintXMPErrorInfo ( excep, "## Caught reparsing exception" );
		fprintf ( log, "\n" );
	}
	
	#if 0
		fprintf ( log, "After UTF-8 roundtrip\n" );
		meta8.DumpObject ( DumpToFile, log );
		fprintf ( log, "\nAfter UTF-16 BE roundtrip\n" );
		meta16b.DumpObject ( DumpToFile, log );
		fprintf ( log, "\nAfter UTF-16 LE roundtrip\n" );
		meta16l.DumpObject ( DumpToFile, log );
		fprintf ( log, "\nAfter UTF-32 BE roundtrip\n" );
		meta32b.DumpObject ( DumpToFile, log );
		fprintf ( log, "\nAfter UTF-32 LE roundtrip\n" );
		meta32l.DumpObject ( DumpToFile, log );
	#endif
	
	rtDump.clear();
	meta8.DumpObject ( DumpToString, &rtDump );
	if ( rtDump != origDump ) fprintf ( log, "#ERROR: Roundtrip failure for UTF-8\n%s\n", rtDump.c_str() );

	rtDump.clear();
	meta16b.DumpObject ( DumpToString, &rtDump );
	if ( rtDump != origDump ) fprintf ( log, "#ERROR: Roundtrip failure for UTF-16BE\n%s\n", rtDump.c_str() );

	rtDump.clear();
	meta16l.DumpObject ( DumpToString, &rtDump );
	if ( rtDump != origDump ) fprintf ( log, "#ERROR: Roundtrip failure for UTF-16LE\n%s\n", rtDump.c_str() );

	#if IncludeUTF32

		rtDump.clear();
		meta32b.DumpObject ( DumpToString, &rtDump );
		if ( rtDump != origDump ) fprintf ( log, "#ERROR: Roundtrip failure for UTF-32BE\n%s\n", rtDump.c_str() );

		rtDump.clear();
		meta32l.DumpObject ( DumpToString, &rtDump );
		if ( rtDump != origDump ) fprintf ( log, "#ERROR: Roundtrip failure for UTF-32LE\n%s\n", rtDump.c_str() );

	#endif
	
	fprintf ( log, "Basic round-trip parsing tests done\n\n" );

	// ---------------------------------------------------------------------------------------------
	
	fprintf ( log, "// --------------------------------------------------\n" );
	fprintf ( log, "// Test parse buffering logic using full Unicode data\n\n" );

	// --------------------------------------------------------------------------------------------
	// Construct the packets to parse in all encodings. There is just one property with a value
	// containing all of the Unicode representations. This isn't all of the Unicode characters, but
	// is more than enough to establish correctness of the buffering logic. It is almost everything
	// in the BMP, plus the range U+100000..U+10FFFF beyond the BMP. Doing all Unicode characters
	// takes far to long to execute and does not provide additional confidence. Skip ASCII controls,
	// they are not allowed in XML and get changed to spaces by SetProperty. Skip U+FFFE and U+FFFF,
	// the expat parser rejects them.
	
	#define kTab	0x09
	#define kLF		0x0A
	#define kCR		0x0D
	
	size_t i;
	UTF32Unit cp;
	sU32[0] = kTab; sU32[1] = kLF; sU32[2] = kCR;
	for ( i = 3, cp = 0x20; cp < 0x7F; ++i, ++cp ) sU32[i] = cp;
	for ( cp = 0x80; cp < 0xD800; ++i, ++cp ) sU32[i] = cp;
	for ( cp = 0xE000; cp < 0xFFFE; ++i, ++cp ) sU32[i] = cp;
	for ( cp = 0x100000; cp < 0x110000; ++i, ++cp ) sU32[i] = cp;
	u32Count = i;
	assert ( u32Count == (3 + (0x7F-0x20) + (0xD800-0x80) + (0xFFFE - 0xE000) + (0x110000-0x100000)) );

	if ( kBigEndianHost ) {
		UTF32BE_to_UTF8 ( sU32, u32Count, sU8, sizeof(sU8), &i, &u8Count );
	} else {
		UTF32LE_to_UTF8 ( sU32, u32Count, sU8, sizeof(sU8), &i, &u8Count );
	}
	if ( i != u32Count ) fprintf ( log, "#ERROR: Failed to convert full UTF-32 buffer\n" );
	assert ( u8Count == (3 + (0x7F-0x20) + 2*(0x800-0x80) + 3*(0xD800-0x800) + 3*(0xFFFE - 0xE000) + 4*(0x110000-0x100000)) );
	sU8[u8Count] = 0;
	
	std::string fullUnicode;
	SXMPUtils::RemoveProperties ( &meta, "", "", kXMPUI_DoAllProperties );
	meta.SetProperty ( kNS1, "FullUnicode", XMP_StringPtr(sU8) );
	meta.GetProperty ( kNS1, "FullUnicode", &fullUnicode, 0 );
	if ( (fullUnicode.size() != u8Count) || (fullUnicode != XMP_StringPtr(sU8)) ) {
		fprintf ( log, "#ERROR: Failed to set full UTF-8 value\n" );
		if ( (fullUnicode.size() != u8Count) ) {
			fprintf ( log, "        Size mismatch, want %d, got %d\n", u8Count, fullUnicode.size() );
		} else {
			for ( size_t b = 0; b < u8Count; ++b ) {
				if ( fullUnicode[b] != sU8[b] ) fprintf ( log, "        Byte mismatch at %d\n", b );
			}
		}
	}
	
	u8Packet.clear();
	u16bPacket.clear();
	u16lPacket.clear();
	u32bPacket.clear();
	u32lPacket.clear();
	
	meta.SerializeToBuffer ( &u8Packet,   (kXMP_OmitPacketWrapper | kXMP_EncodeUTF8) );
	meta.SerializeToBuffer ( &u16bPacket, (kXMP_OmitPacketWrapper | kXMP_EncodeUTF16Big) );
	meta.SerializeToBuffer ( &u16lPacket, (kXMP_OmitPacketWrapper | kXMP_EncodeUTF16Little) );
	#if IncludeUTF32
		meta.SerializeToBuffer ( &u32bPacket, (kXMP_OmitPacketWrapper | kXMP_EncodeUTF32Big) );
		meta.SerializeToBuffer ( &u32lPacket, (kXMP_OmitPacketWrapper | kXMP_EncodeUTF32Little) );
	#endif
	
	// ---------------------------------------------------------------------
	// Parse the whole packet as a sanity check, then at a variety of sizes.

	FullUnicodeParse ( log, "UTF-8", u8Packet.size(), u8Packet, fullUnicode );
	FullUnicodeParse ( log, "UTF-16BE", u16bPacket.size(), u16bPacket, fullUnicode );
	FullUnicodeParse ( log, "UTF-16LE", u16lPacket.size(), u16lPacket, fullUnicode );
	#if IncludeUTF32
		FullUnicodeParse ( log, "UTF-32BE", u32bPacket.size(), u32bPacket, fullUnicode );
		FullUnicodeParse ( log, "UTF-32LE", u32lPacket.size(), u32lPacket, fullUnicode );
	#endif
	fprintf ( log, "Full packet, no BOM, buffered parsing tests done\n" );

#if 0	// Skip the partial buffer tests, there seem to be problems, but no client uses partial buffers.

	for ( i = 1; i <= 3; ++i ) {
		FullUnicodeParse ( log, "UTF-8", i, u8Packet, fullUnicode );
		FullUnicodeParse ( log, "UTF-16BE", i, u16bPacket, fullUnicode );
		FullUnicodeParse ( log, "UTF-16LE", i, u16lPacket, fullUnicode );
		#if IncludeUTF32
			FullUnicodeParse ( log, "UTF-32BE", i, u32bPacket, fullUnicode );
			FullUnicodeParse ( log, "UTF-32LE", i, u32lPacket, fullUnicode );
		#endif
		fprintf ( log, "%d byte buffers, no BOM, buffered parsing tests done\n", i );
	}
	
	for ( i = 4; i <= 16; i *= 2 ) {
		FullUnicodeParse ( log, "UTF-8", i, u8Packet, fullUnicode );
		FullUnicodeParse ( log, "UTF-16BE", i, u16bPacket, fullUnicode );
		FullUnicodeParse ( log, "UTF-16LE", i, u16lPacket, fullUnicode );
		#if IncludeUTF32
			FullUnicodeParse ( log, "UTF-32BE", i, u32bPacket, fullUnicode );
			FullUnicodeParse ( log, "UTF-32LE", i, u32lPacket, fullUnicode );
		#endif
		fprintf ( log, "%d byte buffers, no BOM, buffered parsing tests done\n", i );
	}

#endif

	fprintf ( log, "\n" );	

	// -----------------------------------------------------------------------
	// Redo the buffered parsing tests, now with a leading BOM in the packets.

	u8Packet.insert ( 0, "\xEF\xBB\xBF", 3 );
	
	UTF32Unit NatBOM  = 0x0000FEFF;
	UTF32Unit SwapBOM = 0xFFFE0000;
	
	if ( kBigEndianHost ) {
		u16bPacket.insert ( 0, XMP_StringPtr(&NatBOM)+2, 2 );
		u16lPacket.insert ( 0, XMP_StringPtr(&SwapBOM), 2 );
		u32bPacket.insert ( 0, XMP_StringPtr(&NatBOM), 4 );
		u32lPacket.insert ( 0, XMP_StringPtr(&SwapBOM), 4 );
	} else {
		u16lPacket.insert ( 0, XMP_StringPtr(&NatBOM), 2 );
		u16bPacket.insert ( 0, XMP_StringPtr(&SwapBOM)+2, 2 );
		u32lPacket.insert ( 0, XMP_StringPtr(&NatBOM), 4 );
		u32bPacket.insert ( 0, XMP_StringPtr(&SwapBOM), 4 );
	}
	
	FullUnicodeParse ( log, "UTF-8", u8Packet.size(), u8Packet, fullUnicode );
	FullUnicodeParse ( log, "UTF-16BE", u16bPacket.size(), u16bPacket, fullUnicode );
	FullUnicodeParse ( log, "UTF-16LE", u16lPacket.size(), u16lPacket, fullUnicode );
	#if IncludeUTF32
		FullUnicodeParse ( log, "UTF-32BE", u32bPacket.size(), u32bPacket, fullUnicode );
		FullUnicodeParse ( log, "UTF-32LE", u32lPacket.size(), u32lPacket, fullUnicode );
	#endif
	fprintf ( log, "Full packet, leading BOM, buffered parsing tests done\n" );
		
#if 0	// Skip the partial buffer tests, there seem to be problems, but no client uses partial buffers.

	for ( i = 1; i <= 3; ++i ) {
		FullUnicodeParse ( log, "UTF-8", i, u8Packet, fullUnicode );
		FullUnicodeParse ( log, "UTF-16BE", i, u16bPacket, fullUnicode );
		FullUnicodeParse ( log, "UTF-16LE", i, u16lPacket, fullUnicode );
		#if IncludeUTF32
			FullUnicodeParse ( log, "UTF-32BE", i, u32bPacket, fullUnicode );
			FullUnicodeParse ( log, "UTF-32LE", i, u32lPacket, fullUnicode );
		#endif
		fprintf ( log, "%d byte buffers, leading BOM, buffered parsing tests done\n", i );
	}
	
	for ( i = 4; i <= 16; i *= 2 ) {
		FullUnicodeParse ( log, "UTF-8", i, u8Packet, fullUnicode );
		FullUnicodeParse ( log, "UTF-16BE", i, u16bPacket, fullUnicode );
		FullUnicodeParse ( log, "UTF-16LE", i, u16lPacket, fullUnicode );
		#if IncludeUTF32
			FullUnicodeParse ( log, "UTF-32BE", i, u32bPacket, fullUnicode );
			FullUnicodeParse ( log, "UTF-32LE", i, u32lPacket, fullUnicode );
		#endif
		fprintf ( log, "%d byte buffers, leading BOM, buffered parsing tests done\n", i );
	}

#endif

	fprintf ( log, "\n" );	

}	// DoTest

// =================================================================================================

extern "C" int main ( void )
{
	int  result = 0;
	char buffer [1000];
	
	sLogFile = stdout;
	
	time_t now;
	time ( &now );
	sprintf ( buffer, "// Starting test for UTF-16 and UTF-32 serialization and parsing, %s", ctime ( &now ) );

	fprintf ( sLogFile, "// " );
	for ( int i = 4; i < strlen(buffer); ++i ) fprintf ( sLogFile, "=" );
	fprintf ( sLogFile, "\n%s", buffer );
	
	fprintf ( sLogFile, "// =====================================================================================\n" );
	fprintf ( sLogFile, "// A thorough test for UTF-16 and UTF-32 serialization and parsing. It assumes the basic\n" );
	fprintf ( sLogFile, "// Unicode conversion functions are working - they have their own exhaustive test.\n\n" );
	
	#if ! IncludeUTF32
		fprintf ( sLogFile, "// ** Skipping UTF-32 tests, Expat seems to not handle it.\n\n" );
	#endif
	
	#if 0
	if ( sLogFile == stdout ) {
		// Use this to be able to move the app window away from debugger windows.
		fprintf ( sLogFile, "Move window, type return to continue" );
		fread ( buffer, 1, 1, stdin );
	}
	#endif

	try {
		
		if ( ! SXMPMeta::Initialize() ) {
			fprintf ( sLogFile, "\n## SXMPMeta::Initialize failed!\n" );
			return -1;
		}
		
		DoTest ( sLogFile );
		
		SXMPMeta::Terminate();

	} catch ( XMP_Error& excep ) {
	
		PrintXMPErrorInfo ( excep, "\n## Unhandled XMP_Error exception" );

	} catch ( ... ) {

		fprintf ( sLogFile, "\n## Unexpected exception\n" );
		return -1;

	}

	time ( &now );
	sprintf ( buffer, "// Finished test for UTF-16 and UTF-32 serialization and parsing, %s", ctime ( &now ) );

	fprintf ( sLogFile, "// " );
	for ( int i = 4; i < strlen(buffer); ++i ) fprintf ( sLogFile, "=" );
	fprintf ( sLogFile, "\n%s\n", buffer );

	fclose ( sLogFile );
	return 0;

}
