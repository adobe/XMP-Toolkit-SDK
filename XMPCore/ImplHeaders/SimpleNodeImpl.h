#ifndef __SimpleNodeImpl_h__
#define __SimpleNodeImpl_h__ 1

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

#include "XMPCore/Interfaces/ISimpleNode_I.h"
#include "XMPCore/ImplHeaders/NodeImpl.h"


#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {

	class SimpleNodeImpl
		: public virtual ISimpleNode_I
		, public virtual NodeImpl
	{
	public:
		SimpleNodeImpl( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, const char * value, sizet valueLength );
		virtual spcIUTF8String APICALL GetValue() const;
		virtual void APICALL SetValue( const char * value, sizet valueLength );
		virtual bool APICALL IsURIType() const;
		virtual void APICALL SetURIType( bool isURI );
		virtual eNodeType APICALL GetNodeType() const;

		virtual bool APICALL HasContent() const;
		virtual void APICALL ClearContents();
		virtual spINode APICALL CloneContents( bool ignoreEmptyNodes, bool ignoreNodesWithOnlyQualifiers, sizet qualifiersCount ) const;
		virtual spISimpleNode APICALL ConvertToSimpleNode();

	protected:
		virtual ~SimpleNodeImpl() __NOTHROW__ {}

		virtual void resetChangesForChildren() const;

		spIUTF8String					mValue;
		bool							mIsURIType;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif

#endif  // __SimpleNodeImpl_h__
