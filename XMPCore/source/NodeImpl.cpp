// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/NodeImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCommon/Utilities/AutoSharedLock.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"
#include "XMPCore/Interfaces/IPath.h"
#include "XMPCore/Interfaces/IPathSegment_I.h"
#include "XMPCore/Interfaces/IStructureNode_I.h"

#include "source/XMP_LibUtils.hpp"
#include "source/UnicodeInlines.incl_cpp"
#include <cstring>
#include <algorithm>

namespace AdobeXMPCore_Int {

	// All virtual functions
	static bool VerifyNameSpace( const char * nameSpace, sizet nameSpaceLength ) {
		if ( ( nameSpace == NULL ) || ( nameSpaceLength == 0 )  || ( nameSpaceLength == AdobeXMPCommon::npos && strlen( nameSpace ) == 0 ) ) {
			NOTIFY_ERROR( IError_v1::kEDGeneral, kGECParametersNotAsExpected,
				"nameSpace can't be null or empty", IError_v1::kESOperationFatal,
				true, ( void * ) nameSpace, true, nameSpaceLength, nameSpace, nameSpace );
			return false;
		}
		return true;
	}

	static bool VerifyName( const char * name, sizet nameLength ) {
		if ( ( name == NULL ) || ( nameLength == 0 ) || ( nameLength == AdobeXMPCommon::npos && strlen( name ) == 0 ) ) {
			NOTIFY_ERROR( IError_v1::kEDGeneral, kGECParametersNotAsExpected,
				"name can't be null or empty", IError_v1::kESOperationFatal,
				true, ( void * ) name, true, nameLength, name, name );
			return false;
		}
		if ( nameLength == AdobeXMPCommon::npos ) nameLength = strlen( name );
		try {
			if ( nameLength != 2 && strncmp( name, "[]", 2 ) != 0 )
				VerifySimpleXMLName( name, name + nameLength );
		} catch ( ... ) {
			NOTIFY_ERROR( IError_v1::kEDDataModel, kDMECBadXPath,
				"name is not a valid XML Name", IError_base::kESOperationFatal,
				true, name );
			return false;
		}
		return true;
	}

	static spcIPathSegment CreatePathSegmentToParent( pcINode node ) {
		if ( node->IsQualifierNode() ) {
			return IPathSegment_I::CreateQualifierPathSegment( node->GetNameSpace(), node->GetName() );
		}
		if ( node->IsArrayItem() ) {
			return IPathSegment_I::CreateArrayIndexPathSegment( node->GetNameSpace(), node->GetIndex() );
		}
		return IPathSegment_I::CreatePropertyPathSegment( node->GetNameSpace(), node->GetName() );
	}

	static const char * kQualifierNodeNameSpace( "http://qualifiers" );
	static const AdobeXMPCommon::sizet kQualifiersNodeNameSpaceLength( 17 );
	static const char * kQualifierNodeLocalName( "_qualifiers_" );
	static const AdobeXMPCommon::sizet kQualifierNodeLocalNameLength( 12 );

	NodeImpl::NodeImpl( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength )
		: mNameSpace( IUTF8String_I::CreateUTF8String( nameSpace, nameSpaceLength ) )
		, mName( IUTF8String_I::CreateUTF8String( name, nameLength ) )
		, mIndex( 0 )
		, mpParent( NULL )
		, mspParent()
		, mChangeCount( 0 )
		, mIsQualifierNode( false )
	{
		VerifyNameSpace( nameSpace, nameSpaceLength );
		VerifyName( name, nameLength );
	}

	void APICALL NodeImpl::ChangeParent( pINode parent ) {
		AutoSharedLock lock( mSharedMutex, true );
		if ( mspParent ) {
			if ( mspParent.get() == parent ) {
				return;
			} else {
				mspParent.reset();
				if ( mChangeCount > 1 ) {
					mpParent->GetINode_I()->UnRegisterChange();
					if ( parent ) parent->GetINode_I()->RegisterChange();
				}
				mpParent = parent;
				updateParentSharedPointer();
			}
		} else {
			if ( mChangeCount > 1 ) {
				if ( mpParent ) mpParent->GetINode_I()->UnRegisterChange();
				if ( parent ) parent->GetINode_I()->RegisterChange();
			}
			if ( parent ) parent->GetINode_I()->RegisterChange();
			mpParent = parent;
			updateParentSharedPointer();
		}

		if ( !mpParent ) {
			mIsQualifierNode = false;
			mIndex = 0;
		}
	}

	pINode APICALL NodeImpl::GetRawParentPointer() {
		AutoSharedLock lock( mSharedMutex );
		return mpParent;
	}

	spINode APICALL NodeImpl::GetParent() {
		AutoSharedLock lock( mSharedMutex );
		if ( mpParent ) {
			if ( mIsQualifierNode )
				return MakeUncheckedSharedPointer( mpParent->GetINode_I()->GetRawParentPointer(), __FILE__, __LINE__ );
			else
				return MakeUncheckedSharedPointer( mpParent, __FILE__, __LINE__ );
		}
		else
			return spINode();
	}

	void APICALL NodeImpl::SetName( const char * name, sizet nameLength ) {
		if ( VerifyName( name, nameLength ) ) {
			spIUTF8String newName = IUTF8String_I::CreateUTF8String( name, nameLength );
			if ( mName->compare( newName ) == 0 ) return;
			if ( mpParent ) {
				if ( mpParent->GetINode_I()->ValidateNameOrNameSpaceChangeForAChild( mNameSpace, mName, mNameSpace, newName ) ) {
					AutoSharedLock( mSharedMutex, true );
					mName = newName;
					RegisterChange();
				} else {
					NOTIFY_ERROR( IError_v1::kEDDataModel, kDMECNodeAlreadyExists,
						"One of the sibling have same name and nameSpace combination", IError_v1::kESOperationFatal,
						true, mNameSpace->c_str(), true, mName->c_str(), true, mNameSpace->c_str(), true, newName->c_str() );
				}
			} else {
				AutoSharedLock( mSharedMutex, true );
				mName = newName;
				RegisterChange();
			}
		}
	}

	spcIUTF8String APICALL NodeImpl::GetName() const {
		if ( this->IsArrayItem() )
			return mpParent->GetName();
		AutoSharedLock lock( mSharedMutex );
		return mName;
	}

	void APICALL NodeImpl::SetNameSpace( const char * nameSpace, sizet nameSpaceLength ) {
		if ( VerifyNameSpace( nameSpace, nameSpaceLength ) ) {
			spIUTF8String newNameSpace = IUTF8String_I::CreateUTF8String( nameSpace, nameSpaceLength );
			if ( mNameSpace->compare( newNameSpace ) == 0 ) return;
			if ( mpParent ) {
				if ( mpParent->GetINode_I()->ValidateNameOrNameSpaceChangeForAChild( mNameSpace, mName, newNameSpace, mName ) ) {
					AutoSharedLock( mSharedMutex, true );
					mNameSpace = newNameSpace;
					RegisterChange();
				} else {
					NOTIFY_ERROR( IError_v1::kEDDataModel, kDMECNodeAlreadyExists,
						"One of the sibling have same name and nameSpace combination", IError_v1::kESOperationFatal,
						true, mNameSpace->c_str(), true, mName->c_str(), true, newNameSpace->c_str(), true, mName->c_str() );
				}
			} else {
				AutoSharedLock( mSharedMutex, true );
				mNameSpace = newNameSpace;
				RegisterChange();
			}
		}
	}

	spcIUTF8String APICALL NodeImpl::GetNameSpace() const {
		if ( this->IsArrayItem() )
			return( mpParent->GetNameSpace() );
		AutoSharedLock lock( mSharedMutex );
		return mNameSpace;
	}

	spIPath APICALL NodeImpl::GetPath() const {
		typedef std::vector< pcINode, TAllocator< pcINode > > pcINodeList;
		pcINodeList pathToParent;

		pcINode node = this;

		while ( node != NULL ) {
			pcINode_I node_I = node->GetINode_I();
		
			pathToParent.push_back( node );
			node = node_I->GetRawParentPointer();
		}
		
		// reverse the array
		std::reverse( pathToParent.begin(), pathToParent.end() );
		spIPath xmpPath = IPath::CreatePath();
		pcINode parent = NULL;

		sizet nElements = pathToParent.size();
		for ( sizet i = 0; i < nElements; i++ ) {
			if ( pathToParent[ i ]->GetINode_I()->GetRawParentPointer() == NULL ) {
				continue;
			}

			spcIPathSegment segment = CreatePathSegmentToParent( pathToParent[ i ] );

			if ( segment ) {
				xmpPath->AppendPathSegment( segment );
			}

			parent = pathToParent[ i ];
		}
		return xmpPath;
	}

	spINode APICALL NodeImpl::GetQualifier( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		{
			AutoSharedLock lock( mSharedMutex );
			if ( !mQualifiers ) return spINode();
		}
		return mQualifiers->GetNode( nameSpace, nameSpaceLength, name, nameLength );
	}

	spINode APICALL NodeImpl::GetQualifier( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) {
		{
			AutoSharedLock lock( mSharedMutex );
			if ( !mQualifiers ) return spINode();
		}
		return mQualifiers->GetIStructureNode_I()->GetNode( nameSpace, name );
	}

	void APICALL NodeImpl::InsertQualifier( const spINode & node ) {
		CreateQualifierNode();
		mQualifiers->InsertNode( node );
		node->GetINode_I()->SetIsQualifierNode( true );
	}

	spINode APICALL NodeImpl::ReplaceQualifier( const spINode & node ) {
		CreateQualifierNode();
		auto retValue = mQualifiers->ReplaceNode( node );
		node->GetINode_I()->SetIsQualifierNode( true );
		return retValue;
	}

	spINode APICALL NodeImpl::RemoveQualifier( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		CreateQualifierNode();
		return mQualifiers->RemoveNode( nameSpace, nameSpaceLength, name, nameLength );
	}

	spINode APICALL NodeImpl::RemoveQualifier( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) {
		CreateQualifierNode();
		return mQualifiers->GetIStructureNode_I()->RemoveQualifier( nameSpace, name );
	}

	bool APICALL NodeImpl::IsArrayItem() const {
		AutoSharedLock lock( mSharedMutex );
		return mIndex > 0;
	}

	bool APICALL NodeImpl::HasQualifiers() const {
		{
			AutoSharedLock lock( mSharedMutex );
			if ( !mQualifiers ) return false;
		}
		return mQualifiers->ChildCount() > 0;
	}

	bool APICALL NodeImpl::IsEmpty() const {
		return !HasContent() && !HasQualifiers();
	}

	bool APICALL NodeImpl::HasChanged() const {
		return mChangeCount != 0;
	}

	void APICALL NodeImpl::AcknowledgeChanges() const __NOTHROW__ {
		{
			AutoSharedLock lock( mSharedMutex );
			if ( mChangeCount > 1 && mpParent )
				mpParent->GetINode_I()->UnRegisterChange();
		}
		
		resetChangesForChildren();

		{
			AutoSharedLock lock( mSharedMutex );
			if ( mQualifiers )
				mQualifiers->AcknowledgeChanges();
		}
		AutoSharedLock lock( mSharedMutex, true );
		mChangeCount = 0;
	}

	void APICALL NodeImpl::Clear( bool contents, bool qualifiers ) {
		if ( qualifiers && mQualifiers ) {
			mQualifiers->Clear();
		}
		if ( contents ) {
			ClearContents();
			RegisterChange();
		}
	}

	spINode APICALL NodeImpl::Clone( bool ignoreEmptyNodes, bool ignoreNodesWithOnlyQualifiers ) const {
		AutoSharedLock lock( mSharedMutex );

		sizet qualifiersCount( mQualifiers ? mQualifiers->ChildCount() : 0 );
		
		spIStructureNode qualifierNode;
		// copy the qualifiers
		if ( qualifiersCount > 0 ) {
			auto node = mQualifiers->Clone( ignoreEmptyNodes, ignoreNodesWithOnlyQualifiers );
			if ( node ) qualifierNode = node->ConvertToStructureNode();
			qualifiersCount = qualifierNode ? qualifierNode->ChildCount() : 0;
		}
		
		spINode newNode = CloneContents( ignoreEmptyNodes, ignoreNodesWithOnlyQualifiers, qualifiersCount );

		if ( newNode )
			newNode->GetINode_I()->SetIsQualifierNode( newNode->IsQualifierNode() );

		// set the qualifier
		if ( newNode && qualifierNode && qualifiersCount > 0 ) {
			newNode->GetINode_I()->SetQualifiers( qualifierNode );
		}

		return newNode;
	}

	void NodeImpl::updateParentSharedPointer( bool calledFromRelease ) {
		if ( !calledFromRelease ) {
			if ( !mspParent && mRefCount > 1 && mpParent )
				mspParent = MakeUncheckedSharedPointer( mpParent, __FILE__, __LINE__, true );
		} else {
			if ( mspParent && mRefCount <= 2 )
				mspParent.reset();
		}
	}

	void NodeImpl::CreateQualifierNode() {
		AutoSharedLock( mSharedMutex, true );
		if ( !mQualifiers ) {
			mQualifiers = IStructureNode::CreateStructureNode( kQualifierNodeNameSpace, kQualifierNodeLocalNameLength, kQualifierNodeLocalName, kQualifierNodeLocalNameLength );
			mQualifiers->GetINode_I()->ChangeParent( this );
		}
	}

	void NodeImpl::SetQualifiers( const spIStructureNode & node ) {
		AutoSharedLock( mSharedMutex, true );
		mQualifiers = node;
	}

	sizet APICALL NodeImpl::QualifiersCount() const __NOTHROW__ {
		{
			AutoSharedLock lock( mSharedMutex );
			if ( !mQualifiers ) return 0;
		}
		return mQualifiers->ChildCount();
	}

	spINodeIterator APICALL NodeImpl::QualifiersIterator() {
		{
			AutoSharedLock lock( mSharedMutex );
			if ( !mQualifiers ) return spINodeIterator();
		}
		return mQualifiers->Iterator();
	}

	bool NodeImpl::ValidateNameOrNameSpaceChangeForAChild( const spcIUTF8String & currentNameSpace, const spcIUTF8String & currentName, const spcIUTF8String & newNameSpace, const spcIUTF8String & newName ) {
		return true;
	}

	INode_v1::eNodeType APICALL NodeImpl::GetParentNodeType() const {
		if ( mpParent )
			return mpParent->GetNodeType();
		return INode_v1::kNTNone;
	}

	INode_v1::eNodeType APICALL NodeImpl::GetQualifierNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) const {
		{
			AutoSharedLock lock( mSharedMutex );
			if ( !mQualifiers ) return INode::kNTNone;
		}
		return mQualifiers->GetChildNodeType( nameSpace, nameSpaceLength, name, nameLength );
	}

	spISimpleNode APICALL NodeImpl::ConvertToSimpleNode() {
		return spISimpleNode();
	}

	spIStructureNode APICALL NodeImpl::ConvertToStructureNode() {
		return spIStructureNode();
	}

	spIArrayNode APICALL NodeImpl::ConvertToArrayNode() {
		return spIArrayNode();
	}

	spIMetadata APICALL NodeImpl::ConvertToMetadata() {
		return spIMetadata();
	}

	void NodeImpl::UnRegisterChange() {
		if ( mChangeCount > 0 )
			mChangeCount--;
		if ( mChangeCount == 0 && mpParent ) {
			mpParent->GetINode_I()->UnRegisterChange();
		}
	}

	void APICALL NodeImpl::Acquire() const __NOTHROW__ {
		SharedObjectImpl::Acquire();
		AutoSharedLock lock( mSharedMutex, true );
		const_cast< NodeImpl * >( this )->updateParentSharedPointer();
	}

	void APICALL NodeImpl::Release() const __NOTHROW__ {
		AutoSharedLock lock( mSharedMutex, true );
		const_cast< NodeImpl * >( this )->updateParentSharedPointer( true );
		SharedObjectImpl::Release();
	}

	void APICALL NodeImpl::AcquireInternal() const __NOTHROW__ {
		SharedObjectImpl::AcquireInternal();
		AutoSharedLock lock( mSharedMutex, true );
		const_cast< NodeImpl * >( this )->updateParentSharedPointer();
	}

	void NodeImpl::RegisterChange() {
		mChangeCount++;
		if ( mChangeCount == 1 ) {
			auto parent = GetRawParentPointer();
			if ( parent ) parent->GetINode_I()->RegisterChange();
		}
	}

	void NodeImpl::SetIndex( sizet currentIndex ) {
		mIndex = currentIndex;
	}

	void NodeImpl::SetIsQualifierNode( bool isQualifierNode ) {
		mIsQualifierNode = isQualifierNode;
	}

	bool APICALL NodeImpl::IsQualifierNode() const {
		return mIsQualifierNode;
	}

	sizet APICALL NodeImpl::GetIndex() const {
		return mIndex;
	}

}

namespace AdobeXMPCore {
	using namespace AdobeXMPCore_Int;
	spINode INode_v1::MakeShared( pINode_base ptr ) {
		if ( !ptr ) return spINode();
		pINode p = INode::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< INode >() : ptr;
		return MakeUncheckedSharedPointer( p, __FILE__, __LINE__, false );
	}

}
