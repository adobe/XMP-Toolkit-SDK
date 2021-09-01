#ifndef __ICompositeNode_h__
#define __ICompositeNode_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCore/Interfaces/INode.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"

namespace AdobeXMPCore {

	//!
	//! \brief Version1 of the interface that serves as a base interface to all composite types of nodes
	//! in the XMP DOM ( like Arrays and Structures ).
	//! \details Provides all the functions to get various properties of the composite node.
	//! \attention Support multi threading through locks but can be enabled/disabled by the client. By default
	//!  every object created does not support multi-threading.
	//!
	class XMP_PUBLIC ICompositeNode_v1
		: public virtual INode_v1
	{
	public:

		//!
		//! @brief Get the node type specified by the path relative to the composite node.
		//! \param[in] path Shared pointer to a const \#AdobeXMPCore::IPath object containing the relative path
		//! from the node to the node client is interested in.
		//! \return The type of the node.
		//! \note In case no node exists at the given path a value \#eNodeType::kNTNone is returned.
		//!
		virtual eNodeType APICALL GetNodeTypeAtPath( const spcIPath & path ) const = 0;

		//!
		//! @{
		//! @brief Get the node specified by the path relative to the composite node.
		//! \param[in] path Shared pointer to a const \#AdobeXMPCore::IPath object containing the relative path
		//! from the node to the node client is interested in.
		//! \return A shared pointer to either a const or non const \#AdobeXMPCore::INode object containing node.
		//! \note In case no node exists at the given path an invalid shared pointer is returned.
		//!
		XMP_PRIVATE spcINode GetNodeAtPath( const spcIPath & path ) const {
			return const_cast< ICompositeNode_v1 * >( this )->GetNodeAtPath( path );
		}
		virtual spINode APICALL GetNodeAtPath( const spcIPath & path ) = 0;
		//! @}

		//!
		//! @brief Appends a given node as the child of the node. 
		//! @details In case of array node it is appended at the last
		//! and in case of structure node qualified name of the node to be inserted determines its position.
		//! \param[in] node Shared pointer to an object of \#AdobeXMPCore::INode containing the node to be
		//!  appended as the last child.
		//! \note This operation is not currently implemented for the ICompositeNode interface.
		//! \attention Error is thrown in following cases:
		//!		-# provided node is invalid.
		//!		-# type of given node is not same as other child items of the array node.
		//!		-# given node is already a child of some other node.
		//!		-# composite node already has a child node with the same qualified name in case of structure node.
		//!
		virtual void APICALL AppendNode( const spINode & node ) = 0;

		//!
		//! @brief Inserts a given node at the path relative to the composite node.
		//! \param[in] node Shared pointer to an object of \#AdobeXMPCore::INode containing the node to be
		//!  inserted at the specified relative path.
		//! \param[in] path Shared pointer to a const \#AdobeXMPCore::IPath object containing the relative path.
		//! \note All the hierarchy of nodes is created if not present.
		//! \note This operation is not currently implemented for the ICompositeNode interface.
		//! \attention Error is thrown in following cases:
		//!		-# given node is invalid.
		//!		-# type of given node is not same as other child items of the array node.
		//!		-# given node is already a child of some other node.
		//!		-# given path is invalid or logically incorrect.
		//!		-# type of given node is not suitable for the destination location.
		//!		-# a node already exists at the specified path.
		//!
		virtual void APICALL InsertNodeAtPath( const spINode & node, const spcIPath & path ) = 0;

		//!
		//! @brief Replaces an existing node with the given node at the path relative to the composite node..
		//! \param[in] node Shared pointer to an object of \#AdobeXMPCore::INode.
		//! \param[in] path Shared pointer to a const \#AdobeXMPCore::IPath object containing the relative path.
		//! \return a Shared pointer to the node being replaced.
		//! \note This operation is not currently implemented for the ICompositeNode interface.
		//! \attention Error is thrown in following cases:
		//!		-# given node is invalid.
		//!		-# type of given node is not same as other child items of the array node.
		//!		-# given node is already a child of some other node.
		//!		-# given index is less than 1 or greater than current child count.
		//!		-# type of given node is not suitable for the destination location.
		//!		-# no node exists at the specified path.
		//!
		virtual spINode APICALL ReplaceNodeAtPath( const spINode & node, const spcIPath & path ) = 0;

		//!
		//! @brief Removes the node specified by the path relative to the composite node.
		//! \param[in] path Shared pointer to a const \#AdobeXMPCore::IPath object containing the relative path
		//!  from the node to the node client is interested in.
		//! \return A shared pointer to \#AdobeXMPCore::INode object containing node which is removed from the tree.
		//! \note In case no node exists at the given path an invalid shared pointer is returned.
		//!
		virtual spINode APICALL RemoveNodeAtPath( const spcIPath & path ) = 0;

		//!
		//! @{
		//! @brief Get an iterator object to iterate over all the child nodes of the composite node.
		//! \return a shared pointer to a const or non const \#INodeIterator object.
		//!
		virtual spINodeIterator APICALL Iterator() = 0;
		XMP_PRIVATE spcINodeIterator Iterator() const {
			return const_cast< ICompositeNode_v1 * >( this )->Iterator();
		}
		// @}

		//!
		//! @brief Get the count of child nodes of the composite node.
		//! \return an object of type \#AdobeXMPCommon::sizet containing the count of children of the node.
		//!
		virtual sizet APICALL ChildCount() const __NOTHROW__ = 0;

		// Wrapper non virtual functions

		//!
		//! @{
		//! @brief Get a simple node specified by the path relative to the node.
		//! \param[in] path Shared pointer to a const \#AdobeXMPCore::IPath object containing the relative path
		//!  from the node to the node client is interested in.
		//! \return A shared pointer to const or non const \#ISimpleNode object containing node.
		//! \note In case no node exists at the given path an invalid shared pointer is returned.
		//! \attention Error is thrown in case 
		//!		- a node exists at the given path but is not a simple node.
		//!
		XMP_PRIVATE spcISimpleNode GetSimpleNodeAtPath( const spcIPath & path ) const {
			auto node = GetNodeAtPath( path );
			if ( node ) return node->ConvertToSimpleNode();
			return spcISimpleNode();
		}

		XMP_PRIVATE spISimpleNode GetSimpleNodeAtPath( const spcIPath & path ) {
			auto node = GetNodeAtPath( path );
			if ( node ) return node->ConvertToSimpleNode();
			return spISimpleNode();
		}
		//! @}

		//!
		//! @{
		//! @brief Get a structure node specified by the path relative to the node.
		//! \param[in] path Shared pointer to a const \#AdobeXMPCore::IPath object containing the relative path
		//!  from the node to the node client is interested in.
		//! \return A shared pointer to const or non const \#IStructureNode object containing node.
		//! \note In case no node exists at the given path an invalid shared pointer is returned.
		//! \attention Error is thrown in case 
		//!		- a node exists at the given path but is not a structure node.
		//!
		XMP_PRIVATE spcIStructureNode GetStructureNodeAtPath( const spcIPath & path ) const {
			auto node = GetNodeAtPath( path );
			if ( node ) return node->ConvertToStructureNode();
			return spcIStructureNode();
		}

		XMP_PRIVATE spIStructureNode GetStructureNodeAtPath( const spcIPath & path ) {
			auto node = GetNodeAtPath( path );
			if ( node ) return node->ConvertToStructureNode();
			return spIStructureNode();
		}
		// !@}

		//!
		//! @{
		//! @brief Get an array node specified by the path relative to the node.
		//! \param[in] path Shared pointer to a const \#AdobeXMPCore::IPath object containing the relative path
		//! from the node to the node client is interested in.
		//! \return A shared pointer to const or non const \#IArrayNode object containing node.
		//! \note In case no node exists at the given path an invalid shared pointer is returned.
		//! \attention Error is thrown in case 
		//!		- a node exists at the given path but is not an array node.
		//!
		XMP_PRIVATE spcIArrayNode GetArrayNodeAtPath( const spcIPath & path ) const {
			auto node = GetNodeAtPath( path );
			if ( node ) return node->ConvertToArrayNode();
			return spcIArrayNode();
		}

		XMP_PRIVATE spIArrayNode GetArrayNodeAtPath( const spcIPath & path ) {
			auto node = GetNodeAtPath( path );
			if ( node ) return node->ConvertToArrayNode();
			return spIArrayNode();
		}
		// !@}

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Returns the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return Either a const or non const pointer to ICompositeNode interface.
		//!
		virtual pICompositeNode APICALL GetActualICompositeNode() __NOTHROW__ = 0;
		XMP_PRIVATE pcICompositeNode GetActualICompositeNode() const __NOTHROW__ {
			return const_cast< ICompositeNode_v1 * >( this )->GetActualICompositeNode();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Returns the pointer to internal interfaces.
		//! \return Either a const or non const pointer to ICompositeNode_I interface.
		//!
		virtual AdobeXMPCore_Int::pICompositeNode_I APICALL GetICompositeNode_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPCore_Int::pcICompositeNode_I GetICompositeNode_I() const __NOTHROW__ {
			return const_cast< ICompositeNode_v1 * >( this )->GetICompositeNode_I();
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
		XMP_PRIVATE static spICompositeNode MakeShared( pICompositeNode_base ptr );
		XMP_PRIVATE static spcICompositeNode MakeShared( pcICompositeNode_base ptr ) {
			return const_cast< ICompositeNode_v1 * >( ptr )->MakeShared( const_cast<pICompositeNode_base>(ptr) );
		}
		//!
		//! @}

		//!
		//! @brief Returns the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kICompositeNodeID; }

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
		virtual ~ICompositeNode_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual uint32 APICALL getNodeTypeAtPath( pcIPath_base path, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pINode_base APICALL getNodeAtPath( pcIPath_base path, pcIError_base & error ) __NOTHROW__ = 0;
		virtual void APICALL appendNode( pINode_base node, pcIError_base & error ) __NOTHROW__ = 0;
		virtual void APICALL insertNodeAtPath( pINode_base node, pcIPath_base path, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pINode_base APICALL replaceNodeAtPath( pINode_base node, pcIPath_base path, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pINode_base APICALL removeNodeAtPath( pcIPath_base path, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pINodeIterator_base APICALL iterator( pcIError_base & error ) __NOTHROW__ = 0;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

//! \cond XMP_INTERNAL_DOCUMENTATION

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

namespace AdobeXMPCore {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	class ICompositeNodeProxy
		: public virtual ICompositeNode
		, public virtual INodeProxy
	{
	private:
		pICompositeNode mRawPtr;

	public:
		ICompositeNodeProxy( pICompositeNode ptr );
		~ICompositeNodeProxy() __NOTHROW__ ;

		pICompositeNode APICALL GetActualICompositeNode() __NOTHROW__;
		AdobeXMPCore_Int::pICompositeNode_I APICALL GetICompositeNode_I() __NOTHROW__;

		virtual eNodeType APICALL GetNodeTypeAtPath( const spcIPath & path ) const;
		virtual spINode APICALL GetNodeAtPath( const spcIPath & path );
		virtual void APICALL AppendNode( const spINode & node );
		virtual void APICALL InsertNodeAtPath( const spINode & node, const spcIPath & path );
		virtual spINode APICALL ReplaceNodeAtPath( const spINode & node, const spcIPath & path );
		virtual spINode APICALL RemoveNodeAtPath( const spcIPath & path );
		virtual spINodeIterator APICALL Iterator();
		virtual sizet APICALL ChildCount() const __NOTHROW__;

	protected:
		virtual uint32 APICALL getNodeTypeAtPath( pcIPath_base path, pcIError_base & error ) const __NOTHROW__;
		virtual pINode_base APICALL getNodeAtPath( pcIPath_base path, pcIError_base & error ) __NOTHROW__;
		virtual void APICALL appendNode( pINode_base node, pcIError_base & error ) __NOTHROW__;
		virtual void APICALL insertNodeAtPath( pINode_base node, pcIPath_base path, pcIError_base & error ) __NOTHROW__;
		virtual pINode_base APICALL replaceNodeAtPath( pINode_base node, pcIPath_base path, pcIError_base & error ) __NOTHROW__;
		virtual pINode_base APICALL removeNodeAtPath( pcIPath_base path, pcIError_base & error ) __NOTHROW__;
		virtual pINodeIterator_base APICALL iterator( pcIError_base & error ) __NOTHROW__;

	};

#if XMP_WinBuild
	#pragma warning( pop )
#endif

}

#endif  // BUILDING_XMPCORE_LIB
//! \endcond

#endif  // __ICompositeNode_h__
