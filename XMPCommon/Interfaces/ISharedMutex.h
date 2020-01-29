#ifndef ISharedMutex_h__
#define ISharedMutex_h__ 1

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

#include "XMPCommon/XMPCommonFwdDeclarations_I.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject_I.h"
#include "XMPCommon/XMPCommonErrorCodes_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	//!
	//! Internal interface that represents a mutex object.
	//! \details Provides all the functions to lock/unlock the mutex.
	//!
	class XMP_PUBLIC ISharedMutex
		: public virtual ISharedObject
	{
	public:
		//!
		//! Locks the mutex in exclusive mode.
		//! \return a value of type eMutexErrorCode indicating the status of operation.
		//!
		virtual eMultiThreadingErrorCode APICALL Lock() __NOTHROW__ = 0;

		//!
		//! Tries to lock the mutex in exclusive mode.
		//! \return a value of type eMutexErrorCode indicating the status of operation.
		//!
		virtual eMultiThreadingErrorCode APICALL TryLock() __NOTHROW__ = 0;

		//!
		//! Unlocks the mutex in exclusive mode.
		//! \return a value of type eMutexErrorCode indicating the status of operation.
		//!
		virtual eMultiThreadingErrorCode APICALL Unlock() __NOTHROW__ = 0;

		//!
		//! Locks the mutex in non exclusive mode.
		//! \return a value of type eMutexErrorCode indicating the status of operation.
		//!
		virtual eMultiThreadingErrorCode APICALL LockShared() __NOTHROW__ = 0;

		//!
		//! Tries to lock the mutex in non exclusive mode.
		//! \return a value of type eMutexErrorCode indicating the status of operation.
		//!
		virtual eMultiThreadingErrorCode APICALL TryLockShared() __NOTHROW__ = 0;

		//!
		//! Unlocks the mutex in non exclusive mode.
		//! \return a value of type eMutexErrorCode indicating the status of operation.
		//!
		virtual eMultiThreadingErrorCode APICALL UnlockShared() __NOTHROW__ = 0;

		// factory methods
		//!
		//! Creates a mutex.
		//! \return a shared pointer to #ISharedMutex.
		//!
		static spISharedMutex CreateSharedMutex();

	protected:
		//!
		//! Destructor
		//! 
		virtual ~ISharedMutex() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif  // ISharedMutex_h__

