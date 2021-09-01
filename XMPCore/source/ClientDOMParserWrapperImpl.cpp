// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore_Int {
	class ClientDOMParserWrapperImpl;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore_Int::ClientDOMParserWrapperImpl;

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/ClientDOMParserWrapperImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCore/Interfaces/IClientDOMParser.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"
#include "XMPCore/Interfaces/INode.h"

namespace AdobeXMPCore_Int {


	ClientDOMParserWrapperImpl::ClientDOMParserWrapperImpl( pIClientDOMParser_base parser )
		: mpClientParser( parser )
	{
		if ( parser ) {
			pcIError_base pError( NULL );
			uint32 unknownExceptionCaught( 0 );
			TreatKeyAsCaseInsensitive( parser->areKeysCaseSensitive( pError, unknownExceptionCaught ) == 0 );
			if ( !pError && unknownExceptionCaught == 0 )
				parser->initialize( this, pError, unknownExceptionCaught );
			if ( pError ) {
				auto spError = IError::MakeShared( pError );
				pError->Release();
				throw spError;
			}
			if ( unknownExceptionCaught )
				NOTIFY_ERROR( IError::kEDGeneral, kGECUnknownExceptionCaught, "Unknown Exception caught in the client code", IError::kESOperationFatal, false, false );
		}
	}

	spINode APICALL ClientDOMParserWrapperImpl::ParseAsNode( const char * buffer, sizet bufferLength ) {
		pcIError_base pError( NULL );
		uint32 unknownExceptionCaught( 0 );
		auto pnode = mpClientParser->parse( buffer, bufferLength, this, &ReportErrorAndContinueABISafe, pError, unknownExceptionCaught );
		if ( pError ) {
			auto spError = IError::MakeShared( pError );
			pError->Release();
			throw spError;
		}
		if ( unknownExceptionCaught )
			NOTIFY_ERROR( IError::kEDGeneral, kGECUnknownExceptionCaught, "Unknown Exception caught in the client code", IError::kESOperationFatal, false, false );
		return MakeUncheckedSharedPointer( pnode, __FILE__, __LINE__, false );
	}

	ClientDOMParserWrapperImpl::~ClientDOMParserWrapperImpl() __NOTHROW__ {
		if ( mpClientParser ) {
			mpClientParser->Release();
			mpClientParser = NULL;
		}
	}

	eConfigurableErrorCode APICALL ClientDOMParserWrapperImpl::ValidateValue( const uint64 & key, eDataType type, const CombinedDataValue & value ) const {
		pcIError_base pError( NULL );
		uint32 unknownExceptionCaught( 0 );
		auto retValue = mpClientParser->validate( key, static_cast< uint32 >( type ), value, pError, unknownExceptionCaught );
		if ( pError ) {
			auto spError = IError::MakeShared( pError );
			pError->Release();
			throw spError;
		}
		if ( unknownExceptionCaught )
			NOTIFY_ERROR( IError::kEDGeneral, kGECUnknownExceptionCaught, "Unknown Exception caught in the client code", IError::kESOperationFatal, false, false );
		return static_cast< eConfigurableErrorCode >( retValue );
	}

	DOMParserImpl * APICALL ClientDOMParserWrapperImpl::clone() const {
		ClientDOMParserWrapperImpl * cloned = new ClientDOMParserWrapperImpl( NULL );
		if ( cloned )
			cloned->mpClientParser = mpClientParser;
		return cloned;
	}

}
