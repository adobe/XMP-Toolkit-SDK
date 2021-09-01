// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/Utilities/AutoSharedLock.h"
#include "XMPCommon/Interfaces/ISharedMutex.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCommon/XMPCommonErrorCodes_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {
	AutoSharedLock::AutoSharedLock( const spISharedMutex & mutex, bool exclusiveLock /*= false */ )
		: mMutex( mutex )
		, mExclusiveLock( exclusiveLock )
	{
		if ( mMutex ) {
			eMultiThreadingErrorCode er;
			if ( mExclusiveLock )
				er = mMutex->Lock();
			else
				er = mMutex->LockShared();
			if ( er != kMTECNone ) {
				NOTIFY_ERROR( IError_v1::kEDMultiThreading, er, "Unable to lock the mutex", IError_v1::kESProcessFatal, false, false );
			}
		}
	}

	AutoSharedLock::~AutoSharedLock() {
		Release();
	}

	void AutoSharedLock::Release() {
		if ( mMutex ) {
			eMultiThreadingErrorCode er;
			if ( mExclusiveLock )
				er = mMutex->Unlock();
			else
				er = mMutex->UnlockShared();
			if ( er != kMTECNone ) {
				NOTIFY_ERROR( IError_v1::kEDMultiThreading, er, "Unable to lock the mutex", IError_v1::kESProcessFatal, false, false );
			}
		}
	}

}
