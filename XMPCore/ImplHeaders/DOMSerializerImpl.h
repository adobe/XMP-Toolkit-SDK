#ifndef DOMSerializerImpl_h__
#define DOMSerializerImpl_h__ 1

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

#include "XMPCore/Interfaces/IDOMSerializer_I.h"
#include "XMPCommon/BaseClasses/MemoryManagedObject.h"
#include "XMPCommon/ImplHeaders/SharedObjectImpl.h"
#include "XMPCommon/ImplHeaders/ConfigurableImpl.h"

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {

	class DOMSerializerImpl
		: public virtual IDOMSerializer_I
		, public virtual ConfigurableImpl
		, public virtual SharedObjectImpl
		, public virtual MemoryManagedObject
	{
	public:
		DOMSerializerImpl();

		virtual spISharedMutex APICALL GetMutex() const;
		virtual spIDOMSerializer APICALL Clone() const;
		virtual spIUTF8String APICALL SerializeInternal(const spINode & node, XMP_OptionBits options, sizet padding, const char * newline, const char * indent, sizet baseIndent, const spcINameSpacePrefixMap & nameSpacePrefixMap) const = 0;

	protected:
		virtual ~DOMSerializerImpl() __NOTHROW__ {}
        using IDOMSerializer_I::clone;
		virtual DOMSerializerImpl * APICALL clone() const = 0;

		spISharedMutex				mSharedMutex;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif
#endif  // DOMSerializerImpl_h__

