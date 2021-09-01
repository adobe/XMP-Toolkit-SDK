#ifndef CoreConfigurationManagerImpl_h__
#define CoreConfigurationManagerImpl_h__ 1

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

#include "XMPCore/Interfaces/ICoreConfigurationManager_I.h"
#include "XMPCommon/ImplHeaders/ConfigurationManagerImpl.h"

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {

	class CoreConfigurationManagerImpl
		: public virtual ICoreConfigurationManager_I
		, public virtual ConfigurationManagerImpl
	{
	public:
		CoreConfigurationManagerImpl();


	protected:
		virtual ~CoreConfigurationManagerImpl() __NOTHROW__ {}

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif
#endif  // CoreConfigurationManagerImpl_h__

