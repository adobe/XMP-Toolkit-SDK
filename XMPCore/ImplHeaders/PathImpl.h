#ifndef PathImpl_h__
#define PathImpl_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#if !(IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED)
	#error "Not adhering to design constraints"
	// this file should only be included from its own cpp file
#endif

#include "XMPCore/Interfaces/IPath_I.h"
#include "XMPCommon/BaseClasses/MemoryManagedObject.h"
#include "XMPCommon/ImplHeaders/SharedObjectImpl.h"
#include "XMPCommon/Utilities/TAllocator.h"
#include <vector>

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {

	class PathImpl
		: public virtual IPath_I
		, public virtual SharedObjectImpl
		, public virtual MemoryManagedObject
	{
	public:
		typedef std::vector< spcIPathSegment, TAllocator< spcIPathSegment > > spcIPathSegments;


	protected:
		virtual ~PathImpl() __NOTHROW__ {}

		virtual spcINameSpacePrefixMap APICALL RegisterNameSpacePrefixMap( const spcINameSpacePrefixMap & map );
		virtual spIUTF8String APICALL Serialize( const spcINameSpacePrefixMap & map = spcINameSpacePrefixMap() ) const;
		virtual void APICALL AppendPathSegment( const spcIPathSegment & segment );
		virtual spcIPathSegment APICALL RemovePathSegment( sizet index );
		virtual spcIPathSegment APICALL GetPathSegment( sizet index ) const;
		virtual sizet APICALL Size() const __NOTHROW__;
		virtual void APICALL Clear() __NOTHROW__;
		virtual spIPath APICALL Clone( sizet startingIndex = 1, sizet countOfSegments = kMaxSize ) const;

		spcIPathSegments				mSegments;
		spcINameSpacePrefixMap			mNameSpacePrefixMap;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif

#endif // PathImpl_h__
