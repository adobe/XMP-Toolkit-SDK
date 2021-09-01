#ifndef RDFDOMSerializerImpl_h__
#define RDFDOMSerializerImpl_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
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

	class RDFDOMSerializerImpl
		: public virtual DOMSerializerImpl
	{
	public:
		RDFDOMSerializerImpl() {}
		virtual spIUTF8String APICALL Serialize( const spINode & node, const spcINameSpacePrefixMap & nameSpacePrefixMap );
		virtual eConfigurableErrorCode APICALL ValidateValue( const uint64 & key, eDataType type, const CombinedDataValue & value ) const;
		virtual spIUTF8String APICALL SerializeInternal(const spINode & node, XMP_OptionBits options, sizet padding, const char * newline, const char * indent, sizet baseIndent, const spcINameSpacePrefixMap & nameSpacePrefixMap) const;
		void InitializeDefaultValues();

	protected:
		virtual ~RDFDOMSerializerImpl() __NOTHROW__ {}
		virtual DOMSerializerImpl * APICALL clone() const;


	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif

#endif // RDFDOMSerializerImpl_h__
