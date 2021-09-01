#ifndef XMPCoreDefines_h__
#define XMPCoreDefines_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

// =================================================================================================
// XMPCoreDefines.h - Common Defines for XMP Core component
// ================================================================
//
// This header defines common definitions to be used in XMP Core component.
//
// =================================================================================================

// =================================================================================================
// All Platform Settings
// ===========================
#include "XMPCommon/XMPCommonDefines.h"

#ifndef ENABLE_CPP_DOM_MODEL
// =================================================================================================
// Macintosh Specific Settings
// ===========================
#if XMP_MacBuild
	#define ENABLE_CPP_DOM_MODEL 0
#endif

// =================================================================================================
// IOS Specific Settings
// ===========================
#if XMP_iOSBuild
	#define ENABLE_CPP_DOM_MODEL 0
#endif

// =================================================================================================
// Windows Specific Settings
// =========================
#if XMP_WinBuild
	#define ENABLE_CPP_DOM_MODEL 0
#endif

// =================================================================================================
// UNIX Specific Settings
// ======================
#if XMP_UNIXBuild
#define ENABLE_CPP_DOM_MODEL 0
#endif

// =================================================================================================
// Android Specific Settings
// ======================
#if XMP_AndroidBuild
#define ENABLE_CPP_DOM_MODEL 0
#endif

#endif // ENABLE_CPP_DOM_MODEL

#ifndef ENABLE_CPP_DOM_MODEL
	#define ENABLE_CPP_DOM_MODEL 0
#endif

#if ENABLE_CPP_DOM_MODEL

	#if SOURCE_COMPILING_XMP_ALL
		#define SOURCE_COMPILING_XMPCORE_LIB 1
	#endif

	#ifndef SOURCE_COMPILING_XMPCORE_LIB
		#define SOURCE_COMPILING_XMPCORE_LIB 0
	#endif

	#ifndef BUILDING_XMPCORE_LIB
		#define  BUILDING_XMPCORE_LIB 0
	#endif

	#if BUILDING_XMPCORE_LIB
		#if !BUILDING_XMPCORE_AS_STATIC && !BUILDING_XMPCORE_AS_DYNAMIC
			#error "Define either BUILDING_XMPCORE_AS_STATIC as 1 or BUILDING_XMPCORE_AS_DYNAMIC as 1"
		#endif
	#endif

	#ifndef LINKING_XMPCORE_LIB
		#define LINKING_XMPCORE_LIB 1
	#endif

	namespace AdobeXMPCore {};
#endif  // ENABLE_CPP_DOM_MODEL

#endif // XMPCoreDefines_h__
