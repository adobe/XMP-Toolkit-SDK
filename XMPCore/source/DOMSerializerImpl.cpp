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

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/DOMSerializerImpl.h"
	#include "XMPCore/ImplHeaders/ClientDOMSerializerWrapperImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCommon/Interfaces/ISharedMutex.h"
#include "XMPCommon/Utilities/AutoSharedLock.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"

namespace AdobeXMPCore_Int {

	DOMSerializerImpl::DOMSerializerImpl()
		: mSharedMutex( ISharedMutex::CreateSharedMutex() ) { }

	spISharedMutex APICALL DOMSerializerImpl::GetMutex() const {
		return mSharedMutex;
	}

	spIDOMSerializer APICALL DOMSerializerImpl::Clone() const {
		DOMSerializerImpl * cloned = clone();
		if ( cloned ) {
			AutoSharedLock lock( mSharedMutex );
			cloned->mTreatKeyAsCaseInsensitiveCharBuffer = mTreatKeyAsCaseInsensitiveCharBuffer;
			cloned->mAllowDifferentValueTypesForExistingEntries = mAllowDifferentValueTypesForExistingEntries;
			if ( mKeysSet )
				cloned->mKeysSet = new KeysSet( mKeysSet->begin(), mKeysSet->end() );

			if ( mKeyValueTypeMap )
				cloned->mKeyValueTypeMap = new keyValueTypeMap( mKeyValueTypeMap->begin(), mKeyValueTypeMap->end() );

			for ( auto it = mMap.begin(), itEnd = mMap.end(); it != itEnd; ++it ) {
				cloned->mMap[ it->first ] = it->second;
			}
		}
		return MakeUncheckedSharedPointer( cloned, __FILE__, __LINE__, true );
	}

	AdobeXMPCore::spIDOMSerializer IDOMSerializer_I::CreateDOMSerializer( pIClientDOMSerializer_base clientDOMSerializer ) {
		return MakeUncheckedSharedPointer( new ClientDOMSerializerWrapperImpl( clientDOMSerializer ), __FILE__, __LINE__, true );
	}


}
