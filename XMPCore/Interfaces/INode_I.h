#ifndef __INode_I_h__
#define __INode_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/XMPCoreFwdDeclarations_I.h"
#include "XMPCore/Interfaces/INode.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject_I.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IThreadSafe_I.h"

namespace AdobeXMPCore_Int {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	//!
	//! Internal interface that serves as a base interface to all types of nodes in the XMP DOM.
	//! Provides all the functions to get various properties of the node.
	//!
	class INode_I
		: public virtual INode
		, public virtual ISharedObject_I
		, public virtual IThreadSafe_I
	{
	public:

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }

		//!
		//! Change the parent of the node.
		//! \param[in] parent a pointer of #INode representing the new parent of the node.
		//! \note If the parent is invalid shared pointer it will be treated like removing the node from the
		//! tree.
		//!
		virtual void APICALL ChangeParent( pINode parent ) = 0;

		//!
		//! @{
		//! Get the qualifier of the node having specified namespace and name.
		//! \param[in] nameSpace shared pointer to a const IUTF8String object containing name space URI of the qualifier node.
		//! \param[in] name shared pointer to a const IUTF8String object containing local name of the qualifier node.
		//! \return a shared pointer to either a const or const qualifier node.
		//! \note In case no qualifier exists with the specified nameSpace and name combination then an invalid shared pointer
		//! is returned.
		//!
		using INode_v1::GetQualifier;
		XMP_PRIVATE spcINode GetQualifier( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) const {
			return const_cast< INode_I * >( this )->GetQualifier( nameSpace, name );
		}
		virtual spINode APICALL GetQualifier( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) = 0;
		//!
		//! @}

		//!
		//! @{
		//! Get the node's qualifier having specified name space and name as simple node.
		//! \param[in] nameSpace pointer to a const IUTF8String object containing name space URI of the qualifier node.
		//! \param[in] name pointer to a const IUTF8String object containing local name of the qualifier node.
		//! \return a shared pointer to const or non const #ISimpleNode object containing qualifier.
		//! \note In case no qualifier exists with the specified nameSpace and name combination then an invalid shared pointer
		//! is returned.
		//! \attention Error is thrown in case 
		//!		- a qualifier exists with the specified nameSpace and name combination but is not a simple node.
		//!
		using INode_v1::GetSimpleQualifier;
		XMP_PRIVATE spcISimpleNode GetSimpleQualifier( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) const {
			auto node = GetQualifier( nameSpace, name );
			if ( node ) return node->ConvertToSimpleNode();
			return spcISimpleNode();
		}

		XMP_PRIVATE spISimpleNode GetSimpleQualifier( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) {
			auto node = GetQualifier( nameSpace, name );
			if ( node ) return node->ConvertToSimpleNode();
			return spISimpleNode();
		}
		//! @}

		//!
		//! @{
		//! Get the node's qualifier having specified name space and name as structure node.
		//! \param[in] nameSpace pointer to a const IUTF8String object containing name space URI of the qualifier node.
		//! \param[in] name pointer to a const IUTF8String object containing local name of the qualifier node.
		//! \return a shared pointer to const or non const #IStructureNode object containing qualifier.
		//! \note In case no qualifier exists with the specified nameSpace and name combination then an invalid shared pointer
		//! is returned.
		//! \attention Error is thrown in case 
		//!		- a qualifier exists with the specified nameSpace and name combination but is not a structure node.
		//!
		using INode_v1::GetStructureQualifier;
		XMP_PRIVATE spcIStructureNode GetStructureQualifier( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) const {
			auto node = GetQualifier( nameSpace, name );
			if ( node ) return node->ConvertToStructureNode();
			return spcIStructureNode();
		}

		XMP_PRIVATE spIStructureNode GetStructureQualifier( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) {
			auto node = GetQualifier( nameSpace, name );
			if ( node ) return node->ConvertToStructureNode();
			return spIStructureNode();
		}
		//! @}

		//!
		//! @{
		//! Get the node's qualifier having specified name space and name as an array node.
		//! \param[in] nameSpace pointer to a const IUTF8String object containing name space URI of the qualifier node.
		//! \param[in] name pointer to a const IUTF8String object containing local name of the qualifier node.
		//! \return a shared pointer to const or non const #ISimpleNode object containing qualifier.
		//! \note In case no qualifier exists with the specified nameSpace and name combination then an invalid shared pointer
		//! is returned.
		//! \attention Error is thrown in case 
		//!		- a qualifier exists with the specified nameSpace and name combination but is not an array node.
		//!
		using INode_v1::GetArrayQualifier;
		XMP_PRIVATE spcIArrayNode GetArrayQualifier( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) const {
			auto node = GetQualifier( nameSpace, name );
			if ( node ) return node->ConvertToArrayNode();
			return spcIArrayNode();
		}

		XMP_PRIVATE spIArrayNode GetArrayQualifier( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) {
			auto node = GetQualifier( nameSpace, name );
			if ( node ) return node->ConvertToArrayNode();
			return spIArrayNode();
		}
		//! @}

		//!
		//! Remove the qualifier node with the specified nameSpace and name.
		//! \param[in] nameSpace pointer to a const IUTF8String object containing name space URI of the qualifier node.
		//! \param[in] name pointer to a const IUTF8String object containing local name of the qualifier node.
		//! \return a shared pointer to #AdobeXMPCore::INode object representing qualifier node which is removed from the node.
		//! \note In case no qualifier node exists at the given index an invalid shared pointer is returned.
		//!
		using INode_v1::RemoveQualifier;
		virtual spINode APICALL RemoveQualifier( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) = 0;

		//!
		//! @{
		//! Get the raw pointer to the parent of the node.
		//! \return a const or non const pointer to the parent of the node.
		//!
		pcINode GetRawParentPointer() const {
			return const_cast< INode_I * >( this )->GetRawParentPointer();
		}
		virtual pINode APICALL GetRawParentPointer() = 0;
		//! @}


		virtual pINode APICALL GetActualINode() __NOTHROW__ { return this; }
		virtual pINode_I APICALL GetINode_I() __NOTHROW__ { return this; }

		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

		virtual void SetIndex( sizet currentIndex ) = 0;
		virtual void SetIsQualifierNode( bool isQualifierNode ) = 0;
		virtual void RegisterChange() = 0;
		virtual bool ValidateNameOrNameSpaceChangeForAChild( const spcIUTF8String & currentNameSpace, const spcIUTF8String & currentName,
			const spcIUTF8String & newNameSpace, const spcIUTF8String & newName ) = 0;
		virtual void UnRegisterChange() = 0;
		virtual void SetQualifiers( const spIStructureNode & node ) = 0;

	protected:
		virtual ~INode_I() __NOTHROW__ {}
		pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );

		virtual pINode_base APICALL getParent( pcIError_base & error ) __NOTHROW__;
		virtual uint32 APICALL getParentNodeType( pcIError_base & error ) const __NOTHROW__;
		virtual void APICALL setName( const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__;
		virtual pcIUTF8String_base APICALL getName( pcIError_base & error ) const __NOTHROW__;
		virtual void APICALL setNameSpace( const char * nameSpace, sizet nameSpaceLength, pcIError_base & error ) __NOTHROW__;
		virtual pcIUTF8String_base APICALL getNameSpace( pcIError_base & error ) const __NOTHROW__;
		virtual pIPath_base APICALL getPath( pcIError_base & error ) const __NOTHROW__;
		virtual pINodeIterator_base APICALL qualifiersIterator( pcIError_base & error ) __NOTHROW__;
		virtual uint32 APICALL getQualifierNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) const __NOTHROW__;
		virtual pINode_base APICALL getQualifier( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__;
		virtual void APICALL insertQualifier( pINode_base base, pcIError_base & error ) __NOTHROW__;
		virtual pINode_base APICALL replaceQualifier( pINode_base node, pcIError_base & error ) __NOTHROW__;
		virtual pINode_base APICALL removeQualifier( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__;
		virtual uint32 APICALL getNodeType( pcIError_base & error ) const __NOTHROW__;
		virtual uint32 APICALL isArrayItem( pcIError_base & error ) const __NOTHROW__;
		virtual uint32 APICALL isQualifierNode( pcIError_base & error ) const __NOTHROW__;
		virtual sizet APICALL getIndex( pcIError_base & error ) const __NOTHROW__;
		virtual uint32 APICALL hasQualifiers( pcIError_base & error ) const __NOTHROW__;
		virtual uint32 APICALL hasContent( pcIError_base & error ) const __NOTHROW__;
		virtual uint32 APICALL isEmpty( pcIError_base & error ) const __NOTHROW__;
		virtual uint32 APICALL hasChanged( pcIError_base & error ) const __NOTHROW__;
		virtual void APICALL clear( uint32 contents, uint32 qualifiers, pcIError_base & error ) __NOTHROW__;
		virtual pINode_base APICALL clone( uint32 igoreEmptyNodes, uint32 ignoreNodesWithOnlyQualifiers, pcIError_base & error ) const __NOTHROW__;
		virtual pISimpleNode_base APICALL convertToSimpleNode( pcIError_base & error ) __NOTHROW__;
		virtual pIStructureNode_base APICALL convertToStructureNode( pcIError_base & error ) __NOTHROW__;
		virtual pIArrayNode_base APICALL convertToArrayNode( pcIError_base & error ) __NOTHROW__;
		virtual pIMetadata_base APICALL convertToMetadata( pcIError_base & error ) __NOTHROW__;
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

#endif  // __INode_I_h__
