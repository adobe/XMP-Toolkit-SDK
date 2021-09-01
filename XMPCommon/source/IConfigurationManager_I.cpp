// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/Interfaces/IConfigurationManager_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/BaseClasses/MemoryManagedObject.h"


namespace XMP_COMPONENT_INT_NAMESPACE {
	extern pIErrorNotifier CreateErrorNotifierWrapperImpl( pIErrorNotifier_base errorNotifier );

	uint32 APICALL IConfigurationManager_I::registerMemoryAllocator( pIMemoryAllocator_base memoryAllocator, pcIError_base & error ) __NOTHROW__ {
		// since IMemoryAllocator is at version 1, there is no need for the wrapper class.
		return CallUnSafeFunction< IConfigurationManager_v1, uint32, bool, pIMemoryAllocator >(
			error, this, 0, &IConfigurationManager_v1::RegisterMemoryAllocator, __FILE__, __LINE__, memoryAllocator );
	}

	uint32 APICALL IConfigurationManager_I::registerErrorNotifier( pIErrorNotifier_base clientErrorNotifier, pcIError_base & error ) __NOTHROW__ {
		pIErrorNotifier ptr = CreateErrorNotifierWrapperImpl( clientErrorNotifier );
		return CallUnSafeFunction< IConfigurationManager_v1, uint32, bool, pIErrorNotifier >(
			error, this, 0, &IConfigurationManager_v1::RegisterErrorNotifier, __FILE__, __LINE__, ptr );
	}

	uint32 APICALL IConfigurationManager_I::disableMultiThreading( pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< IConfigurationManager_v1, uint32, bool >(
			error, this, 0, &IConfigurationManager_v1::DisableMultiThreading, __FILE__, __LINE__ );
	}

	uint32 APICALL IConfigurationManager_I::isMultiThreaded( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< IConfigurationManager_v1, uint32, bool >(
			error, this, 0, &IConfigurationManager_v1::IsMultiThreaded, __FILE__, __LINE__ );
	}

	pvoid APICALL IConfigurationManager_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL IConfigurationManager_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		typedef pvoid( APICALL IConfigurationManager_I::*Func )( uint64, uint32 );
		Func fnPtr = &IConfigurationManager_I::GetInterfacePointer;
		return CallUnSafeFunction< IConfigurationManager_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, fnPtr, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL IConfigurationManager_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kIConfigurationManagerID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< IConfigurationManager_v1 * >( this );
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
			IError_v1::kESOperationFatal, kIConfigurationManagerID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

}

