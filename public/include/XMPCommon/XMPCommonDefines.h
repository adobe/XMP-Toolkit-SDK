#ifndef __XMPCommonDefines_h__
#define __XMPCommonDefines_h__ 1
// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

// =================================================================================================
// XMP_CommonDefines.h - Common Defines across all the XMP Components
// ================================================================
//
// This header defines common definitions to be used across all the XMP Components.
//
// =================================================================================================

// =================================================================================================
// All Platform Settings
// ===========================
#include "XMP_Environment.h"
#if !XMP_WinBuild
    #include <ciso646>
#endif
	// =================================================================================================
	// Macintosh Specific Settings
	// ===========================
	#if XMP_MacBuild
		#define SUPPORT_STD_ATOMIC_IMPLEMENTATION 1
		#ifdef _LIBCPP_VERSION
			#define SUPPORT_SHARED_POINTERS_IN_TR1 0
			#define SUPPORT_SHARED_POINTERS_IN_STD 1
		#else
			#define SUPPORT_SHARED_POINTERS_IN_TR1 1
			#define SUPPORT_SHARED_POINTERS_IN_STD 0
		#endif
		#define SUPPORT_SHARED_POINTERS_WITH_ALLOCATORS 0
		#define BAD_EXCEPTION_SUPPORT_STRINGS 0
		#define VECTOR_SUPPORT_CONST_ITERATOR_FUNCTIONS 0
		#define SUPPORT_VARIADIC_TEMPLATES 0
		#define libcppNULL 0


    #endif

    #if XMP_AndroidBuild
    #define SUPPORT_STD_ATOMIC_IMPLEMENTATION 0
    #ifdef _LIBCPP_VERSION
        #define SUPPORT_SHARED_POINTERS_IN_TR1 0
        #define SUPPORT_SHARED_POINTERS_IN_STD 1
        #define libcppNULL nullptr
    #else
        #define SUPPORT_SHARED_POINTERS_IN_TR1 1
        #define SUPPORT_SHARED_POINTERS_IN_STD 0
        #define libcppNULL NULL
    #endif
    #define SUPPORT_SHARED_POINTERS_WITH_ALLOCATORS 0
    #define BAD_EXCEPTION_SUPPORT_STRINGS 0
    #define VECTOR_SUPPORT_CONST_ITERATOR_FUNCTIONS 0
    #define SUPPORT_VARIADIC_TEMPLATES 0


    #endif

	// =================================================================================================
	// IOS Specific Settings
	// ===========================
	#if XMP_iOSBuild
		#define SUPPORT_STD_ATOMIC_IMPLEMENTATION 1
		#ifdef _LIBCPP_VERSION
			#define SUPPORT_SHARED_POINTERS_IN_TR1 0
			#define SUPPORT_SHARED_POINTERS_IN_STD 1
		#else
			#define SUPPORT_SHARED_POINTERS_IN_TR1 1
			#define SUPPORT_SHARED_POINTERS_IN_STD 0
		#endif
		#define SUPPORT_SHARED_POINTERS_WITH_ALLOCATORS 0
		#define BAD_EXCEPTION_SUPPORT_STRINGS 0
		#define VECTOR_SUPPORT_CONST_ITERATOR_FUNCTIONS 0
		#define SUPPORT_VARIADIC_TEMPLATES 0
		#define libcppNULL 0
	#endif

	// =================================================================================================
	// Windows Specific Settings
	// =========================
	#if XMP_WinBuild
		#define SUPPORT_SHARED_POINTERS_WITH_ALLOCATORS 1
		#if _MSC_VER <= 1600
			#define SUPPORT_STD_ATOMIC_IMPLEMENTATION 0
			#define SUPPORT_SHARED_POINTERS_IN_TR1 1
			#define SUPPORT_SHARED_POINTERS_IN_STD 0
		#else
			#define SUPPORT_STD_ATOMIC_IMPLEMENTATION 1
			#define SUPPORT_SHARED_POINTERS_IN_TR1 0
			#define SUPPORT_SHARED_POINTERS_IN_STD 1
		#endif
		#define BAD_EXCEPTION_SUPPORT_STRINGS 1
		#define VECTOR_SUPPORT_CONST_ITERATOR_FUNCTIONS 1
		#define libcppNULL 0
	#endif

	// =================================================================================================
	// UNIX Specific Settings
	// ======================
	#if XMP_UNIXBuild
		# if __clang__
			#define SUPPORT_STD_ATOMIC_IMPLEMENTATION 1
		#else	
			#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
			#if GCC_VERSION >= 40800
				#define SUPPORT_STD_ATOMIC_IMPLEMENTATION 1
			#else
				#define REQ_FRIEND_CLASS_DECLARATION() template<typename _Ptr, std::_Lock_policy _Lp> friend class std::_Sp_counted_ptr;
				#define SUPPORT_STD_ATOMIC_IMPLEMENTATION 0
			#endif
		#endif	


		#define SUPPORT_SHARED_POINTERS_IN_TR1 0
		#define SUPPORT_SHARED_POINTERS_IN_STD 1
		#define SUPPORT_SHARED_POINTERS_WITH_ALLOCATORS 0
		#define BAD_EXCEPTION_SUPPORT_STRINGS 0
		#define VECTOR_SUPPORT_CONST_ITERATOR_FUNCTIONS 1
		#define SUPPORT_DYNAMIC_CAST_OPTIMIZATION 0
		#define SUPPORT_VARIADIC_TEMPLATES 0
		#define libcppNULL 0
	#endif
	#ifndef SUPPORT_VARIADIC_TEMPLATES
		#define SUPPORT_VARIADIC_TEMPLATES 1
	#endif

	#ifndef REQ_FRIEND_CLASS_DECLARATION
		#define REQ_FRIEND_CLASS_DECLARATION()
	#endif

	#define JOIN_CLASSNAME_WITH_VERSION_NUMBER_INT(x,y) x ## _v ## y
	#define JOIN_CLASSNAME_WITH_VERSION_NUMBER(x,y)  JOIN_CLASSNAME_WITH_VERSION_NUMBER_INT(x,y)
	#define BASE_CLASS(classNameWithoutVersionNumber, versionNumber) JOIN_CLASSNAME_WITH_VERSION_NUMBER(classNameWithoutVersionNumber, versionNumber)
	#define EXPAND_MACRO(X) X
	#define QUOTEME2(X) #X
	#define QUOTEME(X) QUOTEME2(X)

	#define __NOTHROW__ throw()

	#if SOURCE_COMPILING_XMP_ALL || SOURCE_COMPILING_XMPCORE_LIB || SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB || SOURCE_COMPILING_XMPEXTENSIONS_LIB
		#define SOURCE_COMPILING_XMPCOMMON_LIB 1
	#else
		#define SOURCE_COMPILING_XMPCOMMON_LIB 0
	#endif

	#ifndef BUILDING_XMPCOMMON_LIB
		#define BUILDING_XMPCOMMON_LIB 0
	#endif

	#if BUILDING_XMPCOMMON_LIB
		#if !BUILDING_XMPCOMMON_AS_STATIC && !BUILDING_XMPCOMMON_AS_DYNAMIC
			#error "Define either BUILDING_XMPCOMMON_AS_STATIC as 1 or BUILDING_XMPCOMMON_AS_DYNAMIC as 1"
		#endif
	#endif

	#ifndef __XMP_Const_h__
		#include "XMP_Const.h"
	#endif

	namespace AdobeXMPCommon {

		typedef XMP_Int64	int64;
		typedef XMP_Uns64	uint64;
		typedef XMP_Int32	int32;
		typedef XMP_Uns32	uint32;
	#if !XMP_64
		typedef uint32		sizet;
	#else
		typedef uint64		sizet;
	#endif

		const sizet kMaxSize						( ( sizet ) -1 );
		const sizet npos							( kMaxSize );

		// force an enum type to be represented in 32 bits
		static const uint32 kMaxEnumValue			( Max_XMP_Uns32 );
		static const uint32 kAllBits				( 0xFFFFFFFF );

		// unique ids for the interfaces defined in the namespace
		static const uint64 kIErrorID					( 0x6e4572726f722020 /* nError   */ );
		static const uint64 kIUTF8StringID				( 0x6e55544638537472 /* nUTF8Str */ );
		static const uint64 kIObjectFactoryID			( 0x6e4f626a46616374 /* nObjFact */ );
		static const uint64 kIErrorNotifierID			( 0x6e4572724e6f7466 /* nErrNotf */ );
		static const uint64 kIConfigurationManagerID	( 0x6e436f6e664d6772 /* nConfMgr */ );
	}  // namespace AdobeXMPCommon

#endif  // __XMPCommonDefines_h__
