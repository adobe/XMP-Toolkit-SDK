#ifndef ClientDOMParserWrapperImpl_h__
#define ClientDOMParserWrapperImpl_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#if !(IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED)
	#error "Not adhering to design constraints"
	// this file should only be included from its own cpp file
#endif

#include "XMPCore/ImplHeaders/DOMParserImpl.h"

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {

	class ClientDOMParserWrapperImpl
		: public virtual DOMParserImpl
	{
	public:
		ClientDOMParserWrapperImpl( pIClientDOMParser_base parser );

		virtual spINode APICALL ParseAsNode( const char * buffer, sizet bufferLength );
		virtual eConfigurableErrorCode APICALL ValidateValue( const uint64 & key, eDataType type, const CombinedDataValue & value ) const;
		virtual DOMParserImpl * APICALL clone() const;

	protected:
		virtual ~ClientDOMParserWrapperImpl() __NOTHROW__ ;

		pIClientDOMParser_base		mpClientParser;

	};
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif

#endif  // ClientDOMParserWrapperImpl_h__

