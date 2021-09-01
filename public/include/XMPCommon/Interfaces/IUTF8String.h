#ifndef __IUTF8String_h__
#define __IUTF8String_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"

namespace AdobeXMPCommon {
	using AdobeXMPCommon::npos;

	//!
	//! @brief Version1 of the interface that represents an UTF8String.
	//! @details Provides all the functions to access properties of the string object, appends or assigns content
	//! to the existing string objects and clones existing string objects.
	//!
	class XMP_PUBLIC IUTF8String_v1
		: public virtual ISharedObject
		, public virtual IVersionable
	{
	public:
		//!
		//! @brief Appends more content into the existing string object through a pointer to char buffer.
		//! \param[in] buf pointer to a constant char buffer containing new content. It can be null
		//! terminated or not. NULL pointer will be treated as empty string.
		//! \param[in] count A value of \#AdobeXMPCommon::sizet indicating the length in case buf is not null
		//!  terminated. In case buf is null terminated it can be set to npos.
		//! \return The shared pointer to itself of type \#AdobeXMPCommon::IUTF8String.
		//!
		virtual spIUTF8String APICALL append( const char * buf, sizet count ) = 0;

		//!
		//! @brief Appends the contents of another string into the existing string.
		//! \param[in] src Shared pointer to const \#AdobeXMPCommon::IUTF8String whose contents will be
		//! appended to existing content in the object. Invalid shared pointer will be treated as empty string.
		//! \param[in] srcPos A value of \#AdobeXMPCommon::sizet indicating the position of the first character
		//! in src that is inserted into the object as a substring.
		//! \param[in] count A value of \#AdobeXMPCommon::sizet indicating the length of the substring to be copied
		//! (if the string is shorter, as many characters as possible are copied). A value of npos indicates all
		//! characters until the end of src.
		//! \return The shared pointer to itself of type \#AdobeXMPCommon::IUTF8String.
		//! \attention Error is thrown in case
		//!		- srcPos is greater than length of src.
		//!
		virtual spIUTF8String APICALL append( const spcIUTF8String & src, sizet srcPos = 0, sizet count = npos ) = 0;

		//!
		//! @brief Overwrites new string content into the existing string object through a pointer to char buffer.
		//! \param[in] buf pointer to a constant char buffer containing new content. It can be null
		//!  terminated or not. NULL pointer will be treated as empty string.
		//! \param[in] count A value of \#AdobeXMPCommon::sizet indicating the length in case buf is not null
		//! terminated. In case buf is null terminated it can be set to npos.
		//! \return The shared pointer to itself of type \#AdobeXMPCommon::IUTF8String.
		//!
		virtual spIUTF8String APICALL assign( const char * buf, sizet count ) = 0;

		//!
		//! @brief Overwrites the contents with contents of another string.
		//! \param[in] src shared pointer to const \#AdobeXMPCommon::IUTF8String whose contents will
		//!  overwrite existing content in the object. Invalid shared pointer will be treated as empty string.
		//! \param[in] srcPos A value of \#AdobeXMPCommon::sizet indicating the position of the first character
		//!  in src that is inserted into the object as a substring.
		//! \param[in] count A value of \#AdobeXMPCommon::sizet indicating the length of the substring to be copied.
		//!  A value of npos indicates all characters until the end of src. If this is greater than the available
		//!  characters in the substring then copying is limited to the number of available characters.
		//! \return The shared pointer to itself of type \#AdobeXMPCommon::IUTF8String.
		//! \attention Error is thrown in case
		//!		- srcPos is greater than length of src.
		//!
		virtual spIUTF8String APICALL assign( const spcIUTF8String & src, sizet srcPos = 0, sizet count = npos ) = 0;

		//!
		//! @brief Inserts additional characters into the string right before the character indicated by pos.
		//! \param[in] pos Insertion point: The new contents are inserted before the character at position pos.
		//! \param[in] buf pointer to a constant char buffer containing new content. It can be null
		//!  terminated or not. NULL pointer will be treated as empty string.
		//! \param[in] count A value of \#AdobeXMPCommon::sizet indicating the length in case buf is not null
		//!  terminated. In case buf is null terminated it can be set to npos.
		//! \return The shared pointer to itself of type \#AdobeXMPCommon::IUTF8String.
		//! \\attention Error is thrown in case
		//!		- pos is greater than the object's length.
		//!
		virtual spIUTF8String APICALL insert( sizet pos, const char * buf, sizet count ) = 0;

		//!
		//! @brief Inserts additional characters into the string right before the character indicated by pos.
		//! \param[in] pos Insertion point: The new contents are inserted before the character at position pos.
		//! \param[in] src shared pointer to const \#AdobeXMPCommon::IUTF8String whose contents will
		//!  be copied and pushed into the object. Invalid shared pointer will be treated as empty string.
		//! \param[in] srcPos A value of \#AdobeXMPCommon::sizet indicating the position of the first character
		//!  in src that is inserted into the object as a substring.
		//! \param[in] count A value of \#AdobeXMPCommon::sizet indicating the length of the substring to be copied
		//! (if the string is shorter, as many characters as possible are copied). A value of npos indicates all
		//!  characters until the end of src. If this is greater than the available characters in the substring
		//!  then copying is limited to the number of available characters.
		//! \return The shared pointer to itself of type \#AdobeXMPCommon::IUTF8String.
		//! \attention Error is thrown in case
		//!		- srcPos is greater than length of src.
		//!		- pos is greater than the object's length.
		//!
		virtual spIUTF8String APICALL insert( sizet pos, const spcIUTF8String & src, sizet srcPos = 0, sizet count = npos ) = 0;

		//!
		//! @brief Erases part of the string, reducing its length.
		//! \param[in] pos Position of the first character to be erased.
		//!  If this is greater than the string length, nothing is erased.
		//! \param[in] count Number of characters to erase (if the string is shorter, as many characters as
		//!  possible are erased). A value of npos indicates all characters until the end of the string.
		//! \return The shared pointer to itself of type \#AdobeXMPCommon::IUTF8String.
		//! \attention Error is thrown in case
		//!		- pos is greater than the object's length.
		//!
		virtual spIUTF8String APICALL erase( sizet pos = 0, sizet count = npos ) = 0;

		//!
		//! @brief Resizes the string to the length of n characters.
		//! /param[in] n New string length, expressed in number of characters.
		//! \note  If n is smaller than the current string length, the current value is shortened
		//! to its first size character, removing the characters beyond the nth. If n is greater than the
		//! current string length, the current content is extended by inserting at the end as many NULL
		//! characters as needed to reach a size of n.
		//! \attention Error is thrown in case
		//!		- n is greater than max_size
		//!		- allocation fails
		//!
		virtual void APICALL resize( sizet n ) = 0 ;

		//!
		//! @brief Replace portion of string.
		//! \param[in] pos Position of the first character to be replaced.
		//! \param[in] count Number of characters to replace (if the string is shorter, as many characters as possible
		//!  are replaced). A value of npos indicates all characters until the end of the string.
		//! \param[in] buf pointer to a constant char buffer containing new content. It can be null
		//!  terminated or not. NULL pointer will be treated as empty string.
		//! \param[in] srcCount A value of \#AdobeXMPCommon::sizet indicating the length in case buf is not null
		//!  terminated. In case buf is null terminated it can be set to npos.
		//! \return The shared pointer to itself of type \#AdobeXMPCommon::IUTF8String.
		//! \attention Error is thrown in case
		//!		- pos is greater than the object's length.
		//!
		virtual spIUTF8String APICALL replace( sizet pos, sizet count, const char * buf, sizet srcCount ) = 0;

		//!
		//! @brief Replace portion of string.
		//! \param[in] pos Position of the first character to be replaced.
		//! \param[in] count Number of characters to replace (if the string is shorter, as many characters as possible
		//!  are replaced). A value of npos indicates all characters until the end of the string.
		//! \param[in] src Shared pointer to const \#AdobeXMPCommon::IUTF8String whose contents will
		//!  be copied and pushed into the object. Invalid shared pointer will be treated as empty string.
		//! \param[in] srcPos Position of the first character in str that is copied to the object as replacement.
		//! \param[in] srcCount Length of the substring to be copied (if the string is shorter, as many characters
		//!  as possible are copied). A value of npos indicates all characters until the end of str.
		//! \return The shared pointer to itself of type \#AdobeXMPCommon::IUTF8String.
		//! \attention Error is thrown in case
		//!		- srcPos is greater than length of src.
		//!		- pos is greater than the object's length.
		//!
		virtual spIUTF8String APICALL replace( sizet pos, sizet count, const spcIUTF8String & src, sizet srcPos = 0, sizet srcCount = npos ) = 0;

		//!
		//! @brief Copy sequence of characters from string.
		//! @details Copies a substring of the current value of the string object into the array. This substring
		//! contains the len characters that start at position pos.
		//! \param[in,out] buf Pointer to an array of characters. The array shall contain enough storage for the copied
		//! characters.
		//! \param[in] len Number of characters to copy (if the string is shorter, as many characters as possible are
		//! copied).
		//! \param[in] pos Position of the first character to be copied.
		//! \return The number of characters copied to the array. This may be equal to count or to size() - pos.
		//! \note The function does not append a null character at the end of the copied content.
		//! \attention Error is thrown in case
		//!		- pos is greater than the object's length.
		//!
		virtual sizet APICALL copy( char * buf, sizet len, sizet pos = 0 ) const = 0;

		//!
		//! @brief Find content in string.
		//! @details Searches the string for the first occurrence of the sequence specified by its arguments. When pos
		//! is specified, the search only includes characters at or after position pos, ignoring any possible
		//! occurrences that include characters before pos.
		//! \param[in] buf pointer to a constant char buffer containing content to be matched. It can be null
		//! terminated or not. NULL pointer will be treated as empty string.
		//! \param[in] pos Position of the first character in the string to be considered in the search.
		//! If this is greater than the string length, the function never finds matches.
		//! \return The position of the first character of the first match. If no matches were found, the function
		//! returns npos.
		//!
		sizet find( const char * buf, sizet pos = 0 ) const {
			return find( buf, pos, npos );
		}

		//!
		//! @brief Find content in string.
		//! @details Searches the string for the first occurrence of the sequence specified by its arguments. When pos
		//! is specified, the search only includes characters at or after position pos, ignoring any possible
		//! occurrences that include characters before pos.
		//! \param[in] buf pointer to a constant char buffer containing content to be matched. It can be null
		//!  terminated or not. NULL pointer will be treated as empty string.
		//! \param[in] pos Position of the first character in the string to be considered in the search.
		//!  If this is greater than the string length, the function never finds matches.
		//! \param[in] count Length of sequence of characters to match.
		//! \return The position of the first character of the first match. If no matches were found, the function
		//! returns npos.
		//!
		virtual sizet APICALL find( const char * buf, sizet pos, sizet count ) const = 0;


		//
		//! @brief Find content in string.
		//! @details Searches the string for the first occurrence of the sequence specified by its arguments. When pos
		//! is specified, the search only includes characters at or after position pos, ignoring any possible
		//! occurrences that include characters before pos.
		//! \param[in] src shared pointer to const \#AdobeXMPCommon::IUTF8String containing content to be matched. Invalid
		//! shared pointer will be treated as empty string.
		//! \param[in] pos Position of the first character in the string to be considered in the search.
		//!  If this is greater than the string length, the function never finds matches.
		//! \param[in] count Length of sequence of characters to match.
		//! \return The position of the first character of the first match. If no matches were found, the function
		//! returns npos.
		//!
		virtual sizet APICALL find( const spcIUTF8String & src, sizet pos = 0, sizet count = npos ) const = 0;

		//!
		// @{
		//! @brief Find last occurrence of content in string.
		//! @details Searches the string for the last occurrence of the sequence specified by its arguments. When pos
		//! is specified, the search only includes sequences of characters that begin at or before position pos,
		//! ignoring any possible match beginning after pos.
		//! \param[in] buf pointer to a constant char buffer containing content to be matched. It can be null
		//! terminated or not. NULL pointer will be treated as empty string.
		//! \param[in] pos Position of the last character in the string to be considered as the beginning of a match.
		//!  Any value greater or equal than the string length (including npos) means that the entire string is
		//!  searched.
		//! \return The position of the fist character of the last match. If no matches were found, the function
		//! returns npos.
		//!
		sizet rfind( const char * buf, sizet pos = npos ) const {
			return rfind( buf, pos, npos );
		}
		virtual sizet APICALL rfind( const char * buf, sizet pos, sizet count ) const = 0;
		//@}
		//!

		//!
		//! @brief Find last occurrence of content in string.
		//! @details Searches the string for the last occurrence of the sequence specified by its arguments. When pos
		//! is specified, the search only includes sequences of characters that begin at or before position pos,
		//! ignoring any possible match beginning after pos.
		//! \param[in] src shared pointer to const \#AdobeXMPCommon::IUTF8String containing content to be matched. Invalid
		//! shared pointer will be treated as empty string.
		//! \param[in] pos Position of the last character in the string to be considered as the beginning of a match.
		//!  Any value greater or equal than the string length (including npos) means that the entire string is
		//!  searched.
		//! \param[in] count Length of sequence of characters to match.
		//! \return The position of the fist character of the last match. If no matches were found, the function
		//! returns npos.
		//!
		virtual sizet APICALL rfind( const spcIUTF8String & src, sizet pos = npos, sizet count = npos ) const = 0;

		//!
		//! @{
		//! @brief Compare strings.
		//! @details Compares the value of the string object (or a substring) to the sequence of characters specified by its
		//! arguments.
		//! \param[in] buf pointer to a constant char buffer containing content to be compared. It can be null
		//!  terminated or not. NULL pointer will be treated as empty string.
		//!
		//! \return Returns a signed integral indicating the relation between the strings
		//! | value | relation between compared string and comparing string |
		//! | :---: | :-----------------------------------------------------|
		//! | 0     | They compare equal                                    |
		//! | <0    | Either the value of the first character that does not match is lower in the compared string, or all compared characters match but the compared string is shorter. |
		//! | >0    | Either the value of the first character that does not match is greater in the compared string, or all compared characters match but the compared string is longer. |
		//! \attention Error is thrown in case
		//!		- pos is greater than the object's length.
		//!
		int32 compare( const char * buf ) const {
			return compare( 0, size(), buf, npos );
		}
		//! @brief Compare strings.
		//! @details Compares the value of the string object (or a substring) to the sequence of characters specified by its
		//! arguments.
		//! \param[in] buf pointer to a constant char buffer containing content to be compared. It can be null
		//! terminated or not. NULL pointer will be treated as empty string.
		//! \param[in] pos Position of the first character in the compared string. If this is greater than the string
		//!  length, it is treated as empty string.
		//! \param[in] len Length of compared string (if the string is shorter, as many characters as possible).
		//!  A value of npos indicates all characters until the end of the string.
		//!
		//! \return Returns a signed integral indicating the relation between the strings
		//! | value | relation between compared string and comparing string |
		//! | :---: | :-----------------------------------------------------|
		//! | 0     | They compare equal                                    |
		//! | <0    | Either the value of the first character that does not match is lower in the compared string, or all compared characters match but the compared string is shorter. |
		//! | >0    | Either the value of the first character that does not match is greater in the compared string, or all compared characters match but the compared string is longer. |
		//! \attention Error is thrown in case
		//!		- pos is greater than the object's length.
		//!
		int32 compare( sizet pos, sizet len, const char * buf ) const {
			return compare( pos, len, buf, npos );
		}
		//!@brief Compare strings.
		//! @details Compares the value of the string object (or a substring) to the sequence of characters specified by its
		//! arguments.
		//! \param[in] buf pointer to a constant char buffer containing content to be compared. It can be null
		//!  terminated or not. NULL pointer will be treated as empty string.
		//! \param[in] pos Position of the first character in the compared string. If this is greater than the string
		//! length, it is treated as empty string.
		//! \param[in] len Length of compared string (if the string is shorter, as many characters as possible).
		//!  A value of npos indicates all characters until the end of the string.
		//! \param[in] count Number of characters to compare.
		//!
		//! \return Returns a signed integral indicating the relation between the strings
		//! | value | relation between compared string and comparing string |
		//! | :---: | :-----------------------------------------------------|
		//! | 0     | They compare equal                                    |
		//! | <0    | Either the value of the first character that does not match is lower in the compared string, or all compared characters match but the compared string is shorter. |
		//! | >0    | Either the value of the first character that does not match is greater in the compared string, or all compared characters match but the compared string is longer. |
		//! \attention Error is thrown in case
		//!		- pos is greater than the object's length.
		//!
		virtual int32 APICALL compare( sizet pos, sizet len, const char * buf, sizet count ) const = 0;
		

		//!
		//! @brief Compare strings.
		//! @details Compares the value of the string object (or a substring) to the contents of an string or substring object
		//! specified by its arguments.
		//! \param[in] str shared pointer to const \#AdobeXMPCommon::IUTF8String containing content to be compared.
		//!  Invalid shared pointer will be treated as empty string.
		//! \return Returns a signed integral indicating the relation between the strings
		//! | value | relation between compared string and comparing string |
		//! | :---: | :-----------------------------------------------------|
		//! | 0     | They compare equal                                    |
		//! | <0    | Either the value of the first character that does not match is lower in the compared string, or all compared characters match but the compared string is shorter. |
		//! | >0    | Either the value of the first character that does not match is greater in the compared string, or all compared characters match but the compared string is longer. |
		//! \attention Error is thrown in case
		//!		- strPos is greater than length of str.
		//!		- pos is greater than the object's length.
		//!
		int32 compare( const spcIUTF8String & str ) const {
			return compare( 0, size(), str, 0, str->size() );
		}
		//!
		//! @brief Compare strings.
		//! @details Compares the value of the string object (or a substring) to the contents of an string or substring object
		//! specified by its arguments.
		//! \param[in] pos Position of the first character in the compared string.
		//! \param[in] len Length of compared string (if the string is shorter, as many characters as possible).
		//!  A value of npos indicates all characters until the end of the string.
		//! \param[in] str shared pointer to const \#AdobeXMPCommon::IUTF8String containing content to be compared.
		//! Invalid shared pointer will be treated as empty string.
		//! \param[in] strPos Position of the first character in the comparing string.
		//! \param[in] strLen Length of comparing string (if the string is shorter, as many characters as possible).
		//!
		//! \return Returns a signed integral indicating the relation between the strings
		//! | value | relation between compared string and comparing string |
		//! | :---: | :-----------------------------------------------------|
		//! | 0     | They compare equal                                    |
		//! | <0    | Either the value of the first character that does not match is lower in the compared string, or all compared characters match but the compared string is shorter. |
		//! | >0    | Either the value of the first character that does not match is greater in the compared string, or all compared characters match but the compared string is longer. |
		//! \attention Error is thrown in case
		//!		- strPos is greater than length of str.
		//!		- pos is greater than the object's length.
		//!
		virtual int32 APICALL compare( sizet pos, sizet len, const spcIUTF8String & str, sizet strPos = 0, sizet strLen = npos ) const = 0;

		//!
		//! @brief Returns a new string object which contains a sub string of the actual string object.
		//! \param[in] pos Position of the first character to be copied. If this is greater than the string length, then
		//! nothing is copied.
		//! \param[in] count Number of characters to copy (if the string is shorter, as many characters as possible are
		//! copied).
		//! \return A shared pointer to AdobeXMPCommon::IUTF8String which is exact replica of the current object.
		//! \attention Error is thrown in case
		//!		- pos is greater than the object's length.
		//!		- allocation fails
		//!
		virtual spIUTF8String APICALL substr( sizet pos = 0, sizet count = npos ) const = 0;

		//!
		//! @brief Indicates whether the string object is empty or not.
		//! \return A value of type bool; true in case the contents of the string object is empty.
		//!
		virtual bool APICALL empty() const = 0;

		//!
		//! @brief Provides access to the actual location where contents of string are stored.
		//! \return A pointer to a buffer of const chars containing the contents of the string object.
		//!
		virtual const char * APICALL c_str() const __NOTHROW__ = 0;

		//!
		//! @brief Clears the contents of the string object.
		//!
		virtual void APICALL clear() __NOTHROW__ = 0;

		//!
		//! @brief Indicates the number of bytes used by the contents of the string object.
		//! \return An object of type \#AdobeXMPCommon::sizet containing the number of bytes used to store the contents fo the string object.
		//!
		virtual sizet APICALL size() const __NOTHROW__ = 0;

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		//! @{
		//! @brief Returns the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return Either a const or non const pointer to IUTF8String interface.
		//!
		virtual pIUTF8String APICALL GetActualIUTF8String() __NOTHROW__ = 0;
		XMP_PRIVATE pcIUTF8String GetActualIUTF8String() const __NOTHROW__ {
			return const_cast< IUTF8String_v1 * >( this )->GetActualIUTF8String();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Convert raw pointer to shared pointer. The raw pointer is of version 1 interface
		//! where as the returned shared pointer depends on the version client is interested in.
		//! \return Shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spIUTF8String MakeShared( pIUTF8String_base ptr );
		XMP_PRIVATE static spcIUTF8String MakeShared( pcIUTF8String_base ptr ) {
			return MakeShared( const_cast< pIUTF8String_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief Returns the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIUTF8StringID; }

		//!
		//! @brief Returns the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

		// static factory functions

		//!
		//! @brief Creates an empty IUTF8String object.
		//! \param[in] objFactory A pointer to \#AdobeXMPCommon::IObjectFactory object.
		//! \return A shared pointer to an empty IUTF8String object
		//!
		XMP_PRIVATE static spIUTF8String CreateUTF8String( pIObjectFactory objFactory );

		//!
		//! @brief Creates an IUTF8String object whose initial contents are copied from a char buffer.
		//! \param[in] objFactory A pointer to \#AdobeXMPCommon::IObjectFactory object.
		//! \param[in] buf pointer to a constant char buffer containing content. It can be null
		//!  terminated or not. NULL pointer will be treated as empty string.
		//! \param[in] count A value of \#AdobeXMPCommon::sizet indicating the length in case buf is not null
		//! terminated. In case buf is null terminated it can be set to npos.
		//! \return A shared pointer to a newly created \#AdobeXMPCommon::IUTF8String object
		//!
		XMP_PRIVATE static spIUTF8String CreateUTF8String( pIObjectFactory objFactory, const char * buf, sizet count );

	protected:
		//!
		//! Destructor
		//! 
		virtual ~IUTF8String_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual pIUTF8String_base APICALL assign( const char * buffer, sizet count, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pIUTF8String_base APICALL assign( pcIUTF8String_base str, sizet srcPos, sizet count, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pIUTF8String_base APICALL append( const char * buffer, sizet count, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pIUTF8String_base APICALL append( pcIUTF8String_base str, sizet srcPos, sizet count, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pIUTF8String_base APICALL insert( sizet pos, const char * buf, sizet count, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pIUTF8String_base APICALL insert( sizet pos, pcIUTF8String_base src, sizet srcPos, sizet count, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pIUTF8String_base APICALL erase( sizet pos, sizet count, pcIError_base & error ) __NOTHROW__ = 0;
		virtual void APICALL resize( sizet n, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pIUTF8String_base APICALL replace( sizet pos, sizet count, const char * buf, sizet srcCount, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pIUTF8String_base APICALL replace( sizet pos, sizet count, pcIUTF8String_base src, sizet srcPos, sizet srcCount, pcIError_base & error ) __NOTHROW__ = 0;
		virtual sizet APICALL copy( char * buf, sizet len, sizet pos, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual sizet APICALL find( const char * buf, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual sizet APICALL find( pcIUTF8String_base src, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual sizet APICALL rfind( const char * buf, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual sizet APICALL rfind( pcIUTF8String_base src, sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual int32 APICALL compare( sizet pos, sizet len, const char * buf, sizet count, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual int32 APICALL compare( sizet pos, sizet len, pcIUTF8String_base str, sizet strPos, sizet strLen, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pIUTF8String_base APICALL substr( sizet pos, sizet count, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual uint32 APICALL empty( pcIError_base & error ) const __NOTHROW__ = 0;
	
		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond
	};
}

#endif  // __IUTF8String_h__
