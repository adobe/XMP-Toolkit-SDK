#ifndef __ISimpleNode_I_h__
#define __ISimpleNode_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/XMPCoreFwdDeclarations_I.h"
#include "XMPCore/Interfaces/ISimpleNode.h"
#include "XMPCore/Interfaces/INode_I.h"

namespace AdobeXMPCore_Int {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! \brief Internal interface that represents a Simple Property Node of XMP DOM.
	//! \details Provides all the functions to get and set various properties of the simple node.
	//! \attention Support multi threading through locks but can be enabled/disabled by the client. By default
	//! every object created does not support multi-threading.
	//!
	class ISimpleNode_I
		: public virtual ISimpleNode
		, public virtual INode_I
	{
	public:

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }

		virtual pISimpleNode APICALL GetActualISimpleNode() __NOTHROW__ { return this; }
		virtual pISimpleNode_I APICALL GetISimpleNode_I() __NOTHROW__ { return this; }

		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

		//!
		//! Creates a simple property node which is not part of any metadata document.
		//! \param[in] nameSpace shared pointer to a constant IUTF8String object containing name space URI of the simple node.
		//! \param[in] name shared pointer to a constant IUTF8String object containing local name of the simple node.
		//! \param[in] value shared pointer to a constant IUTF8String object containing value of the simple node.
		//! \return a shared pointer to a #AdobeXMPCore::ISimpleNode object.
		//! \attention Error is thrown in case
		//!		- nameSpace or name are invalid pointers, or
		//!		- their contents are empty.
		//! \note In case the value is an invalid pointer or its contents are empty than the value is set to empty string.
		//!
		XMP_PRIVATE static spISimpleNode CreateSimpleNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name, const spcIUTF8String & value = spcIUTF8String() );

	protected:
		virtual ~ISimpleNode_I() __NOTHROW__ {}
		pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );

		virtual pcIUTF8String_base APICALL getValue( pcIError_base & error ) const __NOTHROW__;
		virtual void APICALL setValue( const char * value, sizet valueLength, pcIError_base & error ) __NOTHROW__;
		virtual uint32 APICALL isURIType( pcIError_base & error ) const __NOTHROW__;
		virtual void APICALL setURIType( uint32 isURI, pcIError_base & error ) __NOTHROW__;
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

#endif  // __ISimpleNode_I_h__
