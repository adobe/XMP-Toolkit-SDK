#ifndef ClientDOMSerializerWrapperImpl_h__
#define ClientDOMSerializerWrapperImpl_h__ 1

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

#if !(IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED)
	#error "Not adhering to design constraints"
	// this file should only be included from its own cpp file
#endif

#include "XMPCore/ImplHeaders/DOMSerializerImpl.h"

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {

	class ClientDOMSerializerWrapperImpl
		: public virtual DOMSerializerImpl
	{
	public:
		ClientDOMSerializerWrapperImpl( pIClientDOMSerializer serializer );
		virtual spIUTF8String APICALL Serialize( const spINode & node, const spcINameSpacePrefixMap & map );
		virtual eConfigurableErrorCode APICALL ValidateValue( const uint64 & key, eDataType type, const CombinedDataValue & value ) const;
		spIUTF8String APICALL SerializeInternal(const spINode & node, XMP_OptionBits options, sizet padding, const char * newline, const char * indent, sizet baseIndent, const spcINameSpacePrefixMap & nameSpacePrefixMap) const;

	protected:
		virtual ~ClientDOMSerializerWrapperImpl() __NOTHROW__ ;
		virtual DOMSerializerImpl * APICALL clone() const;

		pIClientDOMSerializer			mpSerializer;

	};
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif

#endif  // ClientDOMSerializerWrapperImpl_h__

