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


#include "XMPCore/Interfaces/ICoreConfigurationManager_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"

namespace AdobeXMPCore_Int {

	pvoid APICALL ICoreConfigurationManager_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL ICoreConfigurationManager_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< ICoreConfigurationManager_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, &ICoreConfigurationManager_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL ICoreConfigurationManager_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kICoreConfigurationManagerID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< ICoreConfigurationManager_v1 * >( this );
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
			returnValue = IConfigurationManager_I::GetInterfacePointerInternal( interfaceID, interfaceVersion, false );
			if ( returnValue ) return returnValue;
		}

		if ( isTopLevel )
			throw IError_I::CreateInterfaceNotAvailableError(
				IError_v1::kESOperationFatal, kICoreConfigurationManagerID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

}
