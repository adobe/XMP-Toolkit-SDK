// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
	class IArrayNodeProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::IArrayNodeProxy;

#include "XMPCore/Interfaces/IArrayNode.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCore/Interfaces/ICoreObjectFactory.h"

#include <assert.h>

namespace AdobeXMPCore {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	class IArrayNodeProxy
		: public virtual IArrayNode
		, public virtual ICompositeNodeProxy
	{
	private:
		pIArrayNode mRawPtr;

	public:
		IArrayNodeProxy( pIArrayNode ptr )
			: mRawPtr( ptr )
			, ICompositeNodeProxy( ptr )
			, INodeProxy( ptr ) {}

		~IArrayNodeProxy() __NOTHROW__ {}

		pIArrayNode APICALL GetActualIArrayNode() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCore_Int::pIArrayNode_I APICALL GetIArrayNode_I() __NOTHROW__ {
			return mRawPtr->GetIArrayNode_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		virtual eArrayForm APICALL GetArrayForm() const {
			return CallConstSafeFunction< IArrayNode_v1, eArrayForm, uint32 >(
				mRawPtr, &IArrayNode_v1::getArrayForm );
		}

		virtual eNodeType APICALL GetChildNodeType() const {
			return CallConstSafeFunction< IArrayNode_v1, eNodeType, uint32 >(
				mRawPtr, &IArrayNode_v1::getChildNodeType );
		}

		virtual spINode APICALL GetNodeAtIndex( sizet index ) {
			return CallSafeFunctionReturningPointer< IArrayNode_v1, pINode_base, INode, sizet >(
				mRawPtr, &IArrayNode_v1::getNodeAtIndex, index );
		}

		virtual void APICALL InsertNodeAtIndex( const spINode & node, sizet index ) {
			return CallSafeFunctionReturningVoid< IArrayNode_v1, pINode_base, sizet >(
				mRawPtr, &IArrayNode_v1::insertNodeAtIndex, node ? node->GetActualINode() : NULL, index );
		}

		virtual spINode APICALL RemoveNodeAtIndex( sizet index ) {
			return CallSafeFunctionReturningPointer< IArrayNode_v1, pINode_base, INode, sizet >(
				mRawPtr, &IArrayNode_v1::removeNodeAtIndex, index );
		}

		virtual spINode APICALL ReplaceNodeAtIndex( const spINode & node, sizet index ) {
			return CallSafeFunctionReturningPointer< IArrayNode_v1, pINode_base, INode, pINode_base, sizet >(
				mRawPtr, &IArrayNode_v1::replaceNodeAtIndex, node ? node->GetActualINode() :NULL, index );
		}

		virtual uint32 APICALL getArrayForm( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getArrayForm( error );
		}

		virtual pINode_base APICALL getNodeAtIndex( sizet index, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getNodeAtIndex( index, error );
		}

		virtual void APICALL insertNodeAtIndex( pINode_base node, sizet index, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->insertNodeAtIndex( node, index, error );
		}

		virtual pINode_base APICALL removeNodeAtIndex( sizet index, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->removeNodeAtIndex( index, error );
		}

		virtual pINode_base APICALL replaceNodeAtIndex( pINode_base node, sizet index, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->replaceNodeAtIndex( node, index, error );
		}

		virtual uint32 APICALL getChildNodeType( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getChildNodeType( error );
		}

	};

#if XMP_WinBuild
	#pragma warning( pop )
#endif

	spIArrayNode IArrayNode_v1::MakeShared( pIArrayNode_base ptr ) {
		if ( !ptr ) return spIArrayNode();
		pIArrayNode p = IArrayNode::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IArrayNode >() : ptr;
        return shared_ptr< IArrayNode >( new IArrayNodeProxy( p ) );
	}

	spIArrayNode IArrayNode_v1::CreateUnorderedArrayNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pIArrayNode_base, IArrayNode, uint32, const char *, sizet, const char *, sizet >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::CreateArrayNode, static_cast< uint32 >( IArrayNode::kAFUnordered ),
			nameSpace, nameSpaceLength, name, nameLength );
	}

	spIArrayNode IArrayNode_v1::CreateOrderedArrayNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pIArrayNode_base, IArrayNode, uint32, const char *, sizet, const char *, sizet >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::CreateArrayNode, static_cast< uint32 >( IArrayNode::kAFOrdered ),
			nameSpace, nameSpaceLength, name, nameLength );
	}

	spIArrayNode IArrayNode_v1::CreateAlternativeArrayNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pIArrayNode_base, IArrayNode, uint32, const char *, sizet, const char *, sizet >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::CreateArrayNode, static_cast< uint32 >( IArrayNode::kAFAlternative ),
			nameSpace, nameSpaceLength, name, nameLength );
	}

}

#endif  // BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB
