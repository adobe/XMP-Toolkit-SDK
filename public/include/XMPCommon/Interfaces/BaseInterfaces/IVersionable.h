#ifndef IVersionable_h__
#define IVersionable_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/XMPCommonFwdDeclarations.h"

namespace AdobeXMPCommon {

	//!
	//! \brief Interface that serves as the base interface for all the externally exposed interfaces
	//! which needs to provide evolving versions of the interface.
	//!	@details Provide pointer to interface requested by client.
	//! Requirements on the class type
	//!		-# Need to implement a function GetInterfaceID() returning a unique id for the
	//!		interface. Only required to be implemented in first version of the interface.
	//!		-# Need to implement a function GetVersionNumber() returning the version of the
	//!		interface. Required to implemented by each version of the interface.
	//!

	class XMP_PUBLIC IVersionable {
	public:

		//!
		//! @brief Get the raw pointer to an interface object implementing the requested version.
		//! \return a raw pointer to an interface object implementing the requested version.
		//! \attention In case a particular version number is not supported than an error is
		//!            thrown.
		//!
		template< typename requestedInterface >
		XMP_PRIVATE requestedInterface * GetInterfacePointer() {
			pvoid ptr = GetInterfacePointer( requestedInterface::GetInterfaceID(),
				requestedInterface::GetInterfaceVersion() );
			return static_cast< requestedInterface * >( ptr );
		}

		//!
		//! @brief Get the raw pointer to a const interface object implementing the requested version.
		//! \return a raw pointer to a const interface object implementing the requested version.
		//! \attention In case a particular version number is not supported than an error is
		//!            thrown.
		//!
		template< typename requestedInterface >
		XMP_PRIVATE const requestedInterface * GetInterfacePointer() const {
			return const_cast< IVersionable * >( this )->GetInterfacePointer< requestedInterface >();
		}

	protected:
		virtual ~IVersionable() {}
		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) = 0;
		// all safe functions
		virtual pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ = 0;
		//! \endcond

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};

}

#endif // IVersionable_h__
