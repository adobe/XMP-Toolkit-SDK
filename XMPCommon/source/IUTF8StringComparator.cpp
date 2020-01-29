// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#include "XMPCommon/Utilities/IUTF8StringComparator.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include <algorithm>
#include <cstring>
namespace XMP_COMPONENT_INT_NAMESPACE {

	bool IUTF8StringComparator::operator()( const spcIUTF8String & left, const spcIUTF8String & right ) const {
		sizet str1Size = left->size(), str2Size = right->size();
		int result = strncmp( left->c_str(), right->c_str(), std::min( str1Size, str2Size ) );
		if ( result == 0 && str1Size != str2Size )
			return str1Size < str2Size;
		if ( result < 0 )
			return true;
		return false;
	}

}
