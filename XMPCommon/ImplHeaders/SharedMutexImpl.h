#ifndef __SharedMutexImpl_h__
#define __SharedMutexImpl_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#if !(IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED)
	#error "Not adhering to design constraints"
	// this file should only be included from its own cpp file
#endif

#include "XMPCommon/Interfaces/ISharedMutex.h"
#include "XMPCommon/BaseClasses/MemoryManagedObject.h"
#include "XMPCommon/ImplHeaders/SharedObjectImpl.h"

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace XMP_COMPONENT_INT_NAMESPACE {

	class SharedMutexImpl
		: public virtual ISharedMutex
		, public virtual SharedObjectImpl
		, public virtual MemoryManagedObject
	{
	public:
		virtual eMultiThreadingErrorCode APICALL Lock() __NOTHROW__;
		virtual eMultiThreadingErrorCode APICALL TryLock() __NOTHROW__;
		virtual eMultiThreadingErrorCode APICALL Unlock() __NOTHROW__;

		virtual eMultiThreadingErrorCode APICALL LockShared() __NOTHROW__;
		virtual eMultiThreadingErrorCode APICALL TryLockShared() __NOTHROW__;
		virtual eMultiThreadingErrorCode APICALL UnlockShared() __NOTHROW__;

		virtual ~SharedMutexImpl() __NOTHROW__ { }
	};

}

#if XMP_WinBuild
	#pragma warning( pop )
#endif

#endif  // __SharedMutexImpl_h__
