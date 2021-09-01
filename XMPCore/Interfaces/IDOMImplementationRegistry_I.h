#ifndef IDOMImplementationRegistry_I_h__
#define IDOMImplementationRegistry_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/XMPCoreFwdDeclarations_I.h"
#include "XMPCore/Interfaces/IDOMImplementationRegistry.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject_I.h"

namespace AdobeXMPCore_Int {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! \brief Internal interface that serves as a database/registry of all the parsers and
	//! serializers available with the XMPCore library.
	//! \details Provides all the functions to
	//!		-# get registered serializers and parsers from the database.
	//!		-# add client defined serializers and parsers to the database.
	//! \attention Support multi threading if library is configured to support multi-threading by default.
	//! \note By default following keys are registered by default with the database by the library:
	//!		-# rdf
	//!
	class IDOMImplementationRegistry_I
		: public virtual IDOMImplementationRegistry
		, public virtual ISharedObject_I
	{
	public:

		//!
		//! Registers a parser with the database along with the key.
		//! \param[in] key pointer to a const NULL terminated char buffer containing key of the parser to be used while registering.
		//! \param[in] parser a shared pointer to #ICDOMParser object to be registered with the database
		//! \return true in case parser is successfully registered, false otherwise like in case key is already registered.
		//! \attention currently it is unimplemented.
		//!
		virtual bool APICALL RegisterParser( const char * key, const spcIDOMParser & parser ) = 0;
		virtual bool APICALL RegisterParser( const char * key, pIClientDOMParser_base parser );

		//!
		//! Registers a serializer with the database along with the key.
		//! \param[in] key pointer to a const NULL terminated char buffer containing key of the serializer to be used while registering.
		//! \param[in] parser a shared pointer to #IDOMSerializer object to be registered with the database.
		//! \return true in case serializer is successfully registered, false otherwise like in case key is already registered.
		//! \attention currently it is unimplemented.
		//!
		virtual bool APICALL RegisterSerializer( const char * key, const spcIDOMSerializer & serializer ) = 0;
		virtual bool APICALL RegisterSerializer( const char * key, pIClientDOMSerializer_base serializer );

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }

		virtual pIDOMImplementationRegistry APICALL GetActualIDOMImplementationRegistry() __NOTHROW__ { return this; }
		virtual pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__{ return this; }
		virtual pIDOMImplementationRegistry_I APICALL GetIDOMImplementationRegistry_I() __NOTHROW__ { return this; }

		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

		// static factory functions

		//!
		//! Creates an IDOMImplementationRegistry object.
		//! \return a shared pointer to an IDOMImplementationRegistry object
		//!
		XMP_PRIVATE static spIDOMImplementationRegistry CreateDOMImplementationRegistry();

		//!
		//! Destory the IDOMImplementationRegistry object.
		//!
		XMP_PRIVATE static void DestoryDOMImplementationRegistry();

	protected:
		virtual ~IDOMImplementationRegistry_I() __NOTHROW__ {}
		pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );

		virtual pIDOMParser_base APICALL getParser( const char * key, pcIError_base & error ) const __NOTHROW__;
		virtual pIDOMSerializer_base APICALL getSerializer( const char * key, pcIError_base & error ) const __NOTHROW__;
		virtual uint32 APICALL registerParser( const char * key, pIClientDOMParser_base parser, pcIError_base & error ) __NOTHROW__;
		virtual uint32 APICALL registerSerializer( const char * key, pIClientDOMSerializer_base serializer, pcIError_base & error ) __NOTHROW__;
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

#endif // IDOMImplementationRegistry_I_h__
