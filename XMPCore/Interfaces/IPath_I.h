#ifndef __IPath_I_h__
#define __IPath_I_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================


#include "XMPCore/XMPCoreFwdDeclarations_I.h"
#include "XMPCore/Interfaces/IPath.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject_I.h"

namespace AdobeXMPCore_Int {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! Internal interface that provides an easy iterative description of a specific path into the XMP tree.
	//! \details Path consists of multiple path segments in an order and each #AdobeXMPCore::IPathSegment represents one segment
	//! of the path into the XMP tree. Provides all the functions to create path and get the various properties of a path.
	//! \attention Do not support multi-threading.
	//! \note Index in the path are 1-based.
	//!
	class IPath_I
		: public virtual IPath
		, public virtual ISharedObject_I
	{
	public:

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }

		virtual pIPath APICALL GetActualIPath() __NOTHROW__ { return this; }
		virtual pIPath_I APICALL GetIPath_I() __NOTHROW__ { return this; }

		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

	protected:
		virtual ~IPath_I() __NOTHROW__ {}
		pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );

		virtual pcINameSpacePrefixMap_base APICALL registerNameSpacePrefixMap( pcINameSpacePrefixMap_base map, pcIError_base & error ) __NOTHROW__;
		virtual pIUTF8String_base APICALL serialize( pcINameSpacePrefixMap_base map, pcIError_base & error ) const __NOTHROW__;
		virtual void APICALL appendPathSegment( pcIPathSegment_base segment, pcIError_base & error ) __NOTHROW__;
		virtual pcIPathSegment_base APICALL removePathSegment( sizet index, pcIError_base & error ) __NOTHROW__;
		virtual pcIPathSegment_base APICALL getPathSegment( sizet index, pcIError_base & error ) const __NOTHROW__;
		virtual pIPath_base APICALL clone( sizet startingIndex, sizet countOfSegemetns, pcIError_base & error ) const __NOTHROW__;
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

#endif  // __IPath_I_h__
