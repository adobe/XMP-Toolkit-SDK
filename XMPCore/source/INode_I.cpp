// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/Interfaces/INode_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCore/Interfaces/IPath.h"
#include "XMPCore/Interfaces/ISimpleNode.h"
#include "XMPCore/Interfaces/IMetadata.h"
#include "XMPCore/Interfaces/IArrayNode.h"
#include "XMPCore/Interfaces/INodeIterator.h"

namespace AdobeXMPCore_Int {

	pvoid APICALL INode_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL INode_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< INode_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, &INode_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL INode_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kINodeID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< INode_v1 * >( this );
				break;

			case kInternalInterfaceVersionNumber:
				return this;
				break;

			default:
				throw IError_I::CreateInterfaceVersionNotAvailableError(
					IError_v1::kESOperationFatal, interfaceID, interfaceVersion, __FILE__, __LINE__ );
				break;
			}
		}
		if ( isTopLevel )
			throw IError_I::CreateInterfaceNotAvailableError(
				IError_v1::kESOperationFatal, kINodeID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

	pINode_base APICALL INode_I::getParent( pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< INode_v1, pINode_base, INode >(
			error, this, &INode_v1::GetParent, __FILE__, __LINE__ );
	}

	uint32 APICALL INode_I::getParentNodeType( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< INode_v1, uint32, eNodeType >(
			error, this, 0, &INode_v1::GetParentNodeType, __FILE__, __LINE__ );
	}

	void APICALL INode_I::setName( const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningVoid< INode_v1, const char *, sizet >(
			error, this, &INode_v1::SetName, __FILE__, __LINE__, name, nameLength );
	}

	pcIUTF8String_base APICALL INode_I::getName( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< INode_v1, pcIUTF8String_base, const IUTF8String >(
			error, this, &INode_v1::GetName, __FILE__, __LINE__ );
	}

	void APICALL INode_I::setNameSpace( const char * nameSpace, sizet nameSpaceLength, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningVoid< INode_v1, const char *, sizet >(
			error, this, &INode_v1::SetNameSpace, __FILE__, __LINE__, nameSpace, nameSpaceLength );

	}

	pcIUTF8String_base APICALL INode_I::getNameSpace( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< INode_v1, pcIUTF8String_base, const IUTF8String >(
			error, this, &INode_v1::GetNameSpace, __FILE__, __LINE__ );
	}

	pIPath_base APICALL INode_I::getPath( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< INode_v1, pIPath_base, IPath >(
			error, this, &INode_v1::GetPath, __FILE__, __LINE__ );
	}

	pINodeIterator_base APICALL INode_I::qualifiersIterator( pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< INode_v1, pINodeIterator_base, INodeIterator >(
			error, this, &INode_v1::QualifiersIterator, __FILE__, __LINE__ );
	}

	uint32 APICALL INode_I::getQualifierNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< INode_v1, uint32, eNodeType, const char *, sizet, const char *, sizet >(
			error, this, 0, &INode_v1::GetQualifierNodeType, __FILE__, __LINE__, nameSpace, nameSpaceLength, name, nameLength );
	}

	pINode_base APICALL INode_I::getQualifier( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ {
		typedef spINode( APICALL INode_v1::*Func )( const char *, sizet, const char *, sizet );
		Func fnPtr = &INode_v1::GetQualifier;
		return CallUnSafeFunctionReturningSharedPointer< INode_v1, pINode_base, INode, const char *, sizet, const char *, sizet >(
			error, this, fnPtr, __FILE__, __LINE__, nameSpace, nameSpaceLength, name, nameLength );
	}

	void APICALL INode_I::insertQualifier( pINode_base base, pcIError_base & error ) __NOTHROW__ {
		CallUnSafeFunctionReturningVoid< INode_v1, const spINode & >( error, this, &INode_v1::InsertQualifier, __FILE__, __LINE__, INode_v1::MakeShared( base ) );
	}

	pINode_base APICALL INode_I::replaceQualifier( pINode_base node, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< INode_v1, pINode_base, INode, const spINode & >(
			error, this, &INode_v1::ReplaceQualifier, __FILE__, __LINE__, INode_v1::MakeShared( node ) );
	}

	pINode_base APICALL INode_I::removeQualifier( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< INode_v1, pINode_base, INode, const char *, sizet, const char *, sizet >(
			error, this, &INode_v1::RemoveQualifier, __FILE__, __LINE__, nameSpace, nameSpaceLength, name, nameLength );
	}

	uint32 APICALL INode_I::getNodeType( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< INode_v1, uint32, eNodeType >( error, this, 0, &INode_v1::GetNodeType, __FILE__, __LINE__ );
	}

	uint32 APICALL INode_I::isArrayItem( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< INode_v1, uint32, bool>( error, this, 0, &INode_v1::IsArrayItem, __FILE__, __LINE__ );
	}

	uint32 APICALL INode_I::isQualifierNode( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< INode_v1, uint32, bool >( error, this, 0, &INode_v1::IsQualifierNode, __FILE__, __LINE__ );
	}

	sizet APICALL INode_I::getIndex( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< INode_v1, sizet, sizet >( error, this, 0, &INode_v1::GetIndex, __FILE__, __LINE__ );
	}

	uint32 APICALL INode_I::hasQualifiers( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< INode_v1, uint32, bool>( error, this, 0, &INode_v1::HasQualifiers, __FILE__, __LINE__ );
	}

	uint32 APICALL INode_I::hasContent( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< INode_v1, uint32, bool>( error, this, 0, &INode_v1::HasContent, __FILE__, __LINE__ );
	}

	uint32 APICALL INode_I::isEmpty( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< INode_v1, uint32, bool>( error, this, 0, &INode_v1::IsEmpty, __FILE__, __LINE__ );
	}

	uint32 APICALL INode_I::hasChanged( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< INode_v1, uint32, bool>( error, this, 0, &INode_v1::HasChanged, __FILE__, __LINE__ );
	}

	void APICALL INode_I::clear( uint32 contents, uint32 qualifiers, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningVoid< INode_v1, bool, bool >( error, this, &INode_v1::Clear, __FILE__, __LINE__, contents != 0, qualifiers != 0 );
	}

	pINode_base APICALL INode_I::clone( uint32 igoreEmptyNodes, uint32 ignoreNodesWithOnlyQualifiers, pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< INode_v1, pINode_base, INode, bool, bool >(
			error, this, &INode_v1::Clone, __FILE__, __LINE__, igoreEmptyNodes != 0, ignoreNodesWithOnlyQualifiers != 0 );
	}

	pISimpleNode_base APICALL INode_I::convertToSimpleNode( pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< INode_v1, pISimpleNode_base, ISimpleNode >(
			error, this, &INode_v1::ConvertToSimpleNode, __FILE__, __LINE__ );
	}

	pIStructureNode_base APICALL INode_I::convertToStructureNode( pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< INode_v1, pIStructureNode_base, IStructureNode >(
			error, this, &INode_v1::ConvertToStructureNode, __FILE__, __LINE__ );
	}

	pIArrayNode_base APICALL INode_I::convertToArrayNode( pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< INode_v1, pIArrayNode_base, IArrayNode >(
			error, this, &INode_v1::ConvertToArrayNode, __FILE__, __LINE__ );
	}

	pIMetadata_base APICALL INode_I::convertToMetadata( pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< INode_v1, pIMetadata_base, IMetadata >(
			error, this, &INode_v1::ConvertToMetadata, __FILE__, __LINE__ );
	}

}
