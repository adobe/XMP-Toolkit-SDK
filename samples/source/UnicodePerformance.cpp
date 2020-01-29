// =================================================================================================

#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <ctime>

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <stdexcept>

//#define ENABLE_XMP_CPP_INTERFACE 1;

using namespace std;

#if WIN_ENV
	#pragma warning ( disable : 4701 )	// local variable may be used without having been initialized
#endif

// =================================================================================================

#include "public/include/XMP_Environment.h"
#include "public/include/XMP_Const.h"

#include "source/EndianUtils.hpp"
#include "source/UnicodeConversions.hpp"
#include "source/UnicodeConversions.cpp"

#define TestUnicodeConsortium	0

#if TestUnicodeConsortium
	#include "ConvertUTF.c"	// The Unicode Consortium implementations.
#endif

// =================================================================================================

#define kCodePointCount 0x110000

UTF8Unit  sU8  [kCodePointCount*4 + 8];
UTF16Unit sU16 [kCodePointCount*2 + 4];
UTF32Unit sU32 [kCodePointCount + 2];

// =================================================================================================

static UTF8_to_UTF16_Proc  OurUTF8_to_UTF16;	// ! Don't use static initialization, VS.Net strips it!
static UTF8_to_UTF32_Proc  OurUTF8_to_UTF32;
static UTF16_to_UTF8_Proc  OurUTF16_to_UTF8;
static UTF16_to_UTF32_Proc OurUTF16_to_UTF32;
static UTF32_to_UTF8_Proc  OurUTF32_to_UTF8;
static UTF32_to_UTF16_Proc OurUTF32_to_UTF16;

// =================================================================================================

static void ReportPerformance ( FILE * log, const char * content, const size_t u32Count, const size_t u16Count, const size_t u8Count )
{
	size_t inCount, outCount;
	UTF32Unit * u32Ptr;
	UTF16Unit * u16Ptr;
	UTF8Unit *  u8Ptr;
	
	size_t i;
	const size_t cycles = 100;
	clock_t start, end;
	double elapsed;

	// --------------------------------------------------
	fprintf ( log, "\n  Adobe code over %s\n", content );
	
	start = clock();
	for ( i = 0; i < cycles; ++i ) OurUTF32_to_UTF8 ( sU32, u32Count, sU8, sizeof(sU8), &inCount, &outCount );
	end = clock();
	elapsed = double(end-start) / CLOCKS_PER_SEC;

	fprintf ( log, "    UTF32_to_UTF8  : %.3f seconds\n", elapsed );
	if ( (inCount != u32Count) || (outCount != u8Count) ) fprintf ( log, "    *** Our UTF32_to_UTF8 count error, %d -> %d\n", inCount, outCount );

	start = clock();
	for ( i = 0; i < cycles; ++i ) OurUTF32_to_UTF16 ( sU32, u32Count, sU16, sizeof(sU16), &inCount, &outCount );
	end = clock();
	elapsed = double(end-start) / CLOCKS_PER_SEC;

	fprintf ( log, "    UTF32_to_UTF16 : %.3f seconds\n", elapsed );
	if ( (inCount != u32Count) || (outCount != u16Count) ) fprintf ( log, "    *** Our UTF32_to_UTF16 count error, %d -> %d\n", inCount, outCount );

	start = clock();
	for ( i = 0; i < cycles; ++i ) OurUTF16_to_UTF8 ( sU16, u16Count, sU8, sizeof(sU8), &inCount, &outCount );
	end = clock();
	elapsed = double(end-start) / CLOCKS_PER_SEC;

	fprintf ( log, "    UTF16_to_UTF8  : %.3f seconds\n", elapsed );
	if ( (inCount != u16Count) || (outCount != u8Count) ) fprintf ( log, "    *** Our UTF16_to_UTF8 count error, %d -> %d\n", inCount, outCount );

	start = clock();
	for ( i = 0; i < cycles; ++i ) OurUTF16_to_UTF32 ( sU16, u16Count, sU32, sizeof(sU32), &inCount, &outCount );
	end = clock();
	elapsed = double(end-start) / CLOCKS_PER_SEC;

	fprintf ( log, "    UTF16_to_UTF32 : %.3f seconds\n", elapsed );
	if ( (inCount != u16Count) || (outCount != u32Count) ) fprintf ( log, "    *** Our UTF16_to_UTF32 count error, %d -> %d\n", inCount, outCount );

	start = clock();
	for ( i = 0; i < cycles; ++i ) OurUTF8_to_UTF16 ( sU8, u8Count, sU16, sizeof(sU16), &inCount, &outCount );
	end = clock();
	elapsed = double(end-start) / CLOCKS_PER_SEC;

	fprintf ( log, "    UTF8_to_UTF16  : %.3f seconds\n", elapsed );
	if ( (inCount != u8Count) || (outCount != u16Count) ) fprintf ( log, "    *** Our UTF8_to_UTF16 count error, %d -> %d\n", inCount, outCount );

	start = clock();
	for ( i = 0; i < cycles; ++i ) OurUTF8_to_UTF32 ( sU8, u8Count, sU32, sizeof(sU32), &inCount, &outCount );
	end = clock();
	elapsed = double(end-start) / CLOCKS_PER_SEC;

	fprintf ( log, "    UTF8_to_UTF32  : %.3f seconds\n", elapsed );
	if ( (inCount != u8Count) || (outCount != u32Count) ) fprintf ( log, "    *** Our UTF8_to_UTF32 count error, %d -> %d\n", inCount, outCount );

	#if TestUnicodeConsortium
	
		// ---------------------------------------------------------------
		fprintf ( log, "\n  Unicode Consortium code over %s\n", content );

		ConversionResult ucStatus;
		
		start = clock();
		for ( i = 0; i < cycles; ++i ) {
			u32Ptr = sU32; u8Ptr = sU8;
			ucStatus = ConvertUTF32toUTF8 ( (const UTF32**)(&u32Ptr), (const UTF32*)(sU32+u32Count), &u8Ptr, sU8+sizeof(sU8), strictConversion );
		}
		end = clock();
		elapsed = double(end-start) / CLOCKS_PER_SEC;
	
		fprintf ( log, "    UTF32_to_UTF8  : %.3f seconds\n", elapsed );
		inCount = u32Ptr - sU32; outCount = u8Ptr - sU8;
		if ( ucStatus != conversionOK ) fprintf ( log, "  *** UC ConvertUTF32toUTF8 status error, %d\n", ucStatus );
		if ( (inCount != u32Count) || (outCount != u8Count) ) fprintf ( log, "  *** UC ConvertUTF32toUTF8 count error, %d, %d -> %d\n", inCount, outCount );
	
		start = clock();
		for ( i = 0; i < cycles; ++i ) {
			u32Ptr = sU32; u16Ptr = sU16;
			ucStatus = ConvertUTF32toUTF16 ( (const UTF32**)(&u32Ptr), (const UTF32*)(sU32+u32Count), &u16Ptr, sU16+sizeof(sU16), strictConversion );
		}
		end = clock();
		elapsed = double(end-start) / CLOCKS_PER_SEC;
	
		fprintf ( log, "    UTF32_to_UTF16 : %.3f seconds\n", elapsed );
		inCount = u32Ptr - sU32; outCount = u16Ptr - sU16;
		if ( ucStatus != conversionOK ) fprintf ( log, "  *** UC ConvertUTF32toUTF16 status error, %d\n", ucStatus );
		if ( (inCount != u32Count) || (outCount != u16Count) ) fprintf ( log, "  *** UC ConvertUTF32toUTF16 count error, %d, %d -> %d\n", inCount, outCount );
	
		start = clock();
		for ( i = 0; i < cycles; ++i ) {
			u16Ptr = sU16; u8Ptr = sU8;
			ucStatus = ConvertUTF16toUTF8 ( (const UTF16**)(&u16Ptr), (const UTF16*)(sU16+u16Count), &u8Ptr, sU8+sizeof(sU8), strictConversion );
		}
		end = clock();
		elapsed = double(end-start) / CLOCKS_PER_SEC;
	
		fprintf ( log, "    UTF16_to_UTF8  : %.3f seconds\n", elapsed );
		inCount = u16Ptr - sU16; outCount = u8Ptr - sU8;
		if ( ucStatus != conversionOK ) fprintf ( log, "  *** UC ConvertUTF16toUTF8 status error, %d\n", ucStatus );
		if ( (inCount != u16Count) || (outCount != u8Count) ) fprintf ( log, "  *** UC ConvertUTF16toUTF8 count error, %d, %d -> %d\n", inCount, outCount );
	
		start = clock();
		for ( i = 0; i < cycles; ++i ) {
			u16Ptr = sU16; u32Ptr = sU32;
			ucStatus = ConvertUTF16toUTF32 ( (const UTF16**)(&u16Ptr), (const UTF16*)(sU16+u16Count), &u32Ptr, sU32+sizeof(sU32), strictConversion );
		}
		end = clock();
		elapsed = double(end-start) / CLOCKS_PER_SEC;
	
		fprintf ( log, "    UTF16_to_UTF32 : %.3f seconds\n", elapsed );
		inCount = u16Ptr - sU16; outCount = u32Ptr - sU32;
		if ( ucStatus != conversionOK ) fprintf ( log, "  *** UC ConvertUTF16toUTF32 status error, %d\n", ucStatus );
		if ( (inCount != u16Count) || (outCount != u32Count) ) fprintf ( log, "  *** UC ConvertUTF16toUTF32 count error, %d, %d -> %d\n", inCount, outCount );
	
		start = clock();
		for ( i = 0; i < cycles; ++i ) {
			u8Ptr = sU8; u16Ptr = sU16;
			ucStatus = ConvertUTF8toUTF16 ( (const UTF8**)(&u8Ptr), (const UTF8*)(sU8+u8Count), &u16Ptr, sU16+sizeof(sU16), strictConversion );
		}
		end = clock();
		elapsed = double(end-start) / CLOCKS_PER_SEC;
	
		fprintf ( log, "    UTF8_to_UTF16  : %.3f seconds\n", elapsed );
		inCount = u8Ptr - sU8; outCount = u16Ptr - sU16;
		if ( ucStatus != conversionOK ) fprintf ( log, "  *** UC ConvertUTF8toUTF16 status error, %d\n", ucStatus );
		if ( (inCount != u8Count) || (outCount != u16Count) ) fprintf ( log, "  *** UC ConvertUTF8toUTF16 count error, %d, %d -> %d\n", inCount, outCount );
	
		start = clock();
		for ( i = 0; i < cycles; ++i ) {
			u8Ptr = sU8; u32Ptr = sU32;
			ucStatus = ConvertUTF8toUTF32 ( (const UTF8**)(&u8Ptr), (const UTF8*)(sU8+u8Count), &u32Ptr, sU32+sizeof(sU32), strictConversion );
		}
		end = clock();
		elapsed = double(end-start) / CLOCKS_PER_SEC;
	
		fprintf ( log, "    UTF8_to_UTF32  : %.3f seconds\n", elapsed );
		inCount = u8Ptr - sU8; outCount = u32Ptr - sU32;
		if ( ucStatus != conversionOK ) fprintf ( log, "  *** UC ConvertUTF8toUTF32 status error, %d\n", ucStatus );
		if ( (inCount != u8Count) || (outCount != u32Count) ) fprintf ( log, "  *** UC ConvertUTF8toUTF32 count error, %d, %d -> %d\n", inCount, outCount );
	
	#endif

}	// ReportPerformance

// =================================================================================================

static void ComparePerformance ( FILE * log )
{
	size_t i, u32Count, u16Count, u8Count;
	UTF32Unit cp;

	if ( kBigEndianHost ) {
		OurUTF8_to_UTF16  = UTF8_to_UTF16BE;
		OurUTF8_to_UTF32  = UTF8_to_UTF32BE;
		OurUTF16_to_UTF8  = UTF16BE_to_UTF8;
		OurUTF16_to_UTF32 = UTF16BE_to_UTF32BE;
		OurUTF32_to_UTF8  = UTF32BE_to_UTF8;
		OurUTF32_to_UTF16 = UTF32BE_to_UTF16BE;
	} else {
		OurUTF8_to_UTF16  = UTF8_to_UTF16LE;
		OurUTF8_to_UTF32  = UTF8_to_UTF32LE;
		OurUTF16_to_UTF8  = UTF16LE_to_UTF8;
		OurUTF16_to_UTF32 = UTF16LE_to_UTF32LE;
		OurUTF32_to_UTF8  = UTF32LE_to_UTF8;
		OurUTF32_to_UTF16 = UTF32LE_to_UTF16LE;
	}
	
	for ( i = 0, cp = 0; cp < 0xD800; ++i, ++cp ) sU32[i] = cp;	// Measure using the full Unicode set.
	for ( cp = 0xE000; cp < 0x110000; ++i, ++cp ) sU32[i] = cp;
	u32Count = 0xD800 + (0x110000 - 0xE000);
	u16Count = 0xD800 + (0x10000 - 0xE000) + (0x110000 - 0x10000)*2;
	u8Count  = 0x80 + (0x800 - 0x80)*2 + (0xD800 - 0x800)*3 + (0x10000 - 0xE000)*3 + (0x110000 - 0x10000)*4;
	ReportPerformance ( log, "full Unicode set", u32Count, u16Count, u8Count );
	
	for ( i = 0; i < 0x110000; ++i ) sU32[i] = i & 0x7F;	// Measure using just ASCII.
	u32Count = 0x110000;
	u16Count = 0x110000;
	u8Count  = 0x110000;
	ReportPerformance ( log, "just ASCII", u32Count, u16Count, u8Count );
	
	for ( i = 0; i < 0x110000; ++i ) sU32[i] = 0x4000 + (i & 0x7FFF);	// Measure using just non-ASCII inside the BMP.
	u32Count = 0x110000;
	u16Count = 0x110000;
	u8Count  = 0x110000*3;
	ReportPerformance ( log, "just non-ASCII inside the BMP", u32Count, u16Count, u8Count );
	
	for ( i = 0; i < 0x110000; ++i ) sU32[i] = 0x40000 + (i & 0xFFFF);	// Measure using just outside the BMP.
	u32Count = 0x110000;
	u16Count = 0x110000*2;
	u8Count  = 0x110000*4;
	ReportPerformance ( log, "just outside the BMP", u32Count, u16Count, u8Count );
	
}	// ComparePerformance

// =================================================================================================

static void DoTest ( FILE * log )
{

	InitializeUnicodeConversions();
	ComparePerformance ( log );

}	// DoTest

// =================================================================================================

extern "C" int main ( void )
{
	char buffer [1000];
	
	#if !XMP_AutomatedTestBuild
		FILE * log = stdout;
	#else
		FILE * log = fopen ( "TestUnicode.out", "wb" );
	#endif
	
	time_t now;
	time ( &now );
	sprintf ( buffer, "// Starting test for Unicode conversion performance, %s", ctime ( &now ) );

	fprintf ( log, "// " );
	for ( size_t i = 4; i < strlen(buffer); ++i ) fprintf ( log, "=" );
	fprintf ( log, "\n%s", buffer );
	fprintf ( log, "// Native %s endian\n", (kBigEndianHost ? "big" : "little") );

	try {
		
		DoTest ( log );

	} catch ( ... ) {

		fprintf ( log, "\n## Caught unexpected exception\n" );
		return -1;

	}

	time ( &now );
	sprintf ( buffer, "// Finished test for Unicode conversion performance, %s", ctime ( &now ) );

	fprintf ( log, "\n// " );
	for ( size_t i = 4; i < strlen(buffer); ++i ) fprintf ( log, "=" );
	fprintf ( log, "\n%s\n", buffer );

	fclose ( log );
	return 0;

}
