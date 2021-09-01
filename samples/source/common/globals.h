// =================================================================================================
// Copyright 2007 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
//
// =================================================================================================

#ifndef XMPQE_GLOBALS_H
#define XMPQE_GLOBALS_H

	#include <string>

	#include <cstdlib>	// Various libraries needed for gcc 4.x builds.
	#include <cstring>
	#include <cstdio>

	//sanity check platform/endianess
	#if !defined(WIN_ENV) && !defined(MAC_ENV) && !defined(UNIX_ENV) && !defined(IOS_ENV) && !defined(ANDROID_ENV)
		#error "XMP environment error - must define one of MAC_ENV, WIN_ENV, UNIX_ENV or IOS_ENV or ANDROID_ENV"
	#endif

	#ifdef WIN_ENV
		#define XMPQE_LITTLE_ENDIAN 1
	#elif (defined(MAC_ENV) || defined(UNIX_ENV) || defined(IOS_ENV)) || defined(ANDROID_ENV)
		#if __BIG_ENDIAN__
			#define XMPQE_BIG_ENDIAN 1
		#elif __LITTLE_ENDIAN__
			#define XMPQE_LITTLE_ENDIAN 1
		#else
			#error "Neither __BIG_ENDIAN__ nor __LITTLE_ENDIAN__ is set"
		#endif
	#else
		#error "Unknown build environment, neither WIN_ENV nor MAC_ENV nor UNIX_ENV nor ANDROID_ENV"
	#endif

	const static unsigned int XMPQE_BUFFERSIZE=4096; //should do for all my buffer output, but you never now (stdarg dilemma)



	const char OMNI_CSTRING[]={0x41,0xE4,0xB8,0x80,0x42,0xE4,0xBA,0x8C,0x43,0xC3,0x96,0x44,0xF0,0x90,0x81,0x91,0x45,'\0'};
	const char BOM_CSTRING[]={0xEF,0xBB,0xBF,'\0'}; // nb: forgetting the '\0' is a very evil mistake.
	const std::string OMNI_STRING(OMNI_CSTRING);
	// if plain utf8 conversion, mac/win local encoding is a different story...
	const std::string OMNI_BUGINESE("A<E4 B8 80>B<E4 BA 8C>C<C3 96>D<F0 90 81 91>E");
	// if utf16LE
	const std::string OMNI_BUGINESE_16LE("A<00 4E>B<8C 4E>C<D6 00>D<00 D8 51 DC>E");
	// if utf16 BE
	const std::string OMNI_BUGINESE_16BE("A<4E 00>B<4E 8C>C<00 D6>D<D8 00 DC 51>E");

	// degraded version of omni-strings
	// (sometimes useful for asserts after roundtrips, for setting values see OMNI_STRING in TestCase.h)
	// ==> filed bug 2302354 on this issue.
	#if WIN_ENV
		// a *wrongly* degraded omni-string (non-BMP-char to ??)
		const std::string DEG_OMNI_BUGINESE("A?B?C<C3 96>D??E");
		// ditto albeit MacRoman encoding
		const std::string MAC_OMNI_BUGINESE("A?B?C<85>D??E");
	#else
		// a *correctly* degraded omni-string (non-BMP-char to ?)
		const std::string DEG_OMNI_BUGINESE("A?B?C<C3 96>D?E");
		// ditto albeit MacRoman encoding
		const std::string MAC_OMNI_BUGINESE("A?B?C<85>D?E");
	#endif
	//  -> #issue# the non-BMP character in OMNI_STRING between D and E gets converted
	//      into two question marks (wrong) , not into one (correct)

	const char AEOEUE_WIN_LOCAL_CSTRING[]={0xC4,0xD6,0xDC,'\0'};
	const char AEOEUE_MAC_LOCAL_CSTRING[]={0x80,0x85,0x86,'\0'};
	const char AEOEUE_UTF8_CSTRING[]={0xC3, 0x84, 0xC3, 0x96, 0xC3, 0x9C,'\0'};
	const std::string AEOEUE_UTF8(AEOEUE_UTF8_CSTRING);
	const std::string AEOEUE_UTF8_BUGINESE("<C3 84 C3 96 C3 9C>");
	const std::string AEOEUE_UTF8_BUGINESE_EVEN ( "<C3 84 C3 96 C3 9C 00>" );

	const std::string AEOEUE_WIN_LOCAL_BUGINESE("<C4 D6 DC>");
	const std::string AEOEUE_WIN_LOCAL_BUGINESE_EVEN ( "<C4 D6 DC 00>" );
	const std::string AEOEUE_MAC_LOCAL_BUGINESE("<80 85 86>");

	const std::string AEOEUE_WIN_MOJIBAKE_BUGINESE("<E2 82 AC E2 80 A6 E2 80 A0>");
	const std::string AEOEUE_MAC_MOJIBAKE_BUGINESE("<C6 92 C3 B7 E2 80 B9>");
	const std::string AEOEUE_LATIN1_MOJIBAKE_BUGINESE("<C2 80 C2 85 C2 86>");

	#if MAC_ENV || IOS_ENV
		const std::string AEOEUE_LOCAL = std::string(AEOEUE_MAC_LOCAL_CSTRING);
		const std::string AEOEUE_WIN_LOCAL_TO_UTF8 = AEOEUE_MAC_MOJIBAKE_BUGINESE;
		const std::string AEOEUE_MAC_LOCAL_TO_UTF8 = AEOEUE_UTF8_BUGINESE;
	#elif WIN_ENV
		const std::string AEOEUE_LOCAL = std::string(AEOEUE_WIN_LOCAL_CSTRING);
		const std::string AEOEUE_WIN_LOCAL_TO_UTF8 = AEOEUE_UTF8_BUGINESE;
		const std::string AEOEUE_MAC_LOCAL_TO_UTF8 = AEOEUE_WIN_MOJIBAKE_BUGINESE;
	#else
		// windows local encoding will work for UNIX (Latin1), but mac will result MOJIBAKE
		const std::string AEOEUE_WIN_LOCAL_TO_UTF8 = AEOEUE_UTF8_BUGINESE;
		const std::string AEOEUE_MAC_LOCAL_TO_UTF8 = AEOEUE_LATIN1_MOJIBAKE_BUGINESE;
	#endif

#endif // XMPQE_GLOBALS_H

