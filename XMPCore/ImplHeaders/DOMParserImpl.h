#ifndef DOMParserImpl_h__
#define DOMParserImpl_h__ 1

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

#include "XMPCore/Interfaces/IDOMParser_I.h"
#include "XMPCommon/BaseClasses/MemoryManagedObject.h"
#include "XMPCommon/ImplHeaders/SharedObjectImpl.h"
#include "XMPCommon/ImplHeaders/ConfigurableImpl.h"

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {

	class DOMParserImpl
		: public virtual IDOMParser_I
		, public virtual ConfigurableImpl
		, public virtual SharedObjectImpl
		, public virtual MemoryManagedObject
	{
	public:
		DOMParserImpl();

		virtual spIDOMParser APICALL Clone() const;
		virtual spIMetadata APICALL Parse( const char * buffer, sizet bufferLength );
		virtual void APICALL ParseWithSpecificAction( const char * buffer, sizet bufferLength, eActionType actionType, spINode & node );
		virtual spISharedMutex APICALL GetMutex() const;
		virtual void SetErrorCallback(XMPMeta::ErrorCallbackInfo * ec);

	protected:
		virtual ~DOMParserImpl() __NOTHROW__ {}
        using IDOMParser_I::clone;
		virtual DOMParserImpl * APICALL clone() const = 0;

		spISharedMutex				mSharedMutex;
		XMPMeta::ErrorCallbackInfo *	mGenericErrorCallbackPtr;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif

#endif  // DOMParserImpl_h__

