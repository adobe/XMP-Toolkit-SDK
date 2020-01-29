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

#include <sstream>
#include <assert.h>

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
#include "XMPCore/ImplHeaders/MetadataConverterUtilsImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

namespace AdobeXMPCore_Int {
    
   /* AdobeXMPCore::spIMetadata APICALL IMetadataConverterUtils_I::convertXMPMetatoIMetadata( XMPMetaRef metaRef ) __NOTHROW__
    {
        XMPMeta* meta = ((XMPMeta*)(metaRef));
        return MetadataConverterUtilsImpl::ConvertOldDOMtoNewDOM(meta);
    }*/
    
    AdobeXMPCore::spIMetadata IMetadataConverterUtils_I::convertXMPMetatoIMetadata( XMPMeta* inpMeta) __NOTHROW__
    {
        return MetadataConverterUtilsImpl::ConvertOldDOMtoNewDOM(inpMeta);
    }
    
    XMPMetaRef IMetadataConverterUtils_I::convertIMetadatatoXMPMeta(AdobeXMPCore::pIMetadata_base iMeta ,const AdobeXMPCore::spcINameSpacePrefixMap & nameSpacePrefixMap) __NOTHROW__
    {
        XMP_OptionBits options = 0;
        return MetadataConverterUtilsImpl::ConvertNewDOMtoOldDOM(INode::MakeShared(iMeta), nameSpacePrefixMap,options);
    }
    
    XMPMetaRef IMetadataConverterUtils_I::convertIMetadatatoXMPMeta(const spINode & node,XMP_OptionBits options, const AdobeXMPCore::spcINameSpacePrefixMap & nameSpacePrefixMap) __NOTHROW__
    {
        return MetadataConverterUtilsImpl::ConvertNewDOMtoOldDOM(node, nameSpacePrefixMap,options);
    }

    
    void MetadataConverterUtilsImpl::CreateAndPopulateNode( const AdobeXMPCore::spINode & parentNode, XMP_Node * node, bool nodeIsQualifier /*= false*/ ) {
        const char * kItemName = "arrayItem";
        const char * kItemNameSpace = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
        
        XMP_StringPtr nameSpaceStr, nameStr;
        XMP_StringLen nameSpaceLen, nameLen;
        node->GetFullQualifiedName( &nameSpaceStr, &nameSpaceLen, &nameStr, &nameLen );
        if ( nameSpaceLen == 0 && nameLen == 0 ) { //true in case of array element
            nameSpaceStr = kItemNameSpace;
            nameStr = kItemName;
            nameSpaceLen = (XMP_Uns32)strlen( kItemNameSpace );
            nameLen = (XMP_Uns32)strlen( kItemName );
        }
        
        AdobeXMPCore::spINode spNode;
        if ( XMP_PropIsSimple( node->options ) ) {
            AdobeXMPCore::spISimpleNode spSimpleNode = AdobeXMPCore::ISimpleNode::CreateSimpleNode( nameSpaceStr, nameSpaceLen, nameStr, nameLen, node->value.c_str(), node->value.size() );
            spSimpleNode->SetURIType( XMP_OptionIsSet( node->options, kXMP_PropValueIsURI ) );
            spNode = spSimpleNode;
        } else if ( XMP_PropIsStruct( node->options ) ) {
            AdobeXMPCore::spIStructureNode spStructNode = AdobeXMPCore::IStructureNode::CreateStructureNode( nameSpaceStr, nameSpaceLen, nameStr, nameLen );
            for ( XMP_Uns64 index = 0, count = node->children.size(); index < count; index++ )
                CreateAndPopulateNode( spStructNode, node->children[ index ] );
            spNode = spStructNode;
        } else if ( XMP_PropIsArray( node->options ) ) {
            AdobeXMPCore::IArrayNode::eArrayForm arrayNodeForm = AdobeXMPCore::IArrayNode::kAFUnordered;
            if ( XMP_ArrayIsAlternate( node->options ) ) arrayNodeForm = AdobeXMPCore::IArrayNode::kAFAlternative;
            else if ( XMP_ArrayIsOrdered( node->options ) ) arrayNodeForm = AdobeXMPCore::IArrayNode::kAFOrdered;
            AdobeXMPCore::spIUTF8String nameSpaceUTF8Str( AdobeXMPCore_Int::IUTF8String_I::CreateUTF8String( nameSpaceStr, nameSpaceLen ) );
            AdobeXMPCore::spIUTF8String nameUTF8Str( AdobeXMPCore_Int::IUTF8String_I::CreateUTF8String( nameStr, nameLen ) );
            AdobeXMPCore::spIArrayNode spArrayNode = AdobeXMPCore_Int::IArrayNode_I::CreateArrayNode( nameSpaceUTF8Str, nameUTF8Str, arrayNodeForm );
            for ( XMP_Uns64 index = 0, count = node->children.size(); index < count; index++ )
                CreateAndPopulateNode( spArrayNode, node->children[ index ] );
            spNode = spArrayNode;
        }
        
        if ( spNode ) {
            // append qualifiers.
            if ( node->qualifiers.size() > 0 ) {
                for ( XMP_Uns64 index = 0, count = node->qualifiers.size(); index < count; index++ ) {
                    CreateAndPopulateNode( spNode, node->qualifiers[ index ], true );
                }
            }
            if ( nodeIsQualifier )
                parentNode->InsertQualifier( spNode );
            else
                parentNode->GetInterfacePointer< AdobeXMPCore::ICompositeNode_v1 >()->AppendNode( spNode );
        }
        
    }
    
    
    AdobeXMPCore::spIMetadata MetadataConverterUtilsImpl::ConvertOldDOMtoNewDOM(const XMPMeta* inOldMeta)
    {
        AdobeXMPCore::spIMetadata metadata = AdobeXMPCore::IMetadata::CreateMetadata();
        if ( inOldMeta ) {
            metadata->SetAboutURI( inOldMeta->tree.name.c_str(), inOldMeta->tree.name.size() );
            
            // all the top level children of this tree are actually top level namespace entries.
            // name begin the namespace string and value contains the prefix with colon.
            // actual nodes are below these top level children.
            for ( sizet index = 0, count = inOldMeta->tree.children.size(); index < count; ++index ) {
                XMP_Node * topLevelNode = inOldMeta->tree.children[ index ];
                for ( sizet innerIndex = 0, innerCount = topLevelNode->children.size(); innerIndex < innerCount; ++innerIndex ) {
                    CreateAndPopulateNode( metadata, topLevelNode->children[ innerIndex ] );
                }
            }
        }
        metadata->AcknowledgeChanges();
        return metadata;
    }
    
    spcIUTF8String MetadataConverterUtilsImpl::CreateQualifiedName( const spINode & node, const spcINameSpacePrefixMap_I & userSuppliedMap, spINameSpacePrefixMap_I & generatedMap ) {
        spIUTF8String qualName = IUTF8String_I::CreateUTF8String( NULL, AdobeXMPCommon::npos );
        spcIUTF8String nameSpace = node->GetNameSpace();
        static sizet count( 0 );
        
        spcIUTF8String prefixStr = userSuppliedMap->GetPrefix( node->GetNameSpace() );
        
        if ( !prefixStr && !generatedMap ) {
            generatedMap = MakeUncheckedSharedPointer( INameSpacePrefixMap_I::CreateNameSpacePrefixMap()->GetINameSpacePrefixMap_I(), __FILE__, __LINE__, true );
            count = 0;
        }
        else if (!prefixStr && generatedMap) {
            
            if (generatedMap->GetPrefix(node->GetNameSpace())) {
                
                prefixStr = generatedMap->GetPrefix(node->GetNameSpace());
            }
        }
        if ( !prefixStr ) {
            
            spIUTF8String autoGeneratedPrefix = IUTF8String_I::CreateUTF8String( NULL, AdobeXMPCommon::npos );
            do {
                autoGeneratedPrefix->clear();
                autoGeneratedPrefix->append( "ns", (sizet) 2 );
                //std::string numStr = std::to_string( ++count );
                std::ostringstream oss;
                oss << ++count;
                std::string numStr = oss.str();
                autoGeneratedPrefix->append( numStr.c_str(), numStr.size() );
            } while (generatedMap->IsPrefixPresent ( autoGeneratedPrefix->c_str ( ), autoGeneratedPrefix->size ( ) ));
            generatedMap->Insert( autoGeneratedPrefix->c_str(), autoGeneratedPrefix->size(), nameSpace->c_str(), nameSpace->size() );
            prefixStr = autoGeneratedPrefix;
        }
        
        qualName->append( prefixStr );
        qualName->append( ":", 1 );
        qualName->append( node->GetName() );
        return qualName;
    }

    
    XMP_Node * MetadataConverterUtilsImpl::AddQualifierNode( XMP_Node * xmpParent, const spINode & node, const char * value, const spcINameSpacePrefixMap_I & userSuppliedMap, spINameSpacePrefixMap_I & generatedMap ) {
        spcIUTF8String qualName = CreateQualifiedName( node, userSuppliedMap, generatedMap );
        XMP_StringPtr childName = qualName->c_str();
        
        static const char * kLanguageName = "lang";
        static const char * kTypeName = "type";
        
        const bool isLang = ( node->GetName()->compare( kLanguageName ) == 0 ) && ( node->GetNameSpace()->compare( kXMP_NS_XML ) == 0 );
        const bool isType = ( node->GetName()->compare( kTypeName ) == 0 ) && ( node->GetNameSpace()->compare( kXMP_NS_RDF ) == 0 );
        
        bool isArrayItem = node->IsArrayItem();
        
        XMP_OptionBits childOptions = 0;
        
        XMP_StringPtr nameSpaceStrPtr = node->GetNameSpace()->c_str();
        
        if ( isArrayItem ) {
            childName = kXMP_ArrayItemName;
        }
        
        XMP_Node * newQual = 0;
        
        newQual = new XMP_Node( xmpParent, childName, value, kXMP_PropIsQualifier );
        
        if ( !( isLang | isType ) ) {
            xmpParent->qualifiers.push_back( newQual );
        } else if ( isLang ) {
            if ( xmpParent->qualifiers.empty() ) {
                xmpParent->qualifiers.push_back( newQual );
            } else {
                xmpParent->qualifiers.insert( xmpParent->qualifiers.begin(), newQual );
            }
            xmpParent->options |= kXMP_PropHasLang;
        } else {
            XMP_Assert( isType );
            if ( xmpParent->qualifiers.empty() ) {
                xmpParent->qualifiers.push_back( newQual );
            } else {
                size_t offset = 0;
                if ( XMP_PropHasLang( xmpParent->options ) ) offset = 1;
                xmpParent->qualifiers.insert( xmpParent->qualifiers.begin() + offset, newQual );
            }
            xmpParent->options |= kXMP_PropHasType;
        }
        
        xmpParent->options |= kXMP_PropHasQualifiers;
        
        return newQual;
    }
    
    bool MetadataConverterUtilsImpl::FindPrefixFromUserSuppliedMap ( void * voidUserSuppliedMap, XMP_StringPtr nsURI, XMP_StringPtr * namespacePrefix, XMP_StringLen * prefixSize ) {
        if (voidUserSuppliedMap) {
            pcINameSpacePrefixMap userSuppliedMap = reinterpret_cast<pcINameSpacePrefixMap>( voidUserSuppliedMap );
            auto prefix = userSuppliedMap->GetPrefix ( nsURI, AdobeXMPCommon::npos );
            if (prefix) {
                *namespacePrefix = prefix->c_str ( );
                *prefixSize = (XMP_Uns32)prefix->size ( );
                return true;
            }
        }
        return false;
    }
    
    XMP_Node * MetadataConverterUtilsImpl::AddChildNode( XMP_Node * xmpParent, const spINode & node, const char * value, const spcINameSpacePrefixMap_I & userSuppliedMap, spINameSpacePrefixMap_I & generatedMap, bool isTopLevel ) {
        bool isArrayItem = node->IsArrayItem();
        if ( isTopLevel ) {
            isArrayItem = false;
        }
        
        XMP_OptionBits childOptions = 0;
        
        spcIUTF8String qualName = CreateQualifiedName( node, userSuppliedMap, generatedMap );
        
        XMP_StringPtr childName = qualName->c_str();
        XMP_StringPtr nameSpaceStrPtr = node->GetNameSpace()->c_str();
        
        if ( isTopLevel ) {
            // Lookup the schema node, adjust the XMP parent pointer.
            XMP_Assert( xmpParent->parent == 0 );	// Incoming parent must be the tree root.
            XMP_Node * schemaNode = FindSchemaNode( xmpParent, nameSpaceStrPtr, kXMP_CreateNodes, NULL, &FindPrefixFromUserSuppliedMap, generatedMap ? generatedMap->GetActualINameSpacePrefixMap() : NULL );
            if ( schemaNode->options & kXMP_NewImplicitNode ) schemaNode->options ^= kXMP_NewImplicitNode;	// Clear the implicit node bit.
            // *** Should use "opt &= ~flag" (no conditional), need runtime check for proper 32 bit code.
            xmpParent = schemaNode;
            
            // If this is an alias set the isAlias flag in the node and the hasAliases flag in the tree.
            XMP_StringPtr prefixForAlias( NULL );
            XMP_StringLen prefixLenForAlias( 0 );
            if ( sRegisteredNamespaces->GetPrefix( nameSpaceStrPtr, &prefixForAlias, &prefixLenForAlias ) && prefixForAlias && prefixLenForAlias > 0 ) {
                spIUTF8String childNameForAlias( IUTF8String_I::CreateUTF8String( NULL, npos ) );
                childNameForAlias->append( prefixForAlias, prefixLenForAlias )->append( node->GetName() );
                if ( sRegisteredAliasMap->find( childNameForAlias->c_str() ) != sRegisteredAliasMap->end() ) {
                    childOptions |= kXMP_PropIsAlias;
                    schemaNode->parent->options |= kXMP_PropHasAliases;
                }
            }
            
        }
        
        if ( isArrayItem ) {
            childName = kXMP_ArrayItemName;
        }
        
        // Add the new child to the XMP parent node.
        XMP_Node * newChild = new XMP_Node( xmpParent, childName, value, childOptions );
        xmpParent->children.push_back( newChild );
        
        return newChild;
    }
    
    XMP_Node * MetadataConverterUtilsImpl::HandleStructureNode( const spIStructureNode & structureNode, XMP_Node * parent, const spcINameSpacePrefixMap_I & userSuppliedMap, spINameSpacePrefixMap_I & generatedMap, bool isTopLevel, bool isQualifierNode ) {
        bool metadataNode = false;
        if ( isTopLevel ) {
            // check if it is a XMPMetadata node
            spIMetadata metadata = structureNode->ConvertToMetadata();
            if ( metadata ) {
                metadataNode = true;
                parent->name = metadata->GetAboutURI()->c_str();
            }
        }
        
        XMP_Node * newComposite = NULL;
        if ( !metadataNode ) {
            if ( isQualifierNode ) {
                newComposite = AddQualifierNode( parent, structureNode, "", userSuppliedMap, generatedMap );
            } else {
                newComposite = AddChildNode( parent, structureNode, "", userSuppliedMap, generatedMap, isTopLevel );
            }
        } else {
            newComposite = parent;
        }
        
        if ( newComposite == 0 ) return NULL;	// Ignore lower level errors.
        
        newComposite->options |= kXMP_PropValueIsStruct;
        
        // iterate over all the children
        spINodeIterator it = structureNode->Iterator();
        
        while ( it ) {
            spINode spNode = it->GetNode();
            if ( metadataNode )
                HandleNode( spNode, newComposite, userSuppliedMap, generatedMap, true, false );
            else
                HandleNode( spNode, newComposite, userSuppliedMap, generatedMap, false, false );
            it = it->Next();
        }
        
        return newComposite;
    }
    
    
    XMP_Node * MetadataConverterUtilsImpl::HandleArrayNode( const spIArrayNode & arrayNode, XMP_Node * parent, const spcINameSpacePrefixMap_I & userSuppliedMap, spINameSpacePrefixMap_I & generatedMap, bool isTopLevel, bool isQualifierNode ) {
        XMP_Node * newComposite = NULL;
        if ( isQualifierNode ) {
            newComposite = AddQualifierNode( parent, arrayNode, "", userSuppliedMap, generatedMap );
        } else {
            newComposite = AddChildNode( parent, arrayNode, "", userSuppliedMap, generatedMap, isTopLevel );
        }
        
        if ( newComposite == 0 ) return NULL;	// Ignore lower level errors.
        
        newComposite->options |= kXMP_PropValueIsArray;
        
        IArrayNode::eArrayForm arrayNodeForm = arrayNode->GetArrayForm();
        if ( arrayNodeForm == IArrayNode::kAFAlternative )
            newComposite->options |= kXMP_PropValueIsArray | kXMP_PropArrayIsOrdered | kXMP_PropArrayIsAlternate;
        else if ( arrayNodeForm == IArrayNode::kAFOrdered )
            newComposite->options |= kXMP_PropValueIsArray | kXMP_PropArrayIsOrdered;
        
        // iterate over all the children
        spINodeIterator it = arrayNode->Iterator();
        
        while ( it ) {
            spINode spNode = it->GetNode();
            HandleNode( spNode, newComposite, userSuppliedMap, generatedMap, false, false );
            it = it->Next();
        }
        return newComposite;
    }
    



    
    XMP_Node * MetadataConverterUtilsImpl::HandleSimpleNode( const spISimpleNode & simpleNode, XMP_Node * parent, const spcINameSpacePrefixMap_I & userSuppliedMap, spINameSpacePrefixMap_I & generatedMap, bool isTopLevel, bool isQualifierNode ) {
        XMP_Node * node = NULL;
        if ( isQualifierNode ) {
            node = AddQualifierNode( parent, simpleNode, simpleNode->GetValue()->c_str(), userSuppliedMap, generatedMap );
        } else {
            node = AddChildNode( parent, simpleNode, simpleNode->GetValue()->c_str(), userSuppliedMap, generatedMap, isTopLevel );
        }
        if ( simpleNode->IsURIType() )
            node->options |= kXMP_PropValueIsURI;
        return node;
    }
    
    void MetadataConverterUtilsImpl::HandleNode( const spINode & node, XMP_Node * parent, const spcINameSpacePrefixMap_I & userSuppliedMap, spINameSpacePrefixMap_I & generatedMap, bool isTopLevel, bool isQualifierNode ) {
        XMP_Node * nodeCreated = NULL;
        if (!node) return;
        switch ( node->GetNodeType() ) {
            case INode::kNTSimple:
                nodeCreated = HandleSimpleNode( node->ConvertToSimpleNode(), parent, userSuppliedMap, generatedMap, isTopLevel, isQualifierNode );
                break;
                
            case INode::kNTStructure:
                nodeCreated = HandleStructureNode( node->ConvertToStructureNode(), parent, userSuppliedMap, generatedMap, isTopLevel, isQualifierNode );
                break;
                
            case INode::kNTArray:
                nodeCreated = HandleArrayNode( node->ConvertToArrayNode(), parent, userSuppliedMap, generatedMap, isTopLevel, isQualifierNode );
                break;
                
            default:
                break;
        }
        
        if ( node->HasQualifiers() && nodeCreated ) {
            spINodeIterator it = node->QualifiersIterator();
            while ( it ) {
                spINode spNode = it->GetNode();
                HandleNode( spNode, nodeCreated, userSuppliedMap, generatedMap, false, true );
                it = it->Next();
            }
        }
    }
    
    XMPMetaRef MetadataConverterUtilsImpl::ConvertNewDOMtoOldDOM(const AdobeXMPCore::spINode node, const AdobeXMPCore::spcINameSpacePrefixMap & nameSpacePrefixMap, XMP_OptionBits& options)
    {
        XMPMeta* xmpObj = new XMPMeta();
        spINameSpacePrefixMap_I genereatedMap;
        
        spcINameSpacePrefixMap mergedMap = INameSpacePrefixMap::GetDefaultNameSpacePrefixMap();
        if ( nameSpacePrefixMap ) {
            spINameSpacePrefixMap newMergedMap = mergedMap->Clone();
            newMergedMap->GetINameSpacePrefixMap_I()->Merge( nameSpacePrefixMap );
            mergedMap = newMergedMap;
        }
        spINameSpacePrefixMap_I userSuppliedMap( MakeUncheckedSharedPointer( const_pointer_cast< INameSpacePrefixMap >( mergedMap )->GetINameSpacePrefixMap_I(), __FILE__, __LINE__, true ) );
        
        // TODO:meta->SetErrorCallback()
        HandleNode( node, &xmpObj->tree, userSuppliedMap, genereatedMap, true, false );
        
        NormalizeDCArrays( &( xmpObj->tree ) );
        if ( xmpObj->tree.options & kXMP_PropHasAliases ) MoveExplicitAliases( &xmpObj->tree, options, xmpObj->errorCallback );
        TouchUpDataModel( xmpObj, xmpObj->errorCallback );
        
        // Delete empty schema nodes. Do this last, other cleanup can make empty schema.
        size_t schemaNum = 0;
        while ( schemaNum < xmpObj->tree.children.size() ) {
            XMP_Node * currSchema = xmpObj->tree.children[ schemaNum ];
            if ( currSchema->children.size() > 0 ) {
                ++schemaNum;
            } else {
                delete xmpObj->tree.children[ schemaNum ];	// ! Delete the schema node itself.
                xmpObj->tree.children.erase( xmpObj->tree.children.begin() + schemaNum );
            }
        }
        
        return XMPMetaRef ( xmpObj );
    }
    
}

#if BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB

#include "client-glue/TXMPMeta.incl_cpp"

namespace AdobeXMPCore {
    using namespace AdobeXMPCore_Int;
    
    spIMetadataConverterUtils IMetadataConverterUtils_v1::MakeShared( pIMetadataConverterUtils_base ptr ) {
        if ( !ptr ) return spIMetadataConverterUtils();
        pIMetadataConverterUtils p = IMetadataConverterUtils::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IMetadataConverterUtils >() : ptr;
        return MakeUncheckedSharedPointer( p, __FILE__, __LINE__, false );
    }
    
    AdobeXMPCore::spIMetadata IMetadataConverterUtils::ConvertXMPMetatoIMetadata(const SXMPMeta* inOldXMP){
		if(!inOldXMP) return AdobeXMPCore::spIMetadata();
        XMPMeta* meta = (XMPMeta*)(inOldXMP->xmpRef);
        return MetadataConverterUtilsImpl::ConvertOldDOMtoNewDOM(meta);
    }
    
    SXMPMeta IMetadataConverterUtils::ConvertIMetadatatoXMPMeta(AdobeXMPCore::spIMetadata inNewXMP)
    {
        XMP_OptionBits optionBits = 0;
        return MetadataConverterUtilsImpl::ConvertNewDOMtoOldDOM(inNewXMP, AdobeXMPCore::spcINameSpacePrefixMap(),optionBits);
    }
}
#endif  // BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
