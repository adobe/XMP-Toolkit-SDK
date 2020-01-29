#ifndef ICompositeNode_I_h__
#define ICompositeNode_I_h__ 1

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
#include "XMPCore/Interfaces/ICompositeNode.h"
#include "XMPCore/Interfaces/INode_I.h"

namespace AdobeXMPCore_Int {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! \brief Version1 of the interface that serves as a base interface to all composite types of nodes in the XMP DOM ( like Arrays and Structures ).
	//! \details Provides all the functions to get various properties of the composite node.
	//! \attention Support multi threading through locks but can be enabled/disabled by the client. By default
	//! every object created does not support multi-threading.
	//!
	class ICompositeNode_I
		: public virtual ICompositeNode
		, public virtual INode_I
	{
	public:

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }

		virtual pICompositeNode APICALL GetActualICompositeNode() __NOTHROW__ { return this; }
		virtual pICompositeNode_I APICALL GetICompositeNode_I() __NOTHROW__ { return this; }

		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

	protected:
		virtual ~ICompositeNode_I() __NOTHROW__ {}
		pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );

		virtual uint32 APICALL getNodeTypeAtPath( pcIPath_base path, pcIError_base & error ) const __NOTHROW__;
		virtual pINode_base APICALL getNodeAtPath( pcIPath_base path, pcIError_base & error ) __NOTHROW__;
		virtual void APICALL appendNode( pINode_base node, pcIError_base & error ) __NOTHROW__;
		virtual void APICALL insertNodeAtPath( pINode_base node, pcIPath_base path, pcIError_base & error ) __NOTHROW__;
		virtual pINode_base APICALL replaceNodeAtPath( pINode_base node, pcIPath_base path, pcIError_base & error ) __NOTHROW__;
		virtual pINode_base APICALL removeNodeAtPath( pcIPath_base path, pcIError_base & error ) __NOTHROW__;
		virtual pINodeIterator_base APICALL iterator( pcIError_base & error ) __NOTHROW__;
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

#endif // ICompositeNode_I_h__
