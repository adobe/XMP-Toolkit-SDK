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


#include "XMPCore/Interfaces/ISimpleNode_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCommon/Interfaces/IUTF8String.h"

namespace AdobeXMPCore_Int {

	pvoid APICALL ISimpleNode_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL ISimpleNode_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< ISimpleNode_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, &ISimpleNode_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL ISimpleNode_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		pvoid returnValue( NULL );
		if ( interfaceID == kISimpleNodeID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< ISimpleNode_v1 * >( this );
				break;

			case kInternalInterfaceVersionNumber:
				return this;
				break;

			default:
				throw IError_I::CreateInterfaceVersionNotAvailableError(
					IError_v1::kESOperationFatal, interfaceID, interfaceVersion, __FILE__, __LINE__ );
				break;
			}
		} else {
			returnValue = INode_I::GetInterfacePointerInternal( interfaceID, interfaceVersion, false );
			if ( returnValue ) return returnValue;
		}
		if ( isTopLevel )
			throw IError_I::CreateInterfaceNotAvailableError(
				IError_v1::kESOperationFatal, kISimpleNodeID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

	pcIUTF8String_base APICALL ISimpleNode_I::getValue( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< ISimpleNode_v1, pcIUTF8String_base, const IUTF8String >(
			error, this, &ISimpleNode_v1::GetValue, __FILE__, __LINE__ );
	}

	void APICALL ISimpleNode_I::setValue( const char * value, sizet valueLength, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningVoid< ISimpleNode_v1, const char *, sizet >(
			error, this, &ISimpleNode_v1::SetValue, __FILE__, __LINE__, value, valueLength );
	}

	uint32 APICALL ISimpleNode_I::isURIType( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< ISimpleNode_v1, uint32, bool >(
			error, this, 0, &ISimpleNode_v1::IsURIType, __FILE__, __LINE__ );
	}

	void APICALL ISimpleNode_I::setURIType( uint32 isURI, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningVoid< ISimpleNode_v1, bool >(
			error, this, &ISimpleNode_v1::SetURIType, __FILE__, __LINE__, isURI != 0 );
	}

}
