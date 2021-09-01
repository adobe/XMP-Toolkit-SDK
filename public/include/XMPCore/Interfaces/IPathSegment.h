#ifndef __IPathSegment_h__
#define __IPathSegment_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"

namespace AdobeXMPCore {

	//! 
	//! @brief Version1 of the interface that represents one segment in a path to a node into the XMP tree.
	//! @details Provides all the functions to access properties of the path segment and factory functions
	//! to create various kinds of path segments.
	//! \attention Not thread safe and not required as only read only access is provided to client.
	//!
	class XMP_PUBLIC IPathSegment_v1
		: public virtual ISharedObject
		, public virtual IVersionable
	{
	public:

		//!
		//! @brief This enumeration represents the types of a path segment.
		//!
		typedef enum {
			//! None type
			kPSTNone							= 0,

			//! Any property that consists of namespace and a localName
			kPSTProperty						= 1,
			
			//! An array index which does not have a namespace or localName itself
			kPSTArrayIndex						= 1 << 1,
			
			//! A qualifier of a property, also consists of namespace and localName
			kPSTQualifier						= 1 << 2,
			
			//! Selects a specific qualifier by its value (e.g. specific language)
			kPSTQualifierSelector				= 1 << 3,

			//! Represents all property types
			kPSTAll								= kAllBits
		} ePathSegmentType;

		//!
		//! @brief Gets the name space of the path segment.
		//! \return A shared pointer to const \#AdobeXMPCommon::IUTF8String object representing namespace of the path segment.
		//!
		virtual spcIUTF8String APICALL GetNameSpace() const = 0;

		//!
		//! @brief Gets the name of the path segment.
		//! \return A shared pointer to const \#AdobeXMPCommon::IUTF8String object containing name of the path segment. In case
		//! path segment has no name space, an invalid shared pointer is returned.
		//!
		virtual spcIUTF8String APICALL GetName() const = 0;

		//!
		//! @brief Gets the type of the path segment.
		//! \return An object of type \#ePathSegmentType representing type of the path segment.
		//!
		virtual ePathSegmentType APICALL GetType() const = 0;

		//!
		//! @brief Gets the index of the array type path segment.
		//! \return An objet of type \#AdobeXMPCommon::sizet object representing index of the array type path segment. In case
		//!  path segment is not of type kPSTArrayIndex, it returns \#AdobeXMPCommon::kMaxSize.
		//!
		virtual sizet APICALL GetIndex() const __NOTHROW__ = 0;

		//!
		//! @brief Gets the value of the qualifier type path segment.
		//! \return A shared pointer to const \#AdobeXMP::IUTF8String object representing value of the qualifier type path segment.
		//! In case path segment is not of type kPSTQualifier an invalid shared pointer is returned.
		//!
		virtual spcIUTF8String APICALL GetValue() const  = 0;

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Returns the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return Either a const or non const pointer to IPathSegment interface.
		//!
		virtual pIPathSegment APICALL GetActualIPathSegment() __NOTHROW__ = 0;
		XMP_PRIVATE pcIPathSegment GetActualIPathSegment() const __NOTHROW__ {
			return const_cast< IPathSegment_v1 * >( this )->GetActualIPathSegment();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Returns the pointer to internal interfaces.
		//! \return Either a const or non const pointer to IPathSegment_I interface.
		//!
		virtual AdobeXMPCore_Int::pIPathSegment_I APICALL GetIPathSegment_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPCore_Int::pcIPathSegment_I GetIPathSegment_I() const __NOTHROW__ {
			return const_cast< IPathSegment_v1 * >( this )->GetIPathSegment_I();
		}
		//!
		//! @}
		
		//!
		//! @{
		//! @brief Converts raw pointer to shared pointer. 
		//! @details The raw pointer is of version 1 interface
		//! where as the returned shared pointer depends on the version client is interested in.
		//! \return Shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spIPathSegment MakeShared( pIPathSegment_base ptr );
		XMP_PRIVATE static spcIPathSegment MakeShared( pcIPathSegment_base ptr ) {
			return MakeShared( const_cast< pIPathSegment_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief Returns the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIPathSegmentID; }

		//!
		//! @brief Returns the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

		// static factory functions

		// Factories to create the specific segments

		//!
		//! @brief Creates a normal property path segment.These are essentially all properties (simple, struct and arrays).
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the property.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the property.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return A shared pointer to const \#IPathSegment.
		//! \attention Throws \#AdobeXMPCommon::pcIError in case
		//!		- pointers to const char buffers are NULL,
		//!		- their content is empty.
		//!
		XMP_PRIVATE static spcIPathSegment CreatePropertyPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength );

		//!
		//! @brief Creates an array index path segment that denotes a specific element of an array.
		//! @details Such segments do not have an own name and inherits the namespace from the Array property itself.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the property.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] index An object of type \#AdobeXMP::sizet containting the index of the array element.
		//! \return A shared pointer to const \#IPathSegment.
		//! \attention Throws \#AdobeXMP::pcIError in case
		//!		- pointers to const char buffers are NULL,
		//!		- their content is empty.
		//!
		//!
		XMP_PRIVATE static spcIPathSegment CreateArrayIndexPathSegment( const char * nameSpace, sizet nameSpaceLength, sizet index );

		//!
		//! @brief Creates a Qualifier path segment, which behaves like a normal property
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the property.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the property.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return A shared pointer to const \#IPathSegment.
		//! \attention Throws \#AdobeXMPCommon::pcIError in case
		//!		- pointers to const char buffers are NULL,
		//!		- their content is empty.
		//!
		XMP_PRIVATE static spcIPathSegment CreateQualifierPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength );

		//!
		//! @brief Creates a path segment that selects a specific qualifier by its value.
		//!        For example a specific language in a alternative array of languages.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the property.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the property.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] value Pointer to a constant char buffer containing value of the language (xml:lang)
		//! \param[in] valueLength Number of characters in value. In case value is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return A shared pointer to const \#IPathSegment.
		//! \attention Throws #AdobeXMPCommon::pcIError in case
		//!		- pointers to const char buffers are NULL,
		//!		- their content is empty.
		//!
		XMP_PRIVATE static spcIPathSegment CreateQualifierSelectorPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name,
			sizet nameLength, const char * value, sizet valueLength );



	protected:
		//!
		//! Destructor
		//! 
		virtual ~IPathSegment_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual pcIUTF8String_base APICALL getNameSpace( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pcIUTF8String_base APICALL getName( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual uint32 APICALL getType( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pcIUTF8String_base APICALL getValue( pcIError_base & error ) const __NOTHROW__ = 0;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif  // __IPathSegment_h__
