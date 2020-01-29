// =================================================================================================

#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <ctime>

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

// =================================================================================================

#define kCodePointCount 0x110000

UTF8Unit  sU8  [kCodePointCount*4 + 8];
UTF16Unit sU16 [kCodePointCount*2 + 4];
UTF32Unit sU32 [kCodePointCount + 2];

// =================================================================================================

static UTF16Unit NativeUTF16BE ( UTF16Unit value )
{
	if ( ! kBigEndianHost ) SwapUTF16 ( &value, &value, 1 );
	return value;
}

static UTF16Unit NativeUTF16LE ( UTF16Unit value )
{
	if ( kBigEndianHost ) SwapUTF16 ( &value, &value, 1 );
	return value;
}

static UTF32Unit NativeUTF32BE ( UTF32Unit value )
{
	if ( ! kBigEndianHost ) SwapUTF32 ( &value, &value, 1 );
	return value;
}

static UTF32Unit NativeUTF32LE ( UTF32Unit value )
{
	if ( kBigEndianHost ) SwapUTF32 ( &value, &value, 1 );
	return value;
}

// =================================================================================================

static void Bad_CodePoint_to_UTF8 ( FILE * log, UTF32Unit cp )
{
	UTF8Unit u8[8];
	size_t   len;

	try {
		CodePoint_to_UTF8 ( cp, u8, sizeof(u8), &len );		
		fprintf ( log, "  *** CodePoint_to_UTF8 failure, no exception for 0x%X\n", cp );
	} catch ( ... ) {
		// Do nothing, the exception is expected.
	}

}

// =================================================================================================

static void Bad_CodePoint_to_UTF16BE ( FILE * log, UTF32Unit cp )
{
	UTF16Unit u16[4];
	size_t    len;

	try {
		CodePoint_to_UTF16BE ( cp, u16, sizeof(u16), &len );		
		fprintf ( log, "  *** CodePoint_to_UTF16BE failure, no exception for 0x%X\n", cp );
	} catch ( ... ) {
		// Do nothing, the exception is expected.
	}

}

// =================================================================================================

static void Bad_CodePoint_to_UTF16LE ( FILE * log, UTF32Unit cp )
{
	UTF16Unit u16[4];
	size_t    len;

	try {
		CodePoint_to_UTF16LE ( cp, u16, sizeof(u16), &len );		
		fprintf ( log, "  *** CodePoint_to_UTF16LE failure, no exception for 0x%X\n", cp );
	} catch ( ... ) {
		// Do nothing, the exception is expected.
	}

}

// =================================================================================================

static void Bad_CodePoint_from_UTF8 ( FILE * log, const char * inU8, const char * message )
{
	UTF32Unit cp;
	size_t    len;
	
	try {
		CodePoint_from_UTF8 ( (UTF8Unit*)inU8, strlen(inU8), &cp, &len );
		fprintf ( log, "  *** CodePoint_from_UTF8 failure, no exception for %s\n", message );
	} catch ( ... ) {
		// Do nothing, the exception is expected.
	}

}

// =================================================================================================

static void Bad_CodePoint_from_UTF16BE ( FILE * log, const UTF16Unit * inU16, const size_t inLen, const char * message )
{
	UTF32Unit cp;
	size_t    outLen;
	
	try {
		CodePoint_from_UTF16BE ( inU16, inLen, &cp, &outLen );
		fprintf ( log, "  *** CodePoint_from_UTF16BE failure, no exception for %s\n", message );
	} catch ( ... ) {
		// Do nothing, the exception is expected.
	}

}

// =================================================================================================

static void Bad_CodePoint_from_UTF16LE ( FILE * log, const UTF16Unit * inU16, const size_t inLen, const char * message )
{
	UTF32Unit cp;
	size_t    outLen;
	
	try {
		CodePoint_from_UTF16LE ( inU16, inLen, &cp, &outLen );
		fprintf ( log, "  *** CodePoint_from_UTF16LE failure, no exception for %s\n", message );
	} catch ( ... ) {
		// Do nothing, the exception is expected.
	}

}

// =================================================================================================

static void Test_SwappingPrimitives ( FILE * log )
{
	UTF16Unit u16[8];
	UTF32Unit u32[8];
	UTF32Unit i;
	
	fprintf ( log, "\nTesting byte swapping primitives\n" );
	
	u16[0] = 0x1122;
	if ( UTF16InSwap(&u16[0]) == 0x2211 ) printf ( "  UTF16InSwap OK\n" );
	
	u32[0] = 0x11223344;
	if ( UTF32InSwap(&u32[0]) == 0x44332211 ) printf ( "  UTF32InSwap OK\n" );
	
	UTF16OutSwap ( &u16[0], 0x1122 );
	if ( u16[0] == 0x2211 ) printf ( "  UTF16OutSwap OK\n" );
	
	UTF32OutSwap ( &u32[0], 0x11223344 );
	if ( u32[0] == 0x44332211 ) printf ( "  UTF32OutSwap OK\n" );
	
	for ( i = 0; i < 8; ++i ) u16[i] = 0x1100 | UTF16Unit(i);
	SwapUTF16 ( u16, u16, 8 );
	for ( i = 0; i < 8; ++i ) {
		if ( u16[i] != ((UTF16Unit(i) << 8) | 0x11) ) break;
	}
	if ( i == 8 ) printf ( "  SwapUTF16 OK\n" );
	
	for ( i = 0; i < 8; ++i ) u32[i] = 0x11223300 | i;
	SwapUTF32 ( u32, u32, 8 );
	for ( i = 0; i < 8; ++i ) {
		if ( u32[i] != ((i << 24) | 0x00332211) ) break;
	}
	if ( i == 8 ) printf ( "  SwapUTF32 OK\n" );

}	// Test_SwappingPrimitives

// =================================================================================================

static void Test_CodePoint_to_UTF8 ( FILE * log )
{
	size_t    len, lenx;
	UTF32Unit cp, cp0, cpx;
	UTF8Unit  u8[8];

	// -------------------------------------
	// Test CodePoint_to_UTF8 on good input.
	
	fprintf ( log, "\nTesting CodePoint_to_UTF8 on good input\n" );
	
	// Test ASCII, 00..7F.
	cp0 = 0;
	for ( cp = cp0; cp < 0x80; ++cp ) {
		CodePoint_to_UTF8 ( cp, u8, 0, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 1, &len );
		CodePoint_from_UTF8 ( u8, len, &cpx, &lenx );
		if ( (len != 1) || (cp != cpx) || (lenx != 1) ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 2, &len );
		CodePoint_from_UTF8 ( u8, len, &cpx, &lenx );
		if ( (len != 1) || (cp != cpx) || (lenx != 1) ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
	}
	fprintf ( log, "  CodePoint_to_UTF8 done for %.4X..%.4X\n", cp0, cpx );
	
	// Test 2 byte values, 0080..07FF : 110x xxxx 10xx xxxx
	cp0 = cpx+1;
	for ( cp = cp0; cp < 0x800; ++cp ) {
		CodePoint_to_UTF8 ( cp, u8, 0, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 1, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 2, &len );
		CodePoint_from_UTF8 ( u8, len, &cpx, &lenx );
		if ( (len != 2) || (cp != cpx) || (lenx != 2) ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 3, &len );
		CodePoint_from_UTF8 ( u8, len, &cpx, &lenx );
		if ( (len != 2) || (cp != cpx) || (lenx != 2) ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
	}
	fprintf ( log, "  CodePoint_to_UTF8 done for %.4X..%.4X\n", cp0, cpx );
	
	// Test 3 byte values, 0800..D7FF : 1110 xxxx 10xx xxxx 10xx xxxx
	cp0 = cpx+1;
	for ( cp = cp0; cp < 0xD800; ++cp ) {
		CodePoint_to_UTF8 ( cp, u8, 0, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 1, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 2, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 3, &len );
		CodePoint_from_UTF8 ( u8, len, &cpx, &lenx );
		if ( (len != 3) || (cp != cpx) || (lenx != 3) ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 4, &len );
		CodePoint_from_UTF8 ( u8, len, &cpx, &lenx );
		if ( (len != 3) || (cp != cpx) || (lenx != 3) ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
	}
	fprintf ( log, "  CodePoint_to_UTF8 done for %.4X..%.4X\n", cp0, cpx );
	
	// Test 3 byte values, E000..FFFF : 1110 xxxx 10xx xxxx 10xx xxxx
	cp0 = 0xE000;
	for ( cp = cp0; cp < 0x10000; ++cp ) {
		CodePoint_to_UTF8 ( cp, u8, 0, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 1, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 2, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 3, &len );
		CodePoint_from_UTF8 ( u8, len, &cpx, &lenx );
		if ( (len != 3) || (cp != cpx) || (lenx != 3) ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 4, &len );
		CodePoint_from_UTF8 ( u8, len, &cpx, &lenx );
		if ( (len != 3) || (cp != cpx) || (lenx != 3) ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
	}
	fprintf ( log, "  CodePoint_to_UTF8 done for %.4X..%.4X\n", cp0, cpx );
	
	// Test 4 byte values, 10000..10FFFF : 1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx
	cp0 = cpx+1;
	for ( cp = cp0; cp < 0x110000; ++cp ) {
		CodePoint_to_UTF8 ( cp, u8, 0, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 1, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 2, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 3, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 4, &len );
		CodePoint_from_UTF8 ( u8, len, &cpx, &lenx );
		if ( (len != 4) || (cp != cpx) || (lenx != 4) ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
		CodePoint_to_UTF8 ( cp, u8, 5, &len );
		CodePoint_from_UTF8 ( u8, len, &cpx, &lenx );
		if ( (len != 4) || (cp != cpx) || (lenx != 4) ) fprintf ( log, "  *** CodePoint_to_UTF8 failure for U+%.4X\n", cp );
	}
	fprintf ( log, "  CodePoint_to_UTF8 done for %.4X..%.4X\n", cp0, cpx );
		
	// --------------------------------------
	// Test CodePoint_to_UTF8 with bad input.
	
	fprintf ( log, "\nTesting CodePoint_to_UTF8 with bad input\n" );
	
	Bad_CodePoint_to_UTF8 ( log, 0x110000 );	// Code points beyond the defined range.
	Bad_CodePoint_to_UTF8 ( log, 0x123456 );
	Bad_CodePoint_to_UTF8 ( log, 0xFFFFFFFF );
	Bad_CodePoint_to_UTF8 ( log, 0xD800 );		// Surrogate code points.
	Bad_CodePoint_to_UTF8 ( log, 0xDC00 );
	Bad_CodePoint_to_UTF8 ( log, 0xDFFF );

	fprintf ( log, "  CodePoint_to_UTF8 done with bad input\n" );

}	// Test_CodePoint_to_UTF8

// =================================================================================================

static void Test_CodePoint_from_UTF8 ( FILE * log )
{
	UTF32Unit i, j, k, l;
	size_t    len;
	UTF32Unit cp, cp0, cpx;
	UTF8Unit  u8[5];
		
	// ---------------------------------------
	// Test CodePoint_from_UTF8 on good input.
	
	fprintf ( log, "\nTesting CodePoint_from_UTF8 on good input\n" );
	
	// Test ASCII, 00..7F.
	cp0 = 0;
	for ( i = 0; i < 0x80; ++i ) {
		u8[0] = UTF8Unit(i); u8[1] = 0xFF; cpx = i;
		CodePoint_from_UTF8 ( u8, 0, &cp, &len );
		if ( len != 0 ) fprintf ( log, "CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
		CodePoint_from_UTF8 ( u8, 1, &cp, &len );
		if ( (cp != cpx) || (len != 1) ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
		CodePoint_from_UTF8 ( u8, 2, &cp, &len );
		if ( (cp != cpx) || (len != 1) ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
	}
	fprintf ( log, "  CodePoint_from_UTF8 done for %.4X..%.4X\n", cp0, cpx );
	
	// Test 2 byte values, 0080..07FF : 110x xxxx 10xx xxxx
	cp0 = cpx+1;
	for ( i = 0; i < 0x20; ++i ) {
		for ( j = 0; j < 0x40; ++j ) {
			cpx = (i<<6) + j; if ( cpx < cp0 ) continue;
			u8[0] = 0xC0+UTF8Unit(i); u8[1] = 0x80+UTF8Unit(j); u8[2] = 0xFF;
			CodePoint_from_UTF8 ( u8, 0, &cp, &len );
			if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
			CodePoint_from_UTF8 ( u8, 1, &cp, &len );
			if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
			CodePoint_from_UTF8 ( u8, 2, &cp, &len );
			if ( (cp != cpx) || (len != 2) ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
			CodePoint_from_UTF8 ( u8, 3, &cp, &len );
			if ( (cp != cpx) || (len != 2) ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
		}
	}
	fprintf ( log, "  CodePoint_from_UTF8 done for %.4X..%.4X\n", cp0, cpx );
	
	// Test 3 byte values, 0800..D7FF : 1110 xxxx 10xx xxxx 10xx xxxx
	cp0 = cpx+1;
	for ( i = 0; i < 0x10; ++i ) {
		for ( j = 0; j < 0x40; ++j ) {
			for ( k = 0; k < 0x40; ++k ) {
				cpx = (i<<12) + (j<<6) + k; if ( cpx < cp0 ) continue;
				u8[0] = 0xE0+UTF8Unit(i); u8[1] = 0x80+UTF8Unit(j); u8[2] = 0x80+UTF8Unit(k); u8[3] = 0xFF;
				CodePoint_from_UTF8 ( u8, 0, &cp, &len );
				if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
				CodePoint_from_UTF8 ( u8, 1, &cp, &len );
				if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
				CodePoint_from_UTF8 ( u8, 2, &cp, &len );
				if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
				CodePoint_from_UTF8 ( u8, 3, &cp, &len );
				if ( (cp != cpx) || (len != 3) ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
				CodePoint_from_UTF8 ( u8, 4, &cp, &len );
				if ( (cp != cpx) || (len != 3) ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
			}
			if ( cpx == 0xD7FF ) break;
		}
		if ( cpx == 0xD7FF ) break;
	}
	fprintf ( log, "  CodePoint_from_UTF8 done for %.4X..%.4X\n", cp0, cpx );
	
	// Test 3 byte values, E000..FFFF : 1110 xxxx 10xx xxxx 10xx xxxx
	cp0 = 0xE000;
	for ( i = 0; i < 0x10; ++i ) {
		for ( j = 0; j < 0x40; ++j ) {
			for ( k = 0; k < 0x40; ++k ) {
				cpx = (i<<12) + (j<<6) + k; if ( cpx < cp0 ) continue;
				u8[0] = 0xE0+UTF8Unit(i); u8[1] = 0x80+UTF8Unit(j); u8[2] = 0x80+UTF8Unit(k); u8[3] = 0xFF;
				CodePoint_from_UTF8 ( u8, 0, &cp, &len );
				if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
				CodePoint_from_UTF8 ( u8, 1, &cp, &len );
				if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
				CodePoint_from_UTF8 ( u8, 2, &cp, &len );
				if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
				CodePoint_from_UTF8 ( u8, 3, &cp, &len );
				if ( (cp != cpx) || (len != 3) ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
				CodePoint_from_UTF8 ( u8, 4, &cp, &len );
				if ( (cp != cpx) || (len != 3) ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
			}
		}
	}
	fprintf ( log, "  CodePoint_from_UTF8 done for %.4X..%.4X\n", cp0, cpx );
	
	// Test 4 byte values, 10000..10FFFF : 1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx
	cp0 = cpx+1;
	for ( i = 0; i < 0x7; ++i ) {
		for ( j = 0; j < 0x40; ++j ) {
			for ( k = 0; k < 0x40; ++k ) {
				for ( l = 0; l < 0x40; ++l ) {
					cpx = (i<<18) + (j<<12) + (k<<6) + l; if ( cpx < cp0 ) continue;
					u8[0] = 0xF0+UTF8Unit(i); u8[1] = 0x80+UTF8Unit(j); u8[2] = 0x80+UTF8Unit(k); u8[3] = 0x80+UTF8Unit(l); u8[4] = 0xFF;
					CodePoint_from_UTF8 ( u8, 0, &cp, &len );
					if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
					CodePoint_from_UTF8 ( u8, 1, &cp, &len );
					if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
					CodePoint_from_UTF8 ( u8, 2, &cp, &len );
					if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
					CodePoint_from_UTF8 ( u8, 3, &cp, &len );
					if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
					CodePoint_from_UTF8 ( u8, 4, &cp, &len );
					if ( (cp != cpx) || (len != 4) ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
					CodePoint_from_UTF8 ( u8, 5, &cp, &len );
					if ( (cp != cpx) || (len != 4) ) fprintf ( log, "  *** CodePoint_from_UTF8 failure for U+%.4X\n", cpx );
				}
				if ( cpx == 0x10FFFF ) break;
			}
			if ( cpx == 0x10FFFF ) break;
		}
		if ( cpx == 0x10FFFF ) break;
	}
	fprintf ( log, "  CodePoint_from_UTF8 done for %.4X..%.4X\n", cp0, cpx );
		
	// ----------------------------------------
	// Test CodePoint_from_UTF8 with bad input.
	
	fprintf ( log, "\nTesting CodePoint_from_UTF8 with bad input\n" );
	
	Bad_CodePoint_from_UTF8 ( log, "\x88\x20", "bad leading byte count" );	// One byte "sequence".
	Bad_CodePoint_from_UTF8 ( log, "\xF9\x90\x80\x80\x80\x20", "bad leading byte count" );	// Five byte sequence.
	Bad_CodePoint_from_UTF8 ( log, "\xFE\x90\x80\x80\x80\x80\x80\x20", "bad leading byte count" );	// Seven byte sequence.
	Bad_CodePoint_from_UTF8 ( log, "\xFF\x90\x80\x80\x80\x80\x80\x80\x20", "bad leading byte count" );	// Eight byte sequence.
	
	Bad_CodePoint_from_UTF8 ( log, "\xF1\x80\x01\x80\x20", "bad following high bits" );	// 00xx xxxx
	Bad_CodePoint_from_UTF8 ( log, "\xF1\x80\x40\x80\x20", "bad following high bits" );	// 01xx xxxx
	Bad_CodePoint_from_UTF8 ( log, "\xF1\x80\xC0\x80\x20", "bad following high bits" );	// 11xx xxxx
	
	Bad_CodePoint_from_UTF8 ( log, "\xF4\x90\x80\x80\x20", "out of range code point" );	// U+110000
	Bad_CodePoint_from_UTF8 ( log, "\xF7\xBF\xBF\xBF\x20", "out of range code point" );	// U+1FFFFF

	Bad_CodePoint_from_UTF8 ( log, "\xED\xA0\x80\x20", "surrogate code point" );	// U+D800
	Bad_CodePoint_from_UTF8 ( log, "\xED\xB0\x80\x20", "surrogate code point" );	// U+DC00
	Bad_CodePoint_from_UTF8 ( log, "\xED\xBF\xBF\x20", "surrogate code point" );	// U+DFFF
	
	fprintf ( log, "  CodePoint_from_UTF8 done with bad input\n" );

}	// Test_CodePoint_from_UTF8

// =================================================================================================

static void Test_CodePoint_to_UTF16 ( FILE * log )
{
	size_t    len, lenx;
	UTF32Unit cp, cp0, cpx;
	UTF16Unit u16[3];

	// ----------------------------------------
	// Test CodePoint_to_UTF16BE on good input.
	
	fprintf ( log, "\nTesting CodePoint_to_UTF16BE on good input\n" );
	
	// Some explicit sanity tests, in case the code and exhaustive tests have inverse bugs.
	if ( kBigEndianHost ) {
		CodePoint_to_UTF16BE ( 0x1234, u16, 1, &len );
		if ( (len != 1) || (u16[0] != 0x1234) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+1234\n" );
		CodePoint_to_UTF16BE ( 0xFEDC, u16, 1, &len );
		if ( (len != 1) || (u16[0] != 0xFEDC) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+FEDC\n" );
		CodePoint_to_UTF16BE ( 0x14834, u16, 2, &len );
		if ( (len != 2) || (u16[0] != 0xD812) || (u16[1] != 0xDC34) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+14834\n" );
	} else {
		CodePoint_to_UTF16BE ( 0x1234, u16, 1, &len );
		if ( (len != 1) || (u16[0] != 0x3412) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+1234\n" );
		CodePoint_to_UTF16BE ( 0xFEDC, u16, 1, &len );
		if ( (len != 1) || (u16[0] != 0xDCFE) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+FEDC\n" );
		CodePoint_to_UTF16BE ( 0x14834, u16, 2, &len );
		if ( (len != 2) || (u16[0] != 0x12D8) || (u16[1] != 0x34DC) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+14834\n" );
	}
	fprintf ( log, "  CodePoint_to_UTF16BE sanity tests done\n" );
	
	// Test the low part of the BMP, 0000..D7FF.
	cp0 = 0;
	for ( cp = cp0; cp < 0xD800; ++cp ) {
		CodePoint_to_UTF16BE ( cp, u16, 0, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
		CodePoint_to_UTF16BE ( cp, u16, 1, &len );
		if ( (len != 1) || (NativeUTF16BE(u16[0]) != cp) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
		CodePoint_from_UTF16BE ( u16, len, &cpx, &lenx );
		if ( (cp != cpx) || (lenx != 1) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
		CodePoint_to_UTF16BE ( cp, u16, 2, &len );
		if ( (len != 1) || (NativeUTF16BE(u16[0]) != cp) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
		CodePoint_from_UTF16BE ( u16, len, &cpx, &lenx );
		if ( (cp != cpx) || (lenx != 1) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
	}
	fprintf ( log, "  CodePoint_to_UTF16BE done for %.4X..%.4X\n", cp0, cpx );
	
	// Test the high part of the BMP, E000..FFFF.
	cp0 = 0xE000;
	for ( cp = cp0; cp < 0x10000; ++cp ) {
		CodePoint_to_UTF16BE ( cp, u16, 0, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
		CodePoint_to_UTF16BE ( cp, u16, 1, &len );
		if ( (len != 1) || (NativeUTF16BE(u16[0]) != cp) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
		CodePoint_from_UTF16BE ( u16, len, &cpx, &lenx );
		if ( (cp != cpx) || (lenx != 1) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
		CodePoint_to_UTF16BE ( cp, u16, 2, &len );
		if ( (len != 1) || (NativeUTF16BE(u16[0]) != cp) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
		CodePoint_from_UTF16BE ( u16, len, &cpx, &lenx );
		if ( (cp != cpx) || (lenx != 1) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
	}
	fprintf ( log, "  CodePoint_to_UTF16BE done for %.4X..%.4X\n", cp0, cpx );
	
	// Test beyond the BMP, 10000..10FFFF.
	cp0 = 0x10000;
	for ( cp = cp0; cp < 0x110000; ++cp ) {
		CodePoint_to_UTF16BE ( cp, u16, 0, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
		CodePoint_to_UTF16BE ( cp, u16, 1, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
		CodePoint_to_UTF16BE ( cp, u16, 2, &len );
		if ( (len != 2) ||
		     (NativeUTF16BE(u16[0]) != (0xD800 | ((cp-0x10000) >> 10))) ||
		     (NativeUTF16BE(u16[1]) != (0xDC00 | ((cp-0x10000) & 0x3FF))) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
		CodePoint_from_UTF16BE ( u16, len, &cpx, &lenx );
		if ( (cp != cpx) || (lenx != 2) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
		CodePoint_to_UTF16BE ( cp, u16, 3, &len );
		if ( (len != 2) ||
		     (NativeUTF16BE(u16[0]) != (0xD800 | ((cp-0x10000) >> 10))) ||
		     (NativeUTF16BE(u16[1]) != (0xDC00 | ((cp-0x10000) & 0x3FF))) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
		CodePoint_from_UTF16BE ( u16, len, &cpx, &lenx );
		if ( (cp != cpx) || (lenx != 2) ) fprintf ( log, "  *** CodePoint_to_UTF16BE failure for U+%.4X\n", cp );
	}
	fprintf ( log, "  CodePoint_to_UTF16BE done for %.4X..%.4X\n", cp0, cpx );

	// ----------------------------------------
	// Test CodePoint_to_UTF16LE on good input.
	
	fprintf ( log, "\nTesting CodePoint_to_UTF16LE on good input\n" );
	
	// Some explicit sanity tests, in case the code and exhaustive tests have inverse bugs.
	if ( kBigEndianHost ) {
		CodePoint_to_UTF16LE ( 0x1234, u16, 1, &len );
		if ( (len != 1) || (u16[0] != 0x3412) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+1234\n" );
		CodePoint_to_UTF16LE ( 0xFEDC, u16, 1, &len );
		if ( (len != 1) || (u16[0] != 0xDCFE) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+FEDC\n" );
		CodePoint_to_UTF16LE ( 0x14834, u16, 2, &len );
		if ( (len != 2) || (u16[0] != 0x12D8) || (u16[1] != 0x34DC) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+14834\n" );
	} else {
		CodePoint_to_UTF16LE ( 0x1234, u16, 1, &len );
		if ( (len != 1) || (u16[0] != 0x1234) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+1234\n" );
		CodePoint_to_UTF16LE ( 0xFEDC, u16, 1, &len );
		if ( (len != 1) || (u16[0] != 0xFEDC) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+FEDC\n" );
		CodePoint_to_UTF16LE ( 0x14834, u16, 2, &len );
		if ( (len != 2) || (u16[0] != 0xD812) || (u16[1] != 0xDC34) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+14834\n" );
	}
	fprintf ( log, "  CodePoint_to_UTF16LE sanity tests done\n" );
	
	// Test the low part of the BMP, 0000..D7FF.
	cp0 = 0;
	for ( cp = cp0; cp < 0xD800; ++cp ) {
		CodePoint_to_UTF16LE ( cp, u16, 0, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
		CodePoint_to_UTF16LE ( cp, u16, 1, &len );
		if ( (len != 1) || (NativeUTF16LE(u16[0]) != cp) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
		CodePoint_from_UTF16LE ( u16, len, &cpx, &lenx );
		if ( (cp != cpx) || (lenx != 1) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
		CodePoint_to_UTF16LE ( cp, u16, 2, &len );
		if ( (len != 1) || (NativeUTF16LE(u16[0]) != cp) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
		CodePoint_from_UTF16LE ( u16, len, &cpx, &lenx );
		if ( (cp != cpx) || (lenx != 1) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
	}
	fprintf ( log, "  CodePoint_to_UTF16LE done for %.4X..%.4X\n", cp0, cpx );
	
	// Test the high part of the BMP, E000..FFFF.
	cp0 = 0xE000;
	for ( cp = cp0; cp < 0x10000; ++cp ) {
		CodePoint_to_UTF16LE ( cp, u16, 0, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
		CodePoint_to_UTF16LE ( cp, u16, 1, &len );
		if ( (len != 1) || (NativeUTF16LE(u16[0]) != cp) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
		CodePoint_from_UTF16LE ( u16, len, &cpx, &lenx );
		if ( (cp != cpx) || (lenx != 1) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
		CodePoint_to_UTF16LE ( cp, u16, 2, &len );
		if ( (len != 1) || (NativeUTF16LE(u16[0]) != cp) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
		CodePoint_from_UTF16LE ( u16, len, &cpx, &lenx );
		if ( (cp != cpx) || (lenx != 1) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
	}
	fprintf ( log, "  CodePoint_to_UTF16LE done for %.4X..%.4X\n", cp0, cpx );
	
	// Test beyond the BMP, 10000..10FFFF.
	cp0 = 0x10000;
	for ( cp = cp0; cp < 0x110000; ++cp ) {
		CodePoint_to_UTF16LE ( cp, u16, 0, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
		CodePoint_to_UTF16LE ( cp, u16, 1, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
		CodePoint_to_UTF16LE ( cp, u16, 2, &len );
		if ( (len != 2) ||
		     (NativeUTF16LE(u16[0]) != (0xD800 | ((cp-0x10000) >> 10))) ||
		     (NativeUTF16LE(u16[1]) != (0xDC00 | ((cp-0x10000) & 0x3FF))) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
		CodePoint_from_UTF16LE ( u16, len, &cpx, &lenx );
		if ( (cp != cpx) || (lenx != 2) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
		CodePoint_to_UTF16LE ( cp, u16, 3, &len );
		if ( (len != 2) ||
		     (NativeUTF16LE(u16[0]) != (0xD800 | ((cp-0x10000) >> 10))) ||
		     (NativeUTF16LE(u16[1]) != (0xDC00 | ((cp-0x10000) & 0x3FF))) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
		CodePoint_from_UTF16LE ( u16, len, &cpx, &lenx );
		if ( (cp != cpx) || (lenx != 2) ) fprintf ( log, "  *** CodePoint_to_UTF16LE failure for U+%.4X\n", cp );
	}
	fprintf ( log, "  CodePoint_to_UTF16LE done for %.4X..%.4X\n", cp0, cpx );
		
	// ---------------------------------------
	// Test CodePoint_to_UTF16 with bad input.
	
	fprintf ( log, "\nTesting CodePoint_to_UTF16 with bad input\n" );
	
	Bad_CodePoint_to_UTF16BE ( log, 0x110000 );	// Code points beyond the defined range.
	Bad_CodePoint_to_UTF16BE ( log, 0x123456 );
	Bad_CodePoint_to_UTF16BE ( log, 0xFFFFFFFF );
	Bad_CodePoint_to_UTF16BE ( log, 0xD800 );	// Surrogate code points.
	Bad_CodePoint_to_UTF16BE ( log, 0xDC00 );
	Bad_CodePoint_to_UTF16BE ( log, 0xDFFF );

	fprintf ( log, "  CodePoint_to_UTF16BE done with bad input\n" );
	
	Bad_CodePoint_to_UTF16LE ( log, 0x110000 );	// Code points beyond the defined range.
	Bad_CodePoint_to_UTF16LE ( log, 0x123456 );
	Bad_CodePoint_to_UTF16LE ( log, 0xFFFFFFFF );
	Bad_CodePoint_to_UTF16LE ( log, 0xD800 );	// Surrogate code points.
	Bad_CodePoint_to_UTF16LE ( log, 0xDC00 );
	Bad_CodePoint_to_UTF16LE ( log, 0xDFFF );

	fprintf ( log, "  CodePoint_to_UTF16LE done with bad input\n" );

}	// Test_CodePoint_to_UTF16

// =================================================================================================

static void Test_CodePoint_from_UTF16 ( FILE * log )
{
	UTF32Unit i, j;
	size_t    len;
	UTF32Unit cp, cp0, cpx;
	UTF16Unit u16[3];

	// ------------------------------------------
	// Test CodePoint_from_UTF16BE on good input.
	
	fprintf ( log, "\nTesting CodePoint_from_UTF16BE on good input\n" );
	
	// Some explicit sanity tests, in case the code and exhaustive tests have inverse bugs.
	if ( kBigEndianHost ) {
		u16[0] = 0x1234;
		CodePoint_from_UTF16BE ( u16, 1, &cp, &len );
		if ( (len != 1) || (cp != 0x1234) ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+1234\n" );
		u16[0] = 0xFEDC;
		CodePoint_from_UTF16BE ( u16, 1, &cp, &len );
		if ( (len != 1) || (cp != 0xFEDC) ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+FEDC\n" );
		u16[0] = 0xD812; u16[1] = 0xDC34;
		CodePoint_from_UTF16BE ( u16, 2, &cp, &len );
		if ( (len != 2) || (cp != 0x14834) ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+14834\n" );
	} else {
		u16[0] = 0x3412;
		CodePoint_from_UTF16BE ( u16, 1, &cp, &len );
		if ( (len != 1) || (cp != 0x1234) ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+1234\n" );
		u16[0] = 0xDCFE;
		CodePoint_from_UTF16BE ( u16, 1, &cp, &len );
		if ( (len != 1) || (cp != 0xFEDC) ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+FEDC\n" );
		u16[0] = 0x12D8; u16[1] = 0x34DC;
		CodePoint_from_UTF16BE ( u16, 2, &cp, &len );
		if ( (len != 2) || (cp != 0x14834) ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+14834\n" );
	}
	fprintf ( log, "  CodePoint_from_UTF16BE sanity tests done\n" );
	
	// Test the low part of the BMP, 0000..D7FF.
	cp0 = 0;
	for ( i = 0; i < 0xD800; ++i ) {
		u16[0] = NativeUTF16BE(UTF16Unit(i)); u16[1] = 0xFFFF; cpx = i;
		CodePoint_from_UTF16BE ( u16, 0, &cp, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+%.4X\n", cpx );
		CodePoint_from_UTF16BE ( u16, 1, &cp, &len );
		if ( (cp != cpx) || (len != 1) ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+%.4X\n", cpx );
		CodePoint_from_UTF16BE ( u16, 2, &cp, &len );
		if ( (cp != cpx) || (len != 1) ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+%.4X\n", cpx );
	}
	fprintf ( log, "  CodePoint_from_UTF16BE done for %.4X..%.4X\n", cp0, cpx );
	
	// Test the high part of the BMP, E000..FFFF.
	cp0 = 0xE000;
	for ( i = cp0; i < 0x10000; ++i ) {
		u16[0] = NativeUTF16BE(UTF16Unit(i)); u16[1] = 0xFFFF; cpx = i;
		CodePoint_from_UTF16BE ( u16, 0, &cp, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+%.4X\n", cpx );
		CodePoint_from_UTF16BE ( u16, 1, &cp, &len );
		if ( (cp != cpx) || (len != 1) ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+%.4X\n", cpx );
		CodePoint_from_UTF16BE ( u16, 2, &cp, &len );
		if ( (cp != cpx) || (len != 1) ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+%.4X\n", cpx );
	}
	fprintf ( log, "  CodePoint_from_UTF16BE done for %.4X..%.4X\n", cp0, cpx );
	
	// Test beyond the BMP, 10000..10FFFF.
	cp0 = 0x10000;
	for ( i = 0; i < 0x400; ++i ) {
		for ( j = 0; j < 0x400; ++j ) {
			cpx = (i<<10) + j + cp0;
			u16[0] = NativeUTF16BE(0xD800+UTF16Unit(i)); u16[1] = NativeUTF16BE(0xDC00+UTF16Unit(j)); u16[2] = 0xFFFF;
			CodePoint_from_UTF16BE ( u16, 0, &cp, &len );
			if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+%.4X\n", cpx );
			CodePoint_from_UTF16BE ( u16, 1, &cp, &len );
			if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+%.4X\n", cpx );
			CodePoint_from_UTF16BE ( u16, 2, &cp, &len );
			if ( (cp != cpx) || (len != 2) ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+%.4X\n", cpx );
			CodePoint_from_UTF16BE ( u16, 3, &cp, &len );
			if ( (cp != cpx) || (len != 2) ) fprintf ( log, "  *** CodePoint_from_UTF16BE failure for U+%.4X\n", cpx );
		}
	}
	fprintf ( log, "  CodePoint_from_UTF16BE done for %.4X..%.4X\n", cp0, cpx );

	// ------------------------------------------
	// Test CodePoint_from_UTF16LE on good input.
	
	fprintf ( log, "\nTesting CodePoint_from_UTF16LE on good input\n" );
	
	// Some explicit sanity tests, in case the code and exhaustive tests have inverse bugs.
	if ( kBigEndianHost ) {
		u16[0] = 0x3412;
		CodePoint_from_UTF16LE ( u16, 1, &cp, &len );
		if ( (len != 1) || (cp != 0x1234) ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+1234\n" );
		u16[0] = 0xDCFE;
		CodePoint_from_UTF16LE ( u16, 1, &cp, &len );
		if ( (len != 1) || (cp != 0xFEDC) ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+FEDC\n" );
		u16[0] = 0x12D8; u16[1] = 0x34DC;
		CodePoint_from_UTF16LE ( u16, 2, &cp, &len );
		if ( (len != 2) || (cp != 0x14834) ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+14834\n" );
	} else {
		u16[0] = 0x1234;
		CodePoint_from_UTF16LE ( u16, 1, &cp, &len );
		if ( (len != 1) || (cp != 0x1234) ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+1234\n" );
		u16[0] = 0xFEDC;
		CodePoint_from_UTF16LE ( u16, 1, &cp, &len );
		if ( (len != 1) || (cp != 0xFEDC) ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+FEDC\n" );
		u16[0] = 0xD812; u16[1] = 0xDC34;
		CodePoint_from_UTF16LE ( u16, 2, &cp, &len );
		if ( (len != 2) || (cp != 0x14834) ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+14834\n" );
	}
	fprintf ( log, "  CodePoint_from_UTF16LE sanity tests done\n" );
	
	// Test the low part of the BMP, 0000..D7FF.
	cp0 = 0;
	for ( i = 0; i < 0xD800; ++i ) {
		u16[0] = NativeUTF16LE(UTF16Unit(i)); u16[1] = 0xFFFF; cpx = i;
		CodePoint_from_UTF16LE ( u16, 0, &cp, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+%.4X\n", cpx );
		CodePoint_from_UTF16LE ( u16, 1, &cp, &len );
		if ( (cp != cpx) || (len != 1) ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+%.4X\n", cpx );
		CodePoint_from_UTF16LE ( u16, 2, &cp, &len );
		if ( (cp != cpx) || (len != 1) ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+%.4X\n", cpx );
	}
	fprintf ( log, "  CodePoint_from_UTF16LE done for %.4X..%.4X\n", cp0, cpx );
	
	// Test the high part of the BMP, E000..FFFF.
	cp0 = 0xE000;
	for ( i = cp0; i < 0x10000; ++i ) {
		u16[0] = NativeUTF16LE(UTF16Unit(i)); u16[1] = 0xFFFF; cpx = i;
		CodePoint_from_UTF16LE ( u16, 0, &cp, &len );
		if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+%.4X\n", cpx );
		CodePoint_from_UTF16LE ( u16, 1, &cp, &len );
		if ( (cp != cpx) || (len != 1) ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+%.4X\n", cpx );
		CodePoint_from_UTF16LE ( u16, 2, &cp, &len );
		if ( (cp != cpx) || (len != 1) ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+%.4X\n", cpx );
	}
	fprintf ( log, "  CodePoint_from_UTF16LE done for %.4X..%.4X\n", cp0, cpx );
	
	// Test beyond the BMP, 10000..10FFFF.
	cp0 = 0x10000;
	for ( i = 0; i < 0x400; ++i ) {
		for ( j = 0; j < 0x400; ++j ) {
			cpx = (i<<10) + j + cp0;
			u16[0] = NativeUTF16LE(0xD800+UTF16Unit(i)); u16[1] = NativeUTF16LE(0xDC00+UTF16Unit(j)); u16[2] = 0xFFFF;
			CodePoint_from_UTF16LE ( u16, 0, &cp, &len );
			if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+%.4X\n", cpx );
			CodePoint_from_UTF16LE ( u16, 1, &cp, &len );
			if ( len != 0 ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+%.4X\n", cpx );
			CodePoint_from_UTF16LE ( u16, 2, &cp, &len );
			if ( (cp != cpx) || (len != 2) ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+%.4X\n", cpx );
			CodePoint_from_UTF16LE ( u16, 3, &cp, &len );
			if ( (cp != cpx) || (len != 2) ) fprintf ( log, "  *** CodePoint_from_UTF16LE failure for U+%.4X\n", cpx );
		}
	}
	fprintf ( log, "  CodePoint_from_UTF16LE done for %.4X..%.4X\n", cp0, cpx );
		
	// ---------------------------------------------------------------
	// Test CodePoint_from_UTF16 with bad input. U+12345 is D808 DF45.
	
	fprintf ( log, "\nTesting CodePoint_from_UTF16 with bad input\n" );
	
	memcpy ( sU16, "\xD8\x08\x00\x20\x00\x00", 6 );	// ! HPPA (maybe others) won't tolerate misaligned loads.
	Bad_CodePoint_from_UTF16BE ( log, sU16, 3, "missing low surrogate" );
	memcpy ( sU16, "\xDF\x45\x00\x20\x00\x00", 6 );
	Bad_CodePoint_from_UTF16BE ( log, sU16, 3, "leading low surrogate" );
	memcpy ( sU16, "\xD8\x08\xD8\x08\x00\x20\x00\x00", 8 );
	Bad_CodePoint_from_UTF16BE ( log, sU16, 4, "double high surrogate" );

	fprintf ( log, "  CodePoint_from_UTF16BE done with bad input\n" );
	
	memcpy ( sU16, "\x08\xD8\x20\x00\x00\x00", 6 );
	Bad_CodePoint_from_UTF16LE ( log, sU16, 3, "missing low surrogate" );
	memcpy ( sU16, "\x45\xDF\x20\x00\x00\x00", 6 );
	Bad_CodePoint_from_UTF16LE ( log, sU16, 3, "leading low surrogate" );
	memcpy ( sU16, "\x08\xD8\x08\xD8\x20\x00\x00\x00", 8 );
	Bad_CodePoint_from_UTF16LE ( log, sU16, 4, "double high surrogate" );

	fprintf ( log, "  CodePoint_from_UTF16LE done with bad input\n" );

}	// Test_CodePoint_from_UTF16

// =================================================================================================

static void Test_UTF8_to_UTF16 ( FILE * log )
{
	size_t i;
	size_t len8, len16, len8x, len16x;
	UTF32Unit cp, cpx, cpLo, cpHi;

	// ---------------------------------------------------------------------------------------
	// Test UTF8_to_UTF16BE on good input. The CodePoint to/from functions are already tested,
	// use them to verify the results here.
	
	fprintf ( log, "\nTesting UTF8_to_UTF16BE on good input\n" );

	// Test ASCII.

	cpLo = 0; cpHi = 0x80; len8 = len16 = 0x80;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU8[i] = UTF8Unit(cp);
	sU8[len8] = 0xFF;

	UTF8_to_UTF16BE ( sU8, len8, sU16, sizeof(sU16), &len8x, &len16x );
	if ( (len8 != len8x) || (len16 != len16x) ) fprintf ( log, "  *** UTF8_to_UTF16BE length failure, %d -> %d\n", len8x, len16x );

	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16BE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF8_to_UTF16BE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF8_to_UTF16BE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF8_to_UTF16BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test non-ASCII inside the BMP, below the surrogates.

	cpLo = 0x80; cpHi = 0xD800; len16 = cpHi-cpLo;
	for ( cp = cpLo, len8 = 0; cp < cpHi; ++cp, len8 += len8x ) CodePoint_to_UTF8 ( cp, &sU8[len8], 8, &len8x );
	if ( len8 != (2*(0x800-cpLo) + 3*(cpHi-0x800)) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF16BE ( sU8, len8, sU16, sizeof(sU16), &len8x, &len16x );
	if ( (len8 != len8x) || (len16 != len16x) ) fprintf ( log, "  *** UTF8_to_UTF16BE length failure, %d -> %d\n", len8x, len16x );

	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16BE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF8_to_UTF16BE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF8_to_UTF16BE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF8_to_UTF16BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len16 = cpHi-cpLo;
	for ( cp = cpLo, len8 = 0; cp < cpHi; ++cp, len8 += len8x ) CodePoint_to_UTF8 ( cp, &sU8[len8], 8, &len8x );
	if ( len8 != 3*(cpHi-cpLo) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF16BE ( sU8, len8, sU16, sizeof(sU16), &len8x, &len16x );
	if ( (len8 != len8x) || (len16 != len16x) ) fprintf ( log, "  *** UTF8_to_UTF16BE length failure, %d -> %d\n", len8x, len16x );

	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16BE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF8_to_UTF16BE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF8_to_UTF16BE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF8_to_UTF16BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len16 = (cpHi-cpLo)*2;
	for ( cp = cpLo, len8 = 0; cp < cpHi; ++cp, len8 += len8x ) CodePoint_to_UTF8 ( cp, &sU8[len8], 8, &len8x );
	if ( len8 != 4*(cpHi-cpLo) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF16BE ( sU8, len8, sU16, sizeof(sU16), &len8x, &len16x );
	if ( (len8 != len8x) || (len16 != len16x) ) fprintf ( log, "  *** UTF8_to_UTF16BE length failure, %d -> %d\n", len8x, len16x );

	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16BE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 2) || (cpx != cp) ) fprintf ( log, "  *** UTF8_to_UTF16BE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF8_to_UTF16BE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF8_to_UTF16BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating ASCII, non-ASCII BMP, beyond BMP.

	len16 = 0x80*(1+1+1+2);
	for ( i = 0, len8 = 0; i < 0x80; ++i ) {
		CodePoint_to_UTF8 ( i, &sU8[len8], 8, &len8x );
		len8 += len8x;
		CodePoint_to_UTF8 ( i+0x100, &sU8[len8], 8, &len8x );
		len8 += len8x;
		CodePoint_to_UTF8 ( i+0x1000, &sU8[len8], 8, &len8x );
		len8 += len8x;
		CodePoint_to_UTF8 ( i+0x10000, &sU8[len8], 8, &len8x );
		len8 += len8x;
	}
	if ( len8 != 0x80*(1+2+3+4) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF16BE ( sU8, len8, sU16, sizeof(sU16), &len8x, &len16x );
	if ( (len8 != len8x) || (len16 != len16x) ) fprintf ( log, "  *** UTF8_to_UTF16BE length failure, %d -> %d\n", len8x, len16x );

	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, len16 = 0; i < 0x80; ++i ) {
		CodePoint_from_UTF16BE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != i) ) fprintf ( log, "  *** UTF8_to_UTF16BE failure for U+%.4X\n", i );
		len16 += len16x;
		CodePoint_from_UTF16BE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != i+0x100) ) fprintf ( log, "  *** UTF8_to_UTF16BE failure for U+%.4X\n", i+0x100 );
		len16 += len16x;
		CodePoint_from_UTF16BE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != i+0x1000) ) fprintf ( log, "  *** UTF8_to_UTF16BE failure for U+%.4X\n", i+0x1000 );
		len16 += len16x;
		CodePoint_from_UTF16BE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 2) || (cpx != i+0x10000) ) fprintf ( log, "  *** UTF8_to_UTF16BE failure for U+%.4X\n", i+0x10000 );
		len16 += len16x;
	}
	if ( len16 != 0x80*(1+1+1+2) ) fprintf ( log, "  *** UTF8_to_UTF16BE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF8_to_UTF16BE done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len8 = 0x80*(1+2+3+4); len16 = 0x80*(1+1+1+2);

	UTF8_to_UTF16BE ( sU8, 0, sU16, sizeof(sU16), &len8x, &len16x );
	if ( (len8x != 0) || (len16x != 0) )  fprintf ( log, "  *** UTF8_to_UTF16BE empty input failure, %d -> %d\n", len8x, len16x );
	UTF8_to_UTF16BE ( sU8, len8, sU16, 0, &len8x, &len16x );
	if ( (len8x != 0) || (len16x != 0) )  fprintf ( log, "  *** UTF8_to_UTF16BE empty output failure, %d -> %d\n", len8x, len16x );
	UTF8_to_UTF16BE ( sU8, 8, sU16, sizeof(sU16), &len8x, &len16x );
	if ( (len8x != 6) || (len16x != 3) )  fprintf ( log, "  *** UTF8_to_UTF16BE partial input failure, %d -> %d\n", len8x, len16x );
	UTF8_to_UTF16BE ( sU8, len8, sU16, 4, &len8x, &len16x );
	if ( (len8x != 6) || (len16x != 3) )  fprintf ( log, "  *** UTF8_to_UTF16BE partial output failure, %d -> %d\n", len8x, len16x );

	fprintf ( log, "  UTF8_to_UTF16BE done for empty buffers and buffers ending in mid character\n" );

	// -----------------------------------
	// Test UTF8_to_UTF16LE on good input.
	
	fprintf ( log, "\nTesting UTF8_to_UTF16LE on good input\n" );

	// Test ASCII.

	cpLo = 0; cpHi = 0x80; len8 = len16 = 0x80;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU8[i] = UTF8Unit(cp);
	sU8[len8] = 0xFF;

	UTF8_to_UTF16LE ( sU8, len8, sU16, sizeof(sU16), &len8x, &len16x );
	if ( (len8 != len8x) || (len16 != len16x) ) fprintf ( log, "  *** UTF8_to_UTF16LE length failure, %d -> %d\n", len8x, len16x );

	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16LE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF8_to_UTF16LE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF8_to_UTF16LE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF8_to_UTF16LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test non-ASCII inside the BMP, below the surrogates.

	cpLo = 0x80; cpHi = 0xD800; len16 = cpHi-cpLo;
	for ( cp = cpLo, len8 = 0; cp < cpHi; ++cp, len8 += len8x ) CodePoint_to_UTF8 ( cp, &sU8[len8], 8, &len8x );
	if ( len8 != (2*(0x800-cpLo) + 3*(cpHi-0x800)) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF16LE ( sU8, len8, sU16, sizeof(sU16), &len8x, &len16x );
	if ( (len8 != len8x) || (len16 != len16x) ) fprintf ( log, "  *** UTF8_to_UTF16LE length failure, %d -> %d\n", len8x, len16x );

	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16LE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF8_to_UTF16LE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF8_to_UTF16LE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF8_to_UTF16LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len16 = cpHi-cpLo;
	for ( cp = cpLo, len8 = 0; cp < cpHi; ++cp, len8 += len8x ) CodePoint_to_UTF8 ( cp, &sU8[len8], 8, &len8x );
	if ( len8 != 3*(cpHi-cpLo) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF16LE ( sU8, len8, sU16, sizeof(sU16), &len8x, &len16x );
	if ( (len8 != len8x) || (len16 != len16x) ) fprintf ( log, "  *** UTF8_to_UTF16LE length failure, %d -> %d\n", len8x, len16x );

	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16LE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF8_to_UTF16LE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF8_to_UTF16LE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF8_to_UTF16LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len16 = (cpHi-cpLo)*2;
	for ( cp = cpLo, len8 = 0; cp < cpHi; ++cp, len8 += len8x ) CodePoint_to_UTF8 ( cp, &sU8[len8], 8, &len8x );
	if ( len8 != 4*(cpHi-cpLo) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF16LE ( sU8, len8, sU16, sizeof(sU16), &len8x, &len16x );
	if ( (len8 != len8x) || (len16 != len16x) ) fprintf ( log, "  *** UTF8_to_UTF16LE length failure, %d -> %d\n", len8x, len16x );

	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16LE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 2) || (cpx != cp) ) fprintf ( log, "  *** UTF8_to_UTF16LE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF8_to_UTF16LE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF8_to_UTF16LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating ASCII, non-ASCII BMP, beyond BMP.

	len16 = 0x80*(1+1+1+2);
	for ( i = 0, len8 = 0; i < 0x80; ++i ) {
		CodePoint_to_UTF8 ( i, &sU8[len8], 8, &len8x );
		len8 += len8x;
		CodePoint_to_UTF8 ( i+0x100, &sU8[len8], 8, &len8x );
		len8 += len8x;
		CodePoint_to_UTF8 ( i+0x1000, &sU8[len8], 8, &len8x );
		len8 += len8x;
		CodePoint_to_UTF8 ( i+0x10000, &sU8[len8], 8, &len8x );
		len8 += len8x;
	}
	if ( len8 != 0x80*(1+2+3+4) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF16LE ( sU8, len8, sU16, sizeof(sU16), &len8x, &len16x );
	if ( (len8 != len8x) || (len16 != len16x) ) fprintf ( log, "  *** UTF8_to_UTF16LE length failure, %d -> %d\n", len8x, len16x );

	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, len16 = 0; i < 0x80; ++i ) {
		CodePoint_from_UTF16LE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != i) ) fprintf ( log, "  *** UTF8_to_UTF16LE failure for U+%.4X\n", i );
		len16 += len16x;
		CodePoint_from_UTF16LE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != i+0x100) ) fprintf ( log, "  *** UTF8_to_UTF16LE failure for U+%.4X\n", i+0x100 );
		len16 += len16x;
		CodePoint_from_UTF16LE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != i+0x1000) ) fprintf ( log, "  *** UTF8_to_UTF16LE failure for U+%.4X\n", i+0x1000 );
		len16 += len16x;
		CodePoint_from_UTF16LE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 2) || (cpx != i+0x10000) ) fprintf ( log, "  *** UTF8_to_UTF16LE failure for U+%.4X\n", i+0x10000 );
		len16 += len16x;
	}
	if ( len16 != 0x80*(1+1+1+2) ) fprintf ( log, "  *** UTF8_to_UTF16LE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF8_to_UTF16LE done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len8 = 0x80*(1+2+3+4); len16 = 0x80*(1+1+1+2);

	UTF8_to_UTF16LE ( sU8, 0, sU16, sizeof(sU16), &len8x, &len16x );
	if ( (len8x != 0) || (len16x != 0) )  fprintf ( log, "  *** UTF8_to_UTF16LE empty input failure, %d -> %d\n", len8x, len16x );
	UTF8_to_UTF16LE ( sU8, len8, sU16, 0, &len8x, &len16x );
	if ( (len8x != 0) || (len16x != 0) )  fprintf ( log, "  *** UTF8_to_UTF16LE empty output failure, %d -> %d\n", len8x, len16x );
	UTF8_to_UTF16LE ( sU8, 8, sU16, sizeof(sU16), &len8x, &len16x );
	if ( (len8x != 6) || (len16x != 3) )  fprintf ( log, "  *** UTF8_to_UTF16LE partial input failure, %d -> %d\n", len8x, len16x );
	UTF8_to_UTF16LE ( sU8, len8, sU16, 4, &len8x, &len16x );
	if ( (len8x != 6) || (len16x != 3) )  fprintf ( log, "  *** UTF8_to_UTF16LE partial output failure, %d -> %d\n", len8x, len16x );

	fprintf ( log, "  UTF8_to_UTF16LE done for empty buffers and buffers ending in mid character\n" );

}	// Test_UTF8_to_UTF16

// =================================================================================================

static void Test_UTF8_to_UTF32 ( FILE * log )
{
	size_t i;
	size_t len8, len32, len8x, len32x;
	UTF32Unit cp, cpLo, cpHi;

	// ---------------------------------------------------------------------------------------
	// Test UTF8_to_UTF32BE on good input. The CodePoint to/from functions are already tested,
	// use them to verify the results here.
	
	fprintf ( log, "\nTesting UTF8_to_UTF32BE on good input\n" );

	// Test ASCII.

	cpLo = 0; cpHi = 0x80; len8 = len32 = 0x80;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU8[i] = UTF8Unit(cp);
	sU8[len8] = 0xFF;

	UTF8_to_UTF32BE ( sU8, len8, sU32, sizeof(sU32), &len8x, &len32x );
	if ( (len8 != len8x) || (len32 != len32x) ) fprintf ( log, "  *** UTF8_to_UTF32BE length failure, %d -> %d\n", len8x, len32x );

	sU32[len32x] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32BE(cp) ) fprintf ( log, "  *** UTF8_to_UTF32BE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF8_to_UTF32BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test non-ASCII inside the BMP, below the surrogates.

	cpLo = 0x80; cpHi = 0xD800; len32 = cpHi-cpLo;
	for ( cp = cpLo, len8 = 0; cp < cpHi; ++cp, len8 += len8x ) CodePoint_to_UTF8 ( cp, &sU8[len8], 8, &len8x );
	if ( len8 != (2*(0x800-cpLo) + 3*(cpHi-0x800)) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF32BE ( sU8, len8, sU32, sizeof(sU32), &len8x, &len32x );
	if ( (len8 != len8x) || (len32 != len32x) ) fprintf ( log, "  *** UTF8_to_UTF32BE length failure, %d -> %d\n", len8x, len32x );

	sU32[len32x] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32BE(cp) ) fprintf ( log, "  *** UTF8_to_UTF32BE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF8_to_UTF32BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len32 = cpHi-cpLo;
	for ( cp = cpLo, len8 = 0; cp < cpHi; ++cp, len8 += len8x ) CodePoint_to_UTF8 ( cp, &sU8[len8], 8, &len8x );
	if ( len8 !=3*(cpHi-cpLo) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF32BE ( sU8, len8, sU32, sizeof(sU32), &len8x, &len32x );
	if ( (len8 != len8x) || (len32 != len32x) ) fprintf ( log, "  *** UTF8_to_UTF32BE length failure, %d -> %d\n", len8x, len32x );

	sU32[len32x] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32BE(cp) ) fprintf ( log, "  *** UTF8_to_UTF32BE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF8_to_UTF32BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len32 = cpHi-cpLo;
	for ( cp = cpLo, len8 = 0; cp < cpHi; ++cp, len8 += len8x ) CodePoint_to_UTF8 ( cp, &sU8[len8], 8, &len8x );
	if ( len8 !=4*(cpHi-cpLo) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF32BE ( sU8, len8, sU32, sizeof(sU32), &len8x, &len32x );
	if ( (len8 != len8x) || (len32 != len32x) ) fprintf ( log, "  *** UTF8_to_UTF32BE length failure, %d -> %d\n", len8x, len32x );

	sU32[len32x] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32BE(cp) ) fprintf ( log, "  *** UTF8_to_UTF32BE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF8_to_UTF32BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating ASCII, non-ASCII BMP, beyond BMP.

	len32 = 0x80*(1+1+1+1);
	for ( i = 0, len8 = 0; i < 0x80; ++i ) {
		CodePoint_to_UTF8 ( i, &sU8[len8], 8, &len8x );
		len8 += len8x;
		CodePoint_to_UTF8 ( i+0x100, &sU8[len8], 8, &len8x );
		len8 += len8x;
		CodePoint_to_UTF8 ( i+0x1000, &sU8[len8], 8, &len8x );
		len8 += len8x;
		CodePoint_to_UTF8 ( i+0x10000, &sU8[len8], 8, &len8x );
		len8 += len8x;
	}
	if ( len8 != 0x80*(1+2+3+4) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF32BE ( sU8, len8, sU32, sizeof(sU32), &len8x, &len32x );
	if ( (len8 != len8x) || (len32 != len32x) ) fprintf ( log, "  *** UTF8_to_UTF32BE length failure, %d -> %d\n", len8x, len32x );

	sU32[len32x] = 0xFFFFFFFF;
	for ( i = 0, len32 = 0; i < 0x80; ++i ) {
		if ( sU32[len32] != NativeUTF32BE(i) ) fprintf ( log, "  *** UTF8_to_UTF32BE failure for U+%.4X\n", i );
		++len32;
		if ( sU32[len32] != NativeUTF32BE(i+0x100) ) fprintf ( log, "  *** UTF8_to_UTF32BE failure for U+%.4X\n", i+0x100 );
		++len32;
		if ( sU32[len32] != NativeUTF32BE(i+0x1000) ) fprintf ( log, "  *** UTF8_to_UTF32BE failure for U+%.4X\n", i+0x1000 );
		++len32;
		if ( sU32[len32] != NativeUTF32BE(i+0x10000) ) fprintf ( log, "  *** UTF8_to_UTF32BE failure for U+%.4X\n", i+0x10000 );
		++len32;
	}

	fprintf ( log, "  UTF8_to_UTF32BE done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len8 = 0x80*(1+2+3+4); len32 = 0x80*(1+1+1+1);

	UTF8_to_UTF32BE ( sU8, 0, sU32, sizeof(sU32), &len8x, &len32x );
	if ( (len8x != 0) || (len32x != 0) )  fprintf ( log, "  *** UTF8_to_UTF32BE empty input failure, %d -> %d\n", len8x, len32x );
	UTF8_to_UTF32BE ( sU8, len8, sU32, 0, &len8x, &len32x );
	if ( (len8x != 0) || (len32x != 0) )  fprintf ( log, "  *** UTF8_to_UTF32BE empty output failure, %d -> %d\n", len8x, len32x );
	UTF8_to_UTF32BE ( sU8, 8, sU32, sizeof(sU32), &len8x, &len32x );
	if ( (len8x != 6) || (len32x != 3) )  fprintf ( log, "  *** UTF8_to_UTF32BE partial input failure, %d -> %d\n", len8x, len32x );

	fprintf ( log, "  UTF8_to_UTF32BE done for empty buffers and buffers ending in mid character\n" );

	// -----------------------------------
	// Test UTF8_to_UTF32LE on good input.
	
	fprintf ( log, "\nTesting UTF8_to_UTF32LE on good input\n" );

	// Test ASCII.

	cpLo = 0; cpHi = 0x80; len8 = len32 = 0x80;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU8[i] = UTF8Unit(cp);
	sU8[len8] = 0xFF;

	UTF8_to_UTF32LE ( sU8, len8, sU32, sizeof(sU32), &len8x, &len32x );
	if ( (len8 != len8x) || (len32 != len32x) ) fprintf ( log, "  *** UTF8_to_UTF32LE length failure, %d -> %d\n", len8x, len32x );

	sU32[len32x] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32LE(cp) ) fprintf ( log, "  *** UTF8_to_UTF32LE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF8_to_UTF32LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test non-ASCII inside the BMP, below the surrogates.

	cpLo = 0x80; cpHi = 0xD800; len32 = cpHi-cpLo;
	for ( cp = cpLo, len8 = 0; cp < cpHi; ++cp, len8 += len8x ) CodePoint_to_UTF8 ( cp, &sU8[len8], 8, &len8x );
	if ( len8 != (2*(0x800-cpLo) + 3*(cpHi-0x800)) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF32LE ( sU8, len8, sU32, sizeof(sU32), &len8x, &len32x );
	if ( (len8 != len8x) || (len32 != len32x) ) fprintf ( log, "  *** UTF8_to_UTF32LE length failure, %d -> %d\n", len8x, len32x );

	sU32[len32x] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32LE(cp) ) fprintf ( log, "  *** UTF8_to_UTF32LE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF8_to_UTF32LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len32 = cpHi-cpLo;
	for ( cp = cpLo, len8 = 0; cp < cpHi; ++cp, len8 += len8x ) CodePoint_to_UTF8 ( cp, &sU8[len8], 8, &len8x );
	if ( len8 !=3*(cpHi-cpLo) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF32LE ( sU8, len8, sU32, sizeof(sU32), &len8x, &len32x );
	if ( (len8 != len8x) || (len32 != len32x) ) fprintf ( log, "  *** UTF8_to_UTF32LE length failure, %d -> %d\n", len8x, len32x );

	sU32[len32x] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32LE(cp) ) fprintf ( log, "  *** UTF8_to_UTF32LE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF8_to_UTF32LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len32 = cpHi-cpLo;
	for ( cp = cpLo, len8 = 0; cp < cpHi; ++cp, len8 += len8x ) CodePoint_to_UTF8 ( cp, &sU8[len8], 8, &len8x );
	if ( len8 !=4*(cpHi-cpLo) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF32LE ( sU8, len8, sU32, sizeof(sU32), &len8x, &len32x );
	if ( (len8 != len8x) || (len32 != len32x) ) fprintf ( log, "  *** UTF8_to_UTF32LE length failure, %d -> %d\n", len8x, len32x );

	sU32[len32x] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32LE(cp) ) fprintf ( log, "  *** UTF8_to_UTF32LE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF8_to_UTF32LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating ASCII, non-ASCII BMP, beyond BMP.

	len32 = 0x80*(1+1+1+1);
	for ( i = 0, len8 = 0; i < 0x80; ++i ) {
		CodePoint_to_UTF8 ( i, &sU8[len8], 8, &len8x );
		len8 += len8x;
		CodePoint_to_UTF8 ( i+0x100, &sU8[len8], 8, &len8x );
		len8 += len8x;
		CodePoint_to_UTF8 ( i+0x1000, &sU8[len8], 8, &len8x );
		len8 += len8x;
		CodePoint_to_UTF8 ( i+0x10000, &sU8[len8], 8, &len8x );
		len8 += len8x;
	}
	if ( len8 != 0x80*(1+2+3+4) ) fprintf ( log, "  *** CodePoint_to_UTF8 length failure, %d\n", len8 );
	sU8[len8] = 0xFF;

	UTF8_to_UTF32LE ( sU8, len8, sU32, sizeof(sU32), &len8x, &len32x );
	if ( (len8 != len8x) || (len32 != len32x) ) fprintf ( log, "  *** UTF8_to_UTF32LE length failure, %d -> %d\n", len8x, len32x );

	sU32[len32x] = 0xFFFFFFFF;
	for ( i = 0, len32 = 0; i < 0x80; ++i ) {
		if ( sU32[len32] != NativeUTF32LE(i) ) fprintf ( log, "  *** UTF8_to_UTF32LE failure for U+%.4X\n", i );
		++len32;
		if ( sU32[len32] != NativeUTF32LE(i+0x100) ) fprintf ( log, "  *** UTF8_to_UTF32LE failure for U+%.4X\n", i+0x100 );
		++len32;
		if ( sU32[len32] != NativeUTF32LE(i+0x1000) ) fprintf ( log, "  *** UTF8_to_UTF32LE failure for U+%.4X\n", i+0x1000 );
		++len32;
		if ( sU32[len32] != NativeUTF32LE(i+0x10000) ) fprintf ( log, "  *** UTF8_to_UTF32LE failure for U+%.4X\n", i+0x10000 );
		++len32;
	}

	fprintf ( log, "  UTF8_to_UTF32LE done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len8 = 0x80*(1+2+3+4); len32 = 0x80*(1+1+1+1);

	UTF8_to_UTF32LE ( sU8, 0, sU32, sizeof(sU32), &len8x, &len32x );
	if ( (len8x != 0) || (len32x != 0) )  fprintf ( log, "  *** UTF8_to_UTF32LE empty input failure, %d -> %d\n", len8x, len32x );
	UTF8_to_UTF32LE ( sU8, len8, sU32, 0, &len8x, &len32x );
	if ( (len8x != 0) || (len32x != 0) )  fprintf ( log, "  *** UTF8_to_UTF32LE empty output failure, %d -> %d\n", len8x, len32x );
	UTF8_to_UTF32LE ( sU8, 8, sU32, sizeof(sU32), &len8x, &len32x );
	if ( (len8x != 6) || (len32x != 3) )  fprintf ( log, "  *** UTF8_to_UTF32LE partial input failure, %d -> %d\n", len8x, len32x );

	fprintf ( log, "  UTF8_to_UTF32LE done for empty buffers and buffers ending in mid character\n" );

}	// Test_UTF8_to_UTF32

// =================================================================================================

static void Test_UTF16_to_UTF8 ( FILE * log )
{
	size_t i;
	size_t len16, len8, len16x, len8x;
	UTF32Unit cp, cpx, cpLo, cpHi;

	// ---------------------------------------------------------------------------------------
	// Test UTF16BE_to_UTF8 on good input. The CodePoint to/from functions are already tested,
	// use them to verify the results here.
	
	fprintf ( log, "\nTesting UTF16BE_to_UTF8 on good input\n" );

	// Test ASCII.

	cpLo = 0; cpHi = 0x80; len16 = len8 = 0x80;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16BE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.

	UTF16BE_to_UTF8 ( sU16, len16, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16 != len16x) || (len8 != len8x) ) fprintf ( log, "  *** UTF16BE_to_UTF8 length failure, %d -> %d\n", len16x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF16BE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF16BE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF16BE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test 2 byte non-ASCII inside the BMP.

	cpLo = 0x80; cpHi = 0x800; len16 = cpHi-cpLo; len8 = 2*(cpHi-cpLo);
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16BE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.

	UTF16BE_to_UTF8 ( sU16, len16, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16 != len16x) || (len8 != len8x) ) fprintf ( log, "  *** UTF16BE_to_UTF8 length failure, %d -> %d\n", len16x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 2) || (cpx != cp) ) fprintf ( log, "  *** UTF16BE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF16BE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF16BE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test 3 byte non-ASCII inside the BMP, below the surrogates.

	cpLo = 0x800; cpHi = 0xD800; len16 = cpHi-cpLo; len8 = 3*(cpHi-cpLo);
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16BE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.

	UTF16BE_to_UTF8 ( sU16, len16, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16 != len16x) || (len8 != len8x) ) fprintf ( log, "  *** UTF16BE_to_UTF8 length failure, %d -> %d\n", len16x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 3) || (cpx != cp) ) fprintf ( log, "  *** UTF16BE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF16BE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF16BE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len16 = cpHi-cpLo; len8 = 3*(cpHi-cpLo);
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16BE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.

	UTF16BE_to_UTF8 ( sU16, len16, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16 != len16x) || (len8 != len8x) ) fprintf ( log, "  *** UTF16BE_to_UTF8 length failure, %d -> %d\n", len16x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 3) || (cpx != cp) ) fprintf ( log, "  *** UTF16BE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF16BE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF16BE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len8 = (cpHi-cpLo)*4;
	for ( cp = cpLo, len16 = 0; cp < cpHi; ++cp, len16 += len16x ) CodePoint_to_UTF16BE ( cp, &sU16[len16], 4, &len16x );
	if ( len16 != 2*(cpHi-cpLo) ) fprintf ( log, "  *** CodePoint_to_UTF16BE length failure, %d\n", len16 );
	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.

	UTF16BE_to_UTF8 ( sU16, len16, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16 != len16x) || (len8 != len8x) ) fprintf ( log, "  *** UTF16BE_to_UTF8 length failure, %d -> %d\n", len16x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 4) || (cpx != cp) ) fprintf ( log, "  *** UTF16BE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF16BE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF16BE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating ASCII, non-ASCII BMP, beyond BMP.

	len8 = 0x80*(1+2+3+4);
	for ( i = 0, len16 = 0; i < 0x80; ++i ) {
		CodePoint_to_UTF16BE ( i, &sU16[len16], 4, &len16x );
		len16 += len16x;
		CodePoint_to_UTF16BE ( i+0x100, &sU16[len16], 4, &len16x );
		len16 += len16x;
		CodePoint_to_UTF16BE ( i+0x1000, &sU16[len16], 4, &len16x );
		len16 += len16x;
		CodePoint_to_UTF16BE ( i+0x10000, &sU16[len16], 4, &len16x );
		len16 += len16x;
	}
	if ( len16 != 0x80*(1+1+1+2) ) fprintf ( log, "  *** CodePoint_to_UTF16BE length failure, %d\n", len16 );
	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.

	UTF16BE_to_UTF8 ( sU16, len16, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16 != len16x) || (len8 != len8x) ) fprintf ( log, "  *** UTF16BE_to_UTF8 length failure, %d -> %d\n", len16x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, len8 = 0; i < 0x80; ++i ) {
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 1) || (cpx != i) ) fprintf ( log, "  *** UTF16BE_to_UTF8 failure for U+%.4X\n", i );
		len8 += len8x;
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 2) || (cpx != i+0x100) ) fprintf ( log, "  *** UTF16BE_to_UTF8 failure for U+%.4X\n", i+0x100 );
		len8 += len8x;
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 3) || (cpx != i+0x1000) ) fprintf ( log, "  *** UTF16BE_to_UTF8 failure for U+%.4X\n", i+0x1000 );
		len8 += len8x;
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 4) || (cpx != i+0x10000) ) fprintf ( log, "  *** UTF16BE_to_UTF8 failure for U+%.4X\n", i+0x10000 );
		len8 += len8x;
	}
	if ( len8 != 0x80*(1+2+3+4) ) fprintf ( log, "  *** UTF16BE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF16BE_to_UTF8 done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len16 = 0x80*(1+1+1+2); len8 = 0x80*(1+2+3+4);

	UTF16BE_to_UTF8 ( sU16, 0, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16x != 0) || (len8x != 0) )  fprintf ( log, "  *** UTF16BE_to_UTF8 empty input failure, %d -> %d\n", len16x, len8x );
	UTF16BE_to_UTF8 ( sU16, len16, sU8, 0, &len16x, &len8x );
	if ( (len16x != 0) || (len8x != 0) )  fprintf ( log, "  *** UTF16BE_to_UTF8 empty output failure, %d -> %d\n", len16x, len8x );
	UTF16BE_to_UTF8 ( sU16, 4, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16x != 3) || (len8x != 6) )  fprintf ( log, "  *** UTF16BE_to_UTF8 partial input failure, %d -> %d\n", len16x, len8x );
	UTF16BE_to_UTF8 ( sU16, len16, sU8, 8, &len16x, &len8x );
	if ( (len16x != 3) || (len8x != 6) )  fprintf ( log, "  *** UTF16BE_to_UTF8 partial output failure, %d -> %d\n", len16x, len8x );

	fprintf ( log, "  UTF16BE_to_UTF8 done for empty buffers and buffers ending in mid character\n" );

	// -----------------------------------
	// Test UTF16LE_to_UTF8 on good input.
	
	fprintf ( log, "\nTesting UTF16LE_to_UTF8 on good input\n" );

	// Test ASCII.

	cpLo = 0; cpHi = 0x80; len16 = len8 = 0x80;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16LE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.

	UTF16LE_to_UTF8 ( sU16, len16, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16 != len16x) || (len8 != len8x) ) fprintf ( log, "  *** UTF16LE_to_UTF8 length failure, %d -> %d\n", len16x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF16LE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF16LE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF16LE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test 2 byte non-ASCII inside the BMP.

	cpLo = 0x80; cpHi = 0x800; len16 = cpHi-cpLo; len8 = 2*(cpHi-cpLo);
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16LE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.

	UTF16LE_to_UTF8 ( sU16, len16, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16 != len16x) || (len8 != len8x) ) fprintf ( log, "  *** UTF16LE_to_UTF8 length failure, %d -> %d\n", len16x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 2) || (cpx != cp) ) fprintf ( log, "  *** UTF16LE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF16LE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF16LE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test 3 byte non-ASCII inside the BMP, below the surrogates.

	cpLo = 0x800; cpHi = 0xD800; len16 = cpHi-cpLo; len8 = 3*(cpHi-cpLo);
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16LE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.

	UTF16LE_to_UTF8 ( sU16, len16, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16 != len16x) || (len8 != len8x) ) fprintf ( log, "  *** UTF16LE_to_UTF8 length failure, %d -> %d\n", len16x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 3) || (cpx != cp) ) fprintf ( log, "  *** UTF16LE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF16LE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF16LE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len16 = cpHi-cpLo; len8 = 3*(cpHi-cpLo);
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16LE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.

	UTF16LE_to_UTF8 ( sU16, len16, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16 != len16x) || (len8 != len8x) ) fprintf ( log, "  *** UTF16LE_to_UTF8 length failure, %d -> %d\n", len16x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 3) || (cpx != cp) ) fprintf ( log, "  *** UTF16LE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF16LE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF16LE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len8 = (cpHi-cpLo)*4;
	for ( cp = cpLo, len16 = 0; cp < cpHi; ++cp, len16 += len16x ) CodePoint_to_UTF16LE ( cp, &sU16[len16], 4, &len16x );
	if ( len16 != 2*(cpHi-cpLo) ) fprintf ( log, "  *** CodePoint_to_UTF16LE length failure, %d\n", len16 );
	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.

	UTF16LE_to_UTF8 ( sU16, len16, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16 != len16x) || (len8 != len8x) ) fprintf ( log, "  *** UTF16LE_to_UTF8 length failure, %d -> %d\n", len16x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 4) || (cpx != cp) ) fprintf ( log, "  *** UTF16LE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF16LE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF16LE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating ASCII, non-ASCII BMP, beyond BMP.

	len8 = 0x80*(1+2+3+4);
	for ( i = 0, len16 = 0; i < 0x80; ++i ) {
		CodePoint_to_UTF16LE ( i, &sU16[len16], 4, &len16x );
		len16 += len16x;
		CodePoint_to_UTF16LE ( i+0x100, &sU16[len16], 4, &len16x );
		len16 += len16x;
		CodePoint_to_UTF16LE ( i+0x1000, &sU16[len16], 4, &len16x );
		len16 += len16x;
		CodePoint_to_UTF16LE ( i+0x10000, &sU16[len16], 4, &len16x );
		len16 += len16x;
	}
	if ( len16 != 0x80*(1+1+1+2) ) fprintf ( log, "  *** CodePoint_to_UTF16LE length failure, %d\n", len16 );
	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.

	UTF16LE_to_UTF8 ( sU16, len16, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16 != len16x) || (len8 != len8x) ) fprintf ( log, "  *** UTF16LE_to_UTF8 length failure, %d -> %d\n", len16x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, len8 = 0; i < 0x80; ++i ) {
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 1) || (cpx != i) ) fprintf ( log, "  *** UTF16LE_to_UTF8 failure for U+%.4X\n", i );
		len8 += len8x;
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 2) || (cpx != i+0x100) ) fprintf ( log, "  *** UTF16LE_to_UTF8 failure for U+%.4X\n", i+0x100 );
		len8 += len8x;
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 3) || (cpx != i+0x1000) ) fprintf ( log, "  *** UTF16LE_to_UTF8 failure for U+%.4X\n", i+0x1000 );
		len8 += len8x;
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 4) || (cpx != i+0x10000) ) fprintf ( log, "  *** UTF16LE_to_UTF8 failure for U+%.4X\n", i+0x10000 );
		len8 += len8x;
	}
	if ( len8 != 0x80*(1+2+3+4) ) fprintf ( log, "  *** UTF16LE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF16LE_to_UTF8 done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len16 = 0x80*(1+1+1+2); len8 = 0x80*(1+2+3+4);

	UTF16LE_to_UTF8 ( sU16, 0, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16x != 0) || (len8x != 0) )  fprintf ( log, "  *** UTF16LE_to_UTF8 empty input failure, %d -> %d\n", len16x, len8x );
	UTF16LE_to_UTF8 ( sU16, len16, sU8, 0, &len16x, &len8x );
	if ( (len16x != 0) || (len8x != 0) )  fprintf ( log, "  *** UTF16LE_to_UTF8 empty output failure, %d -> %d\n", len16x, len8x );
	UTF16LE_to_UTF8 ( sU16, 4, sU8, sizeof(sU8), &len16x, &len8x );
	if ( (len16x != 3) || (len8x != 6) )  fprintf ( log, "  *** UTF16LE_to_UTF8 partial input failure, %d -> %d\n", len16x, len8x );
	UTF16LE_to_UTF8 ( sU16, len16, sU8, 8, &len16x, &len8x );
	if ( (len16x != 3) || (len8x != 6) )  fprintf ( log, "  *** UTF16LE_to_UTF8 partial output failure, %d -> %d\n", len16x, len8x );

	fprintf ( log, "  UTF16LE_to_UTF8 done for empty buffers and buffers ending in mid character\n" );

}	// Test_UTF16_to_UTF8

// =================================================================================================

static void Test_UTF32_to_UTF8 ( FILE * log )
{
	size_t i;
	size_t len32, len8, len32x, len8x;
	UTF32Unit cp, cpx, cpLo, cpHi;

	// -----------------------------------
	// Test UTF32BE_to_UTF8 on good input.
	
	fprintf ( log, "\nTesting UTF32BE_to_UTF8 on good input\n" );

	// Test ASCII.

	cpLo = 0; cpHi = 0x80; len32 = len8 = 0x80;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32BE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32BE_to_UTF8 ( sU32, len32, sU8, sizeof(sU8), &len32x, &len8x );
	if ( (len32 != len32x) || (len8 != len8x) ) fprintf ( log, "  *** UTF32BE_to_UTF8 length failure, %d -> %d\n", len32x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF32BE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF32BE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF32BE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test 2 byte non-ASCII inside the BMP.

	cpLo = 0x80; cpHi = 0x800; len32 = cpHi-cpLo; len8 = 2*(cpHi-cpLo);
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32BE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32BE_to_UTF8 ( sU32, len32, sU8, sizeof(sU8), &len32x, &len8x );
	if ( (len32 != len32x) || (len8 != len8x) ) fprintf ( log, "  *** UTF32BE_to_UTF8 length failure, %d -> %d\n", len32x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 2) || (cpx != cp) ) fprintf ( log, "  *** UTF32BE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF32BE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF32BE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test 3 byte non-ASCII inside the BMP, below the surrogates.

	cpLo = 0x800; cpHi = 0xD800; len32 = cpHi-cpLo; len8 = 3*(cpHi-cpLo);
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32BE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32BE_to_UTF8 ( sU32, len32, sU8, sizeof(sU8), &len32x, &len8x );
	if ( (len32 != len32x) || (len8 != len8x) ) fprintf ( log, "  *** UTF32BE_to_UTF8 length failure, %d -> %d\n", len32x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 3) || (cpx != cp) ) fprintf ( log, "  *** UTF32BE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF32BE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF32BE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len32 = cpHi-cpLo; len8 = 3*(cpHi-cpLo);
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32BE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32BE_to_UTF8 ( sU32, len32, sU8, sizeof(sU8), &len32x, &len8x );
	if ( (len32 != len32x) || (len8 != len8x) ) fprintf ( log, "  *** UTF32BE_to_UTF8 length failure, %d -> %d\n", len32x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 3) || (cpx != cp) ) fprintf ( log, "  *** UTF32BE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF32BE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF32BE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len32 = cpHi-cpLo; len8 = (cpHi-cpLo)*4;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32BE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32BE_to_UTF8 ( sU32, len32, sU8, sizeof(sU8), &len32x, &len8x );
	if ( (len32 != len32x) || (len8 != len8x) ) fprintf ( log, "  *** UTF32BE_to_UTF8 length failure, %d -> %d\n", len32x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 4) || (cpx != cp) ) fprintf ( log, "  *** UTF32BE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF32BE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF32BE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating ASCII, non-ASCII BMP, beyond BMP.

	len8 = 0x80*(1+2+3+4);
	for ( i = 0, len32 = 0; i < 0x80; ++i ) {
		sU32[len32] = NativeUTF32BE(i);
		++len32;
		sU32[len32] = NativeUTF32BE(i+0x100);
		++len32;
		sU32[len32] = NativeUTF32BE(i+0x1000);
		++len32;
		sU32[len32] = NativeUTF32BE(i+0x10000);
		++len32;
	}
	sU32[len32] = 0xFFFFFFFF;

	UTF32BE_to_UTF8 ( sU32, len32, sU8, sizeof(sU8), &len32x, &len8x );
	if ( (len32 != len32x) || (len8 != len8x) ) fprintf ( log, "  *** UTF32BE_to_UTF8 length failure, %d -> %d\n", len32x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, len8 = 0; i < 0x80; ++i ) {
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 1) || (cpx != i) ) fprintf ( log, "  *** UTF32BE_to_UTF8 failure for U+%.4X\n", i );
		len8 += len8x;
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 2) || (cpx != i+0x100) ) fprintf ( log, "  *** UTF32BE_to_UTF8 failure for U+%.4X\n", i+0x100 );
		len8 += len8x;
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 3) || (cpx != i+0x1000) ) fprintf ( log, "  *** UTF32BE_to_UTF8 failure for U+%.4X\n", i+0x1000 );
		len8 += len8x;
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 4) || (cpx != i+0x10000) ) fprintf ( log, "  *** UTF32BE_to_UTF8 failure for U+%.4X\n", i+0x10000 );
		len8 += len8x;
	}
	if ( len8 != 0x80*(1+2+3+4) ) fprintf ( log, "  *** UTF32BE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF32BE_to_UTF8 done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len32 = 0x80*(1+1+1+2); len8 = 0x80*(1+2+3+4);

	UTF32BE_to_UTF8 ( sU32, 0, sU8, sizeof(sU8), &len32x, &len8x );
	if ( (len32x != 0) || (len8x != 0) )  fprintf ( log, "  *** UTF32BE_to_UTF8 empty input failure, %d -> %d\n", len32x, len8x );
	UTF32BE_to_UTF8 ( sU32, len32, sU8, 0, &len32x, &len8x );
	if ( (len32x != 0) || (len8x != 0) )  fprintf ( log, "  *** UTF32BE_to_UTF8 empty output failure, %d -> %d\n", len32x, len8x );
	UTF32BE_to_UTF8 ( sU32, len32, sU8, 8, &len32x, &len8x );
	if ( (len32x != 3) || (len8x != 6) )  fprintf ( log, "  *** UTF32BE_to_UTF8 partial output failure, %d -> %d\n", len32x, len8x );

	fprintf ( log, "  UTF32BE_to_UTF8 done for empty buffers and buffers ending in mid character\n" );

	// -----------------------------------
	// Test UTF32LE_to_UTF8 on good input.
	
	fprintf ( log, "\nTesting UTF32LE_to_UTF8 on good input\n" );

	// Test ASCII.

	cpLo = 0; cpHi = 0x80; len32 = len8 = 0x80;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32LE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32LE_to_UTF8 ( sU32, len32, sU8, sizeof(sU8), &len32x, &len8x );
	if ( (len32 != len32x) || (len8 != len8x) ) fprintf ( log, "  *** UTF32LE_to_UTF8 length failure, %d -> %d\n", len32x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF32LE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF32LE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF32LE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test 2 byte non-ASCII inside the BMP.

	cpLo = 0x80; cpHi = 0x800; len32 = cpHi-cpLo; len8 = 2*(cpHi-cpLo);
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32LE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32LE_to_UTF8 ( sU32, len32, sU8, sizeof(sU8), &len32x, &len8x );
	if ( (len32 != len32x) || (len8 != len8x) ) fprintf ( log, "  *** UTF32LE_to_UTF8 length failure, %d -> %d\n", len32x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 2) || (cpx != cp) ) fprintf ( log, "  *** UTF32LE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF32LE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF32LE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test 3 byte non-ASCII inside the BMP, below the surrogates.

	cpLo = 0x800; cpHi = 0xD800; len32 = cpHi-cpLo; len8 = 3*(cpHi-cpLo);
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32LE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32LE_to_UTF8 ( sU32, len32, sU8, sizeof(sU8), &len32x, &len8x );
	if ( (len32 != len32x) || (len8 != len8x) ) fprintf ( log, "  *** UTF32LE_to_UTF8 length failure, %d -> %d\n", len32x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 3) || (cpx != cp) ) fprintf ( log, "  *** UTF32LE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF32LE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF32LE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len32 = cpHi-cpLo; len8 = 3*(cpHi-cpLo);
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32LE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32LE_to_UTF8 ( sU32, len32, sU8, sizeof(sU8), &len32x, &len8x );
	if ( (len32 != len32x) || (len8 != len8x) ) fprintf ( log, "  *** UTF32LE_to_UTF8 length failure, %d -> %d\n", len32x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 3) || (cpx != cp) ) fprintf ( log, "  *** UTF32LE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF32LE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF32LE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len32 = cpHi-cpLo; len8 = (cpHi-cpLo)*4;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32LE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32LE_to_UTF8 ( sU32, len32, sU8, sizeof(sU8), &len32x, &len8x );
	if ( (len32 != len32x) || (len8 != len8x) ) fprintf ( log, "  *** UTF32LE_to_UTF8 length failure, %d -> %d\n", len32x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, cp = cpLo; cp < cpHi; i += len8x, ++cp ) {
		CodePoint_from_UTF8 ( &sU8[i], 8, &cpx, &len8x );
		if ( (len8x != 4) || (cpx != cp) ) fprintf ( log, "  *** UTF32LE_to_UTF8 failure for U+%.4X\n", cp );
	}
	if ( i != len8 ) fprintf ( log, "  *** UTF32LE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF32LE_to_UTF8 done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating ASCII, non-ASCII BMP, beyond BMP.

	len8 = 0x80*(1+2+3+4);
	for ( i = 0, len32 = 0; i < 0x80; ++i ) {
		sU32[len32] = NativeUTF32LE(i);
		++len32;
		sU32[len32] = NativeUTF32LE(i+0x100);
		++len32;
		sU32[len32] = NativeUTF32LE(i+0x1000);
		++len32;
		sU32[len32] = NativeUTF32LE(i+0x10000);
		++len32;
	}
	sU32[len32] = 0xFFFFFFFF;

	UTF32LE_to_UTF8 ( sU32, len32, sU8, sizeof(sU8), &len32x, &len8x );
	if ( (len32 != len32x) || (len8 != len8x) ) fprintf ( log, "  *** UTF32LE_to_UTF8 length failure, %d -> %d\n", len32x, len8x );

	sU8[len8] = 0xFF;
	for ( i = 0, len8 = 0; i < 0x80; ++i ) {
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 1) || (cpx != i) ) fprintf ( log, "  *** UTF32LE_to_UTF8 failure for U+%.4X\n", i );
		len8 += len8x;
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 2) || (cpx != i+0x100) ) fprintf ( log, "  *** UTF32LE_to_UTF8 failure for U+%.4X\n", i+0x100 );
		len8 += len8x;
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 3) || (cpx != i+0x1000) ) fprintf ( log, "  *** UTF32LE_to_UTF8 failure for U+%.4X\n", i+0x1000 );
		len8 += len8x;
		CodePoint_from_UTF8 ( &sU8[len8], 8, &cpx, &len8x );
		if ( (len8x != 4) || (cpx != i+0x10000) ) fprintf ( log, "  *** UTF32LE_to_UTF8 failure for U+%.4X\n", i+0x10000 );
		len8 += len8x;
	}
	if ( len8 != 0x80*(1+2+3+4) ) fprintf ( log, "  *** UTF32LE_to_UTF8 consume failure, %d != %d\n", i, len8 );

	fprintf ( log, "  UTF32LE_to_UTF8 done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len32 = 0x80*(1+1+1+2); len8 = 0x80*(1+2+3+4);

	UTF32LE_to_UTF8 ( sU32, 0, sU8, sizeof(sU8), &len32x, &len8x );
	if ( (len32x != 0) || (len8x != 0) )  fprintf ( log, "  *** UTF32LE_to_UTF8 empty input failure, %d -> %d\n", len32x, len8x );
	UTF32LE_to_UTF8 ( sU32, len32, sU8, 0, &len32x, &len8x );
	if ( (len32x != 0) || (len8x != 0) )  fprintf ( log, "  *** UTF32LE_to_UTF8 empty output failure, %d -> %d\n", len32x, len8x );
	UTF32LE_to_UTF8 ( sU32, len32, sU8, 8, &len32x, &len8x );
	if ( (len32x != 3) || (len8x != 6) )  fprintf ( log, "  *** UTF32LE_to_UTF8 partial output failure, %d -> %d\n", len32x, len8x );

	fprintf ( log, "  UTF32LE_to_UTF8 done for empty buffers and buffers ending in mid character\n" );

}	// Test_UTF32_to_UTF8

// =================================================================================================

static void Test_UTF16_to_UTF32 ( FILE * log )
{
	size_t i;
	size_t len16, len32, len16x, len32x;
	UTF32Unit cp, cpLo, cpHi;

	// --------------------------------------
	// Test UTF16BE_to_UTF32BE on good input.
	
	fprintf ( log, "\nTesting UTF16BE_to_UTF32BE on good input\n" );

	// Test inside the BMP, below the surrogates.

	cpLo = 0; cpHi = 0xD800; len16 = len32 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16BE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.

	UTF16BE_to_UTF32BE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16BE_to_UTF32BE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32BE(cp) ) fprintf ( log, "  *** UTF16BE_to_UTF32BE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF16BE_to_UTF32BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len16 = len32 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16BE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.

	UTF16BE_to_UTF32BE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16BE_to_UTF32BE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32BE(cp) ) fprintf ( log, "  *** UTF16BE_to_UTF32BE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF16BE_to_UTF32BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len32 = cpHi-cpLo;
	for ( cp = cpLo, len16 = 0; cp < cpHi; ++cp, len16 += len16x ) CodePoint_to_UTF16BE ( cp, &sU16[len16], 4, &len16x );
	if ( len16 != 2*(cpHi-cpLo) ) fprintf ( log, "  *** CodePoint_to_UTF16BE length failure, %d\n", len16 );
	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.

	UTF16BE_to_UTF32BE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16BE_to_UTF32BE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32BE(cp) ) fprintf ( log, "  *** UTF16BE_to_UTF32BE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF16BE_to_UTF32BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating BMP, beyond BMP.

	len16 = 0x8000*(1+2); len32 = 0x8000*(1+1);
	for ( i = 0, len16 = 0; i < 0x8000; ++i ) {
		CodePoint_to_UTF16BE ( i, &sU16[len16], 8, &len16x );
		len16 += len16x;
		CodePoint_to_UTF16BE ( i+0x10000, &sU16[len16], 8, &len16x );
		len16 += len16x;
	}
	if ( len16 != 0x8000*(1+2) ) fprintf ( log, "  *** CodePoint_to_UTF16BE length failure, %d\n", len16 );
	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.

	UTF16BE_to_UTF32BE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16BE_to_UTF32BE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, len32 = 0; i < 0x8000; ++i ) {
		if ( sU32[len32] != NativeUTF32BE(i) ) fprintf ( log, "  *** UTF16BE_to_UTF32BE failure for U+%.4X\n", i );
		++len32;
		if ( sU32[len32] != NativeUTF32BE(i+0x10000) ) fprintf ( log, "  *** UTF16BE_to_UTF32BE failure for U+%.4X\n", i+0x10000 );
		++len32;
	}

	fprintf ( log, "  UTF16BE_to_UTF32BE done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len16 = 0x8000*(1+2); len32 = 0x8000*(1+1);

	UTF16BE_to_UTF32BE ( sU16, 0, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16x != 0) || (len32x != 0) )  fprintf ( log, "  *** UTF16BE_to_UTF32BE empty input failure, %d -> %d\n", len16x, len32x );
	UTF16BE_to_UTF32BE ( sU16, len16, sU32, 0, &len16x, &len32x );
	if ( (len16x != 0) || (len32x != 0) )  fprintf ( log, "  *** UTF16BE_to_UTF32BE empty output failure, %d -> %d\n", len16x, len32x );
	UTF16BE_to_UTF32BE ( sU16, 5, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16x != 4) || (len32x != 3) )  fprintf ( log, "  *** UTF16BE_to_UTF32BE partial input failure, %d -> %d\n", len16x, len32x );

	fprintf ( log, "  UTF16BE_to_UTF32BE done for empty buffers and buffers ending in mid character\n" );

	// --------------------------------------
	// Test UTF16LE_to_UTF32LE on good input.
	
	fprintf ( log, "\nTesting UTF16LE_to_UTF32LE on good input\n" );

	// Test inside the BMP, below the surrogates.

	cpLo = 0; cpHi = 0xD800; len16 = len32 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16LE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.

	UTF16LE_to_UTF32LE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16LE_to_UTF32LE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32LE(cp) ) fprintf ( log, "  *** UTF16LE_to_UTF32LE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF16LE_to_UTF32LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len16 = len32 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16LE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.

	UTF16LE_to_UTF32LE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16LE_to_UTF32LE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32LE(cp) ) fprintf ( log, "  *** UTF16LE_to_UTF32LE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF16LE_to_UTF32LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len32 = cpHi-cpLo;
	for ( cp = cpLo, len16 = 0; cp < cpHi; ++cp, len16 += len16x ) CodePoint_to_UTF16LE ( cp, &sU16[len16], 4, &len16x );
	if ( len16 != 2*(cpHi-cpLo) ) fprintf ( log, "  *** CodePoint_to_UTF16LE length failure, %d\n", len16 );
	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.

	UTF16LE_to_UTF32LE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16LE_to_UTF32LE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32LE(cp) ) fprintf ( log, "  *** UTF16LE_to_UTF32LE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF16LE_to_UTF32LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating BMP, beyond BMP.

	len16 = 0x8000*(1+2); len32 = 0x8000*(1+1);
	for ( i = 0, len16 = 0; i < 0x8000; ++i ) {
		CodePoint_to_UTF16LE ( i, &sU16[len16], 8, &len16x );
		len16 += len16x;
		CodePoint_to_UTF16LE ( i+0x10000, &sU16[len16], 8, &len16x );
		len16 += len16x;
	}
	if ( len16 != 0x8000*(1+2) ) fprintf ( log, "  *** CodePoint_to_UTF16LE length failure, %d\n", len16 );
	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.

	UTF16LE_to_UTF32LE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16LE_to_UTF32LE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, len32 = 0; i < 0x8000; ++i ) {
		if ( sU32[len32] != NativeUTF32LE(i) ) fprintf ( log, "  *** UTF16LE_to_UTF32LE failure for U+%.4X\n", i );
		++len32;
		if ( sU32[len32] != NativeUTF32LE(i+0x10000) ) fprintf ( log, "  *** UTF16LE_to_UTF32LE failure for U+%.4X\n", i+0x10000 );
		++len32;
	}

	fprintf ( log, "  UTF16LE_to_UTF32LE done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len16 = 0x8000*(1+2); len32 = 0x8000*(1+1);

	UTF16LE_to_UTF32LE ( sU16, 0, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16x != 0) || (len32x != 0) )  fprintf ( log, "  *** UTF16LE_to_UTF32LE empty input failure, %d -> %d\n", len16x, len32x );
	UTF16LE_to_UTF32LE ( sU16, len16, sU32, 0, &len16x, &len32x );
	if ( (len16x != 0) || (len32x != 0) )  fprintf ( log, "  *** UTF16LE_to_UTF32LE empty output failure, %d -> %d\n", len16x, len32x );
	UTF16LE_to_UTF32LE ( sU16, 5, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16x != 4) || (len32x != 3) )  fprintf ( log, "  *** UTF16LE_to_UTF32LE partial input failure, %d -> %d\n", len16x, len32x );

	fprintf ( log, "  UTF16LE_to_UTF32LE done for empty buffers and buffers ending in mid character\n" );

	// --------------------------------------
	// Test UTF16BE_to_UTF32LE on good input.
	
	fprintf ( log, "\nTesting UTF16BE_to_UTF32LE on good input\n" );

	// Test inside the BMP, below the surrogates.

	cpLo = 0; cpHi = 0xD800; len16 = len32 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16BE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.

	UTF16BE_to_UTF32LE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16BE_to_UTF32LE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32LE(cp) ) fprintf ( log, "  *** UTF16BE_to_UTF32LE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF16BE_to_UTF32LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len16 = len32 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16BE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.

	UTF16BE_to_UTF32LE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16BE_to_UTF32LE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32LE(cp) ) fprintf ( log, "  *** UTF16BE_to_UTF32LE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF16BE_to_UTF32LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len32 = cpHi-cpLo;
	for ( cp = cpLo, len16 = 0; cp < cpHi; ++cp, len16 += len16x ) CodePoint_to_UTF16BE ( cp, &sU16[len16], 4, &len16x );
	if ( len16 != 2*(cpHi-cpLo) ) fprintf ( log, "  *** CodePoint_to_UTF16BE length failure, %d\n", len16 );
	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.

	UTF16BE_to_UTF32LE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16BE_to_UTF32LE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32LE(cp) ) fprintf ( log, "  *** UTF16BE_to_UTF32LE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF16BE_to_UTF32LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating BMP, beyond BMP.

	len16 = 0x8000*(1+2); len32 = 0x8000*(1+1);
	for ( i = 0, len16 = 0; i < 0x8000; ++i ) {
		CodePoint_to_UTF16BE ( i, &sU16[len16], 8, &len16x );
		len16 += len16x;
		CodePoint_to_UTF16BE ( i+0x10000, &sU16[len16], 8, &len16x );
		len16 += len16x;
	}
	if ( len16 != 0x8000*(1+2) ) fprintf ( log, "  *** CodePoint_to_UTF16BE length failure, %d\n", len16 );
	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.

	UTF16BE_to_UTF32LE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16BE_to_UTF32LE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, len32 = 0; i < 0x8000; ++i ) {
		if ( sU32[len32] != NativeUTF32LE(i) ) fprintf ( log, "  *** UTF16BE_to_UTF32LE failure for U+%.4X\n", i );
		++len32;
		if ( sU32[len32] != NativeUTF32LE(i+0x10000) ) fprintf ( log, "  *** UTF16BE_to_UTF32LE failure for U+%.4X\n", i+0x10000 );
		++len32;
	}

	fprintf ( log, "  UTF16BE_to_UTF32LE done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len16 = 0x8000*(1+2); len32 = 0x8000*(1+1);

	UTF16BE_to_UTF32LE ( sU16, 0, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16x != 0) || (len32x != 0) )  fprintf ( log, "  *** UTF16BE_to_UTF32LE empty input failure, %d -> %d\n", len16x, len32x );
	UTF16BE_to_UTF32LE ( sU16, len16, sU32, 0, &len16x, &len32x );
	if ( (len16x != 0) || (len32x != 0) )  fprintf ( log, "  *** UTF16BE_to_UTF32LE empty output failure, %d -> %d\n", len16x, len32x );
	UTF16BE_to_UTF32LE ( sU16, 5, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16x != 4) || (len32x != 3) )  fprintf ( log, "  *** UTF16BE_to_UTF32LE partial input failure, %d -> %d\n", len16x, len32x );

	fprintf ( log, "  UTF16BE_to_UTF32LE done for empty buffers and buffers ending in mid character\n" );

	// --------------------------------------
	// Test UTF16LE_to_UTF32BE on good input.
	
	fprintf ( log, "\nTesting UTF16LE_to_UTF32BE on good input\n" );

	// Test inside the BMP, below the surrogates.

	cpLo = 0; cpHi = 0xD800; len16 = len32 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16LE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.

	UTF16LE_to_UTF32BE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16LE_to_UTF32BE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32BE(cp) ) fprintf ( log, "  *** UTF16LE_to_UTF32BE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF16LE_to_UTF32BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len16 = len32 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU16[i] = NativeUTF16LE(UTF16Unit(cp));
	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.

	UTF16LE_to_UTF32BE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16LE_to_UTF32BE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32BE(cp) ) fprintf ( log, "  *** UTF16LE_to_UTF32BE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF16LE_to_UTF32BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len32 = cpHi-cpLo;
	for ( cp = cpLo, len16 = 0; cp < cpHi; ++cp, len16 += len16x ) CodePoint_to_UTF16LE ( cp, &sU16[len16], 4, &len16x );
	if ( len16 != 2*(cpHi-cpLo) ) fprintf ( log, "  *** CodePoint_to_UTF16LE length failure, %d\n", len16 );
	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.

	UTF16LE_to_UTF32BE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16LE_to_UTF32BE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) {
		if ( sU32[i] != NativeUTF32BE(cp) ) fprintf ( log, "  *** UTF16LE_to_UTF32BE failure for U+%.4X\n", cp );
	}

	fprintf ( log, "  UTF16LE_to_UTF32BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating BMP, beyond BMP.

	len16 = 0x8000*(1+2); len32 = 0x8000*(1+1);
	for ( i = 0, len16 = 0; i < 0x8000; ++i ) {
		CodePoint_to_UTF16LE ( i, &sU16[len16], 8, &len16x );
		len16 += len16x;
		CodePoint_to_UTF16LE ( i+0x10000, &sU16[len16], 8, &len16x );
		len16 += len16x;
	}
	if ( len16 != 0x8000*(1+2) ) fprintf ( log, "  *** CodePoint_to_UTF16LE length failure, %d\n", len16 );
	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.

	UTF16LE_to_UTF32BE ( sU16, len16, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16 != len16x) || (len32 != len32x) ) fprintf ( log, "  *** UTF16LE_to_UTF32BE length failure, %d -> %d\n", len16x, len32x );

	sU32[len32] = 0xFFFFFFFF;
	for ( i = 0, len32 = 0; i < 0x8000; ++i ) {
		if ( sU32[len32] != NativeUTF32BE(i) ) fprintf ( log, "  *** UTF16LE_to_UTF32BE failure for U+%.4X\n", i );
		++len32;
		if ( sU32[len32] != NativeUTF32BE(i+0x10000) ) fprintf ( log, "  *** UTF16LE_to_UTF32BE failure for U+%.4X\n", i+0x10000 );
		++len32;
	}

	fprintf ( log, "  UTF16LE_to_UTF32BE done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len16 = 0x8000*(1+2); len32 = 0x8000*(1+1);

	UTF16LE_to_UTF32BE ( sU16, 0, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16x != 0) || (len32x != 0) )  fprintf ( log, "  *** UTF16LE_to_UTF32BE empty input failure, %d -> %d\n", len16x, len32x );
	UTF16LE_to_UTF32BE ( sU16, len16, sU32, 0, &len16x, &len32x );
	if ( (len16x != 0) || (len32x != 0) )  fprintf ( log, "  *** UTF16LE_to_UTF32BE empty output failure, %d -> %d\n", len16x, len32x );
	UTF16LE_to_UTF32BE ( sU16, 5, sU32, sizeof(sU32), &len16x, &len32x );
	if ( (len16x != 4) || (len32x != 3) )  fprintf ( log, "  *** UTF16LE_to_UTF32BE partial input failure, %d -> %d\n", len16x, len32x );

	fprintf ( log, "  UTF16LE_to_UTF32BE done for empty buffers and buffers ending in mid character\n" );

}	// Test_UTF16_to_UTF32

// =================================================================================================

static void Test_UTF32_to_UTF16 ( FILE * log )
{
	size_t i;
	size_t len32, len16, len32x, len16x;
	UTF32Unit cp, cpx, cpLo, cpHi;

	// --------------------------------------
	// Test UTF32BE_to_UTF16BE on good input.
	
	fprintf ( log, "\nTesting UTF32BE_to_UTF16BE on good input\n" );
	
	// Test inside the BMP, below the surrogates.

	cpLo = 0; cpHi = 0xD800; len32 = len16 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32BE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32BE_to_UTF16BE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32BE_to_UTF16BE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16BE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF32BE_to_UTF16BE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF32BE_to_UTF16BE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32BE_to_UTF16BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len32 = len16 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32BE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32BE_to_UTF16BE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32BE_to_UTF16BE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16BE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF32BE_to_UTF16BE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF32BE_to_UTF16BE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32BE_to_UTF16BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len32 = cpHi-cpLo; len16 = (cpHi-cpLo)*2;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32BE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32BE_to_UTF16BE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32BE_to_UTF16BE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16BE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 2) || (cpx != cp) ) fprintf ( log, "  *** UTF32BE_to_UTF16BE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF32BE_to_UTF16BE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32BE_to_UTF16BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating BMP, beyond BMP.

	len32 = 0x8000*(1+1); len16 = 0x8000*(1+2);
	for ( i = 0, len32 = 0; i < 0x8000; ++i ) {
		sU32[len32] = NativeUTF32BE(i);
		++len32;
		sU32[len32] = NativeUTF32BE(i+0x10000);
		++len32;
	}
	sU32[len32] = 0xFFFFFFFF;

	UTF32BE_to_UTF16BE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32BE_to_UTF16BE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, len16 = 0; i < 0x8000; ++i ) {
		CodePoint_from_UTF16BE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != i) ) fprintf ( log, "  *** UTF32BE_to_UTF16BE failure for U+%.4X\n", i );
		len16 += len16x;
		CodePoint_from_UTF16BE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 2) || (cpx != i+0x10000) ) fprintf ( log, "  *** UTF32BE_to_UTF16BE failure for U+%.4X\n", i );
		len16 += len16x;
	}
	if ( len16 != 0x8000*(1+2) ) fprintf ( log, "  *** UTF32BE_to_UTF16BE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32BE_to_UTF16BE done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len32 = 0x8000*(1+1); len16 = 0x8000*(1+2);

	UTF32BE_to_UTF16BE ( sU32, 0, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32x != 0) || (len16x != 0) )  fprintf ( log, "  *** UTF32BE_to_UTF16BE empty input failure, %d -> %d\n", len32x, len16x );
	UTF32BE_to_UTF16BE ( sU32, len32, sU16, 0, &len32x, &len16x );
	if ( (len32x != 0) || (len16x != 0) )  fprintf ( log, "  *** UTF32BE_to_UTF16BE empty output failure, %d -> %d\n", len32x, len16x );
	UTF32BE_to_UTF16BE ( sU32, len32, sU16, 5, &len32x, &len16x );
	if ( (len32x != 3) || (len16x != 4) )  fprintf ( log, "  *** UTF32BE_to_UTF16BE partial output failure, %d -> %d\n", len32x, len16x );

	fprintf ( log, "  UTF32BE_to_UTF16BE done for empty buffers and buffers ending in mid character\n" );

// =================================================================================================

	// --------------------------------------
	// Test UTF32LE_to_UTF16LE on good input.
	
	fprintf ( log, "\nTesting UTF32LE_to_UTF16LE on good input\n" );
	
	// Test inside the BMP, below the surrogates.

	cpLo = 0; cpHi = 0xD800; len32 = len16 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32LE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32LE_to_UTF16LE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32LE_to_UTF16LE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16LE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF32LE_to_UTF16LE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF32LE_to_UTF16LE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32LE_to_UTF16LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len32 = len16 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32LE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32LE_to_UTF16LE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32LE_to_UTF16LE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16LE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF32LE_to_UTF16LE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF32LE_to_UTF16LE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32LE_to_UTF16LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len32 = cpHi-cpLo; len16 = (cpHi-cpLo)*2;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32LE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32LE_to_UTF16LE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32LE_to_UTF16LE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16LE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 2) || (cpx != cp) ) fprintf ( log, "  *** UTF32LE_to_UTF16LE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF32LE_to_UTF16LE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32LE_to_UTF16LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating BMP, beyond BMP.

	len32 = 0x8000*(1+1); len16 = 0x8000*(1+2);
	for ( i = 0, len32 = 0; i < 0x8000; ++i ) {
		sU32[len32] = NativeUTF32LE(i);
		++len32;
		sU32[len32] = NativeUTF32LE(i+0x10000);
		++len32;
	}
	sU32[len32] = 0xFFFFFFFF;

	UTF32LE_to_UTF16LE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32LE_to_UTF16LE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, len16 = 0; i < 0x8000; ++i ) {
		CodePoint_from_UTF16LE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != i) ) fprintf ( log, "  *** UTF32LE_to_UTF16LE failure for U+%.4X\n", i );
		len16 += len16x;
		CodePoint_from_UTF16LE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 2) || (cpx != i+0x10000) ) fprintf ( log, "  *** UTF32LE_to_UTF16LE failure for U+%.4X\n", i );
		len16 += len16x;
	}
	if ( len16 != 0x8000*(1+2) ) fprintf ( log, "  *** UTF32LE_to_UTF16LE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32LE_to_UTF16LE done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len32 = 0x8000*(1+1); len16 = 0x8000*(1+2);

	UTF32LE_to_UTF16LE ( sU32, 0, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32x != 0) || (len16x != 0) )  fprintf ( log, "  *** UTF32LE_to_UTF16LE empty input failure, %d -> %d\n", len32x, len16x );
	UTF32LE_to_UTF16LE ( sU32, len32, sU16, 0, &len32x, &len16x );
	if ( (len32x != 0) || (len16x != 0) )  fprintf ( log, "  *** UTF32LE_to_UTF16LE empty output failure, %d -> %d\n", len32x, len16x );
	UTF32LE_to_UTF16LE ( sU32, len32, sU16, 5, &len32x, &len16x );
	if ( (len32x != 3) || (len16x != 4) )  fprintf ( log, "  *** UTF32LE_to_UTF16LE partial output failure, %d -> %d\n", len32x, len16x );

	fprintf ( log, "  UTF32LE_to_UTF16LE done for empty buffers and buffers ending in mid character\n" );

// =================================================================================================

	// --------------------------------------
	// Test UTF32BE_to_UTF16LE on good input.
	
	fprintf ( log, "\nTesting UTF32BE_to_UTF16LE on good input\n" );
	
	// Test inside the BMP, below the surrogates.

	cpLo = 0; cpHi = 0xD800; len32 = len16 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32BE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32BE_to_UTF16LE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32BE_to_UTF16LE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16LE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF32BE_to_UTF16LE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF32BE_to_UTF16LE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32BE_to_UTF16LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len32 = len16 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32BE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32BE_to_UTF16LE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32BE_to_UTF16LE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16LE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF32BE_to_UTF16LE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF32BE_to_UTF16LE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32BE_to_UTF16LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len32 = cpHi-cpLo; len16 = (cpHi-cpLo)*2;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32BE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32BE_to_UTF16LE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32BE_to_UTF16LE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16LE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 2) || (cpx != cp) ) fprintf ( log, "  *** UTF32BE_to_UTF16LE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF32BE_to_UTF16LE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32BE_to_UTF16LE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating BMP, beyond BMP.

	len32 = 0x8000*(1+1); len16 = 0x8000*(1+2);
	for ( i = 0, len32 = 0; i < 0x8000; ++i ) {
		sU32[len32] = NativeUTF32BE(i);
		++len32;
		sU32[len32] = NativeUTF32BE(i+0x10000);
		++len32;
	}
	sU32[len32] = 0xFFFFFFFF;

	UTF32BE_to_UTF16LE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32BE_to_UTF16LE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16LE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, len16 = 0; i < 0x8000; ++i ) {
		CodePoint_from_UTF16LE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != i) ) fprintf ( log, "  *** UTF32BE_to_UTF16LE failure for U+%.4X\n", i );
		len16 += len16x;
		CodePoint_from_UTF16LE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 2) || (cpx != i+0x10000) ) fprintf ( log, "  *** UTF32BE_to_UTF16LE failure for U+%.4X\n", i );
		len16 += len16x;
	}
	if ( len16 != 0x8000*(1+2) ) fprintf ( log, "  *** UTF32BE_to_UTF16LE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32BE_to_UTF16LE done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len32 = 0x8000*(1+1); len16 = 0x8000*(1+2);

	UTF32BE_to_UTF16LE ( sU32, 0, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32x != 0) || (len16x != 0) )  fprintf ( log, "  *** UTF32BE_to_UTF16LE empty input failure, %d -> %d\n", len32x, len16x );
	UTF32BE_to_UTF16LE ( sU32, len32, sU16, 0, &len32x, &len16x );
	if ( (len32x != 0) || (len16x != 0) )  fprintf ( log, "  *** UTF32BE_to_UTF16LE empty output failure, %d -> %d\n", len32x, len16x );
	UTF32BE_to_UTF16LE ( sU32, len32, sU16, 5, &len32x, &len16x );
	if ( (len32x != 3) || (len16x != 4) )  fprintf ( log, "  *** UTF32BE_to_UTF16LE partial output failure, %d -> %d\n", len32x, len16x );

	fprintf ( log, "  UTF32BE_to_UTF16LE done for empty buffers and buffers ending in mid character\n" );

// =================================================================================================

	// --------------------------------------
	// Test UTF32LE_to_UTF16BE on good input.
	
	fprintf ( log, "\nTesting UTF32LE_to_UTF16BE on good input\n" );
	
	// Test inside the BMP, below the surrogates.

	cpLo = 0; cpHi = 0xD800; len32 = len16 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32LE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32LE_to_UTF16BE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32LE_to_UTF16BE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16BE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF32LE_to_UTF16BE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF32LE_to_UTF16BE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32LE_to_UTF16BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test inside the BMP, above the surrogates.

	cpLo = 0xE000; cpHi = 0x10000; len32 = len16 = cpHi-cpLo;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32LE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32LE_to_UTF16BE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32LE_to_UTF16BE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16BE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != cp) ) fprintf ( log, "  *** UTF32LE_to_UTF16BE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF32LE_to_UTF16BE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32LE_to_UTF16BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test outside the BMP.

	cpLo = 0x10000; cpHi = 0x110000; len32 = cpHi-cpLo; len16 = (cpHi-cpLo)*2;
	for ( i = 0, cp = cpLo; cp < cpHi; ++i, ++cp ) sU32[i] = NativeUTF32LE(cp);
	sU32[len32] = 0xFFFFFFFF;

	UTF32LE_to_UTF16BE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32LE_to_UTF16BE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, cp = cpLo; cp < cpHi; i += len16x, ++cp ) {
		CodePoint_from_UTF16BE ( &sU16[i], 4, &cpx, &len16x );
		if ( (len16x != 2) || (cpx != cp) ) fprintf ( log, "  *** UTF32LE_to_UTF16BE failure for U+%.4X\n", cp );
	}
	if ( i != len16 ) fprintf ( log, "  *** UTF32LE_to_UTF16BE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32LE_to_UTF16BE done for %.4X..%.4X\n", cpLo, cpHi-1 );
	
	// Test alternating BMP, beyond BMP.

	len32 = 0x8000*(1+1); len16 = 0x8000*(1+2);
	for ( i = 0, len32 = 0; i < 0x8000; ++i ) {
		sU32[len32] = NativeUTF32LE(i);
		++len32;
		sU32[len32] = NativeUTF32LE(i+0x10000);
		++len32;
	}
	sU32[len32] = 0xFFFFFFFF;

	UTF32LE_to_UTF16BE ( sU32, len32, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32 != len32x) || (len16 != len16x) ) fprintf ( log, "  *** UTF32LE_to_UTF16BE length failure, %d -> %d\n", len32x, len16x );

	sU16[len16] = NativeUTF16BE(0xDC00);	// Isolated low surrogate.
	for ( i = 0, len16 = 0; i < 0x8000; ++i ) {
		CodePoint_from_UTF16BE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 1) || (cpx != i) ) fprintf ( log, "  *** UTF32LE_to_UTF16BE failure for U+%.4X\n", i );
		len16 += len16x;
		CodePoint_from_UTF16BE ( &sU16[len16], 4, &cpx, &len16x );
		if ( (len16x != 2) || (cpx != i+0x10000) ) fprintf ( log, "  *** UTF32LE_to_UTF16BE failure for U+%.4X\n", i );
		len16 += len16x;
	}
	if ( len16 != 0x8000*(1+2) ) fprintf ( log, "  *** UTF32LE_to_UTF16BE consume failure, %d != %d\n", i, len16 );

	fprintf ( log, "  UTF32LE_to_UTF16BE done for mixed values\n" );
	
	// Test empty buffers and buffers ending in mid character.

	len32 = 0x8000*(1+1); len16 = 0x8000*(1+2);

	UTF32LE_to_UTF16BE ( sU32, 0, sU16, sizeof(sU16), &len32x, &len16x );
	if ( (len32x != 0) || (len16x != 0) )  fprintf ( log, "  *** UTF32LE_to_UTF16BE empty input failure, %d -> %d\n", len32x, len16x );
	UTF32LE_to_UTF16BE ( sU32, len32, sU16, 0, &len32x, &len16x );
	if ( (len32x != 0) || (len16x != 0) )  fprintf ( log, "  *** UTF32LE_to_UTF16BE empty output failure, %d -> %d\n", len32x, len16x );
	UTF32LE_to_UTF16BE ( sU32, len32, sU16, 5, &len32x, &len16x );
	if ( (len32x != 3) || (len16x != 4) )  fprintf ( log, "  *** UTF32LE_to_UTF16BE partial output failure, %d -> %d\n", len32x, len16x );

	fprintf ( log, "  UTF32LE_to_UTF16BE done for empty buffers and buffers ending in mid character\n" );

}	// Test_UTF32_to_UTF16

// =================================================================================================

static void DoTest ( FILE * log )
{
	InitializeUnicodeConversions();

	Test_SwappingPrimitives ( log );

	Test_CodePoint_to_UTF8 ( log );
	Test_CodePoint_from_UTF8 ( log );
	
	Test_CodePoint_to_UTF16 ( log );
	Test_CodePoint_from_UTF16 ( log );
	
	Test_UTF8_to_UTF16 ( log );
	Test_UTF8_to_UTF32 ( log );
	
	Test_UTF16_to_UTF8 ( log );
	Test_UTF32_to_UTF8 ( log );

	Test_UTF16_to_UTF32 ( log );
	Test_UTF32_to_UTF16 ( log );

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
	sprintf ( buffer, "// Starting test for Unicode conversion correctness, %s", ctime ( &now ) );

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
	sprintf ( buffer, "// Finished test for Unicode conversion correctness, %s", ctime ( &now ) );

	fprintf ( log, "\n// " );
	for ( size_t i = 4; i < strlen(buffer); ++i ) fprintf ( log, "=" );
	fprintf ( log, "\n%s\n", buffer );

	fclose ( log );
	return 0;

}
