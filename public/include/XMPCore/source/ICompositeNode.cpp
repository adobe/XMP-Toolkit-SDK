// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
	class ICompositeNodeProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::ICompositeNodeProxy;

#include "XMPCore/Interfaces/ICompositeNode.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include <assert.h>
#include "XMPCore/Interfaces/IPath.h"
#include "XMPCore/Interfaces/INodeIterator.h"

namespace AdobeXMPCore {
	ICompositeNodeProxy::ICompositeNodeProxy( pICompositeNode ptr )
		: mRawPtr( ptr )
		, INodeProxy( ptr ) {}

	ICompositeNodeProxy::~ICompositeNodeProxy() __NOTHROW__ {}

	pICompositeNode APICALL ICompositeNodeProxy::GetActualICompositeNode() __NOTHROW__ { return mRawPtr; }

	AdobeXMPCore_Int::pICompositeNode_I APICALL ICompositeNodeProxy::GetICompositeNode_I() __NOTHROW__ {
		return mRawPtr->GetICompositeNode_I();
	}

	INode_v1::eNodeType APICALL ICompositeNodeProxy::GetNodeTypeAtPath( const spcIPath & path ) const {
		return CallConstSafeFunction< ICompositeNode_v1, eNodeType, uint32, pcIPath_base >(
			mRawPtr, &ICompositeNode_v1::getNodeTypeAtPath, path ? path->GetActualIPath() : NULL );
	}

	spINode APICALL ICompositeNodeProxy::GetNodeAtPath( const spcIPath & path ) {
		return CallSafeFunctionReturningPointer< ICompositeNode_v1, pINode_base, INode, pcIPath_base >(
			mRawPtr, &ICompositeNode_v1::getNodeAtPath, path ? path->GetActualIPath() : NULL );
	}

	void APICALL ICompositeNodeProxy::AppendNode( const spINode & node ) {
		return CallSafeFunctionReturningVoid< ICompositeNode_v1, pINode_base >(
			mRawPtr, &ICompositeNode_v1::appendNode, node ? node->GetActualINode() : NULL );
	}

	void APICALL ICompositeNodeProxy::InsertNodeAtPath( const spINode & node, const spcIPath & path ) {
		return CallSafeFunctionReturningVoid< ICompositeNode_v1, pINode_base, pcIPath_base >(
			mRawPtr, &ICompositeNode_v1::insertNodeAtPath, node ? node->GetActualINode() : NULL, path ? path->GetActualIPath() : NULL );
	}

	spINode APICALL ICompositeNodeProxy::ReplaceNodeAtPath( const spINode & node, const spcIPath & path ) {
		return CallSafeFunctionReturningPointer< ICompositeNode_v1, pINode_base, INode, pINode_base, pcIPath_base >(
			mRawPtr, &ICompositeNode_v1::replaceNodeAtPath, node ? node->GetActualINode() : NULL, path ? path->GetActualIPath() : NULL );
	}

	spINode APICALL ICompositeNodeProxy::RemoveNodeAtPath( const spcIPath & path ) {
		return CallSafeFunctionReturningPointer< ICompositeNode_v1, pINode_base, INode, pcIPath_base >(
			mRawPtr, &ICompositeNode_v1::removeNodeAtPath, path ? path->GetActualIPath() : NULL );
	}

	spINodeIterator APICALL ICompositeNodeProxy::Iterator() {
		return CallSafeFunctionReturningPointer< ICompositeNode_v1, pINodeIterator_base, INodeIterator >(
			mRawPtr, &ICompositeNode_v1::iterator );
	}

	sizet APICALL ICompositeNodeProxy::ChildCount() const __NOTHROW__ {
		return mRawPtr->ChildCount();
	}

	uint32 APICALL ICompositeNodeProxy::getNodeTypeAtPath( pcIPath_base path, pcIError_base & error ) const  __NOTHROW__ {
		assert( false );
		return mRawPtr->getNodeTypeAtPath( path, error );
	}

	pINode_base APICALL ICompositeNodeProxy::getNodeAtPath( pcIPath_base path, pcIError_base & error )  __NOTHROW__ {
		assert( false );
		return mRawPtr->getNodeAtPath( path, error );
	}

	void APICALL ICompositeNodeProxy::appendNode( pINode_base node, pcIError_base & error )  __NOTHROW__ {
		assert( false );
		return mRawPtr->appendNode( node, error );
	}

	void APICALL ICompositeNodeProxy::insertNodeAtPath( pINode_base node, pcIPath_base path, pcIError_base & error )  __NOTHROW__ {
		assert( false );
		return mRawPtr->insertNodeAtPath( node, path, error );
	}

	pINode_base APICALL ICompositeNodeProxy::replaceNodeAtPath( pINode_base node, pcIPath_base path, pcIError_base & error )  __NOTHROW__ {
		assert( false );
		return mRawPtr->replaceNodeAtPath( node, path, error );
	}

	pINode_base APICALL ICompositeNodeProxy::removeNodeAtPath( pcIPath_base path, pcIError_base & error )  __NOTHROW__ {
		assert( false );
		return mRawPtr->removeNodeAtPath( path, error );
	}

	pINodeIterator_base APICALL ICompositeNodeProxy::iterator( pcIError_base & error )  __NOTHROW__ {
		assert( false );
		return mRawPtr->iterator( error );
	}

	spICompositeNode ICompositeNode_v1::MakeShared( pICompositeNode_base ptr ) {
		if ( !ptr ) return spICompositeNode();
		pICompositeNode p = ICompositeNode::GetInterfaceVersion() > 1 ?
			ptr->GetInterfacePointer< ICompositeNode >() : ptr;
		return shared_ptr< ICompositeNode >( new ICompositeNodeProxy( p ) );
	}

}

#endif  // BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB
