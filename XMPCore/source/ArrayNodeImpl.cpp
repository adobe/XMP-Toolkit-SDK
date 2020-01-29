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

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/ArrayNodeImpl.h"
	#include "XMPCore/ImplHeaders/TNodeIteratorImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCommon/Utilities/AutoSharedLock.h"
#include "XMPCore/Interfaces/INodeIterator_I.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"

namespace AdobeXMPCore_Int {

	// All virtual functions
	
	ArrayNodeImpl::ArrayNodeImpl( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, eArrayForm arrayForm )
		: NodeImpl( nameSpace, nameSpaceLength, name, nameLength )
		, mArrayForm( arrayForm ) {}

	IArrayNode_v1::eArrayForm APICALL ArrayNodeImpl::GetArrayForm() const {
		return mArrayForm;
	}

	INode_v1::eNodeType APICALL ArrayNodeImpl::GetChildNodeType() const {
		AutoSharedLock lock( mSharedMutex );
		if ( mChildren.size() != 0 )
			return mChildren[ 0 ]->GetNodeType();
		return kNTAll;
	}

	spINode APICALL ArrayNodeImpl::GetNodeAtIndex( sizet index ) {
		sizet actualIndex = index - 1;

		AutoSharedLock lock( mSharedMutex );

		if ( actualIndex < mChildren.size() ) {
			return MakeUncheckedSharedPointer( mChildren.at( actualIndex ).get(), __FILE__, __LINE__ );
		}
		return spINode_I();
	}

	void APICALL ArrayNodeImpl::InsertNodeAtIndex( const spINode & node, sizet index ) {
		sizet actualIndex = index - 1;
		{
			AutoSharedLock lock( mSharedMutex );
			if ( actualIndex > mChildren.size() )
				NOTIFY_ERROR( IError_base::kEDGeneral, kGECIndexOutOfBounds,
					"Trying to insert a node at an invalid index", IError_v1::kESOperationFatal,
					true, index, true, static_cast< sizet >( mChildren.size() ) );
		}
		bool goAhead = CheckSuitabilityToBeUsedAsChildNode( node );

		if ( goAhead ) {
			AutoSharedLock lock( mSharedMutex, true );
			auto it = mChildren.begin();
			std::advance( it, actualIndex );
			it = mChildren.insert( it, MakeUncheckedSharedPointer( node.get(), __FILE__, __LINE__ ) );
			pINode_I node_I = node->GetINode_I();
			node_I->SetIndex( index );
			node_I->ChangeParent( this );
			std::advance( it, 1 );
			for ( auto endIt = mChildren.end(); it != endIt; ++it ) {
				( *it )->GetINode_I()->SetIndex( ++index );
			}
		}
	}

	spINode APICALL ArrayNodeImpl::RemoveNodeAtIndex( sizet index ) {
		spINode node = GetNodeAtIndex( index );
		if ( node ) {
			sizet actualIndex = index - 1;
			AutoSharedLock lock( mSharedMutex, true );
			auto it = mChildren.begin();
			std::advance( it, actualIndex );
			it = mChildren.erase( it );
			node->GetINode_I()->ChangeParent( NULL );
			for ( auto endIt = mChildren.end(); it != endIt; ++it ) {
				( *it )->GetINode_I()->SetIndex( index++ );
			}
		}
		return node;
	}

	spINode APICALL ArrayNodeImpl::ReplaceNodeAtIndex( const spINode & node, sizet index ) {
		if ( CheckSuitabilityToBeUsedAsChildNode( node ) && GetNodeAtIndex( index ) ) {
			auto retValue = RemoveNodeAtIndex( index );
			InsertNodeAtIndex( node, index );
			return retValue;
		} else {
			NOTIFY_ERROR( IError_v1::kEDGeneral, kGECIndexOutOfBounds,
				"Trying to replace a node at an invalid index", IError_v1::kESOperationFatal,
				true, index, true, static_cast< sizet >( mChildren.size() ) );
		}
		return spINode();
	}

	void APICALL ArrayNodeImpl::AppendNode( const spINode & node ) {
		InsertNodeAtIndex( node, ChildCount() + 1 );
	}

	spINodeIterator APICALL ArrayNodeImpl::Iterator() {
		AutoSharedLock lock( mSharedMutex );
		auto beginIt = mChildren.begin(), endIt = mChildren.end();
		if ( beginIt == endIt )
			return spINodeIterator();
		else
			return MakeUncheckedSharedPointer( new TNodeIteratorImpl< NodeVector::iterator >( beginIt, endIt ), __FILE__, __LINE__, true );
	}

	sizet APICALL ArrayNodeImpl::ChildCount() const __NOTHROW__ {
		AutoSharedLock lock( mSharedMutex );
		return mChildren.size();
	}

	spIArrayNode APICALL ArrayNodeImpl::ConvertToArrayNode() {
		return MakeUncheckedSharedPointer( this, __FILE__, __LINE__ );
	}

	INode_v1::eNodeType APICALL ArrayNodeImpl::GetNodeType() const {
		return kNTArray;
	}

	bool APICALL ArrayNodeImpl::HasContent() const {
		AutoSharedLock lock( mSharedMutex );
		return mChildren.size() > 0;
	}

	void APICALL ArrayNodeImpl::ClearContents() {
		AutoSharedLock lock( mSharedMutex, true );
		for ( auto it = mChildren.begin(), itEnd = mChildren.end(); it != itEnd; ++it ) {
			( *it )->GetINode_I()->ChangeParent( NULL );
		}
		mChildren.clear();
	}

	spINode APICALL ArrayNodeImpl::CloneContents( bool ignoreEmptyNodes, bool ignoreNodesWithOnlyQualifiers, sizet qualifiersCount ) const {
		spIArrayNode newNode;
		if ( ignoreEmptyNodes && mChildren.size() == 0 ) {
			if ( ignoreNodesWithOnlyQualifiers && qualifiersCount == 0 )
				return newNode;
		}

		newNode = IArrayNode_I::CreateArrayNode( mNameSpace, mName, mArrayForm );

		for ( auto it = mChildren.begin(), endIt = mChildren.end(); it != endIt; ++it ) {
			spINode childNode = ( *it )->Clone( ignoreEmptyNodes, ignoreNodesWithOnlyQualifiers );
			if ( childNode ) {
				newNode->AppendNode( childNode );
			}
		}

		if ( ignoreEmptyNodes && newNode->ChildCount() == 0 ) {
			if ( ignoreNodesWithOnlyQualifiers )
				return spINode();
			else if ( !ignoreNodesWithOnlyQualifiers && qualifiersCount == 0 )
				return spINode();
		}
		return newNode;
	}

	void ArrayNodeImpl::resetChangesForChildren() const {
		AutoSharedLock lock( mSharedMutex );
		for ( auto it = mChildren.begin(), itEnd = mChildren.end(); it != itEnd; ++it ) {
			( *it )->AcknowledgeChanges();
		}
	}

	bool ArrayNodeImpl::CheckSuitabilityToBeUsedAsChildNode( const spcINode & node ) const {
		bool result = CompositeNodeImpl::CheckSuitabilityToBeUsedAsChildNode( node );
		if ( !result ) return false;

		eNodeType currentChildType = GetChildNodeType();
		if ( currentChildType == kNTAll || currentChildType == node->GetNodeType() )
			return true;
		else {
			NOTIFY_ERROR( IError_v1::kEDDataModel, kDMECArrayItemTypeDifferent,
				"node type is different than what currently array can hold", IError_v1::kESOperationFatal,
				true, static_cast< uint64 >( currentChildType ), true, static_cast< uint64 >( node->GetNodeType() ) );
		}
		return false;
	}

	spIArrayNode IArrayNode_I::CreateArrayNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name, eArrayForm arrayForm ) {
		return MakeUncheckedSharedPointer( new ArrayNodeImpl(
			nameSpace ? nameSpace->c_str() : NULL, nameSpace ? nameSpace->size() : 0,
			name ? name->c_str() : NULL, name ? name->size() : 0, arrayForm ), __FILE__, __LINE__, true );
	}

	template<>
	spINode TNodeIteratorImpl< ArrayNodeImpl::NodeVector::iterator >::GetNodeFromIterator( const ArrayNodeImpl::NodeVector::iterator & it ) const {
		return MakeUncheckedSharedPointer( it->get(), __FILE__, __LINE__, false );
	}
}

#if BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
namespace AdobeXMPCore {
	using namespace AdobeXMPCore_Int;
	spIArrayNode IArrayNode_v1::MakeShared( pIArrayNode_base ptr ) {
		if ( !ptr ) return spIArrayNode();
		pIArrayNode p = IArrayNode::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IArrayNode >() : ptr;
		return MakeUncheckedSharedPointer( p, __FILE__, __LINE__, false );
	}

	spIArrayNode IArrayNode_v1::CreateUnorderedArrayNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		return MakeUncheckedSharedPointer( new ArrayNodeImpl( nameSpace, nameSpaceLength, name, nameLength, IArrayNode::kAFUnordered ), __FILE__, __LINE__, true );
	}

	spIArrayNode IArrayNode_v1::CreateOrderedArrayNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		return MakeUncheckedSharedPointer( new ArrayNodeImpl( nameSpace, nameSpaceLength, name, nameLength, IArrayNode::kAFOrdered ), __FILE__, __LINE__, true );
	}

	spIArrayNode IArrayNode_v1::CreateAlternativeArrayNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		return MakeUncheckedSharedPointer( new ArrayNodeImpl( nameSpace, nameSpaceLength, name, nameLength, IArrayNode::kAFAlternative ), __FILE__, __LINE__, true );
	}

}
#endif  // BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
