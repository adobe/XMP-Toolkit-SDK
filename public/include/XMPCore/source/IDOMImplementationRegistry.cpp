// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
	class IDOMImplementationRegistryProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::IDOMImplementationRegistryProxy;

#include "XMPCore/Interfaces/IDOMImplementationRegistry.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include <assert.h>
#include "XMPCore/Interfaces/IDOMParser.h"
#include "XMPCore/Interfaces/IDOMSerializer.h"
#include "XMPCore/Interfaces/ICoreObjectFactory.h"

namespace AdobeXMPCore {

	class IDOMImplementationRegistryProxy
		: public virtual IDOMImplementationRegistry
	{
	private:
		pIDOMImplementationRegistry mRawPtr;

	public:
		IDOMImplementationRegistryProxy( pIDOMImplementationRegistry ptr )
			: mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~IDOMImplementationRegistryProxy() __NOTHROW__ { mRawPtr->Release(); }

		pIDOMImplementationRegistry APICALL GetActualIDOMImplementationRegistry() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCore_Int::pIDOMImplementationRegistry_I APICALL GetIDOMImplementationRegistry_I() __NOTHROW__ {
			return mRawPtr->GetIDOMImplementationRegistry_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		virtual spIDOMParser APICALL GetParser( const char * key ) const {
			return CallConstSafeFunctionReturningPointer< IDOMImplementationRegistry, pIDOMParser_base, IDOMParser, const char * >(
				mRawPtr, &IDOMImplementationRegistry::getParser, key );
		}

		virtual spIDOMSerializer APICALL GetSerializer( const char * key ) const {
			return CallConstSafeFunctionReturningPointer< IDOMImplementationRegistry, pIDOMSerializer_base, IDOMSerializer, const char * >(
				mRawPtr, &IDOMImplementationRegistry::getSerializer, key );
		}

		virtual bool APICALL RegisterParser( const char * key, pIClientDOMParser_base parser ) {
			return CallSafeFunction< IDOMImplementationRegistry, bool, uint32, const char *, pIClientDOMParser_base >(
				mRawPtr, &IDOMImplementationRegistry::registerParser, key, parser );
		}

		virtual bool APICALL RegisterSerializer( const char * key, pIClientDOMSerializer_base serializer ) {
			return CallSafeFunction< IDOMImplementationRegistry, bool, uint32, const char *, pIClientDOMSerializer_base >(
				mRawPtr, &IDOMImplementationRegistry::registerSerializer, key, serializer );
		}

		virtual pIDOMParser_base APICALL getParser( const char * key, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getParser( key, error );
		}

		virtual pIDOMSerializer_base APICALL getSerializer( const char * key, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getSerializer( key, error );
		}

		virtual uint32 APICALL registerParser( const char * key, pIClientDOMParser_base parser, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->registerParser( key, parser, error );
		}

		virtual uint32 APICALL registerSerializer( const char * key, pIClientDOMSerializer_base serializer, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->registerSerializer( key, serializer, error );
		}

	};

	spIDOMImplementationRegistry IDOMImplementationRegistry_v1::GetDOMImplementationRegistry() {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pIDOMImplementationRegistry_base, IDOMImplementationRegistry >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::GetDOMImplementationRegistry );
	}

	spIDOMImplementationRegistry IDOMImplementationRegistry_v1::MakeShared( pIDOMImplementationRegistry_base ptr ) {
		if ( !ptr ) return spIDOMImplementationRegistry();
		pIDOMImplementationRegistry p = IDOMImplementationRegistry::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IDOMImplementationRegistry >() : ptr;
		return shared_ptr< IDOMImplementationRegistry >( new IDOMImplementationRegistryProxy( p ) );
	}

}

#endif  // BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB
