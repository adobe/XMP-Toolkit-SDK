// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
	class INameSpacePrefixMapProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::INameSpacePrefixMapProxy;

#include "XMPCore/Interfaces/INameSpacePrefixMap.h"
#include "XMPCore/Interfaces/ICoreObjectFactory.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IUTF8String.h"

#include <assert.h>

namespace AdobeXMPCore {


	bool INameSpacePrefixMap::IsEmpty() const __NOTHROW__{
		return this->Size() == 0;
	}

	class INameSpacePrefixMapProxy
		: public virtual INameSpacePrefixMap
	{
	private:
		pINameSpacePrefixMap mRawPtr;

	public:
		INameSpacePrefixMapProxy( pINameSpacePrefixMap ptr )
			: mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~INameSpacePrefixMapProxy() __NOTHROW__ { mRawPtr->Release(); }

		pINameSpacePrefixMap APICALL GetActualINameSpacePrefixMap()  __NOTHROW__ override { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ override { assert( false ); }

		void APICALL Release() const __NOTHROW__ override { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ override {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCore_Int::pINameSpacePrefixMap_I APICALL GetINameSpacePrefixMap_I() __NOTHROW__ override {
			return mRawPtr->GetINameSpacePrefixMap_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) override {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ override {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		virtual bool APICALL Insert( const char * prefix, sizet prefixLength, const char * nameSpace, sizet nameSpaceLength ) override {
			return CallSafeFunction< INameSpacePrefixMap_v1, bool, uint32, const char *, sizet, const char *, sizet >(
				mRawPtr, &INameSpacePrefixMap_v1::insert, prefix, prefixLength, nameSpace, nameSpaceLength );
		}

		virtual uint32 APICALL insert( const char * prefix, sizet prefixLength, const char * nameSpace, sizet nameSpaceLength, pcIError_base & error ) __NOTHROW__ override {
			assert( false );
			return mRawPtr->insert( prefix, prefixLength, nameSpace, nameSpaceLength, error );
		}

		virtual bool APICALL RemovePrefix( const char * prefix, sizet prefixLength ) override {
			return CallSafeFunction< INameSpacePrefixMap_v1, bool, uint32, uint32, const char *, sizet >(
				mRawPtr, &INameSpacePrefixMap_v1::remove, kPrefixIsParameter, prefix, prefixLength );
		}

		virtual bool APICALL RemoveNameSpace( const char * nameSpace, sizet nameSpaceLength ) override {
			return CallSafeFunction< INameSpacePrefixMap_v1, bool, uint32, uint32, const char *, sizet >(
				mRawPtr, &INameSpacePrefixMap_v1::remove, kNameSpaceIsParameter, nameSpace, nameSpaceLength );
		}

		virtual bool APICALL IsPrefixPresent( const char * prefix, sizet prefixLength ) const override {
			return CallConstSafeFunction< INameSpacePrefixMap_v1, bool, uint32, uint32, const char *, sizet >(
				mRawPtr, &INameSpacePrefixMap_v1::isPresent, kPrefixIsParameter, prefix, prefixLength );
		}

		virtual bool APICALL IsNameSpacePresent( const char * nameSpace, sizet nameSpaceLength ) const override {
			return CallConstSafeFunction< INameSpacePrefixMap_v1, bool, uint32, uint32, const char *, sizet >(
				mRawPtr, &INameSpacePrefixMap_v1::isPresent, kNameSpaceIsParameter, nameSpace, nameSpaceLength );
		}

		virtual spcIUTF8String APICALL GetNameSpace( const char * prefix, sizet prefixLength ) const override {
			return CallConstSafeFunctionReturningPointer< INameSpacePrefixMap_v1, pcIUTF8String_base, const IUTF8String, uint32, const char *, sizet >(
				mRawPtr, &INameSpacePrefixMap_v1::get, kPrefixIsParameter, prefix, prefixLength );
		}

		virtual spcIUTF8String APICALL GetPrefix( const char * nameSpace, sizet nameSpaceLength ) const override {
			return CallConstSafeFunctionReturningPointer< INameSpacePrefixMap_v1, pcIUTF8String_base, const IUTF8String, uint32, const char *, sizet >(
				mRawPtr, &INameSpacePrefixMap_v1::get, kNameSpaceIsParameter, nameSpace, nameSpaceLength );
		}

		virtual sizet APICALL Size() const __NOTHROW__ override {
			return mRawPtr->Size();
		}

		virtual void APICALL Clear() __NOTHROW__ override {
			return mRawPtr->Clear();
		}

		virtual spINameSpacePrefixMap APICALL Clone() const override {
			return CallConstSafeFunctionReturningPointer< INameSpacePrefixMap_v1, pINameSpacePrefixMap_base, INameSpacePrefixMap >(
				mRawPtr, &INameSpacePrefixMap_v1::clone );
		}

		virtual pINameSpacePrefixMap_base APICALL clone( pcIError_base & error ) const __NOTHROW__ override {
			assert( false );
			return mRawPtr->clone( error );
		}

		virtual uint32 APICALL remove( uint32 keyType, const char * key, sizet keyLength, pcIError_base & error ) __NOTHROW__ override {
			assert( false );
			return mRawPtr->remove( keyType, key, keyLength, error );
		}

		virtual uint32 APICALL isPresent( uint32 keyType, const char * key, sizet keyLength, pcIError_base & error ) const __NOTHROW__ override {
			assert( false );
			return mRawPtr->isPresent( keyType, key, keyLength, error );
		}

		virtual pcIUTF8String_base APICALL get( uint32 keyType, const char * key, sizet keyLength, pcIError_base & error ) const __NOTHROW__ override {
			assert( false );
			return mRawPtr->get( keyType, key, keyLength, error );
		}

		virtual void APICALL EnableThreadSafety() const __NOTHROW__  override {
			return mRawPtr->EnableThreadSafety();
		}

		virtual void APICALL DisableThreadSafety() const __NOTHROW__  override {
			return mRawPtr->DisableThreadSafety();
		}

		virtual bool APICALL IsThreadSafe() const override {
			return mRawPtr->isThreadSafe() != 0;
		}

		virtual uint32 APICALL isThreadSafe( ) const __NOTHROW__ override {
			assert( false );
			return mRawPtr->isThreadSafe();
		}

		virtual AdobeXMPCommon_Int::pIThreadSafe_I APICALL GetIThreadSafe_I() __NOTHROW__ override {
			return mRawPtr->GetIThreadSafe_I();
		}
		
	};

	spINameSpacePrefixMap INameSpacePrefixMap_v1::MakeShared( pINameSpacePrefixMap_base ptr ) {
		if ( !ptr ) return spINameSpacePrefixMap();
		pINameSpacePrefixMap p = INameSpacePrefixMap::GetInterfaceVersion() > 1 ?
			ptr->GetInterfacePointer< INameSpacePrefixMap >() : ptr;
		return shared_ptr< INameSpacePrefixMap >( new INameSpacePrefixMapProxy( p ) );
	}

	spINameSpacePrefixMap INameSpacePrefixMap_v1::CreateNameSpacePrefixMap() {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pINameSpacePrefixMap_base, INameSpacePrefixMap >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::CreateNameSpacePrefixMap );
	}

	spcINameSpacePrefixMap INameSpacePrefixMap_v1::GetDefaultNameSpacePrefixMap() {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pcINameSpacePrefixMap_base, const INameSpacePrefixMap >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::GetDefaultNameSpacePrefixMap );
	}

}

#endif  // BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB
