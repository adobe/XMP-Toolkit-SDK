// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
	class IStructureNodeProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::IStructureNodeProxy;

#include "XMPCore/Interfaces/IStructureNode.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCore/Interfaces/ICoreObjectFactory.h"
#include <assert.h>

namespace AdobeXMPCore {


	IStructureNodeProxy::IStructureNodeProxy( pIStructureNode ptr ) : mRawPtr( ptr )
		, ICompositeNodeProxy( ptr )
		, INodeProxy( ptr ) { }

	IStructureNodeProxy::~IStructureNodeProxy() __NOTHROW__ { }

	AdobeXMPCore_Int::pIStructureNode_I APICALL IStructureNodeProxy::GetIStructureNode_I() __NOTHROW__ {
		return mRawPtr->GetIStructureNode_I();
	}

	INode_v1::eNodeType APICALL IStructureNodeProxy::GetChildNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) const {
		return CallConstSafeFunction< IStructureNode_v1, eNodeType, uint32, const char *, sizet, const char *, sizet >(
			mRawPtr, &IStructureNode_v1::getChildNodeType, nameSpace, nameSpaceLength, name, nameLength );
	}

	spINode APICALL IStructureNodeProxy::GetNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		return CallSafeFunctionReturningPointer< IStructureNode_v1, pINode_base, INode, const char *, sizet, const char *, sizet >(
			mRawPtr, &IStructureNode_v1::getNode, nameSpace, nameSpaceLength, name, nameLength );
	}

	void APICALL IStructureNodeProxy::InsertNode( const spINode & node ) {
		return CallSafeFunctionReturningVoid< IStructureNode_v1, pINode_base >(
			mRawPtr, &IStructureNode_v1::insertNode, node ? node->GetActualINode() : NULL );
	}

	spINode APICALL IStructureNodeProxy::ReplaceNode( const spINode & node ) {
		return CallSafeFunctionReturningPointer< IStructureNode_v1, pINode_base, INode, pINode_base >(
			mRawPtr, &IStructureNode_v1::replaceNode, node ? node->GetActualINode() : NULL );
	}

	spINode APICALL IStructureNodeProxy::RemoveNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		return CallSafeFunctionReturningPointer< IStructureNode_v1, pINode_base, INode, const char *, sizet, const char *, sizet >(
			mRawPtr, &IStructureNode_v1::removeNode, nameSpace, nameSpaceLength, name, nameLength );
	}

	uint32 APICALL IStructureNodeProxy::getChildNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->getChildNodeType( nameSpace, nameSpaceLength, name, nameLength, error );
	}

	pINode_base APICALL IStructureNodeProxy::getNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->getNode( nameSpace, nameSpaceLength, name, nameLength, error );
	}

	void APICALL IStructureNodeProxy::insertNode( pINode_base node, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->insertNode( node, error );
	}

	pINode_base APICALL IStructureNodeProxy::replaceNode( pINode_base node, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->replaceNode( node, error );
	}

	pINode_base APICALL IStructureNodeProxy::removeNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->removeNode( nameSpace, nameSpaceLength, name, nameLength, error );
	}

	pIStructureNode APICALL IStructureNodeProxy::GetActualIStructureNode() __NOTHROW__ {
		return mRawPtr;
	}

	spIStructureNode IStructureNode_v1::MakeShared( pIStructureNode_base ptr ) {
		if ( !ptr ) return spIStructureNode();
		pIStructureNode p = IStructureNode::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IStructureNode >() : ptr;
		return shared_ptr< IStructureNode >( new IStructureNodeProxy( p ) );
	}

	spIStructureNode IStructureNode_v1::CreateStructureNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pIStructureNode_base, IStructureNode, const char *, sizet, const char *, sizet >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::CreateStructureNode, nameSpace, nameSpaceLength, name, nameLength );
	}

}

#endif  // BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB
