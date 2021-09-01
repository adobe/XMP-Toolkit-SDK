// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#if !(IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED)
	#error "Not adhering to design constraints"
	// this file should only be included from its own cpp file
#endif

#include "XMPCommon/Interfaces/BaseInterfaces/IThreadSafe_I.h"

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace XMP_COMPONENT_INT_NAMESPACE {

	class ThreadSafeImpl
		: public virtual IThreadSafe_I
	{
	public:
		ThreadSafeImpl() {}


	protected:
		virtual ~ThreadSafeImpl() {}

		virtual void APICALL ShareMutex( const spISharedMutex & mutex );
		virtual void APICALL UnShareMutex();
		virtual void APICALL EnableThreadSafety() const __NOTHROW__;
		virtual void APICALL DisableThreadSafety() const __NOTHROW__;
		virtual bool APICALL IsThreadSafe() const;

		mutable spISharedMutex			mSharedMutex;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif
