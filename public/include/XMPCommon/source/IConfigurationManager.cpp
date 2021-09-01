// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#define FRIEND_CLASS_DECLARATION() friend class IConfigurationManagerProxy;

#include "XMPCommon/Interfaces/IConfigurationManager.h"

#if !BUILDING_XMPCOMMON_LIB && !SOURCE_COMPILING_XMP_ALL

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IError.h"

#include <assert.h>

namespace AdobeXMPCommon {

	void APICALL IConfigurationManagerProxy::Acquire() const __NOTHROW__ {
		assert( false );
	}

	void APICALL IConfigurationManagerProxy::Release() const __NOTHROW__ {
		assert( false );
	}

	pvoid APICALL IConfigurationManagerProxy::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
	}

	pvoid APICALL IConfigurationManagerProxy::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return AdobeXMPCommon::CallSafeFunction<
			IVersionable, pvoid, pvoid, uint64, uint32
		>( mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
	}

	uint32 APICALL IConfigurationManagerProxy::registerMemoryAllocator( pIMemoryAllocator_base memoryAllocator, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->registerMemoryAllocator( memoryAllocator, error );
	}

	bool APICALL IConfigurationManagerProxy::RegisterMemoryAllocator( pIMemoryAllocator memoryAllocator ) {
		return CallSafeFunction< IConfigurationManager, bool, uint32, pIMemoryAllocator_base >(
			mRawPtr, &IConfigurationManager::registerMemoryAllocator, memoryAllocator );
	}

	uint32 APICALL IConfigurationManagerProxy::registerErrorNotifier( pIErrorNotifier_base clientErrorNotifier, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->registerErrorNotifier( clientErrorNotifier, error );
	}

	bool APICALL IConfigurationManagerProxy::RegisterErrorNotifier( pIErrorNotifier_base clientErrorNotifier ) {
		return CallSafeFunction< IConfigurationManager, bool, uint32, pIErrorNotifier_base >(
			mRawPtr, &IConfigurationManager::registerErrorNotifier, clientErrorNotifier );
	}

	uint32 APICALL IConfigurationManagerProxy::disableMultiThreading( pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->disableMultiThreading( error );
	}

	bool APICALL IConfigurationManagerProxy::DisableMultiThreading() {
		return CallSafeFunction< IConfigurationManager, bool, uint32 >(
			mRawPtr, &IConfigurationManager::disableMultiThreading );
	}

	uint32 APICALL IConfigurationManagerProxy::isMultiThreaded( pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->isMultiThreaded( error );
	}

	bool APICALL IConfigurationManagerProxy::IsMultiThreaded() const {
		return CallConstSafeFunction< IConfigurationManager, bool, uint32 >(
			mRawPtr, &IConfigurationManager::isMultiThreaded );
	}

	spIConfigurationManager IConfigurationManager_v1::MakeShared( pIConfigurationManager_base ptr ) {
		if ( !ptr ) return spIConfigurationManager();
		pIConfigurationManager p = IConfigurationManager::GetInterfaceVersion() > 1 ?
			ptr->GetInterfacePointer< IConfigurationManager >() : ptr;
		return shared_ptr< IConfigurationManager >( new IConfigurationManagerProxy( p ) );
	}

}

#endif  // !BUILDING_XMPCOMMON_LIB && !SOURCE_COMPILING_XMP_LIB

