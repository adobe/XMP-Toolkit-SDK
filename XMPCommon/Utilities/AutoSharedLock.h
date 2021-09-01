#ifndef __AutoSharedLock_h__
#define __AutoSharedLock_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/XMPCommonFwdDeclarations_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	class AutoSharedLock {
	public:
		AutoSharedLock( const spISharedMutex & mutex, bool exclusiveLock = false );
		~AutoSharedLock();
		void Release();

	private:
		spISharedMutex			mMutex;
		bool					mExclusiveLock;
	};

}
#endif  // __AutoSharedLock_h__
