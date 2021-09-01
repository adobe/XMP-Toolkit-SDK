#ifndef __ISimpleNode_h__
#define __ISimpleNode_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCore/Interfaces/INode.h"

namespace AdobeXMPCore {

	//!
	//! \brief Version1 of the interface that represents a Simple Property Node of XMP DOM.
	//! \details Provides all the functions to get and set various properties of the simple node.
	//! \attention Support multi threading through locks but can be enabled/disabled by the client. By default
	//!            every object created does not support multi-threading.
	//!
	class XMP_PUBLIC ISimpleNode_v1
		: public virtual INode_v1
	{
	public:

		//!
		//! @brief Gets the value of the simple property node.
		//! \return A shared pointer to const AdobeXMPCommon::IUTF8String object containing value string
		//!         of the simple property node.
		//!
		virtual spcIUTF8String APICALL GetValue() const = 0;

		//!
		//! @brief Changes the value string of the simple property node.
		//! \param[in] value Pointer to a constant char buffer containing value of the simple node.
		//! \param[in] valueLength Number of characters in value. In case name is null terminated set it to AdobeXMPCommon::npos.
		//! \note In case the value is null pointer or its contents are empty than the value is set to empty string.
		//!
		virtual void APICALL SetValue( const char * value, sizet valueLength ) = 0;

		//!
		//! @brief Indicates whether the simple property node is of URI type.
		//! \return A bool value; true in case the simple node is of URI type, false otherwise.
		//!
		virtual bool APICALL IsURIType() const = 0;

		//!
		//! @brief Controls whether the type of simple property node should be of IsURI type or not.
		//! \param[in] isURI A bool value controlling the IsURI type of the simple property node 
		//!
		virtual void APICALL SetURIType( bool isURI ) = 0;

		// Factories to create the simple node

		//!
		//! @brief Creates a simple property node which is not part of any metadata document.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the simple node.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the simple node.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to AdobeXMPCommon::npos.
		//! \param[in] value Pointer to a constant char buffer containing value of the simple node.
		//! \param[in] valueLength Number of characters in value. In case name is null terminated set it to AdobeXMPCommon::npos.
		//! \return A shared pointer to a AdobeXMPCore::ISimpleNode object.
		//! \attention Error is thrown in case
		//!		- nameSpace or name are NULL pointers, or
		//!		- their contents are empty.
		//! \note In case the value is a null pointer or its contents are empty than the value is set to empty string.
		//!
		XMP_PRIVATE static spISimpleNode CreateSimpleNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength,
			const char * value = NULL, sizet valueLength = AdobeXMPCommon::npos );

		
		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Returns the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return Either a const or non const pointer to ISimpleNode interface.
		//!
		virtual pISimpleNode APICALL GetActualISimpleNode() __NOTHROW__ = 0;
		XMP_PRIVATE pcISimpleNode GetActualISimpleNode() const __NOTHROW__ {
			return const_cast< ISimpleNode_v1 * >( this )->GetActualISimpleNode();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Returns the pointer to internal interfaces.
		//! \return Either a const or non const pointer to ISimpleNode_I interface.
		//!
		virtual AdobeXMPCore_Int::pISimpleNode_I APICALL GetISimpleNode_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPCore_Int::pcISimpleNode_I GetISimpleNode_I() const __NOTHROW__ {
			return const_cast< ISimpleNode_v1 * >( this )->GetISimpleNode_I();
		}
		//!
		//! @}
		
		//!
		//! @{
		//! @brief Converts raw pointer to shared pointer. The raw pointer is of version 1 interface
		//!        where as the returned shared pointer depends on the version client is interested in.
		//! \return Shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spISimpleNode MakeShared( pISimpleNode_base ptr );
		XMP_PRIVATE static spcISimpleNode MakeShared( pcISimpleNode_base ptr ) {
			return MakeShared( const_cast< pISimpleNode_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief Returns the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kISimpleNodeID; }

		//!
		//! @brief Returns the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

	protected:
		//!
		//! Destructor
		//! 
		virtual ~ISimpleNode_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual pcIUTF8String_base APICALL getValue( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual void APICALL setValue( const char * value, sizet valueLength, pcIError_base & error ) __NOTHROW__ = 0;
		virtual uint32 APICALL isURIType( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual void APICALL setURIType( uint32 isURI, pcIError_base & error ) __NOTHROW__ = 0;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif  // __ISimpleNode_h__
