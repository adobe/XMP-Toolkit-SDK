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

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/MetadataImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED
#include "XMPCommon/Utilities/UTF8String.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCommon/Utilities/AutoSharedLock.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap_I.h"
#include "XMPCore/Interfaces/IArrayNode_I.h"
#include "XMPCore/Interfaces/INodeIterator_I.h"
#include "XMPCore/Interfaces/ISimpleNode_I.h"
#include "XMPCore/source/XMPCore_Impl.hpp"


#include <assert.h>

namespace AdobeXMPCore_Int {
	static const char * kMetadataNameSpace( "http://metadata" );
	static const AdobeXMPCommon::sizet kMetadataNameSpaceLength( 15 );
	static const char * kMetadataLocalName( "_metadata_" );
	static const AdobeXMPCommon::sizet kMetadataLocalNameLength( 10 );

	MetadataImpl::MetadataImpl()
		: StructureNodeImpl( kMetadataNameSpace, kMetadataNameSpaceLength, kMetadataLocalName, kMetadataLocalNameLength )
		, NodeImpl( kMetadataNameSpace, kMetadataNameSpaceLength, kMetadataLocalName, kMetadataLocalNameLength )
		, mAboutURI( IUTF8String_I::CreateUTF8String() )
		, mSupportAliases( false ) { }

	spcIUTF8String APICALL MetadataImpl::GetAboutURI() const {
		AutoSharedLock lock( mSharedMutex );
		return mAboutURI;
	}

	void APICALL MetadataImpl::SetAboutURI( const char * uri, sizet uriLength ) __NOTHROW__ {
		AutoSharedLock lock( mSharedMutex, true );
		mAboutURI->assign( uri, uriLength );
	}

	spcIUTF8String APICALL MetadataImpl::GetName() const {
		return IUTF8String_I::CreateUTF8String();
	}

	spcIUTF8String APICALL MetadataImpl::GetNameSpace() const {
		return IUTF8String_I::CreateUTF8String();
	}

	spINode APICALL MetadataImpl::GetParent() {
		return spINode();
	}

	INode::eNodeType APICALL MetadataImpl::GetParentNodeType() const {
		return INode::kNTNone;
	}

	void APICALL MetadataImpl::SetName( const char * name, sizet nameLength ) {
		return;
	}

	void APICALL MetadataImpl::SetNameSpace( const char * nameSpace, sizet nameSpaceLength ) {
		return;
	}

	void APICALL MetadataImpl::ClearContents() {
		StructureNodeImpl::ClearContents();
		{
			AutoSharedLock lock( mSharedMutex, true );
			mAboutURI->clear();
		}
	}

	spINode APICALL MetadataImpl::CloneContents( bool ignoreEmptyNodes, bool ignoreNodesWithOnlyQualifiers, sizet qualifiersCount ) const {
		spIMetadata newNode = IMetadata::CreateMetadata();
		auto endIt = mChildrenMap.end();
		for ( auto it = mChildrenMap.begin(); it != endIt; ++it ) {
			spINode childNode = it->second->Clone( ignoreEmptyNodes, ignoreNodesWithOnlyQualifiers );
			if ( childNode ) {
				newNode->AppendNode( childNode );
			}
		}
		newNode->SetAboutURI( mAboutURI->c_str(), mAboutURI->size() );
		return newNode;
	}

	spIMetadata APICALL MetadataImpl::ConvertToMetadata() {
		return MakeUncheckedSharedPointer( this, __FILE__, __LINE__ );
	}

	void APICALL MetadataImpl::EnableFeature( const char * key, sizet keyLength ) const __NOTHROW__ {
		UTF8String keyStr( key, keyLength );
		if ( keyStr.compare( "alias" ) == 0 )
			mSupportAliases = true;
	}

	void APICALL MetadataImpl::DisableFeature( const char * key, sizet keyLength ) const __NOTHROW__ {
		UTF8String keyStr( key, keyLength );
		if ( keyStr.compare( "alias" ) == 0 )
			mSupportAliases = false;
	}

    bool IsNodeAlias( const char * nameSpace, const char * name, XMP_ExpandedXPath & exPath );
	bool IsNodeAlias( const char * nameSpace, const char * name, XMP_ExpandedXPath & exPath ) {
		spIUTF8String qualName = IUTF8String_I::CreateUTF8String();
		auto defaultMap = INameSpacePrefixMap::GetDefaultNameSpacePrefixMap();
		spcIUTF8String prefixStr = defaultMap->GetPrefix( nameSpace, AdobeXMPCommon::npos );
		if ( !prefixStr ) {
			return false;
		}

		qualName->append( prefixStr )->append( ":", AdobeXMPCommon::npos )->append( name, AdobeXMPCommon::npos );

		bool aliasFound = sRegisteredAliasMap->count( qualName->c_str() ) > 0;
		if ( aliasFound ) {
			exPath = ( *sRegisteredAliasMap )[ qualName->c_str() ];
			return aliasFound;
		}
		return aliasFound;
	}

    
    bool HandleConstAlias( const spIMetadata & meta, spINode & destNode, const XMP_ExpandedXPath & expandedXPath, sizet & nodeIndex );
    
    bool HandleConstAlias( const spIMetadata & meta, spINode & destNode, const XMP_ExpandedXPath & expandedXPath, sizet & nodeIndex ) {
		if ( expandedXPath.empty() ) NOTIFY_ERROR( IError::kEDGeneral, kGECLogicalError, "Empty XPath", IError::kESOperationFatal, false, false );

		if ( !( expandedXPath[ kSchemaStep ].options & kXMP_SchemaNode ) ) {
			return false;
		} else {
			XMP_VarString namespaceName = expandedXPath[ kSchemaStep ].step.c_str();
			size_t colonPos = expandedXPath[ kRootPropStep ].step.find( ":" );
			assert( colonPos != std::string::npos );
			XMP_VarString propertyName = expandedXPath[ kRootPropStep ].step.substr( colonPos + 1 );

			// here find the node with this name
			destNode = meta->GetNode( namespaceName.c_str(), namespaceName.size(), propertyName.c_str(), propertyName.size() );
			if ( !destNode ) return false;
			if ( expandedXPath.size() == 2 ) return true;
			assert( destNode->GetNodeType() == INode::kNTArray );

			if ( expandedXPath[ 2 ].options == kXMP_ArrayIndexStep ) {
				assert( expandedXPath[ 2 ].step == "[1]" );
				destNode = destNode->ConvertToArrayNode()->GetNodeAtIndex( 1 );
				auto actualNodeType = destNode->GetNodeType();
				if ( destNode ) {
					if ( nodeIndex ) nodeIndex = 1;
					return true;
				}
				return false;
			} else if ( expandedXPath[ 2 ].options == kXMP_QualSelectorStep ) {
				assert( expandedXPath[ 2 ].step == "[?xml:lang=\"x-default\"]" );
				if ( !destNode || destNode->GetNodeType() != INode::kNTArray ) return false;
				spINodeIterator iter = destNode->ConvertToArrayNode()->Iterator();
				sizet index = 1;
				while ( iter ) {
					spINode node = iter->GetNode();
					try {
						spISimpleNode qualNode = node->GetSimpleQualifier( "http://www.w3.org/XML/1998/namespace", AdobeXMPCommon::npos, "lang", AdobeXMPCommon::npos );
						if ( qualNode->GetValue()->compare( "x-default" ) == 0 ) {
							destNode = node;
							if ( nodeIndex ) nodeIndex = index;
							return true;
						}
					} catch ( spcIError err ) {

					} catch ( ... ) {}
					index++;
					iter = iter->Next();
				}
				return false;
			}
			return false;
		}
	}
    

    static spINode CreateTerminalNode( const char* nameSpace, const char * name, XMP_OptionBits options, const spcINode & nodeToBeCloned = spINode() ) {

		spINode newNode;
		if ( nodeToBeCloned ) {
			newNode = nodeToBeCloned->Clone();
		}

		else if ( XMP_PropIsSimple( options ) ) {
			newNode = ISimpleNode::CreateSimpleNode( nameSpace, AdobeXMPCommon::npos, name, AdobeXMPCommon::npos );
		} else if ( XMP_PropIsStruct( options ) ) {
			newNode = IStructureNode_v1::CreateStructureNode( nameSpace, AdobeXMPCommon::npos, name, AdobeXMPCommon::npos );
		} else if ( XMP_PropIsArray( options ) ) {
			if ( options & kXMP_PropArrayIsAltText )
				newNode = IArrayNode_v1::CreateAlternativeArrayNode( nameSpace, AdobeXMPCommon::npos, name, AdobeXMPCommon::npos );
			else if ( options & kXMP_PropArrayIsOrdered )
				newNode = IArrayNode_v1::CreateOrderedArrayNode( nameSpace, AdobeXMPCommon::npos, name, AdobeXMPCommon::npos );
			else
				newNode = IArrayNode_v1::CreateUnorderedArrayNode( nameSpace, AdobeXMPCommon::npos, name, AdobeXMPCommon::npos );
		}
		return newNode;
	}

   
    static bool HandleNonConstAlias( const spIMetadata & meta, XMP_ExpandedXPath & expandedXPath, bool createNodes, XMP_OptionBits leafOptions, spINode & destNode, sizet & nodeIndex, bool ignoreLastStep, const spINode & inputNode ) {
		destNode = meta;
		spcIUTF8String inputNodeValue;
		if ( inputNode && inputNode->GetNodeType() == INode::kNTSimple ) {

			inputNodeValue = inputNode->ConvertToSimpleNode()->GetValue();
		}
		bool isAliasBeingCreated = expandedXPath.size() == 2;
		if ( expandedXPath.empty() )
			NOTIFY_ERROR( IError::kEDDataModel, kDMECBadXPath, "Empty XPath", IError::kESOperationFatal, false, false );
		if ( !( expandedXPath[ kSchemaStep ].options & kXMP_SchemaNode ) ) {
			return false;
		} else {
			XMP_VarString namespaceName = expandedXPath[ kSchemaStep ].step.c_str();
			size_t colonPos = expandedXPath[ kRootPropStep ].step.find( ":" );
			assert( colonPos != std::string::npos );
			XMP_VarString propertyName = expandedXPath[ kRootPropStep ].step.substr( colonPos + 1 );
			spcINode childNode = meta->GetNode( namespaceName.c_str(), namespaceName.size(), propertyName.c_str(), propertyName.size() );
			if ( !childNode && !createNodes ) return false;
			if ( expandedXPath.size() == 2 ) {
				if ( childNode ) return true;
				XMP_OptionBits createOptions = 0;
				spINode tempNode;
				if ( isAliasBeingCreated ) tempNode = CreateTerminalNode( namespaceName.c_str(), propertyName.c_str(), leafOptions );
				else  tempNode = CreateTerminalNode( namespaceName.c_str(), propertyName.c_str(), createOptions );
				if ( !tempNode ) return false;
				if ( inputNodeValue ) tempNode->ConvertToSimpleNode()->SetValue( inputNodeValue->c_str(), inputNodeValue->size() );
				if ( destNode == meta ) {
					meta->InsertNode( tempNode );
				} else {
					destNode->ConvertToStructureNode()->AppendNode( tempNode );
				}
				destNode = tempNode;
				if ( destNode ) return true;
				return false;
			}

			XMP_Assert( expandedXPath.size() == 3 );
			if ( expandedXPath[ 2 ].options == kXMP_ArrayIndexStep ) {
				XMP_Assert( expandedXPath[ 2 ].step == "[1]" );
				destNode = meta->GetNode( namespaceName.c_str(), namespaceName.size(), propertyName.c_str(), propertyName.size() );
				if ( !destNode && !createNodes ) return false;
				if ( !destNode ) {
					spINode arrayNode = CreateTerminalNode( namespaceName.c_str(), propertyName.c_str(), kXMP_PropArrayIsOrdered | kXMP_PropValueIsArray );
					meta->AppendNode( arrayNode );
					destNode = arrayNode;
				}

				if ( destNode->ConvertToArrayNode()->GetNodeAtIndex( 1 ) ) {
					destNode = destNode->ConvertToArrayNode()->GetNodeAtIndex( 1 );
					if ( nodeIndex ) nodeIndex = 1;
					return true;
				} else {
					spISimpleNode indexNode = ISimpleNode::CreateSimpleNode( namespaceName.c_str(), namespaceName.size(), propertyName.c_str(), propertyName.size() );
					if ( inputNodeValue ) {
						indexNode->SetValue( inputNodeValue->c_str(), inputNodeValue->size() );
					}
					destNode->ConvertToArrayNode()->InsertNodeAtIndex( indexNode, 1 );
					destNode = indexNode;
					return true;
				}
				return false;
			} else if ( expandedXPath[ 2 ].options == kXMP_QualSelectorStep ) {
				assert( expandedXPath[ 2 ].step == "[?xml:lang=\"x-default\"]" );
				destNode = meta->GetNode( namespaceName.c_str(), namespaceName.size(), propertyName.c_str(), propertyName.size() );
				if ( !destNode && !createNodes ) return false;
				spINode arrayNode = CreateTerminalNode( namespaceName.c_str(), propertyName.c_str(), kXMP_PropValueIsArray | kXMP_PropArrayIsAltText);
				meta->AppendNode( arrayNode );
				destNode = arrayNode;
				auto iter = destNode->ConvertToArrayNode()->Iterator();
				XMP_Index index = 1;
				while ( iter ) {
					spINode node = iter->GetNode();
					spINode qualNode = node->GetQualifier( "http://www.w3.org/XML/1998/namespace", AdobeXMPCommon::npos, "lang", AdobeXMPCommon::npos );
					if ( qualNode->GetNodeType() == INode::kNTSimple ) {
						if ( !qualNode->ConvertToSimpleNode()->GetValue()->compare( "x-default" ) ) {
							destNode = node;
							if ( nodeIndex ) nodeIndex = index;
							return true;
						}
					}
					index++;
					iter = iter->Next();
				}
				spISimpleNode qualifierNode = ISimpleNode::CreateSimpleNode( "http://www.w3.org/XML/1998/namespace", AdobeXMPCommon::npos, "lang", AdobeXMPCommon::npos, "x-default", AdobeXMPCommon::npos );
				if ( destNode->ConvertToArrayNode()->GetNodeAtIndex( 1 ) ) {
					destNode = destNode->ConvertToArrayNode()->GetNodeAtIndex( 1 );
					if ( nodeIndex ) nodeIndex = 1;
					destNode->InsertQualifier( qualifierNode );
					return true;
				} else {
					spISimpleNode indexNode = ISimpleNode::CreateSimpleNode( namespaceName.c_str(), AdobeXMPCommon::npos, propertyName.c_str(), AdobeXMPCommon::npos );
					if ( inputNodeValue ) {
						indexNode->SetValue( inputNodeValue->c_str(), inputNodeValue->size() );
					}
					destNode->ConvertToArrayNode()->InsertNodeAtIndex( indexNode, 1 );
					destNode->InsertQualifier( qualifierNode );
					destNode = indexNode;
					return true;
				}
			}
		}
		return false;
	}


	spINode APICALL MetadataImpl::ReplaceNode( const spINode & node ) {
		if ( mSupportAliases  ) {
			XMP_ExpandedXPath exPath;
			QualifiedName qName( node->GetNameSpace(), node->GetName() );
			bool nodeIsAlias = IsNodeAlias( node->GetNameSpace()->c_str(), node->GetName()->c_str(), exPath );
			if ( nodeIsAlias ) {
				spINode actualNodeToBeRemoved;
				sizet nodeIndex = 0;
				auto spSelf = MakeUncheckedSharedPointer( this, __FILE__, __LINE__, false );
				if ( HandleConstAlias( spSelf, actualNodeToBeRemoved, exPath, nodeIndex ) ) {
					qName = QualifiedName( actualNodeToBeRemoved->GetNameSpace(), actualNodeToBeRemoved->GetName() );
				}
			}

			if ( CheckSuitabilityToBeUsedAsChildNode( node ) && GetNode( qName.mNameSpace, qName.mName ) ) {
				auto retValue = RemoveNode( qName.mNameSpace, qName.mName );
				spINode destNode = node;
				if ( nodeIsAlias ) {
					sizet destNodeIndex = 0;
					auto spSelf = MakeUncheckedSharedPointer( this, __FILE__, __LINE__, false );
					if ( !HandleNonConstAlias( spSelf, exPath, true, 0, destNode, destNodeIndex, false, node ) ) {
						return destNode;
					}
				}
				InsertNode( destNode );
				return retValue;
			} else {
				NOTIFY_ERROR( IError_v1::kEDDataModel, kDMECNoSuchNodeExists,
					"no such node exists with the specified qualified name", IError_v1::kESOperationFatal,
					true, node->GetNameSpace(), true, node->GetName() );
			}
			return spINode();
		} else {
			return StructureNodeImpl::ReplaceNode( node );
		}
	}

	void APICALL MetadataImpl::InsertNode( const spINode & node ) {
	
		if ( mSupportAliases ) {
			if ( !node ) return;
			XMP_ExpandedXPath exPath;
			if ( IsNodeAlias( node->GetNameSpace()->c_str(), node->GetName()->c_str(), exPath ) ) {

				spINode destNode;
				sizet destNodeIndex = 0;
				AutoSharedLock lock( mSharedMutex, true );
				auto spSelf = MakeUncheckedSharedPointer(this, __FILE__, __LINE__, false);
				if ( HandleNonConstAlias( spSelf, exPath, true, 0, destNode, destNodeIndex, false, node ) ) {
				}
			} else {

				return StructureNodeImpl::InsertNode( node );
			}
		} else {
			return StructureNodeImpl::InsertNode( node );
		}
	
		return;
	}

	spINode APICALL MetadataImpl::RemoveNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) {
		if ( mSupportAliases ) {
			XMP_ExpandedXPath  exPath;
			if (IsNodeAlias(nameSpace->c_str(), name->c_str(), exPath)) {
				spINode destNode;
				sizet destNodeIndex = 0;
				auto spSelf = MakeUncheckedSharedPointer(this, __FILE__, __LINE__, false);
				AutoSharedLock lock(mSharedMutex, true);
				/*if (!HandleConstAlias(spSelf, destNode, exPath, destNodeIndex)) {
					destNode = spINode();
				}
				return destNode;*/
				if (!HandleConstAlias(spSelf, destNode, exPath, destNodeIndex)) {
					destNode = spINode();
					return destNode;
				}
				else
				{
					return StructureNodeImpl::RemoveNode(destNode->GetNameSpace(), destNode->GetName());
				}
				

			}
			else {

				//return StructureNodeImpl::GetNode(nameSpace, name);
				return StructureNodeImpl::RemoveNode(nameSpace, name);
			}

		} else {
			return StructureNodeImpl::RemoveNode( nameSpace, name );
		}
	}

	spINode APICALL MetadataImpl::GetNode( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) {
		if ( mSupportAliases ) {
			XMP_ExpandedXPath  exPath;
			if (IsNodeAlias(nameSpace->c_str(), name->c_str(), exPath)) {
				spINode destNode;
				sizet destNodeIndex = 0;
				AutoSharedLock lock(mSharedMutex, true);
				auto spSelf = MakeUncheckedSharedPointer(this, __FILE__, __LINE__, false);
				if (!HandleConstAlias(spSelf, destNode, exPath, destNodeIndex)) {
					destNode = spINode();
				}
				return destNode;
			}
			else {

				return StructureNodeImpl::GetNode(nameSpace, name);
			}

		} else {
			return StructureNodeImpl::GetNode( nameSpace, name );
		}
	}

}

#if BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
namespace AdobeXMPCore {
	using namespace AdobeXMPCore_Int;

	spIMetadata IMetadata::CreateMetadata() {
		return MakeUncheckedSharedPointer( new MetadataImpl(), __FILE__, __LINE__, true );
	}

	spIMetadata IMetadata_v1::MakeShared( pIMetadata_base ptr ) {
		if ( !ptr ) return spIMetadata();
		pIMetadata p = IMetadata::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IMetadata >() : ptr;
		return XMP_COMPONENT_INT_NAMESPACE::MakeUncheckedSharedPointer( p, __FILE__, __LINE__, true );
	}

}
#endif  // BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
