// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
	class INodeIteratorProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::INodeIteratorProxy;

#include "XMPCore/Interfaces/INodeIterator.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include <assert.h>

namespace AdobeXMPCore {

	class INodeIteratorProxy
		: public virtual INodeIterator
	{
	private:
		pINodeIterator mRawPtr;

	public:
		INodeIteratorProxy( pINodeIterator ptr )
			: mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~INodeIteratorProxy() __NOTHROW__ { mRawPtr->Release(); }

		pINodeIterator APICALL GetActualINodeIterator() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCore_Int::pINodeIterator_I APICALL GetINodeIterator_I() __NOTHROW__ {
			return mRawPtr->GetINodeIterator_I();
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		virtual spINode APICALL GetNode() {
			return CallSafeFunctionReturningPointer< INodeIterator_v1, pINode_base, INode >(
				mRawPtr, &INodeIterator_v1::getNode );
		}

		virtual INode_v1::eNodeType APICALL GetNodeType() const {
			return CallConstSafeFunction< INodeIterator_v1, INode_v1::eNodeType, uint32 >(
				mRawPtr, &INodeIterator_v1::getNodeType );
		}

		virtual spINodeIterator APICALL Next() {
			return CallSafeFunctionReturningPointer< INodeIterator_v1, pINodeIterator, INodeIterator >(
				mRawPtr, &INodeIterator_v1::next );
		}

		virtual uint32 APICALL getNodeType( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getNodeType( error );
		}

		virtual pINode_base APICALL getNode( pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getNode( error );
		}

		virtual pINodeIterator_base APICALL next( pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->next( error );
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
			mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

	};

	spINodeIterator INodeIterator_v1::MakeShared( pINodeIterator_base ptr ) {
		if ( !ptr ) return spINodeIterator();
		//return shared_ptr< INodeIterator >( new INodeIteratorProxy( ptr ) );
		pINodeIterator p = INodeIterator::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< INodeIterator >() : ptr;
		return shared_ptr< INodeIterator >( new INodeIteratorProxy( p ) );
	}

}

#endif  // BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB
