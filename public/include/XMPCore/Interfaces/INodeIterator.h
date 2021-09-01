#ifndef __INodeIterator_h__
#define __INodeIterator_h__ 1


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
#include "XMPCore/Interfaces/INode.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"

namespace AdobeXMPCore {

	//!
	//! @brief Interface that represents an iterator over the mutable children of a XMP DOM Node.
	//! \note Iterators are valid as long as their are no changes performed on the node. In case there are some
	//!  changes performed on the node then the behavior is undefined.
	//!
	class XMP_PUBLIC INodeIterator_v1
		: public virtual ISharedObject
		, public virtual IVersionable
	{
	public:

		//!
		//! @brief Gets the type of the node currently pointed by the iterator.
		//! \return A value of type \#INode_v1::eNodeType indicating the type of the node currently pointed by the iterator.
		//!
		virtual INode_v1::eNodeType APICALL GetNodeType() const = 0;

		//!
		//! @{
		//! @brief Gets the node currently pointed by the iterator.
		//! \return A shared pointer to a const or non const object of type \#INode.
		//! \note In case iterator has gone beyond its limit, an invalid shared pointer is returned
		//!
		virtual spINode APICALL GetNode() = 0;
		XMP_PRIVATE spcINode APICALL GetNode() const {
			return const_cast< INodeIterator * >( this )->GetNode();
		};
		//! @}

		//!
		//! @{
		//! @brief Gets the iterator's currently pointed node as simple node, if possible.
		//! \return A shared pointer to a const or non const object of type \#ISimpleNode.
		//! \attention Error is thrown in case
		//!		- iterator's currently pointed node is valid but is not a simple node.
		//! \note In case iterator has gone beyond its limit, an invalid shared pointer is returned.
		//!
		XMP_PRIVATE spISimpleNode GetSimpleNode() {
			auto node = GetNode();
			if ( node ) return node->ConvertToSimpleNode();
			return spISimpleNode();
		}

		XMP_PRIVATE spcISimpleNode GetSimpleNode() const {
			return const_cast< INodeIterator * >( this )->GetSimpleNode();
		}
		//! @}

		//!
		//! @brief Gets the iterator's currently pointed node as structure node, if possible.
		//! \return A shared pointer to a const or non const object of type \#IStructureNode.
		//! \attention Error is thrown in case
		//!		- iterator's currently pointed node is valid but is not a structure node.
		//! \note In case iterator has gone beyond its limit, an invalid shared pointer is returned.
		//!
		XMP_PRIVATE spIStructureNode GetStructureNode() {
			auto node = GetNode();
			if ( node ) return node->ConvertToStructureNode();
			return spIStructureNode();
		}

		XMP_PRIVATE spcIStructureNode GetStructureNode() const {
			return const_cast< INodeIterator * >( this )->GetStructureNode();
		}
		//! @}

		//!
		//! @brief Gets the iterator's currently pointed node as an array node, if possible.
		//! \return A shared pointer to a const or non const object of type \#IArrayNode.
		//! \attention Error is thrown in case
		//!		- iterator's currently pointed node is valid but is not an array node.
		//! \note In case iterator has gone beyond its limit, an invalid shared pointer is returned.
		//!
		XMP_PRIVATE spIArrayNode GetArrayNode() {
			auto node = GetNode();
			if ( node ) return node->ConvertToArrayNode();
			return spIArrayNode();
		}

		XMP_PRIVATE spcIArrayNode GetArrayNode() const {
			return const_cast< INodeIterator * >( this )->GetArrayNode();
		}
		//! @}

		//!
		//! @{
		//! @brief Advances iterator by one position.
		//! \return A shared pointer to a const or non object of type \#INodeIterator.
		//! \note Returned shared pointer is invalid in case the current node is the last one.
		//!
		virtual spINodeIterator APICALL Next() = 0;
		XMP_PRIVATE spcINodeIterator APICALL Next() const {
			return const_cast< INodeIterator * >( this )->Next();
		}
		//! @}


		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION

		//!
		//! @{
		//! @brief Returns the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return Either a const or non const pointer to INodeIterator interface.
		//!
		virtual pINodeIterator APICALL GetActualINodeIterator() __NOTHROW__ = 0;
		XMP_PRIVATE pcINodeIterator GetActualINodeIterator() const __NOTHROW__ {
			return const_cast< INodeIterator * >( this )->GetActualINodeIterator();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Returns the pointer to internal interfaces.
		//! \return Either a const or non const pointer to INodeIterator_I interface.
		//!
		virtual AdobeXMPCore_Int::pINodeIterator_I APICALL GetINodeIterator_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPCore_Int::pcINodeIterator_I GetINodeIterator_I() const __NOTHROW__ {
			return const_cast< INodeIterator * >( this )->GetINodeIterator_I();
		}
		//!
		//! @}
		
		//!
		//! @{
		//! @brief Converts raw pointer to shared pointer.
		//! \return Shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spINodeIterator MakeShared( pINodeIterator ptr );
		XMP_PRIVATE static spcINodeIterator MakeShared( pcINodeIterator ptr ) {
			return MakeShared( const_cast< pINodeIterator >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @}

		//!
		//! return the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kINodeIteratorID; }

		//!
		//! return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

	protected:
		//!
		//! Destructor
		//! 
		virtual ~INodeIterator_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual uint32 APICALL getNodeType( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pINode_base APICALL getNode( pcIError_base & error ) __NOTHROW__ = 0;
		virtual pINodeIterator_base APICALL next( pcIError_base & error ) __NOTHROW__ = 0;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif  // __INodeIterator_h__
