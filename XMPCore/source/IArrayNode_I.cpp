// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/Interfaces/IArrayNode_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"

namespace AdobeXMPCore_Int {

	pvoid APICALL IArrayNode_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL IArrayNode_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< IArrayNode_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, &IArrayNode_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL IArrayNode_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kIArrayNodeID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< IArrayNode_v1 * >( this );
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
				IError_v1::kESOperationFatal, kIArrayNodeID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

	uint32 APICALL IArrayNode_I::getArrayForm( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< IArrayNode_v1, uint32, eArrayForm >(
			error, this, 0, &IArrayNode_v1::GetArrayForm, __FILE__, __LINE__ );
	}

	uint32 APICALL IArrayNode_I::getChildNodeType( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< IArrayNode_v1, uint32, eNodeType >(
			error, this, 0, &IArrayNode_v1::GetChildNodeType, __FILE__, __LINE__ );
	}

	pINode_base APICALL IArrayNode_I::getNodeAtIndex( sizet index, pcIError_base & error ) __NOTHROW__ {
		typedef spINode( APICALL IArrayNode_v1::*Func )( sizet );
		Func fnPtr = &IArrayNode_v1::GetNodeAtIndex;
		return CallUnSafeFunctionReturningSharedPointer< IArrayNode_v1, pINode_base, INode, sizet >(
			error, this, fnPtr, __FILE__, __LINE__, index );
	}

	void APICALL IArrayNode_I::insertNodeAtIndex( pINode_base node, sizet index, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningVoid< IArrayNode_v1, const spINode &, sizet >(
			error, this, &IArrayNode_v1::InsertNodeAtIndex, __FILE__, __LINE__, INode::MakeShared( node ), index );
	}

	pINode_base APICALL IArrayNode_I::removeNodeAtIndex( sizet index, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IArrayNode_v1, pINode_base, INode, sizet >(
			error, this, &IArrayNode_v1::RemoveNodeAtIndex, __FILE__, __LINE__, index );
	}

	pINode_base APICALL IArrayNode_I::replaceNodeAtIndex( pINode_base node, sizet index, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IArrayNode_v1, pINode_base, INode, const spINode &, sizet >(
			error, this, &IArrayNode_v1::ReplaceNodeAtIndex, __FILE__, __LINE__, INode::MakeShared( node ), index );
	}

}
