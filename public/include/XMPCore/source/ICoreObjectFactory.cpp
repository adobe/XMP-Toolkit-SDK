// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCore/Interfaces/ICoreObjectFactory.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include <assert.h>

#if LINKING_XMPCORE_LIB
	extern "C" AdobeXMPCore::pICoreObjectFactory_base WXMPMeta_GetXMPDOMFactoryInstance_1();
#endif

namespace AdobeXMPCore {

	pICoreObjectFactory ICoreObjectFactory_v1::MakeCoreObjectFactory( pICoreObjectFactory_base ptr ) {
		if ( ICoreObjectFactory::GetInterfaceVersion() == 1 )
			return ptr;
		else
			return ptr->GetInterfacePointer< ICoreObjectFactory >();
	}

#if LINKING_XMPCORE_LIB
	static pICoreObjectFactory ManageCoreObjectFactory( bool destroy = false ) {
		static pICoreObjectFactory sCoreObjectFactoryPtr( NULL );
		if ( destroy && sCoreObjectFactoryPtr ) {
			sCoreObjectFactoryPtr = NULL;
			return sCoreObjectFactoryPtr;
		}

		if ( !sCoreObjectFactoryPtr ) {
			if ( ICoreObjectFactory::GetInterfaceVersion() != 1 )
				sCoreObjectFactoryPtr = WXMPMeta_GetXMPDOMFactoryInstance_1()->GetInterfacePointer< ICoreObjectFactory >();
			else
				sCoreObjectFactoryPtr = WXMPMeta_GetXMPDOMFactoryInstance_1();
		}
		return sCoreObjectFactoryPtr;
	}


	void ICoreObjectFactory_v1::SetupCoreObjectFactory() {
		ManageCoreObjectFactory();
	}
#else
	static pICoreObjectFactory ManageCoreObjectFactory( bool destroy = false, pICoreObjectFactory_base coreObjectFactory = NULL ) {
		static pICoreObjectFactory sCoreObjectFactoryPtr( NULL );
		if ( destroy && sCoreObjectFactoryPtr ) {
			sCoreObjectFactoryPtr = NULL;
			return sCoreObjectFactoryPtr;
		}

		if ( !sCoreObjectFactoryPtr && coreObjectFactory ) {
			if ( ICoreObjectFactory::GetInterfaceVersion() != 1 )
				sCoreObjectFactoryPtr = coreObjectFactory->GetInterfacePointer< ICoreObjectFactory >();
			else
				sCoreObjectFactoryPtr = coreObjectFactory;
		}
		return sCoreObjectFactoryPtr;
	}

	void ICoreObjectFactory_v1::SetupCoreObjectFactory( pICoreObjectFactory_base coreObjectFactory ) {
		ManageCoreObjectFactory( false, coreObjectFactory );
	}
#endif

	pICoreObjectFactory ICoreObjectFactory_v1::GetCoreObjectFactory() {
		return ManageCoreObjectFactory();
	}

	void ICoreObjectFactory_v1::DestroyCoreObjectFactory() {
		ManageCoreObjectFactory( true );
	}

}

#endif  // BUILDING_XMPCORE_LIB
