#ifndef ConfigurationManagerImpl_h__
#define ConfigurationManagerImpl_h__ 1

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

#include "XMPCommon/Interfaces/IConfigurationManager_I.h"

#include "XMPCommon/ImplHeaders/SharedObjectImpl.h"
#include "XMPCommon/BaseClasses/MemoryManagedObject.h"

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace XMP_COMPONENT_INT_NAMESPACE {

	class ConfigurationManagerImpl
		: public virtual IConfigurationManager_I
		, public virtual SharedObjectImpl
		, public virtual MemoryManagedObject
	{
	public:
		ConfigurationManagerImpl() : mMultiThreadingEnabled( true ) {}

		virtual bool APICALL RegisterMemoryAllocator( pIMemoryAllocator memoryAllocator );
		virtual bool APICALL RegisterErrorNotifier( pIErrorNotifier_base clientErrorNotifier );
		virtual bool APICALL DisableMultiThreading();
		virtual bool APICALL IsMultiThreaded() const;

	protected:
		~ConfigurationManagerImpl() __NOTHROW__ {}
		bool				mMultiThreadingEnabled;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();

	};
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif

#endif  // ConfigurationManagerImpl_h__
