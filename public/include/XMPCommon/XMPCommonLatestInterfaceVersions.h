#ifndef XMPCommonLatestInterfaceVersions_h__
#define XMPCommonLatestInterfaceVersions_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

//!
//!@brief  Macro to include a client file through with client can control the interface versions he wants to stick with
//!if not the latest ones.
//!
#if !SOURCE_COMPILING_XMPCOMMON_LIB
	#ifdef XMPCOMMON_CLIENT_VERSION_NUMBER_FILE
		#include QUOTEME(XMPCOMMON_CLIENT_VERSION_NUMBER_FILE)
	#endif
#endif

#ifndef IOBJECTFACTORY_VERSION
	#define IOBJECTFACTORY_VERSION 1
#endif

#ifndef IERROR_VERSION
	#define IERROR_VERSION 1
#endif

#ifndef IUTF8STRING_VERSION
	#define IUTF8STRING_VERSION 1
#endif

#ifndef IMEMORYALLOCATOR_VERSION
	#define IMEMORYALLOCATOR_VERSION 1
#endif

#ifndef IERRORNOTIFIER_VERSION
	#define IERRORNOTIFIER_VERSION 1
#endif

#ifndef ICONFIGURATIONMANAGER_VERSION
	#define ICONFIGURATIONMANAGER_VERSION 1
#endif

#ifndef ICONFIGURABLE_VERSION
	#define ICONFIGURABLE_VERSION 1
#endif

#endif  // XMPCommonLatestInterfaceVersions_h__
