#ifndef IStructureNode_I_h__
#define IStructureNode_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================


#include "XMPCore/Interfaces/IStructureNode.h"
#include "XMPCore/Interfaces/ICompositeNode_I.h"

namespace AdobeXMPCore_Int {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! \brief Internal interface that represents a structure Node of XMP DOM.
	//! \details Provides all the functions to get and set various properties of the structure node.
	//! \attention Support multi threading through locks but can be enabled/disabled by the client. By default
	//! every object created does not support multi-threading.
	//!
	class IStructureNode_I
		: public virtual IStructureNode
		, public virtual ICompositeNode_I
	{
	public:

		//!
		//! @{
		//! Get the child of the node having specified namespace and name.
		//! \param[in] nameSpace shared pointer to a const IUTF8String object containing name space URI of the child node.
		//! \param[in] name shared pointer to a const IUTF8String object local name of the child node.
		//! \return a shared pointer to either a const or const child node.
		//! \note In case no child exists with the specified nameSpace and name combination then an invalid shared pointer
		//! is returned.
		//!
		using IStructureNode_v1::GetNode;
		XMP_PRIVATE spcINode GetNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) const {
			return const_cast< IStructureNode_I * >( this )->GetNode( nameSpace, name );
		}
		virtual spINode APICALL GetNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) = 0;
		//! @}

		//!
		//! @{
		//! Get the node's child having specified name space and name as simple node.
		//! \param[in] nameSpace shared pointer to a const IUTF8String object name space URI of the child node.
		//! \param[in] name shared pointer to a const IUTF8String object local name of the child node.
		//! \return a shared pointer to const or non const #ISimpleNode object containing child.
		//! \note In case no child exists with the specified nameSpace and name combination then an invalid shared pointer
		//! is returned.
		//! \attention Error is thrown in case 
		//!		- a child exists with the specified nameSpace and name combination but is not a simple node.
		//!
		using IStructureNode_v1::GetSimpleNode;
		XMP_PRIVATE spcISimpleNode GetSimpleNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) const {
			auto node = GetNode( nameSpace, name );
			if ( node ) return node->ConvertToSimpleNode();
			return spcISimpleNode();
		}

		XMP_PRIVATE spISimpleNode GetSimpleNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) {
			auto node = GetNode( nameSpace, name );
			if ( node ) return node->ConvertToSimpleNode();
			return spISimpleNode();
		}
		//! @}

		//!
		//! @{
		//! Get the node's child having specified name space and name as structure node.
		//! \param[in] nameSpace shared pointer to a const IUTF8String object containing name space URI of the child node.
		//! \param[in] name shared pointer to a const IUTF8String object containing local name of the child node.
		//! \return a shared pointer to const or non const #IStructureNode object containing child.
		//! \note In case no child exists with the specified nameSpace and name combination then an invalid shared pointer
		//! is returned.
		//! \attention Error is thrown in case 
		//!		- a child exists with the specified nameSpace and name combination but is not a structure node.
		//!
		using IStructureNode_v1::GetStructureNode;
		XMP_PRIVATE spcIStructureNode GetStructureNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) const {
			auto node = GetNode( nameSpace, name );
			if ( node ) return node->ConvertToStructureNode();
			return spcIStructureNode();
		}

		XMP_PRIVATE spIStructureNode GetStructureNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) {
			auto node = GetNode( nameSpace, name );
			if ( node ) return node->ConvertToStructureNode();
			return spIStructureNode();
		}
		//! @}

		//!
		//! @{
		//! Get the node's child having specified name space and name as an array node.
		//! \param[in] nameSpace shared pointer to a const IUTF8String object containing name space URI of the child node.
		//! \param[in] name shared pointer to a const IUTF8String object containing local name of the child node.
		//! \return a shared pointer to const or non const #ISimpleNode object containing child.
		//! \note In case no child exists with the specified nameSpace and name combination then an invalid shared pointer
		//! is returned.
		//! \attention Error is thrown in case 
		//!		- a child exists with the specified nameSpace and name combination but is not an array node.
		//!
		using IStructureNode_v1::GetArrayNode;
		XMP_PRIVATE spcIArrayNode GetArrayNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) const {
			auto node = GetNode( nameSpace, name );
			if ( node ) return node->ConvertToArrayNode();
			return spcIArrayNode();
		}

		XMP_PRIVATE spIArrayNode GetArrayNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) {
			auto node = GetNode( nameSpace, name );
			if ( node ) return node->ConvertToArrayNode();
			return spIArrayNode();
		}
		//! @}

		//!
		//! Remove the node with the specified nameSpace and name.
		//! \param[in] nameSpace shared pointer to a const IUTF8String object containing name space URI of the child node.
		//! \param[in] name shared pointer to a const IUTF8String object containing local name of the child node.
		//! \return a shared pointer to #AdobeXMPCore::INode object containing node which is removed from the tree.
		//! \note In case no node exists with the given nameSpace and name combination an invalid shared pointer is returned.
		//!
		using IStructureNode_v1::RemoveNode;
		virtual spINode APICALL RemoveNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) = 0;

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }

		virtual pIStructureNode APICALL GetActualIStructureNode() __NOTHROW__ { return this; }
		virtual pIStructureNode_I APICALL GetIStructureNode_I() __NOTHROW__ { return this; }

		//!
		//! list all functions to be defined in this interface
		//!


		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

		// static factory functions

		//!
		//! Creates a simple property node which is not part of any metadata document.
		//! \param[in] nameSpace shared pointer to a constant IUTF8String object containing name space URI of the simple node.
		//! \param[in] name shared pointer to a constant IUTF8String object containing local name of the simple node.
		//! \return a shared pointer to a #IStructureNode object.
		//! \attention Error is thrown in case
		//!		- nameSpace or name are invalid pointers, or
		//!		- their contents are empty.
		//!
		XMP_PRIVATE static spIStructureNode CreateStructureNode( const spcIUTF8String & nameSpace, const spcIUTF8String name );

	protected:
		virtual ~IStructureNode_I() __NOTHROW__ {}
		pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );

		virtual pINode_base APICALL getNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__;
		virtual uint32 APICALL getChildNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) const __NOTHROW__;
		virtual void APICALL insertNode( pINode_base node, pcIError_base & error ) __NOTHROW__;
		virtual pINode_base APICALL removeNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__;
		virtual pINode_base APICALL replaceNode( pINode_base node, pcIError_base & error ) __NOTHROW__;
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

#endif // IStructureNode_I_h__
