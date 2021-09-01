// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#define FRIEND_CLASS_DECLARATION() friend class IUTF8StringProxy;

#include "XMPCommon/Interfaces/IUTF8String.h"

#if !BUILDING_XMPCOMMON_LIB && !SOURCE_COMPILING_XMP_ALL

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IObjectFactory.h"

#include <assert.h>

namespace AdobeXMPCommon {

	class IUTF8StringProxy
		: public virtual IUTF8String
		, public enable_shared_from_this< IUTF8StringProxy >
	{
	private:
		pIUTF8String mRawPtr;

	public:
		IUTF8StringProxy( pIUTF8String ptr )
			: mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~IUTF8StringProxy() __NOTHROW__ { mRawPtr->Release(); }

		pIUTF8String APICALL GetActualIUTF8String() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		spIUTF8String APICALL append( const char * buf, sizet count ) {
			CallSafeFunction< IUTF8String, pIUTF8String_base, pIUTF8String_base, const char *, sizet >(
				mRawPtr, &IUTF8String::append, buf, count )->Release();
			return shared_from_this();
		}

		spIUTF8String APICALL append( const spcIUTF8String & src, sizet srcPos, sizet count ) {
			CallSafeFunction< IUTF8String, pIUTF8String_base, pIUTF8String_base, pcIUTF8String_base, sizet, sizet >(
				mRawPtr, &IUTF8String::append, src ? src->GetActualIUTF8String() : NULL, srcPos, count )->Release();
			return shared_from_this();
		}

		pIUTF8String_base APICALL append( const char * buffer, sizet count, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			mRawPtr->append( buffer, count, error );
			return this;
		}

		pIUTF8String_base APICALL append( pcIUTF8String_base str, sizet srcPos, sizet count, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			mRawPtr->append( str, srcPos, count, error );
			return this;
		}

		spIUTF8String APICALL assign( const char * buf, sizet count ) {
			CallSafeFunction< IUTF8String, pIUTF8String_base, pIUTF8String_base, const char *, sizet >(
				mRawPtr, &IUTF8String::assign, buf, count )->Release();
			return shared_from_this();
		}

		spIUTF8String APICALL assign( const spcIUTF8String & src, sizet srcPos, sizet count ) {
			CallSafeFunction< IUTF8String, pIUTF8String_base, pIUTF8String_base, pcIUTF8String_base, sizet, sizet >(
				mRawPtr, &IUTF8String::assign, src ? src->GetActualIUTF8String() : NULL, srcPos, count )->Release();
			return shared_from_this();
		}

		pIUTF8String_base APICALL assign( const char * buffer, sizet count, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			mRawPtr->assign( buffer, count, error );
			return this;
		}

		pIUTF8String_base APICALL assign( pcIUTF8String_base str, sizet srcPos, sizet count, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			mRawPtr->assign( str, srcPos, count, error );
			return this;
		}

		spIUTF8String APICALL insert( sizet pos, const char * buf, sizet count ) {
			CallSafeFunction< IUTF8String, pIUTF8String_base, pIUTF8String_base, sizet, const char *, sizet >(
				mRawPtr, &IUTF8String::insert, pos, buf, count )->Release();
			return shared_from_this();
		}

		spIUTF8String APICALL insert( sizet pos, const spcIUTF8String & src, sizet srcPos, sizet count ) {
			CallSafeFunction< IUTF8String, pIUTF8String_base, pIUTF8String_base, sizet, pcIUTF8String_base, sizet, sizet >(
				mRawPtr, &IUTF8String::insert, pos, src ? src->GetActualIUTF8String() : NULL, srcPos, count )->Release();
			return shared_from_this();
		}

		pIUTF8String_base APICALL insert( sizet pos, const char * buf, sizet count, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			mRawPtr->insert( pos, buf, count, error );
			return this;
		}

		pIUTF8String_base APICALL insert( sizet pos, pcIUTF8String_base src, sizet srcPos, sizet count, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			mRawPtr->insert( pos, src, srcPos, count, error );
			return this;
		}

		spIUTF8String APICALL erase( sizet pos, sizet count ) {
			CallSafeFunction< IUTF8String, pIUTF8String_base, pIUTF8String_base, sizet, sizet >(
				mRawPtr, &IUTF8String::erase, pos, count )->Release();
			return shared_from_this();
		}

		pIUTF8String_base APICALL erase( sizet pos, sizet count, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			mRawPtr->erase( pos, count, error );
			return this;
		}

		void APICALL resize( sizet n ) {
			CallSafeFunctionReturningVoid< IUTF8String, sizet >(
				mRawPtr, &IUTF8String::resize, n );
		}

		virtual void APICALL resize( sizet n, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			mRawPtr->resize( n, error );
		}

		spIUTF8String APICALL replace( sizet pos, sizet count, const char * buf, sizet srcCount ) {
			CallSafeFunction< IUTF8String, pIUTF8String_base, pIUTF8String_base, sizet, sizet, const char *, sizet >(
				mRawPtr, &IUTF8String::replace, pos, count, buf, srcCount )->Release();
			return shared_from_this();
		}

		spIUTF8String APICALL replace( sizet pos, sizet count, const spcIUTF8String & src, sizet srcPos, sizet srcCount ) {
			CallSafeFunction< IUTF8String, pIUTF8String_base, pIUTF8String_base, sizet, sizet, pcIUTF8String_base, sizet, sizet >(
				mRawPtr, &IUTF8String::replace, pos, count, src ? src->GetActualIUTF8String() : NULL, srcPos, srcCount )->Release();
			return shared_from_this();
		}

		pIUTF8String_base APICALL replace( sizet pos, sizet count, const char * buf, sizet srcCount, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			mRawPtr->replace( pos, count, buf, srcCount, error );
			return this;
		}

		pIUTF8String_base APICALL replace( sizet pos, sizet count, pcIUTF8String_base src, sizet srcPos, sizet srcCount, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			mRawPtr->replace( pos, count, src, srcPos, srcCount, error );
			return this;
		}

		sizet APICALL copy( char * buf, sizet len, sizet pos ) const {
			return CallConstSafeFunction< IUTF8String, sizet, sizet, char *, sizet, sizet >(
				mRawPtr, &IUTF8String::copy, buf, len, pos );
		}

		sizet APICALL copy( char * buf, sizet len, sizet pos, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->copy( buf, len, pos, error );
		}

		sizet APICALL find( const char * buf, sizet pos, sizet count ) const {
			return CallConstSafeFunction< IUTF8String, sizet, sizet, const char *, sizet, sizet >(
				mRawPtr, &IUTF8String::find, buf, pos, count );
		}

		sizet APICALL find( const spcIUTF8String & src, sizet pos, sizet count ) const {
			return CallConstSafeFunction< IUTF8String, sizet, sizet, pcIUTF8String_base, sizet, sizet >(
				mRawPtr, &IUTF8String::find, src ? src->GetActualIUTF8String() : NULL, pos, count );
		}

		sizet APICALL find( const char * buf, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->find( buf, pos, count, error );
		}

		sizet APICALL find( pcIUTF8String_base src, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->find( src, pos, count, error );
		}

		sizet APICALL rfind( const char * buf, sizet pos, sizet count ) const {
			return CallConstSafeFunction< IUTF8String, sizet, sizet, const char *, sizet, sizet >(
				mRawPtr, &IUTF8String::rfind, buf, pos, count );
		}

		sizet APICALL rfind( const spcIUTF8String & src, sizet pos, sizet count ) const {
			return CallConstSafeFunction< IUTF8String, sizet, sizet, pcIUTF8String_base, sizet, sizet >(
				mRawPtr, &IUTF8String::rfind, src ? src->GetActualIUTF8String() : NULL, pos, count );
		}

		sizet APICALL rfind( const char * buf, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->rfind( buf, pos, count, error );
		}

		sizet APICALL rfind( pcIUTF8String_base src, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->rfind( src, pos, count, error );
		}

		int32 APICALL compare( sizet pos, sizet len, const char * buf, sizet count ) const {
			return CallConstSafeFunction< IUTF8String, int32, int32, sizet, sizet, const char *, sizet >(
				mRawPtr, &IUTF8String::compare, pos, len, buf, count );
		}

		int32 APICALL compare( sizet pos, sizet len, const spcIUTF8String & str, sizet strPos, sizet strLen ) const {
			return CallConstSafeFunction< IUTF8String, int32, int32, sizet, sizet, pcIUTF8String_base, sizet, sizet >(
				mRawPtr, &IUTF8String::compare, pos, len, str ? str->GetActualIUTF8String() : NULL, strPos, strLen );
		}

		int32 APICALL compare( sizet pos, sizet len, const char * buf, sizet count, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->compare( pos, len, buf, count, error );
		}

		int32 APICALL compare( sizet pos, sizet len, pcIUTF8String_base str, sizet strPos, sizet strLen, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->compare( pos, len, str, strPos, strLen, error );
		}

		spIUTF8String APICALL substr( sizet pos, sizet count ) const {
			return CallConstSafeFunctionReturningPointer< IUTF8String, pIUTF8String_base, IUTF8String, sizet, sizet >(
				mRawPtr, &IUTF8String::substr, pos, count );
		}

		pIUTF8String_base APICALL substr( sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->substr( pos, count, error );
		}

		bool APICALL empty() const {
			return CallConstSafeFunction< IUTF8String, bool, uint32 >(
				mRawPtr, &IUTF8String::empty );
		}

		uint32 APICALL empty( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->empty();
		}

		const char * APICALL c_str() const __NOTHROW__ {
			return mRawPtr->c_str();
		}

		void APICALL clear() __NOTHROW__ {
			mRawPtr->clear();
		}

		sizet APICALL size() const __NOTHROW__ {
			return mRawPtr->size();
		}

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

	};

	spIUTF8String IUTF8String_v1::MakeShared( pIUTF8String_base ptr ) {
		if ( !ptr ) return spIUTF8String();
		pIUTF8String p = IUTF8String::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IUTF8String >() : ptr;
		return shared_ptr< IUTF8StringProxy >( new IUTF8StringProxy( p ) );
	}

	spIUTF8String IUTF8String_v1::CreateUTF8String( pIObjectFactory objFactory ) {
		return CallSafeFunctionReturningPointer< IObjectFactory, pIUTF8String_base, IUTF8String, const char *, sizet >(
			objFactory, &IObjectFactory::CreateUTF8String, NULL, ( sizet ) 0 );
	}

	spIUTF8String IUTF8String_v1::CreateUTF8String( pIObjectFactory objFactory, const char * buf, sizet count ) {
		return CallSafeFunctionReturningPointer< IObjectFactory, pIUTF8String_base, IUTF8String, const char *, sizet >(
			objFactory, &IObjectFactory::CreateUTF8String, buf, count );
	}

}

#endif  // BUILDING_XMPCOMMON_LIB  && !SOURCE_COMPILING_XMP_ALL
