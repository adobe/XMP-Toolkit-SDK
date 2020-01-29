#ifndef TNodeIteratorImpl_h__
#define TNodeIteratorImpl_h__ 1

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

#include "XMPCore/Interfaces/INodeIterator_I.h"
#include "XMPCommon/BaseClasses/MemoryManagedObject.h"
#include "XMPCommon/ImplHeaders/SharedObjectImpl.h"

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {

	template< typename iterator >
	class TNodeIteratorImpl
		: public virtual INodeIterator_I
		, public virtual SharedObjectImpl
		, public virtual MemoryManagedObject
	{
	public:
		TNodeIteratorImpl( const iterator & begin, const iterator & end )
			: mCurrentPos( begin )
			, mEndPos( end ) {}

		spINode GetNodeFromIterator( const iterator & it ) const;

		virtual spINode APICALL GetNode() {
			if ( mCurrentPos != mEndPos )
				return GetNodeFromIterator( mCurrentPos );
			else
				return spINode();
		}

		virtual INode_v1::eNodeType APICALL GetNodeType() const {
			if ( mCurrentPos != mEndPos )
				return GetNodeFromIterator( mCurrentPos )->GetNodeType();
			else
				return INode_v1::kNTNone;
		}

		virtual spINodeIterator APICALL Next() {
			mCurrentPos++;
			if ( mCurrentPos != mEndPos ) {
				return MakeUncheckedSharedPointer( this, __FILE__, __LINE__, false );
			}
			return spINodeIterator();
		}

	protected:
		virtual ~TNodeIteratorImpl() __NOTHROW__ {}

		iterator			mCurrentPos;
		iterator			mEndPos;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif

#endif // TNodeIteratorImpl_h__
