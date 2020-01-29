// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/DOMImplementationRegistryImpl.h"
	#include "XMPCore/ImplHeaders/RDFDOMParserImpl.h"
	#include "XMPCore/ImplHeaders/RDFDOMSerializerImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCore/Interfaces/IDOMParser.h"
#include "XMPCore/Interfaces/IDOMSerializer.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"

namespace AdobeXMPCore_Int {

	bool APICALL DOMImplementationRegistryImpl::RegisterParser( const char * key, const spcIDOMParser & parser ) {
		spcIUTF8String  keyStr( IUTF8String_I::CreateUTF8String( key, AdobeXMPCommon::npos ) );
		auto it = mParserMap.insert( std::pair< spcIUTF8String, spcIDOMParser >( keyStr, parser ) );
		return it.second;
	}

	bool APICALL DOMImplementationRegistryImpl::RegisterSerializer( const char * key, const spcIDOMSerializer & serializer ) {
		spcIUTF8String  keyStr( IUTF8String_I::CreateUTF8String( key, AdobeXMPCommon::npos ) );
		auto it = mSerializerMap.insert( std::pair< spcIUTF8String, spcIDOMSerializer >( keyStr, serializer ) );
		return it.second;
	}

	spIDOMParser APICALL DOMImplementationRegistryImpl::GetParser( const char * key ) const {
		spcIUTF8String  keyStr( IUTF8String_I::CreateUTF8String( key, AdobeXMPCommon::npos ) );
		auto it = mParserMap.find( keyStr );
		if ( it != mParserMap.end() ) {
			return it->second->Clone();
		}
		return spIDOMParser();
	}

	spIDOMSerializer APICALL DOMImplementationRegistryImpl::GetSerializer( const char * key ) const {
		spcIUTF8String  keyStr( IUTF8String_I::CreateUTF8String( key, AdobeXMPCommon::npos ) );
		auto it = mSerializerMap.find( keyStr );
		if ( it != mSerializerMap.end() ) {
			return it->second->Clone();
		}
		return spIDOMSerializer();
	}

	static spIDOMImplementationRegistry ManageDOMImplementationRegistry( bool destroy = false ) {
		static spIDOMImplementationRegistry sDOMImplementationRegistry;

		if ( destroy ) {
			if ( sDOMImplementationRegistry )
				sDOMImplementationRegistry.reset();
			return sDOMImplementationRegistry;
		}

		if ( !sDOMImplementationRegistry ) {
			DOMImplementationRegistryImpl * ptr = new DOMImplementationRegistryImpl();
			ptr->RegisterParser( "rdf", MakeUncheckedSharedPointer( new RDFDOMParserImpl(), __FILE__, __LINE__, true ) );
			ptr->RegisterSerializer( "rdf", MakeUncheckedSharedPointer( new RDFDOMSerializerImpl(), __FILE__, __LINE__, true ) );
			sDOMImplementationRegistry = MakeUncheckedSharedPointer( ptr, __FILE__, __LINE__, true );
		}
		return sDOMImplementationRegistry;
	}

	spIDOMImplementationRegistry IDOMImplementationRegistry_I::CreateDOMImplementationRegistry() {
		return ManageDOMImplementationRegistry();
	}

	void IDOMImplementationRegistry_I::DestoryDOMImplementationRegistry() {
		ManageDOMImplementationRegistry( true );
	}

}

#if BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
namespace AdobeXMPCore {
	using namespace AdobeXMPCore_Int;
	spIDOMImplementationRegistry IDOMImplementationRegistry_v1::MakeShared( pIDOMImplementationRegistry_base ptr ) {
		if ( !ptr ) return spIDOMImplementationRegistry();
		pIDOMImplementationRegistry p = IDOMImplementationRegistry::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IDOMImplementationRegistry >() : ptr;
		return MakeUncheckedSharedPointer( p, __FILE__, __LINE__, false );
	}

	spIDOMImplementationRegistry IDOMImplementationRegistry::GetDOMImplementationRegistry() {
		return AdobeXMPCore_Int::ManageDOMImplementationRegistry();
	}
}
#endif  // BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
