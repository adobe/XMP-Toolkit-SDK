#ifndef ArrayNodeImpl_h__
#define ArrayNodeImpl_h__ 1

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

#if !(IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED)
	#error "Not adhering to design constraints"
	// this file should only be included from its own cpp file
#endif

#include "XMPCore/Interfaces/IArrayNode_I.h"
#include "XMPCore/ImplHeaders/CompositeNodeImpl.h"
#include "XMPCommon/Utilities/TAllocator.h"

#include <vector>

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {

	class ArrayNodeImpl
		: public virtual IArrayNode_I
		, public virtual CompositeNodeImpl
	{
	public:
		typedef std::vector< spINode, TAllocator< spINode > > NodeVector;

		ArrayNodeImpl( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, eArrayForm arrarForm );

		virtual eArrayForm APICALL GetArrayForm() const;
		virtual eNodeType APICALL GetChildNodeType() const;
		virtual spINode APICALL GetNodeAtIndex( sizet index );
		virtual void APICALL InsertNodeAtIndex( const spINode & node, sizet index );
		virtual spINode APICALL RemoveNodeAtIndex( sizet index );
		virtual spINode APICALL ReplaceNodeAtIndex( const spINode & node, sizet index );
		virtual void APICALL AppendNode( const spINode & node );
		virtual spINodeIterator APICALL Iterator();
		virtual sizet APICALL ChildCount() const __NOTHROW__;
		virtual spIArrayNode APICALL ConvertToArrayNode();
		virtual eNodeType APICALL GetNodeType() const;
		virtual bool APICALL HasContent() const;
		virtual void APICALL ClearContents();
		virtual spINode APICALL CloneContents( bool ignoreEmptyNodes, bool ignoreNodesWithOnlyQualifiers, sizet qualifiersCount ) const;
		virtual bool CheckSuitabilityToBeUsedAsChildNode( const spcINode & node ) const;

	protected:
		virtual ~ArrayNodeImpl() __NOTHROW__ {}
		virtual void resetChangesForChildren() const;


		eArrayForm						mArrayForm;
		NodeVector						mChildren;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#endif // ArrayNodeImpl_h__
