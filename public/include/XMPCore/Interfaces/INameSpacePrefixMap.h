#ifndef INameSpacePrefixMap_h__
#define INameSpacePrefixMap_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IThreadSafe.h"

namespace AdobeXMPCore {

	//!
	//! \brief Version1 of the interface that represents map where each entry consists of prefix string
	//!  as the key and corresponding nameSpace string as its value.
	//! \details Provides all the functions to get/set the entries inside the map.
	//! \attention Supports Multi-threading at object level through locks.
	//!
	class XMP_PUBLIC INameSpacePrefixMap_v1
		: public virtual ISharedObject
		, public virtual IVersionable
		, public virtual IThreadSafe
	{
	public:

		//!
		//! @brief Adds a new pair of prefix string and its corresponding nameSpace string or replace an existing entry.
		//! \param[in] prefix Pointer to a constant char buffer containing prefix string.
		//! \param[in] prefixLength Number of characters in prefix. In case prefix is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] nameSpace Pointer to a constant char buffer containing nameSpace string.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return A bool type object containing true in case operation was successful, false otherwise.
		//! \note Raises warning in case of
		//!		- prefix or nameSpace are null pointers
		//!		- prefixLength or nameSpaceLength is 0.
		//!
		virtual bool APICALL Insert( const char * prefix, sizet prefixLength, const char * nameSpace, sizet nameSpaceLength ) = 0;

		//!
		//! @brief Finds the prefix string in the map and removes an entry corresponding to it in the map.
		//! \param[in] prefix Pointer to a const char buffer containing prefix string.
		//! \param[in] prefixLength Number of characters in prefix. In case prefix is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return A bool type object containing true in case entry was found and then deleted from the map, false otherwise.
		//! \note Raises warning in case of
		//!		- prefix is null pointer, or
		//!		- prefixLength is 0.
		//! \attention Throws AdobeXMPCommon::pcIError in case of failure in removing or searching process.
		//!
		virtual bool APICALL RemovePrefix( const char * prefix, sizet prefixLength ) = 0;

		//!
		//! @brief Finds the nameSpace string in the map and removes an entry corresponding to it in the map.
		//! \param[in] nameSpace Pointer to a constant char buffer containing nameSpace string.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return A bool type object containing true in case entry was found and then deleted from the map, false otherwise.
		//! \note Raises warning in case of
		//!		- nameSpace is null pointer, or
		//!		- nameSpaceLength is 0.
		//! \attention Throws AdobeXMPCommon::pcIError in case of failure in removing or searching process.
		//!
		virtual bool APICALL RemoveNameSpace( const char * nameSpace, sizet nameSpaceLength ) = 0;

		//!
		//! @brief Checks for the existence of a particular prefix in the map.
		//! \param[in] prefix Pointer to a const char buffer containing prefix string.
		//! \param[in] prefixLength Number of characters in prefix. In case prefix is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return A bool type object containing true in case there is an entry present corresponding to the prefix string, otherwise false.
		//! \note Raises warning in case of
		//!		- prefix is null pointer, or
		//!		- prefixLength is 0.
		//!
		virtual bool APICALL IsPrefixPresent( const char * prefix, sizet prefixLength ) const = 0;

		//!
		//! @brief Checks for the existence of a particular nameSpace in the map.
		//! \param[in] nameSpace Pointer to a constant char buffer containing nameSpace string.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return A bool type object containing true in case there is an entry present corresponding to the nameSpace string, otherwise false.
		//! \note Raises warning in case of
		//!		- nameSpace is null pointer, or
		//!		- nameSpaceLength is 0.
		//!
		virtual bool APICALL IsNameSpacePresent( const char * nameSpace, sizet nameSpaceLength ) const = 0;

		//!
		//! @brief Gets the nameSpace string corresponding to the prefix string.
		//! \param[in] prefix Pointer to a const char buffer containing prefix string.
		//! \param[in] prefixLength Number of characters in prefix. In case prefix is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return A shared pointer to const \#AdobeXMPCommon::IUTF8String object containing nameSpace string corresponding to
		//!         prefix string if a mapping exists, otherwise invalid shared pointer is returned.
		//! \note Raises warning in case of
		//!		- prefix is null pointer, or
		//!		- prefixLength is 0.
		//!
		virtual spcIUTF8String APICALL GetNameSpace( const char * prefix, sizet prefixLength ) const = 0;

		//!
		//! @brief Get the prefix string corresponding to the nameSpace string.
		//! \param[in] nameSpace Pointer to a constant char buffer containing nameSpace string.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return A shared pointer to const \#AdobeXMPCommon::IUTF8String object containing prefix string corresponding to
		//!  nameSpace string if a mapping exists, otherwise invalid shared pointer is returned.
		//! \note Raises warning in case of
		//!		- nameSpace is null pointer, or
		//!		- nameSpaceLength is 0.
		//!
		virtual spcIUTF8String APICALL GetPrefix( const char * nameSpace, sizet nameSpaceLength ) const = 0;

		//!
		//! @brief To get the total number of entries in the map.
		//! \return An object of type \#AdobeXMPCommon::sizet containing the count of entries in the map.
		//!
		virtual sizet APICALL Size() const __NOTHROW__ = 0;

		//!
		//! @brief To check whether the map is empty or not.
		//! \return True in case map is empty; false otherwise.
		//!
		bool IsEmpty() const __NOTHROW__;

		//!
		//! @brief Clear all the entries in the map.
		//!
		virtual void APICALL Clear() __NOTHROW__ = 0;

		//!
		//! \brief Virtual Copy Constructor.
		//! \details Makes an another object which is exact replica of the existing object.
		// \return A shared pointer to INameSpacePrefixMap which is exact replica of the current object.
		//!
		virtual spINameSpacePrefixMap APICALL Clone() const = 0;

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION

		//!
		//! @{
		//! @brief Returns the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return Either a const or non const pointer to INameSpacePrefixMap interface.
		//!
		virtual pINameSpacePrefixMap APICALL GetActualINameSpacePrefixMap() __NOTHROW__ = 0;
		XMP_PRIVATE pcINameSpacePrefixMap GetActualINameSpacePrefixMap() const __NOTHROW__ {
			return const_cast< INameSpacePrefixMap_v1 * >( this )->GetActualINameSpacePrefixMap();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Returns the pointer to internal interfaces.
		//! \return Either a const or non const pointer to INameSpacePrefixMap_I interface.
		//!
		virtual AdobeXMPCore_Int::pINameSpacePrefixMap_I APICALL GetINameSpacePrefixMap_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPCore_Int::pcINameSpacePrefixMap_I GetINameSpacePrefixMap_I() const __NOTHROW__ {
			return const_cast< INameSpacePrefixMap_v1 * >( this )->GetINameSpacePrefixMap_I();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Converts raw pointer to shared pointer. The raw pointer is of version 1 interface
		//!        where as the returned shared pointer depends on the version client is interested in.
		//! \return shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spINameSpacePrefixMap MakeShared( pINameSpacePrefixMap_base ptr );
		XMP_PRIVATE static spcINameSpacePrefixMap MakeShared( pcINameSpacePrefixMap_base ptr ) {
			return MakeShared( const_cast< pINameSpacePrefixMap_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief Returns the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kINameSpacePrefixMapID; }

		//!
		//! @brief returns the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		
		//! \endcond


		// static factory functions

		//!
		//! @brief Provides the default mapping of prefix string and nameSpace strings used by XMPCore.
		//! \return A shared pointer to const INameSpacePrefixMap object containing all the mappings used
		//!         as default by the XMPCore.
		//!
		XMP_PRIVATE static spcINameSpacePrefixMap GetDefaultNameSpacePrefixMap();

		//!
		//! @brief Creates an empty namespace - prefix map and returns it to the client as a shared pointer.
		//! \return A shared pointer to an empty INameSpacePrefixMap object.
		//!
		XMP_PRIVATE static spINameSpacePrefixMap CreateNameSpacePrefixMap();

	protected:
		//!
		//! Destructor
		//! 
		virtual ~INameSpacePrefixMap_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		const uint32 kPrefixIsParameter = 0;
		const uint32 kNameSpaceIsParameter = 1;

		virtual uint32 APICALL insert( const char * prefix, sizet prefixLength, const char * nameSpace, sizet nameSpaceLength, pcIError_base & error ) __NOTHROW__ = 0;
		virtual uint32 APICALL remove( uint32 keyType, const char * key, sizet keyLength, pcIError_base & error ) __NOTHROW__ = 0;
		virtual uint32 APICALL isPresent( uint32 keyType, const char * key, sizet keyLength, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pcIUTF8String_base APICALL get( uint32 keyType, const char * key, sizet keyLength, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pINameSpacePrefixMap_base APICALL clone( pcIError_base & error ) const __NOTHROW__ = 0;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif // INameSpacePrefixMap_h__
