#ifndef XMPCommonDefines_I_h__
#define XMPCommonDefines_I_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/XMPCommonDefines.h"

// =================================================================================================
// All Platform Settings
// ===========================

#ifndef XMP_COMPONENT_INT_NAMESPACE
	#error "Please define XMP_COMPONENT_INT_NAMESPACE"
#endif

namespace AdobeXMPCommon_Int {
	using namespace AdobeXMPCommon;
	const uint32 kInternalInterfaceVersionNumber( 0xFFFFFFFF );
}

namespace XMP_COMPONENT_INT_NAMESPACE {
	using namespace AdobeXMPCommon;
	using namespace AdobeXMPCommon_Int;
}

// =================================================================================================
// Macintosh Specific Settings
// ===========================

// =================================================================================================
// Windows Specific Settings
// =========================

// =================================================================================================
// UNIX Specific Settings
// ======================

// =================================================================================================
// IOS Specific Settings
// ===========================


#endif  // XMPCommonDefines_I_h__
