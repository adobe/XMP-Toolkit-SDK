#ifndef ICoreConfigurationManager_h__
#define ICoreConfigurationManager_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPCommon/Interfaces/IConfigurationManager.h"

namespace AdobeXMPCore {
	//!
	//! @brief Version1 of the interface that  represents configuration settings controllable by the client.
	//! \details Provides functions through which client can plug in its own memory allocators, error notifiers.
	//! \attention Not Thread Safe as this functionality is generally used at the initialization phase.
	//!
	class XMP_PUBLIC ICoreConfigurationManager_v1
		: public virtual IConfigurationManager_v1
	{
	public:

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Returns the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return Either a const or non const pointer to ICoreConfigurationManager interface.
		//!
		virtual pICoreConfigurationManager APICALL GetActualICoreConfigurationManager() __NOTHROW__ = 0;
		XMP_PRIVATE pcICoreConfigurationManager GetActualICoreConfigurationManager() const __NOTHROW__ {
			return const_cast< ICoreConfigurationManager_v1 * >( this )->GetActualICoreConfigurationManager();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Returns the pointer to internal interfaces.
		//! \return Either a const or non const pointer to ICoreConfigurationManager_I interface.
		//!
		virtual AdobeXMPCore_Int::pICoreConfigurationManager_I APICALL GetICoreConfigurationManager_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPCore_Int::pcICoreConfigurationManager_I GetICoreConfigurationManager_I() const __NOTHROW__ {
			return const_cast< ICoreConfigurationManager_v1 * >( this )->GetICoreConfigurationManager_I();
		}
		//!
		//! @}
		
		//!
		//! @{
		//! @brief Converts raw pointer to shared pointer. The raw pointer is of version 1 interface
		//! where as the returned shared pointer depends on the version client is interested in.
		//! \return Shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spICoreConfigurationManager MakeShared( pICoreConfigurationManager_base ptr );
		XMP_PRIVATE static spcICoreConfigurationManager MakeShared( pcICoreConfigurationManager_base ptr ) {
			return MakeShared( const_cast< pICoreConfigurationManager_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief Return the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kICoreConfigurationManagerID; }

		//!
		//! @brief Returns the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

		// static factory functions

		//!
		//! @brief Get the configuration manager object associated with XMPCore library..
		//! \return A shared pointer to an object of \#ICoreConfigurationManager.
		//!
		XMP_PRIVATE static spICoreConfigurationManager GetCoreConfigurationManager();


	protected:
		//!
		//! Destructor
		//! 
		virtual ~ICoreConfigurationManager_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}
#endif  // ICoreConfigurationManager_h__

