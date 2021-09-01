#ifndef __NodeImpl_h__
#define __NodeImpl_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#if !(IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED)
	#error "Not adhering to design constraints"
	// this file should only be included from its own cpp file
#endif

#include "XMPCore/Interfaces/INode_I.h"
#include "XMPCommon/BaseClasses/MemoryManagedObject.h"
#include "XMPCommon/ImplHeaders/SharedObjectImpl.h"
#include "XMPCommon/ImplHeaders/ThreadSafeImpl.h"
#include "XMPCommon/Utilities/TAtomicTypes.h"

#include <map>

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {

	class NodeImpl
		: public virtual INode_I
		, public virtual SharedObjectImpl
		, public virtual MemoryManagedObject
		, public virtual ThreadSafeImpl
	{
	public:
		NodeImpl() {}
		NodeImpl( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength );

		virtual void APICALL ChangeParent( pINode parent );
		virtual pINode APICALL GetRawParentPointer();
		virtual spINode APICALL GetParent();
		virtual void APICALL SetName( const char * name, sizet nameLength );
		virtual spcIUTF8String APICALL GetName() const;
		virtual void APICALL SetNameSpace( const char * nameSpace, sizet nameSpaceLength );
		virtual spcIUTF8String APICALL GetNameSpace() const;
		virtual spIPath APICALL GetPath() const;
		virtual sizet APICALL QualifiersCount() const __NOTHROW__;
		virtual spINodeIterator APICALL QualifiersIterator();
		virtual spINode APICALL GetQualifier( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength );
		virtual spINode APICALL GetQualifier( const spcIUTF8String & nameSpace, const spcIUTF8String & name );
		virtual void APICALL InsertQualifier( const spINode & node );
		virtual spINode APICALL ReplaceQualifier( const spINode & node );
		virtual spINode APICALL RemoveQualifier( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength );
		virtual spINode APICALL RemoveQualifier( const spcIUTF8String & nameSpace, const spcIUTF8String & name );
		virtual bool APICALL IsArrayItem() const;
		virtual bool APICALL HasQualifiers() const;
		virtual bool APICALL IsEmpty() const;
		virtual bool APICALL HasChanged() const;
		virtual void APICALL AcknowledgeChanges() const __NOTHROW__;
		virtual void APICALL Clear( bool contents = true, bool qualifiers = true );
		virtual spINode APICALL Clone( bool ignoreEmptyNodes = false, bool ignoreNodesWithOnlyQualifiers = false ) const;
		virtual void SetIndex( sizet currentIndex );
		virtual void SetIsQualifierNode( bool isQualifierNode );
		virtual bool APICALL IsQualifierNode() const;
		virtual sizet APICALL GetIndex() const;
		virtual void RegisterChange();
		virtual eNodeType APICALL GetParentNodeType() const;
		virtual eNodeType APICALL GetQualifierNodeType( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) const;
		virtual spISimpleNode APICALL ConvertToSimpleNode();
		virtual spIStructureNode APICALL ConvertToStructureNode();
		virtual spIArrayNode APICALL ConvertToArrayNode();
		virtual spIMetadata APICALL ConvertToMetadata();
		virtual void UnRegisterChange();
		virtual bool ValidateNameOrNameSpaceChangeForAChild( const spcIUTF8String & currentNameSpace, const spcIUTF8String & currentName, const spcIUTF8String & newNameSpace, const spcIUTF8String & newName );

		virtual void APICALL Acquire() const __NOTHROW__;
		virtual void APICALL Release() const __NOTHROW__;
		virtual void APICALL AcquireInternal() const __NOTHROW__;

		// functions base classes need to implement.
		virtual void APICALL ClearContents() = 0;
		virtual spINode APICALL CloneContents( bool ignoreEmptyNodes, bool ignoreNodesWithOnlyQualifiers, sizet qualifiersCount ) const = 0;
		virtual void SetQualifiers( const spIStructureNode & node );

	protected:
		void updateParentSharedPointer( bool calledFromRelease = false );
		virtual void resetChangesForChildren() const = 0;
		void CreateQualifierNode();
		virtual ~NodeImpl() __NOTHROW__ {}

		spIUTF8String						mNameSpace;
		spIUTF8String						mName;
		sizet								mIndex;
		pINode								mpParent;
		spINode								mspParent;
		spIStructureNode					mQualifiers;
		mutable atomic_sizet				mChangeCount;
		bool								mIsQualifierNode;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#endif  // __NodeImpl_h__
