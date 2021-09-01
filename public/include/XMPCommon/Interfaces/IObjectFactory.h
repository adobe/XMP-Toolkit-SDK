#ifndef IObjectFactory_h__
#define IObjectFactory_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/XMPCommonFwdDeclarations.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"

namespace AdobeXMPCommon {

	//!
	//! \brief Version1 of a interface that represents a factory to create various artifacts defined within
	//! AdobeXMPCommon namespace.
	//! @details Provides all the functions to create instances of various artifacts defined with AdobeXMPCommon namespace. This
	//! is the interface through which clients of the library actually get access to all other interfaces.
	//!

	class XMP_PUBLIC IObjectFactory_v1
		: public IVersionable
	{
	public:

		//!
		//! @brief Creates an IUTF8String object.
		//! \param[in] buf pointer to a constant char buffer containing content. It can be null
		//! terminated or not. NULL pointer will be treated as empty string.
		//! \param[in] count A value of \#AdobeXMPCommon::sizet indicating the length in case buf is not null
		//! terminated. In case buf is null terminated it can be set to npos.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \attention The returned pointer is allocated on heap by the module so client is responsible for its release.
		//! They should call Release once they no longer need this object.
		//!
		virtual pIUTF8String_base APICALL CreateUTF8String( const char * buf, sizet count, pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Creates an IError object.
		//! \param[in] domain An unsigned 32 bit integer value representing the error domain.
		//! \param[in] code An unsigned 32 bit integer value representing the error code.
		//! \param[in] severity An unsigned 32 bit integer value representing the severity of the error.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \attention The returned pointer is allocated on heap by the module so client is responsible for its release.
		//! They should call Release once they no longer need this object.
		//!
		virtual pIError_base APICALL CreateError( uint32 domain, uint32 code, uint32 severity, pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @{
		//! @details Convert raw pointer to shared pointer. The raw pointer is of version 1 interface
		//! where as the returned returned pointer depends on the version client is interested in.
		//! \return Raw pointer to const or non constant IObjectFactory interface.
		//!
		XMP_PRIVATE static pIObjectFactory MakeObjectFactory( pIObjectFactory_base ptr ) {
			return IObjectFactory::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IObjectFactory >() : ptr;
		}
		XMP_PRIVATE static pcIObjectFactory MakeObjectFactory( pcIObjectFactory_base ptr ) {
			return MakeObjectFactory( const_cast< pIObjectFactory_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief Returns the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIObjectFactoryID; }

		//!
		//! @brief Returns the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }

		virtual ~IObjectFactory_v1() __NOTHROW__ {};
		
	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};

}

#endif  // IObjectFactory_h__
