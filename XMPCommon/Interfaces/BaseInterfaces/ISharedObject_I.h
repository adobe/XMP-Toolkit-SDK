#ifndef __ISharedObject_I_h__
#define __ISharedObject_I_h__ 1

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

#include "XMPCommon/XMPCommonDefines_I.h"
#include "XMPCommon/XMPCommonFwdDeclarations_I.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"

namespace AdobeXMPCommon_Int {

	//!
	//! Internal interface that serves as the base interface of all the internal interfaces inherited from
	//! externally exposed interfaces.
	//! This allows all interfaces to be returned as raw pointers through wrapper functions so that internal
	//! object is not deleted as soon as the internal smart pointer goes out of scope.
	//! \note Any class/interface which inherits from this class needs to provide implementation for
	//! AcquireInternal pure virtual function.
	//!
	class ISharedObject_I
		: public virtual ISharedObject
	{
	public:

		//!
		//! It indicates that the pointer owned by the internal smart pointer is required to extend beyond the life span
		//! of smart pointer. This is generally the case where we are returning raw pointer from an internal shared pointer.
		//! Called internally by the library to pass raw pointers across shared libraries from shared pointers.
		//!
		virtual void APICALL AcquireInternal() const __NOTHROW__ = 0;

		virtual pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ { return this; }

	protected:

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#endif  // __ISharedObject_I_h__
