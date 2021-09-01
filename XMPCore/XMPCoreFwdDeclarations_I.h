#ifndef XMPCoreFwdDeclarations_I_h__
#define XMPCoreFwdDeclarations_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCore/XMPCoreDefines_I.h"
#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPCommon/XMPCommonFwdDeclarations_I.h"

namespace AdobeXMPCore_Int {
	using namespace AdobeXMPCore;
	using namespace AdobeXMPCommon;
	using namespace AdobeXMPCommon_Int;

	// INameSpacePrefixMap
	typedef shared_ptr< INameSpacePrefixMap_I >											spINameSpacePrefixMap_I;
	typedef shared_ptr< const INameSpacePrefixMap_I >									spcINameSpacePrefixMap_I;

	// IPath
	typedef shared_ptr< IPath_I >														spIPath_I;
	typedef shared_ptr< const IPath_I >													spcIPath_I;

	// IPathSegment
	typedef shared_ptr< IPathSegment_I >												spIPathSegment_I;
	typedef shared_ptr< const IPathSegment_I >											spcIPathSegment_I;

	// INode
	typedef shared_ptr< INode_I >														spINode_I;
	typedef shared_ptr< const INode_I >													spcINode_I;

	// ISimpleNode
	typedef shared_ptr< ISimpleNode_I >													spISimpleNode_I;
	typedef shared_ptr< const ISimpleNode_I >											spcISimpleNode_I;

	// ICompositeNode
	typedef shared_ptr< ICompositeNode_I >												spICompositeNode_I;
	typedef shared_ptr< const ICompositeNode_I >										spcICompositeNode_I;

	// IStructureNode
	typedef shared_ptr< IStructureNode_I >												spIStructureNode_I;
	typedef shared_ptr< const IStructureNode_I >										spcIStructureNode_I;

	// IArrayNode
	typedef shared_ptr< IArrayNode_I >													spIArrayNode_I;
	typedef shared_ptr< const IArrayNode_I >											spcIArrayNode_I;

	// INodeIterator
	typedef shared_ptr< INodeIterator_I >												spINodeIterator_I;
	typedef shared_ptr< const INodeIterator_I >											spcINodeIterator_I;

	// IMetadata
	typedef shared_ptr< IMetadata_I >													spIXMPMetadata_I;
	typedef shared_ptr< const IMetadata_I >												spcIXMPMetadata_I;

	// IDOMParser
	typedef shared_ptr< IDOMParser_I >													spIIDOMParser_I;
	typedef shared_ptr< const IDOMParser_I >											spcIIDOMParser_I;

	// IDOMSerializer
	typedef shared_ptr< IDOMSerializer_I >												spIDOMSerializer_I;
	typedef shared_ptr< const IDOMSerializer_I >										spcIDOMSerializer_I;

	// IDOMImplementationRegistry
	typedef shared_ptr< IDOMImplementationRegistry_I >									spIDOMImplementationRegistry_I;
	typedef shared_ptr< const IDOMImplementationRegistry_I >							spcIDOMImplementationRegistry_I;

	// ICoreObjectFactory
	typedef shared_ptr< ICoreObjectFactory_I >											spICoreObjectFactory_I;
	typedef shared_ptr< const ICoreObjectFactory_I >									spcICoreObjectFactory_I;

	// ICoreConfigurationManager
	typedef shared_ptr< ICoreConfigurationManager_I >									spICoreConfigurationManager_I;
	typedef shared_ptr< const ICoreConfigurationManager_I >								spcICoreConfigurationManager_I;
}
#endif // XMPCoreFwdDeclarations_I_h__
