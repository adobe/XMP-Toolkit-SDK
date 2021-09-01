// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/CoreConfigurationManagerImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"

namespace AdobeXMPCore_Int {

	CoreConfigurationManagerImpl::CoreConfigurationManagerImpl() { }

	static spICoreConfigurationManager ManageCoreConfigurationManager( bool destroy = false ) {
		static spICoreConfigurationManager sCoreConfigurationManager;
		if ( destroy ) {
			sCoreConfigurationManager.reset();
		} else {
			if ( !sCoreConfigurationManager )
				sCoreConfigurationManager = MakeUncheckedSharedPointer( new CoreConfigurationManagerImpl(), __FILE__, __LINE__, true );
		}
		return sCoreConfigurationManager;
	}

	void ICoreConfigurationManager_I::CreateCoreConfigurationManager() {
		ManageCoreConfigurationManager();
	}

	void ICoreConfigurationManager_I::DestroyCoreConfigurationManager() {
		ManageCoreConfigurationManager( true );
	}

}

#if BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
namespace AdobeXMPCore {
	using namespace AdobeXMPCore_Int;
	spICoreConfigurationManager ICoreConfigurationManager_v1::MakeShared( pICoreConfigurationManager_base ptr ) {
		if ( !ptr ) return spICoreConfigurationManager();
		pICoreConfigurationManager p = ICoreConfigurationManager::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< ICoreConfigurationManager >() : ptr;
		return MakeUncheckedSharedPointer( p, __FILE__, __LINE__, false );
	}

	spICoreConfigurationManager ICoreConfigurationManager::GetCoreConfigurationManager() {
		return AdobeXMPCore_Int::ManageCoreConfigurationManager();
	}
}
#endif  // BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
