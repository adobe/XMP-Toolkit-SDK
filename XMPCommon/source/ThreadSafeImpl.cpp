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

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCommon/ImplHeaders/ThreadSafeImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCommon/Interfaces/ISharedMutex.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	// All virtual functions
	
	// All static functions of _I class.

	void APICALL ThreadSafeImpl::ShareMutex( const spISharedMutex & mutex ) {
		mSharedMutex = mutex;
	}

	void APICALL ThreadSafeImpl::UnShareMutex() {
		if ( mSharedMutex ) {
			mSharedMutex = ISharedMutex::CreateSharedMutex();
		} else {
			mSharedMutex.reset();
		}
	}

	void APICALL ThreadSafeImpl::EnableThreadSafety() const __NOTHROW__ {
		if ( !mSharedMutex ) {
			mSharedMutex = ISharedMutex::CreateSharedMutex();
		}
	}

	void APICALL ThreadSafeImpl::DisableThreadSafety() const __NOTHROW__ {
		if ( mSharedMutex )
			mSharedMutex.reset();
	}

	bool APICALL ThreadSafeImpl::IsThreadSafe() const {
		if ( mSharedMutex ) return true;
		return false;
	}

}
