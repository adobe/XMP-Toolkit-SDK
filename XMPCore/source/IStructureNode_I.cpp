// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/Interfaces/IStructureNode_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"

namespace AdobeXMPCore_Int {

	pvoid APICALL IStructureNode_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL IStructureNode_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< IStructureNode_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, &IStructureNode_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL IStructureNode_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kIStructureNodeID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< IStructureNode_v1 * >( this );
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
			returnValue = ICompositeNode_I::GetInterfacePointerInternal( interfaceID, interfaceVersion, false );
			if ( returnValue ) return returnValue;
		}
		if ( isTopLevel )
			throw IError_I::CreateInterfaceNotAvailableError(
				IError_v1::kESOperationFatal, kIStructureNodeID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

	pINode_base APICALL IStructureNode_I::getNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error )  __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IStructureNode_v1, pINode_base, INode, const char *, sizet, const char *, sizet >(
			error, this, &IStructureNode_v1::GetNode, __FILE__, __LINE__, nameSpace, nameSpaceLength, name, nameLength );
	}

	uint32 APICALL IStructureNode_I::getChildNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) const  __NOTHROW__ {
		return CallConstUnSafeFunction< IStructureNode_v1, uint32, eNodeType, const char *, sizet, const char *, sizet >(
			error, this, 0, &IStructureNode_v1::GetChildNodeType, __FILE__, __LINE__, nameSpace, nameSpaceLength, name, nameLength );
	}

	void APICALL IStructureNode_I::insertNode( pINode_base node, pcIError_base & error )  __NOTHROW__ {
		return CallUnSafeFunctionReturningVoid< IStructureNode_v1, const spINode & >(
			error, this, &IStructureNode_v1::InsertNode, __FILE__, __LINE__, INode::MakeShared( node ) );
	}

	pINode_base APICALL IStructureNode_I::removeNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error )  __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IStructureNode_v1, pINode_base, INode, const char *, sizet, const char *, sizet >(
			error, this, &IStructureNode_v1::RemoveNode, __FILE__, __LINE__, nameSpace, nameSpaceLength, name, nameLength );
	}

	pINode_base APICALL IStructureNode_I::replaceNode( pINode_base node, pcIError_base & error )  __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IStructureNode_v1, pINode_base, INode, const spINode & >(
			error, this, &IStructureNode_v1::ReplaceNode, __FILE__, __LINE__, INode::MakeShared( node ) );
	}

}
