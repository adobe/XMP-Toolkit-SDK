// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/Interfaces/IPathSegment_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCommon/Interfaces/IUTF8String.h"

namespace AdobeXMPCore_Int {

	pvoid APICALL IPathSegment_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL IPathSegment_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< IPathSegment_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, &IPathSegment_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL IPathSegment_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kIPathSegmentID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< IPathSegment_v1 * >( this );
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
				IError_v1::kESOperationFatal, kIPathSegmentID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

	pcIUTF8String_base APICALL IPathSegment_I::getNameSpace( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IPathSegment_v1, pcIUTF8String_base, const IUTF8String >(
			error, this, &IPathSegment_v1::GetNameSpace, __FILE__, __LINE__ );
	}

	pcIUTF8String_base APICALL IPathSegment_I::getName( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IPathSegment_v1, pcIUTF8String_base, const IUTF8String >(
			error, this, &IPathSegment_v1::GetName, __FILE__, __LINE__ );
	}

	uint32 APICALL IPathSegment_I::getType( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< IPathSegment_v1, uint32, ePathSegmentType >(
			error, this, 0, &IPathSegment_v1::GetType, __FILE__, __LINE__ );
	}

	pcIUTF8String_base APICALL IPathSegment_I::getValue( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IPathSegment_v1, pcIUTF8String_base, const IUTF8String >(
			error, this, &IPathSegment_v1::GetValue, __FILE__, __LINE__ );
	}

}
