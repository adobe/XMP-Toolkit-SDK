#ifndef ErrorNotifierWrapperImpl_h__
#define ErrorNotifierWrapperImpl_h__ 1

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

#include "XMPCommon/Interfaces/IErrorNotifier_I.h"
#include "XMPCommon/BaseClasses/MemoryManagedObject.h"

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif


namespace XMP_COMPONENT_INT_NAMESPACE {

	class ErrorNotifierWrapperImpl_v1
		: public virtual IErrorNotifier_I
		, public virtual MemoryManagedObject
	{
	public:
		virtual bool APICALL Notify( const spcIError & error );

	protected:
		ErrorNotifierWrapperImpl_v1( IErrorNotifier_v1 * clientNotifier );
		virtual ~ErrorNotifierWrapperImpl_v1() __NOTHROW__{};

		IErrorNotifier_v1 *					mErrorNotifier;

		friend pIErrorNotifier CreateErrorNotifierWrapperImpl( pIErrorNotifier_base errorNotifier );
	};
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif

#endif  // ErrorNotifierWrapperImpl_h__
