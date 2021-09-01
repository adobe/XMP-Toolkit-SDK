// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/RDFDOMParserImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCore/Interfaces/ISimpleNode_I.h"
#include "XMPCore/Interfaces/IArrayNode_I.h"
#include "XMPCore/Interfaces/IMetadata_I.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCore/Interfaces/IMetadataConverterUtils_I.h"
#include "XMPCore/source/XMPMeta.hpp"
#include "XMPUtils.hpp"

namespace AdobeXMPCore_Int {

//	const char * kArrayItemName = "arrayItem";
//	const char * kArrayItemNameSpace = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";

	namespace Parser {
		static uint64 kAllowedKeys[] = { IConfigurable::ConvertCharBufferToUint64( "rqMetaEl" ), IConfigurable::ConvertCharBufferToUint64( "sctAlias" ) };
		static ConfigurableImpl::KeyValueTypePair kAllowedKeyValueTypes[] = {
			std::make_pair( kAllowedKeys[ 0 ], IConfigurable::kDTBool ),
			std::make_pair( kAllowedKeys[ 1 ], IConfigurable::kDTBool ) };
	}

//	static void CreateAndPopulateNode( const spINode & parentNode, XMP_Node * node, bool nodeIsQualifier = false ) {
//		XMP_StringPtr nameSpaceStr, nameStr;
//		XMP_StringLen nameSpaceLen, nameLen;
//		node->GetFullQualifiedName( &nameSpaceStr, &nameSpaceLen, &nameStr, &nameLen );
//		if ( nameSpaceLen == 0 && nameLen == 0 ) { //true in case of array element
//			nameSpaceStr = kArrayItemNameSpace;
//			nameStr = kArrayItemName;
//			nameSpaceLen = (XMP_Uns32)strlen( kArrayItemNameSpace );
//			nameLen = (XMP_Uns32)strlen( kArrayItemName );
//		}
//
//		spINode spNode;
//		if ( XMP_PropIsSimple( node->options ) ) {
//			spISimpleNode spSimpleNode = ISimpleNode::CreateSimpleNode( nameSpaceStr, nameSpaceLen, nameStr, nameLen, node->value.c_str(), node->value.size() );
//			spSimpleNode->SetURIType( XMP_OptionIsSet( node->options, kXMP_PropValueIsURI ) );
//			spNode = spSimpleNode;
//		} else if ( XMP_PropIsStruct( node->options ) ) {
//			spIStructureNode spStructNode = IStructureNode::CreateStructureNode( nameSpaceStr, nameSpaceLen, nameStr, nameLen );
//			for ( sizet index = 0, count = node->children.size(); index < count; index++ )
//				CreateAndPopulateNode( spStructNode, node->children[ index ] );
//			spNode = spStructNode;
//		} else if ( XMP_PropIsArray( node->options ) ) {
//			IArrayNode::eArrayForm arrayNodeForm = IArrayNode::kAFUnordered;
//			if ( XMP_ArrayIsAlternate( node->options ) ) arrayNodeForm = IArrayNode::kAFAlternative;
//			else if ( XMP_ArrayIsOrdered( node->options ) ) arrayNodeForm = IArrayNode::kAFOrdered;
//			spIUTF8String nameSpaceUTF8Str( IUTF8String_I::CreateUTF8String( nameSpaceStr, nameSpaceLen ) );
//			spIUTF8String nameUTF8Str( IUTF8String_I::CreateUTF8String( nameStr, nameLen ) );
//			spIArrayNode spArrayNode = IArrayNode_I::CreateArrayNode( nameSpaceUTF8Str, nameUTF8Str, arrayNodeForm );
//			for ( sizet index = 0, count = node->children.size(); index < count; index++ )
//				CreateAndPopulateNode( spArrayNode, node->children[ index ] );
//			spNode = spArrayNode;
//		}
//
//		if ( spNode ) {
//			// append qualifiers.
//			if ( node->qualifiers.size() > 0 ) {
//				for ( sizet index = 0, count = node->qualifiers.size(); index < count; index++ ) {
//					CreateAndPopulateNode( spNode, node->qualifiers[ index ], true );
//				}
//			}
//			if ( nodeIsQualifier )
//				parentNode->InsertQualifier( spNode );
//			else 
//				parentNode->GetInterfacePointer< ICompositeNode_v1 >()->AppendNode( spNode );
//		}
//
//	}

	DOMParserImpl * APICALL RDFDOMParserImpl::clone() const {
		return new RDFDOMParserImpl();
	}

	spINode APICALL RDFDOMParserImpl::ParseAsNode( const char * buffer, sizet bufferLength ) {
		shared_ptr < XMPMeta > spMeta( new XMPMeta() );
		try {

			if (mGenericErrorCallbackPtr && mGenericErrorCallbackPtr->wrapperProc) {
				spMeta->SetErrorCallback(mGenericErrorCallbackPtr->wrapperProc, mGenericErrorCallbackPtr->clientProc, mGenericErrorCallbackPtr->context, mGenericErrorCallbackPtr->limit);
				spMeta->errorCallback.notifications = mGenericErrorCallbackPtr->notifications;
			}
			XMP_OptionBits options( 0 );
			bool value;
			if ( GetParameter( Parser::kAllowedKeys[ 0 ], value ) && value )
				options |= kXMP_RequireXMPMeta;
			if ( GetParameter( Parser::kAllowedKeys[ 1 ], value ) && value )
				options |= kXMP_StrictAliasing;
			spMeta->ParseFromBuffer( buffer, static_cast< XMP_StringLen >( bufferLength ), static_cast< XMP_OptionBits >( options ) );
		} catch ( XMP_Error & xmpError ) {
			IError::eErrorDomain domain( IError::kEDNone );
			IError::eErrorCode code( kGECNone );
			if ( mGenericErrorCallbackPtr && mGenericErrorCallbackPtr->wrapperProc ) {
			mGenericErrorCallbackPtr->notifications = spMeta->errorCallback.notifications;
			}
#if ENABLE_CPP_DOM_MODEL
			XMPUtils::MapXMPErrorToIError( xmpError.GetID(), domain, code );
#endif
			NOTIFY_ERROR( domain, code, "XMP Error caught", IError::kESOperationFatal, false, false );
			
		}
		if ( mGenericErrorCallbackPtr && mGenericErrorCallbackPtr->wrapperProc ) {
			mGenericErrorCallbackPtr->notifications = spMeta->errorCallback.notifications;
		}
        
        return IMetadataConverterUtils_I::convertXMPMetatoIMetadata(spMeta.get());
//		spIMetadata metadata = IMetadata::CreateMetadata();
//		if ( spMeta ) {
//			metadata->SetAboutURI( spMeta->tree.name.c_str(), spMeta->tree.name.size() );
//
//			// all the top level children of this tree are actually top level namespace entries.
//			// name begin the namespace string and value contains the prefix with colon.
//			// actual nodes are below these top level children.
//			for ( sizet index = 0, count = spMeta->tree.children.size(); index < count; ++index ) {
//				XMP_Node * topLevelNode = spMeta->tree.children[ index ];
//				for ( sizet innerIndex = 0, innerCount = topLevelNode->children.size(); innerIndex < innerCount; ++innerIndex ) {
//					CreateAndPopulateNode( metadata, topLevelNode->children[ innerIndex ] );
//				}
//			}
//		}
//		metadata->AcknowledgeChanges();
//		return metadata;
	}

	eConfigurableErrorCode APICALL RDFDOMParserImpl::ValidateValue( const uint64 & key, eDataType type, const CombinedDataValue & value ) const {
		return kCECNone;
	}

	void RDFDOMParserImpl::InitializeDefaultValues() {
		TreatKeyAsCaseInsensitive( true );
		AllowDifferentValueTypesForExistingEntries( false );
		
		SetAllowedKeys( &Parser::kAllowedKeys[ 0 ], 2 );
		SetAllowedValueTypesForKeys( &Parser::kAllowedKeyValueTypes[ 0 ], 2 );
		SetParameter( Parser::kAllowedKeys[ 0 ], false );
		SetParameter( Parser::kAllowedKeys[ 1 ], false );
	}

	void RDFDOMParserImpl::SetErrorCallback(XMPMeta::ErrorCallbackInfo * ec) {
		mGenericErrorCallbackPtr = ec;
	}



}
