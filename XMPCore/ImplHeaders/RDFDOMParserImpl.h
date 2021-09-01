#ifndef RDFDOMParserImpl_h__
#define RDFDOMParserImpl_h__ 1

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

#include "XMPCore/ImplHeaders/DOMParserImpl.h"
#include "XMPCore/source/XMPMeta.hpp"

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {

	class RDFDOMParserImpl
		: public virtual DOMParserImpl
	{
	public:
		RDFDOMParserImpl() {
			mGenericErrorCallbackPtr = NULL;
		}
		virtual spINode APICALL ParseAsNode( const char * buffer, sizet bufferLength );
        
		virtual eConfigurableErrorCode APICALL ValidateValue( const uint64 & key, eDataType type, const CombinedDataValue & value ) const;
		void InitializeDefaultValues();
		void SetErrorCallback(XMPMeta::ErrorCallbackInfo * ec);
	protected:
		virtual ~RDFDOMParserImpl() __NOTHROW__ {}
		virtual DOMParserImpl * APICALL clone() const;

	private:
		XMPMeta::ErrorCallbackInfo *	mGenericErrorCallbackPtr;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#endif // RDFDOMParserImpl_h__
