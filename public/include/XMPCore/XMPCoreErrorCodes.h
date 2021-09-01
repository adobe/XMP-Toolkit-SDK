#ifndef XMPCoreErrorCodes_h__
#define XMPCoreErrorCodes_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCore/XMPCoreDefines.h"
#include "XMPCommon/XMPCommonErrorCodes.h"

namespace AdobeXMPCore {

	typedef enum {
		//! Indicates no error.
		kDMECNone									= 0,

		//! Indicates that IXMPNameSpacePrefixMap has an entry missing.
		kDMECNameSpacePrefixMapEntryMissing			= 1,

		//! Indicates that a different type of node is present than one user is expecting
		kDMECDifferentNodeTypePresent				= 2,
		
		//! Indicates that node is already a child of another parent.
		kDMECNodeAlreadyAChild						= 3,

		//! Indicates a node with the same qualified name or index already exists.
		kDMECNodeAlreadyExists						= 4,

		//! Indicates no such node exists.
		kDMECNoSuchNodeExists						= 5,

		//! Indicates current array element type is not same as that of other child items
		kDMECArrayItemTypeDifferent					= 6,

		//! Indicates invalid path segment inside a path.
		kDMECInvalidPathSegment						= 7,

		//! Indicates Bad schema parameter
		kDMECBadSchema								= 101,

		//! Indicates Bad XPath parameter
		kDMECBadXPath								= 102,

		//! Indicates Bad options parameter
		kDMECBadOptions								= 103,

		//! Indicates Bad iteration position
		kDMECBadIterPosition						= 104,

		//! Indicates Unicode error
		kDMECBadUnicode								= 105,

		//! Indicates XMP format error
		kDMECValidationError						= 106,

		//! Indicates Empty iterator
		kDMECEmptyIterator							= 107,

		//! Maximum value this enum can hold, should be treated as invalid value.
		kDMECMaxValue								= kMaxEnumValue
	} eDataModelErrorCode;

	//!
	//! @brief Indicates various errors encountered during parsing.
	//!
	typedef enum {
		//! Indicates no error.
		kPECNone									= 0,
		
		//! Indicates XML parsing error.
		kPECBadXML									= 1,

		//! RDF format error
		kPECBadRDF									= 2,

		//! XMP format error
		kPECBadXMP									= 3,

		//! Context Node is invalid
		kPECInvalidContextNode						= 4,

		//! Context Node is not a composite node
		kPECContextNodeIsNonComposite				= 5,

		//! Parent of Context Node is not an array node
		kPECContextNodeParentIsNonArray				= 6,

		//! Maximum value this enum can hold, should be treated as invalid value.
		kPECMaxValue								= kMaxEnumValue
	} eParserErrorCode;

	//!
	//! @brief Indicates various errors encountered during serialization.
	//!
	typedef enum {
		//! Indicates no error.
		kSECNone									= 0,

		//! Indicates serialization failed to achieve size requirement.
		kSECSizeExceed								= 1,

		//! Indicates un registered namespace encountered during serialization.
		kSECUnRegisteredNameSpace					= 2,

		//! Maximum value this enum can hold, should be treated as invalid value.
		kSECMaxValue								= kMaxEnumValue

	} eSerializerErrorCode;
}

#endif  // XMPCoreErrorCodes_h__
