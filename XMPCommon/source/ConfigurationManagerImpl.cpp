// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCommon/ImplHeaders/ConfigurationManagerImpl.h"
	#include "XMPCommon/ImplHeaders/ErrorNotifierWrapperImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IMemoryAllocator_I.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	bool APICALL ConfigurationManagerImpl::RegisterMemoryAllocator( pIMemoryAllocator memoryAllocator ) {
		IMemoryAllocator_I::SetMemoryAllocator( memoryAllocator );
		return true;
	}

	bool APICALL ConfigurationManagerImpl::RegisterErrorNotifier( pIErrorNotifier_base clientErrorNotifier ) {
		pIErrorNotifier oldErrorNotifer = IErrorNotifier_I::SetErrorNotifier( clientErrorNotifier );
	//	if ( dynamic_cast< ErrorNotifierWrapperImpl_v1 * >( oldErrorNotifer ) )
	//		delete oldErrorNotifer;
		return true;
	}

	bool APICALL ConfigurationManagerImpl::DisableMultiThreading() {
		mMultiThreadingEnabled = false;
		return true;
	}

	bool APICALL ConfigurationManagerImpl::IsMultiThreaded() const {
		return mMultiThreadingEnabled;
	}

}

#if BUILDING_XMPCOMMON_LIB || SOURCE_COMPILING_XMP_ALL
namespace AdobeXMPCommon {

	spIConfigurationManager IConfigurationManager_v1::MakeShared( pIConfigurationManager_base ptr ) {
		if ( !ptr ) return spIConfigurationManager();
		pIConfigurationManager p = IConfigurationManager::GetInterfaceVersion() > 1 ?
			ptr->GetInterfacePointer< IConfigurationManager >() : ptr;
		return XMP_COMPONENT_INT_NAMESPACE::MakeUncheckedSharedPointer( p, __FILE__, __LINE__, true );
	}

}
#endif  // BUILDING_XMPCOMMON_LIB || SOURCE_COMPILING_XMP_ALL
