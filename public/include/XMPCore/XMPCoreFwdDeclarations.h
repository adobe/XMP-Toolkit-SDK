#ifndef XMPCoreFwdDeclarations_h__
#define XMPCoreFwdDeclarations_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/XMPCoreDefines.h"
#include "XMPCommon/XMPCommonFwdDeclarations.h"
#include "XMPCore/XMPCoreLatestInterfaceVersions.h"

namespace AdobeXMPCore {
	using namespace AdobeXMPCommon;

	// INameSpacePrefixMap
	class INameSpacePrefixMap_v1;
	typedef INameSpacePrefixMap_v1														INameSpacePrefixMap_base;
	typedef INameSpacePrefixMap_v1 *													pINameSpacePrefixMap_base;
	typedef const INameSpacePrefixMap_v1 *												pcINameSpacePrefixMap_base;
	typedef BASE_CLASS( INameSpacePrefixMap, INAMESPACEPREFIXMAP_VERSION )				INameSpacePrefixMap;
	typedef INameSpacePrefixMap *														pINameSpacePrefixMap;
	typedef const INameSpacePrefixMap *													pcINameSpacePrefixMap;
	typedef shared_ptr< INameSpacePrefixMap >											spINameSpacePrefixMap;
	typedef shared_ptr< const INameSpacePrefixMap >										spcINameSpacePrefixMap;
	static const uint64 kINameSpacePrefixMapID											( 0x634e5350724d6170 /* cNSPrMap */ );

	// IPathSegment
	class IPathSegment_v1;
	typedef IPathSegment_v1																IPathSegment_base;
	typedef IPathSegment_v1 *															pIPathSegment_base;
	typedef const IPathSegment_v1 *														pcIPathSegment_base;
	typedef BASE_CLASS( IPathSegment, IPATHSEGMENT_VERSION )							IPathSegment;
	typedef IPathSegment *																pIPathSegment;
	typedef const IPathSegment *														pcIPathSegment;
	typedef shared_ptr< IPathSegment >													spIPathSegment;
	typedef shared_ptr< const IPathSegment >											spcIPathSegment;
	static const uint64 kIPathSegmentID													( 0x6350617468536567 /* cPathSeg */ );

	// IPath
	class IPath_v1;
	typedef IPath_v1																	IPath_base;
	typedef IPath_v1 *																	pIPath_base;
	typedef const IPath_v1 *															pcIPath_base;
	typedef BASE_CLASS( IPath, IPATH_VERSION )											IPath;
	typedef IPath *																		pIPath;
	typedef const IPath *																pcIPath;
	typedef shared_ptr< IPath >															spIPath;
	typedef shared_ptr< const IPath >													spcIPath;
	static const uint64 kIPathID														( 0x6350617468202020 /* cPath    */ );

	// INode
	class INode_v1;
	typedef INode_v1																	INode_base;
	typedef INode_v1 *																	pINode_base;
	typedef const INode_v1 *															pcINode_base;
	typedef BASE_CLASS( INode, INODE_VERSION )											INode;
	typedef INode *																		pINode;
	typedef const INode *																pcINode;
	typedef shared_ptr< INode >															spINode;
	typedef shared_ptr< const INode >													spcINode;
	static const uint64 kINodeID														( 0x634e6f6465202020 /* cNode    */ );

	// ISimpleNode
	class ISimpleNode_v1;
	typedef ISimpleNode_v1																ISimpleNode_base;
	typedef ISimpleNode_v1 *															pISimpleNode_base;
	typedef const ISimpleNode_v1 *														pcISimpleNode_base;
	typedef BASE_CLASS( ISimpleNode, ISIMPLENODE_VERSION )								ISimpleNode;
	typedef ISimpleNode *																pISimpleNode;
	typedef const ISimpleNode *															pcISimpleNode;
	typedef shared_ptr< ISimpleNode >													spISimpleNode;
	typedef shared_ptr< const ISimpleNode >												spcISimpleNode;
	static const uint64 kISimpleNodeID													( 0x63536d6c4e6f6465 /* cSmlNode */ );

	// ICompositeNode
	class ICompositeNode_v1;
	typedef ICompositeNode_v1															ICompositeNode_base;
	typedef ICompositeNode_v1 *															pICompositeNode_base;
	typedef const ICompositeNode_v1 *													pcICompositeNode_base;
	typedef BASE_CLASS( ICompositeNode, ICOMPOSITENODE_VERSION )						ICompositeNode;
	typedef ICompositeNode *															pICompositeNode;
	typedef const ICompositeNode *														pcICompositeNode;
	typedef shared_ptr< ICompositeNode >												spICompositeNode;
	typedef shared_ptr< const ICompositeNode >											spcICompositeNode;
	static const uint64 kICompositeNodeID												( 0x63436d704e6f6465 /* cCmpNode */ );

	// IStructureNode
	class IStructureNode_v1;
	typedef IStructureNode_v1															IStructureNode_base;
	typedef IStructureNode_v1 *															pIStructureNode_base;
	typedef const IStructureNode_v1 *													pcIStructureNode_base;
	typedef BASE_CLASS( IStructureNode, ISTRUCTURENODE_VERSION )						IStructureNode;
	typedef IStructureNode *															pIStructureNode;
	typedef const IStructureNode *														pcIStructureNode;
	typedef shared_ptr< IStructureNode >												spIStructureNode;
	typedef shared_ptr< const IStructureNode >											spcIStructureNode;
	static const uint64 kIStructureNodeID												( 0x635374724e6f6465 /* cStrNode */ );

	// IArrayNode
	class IArrayNode_v1;
	typedef IArrayNode_v1																IArrayNode_base;
	typedef IArrayNode_v1 *																pIArrayNode_base;
	typedef const IArrayNode_v1 *														pcIArrayNode_base;
	typedef BASE_CLASS( IArrayNode, IARRAYNODE_VERSION )								IArrayNode;
	typedef IArrayNode *																pIArrayNode;
	typedef const IArrayNode *															pcIArrayNode;
	typedef shared_ptr< IArrayNode >													spIArrayNode;
	typedef shared_ptr< const IArrayNode >												spcIArrayNode;
	static const uint64 kIArrayNodeID													( 0x634172724e6f6465 /* cArrNode */ );

	// INodeIterator
	class INodeIterator_v1;
	typedef INodeIterator_v1															INodeIterator_base;
	typedef INodeIterator_v1 *															pINodeIterator_base;
	typedef const INodeIterator_v1 *													pcINodeIterator_base;
	typedef BASE_CLASS(INodeIterator, INODEITERATOR_VERSION)							INodeIterator;
	typedef INodeIterator *																pINodeIterator;
	typedef const INodeIterator *														pcINodeIterator;
	typedef shared_ptr< INodeIterator >													spINodeIterator;
	typedef shared_ptr< const INodeIterator >											spcINodeIterator;
	static const uint64	kINodeIteratorID												(0x634e6f6465497420 /* cNodeIt */);

	// IMetadata
	class IMetadata_v1;
	typedef IMetadata_v1																IMetadata_base;
	typedef IMetadata_v1 *																pIMetadata_base;
	typedef const IMetadata_v1 *														pcIMetadata_base;
	typedef BASE_CLASS( IMetadata, IMETADATA_VERSION )									IMetadata;
	typedef IMetadata *																	pIMetadata;
	typedef const IMetadata *															pcIMetadata;
	typedef shared_ptr< IMetadata >														spIMetadata;
	typedef shared_ptr< const IMetadata >												spcIMetadata;
	static const uint64 kIMetadataID													( 0x634d657461646174 /* cMetadat */ );

	// IClientDOMParser
	class IClientDOMParser_v1;
	typedef IClientDOMParser_v1															IClientDOMParser_base;
	typedef IClientDOMParser_v1 *														pIClientDOMParser_base;
	typedef const IClientDOMParser_v1 *													pcIClientDOMParser_base;
	typedef BASE_CLASS( IClientDOMParser, ICLIENTDOMPARSER_VERSION )					IClientDOMParser;
	typedef IClientDOMParser *															pIClientDOMParser;
	typedef const IClientDOMParser *													pcIClientDOMParser;

	// IClientDOMSerializer
	class IClientDOMSerializer_v1;
	typedef IClientDOMSerializer_v1														IClientDOMSerializer_base;
	typedef IClientDOMSerializer_v1 *													pIClientDOMSerializer_base;
	typedef const IClientDOMSerializer_v1 *												pcIClientDOMSerializer_base;
	typedef BASE_CLASS( IClientDOMSerializer, ICLIENTDOMSERIALIZER_VERSION )			IClientDOMSerializer;
	typedef IClientDOMSerializer *														pIClientDOMSerializer;
	typedef const IClientDOMSerializer *												pcIClientDOMSerializer;
	
	// IDOMParser
	class IDOMParser_v1;
	typedef IDOMParser_v1																IDOMParser_base;
	typedef IDOMParser_v1 *																pIDOMParser_base;
	typedef const IDOMParser_v1 *														pcIDOMParser_base;
	typedef BASE_CLASS( IDOMParser, IDOMPARSER_VERSION )								IDOMParser;
	typedef IDOMParser *																pIDOMParser;
	typedef const IDOMParser *															pcIDOMParser;
	typedef shared_ptr< IDOMParser >													spIDOMParser;
	typedef shared_ptr< const IDOMParser >												spcIDOMParser;
	static const uint64 kIDOMParserID													( 0x63444f4d50727372 /* cDOMPrsr */ );

	// IDOMSerializer
	class IDOMSerializer_v1;
	typedef IDOMSerializer_v1															IDOMSerializer_base;
	typedef IDOMSerializer_v1 *															pIDOMSerializer_base;
	typedef const IDOMSerializer_v1 *													pcIDOMSerializer_base;
	typedef BASE_CLASS( IDOMSerializer, IDOMSERIALIZER_VERSION )						IDOMSerializer;
	typedef IDOMSerializer *															pIDOMSerializer;
	typedef const IDOMSerializer *														pcIDOMSerializer;
	typedef shared_ptr< IDOMSerializer >												spIDOMSerializer;
	typedef shared_ptr< const IDOMSerializer >											spcIDOMSerializer;
	static const uint64 kIDOMSerializerID												( 0x63444f4d53726c7a /* cDOMSrlz */ );

	// IDOMImplementationRegistry
	class IDOMImplementationRegistry_v1;
	typedef IDOMImplementationRegistry_v1												IDOMImplementationRegistry_base;
	typedef IDOMImplementationRegistry_v1 *												pIDOMImplementationRegistry_base;
	typedef const IDOMImplementationRegistry_v1 *										pcIDOMImplementationRegistry_base;
	typedef BASE_CLASS( IDOMImplementationRegistry, IDOMIMPLEMENTATIONREGISTRY_VERSION)	IDOMImplementationRegistry;
	typedef IDOMImplementationRegistry *												pIDOMImplementationRegistry;
	typedef const IDOMImplementationRegistry *											pcIDOMImplementationRegistry;
	typedef shared_ptr< IDOMImplementationRegistry >									spIDOMImplementationRegistry;
	typedef shared_ptr< const IDOMImplementationRegistry >								spcIDOMImplementationRegistry;
	static const uint64 kIDOMImplementationRegistryID									( 0x63444f4d52677374 /* cDOMRgst */ );

	// ICoreObjectFactory
	class ICoreObjectFactory_v1;
	typedef ICoreObjectFactory_v1														ICoreObjectFactory_base;
	typedef ICoreObjectFactory_v1 *														pICoreObjectFactory_base;
	typedef const ICoreObjectFactory_v1 *												pcICoreObjectFactory_base;
	typedef BASE_CLASS( ICoreObjectFactory, ICOREOBJECTFACTORY_VERSION )				ICoreObjectFactory;
	typedef ICoreObjectFactory *														pICoreObjectFactory;
	typedef const ICoreObjectFactory *													pcICoreObjectFactory;
	static const uint64 kICoreObjectFactoryID											( 0x634f626a46616374 /* cObjFact */ );

	// ICoreConfigurationManager
	class ICoreConfigurationManager_v1;
	typedef ICoreConfigurationManager_v1												ICoreConfigurationManager_base;
	typedef ICoreConfigurationManager_v1 *												pICoreConfigurationManager_base;
	typedef const ICoreConfigurationManager_v1 *										pcICoreConfigurationManager_base;
	typedef BASE_CLASS( ICoreConfigurationManager, ICORECONFIGURATIONMANAGER_VERSION )	ICoreConfigurationManager;
	typedef ICoreConfigurationManager *													pICoreConfigurationManager;
	typedef const ICoreConfigurationManager *											pcICoreConfigurationManager;
	typedef shared_ptr< ICoreConfigurationManager >										spICoreConfigurationManager;
	typedef shared_ptr< const ICoreConfigurationManager >								spcICoreConfigurationManager;	
	static const uint64 kICoreConfigurationManagerID									( 0x63436f6e664d6772 /* cConfMgr */ );

    // IMetadataConverterUtils
    class IMetadataConverterUtils_v1;
    typedef IMetadataConverterUtils_v1	IMetadataConverterUtils_base;
    typedef IMetadataConverterUtils_v1 *																pIMetadataConverterUtils_base;
    typedef const IMetadataConverterUtils_v1 *														pcIMetadataConverterUtils_base;
    typedef BASE_CLASS( IMetadataConverterUtils, IMETADATACONVERTERUTILS_VERSION )									IMetadataConverterUtils;
    typedef IMetadataConverterUtils *																	pIMetadataConverterUtils;
    typedef const IMetadataConverterUtils *															pcIMetadataConverterUtils;
    typedef shared_ptr< IMetadataConverterUtils >														spIMetadataConverterUtils;
    typedef shared_ptr< const IMetadataConverterUtils >												spcIMetadataConverterUtils;
    static const uint64 kIMetadataConverterUtilsID													( 0x63436F6E5574696C /* cConUtil */ );

}

namespace AdobeXMPCore_Int {

	// INameSpacePrefixMap_I
	class INameSpacePrefixMap_I;
	typedef INameSpacePrefixMap_I *														pINameSpacePrefixMap_I;
	typedef const INameSpacePrefixMap_I *												pcINameSpacePrefixMap_I;

	// IPathSegment_I
	class IPathSegment_I;
	typedef IPathSegment_I *															pIPathSegment_I;
	typedef const IPathSegment_I *														pcIPathSegment_I;

	// IPath_I
	class IPath_I;
	typedef IPath_I *																	pIPath_I;
	typedef const IPath_I *																pcIPath_I;

	// INode_I
	class INode_I;
	typedef INode_I *																	pINode_I;
	typedef const INode_I *																pcINode_I;

	// ISimpleNode_I
	class ISimpleNode_I;
	typedef ISimpleNode_I *																pISimpleNode_I;
	typedef const ISimpleNode_I *														pcISimpleNode_I;

	// ICompositeNode_I
	class ICompositeNode_I;
	typedef ICompositeNode_I *															pICompositeNode_I;
	typedef const ICompositeNode_I *													pcICompositeNode_I;

	// IStructureNode_I
	class IStructureNode_I;
	typedef IStructureNode_I *															pIStructureNode_I;
	typedef const IStructureNode_I *													pcIStructureNode_I;

	// IArrayNode_I
	class IArrayNode_I;
	typedef IArrayNode_I *																pIArrayNode_I;
	typedef const IArrayNode_I *														pcIArrayNode_I;

	// INodeIterator_I
	class INodeIterator_I;
	typedef INodeIterator_I *															pINodeIterator_I;
	typedef const INodeIterator_I *														pcINodeIterator_I;

	// IMetadata_I
	class IMetadata_I;
	typedef IMetadata_I *																pIMetadata_I;
	typedef const IMetadata_I *															pcIMetadata_I;

	// IClientDOMParser_I
	class IClientDOMParser_I;
	typedef IClientDOMParser_I *														pIClientDOMParser_I;
	typedef const IClientDOMParser_I *													pcIClientDOMParser_I;

	// IClientDOMSerializer_I
	class IClientDOMSerializer_I;
	typedef IClientDOMSerializer_I *													pIClientDOMSerializer_I;
	typedef const IClientDOMSerializer_I *												pcIClientDOMSerializer_I;

	// IDOMParser_I
	class IDOMParser_I;
	typedef IDOMParser_I *																pIDOMParser_I;
	typedef const IDOMParser_I *														pcIDOMParser_I;

	// IDOMSerializer_I
	class IDOMSerializer_I;
	typedef IDOMSerializer_I *															pIDOMSerializer_I;
	typedef const IDOMSerializer_I *													pcIDOMSerializer_I;

	// IDOMImplementationRegistry_I
	class IDOMImplementationRegistry_I;
	typedef IDOMImplementationRegistry_I *												pIDOMImplementationRegistry_I;
	typedef const IDOMImplementationRegistry_I *										pcIDOMImplementationRegistry_I;

	// ICoreObjectFactory_I
	class ICoreObjectFactory_I;
	typedef ICoreObjectFactory_I *														pICoreObjectFactory_I;
	typedef const ICoreObjectFactory_I *												pcICoreObjectFactory_I;

	// ICoreConfigurationManager_I
	class ICoreConfigurationManager_I;
	typedef ICoreConfigurationManager_I *												pICoreConfigurationManager_I;
	typedef const ICoreConfigurationManager_I *											pcICoreConfigurationManager_I;
    
    // IMetadataConverterUtils_I
    class IMetadataConverterUtils_I;
    typedef IMetadataConverterUtils_I *																pIMetadataConverterUtils_I;
    typedef const IMetadataConverterUtils_I *															pcIMetadataConverterUtils_I;
}

#endif // XMPCoreFwdDeclarations_h__

