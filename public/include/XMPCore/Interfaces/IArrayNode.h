#ifndef __IArrayNode_h__
#define __IArrayNode_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/Interfaces/ICompositeNode.h"

namespace AdobeXMPCore {

	//!
	//! \brief Version1 of the interface that represents an Array Node of XMP DOM.
	//! \details Provides all the functions to get and set various properties of the array node.
	//! \attention Support multi threading through locks but can be enabled/disabled by the client. By default
	//!  every object created does not support multi-threading.
	//! \note The index of the array is 1-based.
	//!
	class XMP_PUBLIC IArrayNode_v1
		: public virtual ICompositeNode_v1
	{
	public:

		//!
		//! @brief Indicates different kinds of array forms possible in XMP i.e, unordered, ordered and alternative.
		//!
		typedef enum {
			//! unknown array form, should be used as invalid value.
			kAFNone									= 0,

			//! Array contains entries which are unordered.
			kAFUnordered							= 1 << 0,

			//! Array contains entries which are ordered.
			kAFOrdered								= 1 << 1,

			//! Array contains entries which are ordered plus default value should be the top one.
			kAFAlternative							= 1 << 2,

			//! Maximum value this enum can hold, should be treated as invalid value
			kAFAll									= kAllBits
		} eArrayForm;

		//!
		//! @brief Get the type of array.
		//! \return a value of type #eArrayForm indicating the type of array.
		//!
		virtual eArrayForm APICALL GetArrayForm() const  = 0;

		//!
		//! @brief Get the type of child nodes.
		//! \return a value of type #eNodeType indicating the type of child nodes array can hold.
		//! \note An empty array will return \#INode_v1::kNTAll indicating that right now it can hold any type of node.
		//!
		virtual eNodeType APICALL GetChildNodeType() const = 0;

		//!
		//! @{
		//! @brief Get the node at the specified index.
		//! \param[in] index an object of type \#sizet indicating the index of the node client who is interested in.
		//! \return A shared pointer to const or non const \#INode object containing node.
		//! \note In case no node exists at the given index an invalid shared pointer is returned.
		//! \note The index of an array is 1-based.
		//!
		virtual spINode APICALL GetNodeAtIndex( sizet index ) = 0;
		XMP_PRIVATE spcINode GetNodeAtIndex( sizet index ) const {
			return const_cast< IArrayNode_v1 * >( this )->GetNodeAtIndex( index );
		}
		//! @}

		//!
		//! @{
		//! @brief Get the node at the specified index as simple node, if possible.
		//! \param[in] index An object of type \#sizet indicating the index of the node client is interested in.
		//! \return A shared pointer to const or non const \#ISimpleNode object containing node.
		//! \note In case no node exists at the given index an invalid shared pointer is returned.
		//! \note The index of an array is 1-based.
		//! \attention Error is thrown in case 
		//!		- a child exists at the given index but is not a simple node.
		//!
		XMP_PRIVATE spcISimpleNode GetSimpleNodeAtIndex( sizet index ) const {
			auto node = GetNodeAtIndex( index );
			if ( node ) return node->ConvertToSimpleNode();
			return spcISimpleNode();
		}

		XMP_PRIVATE spISimpleNode GetSimpleNodeAtIndex( sizet index ) {
			auto node = GetNodeAtIndex( index );
			if ( node ) return node->ConvertToSimpleNode();
			return spISimpleNode();
		}
		//! @}

		//!
		//! @{
		//! @brief Get the node at the specified index as structure node, if possible.
		//! \param[in] index An object of type \#sizet indicating the index of the node client is interested in.
		//! \return A shared pointer to const or non const \#IStructureNode object containing node.
		//! \note In case no node exists at the given index an invalid shared pointer is returned.
		//! \note The index of an array is 1-based.
		//! \attention Error is thrown in case 
		//!		- a child exists at the given index but is not a structure node.
		//!
		XMP_PRIVATE spcIStructureNode GetStructureNodeAtIndex( sizet index ) const {
			auto node = GetNodeAtIndex( index );
			if ( node ) return node->ConvertToStructureNode();
			return spcIStructureNode();
		}

		XMP_PRIVATE spIStructureNode GetStructureNodeAtIndex( sizet index ) {
			auto node = GetNodeAtIndex( index );
			if ( node ) return node->ConvertToStructureNode();
			return spIStructureNode();
		}
		//! @}

		//!
		//! @{
		//! @brief Get the node at the specified index as an array node, if possible.
		//! \param[in] index an object of type \#sizet indicating the index of the node client is interested in.
		//! \return a shared pointer to const or non const \#IArrayNode object containing node.
		//! \note In case no node exists at the given index an invalid shared pointer is returned.
		//! \note The index of an array is 1-based.
		//! \attention Error is thrown in case 
		//!		- a child exists at the given index but is not an array node.
		//!
		XMP_PRIVATE spcIArrayNode GetArrayNodeAtIndex( sizet index ) const {
			auto node = GetNodeAtIndex( index );
			if ( node ) return node->ConvertToArrayNode();
			return spcIArrayNode();
		}

		XMP_PRIVATE spIArrayNode GetArrayNodeAtIndex( sizet index ) {
			auto node = GetNodeAtIndex( index );
			if ( node ) return node->ConvertToArrayNode();
			return spIArrayNode();
		}
		//! @}

		//!
		//! @brief Inserts a given node at the specified index.
		//! \param[in] node Shared pointer to an object of \#INode containing the node to be inserted at the specified index.
		//! \param[in] index An object of type sizet indicating the index where the node should
		//!            be inserted.
		//! \note The index of an array is 1-based.
		//! \attention Error is thrown in following cases:
		//!		-# given node is invalid.
		//!		-# type of given node is not same as other child items of the array node.
		//!		-# given node is already a child of some other node.
		//!		-# given index is less than 1 or greater than current child count + 1.
		//!
		virtual void APICALL InsertNodeAtIndex( const spINode & node, sizet index ) = 0;

		//!
		//! @brief Replaces an existing node with the given node at the specified index.
		//! \param[in] node Shared pointer to an object of \#INode containing the node to be inserted at the specified index.
		//! \param[in] index An object of type \#sizet indicating the index from where the node should be replaced.
		//! \return A shared pointer to the node replaced with the new node.
		//! \note The index of an array is 1-based.
		//! \attention Error is thrown in following cases:
		//!		-# Given node is invalid.
		//!		-# Type of given node is not same as other child items of the array node.
		//!		-# Given node is already a child of some other node.
		//!		-# Given index is less than 1 or greater than current child count.
		//!		-# No node exists at the requested index.
		//!
		virtual spINode APICALL ReplaceNodeAtIndex( const spINode & node, sizet index ) = 0;

		//!
		//! @brief Remove the node at the specified index.
		//! \param[in] index An object of type \#sizet indicating the index from where the node should be removed.
		//! \note The index of an array is 1-based.
		//! \return A shared pointer to \#INode object containing node which is removed from the tree.
		//! \note In case no node exists at the given index an invalid shared pointer is returned.
		//!
		virtual spINode APICALL RemoveNodeAtIndex( sizet index ) = 0;
		
		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Returns the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return Either a const or non const pointer to IArrayNode interface.
		//!
		virtual pIArrayNode APICALL GetActualIArrayNode() __NOTHROW__ = 0;
		XMP_PRIVATE pcIArrayNode GetActualIArrayNode() const __NOTHROW__ {
			return const_cast< IArrayNode_v1 * >( this )->GetActualIArrayNode();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Return the pointer to internal interfaces.
		//! \return either a const or non const pointer to IArrayNode_I interface.
		//!
		virtual AdobeXMPCore_Int::pIArrayNode_I APICALL GetIArrayNode_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPCore_Int::pcIArrayNode_I GetIArrayNode_I() const __NOTHROW__ {
			return const_cast< IArrayNode_v1 * >( this )->GetIArrayNode_I();
		}
		//!
		//! @}
		
		//!
		//! @{
		//! @brief Converts raw pointer to shared pointer. 
		//! @details The raw pointer is of version 1 interface
		//! where as the returned shared pointer depends on the version client is interested in.
		//! \return  Shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spIArrayNode MakeShared( pIArrayNode_base ptr );
		XMP_PRIVATE static spcIArrayNode MakeShared( pcIArrayNode_base ptr ) {
			return MakeShared( const_cast< pIArrayNode_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! return The unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIArrayNodeID; }

		//!
		//! @brief Returns the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

		// Factories to create the array node

		//!
		//! @brief Creates an unordered array node which is not part of any metadata document.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the array node.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated, set it to \#AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the array node.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return A shared pointer to a \#IArrayNode object.
		//! \attention Error is thrown in the following cases:
		//!		-# nameSpace is NULL or its contents are empty.
		//!		-# name is NULL or its contents are empty.
		//!
		XMP_PRIVATE static spIArrayNode CreateUnorderedArrayNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength );

		//!
		//! @brief Creates an ordered array node which is not part of any metadata document.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the array node.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the array node.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return A shared pointer to a \#IArrayNode object.
		//! \attention Error is thrown in the following cases:
		//!		-# nameSpace is NULL or its contents are empty.
		//!		-# name is NULL or its contents are empty.
		//!
		XMP_PRIVATE static spIArrayNode CreateOrderedArrayNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength );

		//!
		//! @brief Creates an alternative array node which is not part of any metadata document.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the array node.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the array node.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return A shared pointer to a \#IArrayNode object.
		//! \attention Error is thrown in the following cases:
		//!		-# nameSpace is NULL or its contents are empty.
		//!		-# name is NULL or its contents are empty.
		//!
		XMP_PRIVATE static spIArrayNode CreateAlternativeArrayNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength );

	protected:
		//!
		//! Destructor
		//! 
		virtual ~IArrayNode_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual uint32 APICALL getArrayForm( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual uint32 APICALL getChildNodeType( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pINode_base APICALL getNodeAtIndex( sizet index, pcIError_base & error ) __NOTHROW__ = 0;
		virtual void APICALL insertNodeAtIndex( pINode_base node, sizet index, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pINode_base APICALL replaceNodeAtIndex( pINode_base node, sizet index, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pINode_base APICALL removeNodeAtIndex( sizet index, pcIError_base & error ) __NOTHROW__ = 0;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif  // __IArrayNode_h__
