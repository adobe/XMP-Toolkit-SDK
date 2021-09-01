#ifndef ICoreObjectFactory_I_h__
#define ICoreObjectFactory_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/XMPCoreFwdDeclarations_I.h"
#include "XMPCore/Interfaces/ICoreObjectFactory.h"

namespace AdobeXMPCore_Int {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! \brief Internal interface that represents a factory to create various artifacts of XMP DOM like array,
	//! structure, path etc.
	//!
	//! Provides all the functions to create instances of various artifacts of XMP DOM and return them as shared pointers
	//! to the clients. This is the interface through which clients of the library actually get access to all other interfaces.
	//!
	//!
	class ICoreObjectFactory_I
		: public virtual ICoreObjectFactory
	{
	public:

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }

		virtual pICoreObjectFactory APICALL GetActualICoreObjectFactory() __NOTHROW__ { return this; }
		virtual pICoreObjectFactory_I APICALL GetICoreObjectFactory_I() __NOTHROW__ { return this; }

		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

	protected:
		virtual ~ICoreObjectFactory_I() __NOTHROW__ {}
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

#endif // ICoreObjectFactory_I_h__
