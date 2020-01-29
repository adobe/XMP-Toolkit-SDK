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

namespace AdobeXMPCore_Int {
	class ClientDOMSerializerWrapperImpl;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore_Int::ClientDOMSerializerWrapperImpl;


#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/ClientDOMSerializerWrapperImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCore/Interfaces/IClientDOMSerializer.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCore/Interfaces/INode.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap_I.h"

namespace AdobeXMPCore_Int {

	ClientDOMSerializerWrapperImpl::ClientDOMSerializerWrapperImpl( pIClientDOMSerializer serializer )
		: mpSerializer( serializer )
	{
		if ( serializer ) {
			pcIError_base pError( NULL );
			uint32 unknownExceptionCaught( 0 );
			TreatKeyAsCaseInsensitive( serializer->areKeysCaseSensitive( pError, unknownExceptionCaught ) == 0 );
			if ( !pError && unknownExceptionCaught == 0 )
				serializer->initialize( this, pError, unknownExceptionCaught );
			if ( pError ) {
				auto spError = IError::MakeShared( pError );
				pError->Release();
				throw spError;
			}
			if ( unknownExceptionCaught )
				NOTIFY_ERROR( IError::kEDGeneral, kGECUnknownExceptionCaught, "Unknown Exception caught in the client code", IError::kESOperationFatal, false, false );
		}
	}

	ClientDOMSerializerWrapperImpl::~ClientDOMSerializerWrapperImpl() __NOTHROW__ {
		if ( mpSerializer ) {
			mpSerializer->Release();
			mpSerializer = NULL;
		}
	}

	DOMSerializerImpl * APICALL ClientDOMSerializerWrapperImpl::clone() const {
		ClientDOMSerializerWrapperImpl * cloned = new ClientDOMSerializerWrapperImpl( NULL );
		if ( cloned )
			cloned->mpSerializer = mpSerializer;
		return cloned;
	}

	spIUTF8String APICALL ClientDOMSerializerWrapperImpl::Serialize( const spINode & node, const spcINameSpacePrefixMap & map ) {
		spIUTF8String str( IUTF8String_I::CreateUTF8String( NULL, 0 ) );
		pcIError_base pError( NULL );
		uint32 unknownExceptionCaught( 0 );
		spcINameSpacePrefixMap mergedMap = INameSpacePrefixMap::GetDefaultNameSpacePrefixMap();

		if ( map ) {
			spINameSpacePrefixMap newMergedMap = mergedMap->Clone();
			newMergedMap->GetINameSpacePrefixMap_I()->Merge( map );
			mergedMap = newMergedMap;
		}

		mpSerializer->serialize( node ? node->GetActualINode() : NULL, mergedMap ? mergedMap->GetActualINameSpacePrefixMap() : NULL, this,
			&ReportErrorAndContinueABISafe, str->GetActualIUTF8String(), pError, unknownExceptionCaught );
		if ( pError ) {
			auto spError = IError::MakeShared( pError );
			pError->Release();
			throw spError;
		}
		if ( unknownExceptionCaught )
			NOTIFY_ERROR( IError::kEDGeneral, kGECUnknownExceptionCaught, "Unknown Exception caught in the client code", IError::kESOperationFatal, false, false );
		return str;
	}

	eConfigurableErrorCode APICALL ClientDOMSerializerWrapperImpl::ValidateValue( const uint64 & key, eDataType type, const CombinedDataValue & value ) const {
		pcIError_base pError( NULL );
		uint32 unknownExceptionCaught( 0 );
		auto retValue = mpSerializer->validate( key, static_cast< uint32 >( type ), value, pError, unknownExceptionCaught );
		if ( pError ) {
			auto spError = IError::MakeShared( pError );
			pError->Release();
			throw spError;
		}
		if ( unknownExceptionCaught )
			NOTIFY_ERROR( IError::kEDGeneral, kGECUnknownExceptionCaught, "Unknown Exception caught in the client code", IError::kESOperationFatal, false, false );
		return static_cast< eConfigurableErrorCode >( retValue );
	}

	spIUTF8String APICALL ClientDOMSerializerWrapperImpl::SerializeInternal(const spINode & node, XMP_OptionBits options, sizet padding, const char * newline, const char * indent, sizet baseIndent, const spcINameSpacePrefixMap & nameSpacePrefixMap) const {

		return spIUTF8String();

	}
}
