#ifndef DOMImplementationRegistryImpl_h__
#define DOMImplementationRegistryImpl_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#if !(IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED)
	#error "Not adhering to design constraints"
	// this file should only be included from its own cpp file
#endif

#include "XMPCore/Interfaces/IDOMImplementationRegistry_I.h"
#include "XMPCommon/BaseClasses/MemoryManagedObject.h"
#include "XMPCommon/ImplHeaders/SharedObjectImpl.h"
#include "XMPCommon/Utilities/IUTF8StringComparator.h"
#include "XMPCommon/Utilities/TAllocator.h"

#include <map>
#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {

	class DOMImplementationRegistryImpl
		: public virtual IDOMImplementationRegistry_I
		, public virtual SharedObjectImpl
		, public virtual MemoryManagedObject
	{
	public:
		typedef std::map< spcIUTF8String, spcIDOMParser, IUTF8StringComparator, TAllocator< std::pair< const spcIUTF8String, spcIDOMParser > > > ParserMap;
		typedef std::map< spcIUTF8String, spcIDOMSerializer, IUTF8StringComparator, TAllocator< std::pair< const spcIUTF8String, spcIDOMSerializer > > > SerializerMap;

		DOMImplementationRegistryImpl() {}

		virtual bool APICALL RegisterParser( const char * key, const spcIDOMParser & parser );
		virtual bool APICALL RegisterSerializer( const char * key, const spcIDOMSerializer & serializer );
		virtual spIDOMParser APICALL GetParser( const char * key ) const;
		virtual spIDOMSerializer APICALL GetSerializer( const char * key ) const;


	protected:
		virtual ~DOMImplementationRegistryImpl() __NOTHROW__ {}

		ParserMap				mParserMap;
		SerializerMap			mSerializerMap;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#endif // DOMImplementationRegistryImpl_h__
