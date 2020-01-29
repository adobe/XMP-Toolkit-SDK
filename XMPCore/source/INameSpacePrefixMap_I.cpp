// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================


#include "XMPCore/Interfaces/INameSpacePrefixMap_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCommon/Interfaces/IUTF8String.h"

namespace AdobeXMPCore_Int {

	pvoid APICALL INameSpacePrefixMap_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL INameSpacePrefixMap_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< INameSpacePrefixMap_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, &INameSpacePrefixMap_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL INameSpacePrefixMap_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kINameSpacePrefixMapID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< INameSpacePrefixMap_v1 * >( this );
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
				IError_v1::kESOperationFatal, kINameSpacePrefixMapID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

	uint32 APICALL INameSpacePrefixMap_I::insert( const char * prefix, sizet prefixLength, const char * nameSpace, sizet nameSpaceLength, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< INameSpacePrefixMap_v1, uint32, bool, const char *, sizet, const char *, sizet >(
			error, this, 0, &INameSpacePrefixMap_v1::Insert, __FILE__, __LINE__, prefix, prefixLength, nameSpace, nameSpaceLength );
	}

	uint32 APICALL INameSpacePrefixMap_I::remove( uint32 keyType, const char * key, sizet keyLength, pcIError_base & error ) __NOTHROW__ {
		typedef bool ( APICALL INameSpacePrefixMap_v1::*Func )( const char *, sizet );
		Func fnPtr = &INameSpacePrefixMap_v1::RemovePrefix;
		if ( keyType == kNameSpaceIsParameter ) fnPtr = &INameSpacePrefixMap_v1::RemoveNameSpace;
		return CallUnSafeFunction< INameSpacePrefixMap_v1, uint32, bool >(
			error, this, 0, fnPtr, __FILE__, __LINE__, key, keyLength );
	}

	uint32 APICALL INameSpacePrefixMap_I::isPresent( uint32 keyType, const char * key, sizet keyLength, pcIError_base & error ) const __NOTHROW__ {
		typedef bool ( APICALL INameSpacePrefixMap_v1::*Func )( const char *, sizet ) const;
		Func fnPtr = &INameSpacePrefixMap_v1::IsPrefixPresent;
		if ( keyType == kNameSpaceIsParameter ) fnPtr = &INameSpacePrefixMap_v1::IsNameSpacePresent;
		return CallConstUnSafeFunction< INameSpacePrefixMap_v1, uint32, bool >(
			error, this, 0, fnPtr, __FILE__, __LINE__, key, keyLength );
	}

	pcIUTF8String_base APICALL INameSpacePrefixMap_I::get( uint32 keyType, const char * key, sizet keyLength, pcIError_base & error ) const __NOTHROW__ {
		typedef spcIUTF8String ( APICALL INameSpacePrefixMap_v1::*Func )( const char *, sizet ) const;
		Func fnPtr = &INameSpacePrefixMap_v1::GetNameSpace;
		if ( keyType == kNameSpaceIsParameter ) fnPtr = &INameSpacePrefixMap_v1::GetPrefix;
		return CallConstUnSafeFunctionReturningSharedPointer< INameSpacePrefixMap_v1, pcIUTF8String_base, const IUTF8String >(
			error, this, fnPtr, __FILE__, __LINE__, key, keyLength );
	}

	pINameSpacePrefixMap_base APICALL INameSpacePrefixMap_I::clone( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< INameSpacePrefixMap_v1, pINameSpacePrefixMap_base, INameSpacePrefixMap >(
			error, this, &INameSpacePrefixMap_v1::Clone, __FILE__, __LINE__ );
	}

}
