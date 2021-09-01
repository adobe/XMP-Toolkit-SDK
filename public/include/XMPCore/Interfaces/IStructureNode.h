#ifndef IStructureNode_h__
#define IStructureNode_h__ 1

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
	//! @brief Version1 of the interface that represents a structure Node of XMP DOM.
	//! \details Provides all the functions to get and set various properties of the structure node.
	//! \attention Support multi threading through locks but can be enabled/disabled by the client. By default
	//!            every object created does not support multi-threading.
	//!
	class XMP_PUBLIC IStructureNode_v1
		: public virtual ICompositeNode_v1
	{
	public:

		//!
		//! @brief Gets the type of the node's child having specified namespace and name.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the child node.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the child node.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to AdobeXMPCommon::npos.
		//! \return An object of type #eNodeType indicating the type of the node's child.
		//! \note In case no child exists with the specified nameSpace and name combination then an eNodeType::kNTNone is returned.
		//!
		virtual eNodeType APICALL GetChildNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) const = 0;


		//!
		//! @{
		//! @brief Gets the child of the node having specified namespace and name.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the child node.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the child node.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to AdobeXMPCommon::npos.
		//! \return A shared pointer to either a const or const child node.
		//! \note In case no child exists with the specified nameSpace and name combination then an invalid shared pointer
		//!       is returned.
		//!
		XMP_PRIVATE spcINode GetNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) const {
			return const_cast< IStructureNode_v1 * >( this )->GetNode( nameSpace, nameSpaceLength, name, nameLength );
		}
		virtual spINode APICALL GetNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) = 0;
		//! @}

		//!
		//! @{
		//! @brief Gets the node's child having specified name space and name as simple node.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the child node.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the child node.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to AdobeXMPCommon::npos.
		//! \return A shared pointer to const or non const ISimpleNode object containing child.
		//! \note In case no child exists with the specified nameSpace and name combination then an invalid shared pointer
		//!       is returned.
		//! \attention Error is thrown in case 
		//!		- a child exists with the specified nameSpace and name combination but is not a simple node.
		//!
		XMP_PRIVATE spcISimpleNode GetSimpleNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) const {
			auto node = GetNode( nameSpace, nameSpaceLength, name, nameLength );
			if ( node ) return node->ConvertToSimpleNode();
			return spcISimpleNode();
		}

		XMP_PRIVATE spISimpleNode GetSimpleNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
			auto node = GetNode( nameSpace, nameSpaceLength, name, nameLength );
			if ( node ) return node->ConvertToSimpleNode();
			return spISimpleNode();
		}
		//! @}

		//!
		//! @{
		//! @brief Gets the node's child having specified name space and name as structure node.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the child node.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the child node.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to AdobeXMPCommon::npos.
		//! \return A shared pointer to const or non const IStructureNode object containing child.
		//! \note In case no child exists with the specified nameSpace and name combination then an invalid shared pointer
		//!       is returned.
		//! \attention Error is thrown in case 
		//!		- a child exists with the specified nameSpace and name combination but is not a structure node.
		//!
		XMP_PRIVATE spcIStructureNode GetStructureNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) const {
			auto node = GetNode( nameSpace, nameSpaceLength, name, nameLength );
			if ( node ) return node->ConvertToStructureNode();
			return spcIStructureNode();
		}

		XMP_PRIVATE spIStructureNode GetStructureNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
			auto node = GetNode( nameSpace, nameSpaceLength, name, nameLength );
			if ( node ) return node->ConvertToStructureNode();
			return spIStructureNode();
		}
		//! @}

		//!
		//! @{
		//! @brief Gets the node's child having specified name space and name as an array node.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the child node.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the child node.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to AdobeXMPCommon::npos.
		//! \return A shared pointer to const or non const ISimpleNode object containing child.
		//! \note In case no child exists with the specified nameSpace and name combination then an invalid shared pointer
		//!       is returned.
		//! \attention Error is thrown in case 
		//!		- a child exists with the specified nameSpace and name combination but is not an array node.
		//!
		XMP_PRIVATE spcIArrayNode GetArrayNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) const {
			auto node = GetNode( nameSpace, nameSpaceLength, name, nameLength );
			if ( node ) return node->ConvertToArrayNode();
			return spcIArrayNode();
		}

		XMP_PRIVATE spIArrayNode GetArrayNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
			auto node = GetNode( nameSpace, nameSpaceLength, name, nameLength );
			if ( node ) return node->ConvertToArrayNode();
			return spIArrayNode();
		}
		//! @}

		//!
		//! @brief Inserts a given node.
		//! \param[in] node Shared pointer to an object of AdobeXMPCore::INode containing the node to be inserted.
		//! \attention Error is thrown in following cases:
		//!		-# given node is invalid.
		//!		-# given node is already a child of some other node.
		//!		-# there exists a node with the same nameSpace and name combination.
		//!
		virtual void APICALL InsertNode( const spINode & node ) = 0;

		//!
		//! @brief Replaces a given node.
		//! \param[in] node Shared pointer to an object of AdobeXMPCore::INode.
		//! \return A shared pointer to the node being replaced.
		//! \attention Error is thrown in following cases:
		//!		-# given node is invalid.
		//!		-# given node is already a child of some other node.
		//!		-# there exists no node with the same nameSpace and name combination.
		//! \note Type of the old existing node may/may not be same as that of new node.
		//!
		virtual spINode APICALL ReplaceNode( const spINode & node ) = 0;

		//!
		//! @brief Removes the node with the specified nameSpace and name.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the child node.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the child node.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to AdobeXMPCommon::npos.
		//! \return A shared pointer to AdobeXMPCore::INode object containing node which is removed from the tree.
		//! \note In case no node exists with the given nameSpace and name combination an invalid shared pointer is returned.
		//!
		virtual spINode APICALL RemoveNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) = 0;

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Returns the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return Either a const or non const pointer to IStructureNode interface.
		//!
		virtual pIStructureNode APICALL GetActualIStructureNode() __NOTHROW__ = 0;
		XMP_PRIVATE pcIStructureNode GetActualIStructureNode() const __NOTHROW__ {
			return const_cast< IStructureNode_v1 * >( this )->GetActualIStructureNode();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Returns the pointer to internal interfaces.
		//! \return Either a const or non const pointer to IStructureNode_I interface.
		//!
		virtual AdobeXMPCore_Int::pIStructureNode_I APICALL GetIStructureNode_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPCore_Int::pcIStructureNode_I GetIStructureNode_I() const __NOTHROW__ {
			return const_cast< IStructureNode_v1 * >( this )->GetIStructureNode_I();
		}
		//!
		//! @}
		
		//!
		//! @{
		//! @brief Converts raw pointer to shared pointer. The raw pointer is of version 1 interface
		//!        where as the returned shared pointer depends on the version client is interested in.
		//! \return Shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spIStructureNode MakeShared( pIStructureNode_base ptr );
		XMP_PRIVATE static spcIStructureNode MakeShared( pcIStructureNode_base ptr ) {
			return MakeShared( const_cast< pIStructureNode_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief Returns the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIStructureNodeID; }

		//!
		//! @brief Returns the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

		// Factories to create the structure node

		//!
		//! @brief Creates a structure node which is not part of any metadata document.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the structure node.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the structure node.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to AdobeXMPCommon::npos.
		//! \return A shared pointer to a AdobeXMPCore::IStructureNode object.
		//! \attention Error is thrown in the following cases:
		//!		-# nameSpace is NULL or its contents are empty.
		//!		-# name is NULL or its contents are empty.
		//!
		XMP_PRIVATE static spIStructureNode CreateStructureNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength );

	protected:
		//!
		//! Destructor
		//! 
		virtual ~IStructureNode_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual uint32 APICALL getChildNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pINode_base APICALL getNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ = 0;
		virtual void APICALL insertNode( pINode_base node, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pINode_base APICALL replaceNode( pINode_base node, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pINode_base APICALL removeNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ = 0;

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

	class IStructureNodeProxy
		: public virtual IStructureNode
		, public virtual ICompositeNodeProxy
	{
	private:
		pIStructureNode mRawPtr;

	public:
		IStructureNodeProxy( pIStructureNode ptr );
		~IStructureNodeProxy() __NOTHROW__ ;

		AdobeXMPCore_Int::pIStructureNode_I APICALL GetIStructureNode_I() __NOTHROW__;
		virtual pIStructureNode APICALL GetActualIStructureNode() __NOTHROW__;

		virtual eNodeType APICALL GetChildNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) const;
		virtual spINode APICALL GetNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength );
		virtual void APICALL InsertNode( const spINode & node );
		virtual spINode APICALL ReplaceNode( const spINode & node );
		virtual spINode APICALL RemoveNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength );

	protected:
		virtual uint32 APICALL getChildNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) const __NOTHROW__;
		virtual pINode_base APICALL getNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__;
		virtual void APICALL insertNode( pINode_base node, pcIError_base & error ) __NOTHROW__;
		virtual pINode_base APICALL replaceNode( pINode_base node, pcIError_base & error ) __NOTHROW__;
		virtual pINode_base APICALL removeNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__;
	};

#if XMP_WinBuild
	#pragma warning( pop )
#endif

}

#endif  // BUILDING_XMPCORE_LIB
//! \endcond

#endif // IStructureNode_h__
