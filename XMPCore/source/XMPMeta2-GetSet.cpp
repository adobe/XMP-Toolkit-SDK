// =================================================================================================
// Copyright 2003 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
//
// Adobe patent application tracking #P435, entitled 'Unique markers to simplify embedding data of
// one format in a file with a different format', inventors: Sean Parent, Greg Gilley.
// =================================================================================================
// =================================================================================================

#include "XMPCore/XMPCoreDefines.h"
#if ENABLE_CPP_DOM_MODEL
#include "public/include/XMP_Environment.h"	// ! This must be the first include!
#if XMP_DebugBuild
	#include <iostream>
#endif
#include "XMPCore/source/XMPCore_Impl.hpp"


#include "XMPCore/source/XMPMeta2.hpp"
#include "XMPCore/source/XMPIterator.hpp"
#include "XMPCore/source/XMPUtils.hpp"

#include "public/include/XMP_Version.h"
#include "source/UnicodeInlines.incl_cpp"
#include "source/UnicodeConversions.hpp"
#include "source/ExpatAdapter.hpp"
#include "third-party/zuid/interfaces/MD5.h"
#include "XMPCore/Interfaces/IMetadata_I.h"
#include "XMPCore/Interfaces/IArrayNode_I.h"
#include "XMPCore/Interfaces/ISimpleNode_I.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCore/Interfaces/IPathSegment_I.h"
#include "XMPCore/Interfaces/IPath_I.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap_I.h"
#include "XMPCore/Interfaces/IDOMImplementationRegistry_I.h"
#include "XMPCore/Interfaces/IDOMParser.h"
#include "XMPCore/Interfaces/IDOMSerializer.h"
#include "XMPCore/Interfaces/INodeIterator.h"
#include "XMPCore/Interfaces/IDOMParser_I.h"
#include "XMPCore/Interfaces/IDOMSerializer_I.h"
#include "XMPCore/Interfaces/ICoreConfigurationManager.h"

using namespace std;

#if XMP_WinBuild
	#pragma warning ( disable : 4533 )	// initialization of '...' is skipped by 'goto ...'
	#pragma warning ( disable : 4702 )	// unreachable code
	#pragma warning ( disable : 4800 )	// forcing value to bool 'true' or 'false' (performance warning)
#endif



// *** Use the XMP_PropIsXyz (Schema, Simple, Struct, Array, ...) macros
// *** Add debug codegen checks, e.g. that typical masking operations really work
// *** Change all uses of strcmp and strncmp to XMP_LitMatch and XMP_LitNMatch


// =================================================================================================
// Local Types and Constants
// =========================

typedef unsigned char XMP_CLTMatch;

enum {	// Values for XMP_CLTMatch.
	kXMP_CLT_NoValues,
	kXMP_CLT_SpecificMatch,
	kXMP_CLT_SingleGeneric,
	kXMP_CLT_MultipleGeneric,
	kXMP_CLT_XDefault,
	kXMP_CLT_FirstItem
};
const XMP_VarString xmlNameSpace  = "http://www.w3.org/XML/1998/namespace";

// =================================================================================================
// Static Variables
// ================


// =================================================================================================
// Local Utilities
// ===============

using namespace AdobeXMPCore_Int;
using namespace AdobeXMPCommon_Int;




static void
AppendIXMPLangItem ( const spIArrayNode & arrayNode, XMP_StringPtr itemLang, XMP_StringPtr itemValue )
{
	

	
	spISimpleNode newItem = ISimpleNode::CreateSimpleNode( arrayNode->GetNameSpace()->c_str(), arrayNode->GetNameSpace()->size(), arrayNode->GetName()->c_str(), arrayNode->GetName()->size(), "", AdobeXMPCommon::npos );
	spISimpleNode langQual = ISimpleNode::CreateSimpleNode( xmlNameSpace.c_str(), xmlNameSpace.size(), "lang", AdobeXMPCommon::npos, "", AdobeXMPCommon::npos );
	
	try {
		
		XMPUtils::SetNode(newItem,itemValue,(kXMP_PropHasQualifiers | kXMP_PropHasLang));
		XMPUtils::SetNode(langQual, itemLang, kXMP_PropIsQualifier);
		
	} catch (...) {
		
		newItem->Clear();
		langQual->Clear();
		throw;
	}
	
	
	newItem->InsertQualifier(langQual);
	if ( (!arrayNode->ChildCount() || !XMP_LitMatch(langQual->GetValue()->c_str(),"x-default") )) {
		
		size_t arraySize = arrayNode->ChildCount();
		arrayNode->InsertNodeAtIndex(newItem, arraySize + 1);
		
	} else {
		
		arrayNode->InsertNodeAtIndex(newItem, 1);
	}
	
}	// AppendLangItem




// -------------------------------------------------------------------------------------------------
// GetProperty
// -----------





XMPMeta2::XMPMeta2()
{
	mDOM = IMetadata::CreateMetadata();
	mDOM->EnableFeature("alias", 5);
	spRegistry = IDOMImplementationRegistry::GetDOMImplementationRegistry();
	spParser = spRegistry->GetParser( "rdf" );
}

XMPMeta2::~XMPMeta2() RELEASE_NO_THROW
{

}



bool
XMPMeta2::GetProperty ( XMP_StringPtr	schemaNS,
					   XMP_StringPtr	propName,
					   XMP_StringPtr *	propValue,
					   XMP_StringLen *	valueSize,
					   XMP_OptionBits *	options ) const
{
	XMP_Assert ( (schemaNS != 0) && (propName != 0) );	// Enforced by wrapper.
	XMP_Assert ( (propValue != 0) && (valueSize != 0) && (options != 0) );	// Enforced by wrapper.
	XMP_ExpandedXPath expPath;
	ExpandXPath ( schemaNS, propName, &expPath );

	
	auto defaultMap = INameSpacePrefixMap::GetDefaultNameSpacePrefixMap();
	spINode destNode = mDOM;
	bool qualifierFlag = false;
	size_t pathStartIdx = 1;
	if (expPath[kRootPropStep].options & kXMP_StepIsAlias) {

		if (!XMPUtils::HandleConstAliasStep(mDOM, destNode, expPath, 0))  return false;
		pathStartIdx = 2;

	}
	for ( size_t i = pathStartIdx, endIndex = expPath.size(); i < endIndex; i++ ) {
		
		if(!destNode) return false;
		XMP_VarString  stepStr = expPath[i].step;
		XMP_VarString  prevStep = ( i == 0 ) ?  "" : expPath[i - 1].step;
		spcIUTF8String nameSpace ;
		
		switch( expPath[i].options ) {
		case kXMP_StructFieldStep:
			{
				size_t colonPos = stepStr.find(':');
				XMP_VarString prefix = stepStr.substr( 0, colonPos );
				// get the namespace from the prefix
				nameSpace = defaultMap->GetNameSpace( prefix.c_str(), prefix.size() );
				if(destNode->GetNodeType() == INode::kNTStructure) {
					spIStructureNode tempNode = destNode->ConvertToStructureNode();
					destNode = tempNode->GetNode(nameSpace->c_str(), AdobeXMPCommon::npos, stepStr.c_str() + colonPos + 1, AdobeXMPCommon::npos );
				}
				else {
					return false;
				}
			}
			break;
		case kXMP_ArrayIndexStep:
			{
				
				if(destNode->GetNodeType() != INode::kNTArray) {
					return false;
				}
				spIArrayNode tempNode = destNode->ConvertToArrayNode();
				XMP_Index index = 0;
				XMP_Assert ( (stepStr.length() >= 2) && (*( stepStr.begin()) == '[') && (stepStr[stepStr.length()-1] == ']') );
				for ( size_t chNum = 1,chEnd = stepStr.length() -1 ; chNum != chEnd; ++chNum ) {
					XMP_Assert ( ('0' <= stepStr[chNum]) && (stepStr[chNum] <= '9') );
					index = (index * 10) + (stepStr[chNum] - '0');
				}
				if ( index < 1) XMP_Throw ( "Array index must be larger than one", kXMPErr_BadXPath );
				size_t colonPos = prevStep.find(':');
				XMP_VarString prefix = prevStep.substr( 0, colonPos );
				nameSpace = defaultMap->GetNameSpace( prefix.c_str(), prefix.size() );
				destNode = tempNode->GetNodeAtIndex( index );
			}
			break;
		case kXMP_ArrayLastStep:
			{
				if(destNode->GetNodeType() != INode::kNTArray) {
					return false;
				}
				spIArrayNode tempNode = destNode->ConvertToArrayNode();
				
				size_t colonPos = prevStep.find(':');
				XMP_VarString prefix = prevStep.substr( 0, colonPos );
				nameSpace = defaultMap->GetNameSpace( prefix.c_str(), prefix.size() );
				spINode parentNode = destNode;
				if(parentNode && parentNode->GetNodeType()== INode::kNTArray) {
					size_t childCount = parentNode->ConvertToArrayNode()->ChildCount();
					if(!childCount) {
						XMP_Throw ( "Array index overflow", kXMPErr_BadXPath );
					}
					destNode = tempNode->GetNodeAtIndex(childCount);
				}
				
			}
			break;
		case kXMP_QualifierStep:
			{

				XMP_Assert(stepStr[0]=='?');
				stepStr = stepStr.substr(1);
				size_t colonPos = stepStr.find(':');
				XMP_VarString prefix = stepStr.substr( 0, colonPos);
				nameSpace = defaultMap->GetNameSpace( prefix.c_str(), prefix.size() );
				destNode = destNode->GetQualifier(nameSpace->c_str(), nameSpace->size(), stepStr.c_str() + colonPos + 1, AdobeXMPCommon::npos );
				qualifierFlag = true;
			}
			
			break;
			
		case kXMP_QualSelectorStep:
			{
				
				if(destNode->GetNodeType() != INode::kNTArray) {
					return false;
				}
				spIArrayNode tempNode = destNode->ConvertToArrayNode();
				XMP_VarString  qualName, qualValue, qualNameSpace;	
				SplitNameAndValue (stepStr, &qualName, &qualValue );
				spINode parentNode = destNode;
				size_t colonPos = qualName.find(':');
				XMP_VarString prefix = qualName.substr( 0, colonPos);
				qualNameSpace = defaultMap->GetNameSpace( prefix.c_str(), prefix.size() )->c_str();
				bool indexFound = false;
				if(parentNode && parentNode->GetNodeType() == INode::kNTArray) {

					spIArrayNode parentArrayNode = parentNode->ConvertToArrayNode();
					size_t arrayChildCount = parentArrayNode->ChildCount();
					for(size_t arrayIdx = 1; arrayIdx <= arrayChildCount; arrayIdx++) {
						spINode currentArrayItem = parentArrayNode->GetNodeAtIndex(arrayIdx);
						spINode qualNode = currentArrayItem->GetQualifier(qualNameSpace.c_str(), qualNameSpace.size(), qualName.c_str() + colonPos + 1, AdobeXMPCommon::npos );
						if(!qualNode) continue;
						XMP_VarString currentQualValue = qualNode->ConvertToSimpleNode()->GetValue()->c_str();
						if( currentQualValue == qualValue) {
							indexFound = true;
							destNode = parentArrayNode->GetNodeAtIndex( arrayIdx);
							break;
						}
					}

				}
				if(!indexFound) {
					return false;
				}
			}
			break;
		
		case kXMP_FieldSelectorStep :
			{
				
				XMP_VarString  fieldName, fieldValue, fieldNameSpace;	
				SplitNameAndValue (stepStr, &fieldName, &fieldValue );
				spINode parentNode = destNode;
				size_t colonPos = fieldName.find(':');
				XMP_VarString prefix = fieldName.substr( 0, colonPos);
				fieldNameSpace = defaultMap->GetNameSpace( prefix.c_str(), prefix.size() )->c_str();
				bool indexFound = false;
				if(parentNode && parentNode->GetNodeType() == INode::kNTArray) {

					spIArrayNode parentArrayNode = parentNode->ConvertToArrayNode();
					size_t arrayChildCount = parentArrayNode->ChildCount();
					for(size_t arrayIdx = 1; arrayIdx <= arrayChildCount; arrayIdx++) {
						
						spINode currentItem = parentArrayNode->GetNodeAtIndex(arrayIdx);
						
						if(currentItem->GetNodeType() != INode::kNTStructure) {
							return false;
						}
						
						spINode fieldNode = currentItem->ConvertToStructureNode()->GetNode(fieldNameSpace.c_str(), fieldNameSpace.size(), fieldName.c_str() + colonPos + 1, AdobeXMPCommon::npos );
						if(!fieldNode || fieldNode->GetNodeType() != INode::kNTSimple) continue;
						XMP_VarString currentFieldValue = fieldNode->ConvertToSimpleNode()->GetValue()->c_str();
						if( currentFieldValue == fieldValue) {
							indexFound = true;
							destNode = parentArrayNode->GetNodeAtIndex( arrayIdx);
							break;
						}
					}
				}
				if(!indexFound) {
					return false;
				}
			}
			break;
		default:
			break;

		}
		
	}

	
	if (!destNode) {
		return false;
	}
	if(options)*options = XMPUtils::GetIXMPOptions(destNode);
	if ( destNode->GetNodeType() == INode::kNTSimple ) {
		
		spcIUTF8String value = destNode->ConvertToSimpleNode()->GetValue();
		*propValue = value->c_str();
		*valueSize = static_cast<XMP_StringLen>( value->size() );
	} 
	return  true;
	
}	// GetProperty

// -------------------------------------------------------------------------------------------------
// CountArrayItems
// ---------------

XMP_Index
XMPMeta2::CountArrayItems ( XMP_StringPtr schemaNS,
						   XMP_StringPtr arrayName ) const
{
	XMP_Assert ( (schemaNS != 0) && (arrayName != 0) );	// Enforced by wrapper.

	XMP_ExpandedXPath	expPath;
	ExpandXPath ( schemaNS, arrayName, &expPath );

	spINode arrayNode ;
	XMP_OptionBits arrayOptions = 0;
	if(!XMPUtils::FindCnstNode(this->mDOM, expPath, arrayNode, &arrayOptions)) return false;
	
	
	if ( ! (arrayOptions & kXMP_PropValueIsArray) ) XMP_Throw ( "The named property is not an array", kXMPErr_BadXPath );
	return static_cast<XMP_Index>( XMPUtils::GetNodeChildCount(arrayNode) );

}	// CountArrayItems


void XMPMeta2::ParseFromBuffer ( XMP_StringPtr buffer, XMP_StringLen bufferSize, XMP_OptionBits options )
{
	bool lastClientCall = (options & kXMP_ParseMoreBuffers) ? false : true;
	if (!mBuffer) {
		mBuffer = IUTF8String_I::CreateUTF8String("", 0);
	}
	sizet bufferSizeIn64Bits = static_cast<sizet>(bufferSize);
	if (bufferSize == kXMP_UseNullTermination) {
		bufferSizeIn64Bits = std::string::npos;
	}
	mBuffer->append(buffer, bufferSizeIn64Bits);
	

	if (!lastClientCall) {
		return;
	}

	spParser->GetIDOMParser_I()->SetErrorCallback(&errorCallback);
	mDOM = spParser->Parse( mBuffer->c_str(), mBuffer->size() );
	mBuffer->clear();
}

void XMPMeta2::SerializeToBuffer ( XMP_VarString * rdfString,
						XMP_OptionBits	options,
						XMP_StringLen	padding,
						XMP_StringPtr	newline,
						XMP_StringPtr	indent,
						XMP_Index		baseIndent ) const
{
	auto registry = IDOMImplementationRegistry::GetDOMImplementationRegistry();
	auto rdfSerializer = registry->GetSerializer( "rdf" );
	auto str = rdfSerializer->GetIDOMSerializer_I()->SerializeInternal( mDOM, options, padding, newline, indent, baseIndent);
	rdfString->clear();
	if (str)
		rdfString->append( str->c_str() );
}


void
XMPMeta2::Sort()
{
	// need internal implementation of sort here
	return;

}	// Sort

void
XMPMeta2::Erase()
{

	if ( this->xmlParser != 0 ) {
		delete ( this->xmlParser );
		this->xmlParser = 0;
	}
	mDOM->Clear();
}	
// DoesPropertyExist
// -----------------

bool
XMPMeta2::DoesPropertyExist ( XMP_StringPtr schemaNS,
							 XMP_StringPtr propName ) const
{
	XMP_Assert ( (schemaNS != 0) && (propName != 0) );	// Enforced by wrapper.

	XMP_ExpandedXPath	expPath;
	ExpandXPath ( schemaNS, propName, &expPath );
	spINode destNode;
	XMP_OptionBits options;
	return XMPUtils::FindCnstNode ( this->mDOM, expPath, destNode, &options );
	
	
}	// DoesPropertyExist

// SetProperty
// -----------

// *** Should handle array items specially, calling SetArrayItem.

void
XMPMeta2::SetProperty ( XMP_StringPtr  schemaNS,
					   XMP_StringPtr  propName,
					   XMP_StringPtr  propValue,
					   XMP_OptionBits options )
{
	XMP_Assert ( (schemaNS != 0) && (propName != 0) );	// Enforced by wrapper.

	options = VerifySetOptions ( options, propValue );

	XMP_ExpandedXPath expPath;
	ExpandXPath ( schemaNS, propName, &expPath );

	spINode node ;
	bool propertyFound = XMPUtils::FindNode ( mDOM, expPath, kXMP_CreateNodes, options, node, 0 );
	if (!propertyFound) XMP_Throw ( "Specified property does not exist", kXMPErr_BadXPath );
	
	XMPUtils::SetNode ( node, propValue, options );
	
}	// SetProperty
// -------------------------------------------------------------------------------------------------'
// -------------------------------------------------------------------------------------------------
// SetArrayItem
// ------------

void
XMPMeta2::SetArrayItem ( XMP_StringPtr  schemaNS,
						XMP_StringPtr  arrayName,
						XMP_Index	   itemIndex,
						XMP_StringPtr  itemValue,
						XMP_OptionBits options )
{
	XMP_Assert ( (schemaNS != 0) && (arrayName != 0) );	// Enforced by wrapper.

	XMP_ExpandedXPath arrayPath;
	ExpandXPath ( schemaNS, arrayName, &arrayPath );
	spINode destNode;
	if(!XMPUtils::FindNode ( mDOM, arrayPath, false,options, destNode ) ) {
		XMP_Throw ( "Specified array does not exist", kXMPErr_BadXPath );
	}
	int x = destNode->GetNodeType();
	if(destNode->GetNodeType() != INode::kNTArray) {
		XMP_Throw ( "Specified array does not exist", kXMPErr_BadXPath );
	}
	
	spIArrayNode arrayNode = destNode->ConvertToArrayNode();
	XMPUtils::DoSetArrayItem ( arrayNode, itemIndex, itemValue, options );
	
}	// SetArrayItem


// -------------------------------------------------------------------------------------------------
// AppendArrayItem
// ---------------

void
XMPMeta2::AppendArrayItem ( XMP_StringPtr  schemaNS,
						   XMP_StringPtr  arrayName,
						   XMP_OptionBits arrayOptions,
						   XMP_StringPtr  itemValue,
						   XMP_OptionBits options )
{
	// TO DO check in case array node doesn't already exist, and the parent of the array to be created is also an array -currently appending the array at the end of the existing array
	XMP_Assert ( (schemaNS != 0) && (arrayName != 0) );	// Enforced by wrapper.

	arrayOptions = VerifySetOptions ( arrayOptions, 0 );
	if ( (arrayOptions & ~kXMP_PropArrayFormMask) != 0 ) {
		XMP_Throw ( "Only array form flags allowed for arrayOptions", kXMPErr_BadOptions );
	}
	

	XMP_ExpandedXPath arrayPath;
	ExpandXPath ( schemaNS, arrayName, &arrayPath );
	spINode destNode;
	spIArrayNode arrayNode;
	XMP_OptionBits dummyOptions;
	XMP_Index insertIndex = 0;
	// either destNode will be the array node or it will be the parent node of the array
	if(XMPUtils::FindCnstNode (mDOM, arrayPath, destNode, &dummyOptions ))	{

		if ( destNode->GetNodeType() != INode::kNTArray) {

			XMP_Throw ( "The named property is not an array", kXMPErr_BadXPath );

		}
		

	} 
	else {
		
		if ( arrayOptions == 0 ) XMP_Throw ( "Explicit arrayOptions required to create new array", kXMPErr_BadOptions );
		XPathStepInfo  lastPathSegment( arrayPath.back());
		XMP_VarString arrayStep = lastPathSegment.step;
		//arrayPath.pop_back();

		if(!XMPUtils::FindNode(this->mDOM, arrayPath, kXMP_CreateNodes, arrayOptions, destNode, &insertIndex)) {
			XMP_Throw ( "Failure creating array node", kXMPErr_BadXPath );
		}
		
	}
	arrayNode = destNode->ConvertToArrayNode();
	XMPUtils::DoSetArrayItem ( arrayNode, kXMP_ArrayLastItem, itemValue, (options | kXMP_InsertAfterItem) );

	
	
}	// AppendArrayItem

// -------------------------------------------------------------------------------------------------
// SetQualifier
// ------------

void
XMPMeta2::SetQualifier ( XMP_StringPtr  schemaNS,
						XMP_StringPtr  propName,
						XMP_StringPtr  qualNS,
						XMP_StringPtr  qualName,
						XMP_StringPtr  qualValue,
						XMP_OptionBits options )
{
	XMP_Assert ( (schemaNS != 0) && (propName != 0) && (qualNS != 0) && (qualName != 0) );	// Enforced by wrapper.

	XMP_ExpandedXPath expPath;
	ExpandXPath ( schemaNS, propName, &expPath );
	spINode destNode ;
	
	if(!XMPUtils::FindCnstNode ( mDOM, expPath, destNode) )
	XMP_Throw ( "Specified property does not exist", kXMPErr_BadXPath );

	XMP_VarString qualPath;
	XMPUtils::ComposeQualifierPath ( schemaNS, propName, qualNS, qualName, &qualPath );
	SetProperty ( schemaNS, qualPath.c_str(), qualValue, options );

}	
// SetQualifier

// Clone
// -----

void
XMPMeta2::Clone ( XMPMeta * clone, XMP_OptionBits options ) const
{
	
	XMPMeta2 * xmpMeta2Ptr = dynamic_cast<XMPMeta2 *>(clone);
	// Possible to do a safer/better cast?
	if (xmpMeta2Ptr== 0 ) XMP_Throw ( "Null clone pointer", kXMPErr_BadParam );
	if ( options != 0 ) XMP_Throw ( "No options are defined yet", kXMPErr_BadOptions );
	
	xmpMeta2Ptr->mDOM->Clear();
	xmpMeta2Ptr->mDOM = mDOM->Clone()->ConvertToMetadata();

}	// Clone


// -------------------------------------------------------------------------------------------------
// DeleteProperty
// --------------


void
XMPMeta2::DeleteProperty	( XMP_StringPtr	schemaNS,
						  XMP_StringPtr	propName )
{
	XMP_Assert ( (schemaNS != 0) && (propName != 0) );	// Enforced by wrapper.

	XMP_ExpandedXPath	expPath;
	ExpandXPath ( schemaNS, propName, &expPath );
	
	XMP_NodePtrPos ptrPos;
	spINode propNode ;
	XMP_OptionBits options = 0;
	XMP_Index arrayIndex = 0;
	if(!XMPUtils::FindCnstNode ( mDOM, expPath, propNode, &options, &arrayIndex ) || !propNode ) {

		return;
	}
	if (!propNode) return;
	
	spINode  parentNode = propNode->GetParent();
	
	// Erase the pointer from the parent's vector, then delete the node and all below it.
	
	if (  (options & kXMP_PropIsQualifier) ) {
		
		parentNode->RemoveQualifier( propNode->GetNameSpace()->c_str(), propNode->GetNameSpace()->size(),
			propNode->GetName()->c_str(), propNode->GetName()->size() );
		

	}
	else if(parentNode->GetNodeType() == INode::kNTArray)  {

		spIArrayNode parentArrayNode = parentNode->ConvertToArrayNode();
		parentArrayNode->RemoveNodeAtIndex(arrayIndex);
		

	}
	else if(parentNode->GetNodeType() == INode::kNTStructure) {
		spIStructureNode parentStructureNode = parentNode->ConvertToStructureNode();
		parentStructureNode->RemoveNode( propNode->GetNameSpace()->c_str(), propNode->GetNameSpace()->size(),
			propNode->GetName()->c_str(), propNode->GetName()->size() );
	}
	// delete subtree - needed ?
	//propNode->Clear();
	
	
}	// DeleteProperty

void
XMPMeta2::GetObjectName ( XMP_StringPtr * namePtr,
						 XMP_StringLen * nameLen ) const
{
	*namePtr = this->mDOM->GetAboutURI()->c_str();
	*nameLen = static_cast<XMP_StringLen> ( this->mDOM->GetAboutURI()->size() );

}	// GetObjectName


// -------------------------------------------------------------------------------------------------
// SetObjectName
// -------------

void
XMPMeta2::SetObjectName ( XMP_StringPtr name )
{
	VerifyUTF8 (name);	// Throws if the string is not legit UTF-8.
	this->mDOM->SetAboutURI(name, AdobeXMPCommon::npos );

}	// SetObjectName

// -------------------------------------------------------------------------------------------------
// ChooseLocalizedText
// -------------------
//
// 1. Look for an exact match with the specific language.
// 2. If a generic language is given, look for partial matches.
// 3. Look for an "x-default" item.
// 4. Choose the first item.

static XMP_CLTMatch
ChooseIXMPLocalizedText ( 
					  const spIArrayNode &arrayNode,
					  XMP_OptionBits    &options,
					  XMP_StringPtr		genericLang,
					  XMP_StringPtr		specificLang,
					  spINode		&itemNode )
{
	spINode currItem ;
	const size_t itemLim = arrayNode->ChildCount();
	size_t itemNum;
	const XMP_VarString  xmlLangQualifierName = "lang";

	// See if the array has the right form. Allow empty alt arrays, that is what parsing returns.
	// *** Should check alt-text bit when that is reliably maintained.

	if ( ! ( XMP_ArrayIsAltText(options) ||
	         (!itemLim && XMP_ArrayIsAlternate(options)) ) ) {
		XMP_Throw ( "Localized text array is not alt-text", kXMPErr_BadXPath );
	}
	if ( !itemLim ) {
		
		return kXMP_CLT_NoValues;
	}

	for ( itemNum = 1; itemNum <= itemLim; ++itemNum ) {
		currItem = arrayNode->GetNodeAtIndex(itemNum);
		if ( currItem->GetNodeType()!= INode::kNTSimple ) {
			XMP_Throw ( "Alt-text array item is not simple", kXMPErr_BadXPath );
		}
		if ( !currItem->HasQualifiers() || !currItem->GetQualifier(xmlNameSpace.c_str(), xmlNameSpace.size(), xmlLangQualifierName.c_str(), xmlLangQualifierName.size() ) ) {
			XMP_Throw ( "Alt-text array item has no language qualifier", kXMPErr_BadXPath );
		}
	}

	// Look for an exact match with the specific language.
	spISimpleNode xmlLangQualifierNode, currItemValue;
	for ( itemNum = 1; itemNum <= itemLim; ++itemNum ) {
		currItem = arrayNode->GetNodeAtIndex(itemNum);
		xmlLangQualifierNode = currItem->QualifiersIterator()->GetNode()->ConvertToSimpleNode();
		currItemValue = currItem->ConvertToSimpleNode();
		if ( !strcmp(xmlLangQualifierNode->GetValue()->c_str(),  specificLang ) ) {
			itemNode = currItem;
			return kXMP_CLT_SpecificMatch;
		}
	}
	
	if ( *genericLang != 0 ) {

		// Look for the first partial match with the generic language.
		const size_t genericLen = strlen ( genericLang );
		for ( itemNum = 1; itemNum <= itemLim; ++itemNum ) {
			currItem = arrayNode->GetNodeAtIndex(itemNum);
			xmlLangQualifierNode = currItem->GetQualifier( xmlNameSpace.c_str(), xmlNameSpace.size(),
				xmlLangQualifierName.c_str(), xmlLangQualifierName.size() )->ConvertToSimpleNode();
			XMP_StringPtr currLang = xmlLangQualifierNode->GetValue()->c_str();
			const size_t currLangSize = xmlLangQualifierNode->GetValue()->size();
			if ( (currLangSize >= genericLen) &&
				 XMP_LitNMatch ( currLang, genericLang, genericLen ) &&
				 ((currLangSize == genericLen) || (currLang[genericLen] == '-')) ) {
				itemNode = currItem;
				break;	// ! Don't return, need to look for other matches.
			}
		}

		if ( itemNum <= itemLim ) {
			
			// Look for a second partial match with the generic language.
			for ( ++itemNum; itemNum <= itemLim; ++itemNum ) {
				currItem = arrayNode->GetNodeAtIndex(itemNum);
				xmlLangQualifierNode = currItem->GetQualifier( xmlNameSpace.c_str(), xmlNameSpace.size(),
					xmlLangQualifierName.c_str(), xmlLangQualifierName.size() )->ConvertToSimpleNode();
				XMP_StringPtr currLang = xmlLangQualifierNode->GetValue()->c_str();
				const size_t currLangSize = xmlLangQualifierNode->GetValue()->size();
				if ( (currLangSize >= genericLen) &&
					 XMP_LitNMatch ( currLang, genericLang, genericLen ) &&
					 ((currLangSize == genericLen) || (currLang[genericLen] == '-')) ) {
					return kXMP_CLT_MultipleGeneric;	// ! Leave itemNode with the first partial match.
				}
			}
			return kXMP_CLT_SingleGeneric;	// No second partial match was found.

		}
		
	}
	
	// Look for an 'x-default' item.
	for ( itemNum = 1; itemNum <= itemLim; ++itemNum ) {
		currItem = arrayNode->GetNodeAtIndex(itemNum);
		xmlLangQualifierNode = currItem->GetQualifier( xmlNameSpace.c_str(), xmlNameSpace.size(),
			xmlLangQualifierName.c_str(), xmlLangQualifierName.size() )->ConvertToSimpleNode();
		if ( !XMP_LitMatch(xmlLangQualifierNode->GetValue()->c_str(), "x-default" ) ) {
			itemNode = currItem;
			return kXMP_CLT_XDefault;
		}
	}
	
	// Everything failed, choose the first item.
	itemNode = arrayNode->GetNodeAtIndex(1);
	return kXMP_CLT_FirstItem;
	
}	// ChooseLocalizedText



// -------------------------------------------------------------------------------------------------
// GetLocalizedText
// ----------------

bool
XMPMeta2::GetLocalizedText ( XMP_StringPtr	 schemaNS,
							XMP_StringPtr	 arrayName,
							XMP_StringPtr	 _genericLang,
							XMP_StringPtr	 _specificLang,
							XMP_StringPtr *	 actualLang,
							XMP_StringLen *	 langSize,
							XMP_StringPtr *	 itemValue,
							XMP_StringLen *	 valueSize,
							XMP_OptionBits * options ) const
{
	// TO DO : options
	XMP_Assert ( (schemaNS != 0) && (arrayName != 0) && (_genericLang != 0) && (_specificLang != 0) );	// Enforced by wrapper.
	XMP_Assert ( (actualLang != 0) && (langSize != 0) );	// Enforced by wrapper.
	XMP_Assert ( (itemValue != 0) && (valueSize != 0) && (options != 0) );	// Enforced by wrapper.

	XMP_VarString zGenericLang  ( _genericLang );
	XMP_VarString zSpecificLang ( _specificLang );
	NormalizeLangValue ( &zGenericLang );
	NormalizeLangValue ( &zSpecificLang );
	
	XMP_StringPtr genericLang  = zGenericLang.c_str();
	XMP_StringPtr specificLang = zSpecificLang.c_str();
	
	XMP_ExpandedXPath arrayPath;
	ExpandXPath ( schemaNS, arrayName, &arrayPath );
	spINode arrayNode, itemNode;
	XMP_OptionBits arrayOptions;
	if(!XMPUtils::FindCnstNode( this->mDOM, arrayPath, arrayNode, &arrayOptions)) return false;
	XMP_CLTMatch match = ChooseIXMPLocalizedText( arrayNode->ConvertToArrayNode(), arrayOptions, genericLang, specificLang, itemNode );
	if ( match == kXMP_CLT_NoValues ) return false;
	
	spISimpleNode qualifierNode = itemNode->GetQualifier( xmlNameSpace.c_str(), xmlNameSpace.size(), "lang", AdobeXMPCommon::npos )->ConvertToSimpleNode();
	*actualLang = qualifierNode->GetValue()->c_str();
	*langSize   = static_cast<XMP_StringLen>( qualifierNode->GetValue()->size() );
	spcIUTF8String itemNodeValue = itemNode->ConvertToSimpleNode()->GetValue();
	*itemValue  = itemNodeValue->c_str();  
	*valueSize  = static_cast<XMP_StringLen>( itemNodeValue->size() );
	*options    = XMPUtils::GetIXMPOptions(itemNode);
	return true;
	
}	// GetLocalizedText

// -------------------------------------------------------------------------------------------------
// DeleteLocalizedText
// -------------------


void
XMPMeta2::DeleteLocalizedText ( XMP_StringPtr schemaNS,
                               XMP_StringPtr arrayName,
                               XMP_StringPtr _genericLang,
                               XMP_StringPtr _specificLang )
{
	XMP_Assert ( (schemaNS != 0) && (arrayName != 0) && (_genericLang != 0) && (_specificLang != 0) );	// Enforced by wrapper.

	XMP_VarString zGenericLang  ( _genericLang );
	XMP_VarString zSpecificLang ( _specificLang );
	NormalizeLangValue ( &zGenericLang );
	NormalizeLangValue ( &zSpecificLang );

	XMP_StringPtr genericLang  = zGenericLang.c_str();
	XMP_StringPtr specificLang = zSpecificLang.c_str();

	XMP_ExpandedXPath arrayPath;
	ExpandXPath ( schemaNS, arrayName, &arrayPath );
	
	// Find the LangAlt array and the selected array item.
	spINode destNode, itemNode;
	spIArrayNode arrayNode;
	XMP_OptionBits arrayOptions;
	if(!XMPUtils::FindCnstNode( this->mDOM, arrayPath, destNode, &arrayOptions)) return;
	arrayNode = destNode->ConvertToArrayNode();
	size_t arraySize = arrayNode->ChildCount();
	XMP_CLTMatch match = ChooseIXMPLocalizedText( arrayNode->ConvertToArrayNode(), arrayOptions, genericLang, specificLang, itemNode );
	spcIUTF8String itemValue = itemNode->ConvertToSimpleNode()->GetValue();
	if ( match != kXMP_CLT_SpecificMatch ) return;


	size_t itemIndex = 1;
	for ( ; itemIndex <= arraySize; ++itemIndex ) {
		if ( arrayNode->GetNodeAtIndex(itemIndex) == itemNode ) break;
	}
	XMP_Enforce ( itemIndex <= arraySize );
	
	// Decide if the selected item is x-default or not, find relevant matching item.
	spISimpleNode qualNode ;
	bool itemIsXDefault = false;
	
	if ( itemNode->HasQualifiers() ) {
		qualNode = itemNode->GetQualifier( xmlNameSpace.c_str(), xmlNameSpace.size(), "lang", AdobeXMPCommon::npos )->ConvertToSimpleNode();
		if (XMP_LitMatch(qualNode->GetValue()->c_str(), "x-default"))  itemIsXDefault = true;
	}
	
	if ( itemIsXDefault && (itemIndex != 1) ) {	// Enforce the x-default is first policy.
        auto sp = arrayNode->GetNodeAtIndex( itemIndex );
		arrayNode->GetNodeAtIndex(1).swap( sp );
		itemIndex = 1;
	}

	spINode assocNode;
	size_t assocIndex = 0;
	size_t assocIsXDefault = false;
	if ( itemIsXDefault ) {

		for ( assocIndex = 2; assocIndex <= arraySize; ++assocIndex ) {
			spISimpleNode indexNode = arrayNode->GetNodeAtIndex( assocIndex )->ConvertToSimpleNode();
			if ( !strcmp(indexNode->GetValue()->c_str(), itemValue->c_str()) ) {
				assocNode = arrayNode->GetNodeAtIndex(assocIndex);
				break;
			}
		}

	} 
	else if ( itemIndex > 1 ) {

		spcIUTF8String itemOneValue = arrayNode->GetNodeAtIndex( 1 )->ConvertToSimpleNode()->GetValue();
		if ( !strcmp(itemOneValue->c_str(), itemValue->c_str()) ) {
			qualNode = arrayNode->GetNodeAtIndex( 1 )->GetQualifier( xmlNameSpace.c_str(), xmlNameSpace.size(), "lang", AdobeXMPCommon::npos )->ConvertToSimpleNode();
			if ( XMP_LitMatch(qualNode->GetValue()->c_str(),"x-default") ) {
				assocNode = arrayNode->GetNodeAtIndex(1);
				assocIndex = 1;
				assocIsXDefault = true;
			}
		}

	}
	if ( !assocIndex) {
		arrayNode->RemoveNodeAtIndex(itemIndex);
	} 
	else if ( itemIndex < assocIndex ) {
		arrayNode->RemoveNodeAtIndex(assocIndex);
		arrayNode->RemoveNodeAtIndex(itemIndex);		
	} 
	else {
		arrayNode->RemoveNodeAtIndex(itemIndex);
		arrayNode->RemoveNodeAtIndex(assocIndex);
	}

}	// DeleteLocalizedText
// -------------------------------------------------------------------------------------------------

// SetLocalizedText
// ----------------


void
XMPMeta2::SetLocalizedText ( XMP_StringPtr  schemaNS,
							XMP_StringPtr  arrayName,
							XMP_StringPtr  _genericLang,
							XMP_StringPtr  _specificLang,
							XMP_StringPtr  itemValue,
							XMP_OptionBits options )
{

	// is new DOM enforcing that first qualifier should be a lang alt
	IgnoreParam(options);

	XMP_Assert ( (schemaNS != 0) && (arrayName != 0) && (_genericLang != 0) && (_specificLang != 0) );	// Enforced by wrapper.

	XMP_VarString zGenericLang  ( _genericLang );
	XMP_VarString zSpecificLang ( _specificLang );
	NormalizeLangValue ( &zGenericLang );
	NormalizeLangValue ( &zSpecificLang );
	
	XMP_StringPtr genericLang  = zGenericLang.c_str();
	XMP_StringPtr specificLang = zSpecificLang.c_str();
	
	XMP_ExpandedXPath arrayPath;
	ExpandXPath ( schemaNS, arrayName, &arrayPath );
	
	// Find the array node and set the options if it was just created.
	spINode destNode;
	spIArrayNode arrayNode;
	XMP_OptionBits arrayOptions;
	if( !XMPUtils::FindCnstNode(this->mDOM, arrayPath, destNode) ) {
		
		XPathStepInfo  lastPathSegment( arrayPath.back());
		XMP_VarString arrayStep = lastPathSegment.step;
		XMP_Index insertIndex = 0;
		if (!XMPUtils::FindNode(this->mDOM, arrayPath, kXMP_CreateNodes, kXMP_PropArrayIsAlternate | kXMP_PropValueIsArray, destNode, &insertIndex)) {
			XMP_Throw ( "Failure creating array node", kXMPErr_BadXPath );
		}
		
	}

	arrayNode = destNode->ConvertToArrayNode();
	arrayOptions = XMPUtils::GetIXMPOptions(arrayNode);
	
	
	
	size_t arrayChildCount = arrayNode->ChildCount();
	if ( !arrayNode  ) XMP_Throw ( "Failed to find or create array node", kXMPErr_BadXPath );
	if ( ! XMP_ArrayIsAltText(arrayOptions) ) {
		if ( !arrayChildCount && XMP_ArrayIsAlternate(arrayOptions) ) {
			arrayOptions |= kXMP_PropArrayIsAltText;
		} 
		else {
			XMP_Throw ( "Localized text array is not alt-text", kXMPErr_BadXPath );
		}
	}
	
	// Make sure the x-default item, if any, is first.
	
	size_t itemNum, itemLim;
	spcISimpleNode firstQualifier;
	spINode xdItem;
	bool haveXDefault = false;
	
	for ( itemNum = 1, itemLim = arrayNode->ChildCount(); itemNum <= itemLim; ++itemNum ) {
		spINode currItem = arrayNode->GetNodeAtIndex(itemNum);
		
		XMP_Assert (XMP_PropHasLang(XMPUtils::GetIXMPOptions(currItem)));
		if(!currItem->HasQualifiers()) {
			XMP_Throw ( "Language qualifier must be first", kXMPErr_BadXPath );
		}
		firstQualifier = currItem->QualifiersIterator()->GetNode()->ConvertToSimpleNode();
		if (!XMP_LitMatch(firstQualifier->GetName()->c_str(),"lang"))  {
			XMP_Throw ( "Language qualifier must be first", kXMPErr_BadXPath );
		}
		if (XMP_LitMatch(firstQualifier->GetValue()->c_str(),"x-default" )) {
			xdItem = currItem;
			haveXDefault = true;
			break;
		}
	}
	
	if ( haveXDefault && (itemNum != 1) ) {
		//TODO or not to do
		XMP_Assert ( XMP_LitMatch(firstQualifier->GetValue()->c_str(), "x-default") );
		spcISimpleNode tempNode = arrayNode->GetNodeAtIndex( itemNum )->GetQualifier( xmlNameSpace.c_str(), xmlNameSpace.size(), "lang", AdobeXMPCommon::npos )->ConvertToSimpleNode();
		
		firstQualifier.swap(tempNode);
	}
	
	spINode itemNode;	
	spcIUTF8String xdValue, itemNodeValue;
	if(xdItem && xdItem->GetNodeType() == INode::kNTSimple) {
		xdValue = xdItem->ConvertToSimpleNode()->GetValue();
	}
	XMP_CLTMatch match = ChooseIXMPLocalizedText ( arrayNode->ConvertToArrayNode(), arrayOptions, genericLang, specificLang, itemNode);
	if(itemNode && itemNode->GetNodeType() == INode::kNTSimple) {
		itemNodeValue = itemNode->ConvertToSimpleNode()->GetValue();
	}
	const bool specificXDefault = XMP_LitMatch ( specificLang, "x-default" );
	switch ( match ) {

		case kXMP_CLT_NoValues :

			// Create the array items for the specificLang and x-default, with x-default first.
			AppendIXMPLangItem ( arrayNode, "x-default", itemValue );
			haveXDefault = true;
			if ( ! specificXDefault ) AppendIXMPLangItem ( arrayNode, specificLang, itemValue );
			break;
			
		case kXMP_CLT_SpecificMatch :
		
			if ( ! specificXDefault ) {
				// Update the specific item, update x-default if it matches the old value.
				if ( xdItem  && haveXDefault && (xdItem != itemNode) && (XMP_LitMatch(xdValue->c_str(), itemNodeValue->c_str())) ) {
					XMPUtils::SetNode ( xdItem,  itemValue, XMPUtils::GetIXMPOptions( xdItem));
				}
				XMPUtils::SetNode(itemNode, itemValue,XMPUtils:: GetIXMPOptions(itemNode));
				
			} else {
				// Update all items whose values match the old x-default value.
				XMP_Assert ( xdItem && haveXDefault && (xdItem.get() == itemNode.get()) );
				for ( itemNum = 1, itemLim = arrayNode->ChildCount(); itemNum <= itemLim; ++itemNum ) {
					spISimpleNode currItem = arrayNode->GetNodeAtIndex( itemNum )->ConvertToSimpleNode();
					if ( (currItem.get() == xdItem.get() ) || (strcmp(currItem->GetValue()->c_str(), xdValue->c_str()) )) continue;
					XMPUtils::SetNode ( currItem, itemValue, XMPUtils::GetIXMPOptions(currItem) );
				}
				
				XMPUtils::SetNode( xdItem, itemValue,XMPUtils:: GetIXMPOptions(xdItem));
			}
			break;

		case kXMP_CLT_SingleGeneric :
		
			// Update the generic item, update x-default if it matches the old value.
			if (  xdItem && haveXDefault && (xdItem != itemNode) && (XMP_LitMatch(xdValue->c_str(),itemNodeValue->c_str()) ) ) {
				XMPUtils::SetNode ( xdItem, itemValue, XMPUtils::GetIXMPOptions(xdItem) );
			}
			XMPUtils::SetNode( itemNode, itemValue,XMPUtils:: GetIXMPOptions(itemNode) );	// ! Do this after the x-default check!
			break;

		case kXMP_CLT_MultipleGeneric :
		
			// Create the specific language, ignore x-default.
			AppendIXMPLangItem ( arrayNode, specificLang, itemValue );
			if ( specificXDefault ) haveXDefault = true;
			break;
			
		case kXMP_CLT_XDefault :

			// Create the specific language, update x-default if it was the only item.
			if ( arrayNode->ChildCount()== 1 ) XMPUtils::SetNode ( xdItem, itemValue, XMPUtils::GetIXMPOptions(xdItem) );
			AppendIXMPLangItem ( arrayNode, specificLang, itemValue );
			break;

		case kXMP_CLT_FirstItem	:

			// Create the specific language, don't add an x-default item.
			AppendIXMPLangItem ( arrayNode, specificLang, itemValue );
			if ( specificXDefault ) haveXDefault = true;
			break;
			
		default :
			XMP_Throw ( "Unexpected result from ChooseLocalizedText", kXMPErr_InternalFailure );

	}

	// Add an x-default at the front if needed.
	if ( (! haveXDefault) && (arrayNode->ChildCount() == 1) ) {
		AppendIXMPLangItem ( arrayNode, "x-default", itemValue );
	}

}	// SetLocalizedText




// -------------------------------------------------------------------------------------------------
// DumpPropertyTree
// ----------------

// *** Extract the validation code into a separate routine to call on exit in debug builds.

static void
DumpIXMPPropertyTree ( const spcINode &  currNode,
				   int				  indent,
				   size_t			  itemIndex,
				   XMP_TextOutputProc outProc,
				   void *			  refCon )
{
	if(!currNode) return;
	char buffer [32];	// Decimal of a 64 bit int is at most about 20 digits.
	XMP_OptionBits options = XMPUtils::GetIXMPOptions(currNode);
	auto defaultMap = INameSpacePrefixMap::GetDefaultNameSpacePrefixMap();
	XMP_VarString currNameSpace = defaultMap->GetPrefix(currNode->GetNameSpace()->c_str(), currNode->GetNameSpace()->size() )->c_str();
	XMP_VarString nodeFullName = currNameSpace + ":" + currNode->GetName()->c_str();
	
	OutProcIndent ( (size_t)indent );

	size_t  childCount = 0;
	if ( itemIndex == 0 ) {
		if ( options & kXMP_PropIsQualifier ) OutProcNChars ( "? ", 2 );
		DumpClearString ( nodeFullName, outProc, refCon );
	} 
	else {
		OutProcNChars ( "[", 1 );
		OutProcDecInt ( itemIndex );
		OutProcNChars ( "]", 1 );
	}

	if ( ! (options & kXMP_PropCompositeMask) ) {
		OutProcNChars ( " = \"", 4 );
		DumpClearString ( currNode->ConvertToSimpleNode()->GetValue()->c_str(), outProc, refCon );
		OutProcNChars ( "\"", 1 );
	}

	if ( options != 0 ) {
		OutProcNChars ( "  ", 2 );
		DumpNodeOptions ( options, outProc, refCon );
	}

	if ( options & kXMP_PropHasLang ) {
		
		spcISimpleNode firstQualifier = currNode->QualifiersIterator()->GetNode()->ConvertToSimpleNode();
		if ( !currNode->HasQualifiers() || !(XMP_LitMatch(firstQualifier->GetName()->c_str(), "lang") ) ) {
			OutProcLiteral ( "  ** bad lang flag **" );
		}
	}
	// *** Check rdf:type also.

	if ( ! (options & kXMP_PropCompositeMask) ) {
		if(currNode->GetNodeType() == INode::kNTArray) {
			childCount = currNode->ConvertToArrayNode()->ChildCount();
		}
		if(currNode->GetNodeType() == INode::kNTStructure) {
			childCount = currNode->ConvertToStructureNode()->ChildCount();
		}
		if ( childCount ) OutProcLiteral ( "  ** bad children **" );
	
	} 
	else if ( options & kXMP_PropValueIsArray ) {
		if ( options & kXMP_PropValueIsStruct ) OutProcLiteral ( "  ** bad comp flags **" );
	} 
	else if ( (options & kXMP_PropCompositeMask) != kXMP_PropValueIsStruct ) {
		OutProcLiteral ( "  ** bad comp flags **" );
	}

	OutProcNewline();
	
	if( currNode->HasQualifiers() ) {
		auto qualIter = currNode->QualifiersIterator();
		for (size_t qualNum = 0 ; qualIter; qualIter = qualIter->Next(), qualNum++ ) {
		spcINode currQual = qualIter->GetNode();
		XMP_OptionBits currQualOptions = XMPUtils::GetIXMPOptions(currQual);
		if ( currQual->GetParent() && currQual->GetParent()->GetParent()!= currNode ) OutProcLiteral ( "** bad parent link => " );
		if ( XMP_LitMatch(currQual->GetName()->c_str(), kXMP_ArrayItemName ) ) OutProcLiteral ( "** bad qual name => " );
		if ( ! (currQualOptions & kXMP_PropIsQualifier) ) OutProcLiteral ( "** bad qual flag => " );
		if ( XMP_LitMatch(currQual->GetName()->c_str(), "lang" )) {
			if ( (qualNum != 0) || (! (options & kXMP_PropHasLang)) ) OutProcLiteral ( "** bad lang qual => " );
		}

		DumpIXMPPropertyTree ( currQual, indent + 2, 0, outProc, refCon );

		}
	}
	spcINodeIterator childIter;
	if(currNode->GetNodeType() == INode::kNTArray) {
		childIter = currNode->ConvertToArrayNode()->Iterator();
	}
	if(currNode->GetNodeType() == INode::kNTStructure) {
		childIter = currNode->ConvertToStructureNode()->Iterator();
	}
	for (size_t childNum = 0; childIter; childIter = childIter->Next(), childNum++)  {
	    spcINode currentChild = childIter->GetNode();
		XMP_OptionBits currentChildOptions = XMPUtils::GetIXMPOptions( currentChild);
		if( !currentChild) continue; 
  	    if ( currentChild->GetParent() != currNode ) OutProcLiteral ( "** bad parent link => " );
		if ( currentChildOptions & kXMP_PropIsQualifier ) OutProcLiteral ( "** bad qual flag => " );

		if ( options & kXMP_PropValueIsArray ) {
			
			itemIndex = childNum + 1;
			if (XMP_LitMatch(currentChild->GetName()->c_str(), kXMP_ArrayItemName) ) OutProcLiteral ( "** bad item name => " );
		} 
		else {
			
			itemIndex = 0;
			if ( XMP_LitMatch(currentChild->GetName()->c_str(), kXMP_ArrayItemName ) ) OutProcLiteral ( "** bad field name => " );
		}

		DumpIXMPPropertyTree ( currentChild, indent + 1, itemIndex, outProc, refCon );

	}

}	// DumpPropertyTree


// -------------------------------------------------------------------------------------------------
// DumpObject
// ----------

void 
XMPMeta2::DumpObject ( XMP_TextOutputProc outProc,
                      void *             refCon ) const
{
	// TODO
	// value of mDOM ?
	
	
	XMP_Assert ( outProc != 0 );	// ! Enforced by wrapper.
	auto defaultMap = INameSpacePrefixMap::GetDefaultNameSpacePrefixMap();
	OutProcLiteral ( "Dumping XMPMeta object \"" );
	DumpClearString (mDOM->GetAboutURI()->c_str(), outProc, refCon );
	OutProcNChars ( "\"  ", 3 );
	DumpNodeOptions ( XMPUtils::GetIXMPOptions(mDOM), outProc, refCon );
	OutProcNewline();

	// One can't possibly allocate mDOM a value ?!
	/*
	if ( ! tree.value.empty() ) {
		OutProcLiteral ( "** bad root value **  \"" );
		DumpClearString ( tree.value, outProc, refCon );
		OutProcNChars ( "\"", 1 );
		OutProcNewline();
	}
	*/
	if ( mDOM->HasQualifiers() ) {
		OutProcLiteral ( "** bad root qualifiers **" );
		OutProcNewline();
		spINodeIterator qualIter = mDOM->QualifiersIterator();
		for ( ; qualIter; qualIter = qualIter->Next() ) {
			DumpIXMPPropertyTree ( qualIter->GetNode(), 3, 0, outProc, refCon );
		}
	}
	map<std::string, bool> schemaUsed;
	if ( mDOM->ChildCount() ) {

		spINodeIterator childIter = mDOM->Iterator(); 
		for ( ;childIter; childIter = childIter->Next() ) {

			spINode currSchema = childIter->GetNode();
			XMP_OptionBits currSchemaOptions = kXMP_SchemaNode;
			if(!schemaUsed.count(currSchema->GetNameSpace()->c_str())) {
				OutProcNewline();
				OutProcIndent ( 1 );
				XMP_VarString prefix = defaultMap->GetPrefix(currSchema->GetNameSpace()->c_str(), currSchema->GetNameSpace()->size() )->c_str();
				prefix += ":";
				DumpClearString ( prefix.c_str(), outProc, refCon );
				OutProcNChars ( "  ", 2 );
				DumpClearString ( currSchema->GetNameSpace()->c_str(), outProc, refCon );
				OutProcNChars ( "  ", 2 );
				DumpNodeOptions ( currSchemaOptions, outProc, refCon );
				OutProcNewline();

				if ( ! (currSchemaOptions & kXMP_SchemaNode) ) {
					OutProcLiteral ( "** bad schema options **" );
					OutProcNewline();
				}
			
				schemaUsed[currSchema->GetNameSpace()->c_str()] = true;
			}
			
			DumpIXMPPropertyTree ( currSchema, 2, 0, outProc, refCon );
			
		}

	}

}	// DumpObject



// -------------------------------------------------------------------------------------------------
// SetErrorCallback
// ----------------

void
XMPMeta2::SetErrorCallback( XMPMeta_ErrorCallbackWrapper wrapperProc,
							XMPMeta_ErrorCallbackProc    clientProc,
							void *    context,
							XMP_Uns32 limit)
{
	XMP_Assert(wrapperProc != 0);	// Must always be set by the glue;

	this->errorCallback.Clear();
	this->errorCallback.wrapperProc = wrapperProc;
	this->errorCallback.clientProc = clientProc;
	this->errorCallback.context = context;
	this->errorCallback.limit = limit;
	spParser->GetIDOMParser_I()->SetErrorCallback(&errorCallback);

}	// SetErrorCallback

// -------------------------------------------------------------------------------------------------
// ResetErrorCallbackLimit
// -----------------------

void
XMPMeta2::ResetErrorCallbackLimit(XMP_Uns32 limit)
{

	this->errorCallback.limit = limit;
	this->errorCallback.notifications = 0;
	this->errorCallback.topSeverity = kXMPErrSev_Recoverable;
	spParser->GetIDOMParser_I()->SetErrorCallback(&errorCallback);

}	// ResetErrorCallbackLimit
#endif
