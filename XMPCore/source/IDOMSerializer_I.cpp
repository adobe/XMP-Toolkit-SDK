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


#include "XMPCore/Interfaces/IDOMSerializer_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/Interfaces/INode.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap.h"

namespace AdobeXMPCore_Int {

	pvoid APICALL IDOMSerializer_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL IDOMSerializer_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< IDOMSerializer_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, &IDOMSerializer_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL IDOMSerializer_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kIDOMSerializerID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< IDOMSerializer_v1 * >( this );
				break;

			case kInternalInterfaceVersionNumber:
				return this;
				break;

			default:
				throw IError_I::CreateInterfaceVersionNotAvailableError(
					IError_v1::kESOperationFatal, interfaceID, interfaceVersion, __FILE__, __LINE__ );
				break;
			}
		}
		if ( isTopLevel )
			throw IError_I::CreateInterfaceNotAvailableError(
				IError_v1::kESOperationFatal, kIDOMSerializerID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

	pIDOMSerializer_base APICALL IDOMSerializer_I::clone( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IDOMSerializer_v1, pIDOMSerializer_base, IDOMSerializer >(
			error, this, &IDOMSerializer_v1::Clone, __FILE__, __LINE__ );
	}

	pIUTF8String_base APICALL IDOMSerializer_I::serialize( pINode_base node, pcINameSpacePrefixMap_base map, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IDOMSerializer_v1, pIUTF8String_base, IUTF8String, const spINode &, const spcINameSpacePrefixMap & >(
			error, this, &IDOMSerializer_v1::Serialize, __FILE__, __LINE__, INode::MakeShared( node ), INameSpacePrefixMap::MakeShared( map ) );
	}

}
