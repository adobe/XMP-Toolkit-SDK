// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCommon/ImplHeaders/SharedMutexImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Utilities/TSmartPointers_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	eMultiThreadingErrorCode SharedMutexImpl::Lock() __NOTHROW__ {
		return kMTECNone;
	}

	eMultiThreadingErrorCode SharedMutexImpl::TryLock() __NOTHROW__ {
		return kMTECNone;
	}

	eMultiThreadingErrorCode SharedMutexImpl::Unlock() __NOTHROW__ {
		return kMTECNone;
	}

	eMultiThreadingErrorCode SharedMutexImpl::LockShared() __NOTHROW__ {
		return kMTECNone;
	}

	eMultiThreadingErrorCode SharedMutexImpl::TryLockShared() __NOTHROW__ {
		return kMTECNone;
	}

	eMultiThreadingErrorCode SharedMutexImpl::UnlockShared() __NOTHROW__ {
		return kMTECNone;
	}

	spISharedMutex ISharedMutex::CreateSharedMutex() {
		return MakeUncheckedSharedPointer( new SharedMutexImpl(), __FILE__, __LINE__ );
	}
}

