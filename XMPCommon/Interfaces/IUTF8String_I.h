#ifndef __IUTF8String_I_h__
#define __IUTF8String_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#include "XMPCommon/XMPCommonFwdDeclarations_I.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! \brief Internal interface that represents an UTF8String.
	//!	\details Provides all the functions to access properties of the string object, append or assign content
	//! to the existing string objects and create empty string object or clone existing string objects.
	//!
	class IUTF8String_I
		: public virtual IUTF8String
		, public virtual ISharedObject_I
	{
	public:

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }

		using IUTF8String_v1::assign;
		using IUTF8String_v1::append;
		using IUTF8String_v1::insert;
		using IUTF8String_v1::erase;
		using IUTF8String_v1::resize;
		using IUTF8String_v1::replace;
		using IUTF8String_v1::copy;
		using IUTF8String_v1::find;
		using IUTF8String_v1::rfind;
		using IUTF8String_v1::compare;
		using IUTF8String_v1::substr;
		using IUTF8String_v1::empty;

		virtual pIUTF8String APICALL GetActualIUTF8String() __NOTHROW__ { return this; }

		virtual pIUTF8String_base APICALL assign( const char * buffer, sizet count, pcIError_base & error ) __NOTHROW__;
		virtual pIUTF8String_base APICALL assign( pcIUTF8String_base str, sizet srcPos, sizet count, pcIError_base & error ) __NOTHROW__;
		virtual pIUTF8String_base APICALL append( const char * buffer, sizet count, pcIError_base & error ) __NOTHROW__;
		virtual pIUTF8String_base APICALL append( pcIUTF8String_base str, sizet srcPos, sizet count, pcIError_base & error ) __NOTHROW__;
		virtual pIUTF8String_base APICALL insert( sizet pos, const char * buf, sizet count, pcIError_base & error ) __NOTHROW__;
		virtual pIUTF8String_base APICALL insert( sizet pos, pcIUTF8String_base src, sizet srcPos, sizet count, pcIError_base & error ) __NOTHROW__;
		virtual pIUTF8String_base APICALL erase( sizet pos, sizet count, pcIError_base & error ) __NOTHROW__;
		virtual void APICALL resize( sizet n, pcIError_base & error ) __NOTHROW__;
		virtual pIUTF8String_base APICALL replace( sizet pos, sizet count, const char * buf, sizet srcCount, pcIError_base & error ) __NOTHROW__;
		virtual pIUTF8String_base APICALL replace( sizet pos, sizet count, pcIUTF8String_base src, sizet srcPos, sizet srcCount, pcIError_base & error ) __NOTHROW__;
		virtual sizet APICALL copy( char * buf, sizet len, sizet pos, pcIError_base & error ) const __NOTHROW__;
		virtual sizet APICALL find( const char * buf, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__;
		virtual sizet APICALL find( pcIUTF8String_base src, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__;
		virtual sizet APICALL rfind( const char * buf, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__;
		virtual sizet APICALL rfind( pcIUTF8String_base src, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__;
		virtual int32 APICALL compare( sizet pos, sizet len, const char * buf, sizet count, pcIError_base & error ) const __NOTHROW__;
		virtual int32 APICALL compare( sizet pos, sizet len, pcIUTF8String_base str, sizet strPos, sizet strLen, pcIError_base & error ) const __NOTHROW__;
		virtual pIUTF8String_base APICALL substr( sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__;
		virtual uint32 APICALL empty( pcIError_base & error ) const __NOTHROW__;

		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

		// static factory functions

		//!
		//! Creates an IUTF8String object whose initial contents are copied from a char buffer.
		//! \param[in] buf pointer to a constant char buffer containing content. It can be null
		//! terminated or not. NULL pointer will be treated as empty string.
		//! \param[in] count a value of #AdobeXMPCommon::sizet indicating the length in case buf is not null
		//! terminated. In case buf is null terminated it can be set to npos.
		//! \return a shared pointer to a newly created #AdobeXMPCommon::IUTF8String object
		//!
		XMP_PRIVATE static spIUTF8String CreateUTF8String( const char * buf = NULL, sizet count = AdobeXMPCommon::npos );

	protected:
		virtual pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__;
		pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );
    
        virtual ~IUTF8String_I() __NOTHROW__ {}

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};

#if XMP_WinBuild
	#pragma warning( pop )
#endif

}

#include "XMPCommon/Utilities/TAllocator.h"
namespace XMP_COMPONENT_INT_NAMESPACE {
	// typedefs for vectors and their corresponding shared pointers.
	typedef std::vector< spIUTF8String, TAllocator< spIUTF8String > >					mmIUTF8StringList;
	typedef std::vector< spcIUTF8String, TAllocator< spcIUTF8String > >					mmcIUTF8StringList;
}

#endif  // __IUTF8String_I_h__
