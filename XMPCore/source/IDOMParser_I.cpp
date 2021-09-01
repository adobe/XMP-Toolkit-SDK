// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/Interfaces/IDOMParser_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/Interfaces/IMetadata.h"

namespace AdobeXMPCore_Int {

	pvoid APICALL IDOMParser_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL IDOMParser_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< IDOMParser_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, &IDOMParser_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL IDOMParser_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kIDOMParserID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< IDOMParser_v1 * >( this );
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
				IError_v1::kESOperationFatal, kIDOMParserID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

	pIDOMParser_base APICALL IDOMParser_I::clone( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IDOMParser_v1, pIDOMParser_base, IDOMParser >(
			error, this, &IDOMParser_v1::Clone, __FILE__, __LINE__ );
	}

	pIMetadata_base APICALL IDOMParser_I::parse( const char * buffer, sizet bufferLength, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IDOMParser_v1, pIMetadata_base, IMetadata, const char *, sizet >(
			error, this, &IDOMParser_v1::Parse, __FILE__, __LINE__, buffer, bufferLength );
	}

	void APICALL IDOMParser_I::parseWithSpecificAction( const char * buffer, sizet bufferLength, uint32 actionType, pINode_base node, pcIError_base & error ) __NOTHROW__ {
		auto spNode = INode::MakeShared( node );
		return CallUnSafeFunctionReturningVoid< IDOMParser_v1, const char *, sizet, eActionType, spINode & >(
			error, this, &IDOMParser_v1::ParseWithSpecificAction, __FILE__, __LINE__, buffer, bufferLength, static_cast< eActionType >( actionType ), spNode );
	}

}
