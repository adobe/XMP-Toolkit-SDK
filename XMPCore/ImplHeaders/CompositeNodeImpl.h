#ifndef CompositeNodeImpl_h__
#define CompositeNodeImpl_h__ 1

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

#include "XMPCore/Interfaces/ICompositeNode_I.h"
#include "XMPCore/ImplHeaders/NodeImpl.h"

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {

	class CompositeNodeImpl
		: public virtual ICompositeNode_I
		, public virtual NodeImpl
	{
	public:
		CompositeNodeImpl() {}

		virtual eNodeType APICALL GetNodeTypeAtPath( const spcIPath & path ) const;
		virtual spINode APICALL GetNodeAtPath( const spcIPath & path );
		virtual void APICALL InsertNodeAtPath( const spINode & node, const spcIPath & path );
		virtual spINode APICALL ReplaceNodeAtPath( const spINode & node, const spcIPath & path );
		virtual spINode APICALL RemoveNodeAtPath( const spcIPath & path );
		virtual bool CheckSuitabilityToBeUsedAsChildNode( const spcINode & node ) const;

	protected:
		virtual ~CompositeNodeImpl() __NOTHROW__ {}

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif

#endif // CompositeNodeImpl_h__
