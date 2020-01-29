#ifndef __IArrayNode_I_h__
#define __IArrayNode_I_h__ 1

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


#include "XMPCore/Interfaces/IArrayNode.h"
#include "XMPCore/Interfaces/ICompositeNode_I.h"


namespace AdobeXMPCore_Int {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! \brief Internal interface that represents an ArrayNode.
	//!	\details Provides all the functions to access
	//!
	class IArrayNode_I
		: public virtual IArrayNode
		, public virtual ICompositeNode_I
	{
	public:

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }

		virtual pIArrayNode APICALL GetActualIArrayNode() __NOTHROW__ { return this; }
		virtual pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__{ return this; }
		virtual pIArrayNode_I APICALL GetIArrayNode_I() __NOTHROW__ { return this; }

		//!
		//! list all functions to be defined in this interface
		//!


		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

		// Factories to create the array node

		//!
		//! Creates an unordered array node which is not part of any metadata document.
		//! \param[in] nameSpace shared pointer to a constant IUTF8String object containing name space URI of the simple node.
		//! \param[in] name shared pointer to a constant IUTF8String object containing local name of the simple node.
		//! \param[in] arrayNodeForm a value of type #eArrayForm indicating the type of array node.
		//! \return a shared pointer to a #IArrayNode object.
		//! \attention Error is thrown in the following cases:
		//!		-# nameSpace is NULL or its contents are empty.
		//!		-# name is NULL or its contents are empty.
		//!
		XMP_PRIVATE static spIArrayNode CreateArrayNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name, eArrayForm arrayForm );

	protected:
		virtual ~IArrayNode_I() __NOTHROW__ {}
		pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );

		virtual uint32 APICALL getArrayForm( pcIError_base & error ) const __NOTHROW__;
		virtual uint32 APICALL getChildNodeType( pcIError_base & error ) const __NOTHROW__;
		virtual pINode_base APICALL getNodeAtIndex( sizet index, pcIError_base & error ) __NOTHROW__;
		virtual void APICALL insertNodeAtIndex( pINode_base node, sizet index, pcIError_base & error ) __NOTHROW__;
		virtual pINode_base APICALL removeNodeAtIndex( sizet index, pcIError_base & error ) __NOTHROW__;
		virtual pINode_base APICALL replaceNodeAtIndex( pINode_base node, sizet index, pcIError_base & error ) __NOTHROW__;
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

#endif  // __IArrayNode_I_h__
