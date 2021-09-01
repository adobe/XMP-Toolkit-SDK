// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	pIUTF8String_base APICALL IUTF8String_I::assign( const char * buffer, sizet count, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IUTF8String_v1, pIUTF8String_base, IUTF8String, const char *, sizet >(
			error, this, &IUTF8String_v1::assign, __FILE__, __LINE__, buffer, count );
	}

	pIUTF8String_base APICALL IUTF8String_I::assign( pcIUTF8String_base str, sizet srcPos, sizet count, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IUTF8String_v1, pIUTF8String_base, IUTF8String, const spcIUTF8String &, sizet, sizet >(
			error, this, &IUTF8String_v1::assign, __FILE__, __LINE__, IUTF8String::MakeShared( str ), srcPos, count );
	}

	pIUTF8String_base APICALL IUTF8String_I::append( const char * buffer, sizet count, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IUTF8String_v1, pIUTF8String_base, IUTF8String, const char *, sizet >(
			error, this, &IUTF8String_v1::append, __FILE__, __LINE__, buffer, count );
	}

	pIUTF8String_base APICALL IUTF8String_I::append( pcIUTF8String_base str, sizet srcPos, sizet count, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IUTF8String_v1, pIUTF8String_base, IUTF8String, const spcIUTF8String &, sizet, sizet >(
			error, this, &IUTF8String_v1::append, __FILE__, __LINE__, IUTF8String::MakeShared( str ), srcPos, count );
	}

	pIUTF8String_base APICALL IUTF8String_I::insert( sizet pos, const char * buf, sizet count, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IUTF8String_v1, pIUTF8String_base, IUTF8String, sizet, const char *, sizet >(
			error, this, &IUTF8String_v1::insert, __FILE__, __LINE__, pos, buf, count );
	}

	pIUTF8String_base APICALL IUTF8String_I::insert( sizet pos, pcIUTF8String_base src, sizet srcPos, sizet count, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IUTF8String_v1, pIUTF8String_base, IUTF8String, sizet, const spcIUTF8String &, sizet, sizet >(
			error, this, &IUTF8String_v1::insert, __FILE__, __LINE__, pos, IUTF8String::MakeShared( src ), srcPos, count );
	}

	pIUTF8String_base APICALL IUTF8String_I::erase( sizet pos, sizet count, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IUTF8String_v1, pIUTF8String_base, IUTF8String, sizet, sizet >(
			error, this, &IUTF8String_v1::erase, __FILE__, __LINE__, pos, count );
	}

	void APICALL IUTF8String_I::resize( sizet n, pcIError_base & error ) __NOTHROW__ {
		typedef void( APICALL IUTF8String_v1::*Func )( sizet );
		Func fnPtr = &IUTF8String_v1::resize;
		return CallUnSafeFunctionReturningVoid< IUTF8String_v1, sizet >(
			error, this, fnPtr, __FILE__, __LINE__, n );
	}

	pIUTF8String_base APICALL IUTF8String_I::replace( sizet pos, sizet count, const char * buf, sizet srcCount, pcIError_base & error ) __NOTHROW__{
		return CallUnSafeFunctionReturningSharedPointer< IUTF8String_v1, pIUTF8String_base, IUTF8String, sizet, sizet, const char *, sizet >(
			error, this, &IUTF8String_v1::replace, __FILE__, __LINE__, pos, count, buf, count );
	}

	pIUTF8String_base APICALL IUTF8String_I::replace( sizet pos, sizet count, pcIUTF8String_base src, sizet srcPos, sizet srcCount, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IUTF8String_v1, pIUTF8String_base, IUTF8String, sizet, sizet, const spcIUTF8String &, sizet, sizet >(
			error, this, &IUTF8String_v1::replace, __FILE__, __LINE__, pos, count, IUTF8String::MakeShared( src ), srcPos, srcCount );
	}

	sizet APICALL IUTF8String_I::copy( char * buf, sizet len, sizet pos, pcIError_base & error ) const __NOTHROW__ {
		typedef sizet ( APICALL IUTF8String_v1::*Func )( char *, sizet, sizet ) const;
		Func fnPtr = &IUTF8String_v1::copy;
		return CallConstUnSafeFunction< IUTF8String_v1, sizet, sizet, char *, sizet, sizet >(
			error, this, 0, fnPtr, __FILE__, __LINE__, buf, len, pos );
	}

	sizet APICALL IUTF8String_I::find( const char * buf, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ {
		typedef sizet ( APICALL IUTF8String_v1::*Func )( const char *, sizet, sizet ) const;
		Func fnPtr = &IUTF8String_v1::find;
		return CallConstUnSafeFunction< IUTF8String_v1, sizet, sizet, const char *, sizet, sizet >(
			error, this, 0, fnPtr, __FILE__, __LINE__, buf, pos, count );
	}

	sizet APICALL IUTF8String_I::find( pcIUTF8String_base src, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ {
		typedef sizet ( APICALL IUTF8String_v1::*Func )( const spcIUTF8String &, sizet, sizet ) const;
		Func fnPtr = &IUTF8String_v1::find;
		return CallConstUnSafeFunction< IUTF8String_v1, sizet, sizet, const spcIUTF8String &, sizet, sizet >(
			error, this, 0, fnPtr, __FILE__, __LINE__, IUTF8String::MakeShared( src ), pos, count );
	}

	sizet APICALL IUTF8String_I::rfind( const char * buf, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ {
		typedef sizet ( APICALL IUTF8String_v1::*Func )( const char *, sizet, sizet ) const;
		Func fnPtr = &IUTF8String_v1::rfind;
		return CallConstUnSafeFunction< IUTF8String_v1, sizet, sizet, const char *, sizet, sizet >(
			error, this, 0, fnPtr, __FILE__, __LINE__, buf, pos, count );
	}

	sizet APICALL IUTF8String_I::rfind( pcIUTF8String_base src, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ {
		typedef sizet( APICALL IUTF8String_v1::*Func )( const spcIUTF8String &, sizet, sizet ) const;
		Func fnPtr = &IUTF8String_v1::rfind;
		return CallConstUnSafeFunction< IUTF8String_v1, sizet, sizet, const spcIUTF8String &, sizet, sizet >(
			error, this, 0, fnPtr, __FILE__, __LINE__, IUTF8String::MakeShared( src ), pos, count );
	}

	int32 APICALL IUTF8String_I::compare( sizet pos, sizet len, const char * buf, sizet count, pcIError_base & error ) const __NOTHROW__ {
		typedef int32 ( APICALL IUTF8String_v1::*Func )( sizet, sizet, const char *, sizet ) const;
		Func fnPtr = &IUTF8String_v1::compare;
		return CallConstUnSafeFunction< IUTF8String_v1, int32, int32, sizet, sizet, const char *, sizet >(
			error, this, 0, fnPtr, __FILE__, __LINE__, pos, len, buf, count );
	}

	int32 APICALL IUTF8String_I::compare( sizet pos, sizet len, pcIUTF8String_base str, sizet strPos, sizet strLen, pcIError_base & error ) const __NOTHROW__ {
		typedef int32( APICALL IUTF8String_v1::*Func )( sizet, sizet, const spcIUTF8String &, sizet, sizet ) const;
		Func fnPtr = &IUTF8String_v1::compare;
		return CallConstUnSafeFunction< IUTF8String_v1, int32, int32, sizet, sizet, const spcIUTF8String &, sizet, sizet >(
			error, this, 0, fnPtr, __FILE__, __LINE__, pos, len, IUTF8String_I::MakeShared( str), strPos, strLen );
	}

	pIUTF8String_base APICALL IUTF8String_I::substr( sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IUTF8String_v1, pIUTF8String_base, IUTF8String, sizet, sizet >(
			error, this, &IUTF8String_v1::substr, __FILE__, __LINE__, pos, count );
	}

	uint32 APICALL IUTF8String_I::empty( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< IUTF8String_v1, uint32, bool >(
			error, this, 0, &IUTF8String_v1::empty, __FILE__, __LINE__ );
	}

	pvoid APICALL IUTF8String_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL IUTF8String_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		typedef pvoid ( APICALL IUTF8String_I::*Func )( uint64, uint32 );
		Func fnPtr = &IUTF8String_I::GetInterfacePointer;
		return CallUnSafeFunction< IUTF8String_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, fnPtr, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL IUTF8String_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kIUTF8StringID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< IUTF8String_v1 * >( this );
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
			IError_v1::kESOperationFatal, kIUTF8StringID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

}
