#ifndef IDOMSerializer_h__
#define IDOMSerializer_h__ 1

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
#include "XMPCommon/Interfaces/BaseInterfaces/IConfigurable.h"

namespace AdobeXMPCore {
	//!
	//! @brief Version1 of the interface that represents an object that can serialize an XMP Data Model to a buffer.
	//!        Provides the functions to serialize the XMP Data Model.
	//!
	class XMP_PUBLIC IDOMSerializer_v1
		: public virtual IConfigurable
		, public virtual ISharedObject
		, public virtual IVersionable
	{
	public:

		//!
		//! @brief Serializes the given XMP Node into an IUTF8String object.
		//! \param[in] node An object of type \#INode which needs to be serialized.
		//! \param[in] nameSpacePrefixMap An object of type \#INameSpacePrefixMap which contains preferred prefixes for namespaces.
		//! \return An object of \#AdobeXMPCommon::IUTF8String type containing the serialized form of the node.
		//!
		virtual spIUTF8String APICALL Serialize( const spINode & node, const spcINameSpacePrefixMap & nameSpacePrefixMap = spcINameSpacePrefixMap() ) = 0;

		//!
		//! @brief Virtual copy constructor.
		//! Creates an exact replica of the object.
		//! \return A shared pointer to an object of \#IDOMSerializer which is the exact replica of the current serializer.
		//!
		virtual spIDOMSerializer APICALL Clone() const = 0;
		
		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Returns the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return Either a const or non const pointer to IDOMSerializer interface.
		//!
		virtual pIDOMSerializer APICALL GetActualIDOMSerializer() __NOTHROW__ = 0;
		XMP_PRIVATE pcIDOMSerializer GetActualIDOMSerializer() const __NOTHROW__ {
			return const_cast< IDOMSerializer_v1 * >( this )->GetActualIDOMSerializer();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Returns the pointer to internal interfaces.
		//! \return Either a const or non const pointer to IDOMSerializer_I interface.
		//!
		virtual AdobeXMPCore_Int::pIDOMSerializer_I APICALL GetIDOMSerializer_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPCore_Int::pcIDOMSerializer_I GetIDOMSerializer_I() const __NOTHROW__ {
			return const_cast< IDOMSerializer_v1 * >( this )->GetIDOMSerializer_I();
		}
		//!
		//! @}
		
		//!
		//! @{
		//! @brief Converts raw pointer to shared pointer. The raw pointer is of version 1 interface
		//! where as the returned shared pointer depends on the version client is interested in.
		//! \return Shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spIDOMSerializer MakeShared( pIDOMSerializer_base ptr );
		XMP_PRIVATE static spcIDOMSerializer MakeShared( pcIDOMSerializer_base ptr ) {
			return MakeShared( const_cast< pIDOMSerializer_base >( ptr ) );
		}

		//!
		//! @}

		//!
		//! @brief Returns the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIDOMSerializerID; }

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
		virtual ~IDOMSerializer_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual pIUTF8String_base APICALL serialize( pINode_base node, pcINameSpacePrefixMap_base map, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pIDOMSerializer_base APICALL clone( pcIError_base & error ) const __NOTHROW__ = 0;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif // IDOMSerializer_h__

