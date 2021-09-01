// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
	class INodeProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::INodeProxy;

#include "XMPCore/Interfaces/INode.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCore/Interfaces/IPath.h"
#include "XMPCore/Interfaces/ISimpleNode.h"
#include "XMPCore/Interfaces/INodeIterator.h"
#include "XMPCore/Interfaces/IArrayNode.h"
#include "XMPCore/Interfaces/IMetadata.h"

#include <assert.h>

namespace AdobeXMPCore {

	INodeProxy::INodeProxy( pINode ptr ) : mRawPtr( ptr ) {
		mRawPtr->Acquire();
	}

	INodeProxy::~INodeProxy() __NOTHROW__ {
		mRawPtr->Release();
	}

	pINode APICALL INodeProxy::GetActualINode() __NOTHROW__ {
		return mRawPtr;
	}

	void APICALL INodeProxy::Acquire() const __NOTHROW__ {
		assert( false );
	}

	void APICALL INodeProxy::Release() const __NOTHROW__ {
		assert( false );
	}

	AdobeXMPCommon_Int::pISharedObject_I APICALL INodeProxy::GetISharedObject_I() __NOTHROW__ {
		return mRawPtr->GetISharedObject_I();
	}

	AdobeXMPCore_Int::pINode_I APICALL INodeProxy::GetINode_I() __NOTHROW__ {
		return mRawPtr->GetINode_I();
	}

	pvoid APICALL INodeProxy::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
	}

	spINode APICALL INodeProxy::GetParent() {
		return CallSafeFunctionReturningPointer< INode_v1, pINode_base, INode >(
			mRawPtr, &INode_v1::getParent );
	}

	void APICALL INodeProxy::SetName( const char * name, sizet nameLength ) {
		return CallSafeFunctionReturningVoid< INode_v1, const char *, sizet >(
			mRawPtr, &INode_v1::setName, name, nameLength );
	}

	spcIUTF8String APICALL INodeProxy::GetName() const {
		return CallConstSafeFunctionReturningPointer< INode_v1, pcIUTF8String_base, const IUTF8String >(
			mRawPtr, &INode_v1::getName );
	}

	void APICALL INodeProxy::SetNameSpace( const char * nameSpace, sizet nameSpaceLength ) {
		return CallSafeFunctionReturningVoid< INode_v1, const char *, sizet >(
			mRawPtr, &INode_v1::setNameSpace, nameSpace, nameSpaceLength );

	}

	spcIUTF8String APICALL INodeProxy::GetNameSpace() const {
		return CallConstSafeFunctionReturningPointer< INode_v1, pcIUTF8String_base, const IUTF8String >(
			mRawPtr, &INode_v1::getNameSpace );
	}

	spIPath APICALL INodeProxy::GetPath() const {
		return CallConstSafeFunctionReturningPointer< INode_v1, pIPath_base, IPath >(
			mRawPtr, &INode_v1::getPath );
	}

	sizet APICALL INodeProxy::QualifiersCount() const __NOTHROW__ {
		return mRawPtr->QualifiersCount();
	}

	spINodeIterator APICALL INodeProxy::QualifiersIterator() {
		return CallSafeFunctionReturningPointer< INode_v1, pINodeIterator_base, INodeIterator >(
			mRawPtr, &INode_v1::qualifiersIterator );
	}

	spINode APICALL INodeProxy::GetQualifier( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		return CallSafeFunctionReturningPointer< INode_v1, pINode_base, INode, const char *, sizet, const char *, sizet >(
			mRawPtr, &INode_v1::getQualifier, nameSpace, nameSpaceLength, name, nameLength );
	}

	void APICALL INodeProxy::InsertQualifier( const spINode & node ) {
		return CallSafeFunctionReturningVoid< INode_v1, pINode_base >(
			mRawPtr, &INode_v1::insertQualifier, node ? node->GetActualINode() : NULL );
	}

	spINode APICALL INodeProxy::ReplaceQualifier( const spINode & node ) {
		return CallSafeFunctionReturningPointer< INode_v1, pINode_base, INode, pINode_base >(
			mRawPtr, &INode_v1::replaceQualifier, node ? node->GetActualINode() : NULL );
	}

	spINode APICALL INodeProxy::RemoveQualifier( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		return CallSafeFunctionReturningPointer< INode_v1, pINode_base, INode, const char *, sizet, const char *, sizet >(
			mRawPtr, &INode_v1::removeQualifier, nameSpace, nameSpaceLength, name, nameLength );
	}

	INode_v1::eNodeType APICALL INodeProxy::GetNodeType() const {
		return CallConstSafeFunction< INode_v1, eNodeType, uint32 >(
			mRawPtr, &INode_v1::getNodeType );
	}

	bool APICALL INodeProxy::IsArrayItem() const {
		return CallConstSafeFunction< INode_v1, bool, uint32 >(
			mRawPtr, &INode_v1::isArrayItem );
	}

	bool APICALL INodeProxy::IsQualifierNode() const {
		return CallConstSafeFunction< INode_v1, bool, uint32 >(
			mRawPtr, &INode_v1::isQualifierNode );
	}

	sizet APICALL INodeProxy::GetIndex() const {
		return CallConstSafeFunction< INode_v1, sizet, sizet >(
			mRawPtr, &INode_v1::getIndex );
	}

	bool APICALL INodeProxy::HasQualifiers() const {
		return CallConstSafeFunction< INode_v1, bool, uint32 >(
			mRawPtr, &INode_v1::hasQualifiers );
	}

	bool APICALL INodeProxy::HasContent() const {
		return CallConstSafeFunction< INode_v1, bool, uint32 >(
			mRawPtr, &INode_v1::hasContent );
	}

	bool APICALL INodeProxy::IsEmpty() const {
		return CallConstSafeFunction< INode_v1, bool, uint32 >(
			mRawPtr, &INode_v1::isEmpty );
	}

	bool APICALL INodeProxy::HasChanged() const {
		return CallConstSafeFunction< INode_v1, bool, uint32 >(
			mRawPtr, &INode_v1::hasChanged );
	}

	void APICALL INodeProxy::AcknowledgeChanges() const __NOTHROW__ {
		return mRawPtr->AcknowledgeChanges(  );
	}

	void APICALL INodeProxy::Clear( bool contents, bool qualifiers ) {
		return CallSafeFunctionReturningVoid< INode_v1, uint32, uint32 >(
			mRawPtr, &INode_v1::clear, static_cast< uint32 >( contents ), static_cast< uint32 >( qualifiers ) );
	}

	spINode APICALL INodeProxy::Clone( bool ignoreEmptyNodes, bool ignoreNodesWithOnlyQualifiers ) const {
		return CallConstSafeFunctionReturningPointer< INode_v1, pINode_base, INode, uint32, uint32 >(
			mRawPtr, &INode_v1::clone, static_cast< uint32 >( ignoreEmptyNodes ), static_cast< uint32 >( ignoreNodesWithOnlyQualifiers ) );
	}

	pINode_base APICALL INodeProxy::getParent( pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->getParent( error );
	}

	void APICALL INodeProxy::setName( const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->setName( name, nameLength, error );
	}

	AdobeXMPCommon::pcIUTF8String_base APICALL INodeProxy::getName( pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->getName( error );
	}

	void APICALL INodeProxy::setNameSpace( const char * nameSpace, sizet nameSpaceLength, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->setNameSpace( nameSpace, nameSpaceLength, error );
	}

	pcIUTF8String_base APICALL INodeProxy::getNameSpace( pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->getNameSpace( error );
	}

	pIPath_base APICALL INodeProxy::getPath( pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->getPath( error );
	}

	pINodeIterator_base APICALL INodeProxy::qualifiersIterator( pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->qualifiersIterator( error );
	}

	pINode_base APICALL INodeProxy::getQualifier( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->getQualifier( nameSpace, nameSpaceLength, name, nameLength, error );
	}

	void APICALL INodeProxy::insertQualifier( pINode_base base, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->insertQualifier( base, error );
	}

	pINode_base APICALL INodeProxy::replaceQualifier( pINode_base node, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->replaceQualifier( node, error );
	}

	pINode_base APICALL INodeProxy::removeQualifier( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->removeQualifier( nameSpace, nameSpaceLength, name, nameLength, error );
	}

	uint32 APICALL INodeProxy::getNodeType( pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->getNodeType( error );
	}

	uint32 APICALL INodeProxy::isArrayItem( pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->isArrayItem( error );
	}

	uint32 APICALL INodeProxy::isQualifierNode( pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->isQualifierNode( error );
	}

	sizet APICALL INodeProxy::getIndex( pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->getIndex( error );
	}

	uint32 APICALL INodeProxy::hasQualifiers( pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->hasQualifiers( error );
	}

	uint32 APICALL INodeProxy::hasContent( pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->hasContent( error );
	}

	uint32 APICALL INodeProxy::isEmpty( pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->isEmpty( error );
	}

	uint32 APICALL INodeProxy::hasChanged( pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->hasChanged( error );
	}

	void APICALL INodeProxy::clear( uint32 contents, uint32 qualifiers, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->clear( contents, qualifiers, error );
	}

	pINode_base APICALL INodeProxy::clone( uint32 igoreEmptyNodes, uint32 ignoreNodesWithOnlyQualifiers, pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->clone( igoreEmptyNodes, ignoreNodesWithOnlyQualifiers, error );
	}

	void APICALL INodeProxy::EnableThreadSafety() const __NOTHROW__ {
		return mRawPtr->EnableThreadSafety(  );
	}

	void APICALL INodeProxy::DisableThreadSafety() const __NOTHROW__ {
		return mRawPtr->DisableThreadSafety(  );
	}

	bool APICALL INodeProxy::IsThreadSafe() const {
		return mRawPtr->isThreadSafe() != 0;
	}

	uint32 APICALL INodeProxy::isThreadSafe() const __NOTHROW__ {
		assert( false );
		return mRawPtr->isThreadSafe();
	}

	spISimpleNode APICALL INodeProxy::ConvertToSimpleNode() {
		return CallSafeFunctionReturningPointer< INode_v1, pISimpleNode_base, ISimpleNode >(
			mRawPtr, &INode_v1::convertToSimpleNode );
	}

	spIStructureNode APICALL INodeProxy::ConvertToStructureNode() {
		return CallSafeFunctionReturningPointer< INode_v1, pIStructureNode_base, IStructureNode >(
			mRawPtr, &INode_v1::convertToStructureNode );
	}

	spIArrayNode APICALL INodeProxy::ConvertToArrayNode() {
		return CallSafeFunctionReturningPointer< INode_v1, pIArrayNode_base, IArrayNode >(
			mRawPtr, &INode_v1::convertToArrayNode );
	}

	spIMetadata APICALL INodeProxy::ConvertToMetadata() {
		return CallSafeFunctionReturningPointer< INode_v1, pIMetadata_base, IMetadata >(
			mRawPtr, &INode_v1::convertToMetadata );
	}

	pISimpleNode_base APICALL INodeProxy::convertToSimpleNode( pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->convertToSimpleNode( error );
	}

	pIStructureNode_base APICALL INodeProxy::convertToStructureNode( pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->convertToStructureNode( error );	}

	pIArrayNode_base APICALL INodeProxy::convertToArrayNode( pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->convertToArrayNode( error );
	}

	pIMetadata_base APICALL INodeProxy::convertToMetadata( pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->convertToMetadata( error );
	}

	uint32 APICALL INodeProxy::getParentNodeType( pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->getParentNodeType( error );
	}

	uint32 APICALL INodeProxy::getQualifierNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mRawPtr->getQualifierNodeType( nameSpace, nameSpaceLength, name, nameLength, error );
	}

	INode_v1::eNodeType APICALL INodeProxy::GetParentNodeType() const {
		return CallConstSafeFunction< INode_v1, eNodeType, uint32 >(
			mRawPtr, &INode_v1::getParentNodeType );
	}

	INode_v1::eNodeType APICALL INodeProxy::GetQualifierNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) const {
		return CallConstSafeFunction< INode_v1, eNodeType, uint32, const char *, sizet, const char *, sizet >(
			mRawPtr, &INode_v1::getQualifierNodeType, nameSpace, nameSpaceLength, name, nameLength );
	}

	AdobeXMPCommon_Int::pIThreadSafe_I APICALL INodeProxy::GetIThreadSafe_I() __NOTHROW__ {
		return mRawPtr->GetIThreadSafe_I(  );
	}

	pvoid APICALL INodeProxy::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
			mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
	}

	spINode INode_v1::MakeShared( pINode_base ptr ) {
		if ( !ptr ) return spINode();
		pINode p = INode::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< INode >() : ptr;
		return shared_ptr< INode >( new INodeProxy( p ) );
	}

}

#endif  // !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB
