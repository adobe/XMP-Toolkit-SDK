// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/SimpleNodeImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCommon/Utilities/AutoSharedLock.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"

#include <assert.h>

namespace AdobeXMPCore_Int {

	SimpleNodeImpl::SimpleNodeImpl( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, const char * value, sizet valueLength )
		: NodeImpl( nameSpace, nameSpaceLength, name, nameLength )
		, mValue( IUTF8String_I::CreateUTF8String( value, valueLength ) )
		, mIsURIType( false ) { }

	spcIUTF8String APICALL SimpleNodeImpl::GetValue() const {
		AutoSharedLock lock( mSharedMutex );
		return mValue;
	}

	void APICALL SimpleNodeImpl::SetValue( const char * value, sizet valueLength ) {
		AutoSharedLock lock( mSharedMutex, true );
		mValue->assign( value, valueLength );
		RegisterChange();
	}

	bool APICALL SimpleNodeImpl::IsURIType() const {
		AutoSharedLock lock( mSharedMutex );
		return mIsURIType;
	}

	void APICALL SimpleNodeImpl::SetURIType( bool isURI ) {
		AutoSharedLock( mSharedMutex, true );
		mIsURIType = isURI;
		RegisterChange();
	}

	INode_v1::eNodeType APICALL SimpleNodeImpl::GetNodeType() const {
		return INode_v1::kNTSimple;
	}

	bool APICALL SimpleNodeImpl::HasContent() const {
		AutoSharedLock lock( mSharedMutex );
		return !mValue->empty();
	}

	void APICALL SimpleNodeImpl::ClearContents() {
		AutoSharedLock  lock( mSharedMutex, true );
		mValue->clear();
		RegisterChange();
	}

	spINode APICALL SimpleNodeImpl::CloneContents( bool ignoreEmptyNodes, bool ignoreNodesWithOnlyQualifiers, sizet qualifiersCount ) const {
		if ( ignoreEmptyNodes && mValue->empty() ) {
			if ( ignoreNodesWithOnlyQualifiers )
				return spINode();
			else if ( !ignoreNodesWithOnlyQualifiers && qualifiersCount == 0 )
				return spINode();
		}

		spISimpleNode newNode = ISimpleNode_I::CreateSimpleNode( mNameSpace, mName, mValue );
		newNode->SetURIType( mIsURIType );
		return newNode;
	}

	void SimpleNodeImpl::resetChangesForChildren() const { }

	spISimpleNode APICALL SimpleNodeImpl::ConvertToSimpleNode() {
		return MakeUncheckedSharedPointer( this, __FILE__, __LINE__ );
	}

	spISimpleNode ISimpleNode_I::CreateSimpleNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name, const spcIUTF8String & value ) {
		return MakeUncheckedSharedPointer(
			new SimpleNodeImpl( nameSpace->c_str(), nameSpace->size(), name->c_str(), name->size(),
				value ? value->c_str() : NULL, value ? value->size() : AdobeXMPCommon::npos ), __FILE__, __LINE__, true );
	}

}

namespace AdobeXMPCore {
	using namespace AdobeXMPCore_Int;
	spISimpleNode ISimpleNode::CreateSimpleNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, const char * value, sizet valueLength ) {
		return MakeUncheckedSharedPointer( new SimpleNodeImpl( nameSpace, nameSpaceLength, name, nameLength, value, valueLength ), __FILE__, __LINE__, true );
	}

	spISimpleNode ISimpleNode_v1::MakeShared( pISimpleNode_base ptr ) {
		if ( !ptr ) return spISimpleNode();
		pISimpleNode p = ISimpleNode::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< ISimpleNode >() : ptr;
		return MakeUncheckedSharedPointer( p, __FILE__, __LINE__, false );
	}

}
