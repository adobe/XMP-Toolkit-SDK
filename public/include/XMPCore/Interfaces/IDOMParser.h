#ifndef IDOMParser_h__
#define IDOMParser_h__ 1

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
	//! @brief Version 1 of the interface that supports parsing of the XMP Data Model.
	//! @details Provides all functions to parse the buffer as well as to configure the parser.
	//!
	class XMP_PUBLIC IDOMParser_v1
		: public virtual IConfigurable
		, public virtual ISharedObject
		, public virtual IVersionable
	{
	public:

		//!
		//! @brief Indicates various types of operations possible while parsing with some node as the context.
		//!
		typedef enum {
			//! @brief Append all the nodes parsed from the buffer as the child of the context node.
			//! \attention Error is thrown in case
			//!		- Context Node is invalid.
			//!		- Context Node is not array or structure node.
			//!		- Context Node is a structure node but a child node with the same qualified name is already present.
			//!		- Context Node is an array node but the type of any parsed node is not same as that of other existing nodes in the array.
			//!
			kATAppendAsChildren				= 0,

			//! @brief Replaces the children of the context node with nodes parsed from the buffer.
			//! \attention Error is thrown in case
			//!		- Context Node is invalid.
			//!		- Context Node is not array or structure node.
			//!		- Context Node is a structure node but a child node with the same qualified name is not already present.
			//!		- Context Node is an array node but the type of all parsed nodes are not same.
			//!
			kATReplaceChildren				= 1,

			//! @brief Either append all the nodes parsed from the buffer as the child/children of the context node.
			//! \attention Error is thrown in case
			//!		- Context Node is invalid.
			//!		- Context Node is not array or structure node.
			//!		- Context Node is an array node but the type of all parsed nodes are not same.
			//!     - If a structure node is the parsed node, it is appended if it already not present, otherwise it is replaced.
			//!     - If an array node is the parsed node, it is appended if it already not present, otherwise it is removed.
			kATAppendOrReplaceChildren		= 2,

			//! @brief Treats all the parsed nodes as the siblings of the context node and place them before the context node, if possible.
			//! \attention Error is thrown in case
			//!		- Context Node is invalid.
			//!		- parent of the Context Node is not an array node.
			//!		- The type of any parsed nodes is not same as that of other existing nodes in the array.
			kATInsertBefore					= 3,

			//! @brief Treats all the parsed nodes as the siblings of the context node and place them after the context node, if possible.
			//! \attention Error is thrown in case
			//!		- Context Node is invalid.
			//!		- parent of the Context Node is not an array node.
			//!		- The type of any parsed nodes is not same as that of other existing nodes in the array.
			kATInsertAfter					= 4,

			//! @brief Replaces the context node and insert the node parsed from the buffer in its place.
			//! \attention Error is thrown in case
			//!		- type of node returned after parsing in not of type which is compatible with the Context Node.
			kATReplace						= 5,
		} eActionType;

		//!
		//! @brief Parses the buffer contents and creates an XMP DOM node.
		//! \param[in] buffer Pointer to a constant char buffer containing serialized XMP Data Model.
		//! \param[in] bufferLength Number of characters in buffer. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return A shared pointer to an object of \#IMetadata containing all the information parsed from the buffer.
		//!
		virtual spIMetadata APICALL Parse( const char * buffer, sizet bufferLength ) = 0;

		//!
		//! @brief Parse the buffer contents and populate the provided node .
		//! \param[in] buffer Pointer to a constant char buffer containing serialized XMP Data Model.
		//! \param[in] bufferLength Number of characters in buffer. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] actionType Value indicating how the parsed content and context node should be used.
		//! \param[in,out] node The context node to be used base on the actionType.
		//!
		virtual void APICALL ParseWithSpecificAction( const char * buffer, sizet bufferLength, eActionType actionType, spINode & node ) = 0;

		//!
		//! @brief Virtual copy constructor.
		//! @details Creates an exact replica of the object.
		//! \return A shared pointer to an object of \#IDOMParser which is the exact replica of the current serializer.
		//!
		virtual spIDOMParser APICALL Clone() const = 0;

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Returns the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return Either a const or non const pointer to IDOMParser interface.
		//!
		virtual pIDOMParser APICALL GetActualIDOMParser() __NOTHROW__ = 0;
		XMP_PRIVATE pcIDOMParser GetActualIDOMParser() const __NOTHROW__ {
			return const_cast< IDOMParser_v1 * >( this )->GetActualIDOMParser();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Returns the pointer to internal interfaces.
		//! \return Either a const or non const pointer to IDOMParser_I interface.
		//!
		virtual AdobeXMPCore_Int::pIDOMParser_I APICALL GetIDOMParser_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPCore_Int::pcIDOMParser_I GetIDOMParser_I() const __NOTHROW__ {
			return const_cast< IDOMParser_v1 * >( this )->GetIDOMParser_I();
		}
		//!
		//! @}
		
		//!
		//! @{
		//! @brief Converts raw pointer to shared pointer. 
		//!	@details The raw pointer is of version 1 interface where as the returned shared pointer depends on the version client is interested in.
		//!          
		//! \return Shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spIDOMParser MakeShared( pIDOMParser_base ptr );
		XMP_PRIVATE static spcIDOMParser MakeShared( pcIDOMParser_base ptr ) {
			return MakeShared( const_cast< pIDOMParser_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! return The unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIDOMParserID; }

		//!
		//! return The version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

	protected:
		//!
		//! Destructor
		//! 
		virtual ~IDOMParser_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual pIMetadata_base APICALL parse( const char * buffer, sizet bufferLength, pcIError_base & error ) __NOTHROW__ = 0;
		virtual void APICALL parseWithSpecificAction( const char * buffer, sizet bufferLength, uint32 actionType, pINode_base node, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pIDOMParser_base APICALL clone( pcIError_base & error ) const __NOTHROW__ = 0;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif // IDOMParser_h__
