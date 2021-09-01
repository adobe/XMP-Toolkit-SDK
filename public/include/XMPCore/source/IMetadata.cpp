// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
	class IMetadataProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::IMetadataProxy;

#include "XMPCore/Interfaces/IMetadata.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCore/Interfaces/ICoreObjectFactory.h"

#include <assert.h>

namespace AdobeXMPCore {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	class IMetadataProxy
		: public virtual IMetadata
		, public virtual IStructureNodeProxy
	{
	private:
		pIMetadata mRawPtr;

	public:
		IMetadataProxy( pIMetadata ptr )
			: mRawPtr( ptr )
			, IStructureNodeProxy( ptr )
			, ICompositeNodeProxy( ptr )
			, INodeProxy( ptr ) {}

		~IMetadataProxy() __NOTHROW__ {}

		pIMetadata APICALL GetActualIMetadata() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCore_Int::pIMetadata_I APICALL GetIMetadata_I() __NOTHROW__ {
			return mRawPtr->GetIMetadata_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		virtual spcIUTF8String APICALL GetAboutURI() const {
			return CallConstSafeFunctionReturningPointer< IMetadata_v1, pcIUTF8String_base, const IUTF8String >(
				mRawPtr, &IMetadata_v1::getAboutURI );
		}

		virtual void APICALL SetAboutURI( const char * uri, sizet uriLength ) __NOTHROW__ {
			mRawPtr->SetAboutURI( uri, uriLength );
		}

		virtual pcIUTF8String_base APICALL getAboutURI( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getAboutURI( error );
		}

		virtual void APICALL EnableFeature( const char * key, sizet keyLength ) const __NOTHROW__ {
			return mRawPtr->EnableFeature( key, keyLength );
		}

		virtual void APICALL DisableFeature( const char * key, sizet keyLength ) const __NOTHROW__ {
			return mRawPtr->DisableFeature( key, keyLength );
		}
	};

#if XMP_WinBuild
	#pragma warning( pop )
#endif

	spIMetadata IMetadata_v1::MakeShared( pIMetadata_base ptr ) {
		if ( !ptr ) return spIMetadata();
		pIMetadata p = IMetadata::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IMetadata >() : ptr;
		return shared_ptr< IMetadata >( new IMetadataProxy( p ) );
	}

	spIMetadata IMetadata_v1::CreateMetadata() {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pIMetadata, IMetadata >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::CreateMetadata );
	}

}

#endif  // BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB
