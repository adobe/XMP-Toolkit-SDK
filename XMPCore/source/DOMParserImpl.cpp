// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/DOMParserImpl.h"
	#include "XMPCore/ImplHeaders/ClientDOMParserWrapperImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCore/Interfaces/IArrayNode_I.h"
#include "XMPCore/Interfaces/IMetadata_I.h"
#include "XMPCore/Interfaces/INodeIterator.h"
#include "XMPCommon/Interfaces/ISharedMutex.h"
#include "XMPCommon/Utilities/AutoSharedLock.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"

namespace AdobeXMPCore_Int {

	DOMParserImpl::DOMParserImpl()
		: mSharedMutex( ISharedMutex::CreateSharedMutex() ) { }

	spIDOMParser APICALL DOMParserImpl::Clone() const {
		DOMParserImpl * cloned = clone();
		if ( cloned ) {
			AutoSharedLock lock( mSharedMutex );
			cloned->mTreatKeyAsCaseInsensitiveCharBuffer = mTreatKeyAsCaseInsensitiveCharBuffer;
			cloned->mAllowDifferentValueTypesForExistingEntries = mAllowDifferentValueTypesForExistingEntries;
			if ( mKeysSet )
				cloned->mKeysSet = new KeysSet( mKeysSet->begin(), mKeysSet->end() );

			if ( mKeyValueTypeMap )
				cloned->mKeyValueTypeMap = new keyValueTypeMap( mKeyValueTypeMap->begin(), mKeyValueTypeMap->end() );

			for ( auto it = mMap.begin(), itEnd = mMap.end(); it != itEnd; ++it ) {
				cloned->mMap[ it->first ] = it->second;
			}
		}
		return MakeUncheckedSharedPointer( cloned, __FILE__, __LINE__, true );
	}

	spIMetadata APICALL DOMParserImpl::Parse( const char * buffer, sizet bufferLength ) {
		auto node = ParseAsNode( buffer, bufferLength );
		if ( node ) {
			switch ( node->GetNodeType() ) {
			case INode::kNTSimple:
			case INode::kNTArray:
				{
					spIMetadata meta = IMetadata::CreateMetadata();
					meta->AppendNode( node );
					return meta;
				}
				break;

			case INode::kNTStructure:
				{
					pIMetadata meta( NULL );
					try {
						meta = node->GetInterfacePointer< IMetadata >();
					} catch ( spcIError err ) {
						meta = NULL;
					}
					if ( meta ) {
						return MakeUncheckedSharedPointer( meta, __FILE__, __LINE__ );
					} else {
						spIMetadata spMeta = IMetadata::CreateMetadata();
						spMeta->AppendNode( node );
						return spMeta;
					}
				}
				break;

			default:
				NOTIFY_ERROR( IError::kEDGeneral, kGECInternalFailure, "Unhandled situation occured", IError::kESOperationFatal, false, false );
			}
		}
		return spIMetadata();
	}

	static void AppendAsChildren( const spINode & contextNode, const spINode & parsedNode ) {
		if ( !contextNode )
			NOTIFY_ERROR( IError::kEDParser, kPECInvalidContextNode, "Context Node is invalid", IError::kESOperationFatal, false, false );
		auto contextNodeType = contextNode->GetNodeType();
		if ( contextNodeType != INode::kNTStructure && contextNodeType != INode::kNTArray )
			NOTIFY_ERROR( IError::kEDParser, kPECContextNodeIsNonComposite, "Context Node is non composite", IError::kESOperationFatal,
				true, static_cast< sizet >( contextNodeType ) );
		pICompositeNode compositeContextNode = contextNode->GetInterfacePointer< ICompositeNode >();
		pIMetadata meta( NULL );
		try {
			meta = parsedNode->GetInterfacePointer< IMetadata >();
		} catch ( spcIError err ) {
			meta = NULL;
		}
		if ( meta ) {
			auto it = meta->Iterator();
			while ( it ) {
				auto childNode = it->GetNode();
				it = it->Next();
				childNode = meta->GetIMetadata_I()->RemoveNode( childNode->GetNameSpace(), childNode->GetName() );
				compositeContextNode->AppendNode( childNode );
			}
		} else {
			compositeContextNode->AppendNode( parsedNode );
		}
	}

	static void ReplaceChildren( pIArrayNode contextArrayNode, const spINode & parsedNode ) {
		contextArrayNode->Clear();
		pIMetadata meta( NULL );
		try {
			meta = parsedNode->GetInterfacePointer< IMetadata >();
		} catch ( spcIError err ) {
			meta = NULL;
		}
		if ( meta ) {
			auto it = meta->Iterator();
			while ( it ) {
				auto childNode = it->GetNode();
				it = it->Next();
				childNode = meta->GetIMetadata_I()->RemoveNode( childNode->GetNameSpace(), childNode->GetName() );
				contextArrayNode->AppendNode( childNode );	
			}
		} else {
			contextArrayNode->AppendNode( parsedNode );
		}
	}

	static void ReplaceChildren( pIStructureNode contextStructureNode, const spINode & parsedNode ) {
		pIMetadata meta( NULL );
		try {
			meta = parsedNode->GetInterfacePointer< IMetadata >();
		} catch ( spcIError err ) {
			meta = NULL;
		}
		if ( meta ) {
			auto it = meta->Iterator();
			while ( it ) {
				auto childNode = it->GetNode();
				it = it->Next();
				childNode = meta->GetIMetadata_I()->RemoveNode( childNode->GetNameSpace(), childNode->GetName() );
				contextStructureNode->ReplaceNode( childNode );		
			}
		} else {
			contextStructureNode->ReplaceNode( parsedNode );
		}
	}

	static void ReplaceChildren( const spINode & contextNode, const spINode & parsedNode ) {
		if ( !contextNode )
			NOTIFY_ERROR( IError::kEDParser, kPECInvalidContextNode, "Context Node is invalid", IError::kESOperationFatal, false, false );
		auto contextNodeType = contextNode->GetNodeType();
		if ( contextNodeType != INode::kNTStructure && contextNodeType != INode::kNTArray )
			NOTIFY_ERROR( IError::kEDParser, kPECContextNodeIsNonComposite, "Context Node is non composite", IError::kESOperationFatal,
				true, static_cast< sizet >( contextNodeType ) );
		switch ( contextNodeType ) {
		case INode::kNTArray:
			ReplaceChildren( contextNode->GetInterfacePointer< IArrayNode >(), parsedNode );
			break;

		case INode::kNTStructure:
			ReplaceChildren( contextNode->GetInterfacePointer< IStructureNode >(), parsedNode );
			break;
        default:
                NOTIFY_ERROR( IError::kEDParser, kPECContextNodeIsNonComposite, "Context Node is non composite", IError::kESOperationFatal,true, static_cast< sizet >( contextNodeType ) );
                break;
                
		}
        
	}

	static void AppendOrReplaceChildren( pIStructureNode contextStructureNode, const spINode & parsedNode ) {
		pIMetadata meta( NULL );
		try {
			meta = parsedNode->GetInterfacePointer< IMetadata >();
		} catch ( spcIError err ) {
			meta = NULL;
		}
		if ( meta ) {
			auto it = meta->Iterator();
			while ( it ) {
				auto childNode = it->GetNode();
				it = it->Next();
				childNode = meta->GetIMetadata_I()->RemoveNode( childNode->GetNameSpace(), childNode->GetName() );
				if ( contextStructureNode->GetIStructureNode_I()->GetNode( childNode->GetNameSpace(), childNode->GetName() ) )
					contextStructureNode->ReplaceNode( childNode );
				else
					contextStructureNode->AppendNode( childNode );	
			}
		} else {
			if ( contextStructureNode->GetIStructureNode_I()->GetNode( parsedNode->GetNameSpace(), parsedNode->GetName() ) )
				contextStructureNode->ReplaceNode( parsedNode );
			else
				contextStructureNode->AppendNode( parsedNode );
		}
	}

	static void AppendOrReplaceChildren( const spINode & contextNode, const spINode & parsedNode ) {
		if ( !contextNode )
			NOTIFY_ERROR( IError::kEDParser, kPECInvalidContextNode, "Context Node is invalid", IError::kESOperationFatal, false, false );
		auto contextNodeType = contextNode->GetNodeType();
		if ( contextNodeType != INode::kNTStructure && contextNodeType != INode::kNTArray )
			NOTIFY_ERROR( IError::kEDParser, kPECContextNodeIsNonComposite, "Context Node is non composite", IError::kESOperationFatal,
				true, static_cast< sizet >( contextNodeType ) );
		switch ( contextNodeType ) {
		case INode::kNTArray:
			ReplaceChildren( contextNode->GetInterfacePointer< IArrayNode >(), parsedNode );
			break;

		case INode::kNTStructure:
			AppendOrReplaceChildren( contextNode->GetInterfacePointer< IStructureNode >(), parsedNode );
			break;
        default:
                NOTIFY_ERROR( IError::kEDParser, kPECContextNodeIsNonComposite, "Context Node is non composite", IError::kESOperationFatal,
                             true, static_cast< sizet >( contextNodeType ) );
                break;
		}
        
	}

	static void InsertBefore( const spINode & contextNode, const spINode & parsedNode ) {
		if ( !contextNode )
			NOTIFY_ERROR( IError::kEDParser, kPECInvalidContextNode, "Context Node is invalid", IError::kESOperationFatal, false, false );
		if ( !contextNode->IsArrayItem() )
			NOTIFY_ERROR( IError::kEDParser, kPECContextNodeParentIsNonArray, "Context Node's Parent is non array node", IError::kESOperationFatal, false, false );
		pIMetadata meta( NULL );
		try {
			meta = parsedNode->GetInterfacePointer< IMetadata >();
		} catch ( spcIError err ) {
			meta = NULL;
		}
		pIArrayNode parentArrayNode = contextNode->GetINode_I()->GetRawParentPointer()->GetInterfacePointer< IArrayNode >();
		if ( meta ) {
			auto it = meta->Iterator();
			while ( it ) {
				auto childNode = it->GetNode();
				it = it->Next();
				childNode = meta->GetIMetadata_I()->RemoveNode( childNode->GetNameSpace(), childNode->GetName() );
				parentArrayNode->InsertNodeAtIndex( childNode, contextNode->GetIndex() );
			}
		} else {
			parentArrayNode->InsertNodeAtIndex( parsedNode, contextNode->GetIndex() );
		}

	}

	static void InsertAfter( const spINode & contextNode, const spINode & parsedNode ) {
		if ( !contextNode )
			NOTIFY_ERROR( IError::kEDParser, kPECInvalidContextNode, "Context Node is invalid", IError::kESOperationFatal, false, false );
		if ( !contextNode->IsArrayItem() )
			NOTIFY_ERROR( IError::kEDParser, kPECContextNodeParentIsNonArray, "Context Node's Parent is non array node", IError::kESOperationFatal, false, false );
		pIMetadata meta( NULL );
		try {
			meta = parsedNode->GetInterfacePointer< IMetadata >();
		} catch ( spcIError err ) {
			meta = NULL;
		}
		pIArrayNode parentArrayNode = contextNode->GetINode_I()->GetRawParentPointer()->GetInterfacePointer< IArrayNode >();
		if ( meta ) {
			auto it = meta->Iterator();
			sizet index = contextNode->GetIndex() + 1;
			while ( it ) {
				auto childNode = it->GetNode();
				it = it->Next();
				childNode = meta->GetIMetadata_I()->RemoveNode( childNode->GetNameSpace(), childNode->GetName() );
				parentArrayNode->InsertNodeAtIndex( childNode, index );
				index = childNode->GetIndex() + 1;
			}
		} else {
			parentArrayNode->InsertNodeAtIndex( parsedNode, contextNode->GetIndex() + 1 );
		}
	}

	static void ReplaceNode( spINode & node, const spINode & parsedNode ) {
		if ( node && node->IsArrayItem() && node->GetNodeType() != parsedNode->GetNodeType() ) {
			NOTIFY_ERROR( IError::kEDDataModel, kDMECArrayItemTypeDifferent, "node type is different than what currently array can hold",
				IError_v1::kESOperationFatal, true, static_cast< sizet >( node->GetNodeType() ), true, static_cast< sizet >( parsedNode->GetNodeType() ) );
		}
		node = parsedNode;
	}

	void APICALL DOMParserImpl::ParseWithSpecificAction( const char * buffer, sizet bufferLength, eActionType actionType, spINode & node ) {
		auto parsedNode = ParseAsNode( buffer, bufferLength );

		if ( parsedNode ) {
			switch ( actionType ) {
			case kATAppendAsChildren:
				AppendAsChildren( node, parsedNode );
				break;

			case kATReplaceChildren:
				ReplaceChildren( node, parsedNode );
				break;

			case kATAppendOrReplaceChildren:
				AppendOrReplaceChildren( node, parsedNode );
				break;

			case kATInsertBefore:
				InsertBefore( node, parsedNode );
				break;

			case kATInsertAfter:
				InsertAfter( node, parsedNode );
				break;

			case kATReplace:
				ReplaceNode( node, parsedNode );
				break;

			default:
				NOTIFY_ERROR( IError::kEDGeneral, kGECNotImplemented, "Not yet implemented", IError::kESOperationFatal, true, static_cast< sizet >( actionType ) );
			}

		}
	}

	spISharedMutex APICALL DOMParserImpl::GetMutex() const {
		return mSharedMutex;
	}

	AdobeXMPCore::spIDOMParser IDOMParser_I::CreateDOMParser( pIClientDOMParser_base clientDOMParser ) {
		return MakeUncheckedSharedPointer( new ClientDOMParserWrapperImpl( clientDOMParser ), __FILE__, __LINE__, true );
	}


	void DOMParserImpl::SetErrorCallback(XMPMeta::ErrorCallbackInfo * ec) {
		mGenericErrorCallbackPtr = ec;
	}
}
