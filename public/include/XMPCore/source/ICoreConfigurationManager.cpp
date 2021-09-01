// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
	class ICoreConfigurationManagerProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::ICoreConfigurationManagerProxy;

#include "XMPCore/Interfaces/ICoreConfigurationManager.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCore/Interfaces/ICoreObjectFactory.h"
#include <assert.h>

namespace AdobeXMPCore {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	class ICoreConfigurationManagerProxy
		: public virtual ICoreConfigurationManager
		, public virtual IConfigurationManagerProxy
	{
	private:
		pICoreConfigurationManager mRawPtr;

	public:
		ICoreConfigurationManagerProxy( pICoreConfigurationManager ptr )
			: IConfigurationManagerProxy( ptr )
			, mRawPtr( ptr ) {}

		~ICoreConfigurationManagerProxy() __NOTHROW__ {}

		pICoreConfigurationManager APICALL GetActualICoreConfigurationManager() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCore_Int::pICoreConfigurationManager_I APICALL GetICoreConfigurationManager_I() __NOTHROW__ {
			return mRawPtr->GetICoreConfigurationManager_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

	};

	spICoreConfigurationManager ICoreConfigurationManager_v1::MakeShared( pICoreConfigurationManager_base ptr ) {
		if ( !ptr ) return spICoreConfigurationManager();
		pICoreConfigurationManager p = ICoreConfigurationManager::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< ICoreConfigurationManager >() : ptr;
		return shared_ptr< ICoreConfigurationManager >( new ICoreConfigurationManagerProxy( p ) );
	}

	spICoreConfigurationManager ICoreConfigurationManager_v1::GetCoreConfigurationManager() {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pICoreConfigurationManager_base, ICoreConfigurationManager >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::GetCoreConfigurationManager );
	}

#if XMP_WinBuild
	#pragma warning( pop )
#endif

}

#endif  // !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB
