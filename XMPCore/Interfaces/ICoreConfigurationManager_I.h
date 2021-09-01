#ifndef ICoreConfigurationManager_I_h__
#define ICoreConfigurationManager_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/XMPCoreFwdDeclarations_I.h"
#include "XMPCore/Interfaces/ICoreConfigurationManager.h"
#include "XMPCommon/Interfaces/IConfigurationManager_I.h"

namespace AdobeXMPCore_Int {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! \brief Internal interface that represents interface that represents configuration settings controllable by the client.
	//! \details Provides functions through which client can plug in its own memory allocators, error notifiers.
	//! \attention Not Thread Safe as this functionality is generally used at the initialization phase.
	//!
	class ICoreConfigurationManager_I
		: public virtual ICoreConfigurationManager
		, public virtual IConfigurationManager_I
	{
	public:

		//!
		//! Create the configuration manager for the XMPCore library.
		//!
		static void CreateCoreConfigurationManager();

		//!
		//! Destroy the configuration manager of the XMPCore library.
		//!
		static void DestroyCoreConfigurationManager();

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }

		virtual pICoreConfigurationManager APICALL GetActualICoreConfigurationManager() __NOTHROW__ { return this; }
		virtual pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__{ return this; }
		virtual pICoreConfigurationManager_I APICALL GetICoreConfigurationManager_I() __NOTHROW__ { return this; }

		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

	protected:
		virtual ~ICoreConfigurationManager_I() __NOTHROW__ {}
		pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );
		virtual pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};

#if XMP_WinBuild
	#pragma warning( pop )
#endif

}
#endif  // ICoreConfigurationManager_I_h__

