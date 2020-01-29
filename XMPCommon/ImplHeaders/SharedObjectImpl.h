#ifndef __SharedObjectImpl_h__
#define __SharedObjectImpl_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject_I.h"
#include "XMPCommon/Utilities/TAtomicTypes.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	class SharedObjectImpl
		: public virtual ISharedObject_I
	{
	public:
		SharedObjectImpl()
			: mRefCount( 0 )
			, mCountInternal( 0 ) { }

		virtual void APICALL Acquire() const __NOTHROW__;
		virtual void APICALL Release() const __NOTHROW__;
		virtual void APICALL AcquireInternal() const __NOTHROW__;

	protected:
		SharedObjectImpl( const SharedObjectImpl & );
		SharedObjectImpl & operator = ( const SharedObjectImpl & );
		virtual ~SharedObjectImpl() __NOTHROW__;

	protected:
		mutable atomic_sizet			mRefCount;
		mutable atomic_sizet			mCountInternal;

	#ifndef FRIEND_CLASS_DECLARATION
		#define FRIEND_CLASS_DECLARATION() 
	#endif
		FRIEND_CLASS_DECLARATION();
		REQ_FRIEND_CLASS_DECLARATION();

	};
}

#endif  // __SharedObjectImpl_h__
