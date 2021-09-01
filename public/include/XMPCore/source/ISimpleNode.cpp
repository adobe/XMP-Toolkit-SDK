// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
	class ISimpleNodeProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::ISimpleNodeProxy;

#include "XMPCore/Interfaces/ISimpleNode.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCore/Interfaces/ICoreObjectFactory.h"

#include <assert.h>

namespace AdobeXMPCore {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	class ISimpleNodeProxy
		: public virtual ISimpleNode
		, public virtual INodeProxy
	{
	private:
		pISimpleNode mRawPtr;

	public:
		ISimpleNodeProxy( pISimpleNode ptr )
			: mRawPtr( ptr )
			, INodeProxy( ptr ) {}

		~ISimpleNodeProxy() __NOTHROW__ {}

		pISimpleNode APICALL GetActualISimpleNode() __NOTHROW__ { return mRawPtr; }

		AdobeXMPCore_Int::pISimpleNode_I APICALL GetISimpleNode_I() __NOTHROW__ {
			return mRawPtr->GetISimpleNode_I();
		}

		virtual spcIUTF8String APICALL GetValue() const {
			return CallConstSafeFunctionReturningPointer< ISimpleNode_v1, pcIUTF8String_base, const IUTF8String >(
				mRawPtr, &ISimpleNode_v1::getValue );
		}

		virtual void APICALL SetValue( const char * value, sizet valueLength ) {
			return CallSafeFunctionReturningVoid< ISimpleNode_v1, const char *, sizet >(
				mRawPtr, &ISimpleNode_v1::setValue, value, valueLength );
		}

		virtual bool APICALL IsURIType() const {
			return CallConstSafeFunction< ISimpleNode_v1, bool, uint32 >(
				mRawPtr, &ISimpleNode_v1::isURIType );
		}

		virtual void APICALL SetURIType( bool isURI ) {
			return CallSafeFunctionReturningVoid< ISimpleNode_v1, uint32 >(
				mRawPtr, &ISimpleNode_v1::setURIType, isURI ? 1 : 0 );
		}

		virtual pcIUTF8String_base APICALL getValue( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getValue( error );
		}

		virtual void APICALL setValue( const char * value, sizet valueLength, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->setValue( value, valueLength, error );
		}

		virtual uint32 APICALL isURIType( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->isURIType( error );
		}

		virtual void APICALL setURIType( uint32 isURI, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->setURIType( isURI, error );
		}

	};

	spISimpleNode ISimpleNode_v1::CreateSimpleNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, const char * value, sizet valueLength ) {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pISimpleNode_base, ISimpleNode, const char *, sizet, const char *, sizet, const char *, sizet >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::CreateSimpleNode, nameSpace, nameSpaceLength, name, nameLength, value, valueLength );
	}

	spISimpleNode ISimpleNode_v1::MakeShared( pISimpleNode_base ptr ) {
		if ( !ptr ) return spISimpleNode();
		pISimpleNode p = ISimpleNode::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< ISimpleNode >() : ptr;
		return shared_ptr< ISimpleNode >( new ISimpleNodeProxy( p ) );
	}

#if XMP_WinBuild
	#pragma warning( pop )
#endif
}

#endif  // !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB
