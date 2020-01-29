#ifndef __TAtomicTypes_Android_h__
#define __TAtomicTypes_Android_h__ 1


// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#if XMP_AndroidBuild

#ifndef __TAtomicTypes_h__
	#error	"This file is supposed to be included from TAtomicTypes and not directly"
#endif

#include <atomic>

namespace AdobeXMPCommon {
    typedef std::atomic<size_t> atomic_sizet;
}
		/*#include <stdatomic.h>
		namespace AdobeXMPCommon {
			typedef atomic_size_t	atomic_sizet;
		}*/

#define NOT_DEFINED_ATOMIC_SIZE_T 0
#endif  // XMP_AndroidBuild

#endif  // __TAtomicTypes_Android_h__