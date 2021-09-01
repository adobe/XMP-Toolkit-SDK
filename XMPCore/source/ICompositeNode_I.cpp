// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/Interfaces/ICompositeNode_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/Interfaces/IPath.h"
#include "XMPCore/Interfaces/INodeIterator.h"

namespace AdobeXMPCore_Int {

	pvoid APICALL ICompositeNode_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL ICompositeNode_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< ICompositeNode_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, &ICompositeNode_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL ICompositeNode_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kICompositeNodeID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< ICompositeNode_v1 * >( this );
				break;

			case kInternalInterfaceVersionNumber:
				return this;
				break;

			default:
				throw IError_I::CreateInterfaceVersionNotAvailableError(
					IError_v1::kESOperationFatal, interfaceID, interfaceVersion, __FILE__, __LINE__ );
				break;
			}
		} else {
			pvoid returnValue( NULL );
			returnValue = INode_I::GetInterfacePointerInternal( interfaceID, interfaceVersion, false );
			if ( returnValue ) return returnValue;
		}
		if ( isTopLevel )
			throw IError_I::CreateInterfaceNotAvailableError(
				IError_v1::kESOperationFatal, kICompositeNodeID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

	uint32 APICALL ICompositeNode_I::getNodeTypeAtPath( pcIPath_base path, pcIError_base & error ) const  __NOTHROW__ {
		return CallConstUnSafeFunction< ICompositeNode_v1, uint32, eNodeType, const spcIPath & >(
			error, this, 0, &ICompositeNode_v1::GetNodeTypeAtPath, __FILE__, __LINE__, IPath_v1::MakeShared( path ) );
	}

	pINode_base APICALL ICompositeNode_I::getNodeAtPath( pcIPath_base path, pcIError_base & error )  __NOTHROW__ {
		typedef spINode( APICALL ICompositeNode_v1::*Func )( const spcIPath & );
		Func fnPtr = &ICompositeNode_v1::GetNodeAtPath;
		return CallUnSafeFunctionReturningSharedPointer< ICompositeNode_v1, pINode_base, INode, const spcIPath & >(
			error, this, fnPtr, __FILE__, __LINE__, IPath_v1::MakeShared( path ) );
	}

	void APICALL ICompositeNode_I::appendNode( pINode_base node, pcIError_base & error )  __NOTHROW__ {
		return CallUnSafeFunctionReturningVoid< ICompositeNode_v1, const spINode & >(
			error, this, &ICompositeNode_v1::AppendNode, __FILE__, __LINE__, INode::MakeShared( node ) );
	}

	void APICALL ICompositeNode_I::insertNodeAtPath( pINode_base node, pcIPath_base path, pcIError_base & error )  __NOTHROW__ {
		return CallUnSafeFunctionReturningVoid< ICompositeNode_v1, const spINode &, const spcIPath & >(
			error, this, &ICompositeNode_v1::InsertNodeAtPath, __FILE__, __LINE__, INode::MakeShared( node ), IPath::MakeShared( path ) );
	}

	pINode_base APICALL ICompositeNode_I::replaceNodeAtPath( pINode_base node, pcIPath_base path, pcIError_base & error )  __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< ICompositeNode_v1, pINode_base, INode, const spINode &, const spcIPath & >(
			error, this, &ICompositeNode_v1::ReplaceNodeAtPath, __FILE__, __LINE__, INode_v1::MakeShared( node ), IPath_v1::MakeShared( path ) );
	}

	pINode_base APICALL ICompositeNode_I::removeNodeAtPath( pcIPath_base path, pcIError_base & error )  __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< ICompositeNode_v1, pINode_base, INode, const spcIPath & >(
			error, this, &ICompositeNode_v1::RemoveNodeAtPath, __FILE__, __LINE__, IPath_v1::MakeShared( path ) );
	}

	pINodeIterator_base APICALL ICompositeNode_I::iterator( pcIError_base & error )  __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< ICompositeNode_v1, pINodeIterator_base, INodeIterator >(
			error, this, &ICompositeNode_v1::Iterator, __FILE__, __LINE__ );
	}

}
