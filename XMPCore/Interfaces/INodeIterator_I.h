#ifndef __INodeIterator_I_h__
#define __INodeIterator_I_h__ 1


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
#include "XMPCore/Interfaces/INodeIterator.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject_I.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"

namespace AdobeXMPCore_Int {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! \brief Internal interface that represents an iterator over the mutable children of a XMP DOM Node.
	//! \attention Support multi threading through locks but can be disabled by the client.
	//! \note Iterators are valid as long as their are no changes performed on the node. In case there are some
	//! changes performed on the node then the behavior is undefined.
	//!
	class INodeIterator_I
		: public virtual INodeIterator
		, public virtual ISharedObject_I
	{
	public:
		virtual pINodeIterator APICALL GetActualINodeIterator() __NOTHROW__ { return this;  }
		virtual pINodeIterator_I APICALL GetINodeIterator_I() __NOTHROW__ { return this;  }
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }
		virtual pvoid APICALL GetInterfacePointer(uint64 interfaceID, uint32 interfaceVersion);
	protected:
		virtual ~INodeIterator_I() __NOTHROW__ {}
		pvoid APICALL GetInterfacePointerInternal(uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel);
		virtual pINode_base APICALL getNode( pcIError_base & error ) __NOTHROW__;
		virtual uint32 APICALL getNodeType( pcIError_base & error ) const __NOTHROW__;
		virtual pINodeIterator_base APICALL next( pcIError_base & error ) __NOTHROW__;
		virtual pvoid APICALL getInterfacePointer(uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error) __NOTHROW__;
	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};

#if XMP_WinBuild
	#pragma warning( pop )
#endif

}

#endif  // __INodeIterator_I_h__
