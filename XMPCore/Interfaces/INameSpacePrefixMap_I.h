#ifndef INameSpacePrefixMap_I_h__
#define INameSpacePrefixMap_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/XMPCoreFwdDeclarations_I.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject_I.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IThreadSafe_I.h"

namespace AdobeXMPCore_Int {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! \brief Internal Interface that represents map where each entry consists of prefix string
	//! as the key and corresponding nameSpace string as its value.
	//! \details Provides all the functions to get/set the entries inside the map
	//! \attention Supports Multi-threading at object level through locks.
	//!

	class INameSpacePrefixMap_I
		: public virtual INameSpacePrefixMap
		, public virtual ISharedObject_I
		, public virtual IThreadSafe_I
	{
	public:

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }

		using INameSpacePrefixMap_v1::GetNameSpace;
		using INameSpacePrefixMap_v1::GetPrefix;

		virtual spcIUTF8String APICALL GetNameSpace( const spcIUTF8String & prefix ) const = 0;
		virtual spcIUTF8String APICALL GetPrefix( const spcIUTF8String & nameSpace ) const = 0;
		virtual void APICALL Merge( const spcINameSpacePrefixMap & otherMap ) = 0;

		virtual pINameSpacePrefixMap APICALL GetActualINameSpacePrefixMap() __NOTHROW__ { return this; }
		virtual pINameSpacePrefixMap_I APICALL GetINameSpacePrefixMap_I() __NOTHROW__ { return this; }

		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

		// static factory functions

		//!
		//! Creates the DefaultNameSpacePrefixMap
		//!
		static void CreateDefaultNameSpacePrefixMap();

		//!
		//! Destroys the DefaultNameSpacePrefixMap
		//!
		static void DestroyDefaultNameSapcePrefixMap();

		//!
		//! Adds an entry in the default mapping used by XMP Core
		//! \param[in] prefix pointer to a constant char buffer containing prefix string.
		//! \param[in] prefixLength number of characters in prefix. In case prefix is null set it to AdobeXMPCommon::npos.
		//! \param[in] nameSpace pointer to a constant char buffer containing nameSpace string.
		//! \param[in] nameSpaceLength number of characters in nameSpace. In case nameSpace is null set it to AdobeXMPCommon::npos.
		//! \return a bool type object containing true in case operation was successful, false otherwise.
		//! \note raises warning in case of
		//!		- prefix or nameSpace are null pointers
		//!		- prefixLength or nameSpaceLength is 0.
		//!
		static bool InsertInDefaultNameSpacePrefixMap( const char * prefix, sizet prefixLength, const char * nameSpace, sizet nameSpaceLength );

	protected:
		virtual ~INameSpacePrefixMap_I() __NOTHROW__ {}
		pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );

		virtual uint32 APICALL insert( const char * prefix, sizet prefixLength, const char * nameSpace, sizet nameSpaceLength, pcIError_base & error ) __NOTHROW__;
		virtual uint32 APICALL remove( uint32 keyType, const char * key, sizet keyLength, pcIError_base & error ) __NOTHROW__;
		virtual uint32 APICALL isPresent( uint32 keyType, const char * key, sizet keyLength, pcIError_base & error ) const __NOTHROW__;
		virtual pcIUTF8String_base APICALL get( uint32 keyType, const char * key, sizet keyLength, pcIError_base & error ) const __NOTHROW__;
		virtual pINameSpacePrefixMap_base APICALL clone( pcIError_base & error ) const __NOTHROW__;
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

#endif // INameSpacePrefixMap_I_h__
