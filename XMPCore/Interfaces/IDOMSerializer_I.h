#ifndef IDOMSerializer_I_h__
#define IDOMSerializer_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================


#include "XMPCore/XMPCoreFwdDeclarations_I.h"
#include "XMPCore/Interfaces/IDOMSerializer.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject_I.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IConfigurable_I.h"

namespace AdobeXMPCore_Int {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! \brief Internal  that represents an object that can serialize an XMP Data Model to a buffer.
	//! Provides the functions to serialize the XMP Data Model.
	//!

	class IDOMSerializer_I
		: public virtual IDOMSerializer
		, public virtual IConfigurable_I
		, public virtual ISharedObject_I
	{
	public:

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }

		virtual pIDOMSerializer APICALL GetActualIDOMSerializer() __NOTHROW__ { return this; }
		virtual pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__{ return this; }
		virtual pIDOMSerializer_I APICALL GetIDOMSerializer_I() __NOTHROW__ { return this; }

		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

		virtual spIUTF8String APICALL SerializeInternal(const spINode & node, XMP_OptionBits options, sizet padding, const char * newline, const char * indent, sizet baseIndent, const spcINameSpacePrefixMap & nameSpacePrefixMap = spcINameSpacePrefixMap()) const = 0;


		// Factory functions

		//!
		//! Creates a DOM Serializer from the client DOM Serializer.
		//! \param[in] clientDOMSerializer a pointer to an object of type #IClientDOMSerializer_v1.
		//! \return a shared pointer to an object of type #IDOMSerializer.
		//!
		static spIDOMSerializer CreateDOMSerializer( pIClientDOMSerializer_base clientDOMSerializer );

	protected:
		virtual ~IDOMSerializer_I() __NOTHROW__ {}
		pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );

		virtual pIDOMSerializer_base APICALL clone( pcIError_base & error ) const __NOTHROW__;
		virtual pIUTF8String_base APICALL serialize( pINode_base node, pcINameSpacePrefixMap_base map, pcIError_base & error ) __NOTHROW__;
		virtual pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};

#if XMP_WinBuild
	#pragma warning( pop )
#endif

}

#endif // IDOMSerializer_I_h__
