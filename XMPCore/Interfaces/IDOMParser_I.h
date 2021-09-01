#ifndef IDOMParser_I_h__
#define IDOMParser_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/XMPCoreFwdDeclarations_I.h"
#include "XMPCore/Interfaces/IDOMParser.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject_I.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IConfigurable_I.h"
#include "XMPCore/source/XMPMeta.hpp"

namespace AdobeXMPCore_Int {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! \brief Internal interface that supports parsing of the XMP Data Model.
	//! Provides all functions to parse the buffer as well as to configure the parser.
	//!
	class IDOMParser_I
		: public virtual IDOMParser
		, public virtual IConfigurable_I
		, public virtual ISharedObject_I
	{
	public:

		//!
		//! Parse the buffer contents and creates an XMP DOM node.
		//! \param[in] buffer pointer to a constant char buffer containing serialized XMP Data Model.
		//! \param[in] bufferLength number of characters in buffer. In case name is null terminated set it to #AdobeXMPCommon::npos.
		//! \return a shared pointer to an object of #INode containing all the information parsed from the buffer.
		//!
		virtual spINode APICALL ParseAsNode( const char * buffer, sizet bufferLength ) = 0;

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }

		virtual pIDOMParser APICALL GetActualIDOMParser() __NOTHROW__ { return this; }
		virtual pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__{ return this; }
		virtual pIDOMParser_I APICALL GetIDOMParser_I() __NOTHROW__ { return this; }

		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

		// Factory functions
		
		//!
		//! Creates a DOM Parser from the client DOM Parser.
		//! \param[in] clientDOMParser a pointer to an object of type #IClientDOMParser_v1.
		//! \return a shared pointer to an object of type #IDOMParser.
		//!
		static spIDOMParser CreateDOMParser( pIClientDOMParser_base clientDOMParser );


		virtual void SetErrorCallback(XMPMeta::ErrorCallbackInfo * ec) = 0;

	protected:
		virtual ~IDOMParser_I() __NOTHROW__ {}
		virtual pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__;
		pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );

		virtual pIDOMParser_base APICALL clone( pcIError_base & error ) const __NOTHROW__;
		virtual pIMetadata_base APICALL parse( const char * buffer, sizet bufferLength, pcIError_base & error ) __NOTHROW__;
		virtual void APICALL parseWithSpecificAction( const char * buffer, sizet bufferLength, uint32 actionType, pINode_base node, pcIError_base & error ) __NOTHROW__;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};

#if XMP_WinBuild
	#pragma warning( pop )
#endif

}


#endif // IDOMParser_I_h__
