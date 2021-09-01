// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/Interfaces/IDOMImplementationRegistry_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/Interfaces/IDOMParser_I.h"
#include "XMPCore/Interfaces/IDOMSerializer_I.h"
#include <stdexcept>

namespace AdobeXMPCore_Int {

	pvoid APICALL IDOMImplementationRegistry_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL IDOMImplementationRegistry_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< IDOMImplementationRegistry_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, &IDOMImplementationRegistry_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL IDOMImplementationRegistry_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kIDOMImplementationRegistryID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< IDOMImplementationRegistry_v1 * >( this );
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
				IError_v1::kESOperationFatal, kIDOMImplementationRegistryID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

	pIDOMParser_base APICALL IDOMImplementationRegistry_I::getParser( const char * key, pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IDOMImplementationRegistry_v1, pIDOMParser_base, IDOMParser, const char * >(
			error, this, &IDOMImplementationRegistry_v1::GetParser, __FILE__, __LINE__, key );
	}

	pIDOMSerializer_base APICALL IDOMImplementationRegistry_I::getSerializer( const char * key, pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IDOMImplementationRegistry_v1, pIDOMSerializer_base, IDOMSerializer, const char * >(
			error, this, &IDOMImplementationRegistry_v1::GetSerializer, __FILE__, __LINE__, key );
	}

	uint32 APICALL IDOMImplementationRegistry_I::registerParser( const char * key, pIClientDOMParser_base parser, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< IDOMImplementationRegistry_I, uint32, bool, const char *, const spcIDOMParser & >(
			error, this, 0, &IDOMImplementationRegistry_I::RegisterParser, __FILE__, __LINE__, key, IDOMParser_I::CreateDOMParser( parser ) );
	}

	uint32 APICALL IDOMImplementationRegistry_I::registerSerializer( const char * key, pIClientDOMSerializer_base serializer, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< IDOMImplementationRegistry_I, uint32, bool, const char *, const spcIDOMSerializer & >(
			error, this, 0, &IDOMImplementationRegistry_I::RegisterSerializer, __FILE__, __LINE__, key, IDOMSerializer_I::CreateDOMSerializer( serializer ) );
	}

	bool APICALL IDOMImplementationRegistry_I::RegisterParser(const char * key, pIClientDOMParser_base parser) {

		spcIDOMParser spParser = IDOMParser_I::CreateDOMParser(parser);
		return RegisterParser(key, spParser);
	}

	bool APICALL IDOMImplementationRegistry_I::RegisterSerializer(const char * key, pIClientDOMSerializer_base serializer) {

		spcIDOMSerializer spSerializer = IDOMSerializer_I::CreateDOMSerializer(serializer);
		return RegisterSerializer(key, spSerializer);
	}

}
