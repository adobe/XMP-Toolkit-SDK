#ifndef __UTF8String_h__
#define __UTF8String_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include <string>
#include <sstream>
#include "XMPCommon/XMPCommonDefines_I.h"
#include "XMPCommon/Utilities/TAllocator.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	typedef std::basic_string< char, std::char_traits< char >, TAllocator< char > > UTF8String;
	typedef std::string UTF8StringUnmanaged;

	typedef std::basic_stringstream< char, std::char_traits< char >, TAllocator< char > > UTF8StringStream;
	typedef std::stringstream UTF8StringStreamUnmanaged;
}

#endif  // __UTF8String_h__
