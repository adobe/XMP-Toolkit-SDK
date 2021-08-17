// =================================================================================================
// Copyright Adobe
// Copyright 2020 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/Interfaces/IMetadata_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"

namespace AdobeXMPCore_Int {

	pvoid APICALL IMetadata_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL IMetadata_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< IMetadata_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, &IMetadata_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL IMetadata_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kIMetadataID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< IMetadata_v1 * >( this );
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
			pvoid returnValue( NULL );
			returnValue = IStructureNode_I::GetInterfacePointerInternal( interfaceID, interfaceVersion, false );
			if ( returnValue ) return returnValue;
		}
		if ( isTopLevel )
			throw IError_I::CreateInterfaceNotAvailableError(
				IError_v1::kESOperationFatal, kIMetadataID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

	pcIUTF8String_base APICALL IMetadata_I::getAboutURI( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IMetadata_v1, pcIUTF8String_base, const IUTF8String >(
			error, this, &IMetadata_v1::GetAboutURI, __FILE__, __LINE__ );
	}

}
