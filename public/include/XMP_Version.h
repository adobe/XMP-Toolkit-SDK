#ifndef __XMP_Version_h__
#define __XMP_Version_h__ 1

/* --------------------------------------------------------------------------------------------- */
/* ** IMPORTANT ** This file must be usable by strict ANSI C compilers. No "//" comments, etc.   */
/* --------------------------------------------------------------------------------------------- */

/*
// =================================================================================================
// Copyright 2002 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================
*/

/* ============================================================================================= */
/**
XMP Toolkit Version Information

Version information for the XMP toolkit is stored in the executable and available through a runtime
call, <tt>SXMPMeta::GetVersionInfo</tt>. In addition a static version number is defined in this
header. The information in the executable or returned by <tt>SXMPMeta::GetVersionInfo</tt> is about
the implementation internals, it is runtime version information. The values defined in this header
describe the version of the API used at client compile time. They do not necessarily relate to the
runtime version.

Important: Do not display the static values defined here to users as the version of XMP in use. Do
not base runtime decisions on just this static version. It is OK to compare the static and runtime
versions.

*/
/* ============================================================================================= */

#define XMPCORE_API_VERSION_MAJOR  6
#define XMPCORE_API_VERSION_MINOR  0
#define XMPCORE_API_VERSION_MICRO  0

#define XMPCORE_API_VERSION 6.0.0
#define XMPCORE_API_VERSION_STRING "6.0.0"

#define XMPFILES_API_VERSION_MAJOR  6
#define XMPFILES_API_VERSION_MINOR  0
#define XMPFILES_API_VERSION_MICRO  0

#define XMPFILES_API_VERSION 6.0.0
#define XMPFILES_API_VERSION_STRING "6.0.0"

/* ============================================================================================= */

#endif /* __XMP_Version_h__ */
