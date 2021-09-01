// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
	class IPathSegmentProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::IPathSegmentProxy;

#include "XMPCore/Interfaces/IPathSegment.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCommon/Interfaces/IError.h"
#include "XMPCore/Interfaces/ICoreObjectFactory.h"

#include <assert.h>

namespace AdobeXMPCore {

	class IPathSegmentProxy
		: public virtual IPathSegment
	{
	private:
		pIPathSegment mRawPtr;

	public:
		IPathSegmentProxy( pIPathSegment ptr )
			: mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~IPathSegmentProxy() __NOTHROW__ { mRawPtr->Release(); }

		pIPathSegment APICALL GetActualIPathSegment() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCore_Int::pIPathSegment_I APICALL GetIPathSegment_I() __NOTHROW__ {
			return mRawPtr->GetIPathSegment_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		virtual spcIUTF8String APICALL GetNameSpace() const {
			return CallConstSafeFunctionReturningPointer< IPathSegment_v1, pcIUTF8String_base, const IUTF8String >(
				mRawPtr, &IPathSegment_v1::getNameSpace );
		}

		virtual pcIUTF8String_base APICALL getNameSpace( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getNameSpace( error );
		}

		virtual spcIUTF8String APICALL GetName() const {
			return CallConstSafeFunctionReturningPointer< IPathSegment_v1, pcIUTF8String_base, const IUTF8String >(
				mRawPtr, &IPathSegment_v1::getName );
		}

		virtual pcIUTF8String_base APICALL getName( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getName( error );
		}

		virtual ePathSegmentType APICALL GetType() const {
			return CallConstSafeFunction< IPathSegment_v1, ePathSegmentType, uint32 >(
				mRawPtr, &IPathSegment_v1::getType );
		}

		virtual uint32 APICALL getType( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getType( error );
		}

		virtual sizet APICALL GetIndex() const __NOTHROW__ {
			return mRawPtr->GetIndex();
		}

		virtual spcIUTF8String APICALL GetValue() const {
			return CallConstSafeFunctionReturningPointer< IPathSegment_v1, pcIUTF8String_base, const IUTF8String >(
				mRawPtr, &IPathSegment_v1::getValue );
		}

		virtual pcIUTF8String_base APICALL getValue( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getValue( error );
		}

	};

	spIPathSegment IPathSegment_v1::MakeShared( pIPathSegment_base ptr ) {
		if ( !ptr ) return spIPathSegment();
		pIPathSegment p = IPathSegment::GetInterfaceVersion() > 1 ?
			ptr->GetInterfacePointer< IPathSegment >() : ptr;
		return shared_ptr< IPathSegment >( new IPathSegmentProxy( p ) );
	}

	spcIPathSegment IPathSegment_v1::CreatePropertyPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pcIPathSegment_base, const IPathSegment, const char *, sizet, const char *, sizet >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::CreatePropertyPathSegment, nameSpace, nameSpaceLength, name, nameLength );
	}

	spcIPathSegment IPathSegment_v1::CreateArrayIndexPathSegment( const char * nameSpace, sizet nameSpaceLength, sizet index ) {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pcIPathSegment_base, const IPathSegment, const char *, sizet, sizet >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::CreateArrayIndexPathSegment, nameSpace, nameSpaceLength, index );
	}

	spcIPathSegment IPathSegment_v1::CreateQualifierPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pcIPathSegment_base, const IPathSegment, const char *, sizet, const char *, sizet >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::CreateQualifierPathSegment, nameSpace, nameSpaceLength, name, nameLength );
	}

	spcIPathSegment IPathSegment_v1::CreateQualifierSelectorPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength,
		const char * value, sizet valueLength )
	{
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pcIPathSegment_base, const IPathSegment, const char *, sizet, const char *, sizet, const char *, sizet >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::CreateQualifierSelectorPathSegment, nameSpace, nameSpaceLength, name, nameLength, value, valueLength );
	}

}

#endif  // BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

