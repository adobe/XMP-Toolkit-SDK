#ifndef XMPCommonErrorCodes_I_h__
#define XMPCommonErrorCodes_I_h__ 1

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

#include "XMPCommon/XMPCommonDefines_I.h"
#include "XMPCommon/XMPCommonErrorCodes.h"

namespace AdobeXMPCommon_Int {

	//!
	//! Indicates various types of errors related to mutex.
	//!
	typedef enum {
		//! Represents no error in operation on mutex.
		kMTECNone					= 0,

		//! Represents an error while interacting with mutex due to memory crunch.
		kMTECMemoryCrunch			= 1,

		//! Represents an error while interacting with mutex due to resource crunch other than memory.
		kMTECResourceCrunch			= 2,

		//! Represents an error while interacting with mutex due to permission issues.
		kMTECPermission				= 3,

		//! Represents an error related to lock operation called on already locked mutex.
		kMTECAlreadyLocked			= 4,

		//! Represents an error because of operation being called on already un-initialized mutex.
		kMTECUninitialized			= 5,

		// Add new errors here

		//! Maximum value this enum can hold, should be treated as invalid value.
		kMTECMaxValue				= 0xFFFFFFFF
	} eMultiThreadingErrorCode;
}

#endif  // XMPCommonErrorCodes_I_h__
