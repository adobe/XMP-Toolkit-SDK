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
	#include "XMPCore/ImplHeaders/StructureNodeImpl.h"
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
	
	StructureNodeImpl::StructureNodeImpl( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength )
		: NodeImpl( nameSpace, nameSpaceLength, name, nameLength ) { }

	spINode APICALL StructureNodeImpl::GetNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) {
		if ( nameSpace->size() == 0 || name->size() == 0 )
			return spINode();
		QualifiedName qName( nameSpace, name );
		AutoSharedLock lock( mSharedMutex );
		auto it = mChildrenMap.find( qName );
		if ( it != mChildrenMap.end() )
			return MakeUncheckedSharedPointer( it->second.get(), __FILE__, __LINE__ );
		return spINode();
	}

	spINode APICALL StructureNodeImpl::GetNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		spIUTF8String nameSpaceStr( IUTF8String_I::CreateUTF8String( nameSpace, nameSpaceLength ) );
		spIUTF8String nameStr( IUTF8String_I::CreateUTF8String( name, nameLength ) );
		return GetNode( nameSpaceStr, nameStr );
	}

	spINode APICALL StructureNodeImpl::RemoveNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) {
		if ( nameSpace->size() == 0 || name->size() == 0 )
			return spINode();
		QualifiedName qName( nameSpace, name );
		AutoSharedLock lock( mSharedMutex, true );
		auto it = mChildrenMap.find( qName );
		if ( it == mChildrenMap.end() ) {
			return spINode();
		} else {
			spINode node = it->second;
			mChildrenMap.erase( it );
			node->GetINode_I()->ChangeParent( NULL );
			return node;
		}
	}

	spINode APICALL StructureNodeImpl::RemoveNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		spIUTF8String nameSpaceStr( IUTF8String_I::CreateUTF8String( nameSpace, nameSpaceLength ) );
		spIUTF8String nameStr( IUTF8String_I::CreateUTF8String( name, nameLength ) );
		return RemoveNode( nameSpaceStr, nameStr );
	}

	INode_v1::eNodeType APICALL StructureNodeImpl::GetChildNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) const {
		auto node = GetNode( nameSpace, nameSpaceLength, name, nameLength );
		if ( node )
			return node->GetNodeType();
		return INode::kNTNone;
	}

	INode_v1::eNodeType APICALL StructureNodeImpl::GetNodeType() const {
		return INode_v1::kNTStructure;
	}

	void APICALL StructureNodeImpl::InsertNode( const spINode & node ) {
		if ( !CheckSuitabilityToBeUsedAsChildNode( node ) )
			return;
		QualifiedName qName( node->GetNameSpace(), node->GetName() );
		AutoSharedLock lock( mSharedMutex, true );
		auto it = mChildrenMap.find( qName );
		if ( it == mChildrenMap.end() ) {
			mChildrenMap[ qName ] = MakeUncheckedSharedPointer( node.get(), __FILE__, __LINE__ );
			node->GetINode_I()->ChangeParent( this );
		} else {
			NOTIFY_ERROR( IError_v1::kEDDataModel, kDMECNodeAlreadyExists,
				"a node already exists with the same qualified name", IError_v1::kESOperationFatal,
				true, node->GetNameSpace(), true, node->GetName() );
		}
	}

	spINode APICALL StructureNodeImpl::ReplaceNode( const spINode & node ) {
		if ( CheckSuitabilityToBeUsedAsChildNode( node ) && GetNode( node->GetNameSpace(), node->GetName() ) ) {
			auto retValue = RemoveNode( node->GetNameSpace(), node->GetName() );
			InsertNode( node );
			return retValue;
		} else {
			NOTIFY_ERROR( IError_v1::kEDDataModel, kDMECNoSuchNodeExists,
				"no such node exists with the specified qualified name", IError_v1::kESOperationFatal,
				true, node->GetNameSpace(), true, node->GetName() );
		}
		return spINode();
	}

	void APICALL StructureNodeImpl::AppendNode( const spINode & node ) {
		InsertNode( node );
	}

	spINodeIterator APICALL StructureNodeImpl::Iterator() {
		AutoSharedLock lock( mSharedMutex );
		auto beginIt = mChildrenMap.begin(), endIt = mChildrenMap.end();
		if ( beginIt == endIt )
			return spINodeIterator();
		else
			return MakeUncheckedSharedPointer( new TNodeIteratorImpl< QualifiedNameNodeMap::iterator >( beginIt, endIt ), __FILE__, __LINE__, true );
	}

	sizet APICALL StructureNodeImpl::ChildCount() const __NOTHROW__ {
		AutoSharedLock lock( mSharedMutex );
		return mChildrenMap.size();
	}

	spIStructureNode APICALL StructureNodeImpl::ConvertToStructureNode() {
		return MakeUncheckedSharedPointer( this, __FILE__, __LINE__ );
	}

	bool APICALL StructureNodeImpl::HasContent() const {
		AutoSharedLock lock( mSharedMutex );
		return mChildrenMap.size() > 0;
	}

	bool StructureNodeImpl::ValidateNameOrNameSpaceChangeForAChild( const spcIUTF8String & currentNameSpace, const spcIUTF8String & currentName, const spcIUTF8String & newNameSpace, const spcIUTF8String & newName ) {
		auto node = GetNode( newNameSpace, newNameSpace );
		if ( node ) {
			return false;
		} else {
			node = RemoveNode( currentNameSpace, currentName );
			AutoSharedLock( mSharedMutex, true );
			mChildrenMap[ QualifiedName( newNameSpace, newName ) ] = node;
			return true;
		}
	}

	void APICALL StructureNodeImpl::ClearContents() {
		AutoSharedLock lock( mSharedMutex, true );
		for ( auto it = mChildrenMap.begin(), itEnd = mChildrenMap.end(); it != itEnd; ++it ) {
			it->second->GetINode_I()->ChangeParent( NULL );
		}
		mChildrenMap.clear();
	}

	spINode APICALL StructureNodeImpl::CloneContents( bool ignoreEmptyNodes, bool ignoreNodesWithOnlyQualifiers, sizet qualifiersCount ) const {
		AutoSharedLock lock( mSharedMutex );
		spIStructureNode newNode;
		if ( ignoreEmptyNodes && mChildrenMap.size() == 0 ) {
			if ( ignoreNodesWithOnlyQualifiers )
				return newNode;
			else if ( !ignoreNodesWithOnlyQualifiers && qualifiersCount == 0 )
				return newNode;
		}

		newNode = IStructureNode_I::CreateStructureNode( mNameSpace, mName );

		auto endIt = mChildrenMap.end();
		for ( auto it = mChildrenMap.begin(); it != endIt; ++it ) {
			spINode childNode = it->second->Clone( ignoreEmptyNodes, ignoreNodesWithOnlyQualifiers );
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

	void StructureNodeImpl::resetChangesForChildren() const {
		AutoSharedLock lock( mSharedMutex );
		for ( auto it = mChildrenMap.begin(), itEnd = mChildrenMap.end(); it != itEnd; ++it ) {
			it->second->AcknowledgeChanges();
		}
	}

	bool StructureNodeImpl::CompareQualifiedName::operator()( const QualifiedName & key1, const QualifiedName & key2 ) const {
		int result = key1.mNameSpace->compare( key2.mNameSpace );

		if ( result < 0 ) {
			return true;
		} else if ( result == 0 ) {
			result = key1.mName->compare( key2.mName );
			if ( result < 0 )
				return true;
		}
		return false;
	}

	spIStructureNode IStructureNode_I::CreateStructureNode( const spcIUTF8String & nameSpace, const spcIUTF8String name ) {
		return MakeUncheckedSharedPointer( new StructureNodeImpl( 
			nameSpace ? nameSpace->c_str() : NULL, nameSpace ? nameSpace->size() : 0,
			name ? name->c_str() : NULL, name ? name->size(): 0 ), __FILE__, __LINE__ );
	}

	template<>
	spINode TNodeIteratorImpl< StructureNodeImpl::QualifiedNameNodeMap::iterator >::GetNodeFromIterator( const StructureNodeImpl::QualifiedNameNodeMap::iterator & it ) const {
		return MakeUncheckedSharedPointer( it->second.get(), __FILE__, __LINE__, false );
	}
}

#if BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
namespace AdobeXMPCore {
	using namespace AdobeXMPCore_Int;

	spIStructureNode IStructureNode_v1::MakeShared( pIStructureNode_base ptr ) {
		if ( !ptr ) return spIStructureNode();
		pIStructureNode p = IStructureNode::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IStructureNode >() : ptr;
		return MakeUncheckedSharedPointer( p, __FILE__, __LINE__, false );
	}

	spIStructureNode IStructureNode_v1::CreateStructureNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		return MakeUncheckedSharedPointer( new StructureNodeImpl( nameSpace, nameSpaceLength, name, nameLength ), __FILE__, __LINE__, true );
	}
}
#endif  // BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
