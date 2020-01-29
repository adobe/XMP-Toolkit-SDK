#ifndef NameSpacePrefixMapImpl_h__
#define NameSpacePrefixMapImpl_h__ 1

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

#if !(IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED)
	#error "Not adhering to design constraints"
	// this file should only be included from its own cpp file
#endif

#include "XMPCore/Interfaces/INameSpacePrefixMap_I.h"
#include "XMPCommon/BaseClasses/MemoryManagedObject.h"
#include "XMPCommon/ImplHeaders/SharedObjectImpl.h"
#include "XMPCommon/ImplHeaders/ThreadSafeImpl.h"
#include "XMPCommon/Utilities/TAllocator.h"
#include "XMPCommon/Utilities/IUTF8StringComparator.h"


#include <map>

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {

	class NameSpacePrefixMapImpl
		: public virtual INameSpacePrefixMap_I
		, public virtual SharedObjectImpl
		, public virtual MemoryManagedObject
		, public virtual ThreadSafeImpl
	{
	public:
		typedef std::pair< spcIUTF8String, spcIUTF8String > NameSpacePrefixPair;
		typedef std::map< spcIUTF8String, spcIUTF8String, IUTF8StringComparator, TAllocator< std::pair< const spcIUTF8String, spcIUTF8String > > > NameSpacePrefixMap;

		NameSpacePrefixMapImpl();


	protected:
		virtual ~NameSpacePrefixMapImpl() __NOTHROW__ {}

		virtual bool APICALL Insert( const char * prefix, sizet prefixLength, const char * nameSpace, sizet nameSpaceLength );
		virtual bool APICALL RemovePrefix( const char * prefix, sizet prefixLength );
		virtual bool APICALL RemoveNameSpace( const char * nameSpace, sizet nameSpaceLength );
		virtual bool APICALL IsPrefixPresent( const char * prefix, sizet prefixLength ) const;
		virtual bool APICALL IsNameSpacePresent( const char * nameSpace, sizet nameSpaceLength ) const;
		virtual spcIUTF8String APICALL GetNameSpace( const char * prefix, sizet prefixLength ) const;
		virtual spcIUTF8String APICALL GetNameSpace( const spcIUTF8String & prefix ) const;
		virtual spcIUTF8String APICALL GetPrefix( const char * nameSpace, sizet nameSpaceLength ) const;
		virtual spcIUTF8String APICALL GetPrefix( const spcIUTF8String & nameSpace ) const;
		virtual void APICALL Merge( const spcINameSpacePrefixMap & otherMap );
		virtual sizet APICALL Size() const __NOTHROW__;
		virtual void APICALL Clear() __NOTHROW__;
		virtual spINameSpacePrefixMap APICALL Clone() const;

		NameSpacePrefixMap				mNameSpaceToPrefixMap;
		NameSpacePrefixMap				mPrefixToNameSpaceMap;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif

#endif // NameSpacePrefixMapImpl_h__
