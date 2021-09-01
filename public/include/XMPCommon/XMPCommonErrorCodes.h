#ifndef XMPCommonErrorCodes_h__
#define XMPCommonErrorCodes_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/XMPCommonDefines.h"

namespace AdobeXMPCommon {

	//!
	//! @brief Indicates various types of error codes within General Domain.
	//!
	typedef enum {
		//! Indicates no error
		kGECNone = 0,

		//! Indicates that parameters passed to function are not as expected.
		kGECParametersNotAsExpected = 1,

		//! Indicates that version expected by client is not available in the library.
		kGECVersionUnavailable = 2,

		//! Indicates that some assertion has failed.
		kGECAssertionFailure = 3,

		//! Indicates logic failure.
		kGECLogicalError = 4,

		//! Indicates index provided is out of bounds.
		kGECIndexOutOfBounds = 5,

		//! Indicates an internal failure.
		kGECInternalFailure = 6,

		//! Indicates a call to deprecated function.
		kGECDeprecatedFunctionCall = 7,

		//! Indicates an external failure.
		kGECExternalFailure = 8,

		//! Indicates an unknown failure.
		kGECUnknownFailure = 9,

		//! Indicates an error due to User Abort.
		kGECUserAbort = 10,

		//! Indicates a particular interface is not available.
		kGECInterfaceUnavailable = 11,

		//! Indicates that client code has thrown some exception.
		kGECClientThrownExceptionCaught = 100,

		//! Indicates that standard exception has occurred.
		kGECStandardException = 101,

		//! Indicates that some unknown exception has occurred.
		kGECUnknownExceptionCaught = 200,

		//! Indicates that functionality is not yet implemented.
		kGECNotImplemented = 10000,

		//! Maximum value this enum can hold, should be treated as invalid value.
		kGECMaxValue = kMaxEnumValue
	} eGeneralErrorCode;

	//!
	//! @brief Indicates various types of error codes within Memory Management domain.
	//!
	typedef enum {
		//! Indicates no error
		kMMECNone = 0,

		//! Indicates that allocation has failed.
		kMMECAllocationFailure = 1,

		//! Maximum value this enum can hold, should be treated as invalid value.
		kMMECMaxValue = kMaxEnumValue
	} eMemoryManagementErrorCode;

	//!
	//! @brief Indicates various types of error codes within Configurable domain.
	//!
	typedef enum {
		//! Indicates no error.
		kCECNone = 0,

		//! Indicates that key is not supported by the object.
		kCECKeyNotSupported = 1,

		//! Indicates different type of value provided than the one supported for a key.
		kCECValueTypeNotSupported = 2,

		//! Indicates that different value type is previously stored for a key.
		kCECPreviousTypeDifferent = 3,

		//! Indicates the type of value stored for a key is different than what client is asking for.
		kCECValueTypeMismatch = 4,

		//! Indicates an invalid value is provided.
		kCECValueNotSupported = 5,

		//! Maximum value this enum can hold, should be treated as invalid value.
		kCECodeMaxValue = 0xFFFFFFFF
	} eConfigurableErrorCode;
}

#endif  // XMPCommonErrorCodes_h__
