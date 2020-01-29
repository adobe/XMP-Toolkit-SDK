#ifndef __IPathSegment_I_h__
#define __IPathSegment_I_h__ 1

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
#include "XMPCore/Interfaces/IPathSegment.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject_I.h"

namespace AdobeXMPCore_Int {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! \brief Internal interface that represents an PathSegment.
	//!	\details Provides all the functions to access
	//!
	class IPathSegment_I
		: public virtual IPathSegment
		, public virtual ISharedObject_I
	{
	public:

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }

		using IPathSegment_v1::GetNameSpace;
		using IPathSegment_v1::GetName;
		using IPathSegment_v1::GetType;
		using IPathSegment_v1::GetValue;

		virtual spcIPathSegment APICALL Clone() const = 0;
		virtual pIPathSegment APICALL GetActualIPathSegment() __NOTHROW__ { return this; }
		virtual pIPathSegment_I APICALL GetIPathSegment_I() __NOTHROW__ { return this; }

		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

		// static factory functions

		// Factories to create the specific segments

		//!
		//! Creates a normal property path segment.These are essentially all properties (simple, struct and arrays).
		//! \param[in] nameSpace shared pointer to a const IUTF8String object containing name space URI of the property.
		//! \param[in] name shared pointer to a const IUTF8String object containing local name of the property.
		//! \return a shared pointer to const #IPathSegment.
		//! \attention throws #AdobeXMPCommon::pcIError in case
		//!		- shared pointers are invalid,
		//!		- their content is empty.
		//!
		XMP_PRIVATE static spcIPathSegment CreatePropertyPathSegment( const spcIUTF8String & nameSpace, const spcIUTF8String & name );

		//!
		//! Creates an array index path segment that denotes a specific element of an array.
		//! Such segments do not have an own name and inherits the namespace from the Array property itself.
		//! \param[in] nameSpace shared pointer to a const IUTF8String object containing name space URI of the property.
		//! \param[in] index an object of type #AdobeXMP::sizet containting the index of the array element.
		//! \return a shared pointer to const #IPathSegment.
		//! \attention throws #AdobeXMP::pcIError in case
		//!		- pointers to const char buffers are NULL,
		//!		- their content is empty.
		//!
		//!
		XMP_PRIVATE static spcIPathSegment CreateArrayIndexPathSegment( const spcIUTF8String & nameSpace, sizet index );

		//!
		//! Creates a Qualifier path segment, which behaves like a normal property
		//! \param[in] nameSpace shared pointer to a const IUTF8String object containing name space URI of the property.
		//! \param[in] name shared pointer to a const IUTF8String object containing local name of the property.
		//! \return a shared pointer to const #IPathSegment.
		//! \attention throws #AdobeXMPCommon::pcIError in case
		//!		- shared pointers are invalid,
		//!		- their content is empty.
		//!
		XMP_PRIVATE static spcIPathSegment CreateQualifierPathSegment( const spcIUTF8String & nameSpace, const spcIUTF8String & name );

		//!
		//! Creates a path segment that selects a specific qualifier by its value.
		//! For example a specific language in a alternative array of languages.
		//! \param[in] nameSpace shared pointer to a const IUTF8String object containing name space URI of the property.
		//! \param[in] name shared pointer to a const IUTF8String object containing local name of the property.
		//! \param[in] value shared pointer to a const IUTF8String object containing value of the language (xml:lang)
		//! \return a shared pointer to const #IPathSegment.
		//! \attention throws #AdobeXMPCommon::pcIError in case
		//!		- shared pointers are invalid,
		//!		- their content is empty.
		//!
		XMP_PRIVATE static spcIPathSegment CreateQualifierSelectorPathSegment( const spcIUTF8String & nameSpace, const spcIUTF8String & name, const spcIUTF8String & value );


	protected:
		virtual ~IPathSegment_I() __NOTHROW__ {}
		pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );

		virtual pcIUTF8String_base APICALL getNameSpace( pcIError_base & error ) const __NOTHROW__;
		virtual pcIUTF8String_base APICALL getName( pcIError_base & error ) const __NOTHROW__;
		virtual uint32 APICALL getType( pcIError_base & error ) const __NOTHROW__;
		virtual pcIUTF8String_base APICALL getValue( pcIError_base & error ) const __NOTHROW__;
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
#endif  // __IPathSegment_I_h__
