#ifndef MetadataConverterUtilsImpl_h__
#define MetadataConverterUtilsImpl_h__ 1

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

#include "XMPCore/Interfaces/IMetadataConverterUtils_I.h"
#include "XMPCommon/Utilities/TAllocator.h"
#include "XMPCommon/BaseClasses/MemoryManagedObject.h"
#include "XMPCommon/ImplHeaders/SharedObjectImpl.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCommon/Utilities/AutoSharedLock.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"
#include "XMPCore/Interfaces/ISimpleNode.h"
#include "XMPCore/Interfaces/IStructureNode.h"
#include "XMPCore/Interfaces/IArrayNode.h"
#include "XMPCore/Interfaces/IArrayNode_I.h"
#include "XMPCore/Interfaces/IMetadata.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap_I.h"
#include "XMPCore/Interfaces/INodeIterator.h"

#include "XMPCore/source/XMPMeta.hpp"

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

namespace AdobeXMPCore_Int {
    
    class MetadataConverterUtilsImpl
    : public virtual IMetadataConverterUtils_I
    , public virtual SharedObjectImpl
    , public virtual MemoryManagedObject
    {
    public:
        MetadataConverterUtilsImpl();
        static AdobeXMPCore::spIMetadata ConvertOldDOMtoNewDOM(const XMPMeta* inOldMeta);
        static XMPMetaRef ConvertNewDOMtoOldDOM(const AdobeXMPCore::spINode node, const AdobeXMPCore::spcINameSpacePrefixMap & nameSpacePrefixMap ,XMP_OptionBits& options);
        
    protected:
        virtual ~MetadataConverterUtilsImpl() __NOTHROW__ {}
        
    private:
        static void CreateAndPopulateNode( const AdobeXMPCore::spINode & parentNode, XMP_Node * node, bool nodeIsQualifier = false );
        static spcIUTF8String CreateQualifiedName( const spINode & node, const spcINameSpacePrefixMap_I & userSuppliedMap, spINameSpacePrefixMap_I & generatedMap );
        static XMP_Node * AddQualifierNode( XMP_Node * xmpParent, const spINode & node, const char * value, const spcINameSpacePrefixMap_I & userSuppliedMap, spINameSpacePrefixMap_I & generatedMap ) ;
        static bool FindPrefixFromUserSuppliedMap ( void * voidUserSuppliedMap, XMP_StringPtr nsURI, XMP_StringPtr * namespacePrefix, XMP_StringLen * prefixSize ) ;
        static XMP_Node * AddChildNode( XMP_Node * xmpParent, const spINode & node, const char * value, const spcINameSpacePrefixMap_I & userSuppliedMap, spINameSpacePrefixMap_I & generatedMap, bool isTopLevel );
        static XMP_Node * HandleStructureNode( const spIStructureNode & structureNode, XMP_Node * parent, const spcINameSpacePrefixMap_I & userSuppliedMap, spINameSpacePrefixMap_I & generatedMap, bool isTopLevel, bool isQualifierNode );
        static XMP_Node * HandleArrayNode( const spIArrayNode & arrayNode, XMP_Node * parent, const spcINameSpacePrefixMap_I & userSuppliedMap, spINameSpacePrefixMap_I & generatedMap, bool isTopLevel, bool isQualifierNode );
        static XMP_Node * HandleSimpleNode( const spISimpleNode & simpleNode, XMP_Node * parent, const spcINameSpacePrefixMap_I & userSuppliedMap, spINameSpacePrefixMap_I & generatedMap, bool isTopLevel, bool isQualifierNode );
        static void HandleNode( const spINode & node, XMP_Node * parent, const spcINameSpacePrefixMap_I & userSuppliedMap, spINameSpacePrefixMap_I & generatedMap, bool isTopLevel, bool isQualifierNode );
        
    #ifdef FRIEND_CLASS_DECLARATION
        FRIEND_CLASS_DECLARATION();
    #endif
        REQ_FRIEND_CLASS_DECLARATION();
    };
}

#if XMP_WinBuild
	#pragma warning( pop )
#endif

#endif /*MetadataConverterUtilsImpl_h__*/
