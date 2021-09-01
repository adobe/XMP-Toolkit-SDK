// =================================================================================================
// Copyright 2003 Adobe
// All Rights Reserved.
//
// NOTICE:	Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! This must be the first include!
#include "XMPCore/source/XMPCore_Impl.hpp"

#include "XMPCore/source/XMPUtils.hpp"

#include "third-party/zuid/interfaces/MD5.h"


#include <map>

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <errno.h>
#include <vector>

#include <stdio.h>	// For snprintf.
#if ENABLE_CPP_DOM_MODEL
#include "source/UnicodeInlines.incl_cpp"
#include "source/UnicodeConversions.hpp"
#include "XMPMeta2.hpp"
#include "XMPCore/Interfaces/IMetadata_I.h"
#include "XMPCore/Interfaces/IArrayNode_I.h"
#include "XMPCore/Interfaces/ISimpleNode_I.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap.h"
#include "XMPCore/Interfaces/INodeIterator.h"
using namespace AdobeXMPCore;
using namespace AdobeXMPCommon;
#endif

#if XMP_WinBuild
	#pragma warning ( disable : 4800 )	// forcing value to bool 'true' or 'false' (performance warning)
	#pragma warning ( disable : 4996 )	// '...' was declared deprecated
#endif

// =================================================================================================
// Local Types and Constants
// =========================

static const char * sBase64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const XMP_VarString xmlNameSpace  = "http://www.w3.org/XML/1998/namespace";
// =================================================================================================
// Local Utilities
// ===============

// -------------------------------------------------------------------------------------------------
// SetINode
// --------------

#if ENABLE_CPP_DOM_MODEL

void XMPUtils::SetNode	( const spINode & node , XMP_StringPtr value, XMP_OptionBits options )
{
	if (!node) return;
	//TO DO check UTF-8
	if ( options & kXMP_DeleteExisting ) {
		XMP_ClearOption ( options, kXMP_DeleteExisting );
		node->Clear();
	}
	if ( value != 0 ) {
		
		if ( options & kXMP_PropCompositeMask ) XMP_Throw ( "Composite nodes can't have values", kXMPErr_BadXPath );
		if ( !node ) return;
		XMP_Assert( node->GetNodeType() == INode::kNTSimple);
		spISimpleNode simpleNode = node->ConvertToSimpleNode();
		std::string newValue = value;	
	
		XMP_Uns8* chPtr = (XMP_Uns8*) newValue.c_str();	// Check for valid UTF-8, replace ASCII controls with a space.
		while ( *chPtr != 0 ) {
			while ( (*chPtr != 0) && (*chPtr < 0x80) ) {
				if ( *chPtr < 0x20 ) {
					if ( (*chPtr != kTab) && (*chPtr != kLF) && (*chPtr != kCR) ) *chPtr = 0x20;
				}	
				else if (*chPtr == 0x7F ) {
				*chPtr = 0x20;
				}
			++chPtr;
			}

			XMP_Assert ( (*chPtr == 0) || (*chPtr >= 0x80) );

			if ( *chPtr != 0 ) {
				XMP_Uns32 cp = GetCodePoint ( (const XMP_Uns8 **) &chPtr );	// Throws for bad UTF-8.
				if ( (cp == 0xFFFE) || (cp == 0xFFFF) ) {
					XMP_Throw ( "U+FFFE and U+FFFF are not allowed in XML", kXMPErr_BadUnicode );
				}
			}

		}
		if ( XMP_PropIsQualifier(options) && XMP_LitMatch(node->GetNameSpace()->c_str(), xmlNameSpace.c_str()) && XMP_LitMatch(node->GetName()->c_str(), "lang")) NormalizeLangValue ( &newValue );

		simpleNode->SetValue(newValue.c_str(), newValue.size());
	}
	else {

		if((node->GetNodeType() == INode::kNTStructure && (options & kXMP_PropValueIsArray)) ||  (node->GetNodeType() == INode::kNTArray && (options & kXMP_PropValueIsStruct))) {
				XMP_Throw ( "Requested and existing composite form mismatch", kXMPErr_BadXPath );
		}
		node->Clear();
		
	}
	
}	// SetINode
XMP_OptionBits  XMPUtils::ConvertNewArrayFormToOldArrayForm (const spcIArrayNode & arrayNode) {
	
	XMP_OptionBits options = 0;
	if(!arrayNode) return options;
	if( arrayNode->GetArrayForm() == IArrayNode::kAFAlternative) return kXMP_PropArrayIsAlternate;
	if( arrayNode->GetArrayForm() == IArrayNode::kAFOrdered) return kXMP_PropArrayIsOrdered;
	if( arrayNode->GetArrayForm() == IArrayNode::kAFUnordered) return kXMP_PropArrayIsUnordered;
	return 0;
}

spINode  XMPUtils::CreateArrayChildNode( const spIArrayNode & arrayNode, XMP_OptionBits options) {
	
	XMP_VarString nodeNameSpace = arrayNode->GetNameSpace()->c_str(), nodeName = arrayNode->GetName()->c_str();
	spINode itemNode;
	size_t arrayChildCount = arrayNode->ChildCount();
	spINode firstChildNode;
	if (!arrayChildCount) {
		itemNode = XMPUtils::CreateTerminalNode(nodeNameSpace.c_str(), nodeName.c_str(), options);
		return itemNode;
	}
	if(arrayChildCount) firstChildNode = arrayNode->GetNodeAtIndex(1);
	XMP_OptionBits childOptions = 0;
	 if(firstChildNode && firstChildNode->GetNodeType() == INode::kNTArray) {
		 childOptions = ConvertNewArrayFormToOldArrayForm(firstChildNode->ConvertToArrayNode());
	 }
	if(XMP_PropIsStruct(options) && (!arrayChildCount || firstChildNode->GetNodeType() == INode::kNTStructure ) ) {
		itemNode = IStructureNode::CreateStructureNode(nodeNameSpace.c_str(), nodeNameSpace.size(), nodeName.c_str(), nodeName.size() );
	}
	else if(XMP_PropIsSimple(options) && (!arrayChildCount || firstChildNode->GetNodeType() == INode::kNTSimple ) ) {
		itemNode = ISimpleNode::CreateSimpleNode(nodeNameSpace.c_str(), nodeNameSpace.size(), nodeName.c_str(), nodeName.size(), "", 0 );
	}
	else if((options & kXMP_PropArrayIsAlternate) && (childOptions & kXMP_PropArrayIsAlternate) ) {
		itemNode = IArrayNode::CreateAlternativeArrayNode( nodeNameSpace.c_str(), nodeNameSpace.size(), nodeName.c_str(), nodeName.size() );
	}
	else if((options & kXMP_PropArrayIsOrdered)  && (childOptions & kXMP_PropArrayIsOrdered)) {
		itemNode = IArrayNode::CreateOrderedArrayNode( nodeNameSpace.c_str(), nodeNameSpace.size(), nodeName.c_str(), nodeName.size() );
	}
	else if((options & kXMP_PropArrayIsUnordered) && (childOptions & kXMP_PropArrayIsUnordered)) {
		itemNode = IArrayNode::CreateUnorderedArrayNode( nodeNameSpace.c_str(), nodeNameSpace.size(), nodeName.c_str(), nodeName.size() );
	}
	if(!itemNode) XMP_Throw("Array has to be homogeneous", kXMPErr_BadXPath);

	return itemNode;

}

void
 XMPUtils::DoSetArrayItem ( const spIArrayNode &arrayNode,
				 XMP_Index		itemIndex,
				 XMP_StringPtr	itemValue,
				 XMP_OptionBits options )
{
	XMP_OptionBits itemLoc = options & kXMP_PropArrayLocationMask;
	XMP_Index      arraySize = static_cast<XMP_Index>( arrayNode->ChildCount() );
	XMP_VarString arrayNameSpace = arrayNode->GetNameSpace()->c_str();
	XMP_VarString arrayName = arrayNode->GetName()->c_str();
	options &= ~kXMP_PropArrayLocationMask;
	options = VerifySetOptions ( options, itemValue );
	
	// Now locate or create the item node and set the value. Note the index parameter is one-based!
	// The index can be in the range [0..size+1] or "last", normalize it and check the insert flags.
	// The order of the normalization checks is important. If the array is empty we end up with an
	// index and location to set item size+1.
	
	
	
	spINode itemNode;
	if ( itemIndex == kXMP_ArrayLastItem ) itemIndex = arraySize;
	if ( (itemIndex == 0) && (itemLoc == kXMP_InsertAfterItem) ) {
		itemIndex = 1;
		itemLoc = kXMP_InsertBeforeItem;
	}
	if ( (itemIndex == arraySize) && (itemLoc == kXMP_InsertAfterItem) ) {
		itemIndex += 1;
		itemLoc = 0;
	}
	if ( (itemIndex == arraySize + 1) && (itemLoc == kXMP_InsertBeforeItem) ) itemLoc = 0;
	
	if ( itemIndex == arraySize + 1 ) {

		if ( itemLoc ) XMP_Throw ( "Can't insert before or after implicit new item", kXMPErr_BadIndex );
		itemNode = CreateArrayChildNode(arrayNode, options);
		arrayNode->InsertNodeAtIndex(itemNode, arraySize + 1);
	} 
	else {
		
		if ( (itemIndex < 1) || (itemIndex > arraySize) ) XMP_Throw ( "Array index out of bounds", kXMPErr_BadIndex );
	
		if ( itemLoc == 0 ) {
			itemNode = arrayNode->GetNodeAtIndex( itemIndex )->ConvertToSimpleNode();
		} 
		else {
			itemNode = CreateArrayChildNode(arrayNode, options);
			if ( itemLoc == kXMP_InsertAfterItem ) ++itemIndex;
			arrayNode->InsertNodeAtIndex (itemNode, itemIndex );
		}

	}
	SetNode(itemNode, itemValue, options);

}	// DoSetIXMPArrayItem


void  XMPUtils::SetImplicitNodeInformation( bool & firstImplicitNodeFound,
								spINode  &implicitNodeRoot,
								spINode &destNode,
								XMP_Index &implicitNodeIndex,
								XMP_Index index )
{
	if(!firstImplicitNodeFound) {
		implicitNodeRoot = destNode;
		firstImplicitNodeFound = true;
		if( index) {
			implicitNodeIndex = index;
		}
	}
}
void  XMPUtils::GetNameSpaceAndNameFromStepValue( const std::string & stepStr,
									  const spcINameSpacePrefixMap & defaultMap,
									  std::string &stepNameSpace,
									  std::string &stepName) 
{	
	size_t colonPos = stepStr.find(':');
	XMP_VarString prefix = stepStr.substr( 0, colonPos );
	stepNameSpace =  defaultMap->GetNameSpace( prefix.c_str(), prefix.size() )->c_str();
	stepName = stepStr.substr( colonPos + 1);
}

// =================================================================================================
// FindNode
// ========
//
// Follow an expanded path expression to find or create a node. Returns a pointer to the node, and
// optionally an iterator for the node's position in the parent's vector of children or qualifiers.
// The iterator is unchanged if no child node (null) is returned.


spINode XMPUtils::CreateTerminalNode(const char* nameSpace, const char * name, XMP_OptionBits options) {

	spINode newNode;
	if(XMP_PropIsSimple(options)) {

		spISimpleNode simpleNode = ISimpleNode::CreateSimpleNode(nameSpace, AdobeXMPCommon::npos, name, AdobeXMPCommon::npos, NULL, 0 );
		newNode = simpleNode;
	}
	if(XMP_PropIsStruct(options)){

		spIStructureNode structNode = IStructureNode::CreateStructureNode(nameSpace, AdobeXMPCommon::npos, name, AdobeXMPCommon::npos);
		newNode = structNode;
	}
	if(XMP_PropIsArray(options)) {

		if ( options & kXMP_PropArrayIsAlternate )
			newNode = IArrayNode::CreateAlternativeArrayNode( nameSpace, AdobeXMPCommon::npos, name, AdobeXMPCommon::npos );
		else if(options & kXMP_PropArrayIsOrdered)
			newNode = IArrayNode::CreateOrderedArrayNode( nameSpace, AdobeXMPCommon::npos, name, AdobeXMPCommon::npos );
		else
			newNode = IArrayNode::CreateUnorderedArrayNode( nameSpace, AdobeXMPCommon::npos, name, AdobeXMPCommon::npos );
	}

	return newNode;
}

bool XMPUtils::HandleConstAliasStep(const spIMetadata & mDOM,
					spINode &destNode,
					const XMP_ExpandedXPath & expandedXPath,
					XMP_Index           *nodeIndex
					)
{
	destNode = mDOM;
	if (expandedXPath.empty()) XMP_Throw("Empty XPath", kXMPErr_BadXPath);
	if (!(expandedXPath[kRootPropStep].options & kXMP_StepIsAlias)) {

		return false;

	}
	else {

		XMP_AliasMapPos aliasPos = sRegisteredAliasMap->find(expandedXPath[kRootPropStep].step);
		XMP_Assert(aliasPos != sRegisteredAliasMap->end());
		XMP_VarString namespaceName = aliasPos->second[kSchemaStep].step.c_str();
		size_t colonPos = aliasPos->second[kRootPropStep].step.find(":");
		XMP_Assert(colonPos != std::string::npos);
		XMP_VarString propertyName = aliasPos->second[kRootPropStep].step.substr( colonPos + 1);
		destNode = mDOM->GetNode( namespaceName.c_str(), namespaceName.size(), propertyName.c_str(), propertyName.size() );
		if (!destNode) return false;
		if (aliasPos->second.size() == 2) return true;
		XMP_Assert(destNode->GetNodeType() == INode::kNTArray);
		if (aliasPos->second[2].options == kXMP_ArrayIndexStep) {

			XMP_Assert(aliasPos->second[2].step == "[1]");
			destNode = destNode->ConvertToArrayNode()->GetNodeAtIndex( 1 );
			INode::eNodeType actualNodeType = destNode->GetNodeType();
			if (destNode) {
				if (nodeIndex) *nodeIndex = 1;
				return true;
			}
			return false;
		}	
		else if (aliasPos->second[2].options == kXMP_QualSelectorStep) {
			XMP_Assert(aliasPos->second[2].step == "[?xml:lang=\"x-default\"]");
			spcINodeIterator iter = XMPUtils::GetNodeChildIterator(destNode);
			XMP_Index index = 1;
			while (iter) {
				spcINode node = iter->GetNode();
				spcINode qualNode = node->GetQualifier(xmlNameSpace.c_str(), xmlNameSpace.size(), "lang", AdobeXMPCommon::npos );
				if (qualNode->GetNodeType() == INode::kNTSimple) {
					if (!strcmp("x-default", qualNode->ConvertToSimpleNode()->GetValue()->c_str())){
						destNode = AdobeXMPCore_Int::const_pointer_cast<INode>(node);
						if (nodeIndex) *nodeIndex = index;
						return true;
					}
				}
				index++;
				iter =iter->Next();
			}
			return false;
		}
		
		return false; 
		
	}
}
bool XMPUtils::HandleAliasStep(const spIMetadata &  mDOM,
	XMP_ExpandedXPath  &expandedXPath,
	bool				   createNodes,
	XMP_OptionBits	   leafOptions /* = 0 */,
	spINode       &destNode,
	XMP_Index           *nodeIndex,
	bool ignoreLastStep
	)
	
{
	destNode = mDOM;
	bool isAliasBeingCreated = expandedXPath.size() == 2;
	if (expandedXPath.empty()) XMP_Throw("Empty XPath", kXMPErr_BadXPath);
	if (!(expandedXPath[kRootPropStep].options & kXMP_StepIsAlias)) {

		return false;

	}
	else {

		XMP_AliasMapPos aliasPos = sRegisteredAliasMap->find(expandedXPath[kRootPropStep].step);
		XMP_Assert(aliasPos != sRegisteredAliasMap->end());
		XMP_VarString namespaceName = aliasPos->second[kSchemaStep].step.c_str();
		size_t colonPos = aliasPos->second[kRootPropStep].step.find(":");
		XMP_Assert(colonPos != std::string::npos);
		XMP_VarString propertyName = aliasPos->second[kRootPropStep].step.substr(colonPos + 1);
		destNode = mDOM->GetNode(namespaceName.c_str(), namespaceName.size(), propertyName.c_str(), propertyName.size() );
		if (!destNode && !createNodes) return false;
		if (aliasPos->second.size() == 2) {
			if (destNode) return true;
			XMP_OptionBits createOptions = 0;
			destNode = mDOM;
			if (isAliasBeingCreated) createOptions = leafOptions;
			spINode tempNode = CreateTerminalNode(namespaceName.c_str(), propertyName.c_str(), createOptions);
			if (!tempNode) return false;
			destNode->ConvertToStructureNode()->AppendNode( tempNode );
			destNode = tempNode;
			if (destNode) return true;
			return false;
		}

		
		//XMP_Assert(destNode->GetNodeType() == INode::kNTArray);
		XMP_Assert(aliasPos->second.size() == 3);
		if (aliasPos->second[2].options == kXMP_ArrayIndexStep) {
			XMP_Assert(aliasPos->second[2].step == "[1]");
			destNode = mDOM->GetNode(namespaceName.c_str(), namespaceName.size(), propertyName.c_str(), propertyName.size() );
			if (!destNode && !createNodes) return false;
			if (!destNode) {
				spINode arrayNode = CreateTerminalNode(namespaceName.c_str(), propertyName.c_str(), kXMP_PropArrayIsOrdered | kXMP_PropValueIsArray);
				mDOM->AppendNode(arrayNode);
				destNode = arrayNode;
			}
		
			if ( destNode->ConvertToArrayNode()->GetNodeAtIndex( 1 ) ) {
				destNode = destNode->ConvertToArrayNode()->GetNodeAtIndex( 1 );
				if (nodeIndex) *nodeIndex = 1;
				return true;
			}
			else {
				spISimpleNode indexNode = ISimpleNode::CreateSimpleNode( namespaceName.c_str(), namespaceName.size(), "[]", AdobeXMPCommon::npos, "", AdobeXMPCommon::npos );
				destNode->ConvertToArrayNode()->InsertNodeAtIndex( indexNode, 1 );
				destNode = destNode->ConvertToArrayNode()->GetNodeAtIndex( 1 );
				return true;
			}
			return false;
		}
		else if (aliasPos->second[2].options == kXMP_QualSelectorStep) {
			XMP_Assert(aliasPos->second[2].step == "[?xml:lang=\"x-default\"]");
			destNode = mDOM->GetNode(namespaceName.c_str(), namespaceName.size(), propertyName.c_str(), propertyName.size() );
			if (!destNode && !createNodes) return false;
			spINode arrayNode = CreateTerminalNode(namespaceName.c_str(), propertyName.c_str(), kXMP_PropArrayIsAltText | kXMP_PropValueIsArray);
			mDOM->AppendNode(arrayNode);
			destNode = arrayNode;
			spcINodeIterator iter = XMPUtils::GetNodeChildIterator(destNode);
			XMP_Index index = 1;
			while (iter) {
				spcINode node = iter->GetNode();
				spcINode qualNode = node->GetQualifier(xmlNameSpace.c_str(), xmlNameSpace.size(), "lang", AdobeXMPCommon::npos );
				if (qualNode->GetNodeType() == INode::kNTSimple) {
					if (!strcmp("x-default", qualNode->ConvertToSimpleNode()->GetValue()->c_str())){
						destNode = AdobeXMPCore_Int::const_pointer_cast<INode>(node);
						if (nodeIndex) *nodeIndex = index;
						return true;
					}
				}
				index++;
				iter = iter->Next();
			}
			spISimpleNode qualifierNode = ISimpleNode::CreateSimpleNode(xmlNameSpace.c_str(), xmlNameSpace.size(), "lang", AdobeXMPCommon::npos, "x-default" );
			if ( destNode->ConvertToArrayNode()->GetNodeAtIndex( 1 ) ) {
				destNode = destNode->ConvertToArrayNode()->GetNodeAtIndex( 1 );
				if (nodeIndex) *nodeIndex = 1;
				destNode->InsertQualifier(qualifierNode);

				return true;
			}
			else {
				spISimpleNode indexNode = ISimpleNode::CreateSimpleNode(namespaceName.c_str(), namespaceName.size(), "[]", AdobeXMPCommon::npos );
				destNode->ConvertToArrayNode()->InsertNodeAtIndex( indexNode, 1 );
				destNode->InsertQualifier(qualifierNode);
				destNode = destNode->ConvertToArrayNode()->GetNodeAtIndex( 1 );
				return true;
			}
		
		}

		

	}
	return false;
}
bool XMPUtils:: FindNode ( const spIMetadata  & mDOM,
		   XMP_ExpandedXPath & expPath,
		   bool				   createNodes,
		   XMP_OptionBits	   leafOptions /* = 0 */,
	 	   spINode       &retNode,
		   XMP_Index           *nodeIndex ,
		   bool ignoreLastStep 
		   )
{
	
	// TO DO - Differentiate between failures on last step and steps before that
	spINode   destNode = mDOM;
	spINode   parentDestNode = mDOM;
	bool firstImplicitNodeFound = false;
	bool leafIsNew = false;
	spINode implicitNodeRoot;
	bool qualifierFlag = false;
	XMP_Index implicitNodeIndex ; // used in case first implicit node's parent is an array
	XMP_Assert ( (leafOptions == 0) || createNodes );

	if ( expPath.empty() ) XMP_Throw ( "Empty XPath", kXMPErr_BadXPath );

	auto defaultMap = INameSpacePrefixMap::GetDefaultNameSpacePrefixMap();
	size_t pathStartIdx = 1;
	if (expPath[kRootPropStep].options & kXMP_StepIsAlias) {

		if (!HandleAliasStep(mDOM, expPath, createNodes, leafOptions, destNode,0, 0 )) return false;
		pathStartIdx = 2;

	}
	try{
		for (size_t i = pathStartIdx, endIndex = (ignoreLastStep) ? expPath.size() - 1 : expPath.size(); i < endIndex; i++) {
			// split the path into prefix and property name
			if (!destNode) goto EXIT;
			XMP_VarString  stepStr = expPath[i].step;
			XMP_VarString  prevStep = (i == 0) ? "" : expPath[i - 1].step;
			spcIUTF8String nameSpace;
			XMP_VarString stepName;

			switch (expPath[i].options) {
			case kXMP_StructFieldStep:
			{
				size_t colonPos = stepStr.find(':');
				XMP_VarString prefix = stepStr.substr(0, colonPos);
				// get the namespace from the prefix
				nameSpace = defaultMap->GetNameSpace(prefix.c_str(), prefix.size());
				stepName = stepStr.substr(colonPos + 1);
				if (destNode->GetNodeType() == INode::kNTStructure) {
					spIStructureNode tempNode = destNode->ConvertToStructureNode();
					parentDestNode = destNode;
					destNode = tempNode->GetNode(nameSpace->c_str(), nameSpace->size(), stepStr.c_str() + colonPos + 1, AdobeXMPCommon::npos );
					if (destNode) continue;
					if (!createNodes) return false;
					if (i == (expPath.size() - 1)) {

						spINode simpleInsertNode = CreateTerminalNode(nameSpace->c_str(), stepName.c_str(), leafOptions);
						tempNode->InsertNode(simpleInsertNode);
						destNode = simpleInsertNode;
						SetImplicitNodeInformation(firstImplicitNodeFound, implicitNodeRoot, destNode, implicitNodeIndex);

					}
					else {

						switch (expPath[i + 1].options) {

						case kXMP_StructFieldStep:
						{
							// TODO : Exit and handle deletetion of implicit node
							// TODO : structInsertNode :
							spIStructureNode structInsertNode = IStructureNode::CreateStructureNode(nameSpace->c_str(), nameSpace->size(), stepName.c_str(), stepName.size() );
							spIStructureNode parentStructNode = parentDestNode->ConvertToStructureNode();
							parentStructNode->InsertNode(structInsertNode);
							destNode = structInsertNode;
							SetImplicitNodeInformation(firstImplicitNodeFound, implicitNodeRoot, destNode, implicitNodeIndex);

						}
						break;
						case kXMP_FieldSelectorStep:
						case kXMP_QualSelectorStep:
						case kXMP_ArrayLastStep:
						case kXMP_ArrayIndexStep:
						{
							// from where will you get arrayform ? which arrayform to set by default?
							spIArrayNode arrayInsertNode = IArrayNode::CreateOrderedArrayNode(nameSpace->c_str(), nameSpace->size(), stepName.c_str(), stepName.size() );
							spIStructureNode parentStructNode = parentDestNode->ConvertToStructureNode();
							parentStructNode->InsertNode(arrayInsertNode);
							destNode = arrayInsertNode;
							SetImplicitNodeInformation(firstImplicitNodeFound, implicitNodeRoot, destNode, implicitNodeIndex);

						}
						break;
						case kXMP_QualifierStep:
						{
							spISimpleNode simpleInsertNode = ISimpleNode::CreateSimpleNode(nameSpace->c_str(), nameSpace->size(), stepName.c_str(), stepName.size() );
							spIStructureNode parentStructNode = parentDestNode->ConvertToStructureNode();
							parentStructNode->InsertNode(simpleInsertNode);
							destNode = simpleInsertNode;
							SetImplicitNodeInformation(firstImplicitNodeFound, implicitNodeRoot, destNode, implicitNodeIndex);


						}
						default:
							break;
						}

					}
				}
				else {
					goto EXIT;
				}
			}
			break;
			case kXMP_ArrayIndexStep:
			{
				// TO DO : type array item 
				// if array not empty -> see type of first array element
				// else if next is array type , arrayitem is array
				// if next is struct select type, array item is struct
				// if next is type qualifier , array item is simple property
				//  TODO : HANDLE EXIT CASE
				// 
				//  we should check if the previous segment is an array segment
				if (destNode->GetNodeType() != INode::kNTArray) {
					XMP_Throw("Indexing applied to non-array", kXMPErr_BadXPath);
				}
				spIArrayNode tempNode = destNode->ConvertToArrayNode();
				size_t index = 0;
				XMP_Assert((stepStr.length() >= 2) && (*(stepStr.begin()) == '[') && (stepStr[stepStr.length() - 1] == ']'));
				for (size_t chNum = 1, chEnd = stepStr.length() - 1; chNum != chEnd; ++chNum) {
					XMP_Assert(('0' <= stepStr[chNum]) && (stepStr[chNum] <= '9'));
					index = (index * 10) + (stepStr[chNum] - '0');
				}
				if (index < 1) XMP_Throw("Array index must be larger than one", kXMPErr_BadXPath);
				if (nodeIndex) *nodeIndex = static_cast<XMP_Index>(index);
				size_t colonPos = prevStep.find(':');
				XMP_VarString prefix = prevStep.substr(0, colonPos);
				nameSpace = defaultMap->GetNameSpace(prefix.c_str(), prefix.size() );
				stepName = kXMP_ArrayItemName;
				parentDestNode = destNode;
				destNode = tempNode->GetNodeAtIndex(index);

				if (destNode)  continue;
				if (!createNodes) return false;
				spIArrayNode parentArrayNode = parentDestNode->ConvertToArrayNode();
				if (parentArrayNode->ChildCount() + 1 < index) goto EXIT;
				if (i == expPath.size() - 1) {
					// to do if array not empty create of type already existing type
					/// else create simple node 
					spINode simpleInsertNode = CreateTerminalNode((nameSpace) ? nameSpace->c_str() : kXMP_NS_XMP, stepName.c_str(), leafOptions);
					parentArrayNode->InsertNodeAtIndex(simpleInsertNode, index);
					destNode = simpleInsertNode;
					if (!firstImplicitNodeFound) {
						firstImplicitNodeFound = true;
						implicitNodeRoot = destNode;
						implicitNodeIndex = static_cast<XMP_Index>(index);
					}
				}
				else {

					switch (expPath[i + 1].options) {

					case kXMP_StructFieldStep:
					{
						// TODO : Exit and handle deletetion of implicit node

						spIStructureNode structInsertNode = IStructureNode::CreateStructureNode(nameSpace->c_str(), nameSpace->size(), stepName.c_str(), stepName.size() );

						parentArrayNode->InsertNodeAtIndex(structInsertNode, index);
						destNode = structInsertNode;
						SetImplicitNodeInformation(firstImplicitNodeFound, implicitNodeRoot, destNode, implicitNodeIndex, (XMP_Index)index);
					}
					break;
					case kXMP_FieldSelectorStep:
					case kXMP_QualSelectorStep:
					case kXMP_ArrayLastStep:
					case kXMP_ArrayIndexStep:
					{
						spIArrayNode arrayInsertNode = IArrayNode::CreateOrderedArrayNode(nameSpace->c_str(), nameSpace->size(), stepName.c_str(), stepName.size());
						parentArrayNode->InsertNodeAtIndex(arrayInsertNode, index);
						destNode = arrayInsertNode;
						SetImplicitNodeInformation(firstImplicitNodeFound, implicitNodeRoot, destNode, implicitNodeIndex, (XMP_Index)index);
					}
					break;
					case kXMP_QualifierStep:
					{
						spISimpleNode simpleInsertNode = ISimpleNode::CreateSimpleNode(nameSpace->c_str(), nameSpace->size(), stepName.c_str(), stepName.size() );
						parentArrayNode->InsertNodeAtIndex(simpleInsertNode, index);
						destNode = simpleInsertNode;
						SetImplicitNodeInformation(firstImplicitNodeFound, implicitNodeRoot, destNode, implicitNodeIndex, (XMP_Index)index);

					}
					default:
						break;
					}

				}

			}
			break;

			case kXMP_ArrayLastStep:
			{
				// what is the interpretation of last when array is empty ? creating an array item for index 1 if creatNodes is true
				// rest of the things are same as above case
				// old implementation has an assertion failing for this case
				if (destNode->GetNodeType() != INode::kNTArray) {
					XMP_Throw("Indexing applied to non-array", kXMPErr_BadXPath);

				}
				spIArrayNode tempNode = destNode->ConvertToArrayNode();

				size_t colonPos = prevStep.find(':');
				XMP_VarString prefix = prevStep.substr(0, colonPos);
				nameSpace = defaultMap->GetNameSpace(prefix.c_str(), prefix.size());
				stepName = prevStep.substr(colonPos + 1);
				spINode parentNode = destNode;
				spIArrayNode parentArrayNode = parentNode->ConvertToArrayNode();
				if (parentNode && parentNode->GetNodeType() == INode::kNTArray) {
					size_t childCount = parentNode->ConvertToArrayNode()->ChildCount();
					if (nodeIndex) *nodeIndex = (XMP_Index)childCount + 1;
					if (childCount) {
						destNode = parentArrayNode->GetNodeAtIndex(childCount);
						continue;
					}
					if (!createNodes) return false;
					if (i == expPath.size() - 1) {

						spINode simpleInsertNode = CreateTerminalNode(nameSpace->c_str(), stepName.c_str(), leafOptions);
						parentArrayNode->InsertNodeAtIndex(simpleInsertNode, 1);
						destNode = simpleInsertNode;
						SetImplicitNodeInformation(firstImplicitNodeFound, implicitNodeRoot, destNode, implicitNodeIndex, (XMP_Index)childCount + 1);
						continue;
					}
					switch (expPath[i + 1].options) {

					case kXMP_QualifierStep:
					{
						spISimpleNode simpleInsertNode = ISimpleNode::CreateSimpleNode(nameSpace->c_str(), nameSpace->size(), stepName.c_str(), stepName.size());
						parentArrayNode->InsertNodeAtIndex(simpleInsertNode, 1);
						destNode = simpleInsertNode;
						SetImplicitNodeInformation(firstImplicitNodeFound, implicitNodeRoot, destNode, implicitNodeIndex, (XMP_Index)childCount + 1);
						continue;
					}
					break;
					case kXMP_ArrayIndexStep:
					case kXMP_ArrayLastStep:
					case kXMP_FieldSelectorStep:
					case kXMP_QualSelectorStep:
					{
						spIArrayNode arrayInsertNode = IArrayNode::CreateOrderedArrayNode(nameSpace->c_str(), nameSpace->size(), stepName.c_str(), stepName.size() );
						parentArrayNode->InsertNodeAtIndex(arrayInsertNode, childCount + 1);
						destNode = arrayInsertNode;
						SetImplicitNodeInformation(firstImplicitNodeFound, implicitNodeRoot, destNode, implicitNodeIndex, (XMP_Index)childCount + 1);
					}
					break;
					case kXMP_StructFieldStep:
					{
						spIStructureNode structInsertNode = IStructureNode::CreateStructureNode(nameSpace->c_str(), nameSpace->size(), stepName.c_str(), stepName.size() );
						parentArrayNode->InsertNodeAtIndex(structInsertNode, childCount + 1);
						destNode = structInsertNode;
						SetImplicitNodeInformation(firstImplicitNodeFound, implicitNodeRoot, destNode, implicitNodeIndex, (XMP_Index)childCount + 1);

					}
					break;

					default:
						break;
					}


				}

			}
			break;
			case kXMP_QualifierStep:
			{
				// if qualifier exists for the parent node, continue
				// else create qualifier if createnodes is true
				XMP_Assert(stepStr[0] == '?');
				stepStr = stepStr.substr(1);
				size_t colonPos = stepStr.find(':');
				XMP_VarString prefix = stepStr.substr(0, colonPos);
				nameSpace = defaultMap->GetNameSpace(prefix.c_str(),prefix.size());
				stepName = stepStr.substr(colonPos + 1);
				qualifierFlag = true;
				parentDestNode = destNode;
				destNode = destNode->GetQualifier(nameSpace->c_str(), nameSpace->size(), stepName.c_str(), stepName.size() );
				if (destNode) continue;
				if (!createNodes) return false;
				spISimpleNode qualifierNode = ISimpleNode::CreateSimpleNode(nameSpace->c_str(), nameSpace->size(), stepName.c_str(), stepName.size() );
				parentDestNode->InsertQualifier(qualifierNode);
				destNode = qualifierNode;
			}

			break;

			case kXMP_QualSelectorStep:
			{
				// TODO - check the old behavior - checked - no implicit nodes except in one case 
				// TODO it is perhaps not required, can be done later
				// if next path step is array(index/lastinddex/qualselect/fieldselect) - this will be an arraynode
				// if next path step is a struct, this will be an structnode
				// if next path step is a qualifier , this will be a simple property

				if (destNode->GetNodeType() != INode::kNTArray) {
					goto EXIT;
				}
				spIArrayNode tempNode = destNode->ConvertToArrayNode();
				XMP_VarString  qualName, qualValue, qualNameSpace;
				SplitNameAndValue(stepStr, &qualName, &qualValue);
				spINode parentNode = destNode;
				size_t colonPos = qualName.find(':');
				XMP_VarString prefix = qualName.substr(0, colonPos);
				qualNameSpace = defaultMap->GetNameSpace(prefix.c_str(), prefix.size())->c_str();
				bool indexFound = false;
				if (parentNode && parentNode->GetNodeType() == INode::kNTArray) {

					spIArrayNode parentArrayNode = parentNode->ConvertToArrayNode();
					size_t arrayChildCount = parentArrayNode->ChildCount();
					for (size_t arrayIdx = 1; arrayIdx <= arrayChildCount; arrayIdx++) {

						spINode currentArrayItem = parentArrayNode->GetNodeAtIndex(arrayIdx);
						spINode qualNode = currentArrayItem->GetQualifier(qualNameSpace.c_str(), qualNameSpace.size(), qualName.c_str() + colonPos + 1, AdobeXMPCommon::npos );
						if (!qualNode) continue;
						XMP_VarString currentQualValue = qualNode->ConvertToSimpleNode()->GetValue()->c_str();
						if (currentQualValue == qualValue) {
							indexFound = true;
							destNode = parentArrayNode->GetNodeAtIndex(arrayIdx);
							break;
						}
					}

				}
				if (!indexFound) {
					goto EXIT;
				}
			}
			break;

			case kXMP_FieldSelectorStep:
			{
				// what if multiple indices match search criterion ?
				// what if parent node isn't an array- exception or return false ?
				// same issue what if one or more child nodes aren't structures ?
				XMP_VarString  fieldName, fieldValue, fieldNameSpace;
				SplitNameAndValue(stepStr, &fieldName, &fieldValue);
				spINode parentNode = destNode;
				size_t colonPos = fieldName.find(':');
				XMP_VarString prefix = fieldName.substr(0, colonPos);
				fieldNameSpace = defaultMap->GetNameSpace(prefix.c_str(), prefix.size())->c_str();
				bool indexFound = false;
				if (parentNode && parentNode->GetNodeType() == INode::kNTArray) {

					spIArrayNode parentArrayNode = parentNode->ConvertToArrayNode();
					size_t arrayChildCount = parentArrayNode->ChildCount();
					for (size_t arrayIdx = 1; arrayIdx <= arrayChildCount; arrayIdx++) {

						spINode currentItem = parentArrayNode->GetNodeAtIndex(arrayIdx);

						if (currentItem->GetNodeType() != INode::kNTStructure) {
							goto EXIT;
						}

						spINode fieldNode = currentItem->ConvertToStructureNode()->GetNode(fieldNameSpace.c_str(), fieldNameSpace.size(), fieldName.c_str() + colonPos + 1, AdobeXMPCommon::npos );
						if (!fieldNode || fieldNode->GetNodeType() != INode::kNTSimple) continue;
						XMP_VarString currentFieldValue = fieldNode->ConvertToSimpleNode()->GetValue()->c_str();
						if (currentFieldValue == fieldValue) {
							indexFound = true;
							destNode = parentArrayNode->GetNodeAtIndex(arrayIdx);
							break;
						}
					}
				}
				if (!indexFound) {
					goto EXIT;
				}
			}
			break;
			default:
				break;

			}

		}
	}
	catch (...) {
		if (firstImplicitNodeFound) {

			spINode parentImplicitNode = implicitNodeRoot->GetParent();
			if (parentImplicitNode->GetNodeType() == INode::kNTArray) {
				parentImplicitNode->ConvertToArrayNode()->RemoveNodeAtIndex( implicitNodeIndex );
			}
			else if (parentImplicitNode->GetNodeType() == INode::kNTStructure) {
				parentImplicitNode->ConvertToStructureNode()->RemoveNode(implicitNodeRoot->GetNameSpace()->c_str(), implicitNodeRoot->GetNameSpace()->size(), implicitNodeRoot->GetName()->c_str(), implicitNodeRoot->GetName()->size() );
			}
		}
		throw;
	}

retNode = destNode;
return true;

EXIT:
{
	//XMP_Assert ( !destNode || (currNode == *currPos) );
	//XMP_Assert ( (destNode!= 0) || (! createNodes) );
	if(!destNode) {

		if( firstImplicitNodeFound ) {

			spINode parentImplicitNode = implicitNodeRoot->GetParent(); 
			if(parentImplicitNode->GetNodeType() == INode::kNTArray) {
				parentImplicitNode->ConvertToArrayNode()->RemoveNodeAtIndex( implicitNodeIndex );
			}
			else if(parentImplicitNode->GetNodeType()== INode::kNTStructure) {
				parentImplicitNode->ConvertToStructureNode()->RemoveNode( implicitNodeRoot->GetNameSpace()->c_str(), implicitNodeRoot->GetNameSpace()->size(), implicitNodeRoot->GetName()->c_str(), implicitNodeRoot->GetName()->size() );
			}
		}
	}	return false;
}
return false;
}	// FindNode

bool  XMPUtils::FindCnstNode ( const spIMetadata & mDOM,XMP_ExpandedXPath &expPath , spINode &destNode, XMP_OptionBits *options , XMP_Index * arrayIndex )
{
	auto defaultMap = INameSpacePrefixMap::GetDefaultNameSpacePrefixMap();
	destNode = mDOM;
	bool qualifierFlag = false;
	size_t pathStartIdx = 1;
	if (expPath[kRootPropStep].options & kXMP_StepIsAlias) {

		if (!HandleConstAliasStep(mDOM, destNode, expPath, 0)) return false;
		pathStartIdx = 2;

	}
	for ( size_t i = pathStartIdx, endIndex = expPath.size(); i < endIndex; i++ ) {
		// split the path into prefix and property name
		if(!destNode) return false;
		XMP_VarString  stepStr = expPath[i].step;
		XMP_VarString  prevStep = (i == 0)? "" : expPath[i - 1].step;
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
					destNode = tempNode->GetNode(nameSpace->c_str(), nameSpace->size(), stepStr.c_str() + colonPos + 1, AdobeXMPCommon::npos );
				}
				else {
					XMP_Throw ( "Named children only allowed for schemas and structs", kXMPErr_BadXPath );
				}
			}
			break;
		case kXMP_ArrayIndexStep:
			{
				// should we check if previous segment is an array segment
				if(destNode->GetNodeType() != INode::kNTArray) {
					XMP_Throw ( "Indexes allowed for arrays only", kXMPErr_BadXPath );
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
				destNode = tempNode->GetNodeAtIndex( index);
				if(arrayIndex)  *arrayIndex = index;
			}
			break;
		case kXMP_ArrayLastStep:
			{
				if(destNode->GetNodeType() != INode::kNTArray) {
					XMP_Throw ( "Indexes allowed for arrays only", kXMPErr_BadXPath );
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
					if(arrayIndex)  *arrayIndex = (XMP_Index)childCount;
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
				qualifierFlag = true;
				destNode = destNode->GetQualifier(nameSpace->c_str(), nameSpace->size(), stepStr.c_str() + colonPos + 1, AdobeXMPCommon::npos);
				
			//	spINode node = mDOM->GetNode( path);
			}
			
			break;
			
		case kXMP_QualSelectorStep:
			{
				// what if multiple indices match search criterion ?
				if(destNode->GetNodeType() != INode::kNTArray) {
					XMP_Throw ( "Indexes allowed for arrays only", kXMPErr_BadXPath );
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
							if(arrayIndex) *arrayIndex = (XMP_Index)arrayIdx;
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
				// what if multiple indices match search criterion ?
				// what if parent node isn't an array- exception or return false ?
				// same issue what if one or more child nodes aren't structures ?
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
							if(arrayIndex) *arrayIndex = (XMP_Index)arrayIdx;
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
	if(!destNode) return false;
	if(!options) return true;
	*options = GetIXMPOptions(destNode);
	return true;
}

size_t  XMPUtils::  GetNodeChildCount(const spcINode & node){
	size_t childCount = 0;
	if( node->GetNodeType() == INode::kNTArray) {
		childCount = node->ConvertToArrayNode()->ChildCount();
	}
	else if (node->GetNodeType() == INode::kNTStructure) {
		childCount = node->ConvertToStructureNode()->ChildCount();
	}
	return childCount;
	
}

spcINodeIterator   XMPUtils::GetNodeChildIterator(const spcINode & node){
	spcINodeIterator childIter;
	if( node->GetNodeType() == INode::kNTArray) {
		childIter = node->ConvertToArrayNode()->Iterator();
	}
	else if (node->GetNodeType() == INode::kNTStructure) {
		childIter = node->ConvertToStructureNode()->Iterator();
	}
	return childIter;
		
}

std::vector<spcINode>  XMPUtils:: GetChildVector( const spINode & node) {

	std::vector<spcINode> childNodes;
	spcINodeIterator childIter = GetNodeChildIterator(node);
	for(; childIter; childIter = childIter->Next()) {
		childNodes.push_back(childIter->GetNode());
	}
	return childNodes;
}
XMP_OptionBits  XMPUtils:: GetIXMPOptions( const spcINode & node) {

	XMP_OptionBits options = 0;
	if(!node) return options;
	if ( node->HasQualifiers()) {
			options |= kXMP_PropHasQualifiers;
			// ( destNode->GetQualifier( "
			if( node->GetQualifier(xmlNameSpace.c_str(), xmlNameSpace.size(), "lang", AdobeXMPCommon::npos )) {
				options |= kXMP_PropHasLang;
		}
		
		if( node->GetQualifier("http://www.w3.org/1999/02/22-rdf-syntax-ns#", AdobeXMPCommon::npos, "type", AdobeXMPCommon::npos )) {
			options |= kXMP_PropHasType;
		}
	}
	XMP_VarString snamespace = node->GetNameSpace()->c_str();
	XMP_VarString sname = node->GetName()->c_str();
	spcINode parentNode = node->GetParent();
	
	if (node->IsQualifierNode()){
		options |= kXMP_PropIsQualifier;
	}

	if ( node->GetNodeType() == INode::kNTSimple ) {
		
		if( node->ConvertToSimpleNode()->IsURIType()){
			options |=  kXMP_PropValueIsURI;
		}
		
	} 
	else if ( node->GetNodeType() == INode::kNTArray) {
		
		spcIArrayNode arrayNode = node->ConvertToArrayNode();
		options |= kXMP_PropValueIsArray;
		
		switch(arrayNode->GetArrayForm()) {

			case IArrayNode::kAFAlternative:
				options |= kXMP_PropArrayIsAlternate;options|= kXMP_PropArrayIsOrdered;
				break;

			case IArrayNode::kAFOrdered:
				options |= kXMP_PropArrayIsOrdered;
				break;

			case IArrayNode::kAFUnordered:
				options |= kXMP_PropArrayIsUnordered;
				break;

			default:
				return false;
				break;
		}
		bool isAltTextArray = (arrayNode->GetArrayForm() == IArrayNode::kAFAlternative );

		for( size_t arrayIndex = 1; arrayIndex <= arrayNode->ChildCount(); arrayIndex++) {
			spcINode childNode = arrayNode->GetNodeAtIndex(arrayIndex);
			if((childNode->GetNodeType() != INode::kNTSimple || !childNode->GetQualifier(xmlNameSpace.c_str(), xmlNameSpace.size(), "lang", AdobeXMPCommon::npos ))) {
				isAltTextArray = false;
				break;
			}
		}
		if(isAltTextArray) {
			options |= kXMP_PropArrayIsAltText;
		}
		
	}
	else if( node->GetNodeType() == INode::kNTStructure && node->GetParent() ) {
		options |= kXMP_PropValueIsStruct;
	}
	return options;
}

spINode 
XMPUtils::FindChildNode	( const spINode &parent,
				  XMP_StringPtr		childName,
				  XMP_StringPtr		childNameSpace,
				  bool				createNodes,
				  size_t *	pos /* = 0 */ )
{
	// need to pass childnamespace too
	spINode childNode;
	XMP_OptionBits parentOptions = XMPUtils::GetIXMPOptions(parent);
	if ( ! (parentOptions & (kXMP_SchemaNode | kXMP_PropValueIsStruct)) ) {
	
		if ( parentOptions & kXMP_PropValueIsArray ) {
			XMP_Throw ( "Named children not allowed for arrays", kXMPErr_BadXPath );
		}
	}
	spcINodeIterator childIter = XMPUtils::GetNodeChildIterator(parent);

	for (size_t idx = 1 ;  childIter; childIter = childIter->Next(), ++idx ) {
		spcINode currChild = childIter->GetNode(); 

		if (currChild && XMP_LitMatch(currChild->GetName()->c_str(), childName) && XMP_LitMatch(currChild->GetNameSpace()->c_str(), childNameSpace) ) {
			childNode = AdobeXMPCore_Int::const_pointer_cast<INode>(currChild);
			if(pos) *pos = idx;
			break;
		}
	}
	
	if ( (!childNode) && createNodes ) {
		childNode = ISimpleNode::CreateSimpleNode(childNameSpace, AdobeXMPCommon::npos, childName, AdobeXMPCommon::npos );
		parent->ConvertToStructureNode()->InsertNode( childNode );
	}
	
	XMP_Assert ( (childNode ) || (! createNodes) );
	return childNode;
	
}	// FindChildNode

spcIUTF8String XMPUtils::GetNodeValue( const spINode & node) {
	

	if (node && node->GetNodeType() == INode::kNTSimple) {
		return node->ConvertToSimpleNode()->GetValue();
	}
	return spIUTF8String();
}


XMP_Index XMPUtils::LookupFieldSelector_v2(const spIArrayNode & arrayNode, XMP_VarString fieldName, XMP_VarString fieldValue) {

	XMP_Index destIdx = -1;
	if (arrayNode->GetNodeType() != INode::kNTArray)	return destIdx;
	for (XMP_Index index = 1, indexLim = (XMP_Index)arrayNode->ChildCount(); index <= indexLim; ++index) {

		spINode childNode = arrayNode->GetNodeAtIndex(index);
		if (childNode->GetNodeType() != INode::kNTStructure) {
			XMP_Throw("Field selector must be used on array of struct", kXMPErr_BadXPath);
		}
		for (spcINodeIterator childNodeIter = XMPUtils::GetNodeChildIterator(childNode); childNodeIter; childNodeIter = childNodeIter->Next()) {
			spcINode currentField = childNodeIter->GetNode();
			if (!XMP_LitMatch(currentField->GetName()->c_str(), fieldName.c_str())) continue;
			if (currentField->GetNodeType() != INode::kNTSimple) continue;
			XMP_VarString currentFieldValue = currentField->ConvertToSimpleNode()->GetValue()->c_str();
			if (currentFieldValue == fieldValue) {
				return index;
			}
		}
	}
	return destIdx;
}

#endif

// -------------------------------------------------------------------------------------------------
// ANSI Time Functions
// -------------------
//
// A bit of hackery to use the best available time functions. Mac, UNIX and iOS have thread safe versions
// of gmtime and localtime.

#if XMP_MacBuild | XMP_UNIXBuild | XMP_iOSBuild | XMP_AndroidBuild

	typedef time_t			ansi_tt;
	typedef struct tm		ansi_tm;

	#define ansi_time		time
	#define ansi_mktime		mktime
	#define ansi_difftime	difftime

	#define ansi_gmtime		gmtime_r
	#define ansi_localtime	localtime_r

#elif XMP_WinBuild

	// ! VS.Net 2003 (VC7) does not provide thread safe versions of gmtime and localtime.
	// ! VS.Net 2005 (VC8) inverts the parameters for the safe versions of gmtime and localtime.

	typedef time_t			ansi_tt;
	typedef struct tm		ansi_tm;

	#define ansi_time		time
	#define ansi_mktime		mktime
	#define ansi_difftime	difftime

	#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		#define ansi_gmtime(tt,tm)		gmtime_s ( tm, tt )
		#define ansi_localtime(tt,tm)	localtime_s ( tm, tt )
	#else
		static inline void ansi_gmtime ( const ansi_tt * ttTime, ansi_tm * tmTime )
		{
			ansi_tm * tmx = gmtime ( ttTime );	// ! Hope that there is no race!
			if ( tmx == 0 ) XMP_Throw ( "Failure from ANSI C gmtime function", kXMPErr_ExternalFailure );
			*tmTime = *tmx;
		}
		static inline void ansi_localtime ( const ansi_tt * ttTime, ansi_tm * tmTime )
		{
			ansi_tm * tmx = localtime ( ttTime );	// ! Hope that there is no race!
			if ( tmx == 0 ) XMP_Throw ( "Failure from ANSI C localtime function", kXMPErr_ExternalFailure );
			*tmTime = *tmx;
		}
	#endif

#endif

// -------------------------------------------------------------------------------------------------
// VerifyDateTimeFlags
// -------------------

static void
VerifyDateTimeFlags ( XMP_DateTime * dt )
{

	if ( (dt->year != 0) || (dt->month != 0) || (dt->day != 0) ) dt->hasDate = true;
	if ( (dt->hour != 0) || (dt->minute != 0) || (dt->second != 0) || (dt->nanoSecond != 0) ) dt->hasTime = true;
	if ( (dt->tzSign != 0) || (dt->tzHour != 0) || (dt->tzMinute != 0) ) dt->hasTimeZone = true;
	if ( dt->hasTimeZone ) dt->hasTime = true;	// ! Don't combine with above line, UTC has zero values.

}	// VerifyDateTimeFlags

// -------------------------------------------------------------------------------------------------
// IsLeapYear
// ----------

static bool
IsLeapYear ( long year )
{

	if ( year < 0 ) year = -year + 1;		// Fold the negative years, assuming there is a year 0.

	if ( (year % 4) != 0 ) return false;	// Not a multiple of 4.
	if ( (year % 100) != 0 ) return true;	// A multiple of 4 but not a multiple of 100.
	if ( (year % 400) == 0 ) return true;	// A multiple of 400.

	return false;							// A multiple of 100 but not a multiple of 400.

}	// IsLeapYear

// -------------------------------------------------------------------------------------------------
// DaysInMonth
// -----------

static int
DaysInMonth ( XMP_Int32 year, XMP_Int32 month )
{

	static short	daysInMonth[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
									   // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec

	int days = daysInMonth [ month ];
	if ( (month == 2) && IsLeapYear ( year ) ) days += 1;

	return days;

}	// DaysInMonth

// -------------------------------------------------------------------------------------------------
// AdjustTimeOverflow
// ------------------

static void
AdjustTimeOverflow ( XMP_DateTime * time )
{
	enum { kBillion = 1000*1000*1000L };

	// ----------------------------------------------------------------------------------------------
	// To be safe against pathalogical overflow we first adjust from month to second, then from
	// nanosecond back up to month. This leaves each value closer to zero before propagating into it.
	// For example if the hour and minute are both near max, adjusting minutes first can cause the
	// hour to overflow.

	// ! Photoshop 8 creates "time only" values with zeros for year, month, and day.

	if ( (time->year != 0) || (time->month != 0) || (time->day != 0) ) {

		while ( time->month < 1 ) {
			time->year -= 1;
			time->month += 12;
		}

		while ( time->month > 12 ) {
			time->year += 1;
			time->month -= 12;
		}

		while ( time->day < 1 ) {
			time->month -= 1;
			if ( time->month < 1 ) {	// ! Keep the months in range for indexing daysInMonth!
				time->year -= 1;
				time->month += 12;
			}
			time->day += DaysInMonth ( time->year, time->month );	// ! Decrement month before so index here is right!
		}

		while ( time->day > DaysInMonth ( time->year, time->month ) ) {
			time->day -= DaysInMonth ( time->year, time->month );	// ! Increment month after so index here is right!
			time->month += 1;
			if ( time->month > 12 ) {
				time->year += 1;
				time->month -= 12;
			}
		}

	}

	while ( time->hour < 0 ) {
		time->day -= 1;
		time->hour += 24;
	}

	while ( time->hour >= 24 ) {
		time->day += 1;
		time->hour -= 24;
	}

	while ( time->minute < 0 ) {
		time->hour -= 1;
		time->minute += 60;
	}

	while ( time->minute >= 60 ) {
		time->hour += 1;
		time->minute -= 60;
	}

	while ( time->second < 0 ) {
		time->minute -= 1;
		time->second += 60;
	}

	while ( time->second >= 60 ) {
		time->minute += 1;
		time->second -= 60;
	}

	while ( time->nanoSecond < 0 ) {
		time->second -= 1;
		time->nanoSecond += kBillion;
	}

	while ( time->nanoSecond >= kBillion ) {
		time->second += 1;
		time->nanoSecond -= kBillion;
	}

	while ( time->second < 0 ) {
		time->minute -= 1;
		time->second += 60;
	}

	while ( time->second >= 60 ) {
		time->minute += 1;
		time->second -= 60;
	}

	while ( time->minute < 0 ) {
		time->hour -= 1;
		time->minute += 60;
	}

	while ( time->minute >= 60 ) {
		time->hour += 1;
		time->minute -= 60;
	}

	while ( time->hour < 0 ) {
		time->day -= 1;
		time->hour += 24;
	}

	while ( time->hour >= 24 ) {
		time->day += 1;
		time->hour -= 24;
	}

	if ( (time->year != 0) || (time->month != 0) || (time->day != 0) ) {

		while ( time->month < 1 ) { // Make sure the months are OK first, for DaysInMonth.
			time->year -= 1;
			time->month += 12;
		}

		while ( time->month > 12 ) {
			time->year += 1;
			time->month -= 12;
		}

		while ( time->day < 1 ) {
			time->month -= 1;
			if ( time->month < 1 ) {
				time->year -= 1;
				time->month += 12;
			}
			time->day += DaysInMonth ( time->year, time->month );
		}

		while ( time->day > DaysInMonth ( time->year, time->month ) ) {
			time->day -= DaysInMonth ( time->year, time->month );
			time->month += 1;
			if ( time->month > 12 ) {
				time->year += 1;
				time->month -= 12;
			}
		}

	}

}	// AdjustTimeOverflow

// -------------------------------------------------------------------------------------------------
// GatherInt
// ---------
//
// Gather into a 64-bit value in order to easily check for overflow. Using a 32-bit value and
// checking for negative isn't reliable, the "*10" part can wrap around to a low positive value.

static XMP_Int32
GatherInt ( XMP_StringPtr strValue, size_t * _pos, const char * errMsg )
{
	size_t	 pos   = *_pos;
	XMP_Int64 value = 0;

	enum { kMaxSInt32 = 0x7FFFFFFF };

	for ( char ch = strValue[pos]; ('0' <= ch) && (ch <= '9'); ++pos, ch = strValue[pos] ) {
		value = (value * 10) + (ch - '0');
		if ( value > kMaxSInt32 ) XMP_Throw ( errMsg, kXMPErr_BadValue );
	}

	if ( pos == *_pos ) XMP_Throw ( errMsg, kXMPErr_BadParam );
	*_pos = pos;
	return (XMP_Int32)value;

}	// GatherInt

// -------------------------------------------------------------------------------------------------

static void FormatFullDateTime ( XMP_DateTime & tempDate, char * buffer, size_t bufferLen )
{

	AdjustTimeOverflow ( &tempDate );	// Make sure all time parts are in range.

	if ( (tempDate.second == 0) && (tempDate.nanoSecond == 0) ) {

		// Output YYYY-MM-DDThh:mmTZD.
		snprintf ( buffer, bufferLen, "%.4d-%02d-%02dT%02d:%02d",	// AUDIT: Callers pass sizeof(buffer).
				   tempDate.year, tempDate.month, tempDate.day, tempDate.hour, tempDate.minute );

	} else if ( tempDate.nanoSecond == 0  ) {

		// Output YYYY-MM-DDThh:mm:ssTZD.
		snprintf ( buffer, bufferLen, "%.4d-%02d-%02dT%02d:%02d:%02d",	// AUDIT: Callers pass sizeof(buffer).
				   tempDate.year, tempDate.month, tempDate.day,
				   tempDate.hour, tempDate.minute, tempDate.second );

	} else {

		// Output YYYY-MM-DDThh:mm:ss.sTZD.
		snprintf ( buffer, bufferLen, "%.4d-%02d-%02dT%02d:%02d:%02d.%09d", // AUDIT: Callers pass sizeof(buffer).
				   tempDate.year, tempDate.month, tempDate.day,
				   tempDate.hour, tempDate.minute, tempDate.second, tempDate.nanoSecond );
		buffer[bufferLen - 1] = 0; // AUDIT warning C6053: make sure string is terminated. buffer is already filled with 0 from caller
		for ( size_t i = strlen(buffer)-1; buffer[i] == '0'; --i ) buffer[i] = 0;	// Trim excess digits.
	}

}	// FormatFullDateTime

// -------------------------------------------------------------------------------------------------
// DecodeBase64Char
// ----------------

// The decode mapping:
//
//	encoded		encoded			raw
//	char		value			value
//	-------		-------			-----
//	A .. Z		0x41 .. 0x5A	 0 .. 25
//	a .. z		0x61 .. 0x7A	26 .. 51
//	0 .. 9		0x30 .. 0x39	52 .. 61
//	+			0x2B			62
//	/			0x2F			63

static unsigned char
DecodeBase64Char ( XMP_Uns8 ch )
{

	if ( ('A' <= ch) && (ch <= 'Z') ) {
		ch = ch - 'A';
	} else if ( ('a' <= ch) && (ch <= 'z') ) {
		ch = ch - 'a' + 26;
	} else if ( ('0' <= ch) && (ch <= '9') ) {
		ch = ch - '0' + 52;
	} else if ( ch == '+' ) {
		ch = 62;
	} else if ( ch == '/' ) {
		ch = 63;
	} else if ( (ch == ' ') || (ch == kTab) || (ch == kLF) || (ch == kCR) ) {
		ch = 0xFF;	// Will be ignored by the caller.
	} else {
		XMP_Throw ( "Invalid base-64 encoded character", kXMPErr_BadParam );
	}

	return ch;

}	// DecodeBase64Char ();

// -------------------------------------------------------------------------------------------------
// EstimateSizeForJPEG
// -------------------
//
// Estimate the serialized size for the subtree of an XMP_Node. Support for PackageForJPEG.

static size_t
EstimateSizeForJPEG ( const XMP_Node * xmpNode )
{

	size_t estSize = 0;
	size_t nameSize = xmpNode->name.size();
	bool   includeName = (! XMP_PropIsArray ( xmpNode->parent->options ));

	if ( XMP_PropIsSimple ( xmpNode->options ) ) {

		if ( includeName ) estSize += (nameSize + 3);	// Assume attribute form.
		estSize += xmpNode->value.size();

	} else if ( XMP_PropIsArray ( xmpNode->options ) ) {

		// The form of the value portion is: <rdf:Xyz><rdf:li>...</rdf:li>...</rdf:Xyx>
		if ( includeName ) estSize += (2*nameSize + 5);
		size_t arraySize = xmpNode->children.size();
		estSize += 9 + 10;	// The rdf:Xyz tags.
		estSize += arraySize * (8 + 9);	// The rdf:li tags.
		for ( size_t i = 0; i < arraySize; ++i ) {
			estSize += EstimateSizeForJPEG ( xmpNode->children[i] );
		}

	} else {

		// The form is: <headTag rdf:parseType="Resource">...fields...</tailTag>
		if ( includeName ) estSize += (2*nameSize + 5);
		estSize += 25;	// The rdf:parseType="Resource" attribute.
		size_t fieldCount = xmpNode->children.size();
		for ( size_t i = 0; i < fieldCount; ++i ) {
			estSize += EstimateSizeForJPEG ( xmpNode->children[i] );
		}

	}

	return estSize;

}	


#if ENABLE_CPP_DOM_MODEL
// -------------------------------------------------------------------------------------------------
// EstimateSizeForJPEG
// -------------------
//
// Estimate the serialized size for the subtree of an XMP_Node. Support for PackageForJPEG.

static size_t
EstimateSizeForJPEG(const spINode &xmpNode)
{

	size_t estSize = 0;
	auto defaultMap = INameSpacePrefixMap::GetDefaultNameSpacePrefixMap();
	
	size_t nameSize = xmpNode->GetName()->size() + 1 ;
	nameSize += defaultMap->GetPrefix(xmpNode->GetNameSpace()->c_str(), xmpNode->GetNameSpace()->size())->size();

	XMP_OptionBits xmpNodeOptions = XMPUtils::GetIXMPOptions(xmpNode);
	bool   includeName = (xmpNode->GetParent()->GetNodeType() != INode::kNTArray);

	if (XMP_PropIsSimple(xmpNodeOptions)) {

		if (includeName) estSize += (nameSize + 3);	// Assume attribute form.
		estSize += xmpNode->ConvertToSimpleNode()->GetValue()->size();
	}
	else if (XMP_PropIsArray(xmpNodeOptions)) {

		// The form of the value portion is: <rdf:Xyz><rdf:li>...</rdf:li>...</rdf:Xyx>
		if (includeName) estSize += (2 * nameSize + 5);
		spIArrayNode structNode = xmpNode->ConvertToArrayNode();
		size_t arraySize = structNode->ChildCount();
		estSize += 9 + 10;	// The rdf:Xyz tags.
		estSize += arraySize * (8 + 9);	// The rdf:li tags.
		
		for (auto structIter = structNode->Iterator(); structIter; structIter = structIter->Next()) {

			estSize += EstimateSizeForJPEG(structIter->GetNode());
		}

	}
	else {

		// The form is: <headTag rdf:parseType="Resource">...fields...</tailTag>
		if (includeName) estSize += (2 * nameSize + 5);
		spIStructureNode structNode = xmpNode->ConvertToStructureNode();
		estSize += 25;	// The rdf:parseType="Resource" attribute.
		size_t fieldCount = structNode->ChildCount();
		
		for (auto structIter = structNode->Iterator(); structIter; structIter = structIter->Next()) {

			estSize += EstimateSizeForJPEG(structIter->GetNode());
		}
	

	}

	return estSize;

}
#endif

// -------------------------------------------------------------------------------------------------
// MoveOneProperty
// ---------------

static bool MoveOneProperty ( XMPMeta & stdXMP, XMPMeta * extXMP,
							  XMP_StringPtr schemaURI, XMP_StringPtr propName )
{

	XMP_Node * propNode = 0;
	XMP_NodePtrPos stdPropPos;

	XMP_Node * stdSchema = FindSchemaNode ( &stdXMP.tree, schemaURI, kXMP_ExistingOnly, 0 );
	if ( stdSchema != 0 ) {
		propNode = FindChildNode ( stdSchema, propName, kXMP_ExistingOnly, &stdPropPos );
	}
	if ( propNode == 0 ) return false;

	XMP_Node * extSchema = FindSchemaNode ( &extXMP->tree, schemaURI, kXMP_CreateNodes );

	propNode->parent = extSchema;

	extSchema->options &= ~kXMP_NewImplicitNode;
	extSchema->children.push_back ( propNode );

	stdSchema->children.erase ( stdPropPos );
	DeleteEmptySchema ( stdSchema );

	return true;

}	// MoveOneProperty



// -------------------------------------------------------------------------------------------------
// MoveOneProperty
// ---------------
#if ENABLE_CPP_DOM_MODEL
static bool MoveOneProperty(XMPMeta2 & stdXMP, XMPMeta2 * extXMP,
	XMP_StringPtr schemaURI, XMP_StringPtr propName)
{

	spINode rootNode = stdXMP.mDOM;
	if (!rootNode) return false;
	spINode propNode = rootNode->ConvertToStructureNode()->GetNode(schemaURI, AdobeXMPCommon::npos, propName, AdobeXMPCommon::npos);
	if (!propNode) return false;

	spINode clonedNode = propNode->Clone();

	spIStructureNode rootNode2 = extXMP->mDOM;
	
	if (rootNode2->GetNode(schemaURI, AdobeXMPCommon::npos, propName, AdobeXMPCommon::npos )) {
		rootNode2->RemoveNode(schemaURI, AdobeXMPCommon::npos, propName, AdobeXMPCommon::npos);
	}
	rootNode2->AppendNode(clonedNode);
	rootNode->ConvertToStructureNode()->RemoveNode( schemaURI, AdobeXMPCommon::npos, propName, AdobeXMPCommon::npos );
	return true;
}	// MoveOneProperty
#endif

// -------------------------------------------------------------------------------------------------
// CreateEstimatedSizeMap
// ----------------------

#ifndef Trace_PackageForJPEG
	#define Trace_PackageForJPEG 0
#endif

typedef std::pair < XMP_VarString*, XMP_VarString* > StringPtrPair;
typedef std::pair < const char *, const char * > StringPtrPair2;
typedef std::multimap < size_t, StringPtrPair > PropSizeMap;
typedef std::multimap < size_t, StringPtrPair2 > PropSizeMap2;

static void CreateEstimatedSizeMap ( XMPMeta & stdXMP, PropSizeMap * propSizes )
{
	#if Trace_PackageForJPEG
		printf ( "  Creating top level property map:\n" );
	#endif

	for ( size_t s = stdXMP.tree.children.size(); s > 0; --s ) {

		XMP_Node * stdSchema = stdXMP.tree.children[s-1];

		for ( size_t p = stdSchema->children.size(); p > 0; --p ) {

			XMP_Node * stdProp = stdSchema->children[p-1];
			if ( (stdSchema->name == kXMP_NS_XMP_Note) &&
				 (stdProp->name == "xmpNote:HasExtendedXMP") ) continue;	// ! Don't move xmpNote:HasExtendedXMP.

			size_t propSize = EstimateSizeForJPEG ( stdProp );
			StringPtrPair namePair ( &stdSchema->name, &stdProp->name );
			PropSizeMap::value_type mapValue ( propSize, namePair );

			(void) propSizes->insert ( propSizes->upper_bound ( propSize ), mapValue );
			#if Trace_PackageForJPEG
				printf ( "    %d bytes, %s in %s\n", propSize, stdProp->name.c_str(), stdSchema->name.c_str() );
			#endif

		}

	}

}	// CreateEstimatedSizeMap

#if ENABLE_CPP_DOM_MODEL
static void CreateEstimatedSizeMap(XMPMeta2 & stdXMP, PropSizeMap2 * propSizes)
{
#if Trace_PackageForJPEG
	printf("  Creating top level property map:\n");
#endif



	spIStructureNode rootNode = stdXMP.mDOM;

	for (auto rootIter = rootNode->Iterator(); rootIter; rootIter = rootIter->Next()) {
		
		const spINode & node = rootIter->GetNode();
		if (!strcmp(node->GetNameSpace()->c_str(), kXMP_NS_XMP_Note) && !strcmp(node->GetName()->c_str(), "HasExtendedXMP")) continue;
		size_t propSize = EstimateSizeForJPEG(node);
		StringPtrPair2 namePair(node->GetNameSpace()->c_str(), node->GetName()->c_str());
		PropSizeMap2::value_type mapValue(propSize, namePair);
		(void)propSizes->insert(propSizes->upper_bound(propSize), mapValue);
#if Trace_PackageForJPEG
		printf("    %d bytes, %s in %s\n", propSize, stdProp->name.c_str(), stdSchema->name.c_str());
#endif
	}
	

}	// CreateEstimatedSizeMap
#endif

#if ENABLE_CPP_DOM_MODEL
// -------------------------------------------------------------------------------------------------
// MoveLargestProperty
// -------------------

static size_t MoveLargestProperty(XMPMeta2 & stdXMP, XMPMeta2 * extXMP, PropSizeMap2 & propSizes)
{
	XMP_Assert(!propSizes.empty());

#if 0
	// *** Xocde 2.3 on Mac OS X 10.4.7 seems to have a bug where this does not pick the last
	// *** item in the map. We'll just avoid it on all platforms until thoroughly tested.
	PropSizeMap::iterator lastPos = propSizes.end();
	--lastPos;	// Move to the actual last item.
#else
	PropSizeMap2::iterator lastPos = propSizes.begin();
	PropSizeMap2::iterator nextPos = lastPos;
	for (++nextPos; nextPos != propSizes.end(); ++nextPos) lastPos = nextPos;
#endif

	size_t propSize = lastPos->first;
	const char * schemaURI = lastPos->second.first;
	const char * propName = lastPos->second.second;

#if Trace_PackageForJPEG
	printf("  Move %s, %d bytes\n", propName, propSize);
#endif

	bool moved = MoveOneProperty(stdXMP, extXMP, schemaURI, propName);
	XMP_Assert(moved);

	propSizes.erase(lastPos);
	return propSize;

}	// MoveLargestProperty
#endif

// -------------------------------------------------------------------------------------------------
// MoveLargestProperty
// -------------------

static size_t MoveLargestProperty ( XMPMeta & stdXMP, XMPMeta * extXMP, PropSizeMap & propSizes )
{
	XMP_Assert ( ! propSizes.empty() );

	#if 0
		// *** Xocde 2.3 on Mac OS X 10.4.7 seems to have a bug where this does not pick the last
		// *** item in the map. We'll just avoid it on all platforms until thoroughly tested.
		PropSizeMap::iterator lastPos = propSizes.end();
		--lastPos;	// Move to the actual last item.
	#else
		PropSizeMap::iterator lastPos = propSizes.begin();
		PropSizeMap::iterator nextPos = lastPos;
		for ( ++nextPos; nextPos != propSizes.end(); ++nextPos ) lastPos = nextPos;
	#endif

	size_t propSize = lastPos->first;
	const char * schemaURI = lastPos->second.first->c_str();
	const char * propName  = lastPos->second.second->c_str();

	#if Trace_PackageForJPEG
		printf ( "  Move %s, %d bytes\n", propName, propSize );
	#endif

	bool moved = MoveOneProperty ( stdXMP, extXMP, schemaURI, propName );
	XMP_Assert ( moved );

	propSizes.erase ( lastPos );
	return propSize;

}	// MoveLargestProperty

// =================================================================================================
// Class Static Functions
// ======================

// -------------------------------------------------------------------------------------------------
// Initialize
// ----------

/* class static */ bool
XMPUtils::Initialize()
{

	if ( WhiteSpaceStrPtr == NULL ) {
		WhiteSpaceStrPtr = new std::string();
		WhiteSpaceStrPtr->append( " \t\n\r" );
	}
	return true;

}	// Initialize

// -------------------------------------------------------------------------------------------------
// Terminate
// ---------

/* class static */ void
XMPUtils::Terminate() RELEASE_NO_THROW
{

	delete WhiteSpaceStrPtr;
	WhiteSpaceStrPtr = NULL;
	return;

}	// Terminate

// -------------------------------------------------------------------------------------------------
// ComposeArrayItemPath
// --------------------
//
// Return "arrayName[index]".

/* class static */ void
XMPUtils::ComposeArrayItemPath ( XMP_StringPtr	 schemaNS,
								 XMP_StringPtr	 arrayName,
								 XMP_Index		 itemIndex,
								 XMP_VarString * _fullPath )
{
	XMP_Assert ( schemaNS != 0 );	// Enforced by wrapper.
	XMP_Assert ( (arrayName != 0) && (*arrayName != 0) ); // Enforced by wrapper.
	XMP_Assert ( _fullPath != 0 );	// Enforced by wrapper.

	XMP_ExpandedXPath expPath;	// Just for side effects to check namespace and basic path.
	ExpandXPath ( schemaNS, arrayName, &expPath );

	if ( (itemIndex < 0) && (itemIndex != kXMP_ArrayLastItem) ) XMP_Throw ( "Array index out of bounds", kXMPErr_BadParam );

	size_t reserveLen = strlen(arrayName) + 2 + 32;	// Room plus padding.

	XMP_VarString fullPath;	// ! Allow for arrayName to be the incoming _fullPath.c_str().
	fullPath.reserve ( reserveLen );
		fullPath = arrayName;

	if ( itemIndex == kXMP_ArrayLastItem ) {
		fullPath += "[last()]";
	} else {
		// AUDIT: Using sizeof(buffer) for the snprintf length is safe.
		char buffer [32];	// A 32 byte buffer is plenty, even for a 64-bit integer.
		snprintf ( buffer, sizeof(buffer), "[%d]", itemIndex );
		fullPath += buffer;
	}

	*_fullPath = fullPath;

}	// ComposeArrayItemPath

// -------------------------------------------------------------------------------------------------
// ComposeStructFieldPath
// ----------------------
//
// Return "structName/ns:fieldName".

/* class static */ void
XMPUtils::ComposeStructFieldPath ( XMP_StringPtr   schemaNS,
								   XMP_StringPtr   structName,
								   XMP_StringPtr   fieldNS,
								   XMP_StringPtr   fieldName,
								   XMP_VarString * _fullPath )
{
	XMP_Assert ( (schemaNS != 0) && (fieldNS != 0) );		// Enforced by wrapper.
	XMP_Assert ( (structName != 0) && (*structName != 0) ); // Enforced by wrapper.
	XMP_Assert ( (fieldName != 0) && (*fieldName != 0) );	// Enforced by wrapper.
	XMP_Assert ( _fullPath != 0 );	// Enforced by wrapper.

	XMP_ExpandedXPath expPath;	// Just for side effects to check namespace and basic path.
	ExpandXPath ( schemaNS, structName, &expPath );

	XMP_ExpandedXPath fieldPath;
	ExpandXPath ( fieldNS, fieldName, &fieldPath );
	if ( fieldPath.size() != 2 ) XMP_Throw ( "The fieldName must be simple", kXMPErr_BadXPath );

	size_t reserveLen = strlen(structName) + fieldPath[kRootPropStep].step.size() + 1;

	XMP_VarString fullPath;	// ! Allow for arrayName to be the incoming _fullPath.c_str().
	fullPath.reserve ( reserveLen );
	fullPath = structName;
	fullPath += '/';
	fullPath += fieldPath[kRootPropStep].step;

	*_fullPath = fullPath;

}	// ComposeStructFieldPath

// -------------------------------------------------------------------------------------------------
// ComposeQualifierPath
// --------------------
//
// Return "propName/?ns:qualName".

/* class static */ void
XMPUtils::ComposeQualifierPath ( XMP_StringPtr	 schemaNS,
								 XMP_StringPtr	 propName,
								 XMP_StringPtr	 qualNS,
								 XMP_StringPtr	 qualName,
								 XMP_VarString * _fullPath )
{
	XMP_Assert ( (schemaNS != 0) && (qualNS != 0) );	// Enforced by wrapper.
	XMP_Assert ( (propName != 0) && (*propName != 0) );	// Enforced by wrapper.
	XMP_Assert ( (qualName != 0) && (*qualName != 0) );	// Enforced by wrapper.
	XMP_Assert ( _fullPath != 0 );	// Enforced by wrapper.

	XMP_ExpandedXPath expPath;	// Just for side effects to check namespace and basic path.
	ExpandXPath ( schemaNS, propName, &expPath );

	XMP_ExpandedXPath qualPath;
	ExpandXPath ( qualNS, qualName, &qualPath );
	if ( qualPath.size() != 2 ) XMP_Throw ( "The qualifier name must be simple", kXMPErr_BadXPath );

	size_t reserveLen = strlen(propName) + qualPath[kRootPropStep].step.size() + 2;

	XMP_VarString fullPath;	// ! Allow for arrayName to be the incoming _fullPath.c_str().
	fullPath.reserve ( reserveLen );
	fullPath = propName;
	fullPath += "/?";
	fullPath += qualPath[kRootPropStep].step;

	*_fullPath = fullPath;

}	// ComposeQualifierPath

// -------------------------------------------------------------------------------------------------
// ComposeLangSelector
// -------------------
//
// Return "arrayName[?xml:lang="lang"]".

// *** #error "handle quotes in the lang - or verify format"

/* class static */ void
XMPUtils::ComposeLangSelector ( XMP_StringPtr	schemaNS,
								XMP_StringPtr	arrayName,
								XMP_StringPtr	_langName,
								XMP_VarString * _fullPath )
{
	XMP_Assert ( schemaNS != 0 );	// Enforced by wrapper.
	XMP_Assert ( (arrayName != 0) && (*arrayName != 0) );	// Enforced by wrapper.
	XMP_Assert ( (_langName != 0) && (*_langName != 0) );	// Enforced by wrapper.
	XMP_Assert ( _fullPath != 0 );	// Enforced by wrapper.

	XMP_ExpandedXPath expPath;	// Just for side effects to check namespace and basic path.
	ExpandXPath ( schemaNS, arrayName, &expPath );

	XMP_VarString langName ( _langName );
	NormalizeLangValue ( &langName );

	size_t reserveLen = strlen(arrayName) + langName.size() + 14;

	XMP_VarString fullPath;	// ! Allow for arrayName to be the incoming _fullPath.c_str().
	fullPath.reserve ( reserveLen );
	fullPath = arrayName;
	fullPath += "[?xml:lang=\"";
	fullPath += langName;
	fullPath += "\"]";

	*_fullPath = fullPath;

}	// ComposeLangSelector

// -------------------------------------------------------------------------------------------------
// ComposeFieldSelector
// --------------------
//
// Return "arrayName[ns:fieldName="fieldValue"]".

// *** #error "handle quotes in the value"

/* class static */ void
XMPUtils::ComposeFieldSelector ( XMP_StringPtr	 schemaNS,
								 XMP_StringPtr	 arrayName,
								 XMP_StringPtr	 fieldNS,
								 XMP_StringPtr	 fieldName,
								 XMP_StringPtr	 fieldValue,
								 XMP_VarString * _fullPath )
{
	XMP_Assert ( (schemaNS != 0) && (fieldNS != 0) && (fieldValue != 0) );	// Enforced by wrapper.
	XMP_Assert ( (*arrayName != 0) && (*fieldName != 0) );	// Enforced by wrapper.
	XMP_Assert ( _fullPath != 0 );		// Enforced by wrapper.

	XMP_ExpandedXPath expPath;	// Just for side effects to check namespace and basic path.
	ExpandXPath ( schemaNS, arrayName, &expPath );

	XMP_ExpandedXPath fieldPath;
	ExpandXPath ( fieldNS, fieldName, &fieldPath );
	if ( fieldPath.size() != 2 ) XMP_Throw ( "The fieldName must be simple", kXMPErr_BadXPath );

	size_t reserveLen = strlen(arrayName) + fieldPath[kRootPropStep].step.size() + strlen(fieldValue) + 5;

	XMP_VarString fullPath;	// ! Allow for arrayName to be the incoming _fullPath.c_str().
	fullPath.reserve ( reserveLen );
	fullPath = arrayName;
	fullPath += '[';
	fullPath += fieldPath[kRootPropStep].step;
	fullPath += "=\"";
	fullPath += fieldValue;
	fullPath += "\"]";

	*_fullPath = fullPath;

}	// ComposeFieldSelector

// -------------------------------------------------------------------------------------------------
// ConvertFromBool
// ---------------

/* class static */ void
XMPUtils::ConvertFromBool ( bool			binValue,
							XMP_VarString * strValue )
{
	XMP_Assert ( strValue != 0 );	// Enforced by wrapper.

	if ( binValue ) {
		*strValue = kXMP_TrueStr;
	} else {
		*strValue = kXMP_FalseStr;
	}

}	// ConvertFromBool

// -------------------------------------------------------------------------------------------------
// ConvertFromInt
// --------------

/* class static */ void
XMPUtils::ConvertFromInt ( XMP_Int32	   binValue,
						   XMP_StringPtr   format,
						   XMP_VarString * strValue )
{
	XMP_Assert ( (format != 0) && (strValue != 0) );	// Enforced by wrapper.

	strValue->erase();
	if ( *format == 0 ) format = "%d";

	// AUDIT: Using sizeof(buffer) for the snprintf length is safe.
	char buffer [32];	// Big enough for a 64-bit integer;
	snprintf ( buffer, sizeof(buffer), format, binValue );

	*strValue = buffer;

}	// ConvertFromInt

// -------------------------------------------------------------------------------------------------
// ConvertFromInt64
// ----------------

/* class static */ void
XMPUtils::ConvertFromInt64 ( XMP_Int64	     binValue,
						     XMP_StringPtr   format,
						     XMP_VarString * strValue )
{
	XMP_Assert ( (format != 0) && (strValue != 0) );	// Enforced by wrapper.

	strValue->erase();
	if ( *format == 0 ) format = "%lld";

	// AUDIT: Using sizeof(buffer) for the snprintf length is safe.
	char buffer [32];	// Big enough for a 64-bit integer;
	snprintf ( buffer, sizeof(buffer), format, binValue );

	*strValue = buffer;

}	// ConvertFromInt64

// -------------------------------------------------------------------------------------------------
// ConvertFromFloat
// ----------------

/* class static */ void
XMPUtils::ConvertFromFloat ( double			 binValue,
							 XMP_StringPtr	 format,
							 XMP_VarString * strValue )
{
	XMP_Assert ( (format != 0) && (strValue != 0) );	// Enforced by wrapper.

	strValue->erase();
	if ( *format == 0 ) format = "%f";

	// AUDIT: Using sizeof(buffer) for the snprintf length is safe.
	char buffer [64];	// Ought to be plenty big enough.
	snprintf ( buffer, sizeof(buffer), format, binValue );

	*strValue = buffer;

}	// ConvertFromFloat

// -------------------------------------------------------------------------------------------------
// ConvertFromDate
// ---------------
//
// Format a date-time string according to ISO 8601 and http://www.w3.org/TR/NOTE-datetime:
//	YYYY
//	YYYY-MM
//	YYYY-MM-DD
//	YYYY-MM-DDThh:mmTZD
//	YYYY-MM-DDThh:mm:ssTZD
//	YYYY-MM-DDThh:mm:ss.sTZD
//
//	YYYY = four-digit year
//	MM	 = two-digit month (01=January, etc.)
//	DD	 = two-digit day of month (01 through 31)
//	hh	 = two digits of hour (00 through 23)
//	mm	 = two digits of minute (00 through 59)
//	ss	 = two digits of second (00 through 59)
//	s	 = one or more digits representing a decimal fraction of a second
//	TZD	 = time zone designator (Z or +hh:mm or -hh:mm)
//
// Note that ISO 8601 does not seem to allow years less than 1000 or greater than 9999. We allow
// any year, even negative ones. The year is formatted as "%.4d". The TZD is also optional in XMP,
// even though required in the W3C profile. Finally, Photoshop 8 (CS) sometimes created time-only
// values so we tolerate that.

/* class static */ void
XMPUtils::ConvertFromDate ( const XMP_DateTime & _inValue,
							XMP_VarString *      strValue )
{
	XMP_Assert ( strValue != 0 );	// Enforced by wrapper.

	char buffer [100];	// Plenty long enough.
	memset( buffer, 0, 100);

	// Pick the format, use snprintf to format into a local buffer, assign to static output string.
	// Don't use AdjustTimeOverflow at the start, that will wipe out zero month or day values.

	// ! Photoshop 8 creates "time only" values with zeros for year, month, and day.

	XMP_DateTime binValue = _inValue;
	VerifyDateTimeFlags ( &binValue );

	// Temporary fix for bug 1269463, silently fix out of range month or day.

	if ( binValue.month == 0 ) {
		if ( (binValue.day != 0) || binValue.hasTime ) binValue.month = 1;
	} else {
		if ( binValue.month < 1 ) binValue.month = 1;
		if ( binValue.month > 12 ) binValue.month = 12;
	}

	if ( binValue.day == 0 ) {
		if ( binValue.hasTime ) binValue.day = 1;
	} else {
		if ( binValue.day < 1 ) binValue.day = 1;
		if ( binValue.day > 31 ) binValue.day = 31;
	}

	// Now carry on with the original logic.

	if ( binValue.month == 0 ) {

		// Output YYYY if all else is zero, otherwise output a full string for the quasi-bogus
		// "time only" values from Photoshop CS.
		if ( (binValue.day == 0) && (! binValue.hasTime) ) {
			snprintf ( buffer, sizeof(buffer), "%.4d", binValue.year ); // AUDIT: Using sizeof for snprintf length is safe.
		} else if ( (binValue.year == 0) && (binValue.day == 0) ) {
			FormatFullDateTime ( binValue, buffer, sizeof(buffer) );
		} else {
			XMP_Throw ( "Invalid partial date", kXMPErr_BadParam);
		}

	} else if ( binValue.day == 0 ) {

		// Output YYYY-MM.
		if ( (binValue.month < 1) || (binValue.month > 12) ) XMP_Throw ( "Month is out of range", kXMPErr_BadParam);
		if ( binValue.hasTime ) XMP_Throw ( "Invalid partial date, non-zeros after zero month and day", kXMPErr_BadParam);
		snprintf ( buffer, sizeof(buffer), "%.4d-%02d", binValue.year, binValue.month );	// AUDIT: Using sizeof for snprintf length is safe.

	} else if ( ! binValue.hasTime ) {

		// Output YYYY-MM-DD.
		if ( (binValue.month < 1) || (binValue.month > 12) ) XMP_Throw ( "Month is out of range", kXMPErr_BadParam);
		if ( (binValue.day < 1) || (binValue.day > 31) ) XMP_Throw ( "Day is out of range", kXMPErr_BadParam);
		snprintf ( buffer, sizeof(buffer), "%.4d-%02d-%02d", binValue.year, binValue.month, binValue.day ); // AUDIT: Using sizeof for snprintf length is safe.

	} else {

		FormatFullDateTime ( binValue, buffer, sizeof(buffer) );

	}

	strValue->assign ( buffer );

	if ( binValue.hasTimeZone ) {

		if ( (binValue.tzHour < 0) || (binValue.tzHour > 23) ||
			 (binValue.tzMinute < 0 ) || (binValue.tzMinute > 59) ||
			 (binValue.tzSign < -1) || (binValue.tzSign > +1) ||
			 ((binValue.tzSign == 0) && ((binValue.tzHour != 0) || (binValue.tzMinute != 0))) ) {
			XMP_Throw ( "Invalid time zone values", kXMPErr_BadParam );
		}

		if ( binValue.tzSign == 0 ) {
			*strValue += 'Z';
		} else {
			snprintf ( buffer, sizeof(buffer), "+%02d:%02d", binValue.tzHour, binValue.tzMinute );	// AUDIT: Using sizeof for snprintf length is safe.
			if ( binValue.tzSign < 0 ) buffer[0] = '-';
			*strValue += buffer;
		}

	}

}	// ConvertFromDate

// -------------------------------------------------------------------------------------------------
// ConvertToBool
// -------------
//
// Formally the string value should be "True" or "False", but we should be more flexible here. Map
// the string to lower case. Allow any of "true", "false", "t", "f", "1", or "0".

/* class static */ bool
XMPUtils::ConvertToBool ( XMP_StringPtr strValue )
{
	if ( (strValue == 0) || (*strValue == 0) ) XMP_Throw ( "Empty convert-from string", kXMPErr_BadValue );

	bool result = false;
	XMP_VarString strObj ( strValue );

	for ( XMP_VarStringPos ch = strObj.begin(); ch != strObj.end(); ++ch ) {
		if ( ('A' <= *ch) && (*ch <= 'Z') ) *ch += 0x20;
	}

	if ( (strObj == "true") || (strObj == "t") || (strObj == "1") ) {
		result = true;
	} else if ( (strObj == "false") || (strObj == "f") || (strObj == "0") ) {
		result = false;
	} else {
		XMP_Throw ( "Invalid Boolean string", kXMPErr_BadParam );
	}

	return result;

}	// ConvertToBool

// -------------------------------------------------------------------------------------------------
// ConvertToInt
// ------------

/* class static */ XMP_Int32
XMPUtils::ConvertToInt ( XMP_StringPtr strValue )
{
	if ( (strValue == 0) || (*strValue == 0) ) XMP_Throw ( "Empty convert-from string", kXMPErr_BadValue );

	int count;
	char nextCh;
	XMP_Int32 result;

	if ( ! XMP_LitNMatch ( strValue, "0x", 2 ) ) {
		count = sscanf ( strValue, "%d%c", &result, &nextCh );
	} else {
		count = sscanf ( strValue, "%x%c", &result, &nextCh );
	}

	if ( count != 1 ) XMP_Throw ( "Invalid integer string", kXMPErr_BadParam );

	return result;

}	// ConvertToInt

// -------------------------------------------------------------------------------------------------
// ConvertToInt64
// --------------

/* class static */ XMP_Int64
XMPUtils::ConvertToInt64 ( XMP_StringPtr strValue )
{
	if ( (strValue == 0) || (*strValue == 0) ) XMP_Throw ( "Empty convert-from string", kXMPErr_BadValue );

	int count;
	char nextCh;
	XMP_Int64 result;

	if ( ! XMP_LitNMatch ( strValue, "0x", 2 ) ) {
		count = sscanf ( strValue, "%lld%c", &result, &nextCh );
	} else {
		count = sscanf ( strValue, "%llx%c", &result, &nextCh );
	}

	if ( count != 1 ) XMP_Throw ( "Invalid integer string", kXMPErr_BadParam );

	return result;

}	// ConvertToInt64

// -------------------------------------------------------------------------------------------------
// ConvertToFloat
// --------------

/* class static */ double
XMPUtils::ConvertToFloat ( XMP_StringPtr strValue )
{
	if ( (strValue == 0) || (*strValue == 0) ) XMP_Throw ( "Empty convert-from string", kXMPErr_BadValue );

	XMP_VarString oldLocale;	// Try to make sure number conversion uses '.' as the decimal point.
	XMP_StringPtr oldLocalePtr = setlocale ( LC_ALL, 0 );
	if ( oldLocalePtr != 0 ) {
		oldLocale.assign ( oldLocalePtr );	// Save the locale to be reset when exiting.
		setlocale ( LC_ALL, "C" );
	}

	errno = 0;
	char * numEnd;
	double result = strtod ( strValue, &numEnd );
	int errnoSave = errno;	// The setlocale call below might change errno.

	if ( ! oldLocale.empty() ) setlocale ( LC_ALL, oldLocale.c_str() );	// ! Reset locale before possible throw!
	if ( (errnoSave != 0) || (*numEnd != 0) ) XMP_Throw ( "Invalid float string", kXMPErr_BadParam );

	return result;

}	// ConvertToFloat

// -------------------------------------------------------------------------------------------------
// ConvertToDate
// -------------
//
// Parse a date-time string according to ISO 8601 and http://www.w3.org/TR/NOTE-datetime:
//	YYYY
//	YYYY-MM
//	YYYY-MM-DD
//	YYYY-MM-DDThh:mmTZD
//	YYYY-MM-DDThh:mm:ssTZD
//	YYYY-MM-DDThh:mm:ss.sTZD
//
//	YYYY = four-digit year
//	MM	 = two-digit month (01=January, etc.)
//	DD	 = two-digit day of month (01 through 31)
//	hh	 = two digits of hour (00 through 23)
//	mm	 = two digits of minute (00 through 59)
//	ss	 = two digits of second (00 through 59)
//	s	 = one or more digits representing a decimal fraction of a second
//	TZD	 = time zone designator (Z or +hh:mm or -hh:mm)
//
// Note that ISO 8601 does not seem to allow years less than 1000 or greater than 9999. We allow
// any year, even negative ones. The year is formatted as "%.4d". The TZD is also optional in XMP,
// even though required in the W3C profile. Finally, Photoshop 8 (CS) sometimes created time-only
// values so we tolerate that.

// *** Put the ISO format comments in the header documentation.

/* class static */ void
XMPUtils::ConvertToDate ( XMP_StringPtr	 strValue,
						  XMP_DateTime * binValue )
{
	if ( (strValue == 0) || (*strValue == 0) ) XMP_Throw ( "Empty convert-from string", kXMPErr_BadValue);

	size_t pos = 0;
	XMP_Int32 temp;

	XMP_Assert ( sizeof(*binValue) == sizeof(XMP_DateTime) );
	(void) memset ( binValue, 0, sizeof(*binValue) );	// AUDIT: Safe, using sizeof destination.

	size_t strSize = strlen ( strValue );
	bool timeOnly = ( (strValue[0] == 'T') ||
					  ((strSize >= 2) && (strValue[1] == ':')) ||
					  ((strSize >= 3) && (strValue[2] == ':')) );

	if ( ! timeOnly ) {

		binValue->hasDate = true;

		if ( strValue[0] == '-' ) pos = 1;

		temp = GatherInt ( strValue, &pos, "Invalid year in date string" ); // Extract the year.
		if ( (strValue[pos] != 0) && (strValue[pos] != '-') ) XMP_Throw ( "Invalid date string, after year", kXMPErr_BadParam );
		if ( strValue[0] == '-' ) temp = -temp;
		binValue->year = temp;
		if ( strValue[pos] == 0 ) return;

		++pos;
		temp = GatherInt ( strValue, &pos, "Invalid month in date string" );	// Extract the month.
		if ( (strValue[pos] != 0) && (strValue[pos] != '-') ) XMP_Throw ( "Invalid date string, after month", kXMPErr_BadParam );
		
		if ( binValue->year != 0 && temp < 1 ) temp = 1;
		if (  temp > 12 ) temp = 12;
		
		binValue->month = temp;
		if ( strValue[pos] == 0 ) return;

		++pos;
		temp = GatherInt ( strValue, &pos, "Invalid day in date string" );	// Extract the day.
		if ( (strValue[pos] != 0) && (strValue[pos] != 'T') ) XMP_Throw ( "Invalid date string, after day", kXMPErr_BadParam );
		
		if ( temp > 31 ) temp = 31;
		binValue->day = temp;
		if ( strValue[pos] == 0 ) return;

		// Allow year, month, and day to all be zero; implies the date portion is missing.
		if ( (binValue->year != 0) || (binValue->month != 0) || (binValue->day != 0) ) {
			// Temporary fix for bug 1269463, silently fix out of range month or day.
			// if ( (binValue->month < 1) || (binValue->month > 12) ) XMP_Throw ( "Month is out of range", kXMPErr_BadParam );
			// if ( (binValue->day < 1) || (binValue->day > 31) ) XMP_Throw ( "Day is out of range", kXMPErr_BadParam );
			if ( binValue->month < 1 ) binValue->month = 1;
			// if ( binValue->month > 12 ) binValue->month = 12;
			if ( binValue->day < 1 ) binValue->day = 1;
			// if ( binValue->day > 31 ) binValue->day = 31;
		}

	}

	// If we get here there is more of the string, otherwise we would have returned above.

	if ( strValue[pos] == 'T' ) {
		++pos;
	} else if ( ! timeOnly ) {
		XMP_Throw ( "Invalid date string, missing 'T' after date", kXMPErr_BadParam );
	}

	binValue->hasTime = true;

	temp = GatherInt ( strValue, &pos, "Invalid hour in date string" ); // Extract the hour.
	if ( strValue[pos] != ':' ) XMP_Throw ( "Invalid date string, after hour", kXMPErr_BadParam );
	if ( temp > 23 ) temp = 23;	// *** 1269463: XMP_Throw ( "Hour is out of range", kXMPErr_BadParam );
	binValue->hour = temp;
	// Don't check for done, we have to work up to the time zone.

	++pos;
	temp = GatherInt ( strValue, &pos, "Invalid minute in date string" );	// And the minute.
	if ( (strValue[pos] != ':') && (strValue[pos] != 'Z') &&
		 (strValue[pos] != '+') && (strValue[pos] != '-') && (strValue[pos] != 0) ) XMP_Throw ( "Invalid date string, after minute", kXMPErr_BadParam );
	if ( temp > 59 ) temp = 59;	// *** 1269463: XMP_Throw ( "Minute is out of range", kXMPErr_BadParam );
	binValue->minute = temp;
	// Don't check for done, we have to work up to the time zone.

	if ( strValue[pos] == ':' ) {

		++pos;
		temp = GatherInt ( strValue, &pos, "Invalid whole seconds in date string" );	// Extract the whole seconds.
		if ( (strValue[pos] != '.') && (strValue[pos] != 'Z') &&
			 (strValue[pos] != '+') && (strValue[pos] != '-') && (strValue[pos] != 0) ) {
			XMP_Throw ( "Invalid date string, after whole seconds", kXMPErr_BadParam );
		}
		if ( temp > 59 ) temp = 59;	// *** 1269463: XMP_Throw ( "Whole second is out of range", kXMPErr_BadParam );
		binValue->second = temp;
		// Don't check for done, we have to work up to the time zone.

		if ( strValue[pos] == '.' ) {

			++pos;
			size_t digits = pos;	// Will be the number of digits later.

			temp = GatherInt ( strValue, &pos, "Invalid fractional seconds in date string" );	// Extract the fractional seconds.
			if ( (strValue[pos] != 'Z') && (strValue[pos] != '+') && (strValue[pos] != '-') && (strValue[pos] != 0) ) {
				XMP_Throw ( "Invalid date string, after fractional second", kXMPErr_BadParam );
			}

			digits = pos - digits;
			for ( ; digits > 9; --digits ) temp = temp / 10;
			for ( ; digits < 9; ++digits ) temp = temp * 10;

			if ( temp >= 1000*1000*1000 ) XMP_Throw ( "Fractional second is out of range", kXMPErr_BadParam );
			binValue->nanoSecond = temp;
			// Don't check for done, we have to work up to the time zone.

		}

	}

	if ( strValue[pos] == 0 ) return;

	binValue->hasTimeZone = true;

	if ( strValue[pos] == 'Z' ) {

		++pos;

	} else {

		if ( strValue[pos] == '+' ) {
			binValue->tzSign = kXMP_TimeEastOfUTC;
		} else if ( strValue[pos] == '-' ) {
			binValue->tzSign = kXMP_TimeWestOfUTC;
		} else {
			XMP_Throw ( "Time zone must begin with 'Z', '+', or '-'", kXMPErr_BadParam );
		}

		++pos;
		temp = GatherInt ( strValue, &pos, "Invalid time zone hour in date string" );	// Extract the time zone hour.
		if ( strValue[pos] != ':' ) XMP_Throw ( "Invalid date string, after time zone hour", kXMPErr_BadParam );
		if ( temp > 23 ) XMP_Throw ( "Time zone hour is out of range", kXMPErr_BadParam );
		binValue->tzHour = temp;

		++pos;
		temp = GatherInt ( strValue, &pos, "Invalid time zone minute in date string" ); // Extract the time zone minute.
		if ( temp > 59 ) XMP_Throw ( "Time zone minute is out of range", kXMPErr_BadParam );
		binValue->tzMinute = temp;

	}

	if ( strValue[pos] != 0 ) XMP_Throw ( "Invalid date string, extra chars at end", kXMPErr_BadParam );

}	// ConvertToDate

// -------------------------------------------------------------------------------------------------
// EncodeToBase64
// --------------
//
// Encode a string of raw data bytes in base 64 according to RFC 2045. For the encoding definition
// see section 6.8 in <http://www.ietf.org/rfc/rfc2045.txt>. Although it isn't needed for RDF, we
// do insert a linefeed character as a newline for every 76 characters of encoded output.

/* class static */ void
XMPUtils::EncodeToBase64 ( XMP_StringPtr   rawStr,
						   XMP_StringLen   rawLen,
						   XMP_VarString * encodedStr )
{
	if ( (rawStr == 0) && (rawLen != 0) ) XMP_Throw ( "Null raw data buffer", kXMPErr_BadParam );
	XMP_Assert ( encodedStr != 0 );	// Enforced by wrapper.

	encodedStr->erase();
	if ( rawLen == 0 ) return;

	char	encChunk[4];

	unsigned long	in, out;
	unsigned char	c1, c2, c3;
	unsigned long	merge;

	const size_t	outputSize	= (rawLen / 3) * 4; // Approximate, might be  small.

	encodedStr->reserve ( outputSize );

	// ----------------------------------------------------------------------------------------
	// Each 6 bits of input produces 8 bits of output, so 3 input bytes become 4 output bytes.
	// Process the whole chunks of 3 bytes first, then deal with any remainder. Be careful with
	// the loop comparison, size-2 could be negative!

	for ( in = 0, out = 0; (in+2) < rawLen; in += 3, out += 4 ) {

		c1	= rawStr[in];
		c2	= rawStr[in+1];
		c3	= rawStr[in+2];

		merge	= (c1 << 16) + (c2 << 8) + c3;

		encChunk[0] = sBase64Chars [ merge >> 18 ];
		encChunk[1] = sBase64Chars [ (merge >> 12) & 0x3F ];
		encChunk[2] = sBase64Chars [ (merge >> 6) & 0x3F ];
		encChunk[3] = sBase64Chars [ merge & 0x3F ];

		if ( out >= 76 ) {
			encodedStr->append ( 1, kLF );
			out = 0;
		}
		encodedStr->append ( encChunk, 4 );

	}

	// ------------------------------------------------------------------------------------------
	// The output must always be a multiple of 4 bytes. If there is a 1 or 2 byte input remainder
	// we need to create another chunk. Zero pad with bits to a 6 bit multiple, then add one or
	// two '=' characters to pad out to 4 bytes.

	switch ( rawLen - in ) {

		case 0:		// Done, no remainder.
			break;

		case 1:		// One input byte remains.

			c1	= rawStr[in];
			merge	= c1 << 16;

			encChunk[0] = sBase64Chars [ merge >> 18 ];
			encChunk[1] = sBase64Chars [ (merge >> 12) & 0x3F ];
			encChunk[2] = encChunk[3] = '=';

			if ( out >= 76 ) encodedStr->append ( 1, kLF );
			encodedStr->append ( encChunk, 4 );
			break;

		case 2:		// Two input bytes remain.

			c1	= rawStr[in];
			c2	= rawStr[in+1];
			merge	= (c1 << 16) + (c2 << 8);

			encChunk[0] = sBase64Chars [ merge >> 18 ];
			encChunk[1] = sBase64Chars [ (merge >> 12) & 0x3F ];
			encChunk[2] = sBase64Chars [ (merge >> 6) & 0x3F ];
			encChunk[3] = '=';

			if ( out >= 76 ) encodedStr->append ( 1, kLF );
			encodedStr->append ( encChunk, 4 );
			break;

	}

}	// EncodeToBase64

// -------------------------------------------------------------------------------------------------
// DecodeFromBase64
// ----------------
//
// Decode a string of raw data bytes from base 64 according to RFC 2045. For the encoding definition
// see section 6.8 in <http://www.ietf.org/rfc/rfc2045.txt>. RFC 2045 talks about ignoring all "bad"
// input but warning about non-whitespace. For XMP use we ignore space, tab, LF, and CR. Any other
// bad input is rejected.

/* class static */ void
XMPUtils::DecodeFromBase64 ( XMP_StringPtr	 encodedStr,
							 XMP_StringLen	 encodedLen,
							 XMP_VarString * rawStr )
{
	if ( (encodedStr == 0) && (encodedLen != 0) ) XMP_Throw ( "Null encoded data buffer", kXMPErr_BadParam );
	XMP_Assert ( rawStr != 0 );	// Enforced by wrapper.

	rawStr->erase();
	if ( encodedLen == 0 ) return;

	unsigned char	ch, rawChunk[3];
	unsigned long	inStr, inChunk, inLimit, merge, padding;

	XMP_StringLen	outputSize	= (encodedLen / 4) * 3; // Only a close approximation.

	rawStr->reserve ( outputSize );


	// ----------------------------------------------------------------------------------------
	// Each 8 bits of input produces 6 bits of output, so 4 input bytes become 3 output bytes.
	// Process all but the last 4 data bytes first, then deal with the final chunk. Whitespace
	// in the input must be ignored. The first loop finds where the last 4 data bytes start and
	// counts the number of padding equal signs.

	padding = 0;
	for ( inStr = 0, inLimit = encodedLen; (inStr < 4) && (inLimit > 0); ) {
		inLimit -= 1;	// ! Don't do in the loop control, the decr/test order is wrong.
		ch = encodedStr[inLimit];
		if ( ch == '=' ) {
			padding += 1;	// The equal sign padding is a data byte.
		} else if ( DecodeBase64Char(ch) == 0xFF ) {
			continue;	// Ignore whitespace, don't increment inStr.
		} else {
			inStr += 1;
		}
	}

	// ! Be careful to count whitespace that is immediately before the final data. Otherwise
	// ! middle portion will absorb the final data and mess up the final chunk processing.

	while ( (inLimit > 0) && (DecodeBase64Char(encodedStr[inLimit-1]) == 0xFF) ) --inLimit;

	if ( inStr == 0 ) return;	// Nothing but whitespace.
	if ( padding > 2 ) XMP_Throw ( "Invalid encoded string", kXMPErr_BadParam );

	// -------------------------------------------------------------------------------------------
	// Now process all but the last chunk. The limit ensures that we have at least 4 data bytes
	// left when entering the output loop, so the inner loop will succeed without overrunning the
	// end of the data. At the end of the outer loop we might be past inLimit though.

	inStr = 0;
	while ( inStr < inLimit ) {

		merge = 0;
		for ( inChunk = 0; inChunk < 4; ++inStr ) { // ! Yes, increment inStr on each pass.
			ch = DecodeBase64Char ( encodedStr [inStr] );
			if ( ch == 0xFF ) continue; // Ignore whitespace.
			merge = (merge << 6) + ch;
			inChunk += 1;
		}

		rawChunk[0] = (unsigned char) (merge >> 16);
		rawChunk[1] = (unsigned char) ((merge >> 8) & 0xFF);
		rawChunk[2] = (unsigned char) (merge & 0xFF);

		rawStr->append ( (char*)rawChunk, 3 );

	}

	// -------------------------------------------------------------------------------------------
	// Process the final, possibly partial, chunk of data. The input is always a multiple 4 bytes,
	// but the raw data can be any length. The number of padding '=' characters determines if the
	// final chunk has 1, 2, or 3 raw data bytes.

	XMP_Assert ( inStr < encodedLen );

	merge = 0;
	for ( inChunk = 0; inChunk < 4-padding; ++inStr ) { // ! Yes, increment inStr on each pass.
		ch = DecodeBase64Char ( encodedStr[inStr] );
		if ( ch == 0xFF ) continue; // Ignore whitespace.
		merge = (merge << 6) + ch;
		inChunk += 1;
	}

	if ( padding == 2 ) {

		rawChunk[0] = (unsigned char) (merge >> 4);
		rawStr->append ( (char*)rawChunk, 1 );

	} else if ( padding == 1 ) {

		rawChunk[0] = (unsigned char) (merge >> 10);
		rawChunk[1] = (unsigned char) ((merge >> 2) & 0xFF);
		rawStr->append ( (char*)rawChunk, 2 );

	} else {

		rawChunk[0] = (unsigned char) (merge >> 16);
		rawChunk[1] = (unsigned char) ((merge >> 8) & 0xFF);
		rawChunk[2] = (unsigned char) (merge & 0xFF);
		rawStr->append ( (char*)rawChunk, 3 );

	}

}	// DecodeFromBase64

// -------------------------------------------------------------------------------------------------
// PackageForJPEG
// --------------

/* class static */ void
XMPUtils::PackageForJPEG ( const XMPMeta & origXMP,
						   XMP_VarString * stdStr,
						   XMP_VarString * extStr,
						   XMP_VarString * digestStr )
{

#if ENABLE_CPP_DOM_MODEL
	if(sUseNewCoreAPIs) {

		const XMPMeta2 & orig = dynamic_cast<const XMPMeta2 &>(origXMP);
		return XMPUtils::PackageForJPEG(orig, stdStr, extStr, digestStr);
	}
#endif
	XMP_Assert ( (stdStr != 0) && (extStr != 0) && (digestStr != 0) );	// ! Enforced by wrapper.

	enum { kStdXMPLimit = 65000 };
	static const char * kPacketTrailer = "<?xpacket end=\"w\"?>";
	static size_t kTrailerLen = strlen ( kPacketTrailer );

	XMP_VarString tempStr;
	XMPMeta stdXMP, extXMP;
	XMP_OptionBits keepItSmall = kXMP_UseCompactFormat | kXMP_OmitAllFormatting;

	stdStr->erase();
	extStr->erase();
	digestStr->erase();

	// Try to serialize everything. Note that we're making internal calls to SerializeToBuffer, so
	// we'll be getting back the pointer and length for its internal string.

	origXMP.SerializeToBuffer ( &tempStr, keepItSmall, 1, "", "", 0 );
	#if Trace_PackageForJPEG
		printf ( "\nXMPUtils::PackageForJPEG - Full serialize %d bytes\n", tempStr.size() );
	#endif

	if ( tempStr.size() > kStdXMPLimit ) {

		// Couldn't fit everything, make a copy of the input XMP and make sure there is no xmp:Thumbnails property.

		stdXMP.tree.options = origXMP.tree.options;
		stdXMP.tree.name    = origXMP.tree.name;
		stdXMP.tree.value   = origXMP.tree.value;
		CloneOffspring ( &origXMP.tree, &stdXMP.tree );

		if ( stdXMP.DoesPropertyExist ( kXMP_NS_XMP, "Thumbnails" ) ) {
			stdXMP.DeleteProperty ( kXMP_NS_XMP, "Thumbnails" );
			stdXMP.SerializeToBuffer ( &tempStr, keepItSmall, 1, "", "", 0 );
			#if Trace_PackageForJPEG
				printf ( "  Delete xmp:Thumbnails, %d bytes left\n", tempStr.size() );
			#endif
		}

	}

	if ( tempStr.size() > kStdXMPLimit ) {

		// Still doesn't fit, move all of the Camera Raw namespace. Add a dummy value for xmpNote:HasExtendedXMP.

		stdXMP.SetProperty ( kXMP_NS_XMP_Note, "HasExtendedXMP", "123456789-123456789-123456789-12", 0 );

		XMP_NodePtrPos crSchemaPos;
		XMP_Node * crSchema = FindSchemaNode ( &stdXMP.tree, kXMP_NS_CameraRaw, kXMP_ExistingOnly, &crSchemaPos );

		if ( crSchema != 0 ) {
			crSchema->parent = &extXMP.tree;
			extXMP.tree.children.push_back ( crSchema );
			stdXMP.tree.children.erase ( crSchemaPos );
			stdXMP.SerializeToBuffer ( &tempStr, keepItSmall, 1, "", "", 0 );
			#if Trace_PackageForJPEG
				printf ( "  Move Camera Raw schema, %d bytes left\n", tempStr.size() );
			#endif
		}

	}

	if ( tempStr.size() > kStdXMPLimit ) {

		// Still doesn't fit, move photoshop:History.

		bool moved = MoveOneProperty ( stdXMP, &extXMP, kXMP_NS_Photoshop, "photoshop:History" );

		if ( moved ) {
			stdXMP.SerializeToBuffer ( &tempStr, keepItSmall, 1, "", "", 0 );
			#if Trace_PackageForJPEG
				printf ( "  Move photoshop:History, %d bytes left\n", tempStr.size() );
			#endif
		}

	}

	if ( tempStr.size() > kStdXMPLimit ) {

		// Still doesn't fit, move top level properties in order of estimated size. This is done by
		// creating a multi-map that maps the serialized size to the string pair for the schema URI
		// and top level property name. Since maps are inherently ordered, a reverse iteration of
		// the map can be done to move the largest things first. We use a double loop to keep going
		// until the serialization actually fits, in case the estimates are off.

		PropSizeMap propSizes;
		CreateEstimatedSizeMap ( stdXMP, &propSizes );

		#if Trace_PackageForJPEG
		if ( ! propSizes.empty() ) {
			printf ( "  Top level property map, smallest to largest:\n" );
			PropSizeMap::iterator mapPos = propSizes.begin();
			PropSizeMap::iterator mapEnd = propSizes.end();
			for ( ; mapPos != mapEnd; ++mapPos ) {
				size_t propSize = mapPos->first;
				const char * schemaName = mapPos->second.first->c_str();
				const char * propName   = mapPos->second.second->c_str();
				printf ( "    %d bytes, %s in %s\n", propSize, propName, schemaName );
			}
		}
		#endif

		#if 0	// Trace_PackageForJPEG		*** Xcode 2.3 on 10.4.7 has bugs in backwards iteration
		if ( ! propSizes.empty() ) {
			printf ( "  Top level property map, largest to smallest:\n" );
			PropSizeMap::iterator mapPos   = propSizes.end();
			PropSizeMap::iterator mapBegin = propSizes.begin();
			for ( --mapPos; true; --mapPos ) {
				size_t propSize = mapPos->first;
				const char * schemaName = mapPos->second.first->c_str();
				const char * propName   = mapPos->second.second->c_str();
				printf ( "    %d bytes, %s in %s\n", propSize, propName, schemaName );
				if ( mapPos == mapBegin ) break;
			}
		}
		#endif

		// Outer loop to make sure enough is actually moved.

		while ( (tempStr.size() > kStdXMPLimit) && (! propSizes.empty()) ) {

			// Inner loop, move what seems to be enough according to the estimates.

			size_t tempLen = tempStr.size();
			while ( (tempLen > kStdXMPLimit) && (! propSizes.empty()) ) {

				size_t propSize = MoveLargestProperty ( stdXMP, &extXMP, propSizes );
				XMP_Assert ( propSize > 0 );

				if ( propSize > tempLen ) propSize = tempLen;	// ! Don't go negative.
				tempLen -= propSize;

			}

			// Reserialize the remaining standard XMP.

			stdXMP.SerializeToBuffer ( &tempStr, keepItSmall, 1, "", "", 0 );

		}

	}

	if ( tempStr.size() > kStdXMPLimit ) {
		// Still doesn't fit, throw an exception and let the client decide what to do.
		// ! This should never happen with the policy of moving any and all top level properties.
		XMP_Throw ( "Can't reduce XMP enough for JPEG file", kXMPErr_TooLargeForJPEG );
	}

	// Set the static output strings.

	if ( extXMP.tree.children.empty() ) {

		// Just have the standard XMP.
		*stdStr = tempStr;

	} else {

		// Have extended XMP. Serialize it, compute the digest, reset xmpNote:HasExtendedXMP, and
		// reserialize the standard XMP.

		extXMP.SerializeToBuffer ( &tempStr, (keepItSmall | kXMP_OmitPacketWrapper), 0, "", "", 0 );
		*extStr = tempStr;

		MD5_CTX  context;
		XMP_Uns8 digest [16];
		MD5Init ( &context );
		MD5Update ( &context, (XMP_Uns8*)tempStr.c_str(), (XMP_Uns32)tempStr.size() );
		MD5Final ( digest, &context );

		digestStr->reserve ( 32 );
		for ( size_t i = 0; i < 16; ++i ) {
			XMP_Uns8 byte = digest[i];
			digestStr->push_back ( kHexDigits [ byte>>4 ] );
			digestStr->push_back ( kHexDigits [ byte&0xF ] );
		}

		stdXMP.SetProperty ( kXMP_NS_XMP_Note, "HasExtendedXMP", digestStr->c_str(), 0 );
		stdXMP.SerializeToBuffer ( &tempStr, keepItSmall, 1, "", "", 0 );
		*stdStr = tempStr;

	}

	// Adjust the standard XMP padding to be up to 2KB.

	XMP_Assert ( (stdStr->size() > kTrailerLen) && (stdStr->size() <= kStdXMPLimit) );
	const char * packetEnd = stdStr->c_str() + stdStr->size() - kTrailerLen;
	XMP_Assert ( XMP_LitMatch ( packetEnd, kPacketTrailer ) );

	size_t extraPadding = kStdXMPLimit - stdStr->size();	// ! Do this before erasing the trailer.
	if ( extraPadding > 2047 ) extraPadding = 2047;
	stdStr->erase ( stdStr->size() - kTrailerLen );
	stdStr->append ( extraPadding, ' ' );
	stdStr->append ( kPacketTrailer );

}	// PackageForJPEG


#if ENABLE_CPP_DOM_MODEL
// -------------------------------------------------------------------------------------------------
// PackageForJPEG
// --------------

/* class static */ void
XMPUtils::PackageForJPEG(const XMPMeta2 & origXMP,
						 XMP_VarString * stdStr,
                         XMP_VarString * extStr,
                         XMP_VarString * digestStr)
{
	XMP_Assert((stdStr != 0) && (extStr != 0) && (digestStr != 0));	// ! Enforced by wrapper.

	enum { kStdXMPLimit = 65000 };
	static const char * kPacketTrailer = "<?xpacket end=\"w\"?>";
	static size_t kTrailerLen = strlen(kPacketTrailer);

	XMP_VarString tempStr;
	XMPMeta2 stdXMP, extXMP;
	XMP_OptionBits keepItSmall = kXMP_UseCompactFormat | kXMP_OmitAllFormatting;

	stdStr->erase();
	extStr->erase();
	digestStr->erase();

	// Try to serialize everything. Note that we're making internal calls to SerializeToBuffer, so
	// we'll be getting back the pointer and length for its internal string.

	origXMP.SerializeToBuffer(&tempStr, keepItSmall, 1, "", "", 0);
#if Trace_PackageForJPEG
	printf("\nXMPUtils::PackageForJPEG - Full serialize %d bytes\n", tempStr.size());
#endif

	if (tempStr.size() > kStdXMPLimit) {

		// Couldn't fit everything, make a copy of the input XMP and make sure there is no xmp:Thumbnails property.

		stdXMP.mDOM = origXMP.mDOM->Clone()->ConvertToMetadata();
		

		if (stdXMP.DoesPropertyExist(kXMP_NS_XMP, "Thumbnails")) {
			stdXMP.DeleteProperty(kXMP_NS_XMP, "Thumbnails");
			stdXMP.SerializeToBuffer(&tempStr, keepItSmall, 1, "", "", 0);
#if Trace_PackageForJPEG
			printf("  Delete xmp:Thumbnails, %d bytes left\n", tempStr.size());
#endif
		}

	}

	if (tempStr.size() > kStdXMPLimit) {

		// Still doesn't fit, move all of the Camera Raw namespace. Add a dummy value for xmpNote:HasExtendedXMP.

		stdXMP.SetProperty(kXMP_NS_XMP_Note, "HasExtendedXMP", "123456789-123456789-123456789-12", 0);

		spIStructureNode currRootNode = stdXMP.mDOM;
		std::vector<XMP_VarString> nodes;
		for (auto rootPropIter = currRootNode->Iterator(); rootPropIter; rootPropIter = rootPropIter->Next()) {
			
			auto rootPropNodeCloned = rootPropIter->GetNode()->Clone();
			if (strcmp(rootPropNodeCloned->GetNameSpace()->c_str(), kXMP_NS_CameraRaw ) ) continue;
			extXMP.mDOM->AppendNode(rootPropNodeCloned);
			nodes.push_back(rootPropNodeCloned->GetName()->c_str());
		}

		for (size_t childIdx = 0, childLim = nodes.size(); childIdx != childLim; ++childIdx) {
		
			stdXMP.mDOM->RemoveNode(kXMP_NS_CameraRaw, AdobeXMPCommon::npos, nodes[childIdx].c_str(), nodes[childIdx].size() );
		}


		if (nodes.size() != 0) {
			
			stdXMP.SerializeToBuffer(&tempStr, keepItSmall, 1, "", "", 0);
#if Trace_PackageForJPEG
			printf("  Move Camera Raw schema, %d bytes left\n", tempStr.size());
#endif
		}

	}

	if (tempStr.size() > kStdXMPLimit) {

		// Still doesn't fit, move photoshop:History.

		bool moved = MoveOneProperty(stdXMP, &extXMP, kXMP_NS_Photoshop, "History");

		if (moved) {
			stdXMP.SerializeToBuffer(&tempStr, keepItSmall, 1, "", "", 0);
#if Trace_PackageForJPEG
			printf("  Move photoshop:History, %d bytes left\n", tempStr.size());
#endif
		}

	}

	if (tempStr.size() > kStdXMPLimit) {

		// Still doesn't fit, move top level properties in order of estimated size. This is done by
		// creating a multi-map that maps the serialized size to the string pair for the schema URI
		// and top level property name. Since maps are inherently ordered, a reverse iteration of
		// the map can be done to move the largest things first. We use a double loop to keep going
		// until the serialization actually fits, in case the estimates are off.

		PropSizeMap2 propSizes;
		CreateEstimatedSizeMap(stdXMP, &propSizes);

#if Trace_PackageForJPEG
		if (!propSizes.empty()) {
			printf("  Top level property map, smallest to largest:\n");
			PropSizeMap::iterator mapPos = propSizes.begin();
			PropSizeMap::iterator mapEnd = propSizes.end();
			for (; mapPos != mapEnd; ++mapPos) {
				size_t propSize = mapPos->first;
				const char * schemaName = mapPos->second.first->c_str();
				const char * propName = mapPos->second.second->c_str();
				printf("    %d bytes, %s in %s\n", propSize, propName, schemaName);
			}
		}
#endif

#if 0	// Trace_PackageForJPEG		*** Xcode 2.3 on 10.4.7 has bugs in backwards iteration
		if (!propSizes.empty()) {
			printf("  Top level property map, largest to smallest:\n");
			PropSizeMap::iterator mapPos = propSizes.end();
			PropSizeMap::iterator mapBegin = propSizes.begin();
			for (--mapPos; true; --mapPos) {
				size_t propSize = mapPos->first;
				const char * schemaName = mapPos->second.first->c_str();
				const char * propName = mapPos->second.second->c_str();
				printf("    %d bytes, %s in %s\n", propSize, propName, schemaName);
				if (mapPos == mapBegin) break;
			}
		}
#endif

		// Outer loop to make sure enough is actually moved.

		while ((tempStr.size() > kStdXMPLimit) && (!propSizes.empty())) {

			// Inner loop, move what seems to be enough according to the estimates.

			size_t tempLen = tempStr.size();
			while ((tempLen > kStdXMPLimit) && (!propSizes.empty())) {

				size_t propSize = MoveLargestProperty(stdXMP, &extXMP, propSizes);
				XMP_Assert(propSize > 0);

				if (propSize > tempLen) propSize = tempLen;	// ! Don't go negative.
				tempLen -= propSize;

			}

			// Reserialize the remaining standard XMP.

			stdXMP.SerializeToBuffer(&tempStr, keepItSmall, 1, "", "", 0);

		}

	}

	if (tempStr.size() > kStdXMPLimit) {
		// Still doesn't fit, throw an exception and let the client decide what to do.
		// ! This should never happen with the policy of moving any and all top level properties.
		XMP_Throw("Can't reduce XMP enough for JPEG file", kXMPErr_TooLargeForJPEG);
	}

	// Set the static output strings.

	if (!extXMP.mDOM->ChildCount()) {

		// Just have the standard XMP.
		*stdStr = tempStr;

	}
	else {

		// Have extended XMP. Serialize it, compute the digest, reset xmpNote:HasExtendedXMP, and
		// reserialize the standard XMP.

		extXMP.SerializeToBuffer(&tempStr, (keepItSmall | kXMP_OmitPacketWrapper), 0, "", "", 0);
		*extStr = tempStr;

		MD5_CTX  context;
		XMP_Uns8 digest[16];
		MD5Init(&context);
		MD5Update(&context, (XMP_Uns8*)tempStr.c_str(), (XMP_Uns32)tempStr.size());
		MD5Final(digest, &context);

		digestStr->reserve(32);
		for (size_t i = 0; i < 16; ++i) {
			XMP_Uns8 byte = digest[i];
			digestStr->push_back(kHexDigits[byte >> 4]);
			digestStr->push_back(kHexDigits[byte & 0xF]);
		}

		stdXMP.SetProperty(kXMP_NS_XMP_Note, "HasExtendedXMP", digestStr->c_str(), 0);
		stdXMP.SerializeToBuffer(&tempStr, keepItSmall, 1, "", "", 0);
		*stdStr = tempStr;

	}

	// Adjust the standard XMP padding to be up to 2KB.

	XMP_Assert((stdStr->size() > kTrailerLen) && (stdStr->size() <= kStdXMPLimit));
	const char * packetEnd = stdStr->c_str() + stdStr->size() - kTrailerLen;
	XMP_Assert(XMP_LitMatch(packetEnd, kPacketTrailer));

	size_t extraPadding = kStdXMPLimit - stdStr->size();	// ! Do this before erasing the trailer.
	if (extraPadding > 2047) extraPadding = 2047;
	stdStr->erase(stdStr->size() - kTrailerLen);
	stdStr->append(extraPadding, ' ');
	stdStr->append(kPacketTrailer);

}	// PackageForJPEG

#endif
// -------------------------------------------------------------------------------------------------
// MergeFromJPEG
// -------------
//
// Copy all of the top level properties from extendedXMP to fullXMP, replacing any duplicates.
// Delete the xmpNote:HasExtendedXMP property from fullXMP.

/* class static */ void
XMPUtils::MergeFromJPEG ( XMPMeta *       fullXMP,
                          const XMPMeta & extendedXMP )
{

	XMP_OptionBits apFlags = (kXMPTemplate_ReplaceExistingProperties | kXMPTemplate_IncludeInternalProperties);
	XMPUtils::ApplyTemplate ( fullXMP, extendedXMP, apFlags );
	fullXMP->DeleteProperty ( kXMP_NS_XMP_Note, "HasExtendedXMP" );

}	// MergeFromJPEG





// -------------------------------------------------------------------------------------------------
// CurrentDateTime
// ---------------

/* class static */ void
XMPUtils::CurrentDateTime ( XMP_DateTime * xmpTime )
{
	XMP_Assert ( xmpTime != 0 );	// ! Enforced by wrapper.

	ansi_tt binTime = ansi_time(0);
	if ( binTime == -1 ) XMP_Throw ( "Failure from ANSI C time function", kXMPErr_ExternalFailure );
	ansi_tm currTime;
	ansi_localtime ( &binTime, &currTime );

	xmpTime->year = currTime.tm_year + 1900;
	xmpTime->month = currTime.tm_mon + 1;
	xmpTime->day = currTime.tm_mday;
	xmpTime->hasDate = true;

	xmpTime->hour = currTime.tm_hour;
	xmpTime->minute = currTime.tm_min;
	xmpTime->second = currTime.tm_sec;
	xmpTime->nanoSecond = 0;
	xmpTime->hasTime = true;

	xmpTime->tzSign = 0;
	xmpTime->tzHour = 0;
	xmpTime->tzMinute = 0;
	xmpTime->hasTimeZone = false;	// ! Needed for SetTimeZone.
	XMPUtils::SetTimeZone ( xmpTime );

}	// CurrentDateTime

// -------------------------------------------------------------------------------------------------
// SetTimeZone
// -----------
//
// Sets just the time zone part of the time.  Useful for determining the local time zone or for
// converting a "zone-less" time to a proper local time. The ANSI C time functions are smart enough
// to do all the right stuff, as long as we call them properly!

/* class static */ void
XMPUtils::SetTimeZone ( XMP_DateTime * xmpTime )
{
	XMP_Assert ( xmpTime != 0 );	// ! Enforced by wrapper.

	VerifyDateTimeFlags ( xmpTime );

	if ( xmpTime->hasTimeZone ) {
		XMP_Throw ( "SetTimeZone can only be used on zone-less times", kXMPErr_BadParam );
	}

	// Create ansi_tt form of the input time. Need the ansi_tm form to make the ansi_tt form.

	ansi_tt ttTime;
	ansi_tm tmLocal, tmUTC;

	if ( (xmpTime->year == 0) && (xmpTime->month == 0) && (xmpTime->day == 0) ) {
		ansi_tt now = ansi_time(0);
		if ( now == -1 ) XMP_Throw ( "Failure from ANSI C time function", kXMPErr_ExternalFailure );
		ansi_localtime ( &now, &tmLocal );
	} else {
		tmLocal.tm_year = xmpTime->year - 1900;
		while ( tmLocal.tm_year < 70 ) tmLocal.tm_year += 4;	// ! Some versions of mktime barf on years before 1970.
		tmLocal.tm_mon	 = xmpTime->month - 1;
		tmLocal.tm_mday	 = xmpTime->day;
	}

	tmLocal.tm_hour = xmpTime->hour;
	tmLocal.tm_min = xmpTime->minute;
	tmLocal.tm_sec = xmpTime->second;
	tmLocal.tm_isdst = -1;	// Don't know if daylight time is in effect.

	ttTime = ansi_mktime ( &tmLocal );
	if ( ttTime == -1 ) XMP_Throw ( "Failure from ANSI C mktime function", kXMPErr_ExternalFailure );

	// Convert back to a localized ansi_tm time and get the corresponding UTC ansi_tm time.

	ansi_localtime ( &ttTime, &tmLocal );
	ansi_gmtime ( &ttTime, &tmUTC );

	// Get the offset direction and amount.

	ansi_tm tmx = tmLocal;	// ! Note that mktime updates the ansi_tm parameter, messing up difftime!
	ansi_tm tmy = tmUTC;
	tmx.tm_isdst = tmy.tm_isdst = 0;
	ansi_tt ttx = ansi_mktime ( &tmx );
	ansi_tt tty = ansi_mktime ( &tmy );
	double diffSecs;

	if ( (ttx != -1) && (tty != -1) ) {
		diffSecs = ansi_difftime ( ttx, tty );
	} else {
		#if XMP_MacBuild | XMP_iOSBuild
			// Looks like Apple's mktime is buggy - see W1140533. But the offset is visible.
			diffSecs = tmLocal.tm_gmtoff;
		#else
			// Win and UNIX don't have a visible offset. Make sure we know about the failure,
			// then try using the current date/time as a close fallback.
			ttTime = ansi_time(0);
			if ( ttTime == -1 ) XMP_Throw ( "Failure from ANSI C time function", kXMPErr_ExternalFailure );
			ansi_localtime ( &ttTime, &tmx );
			ansi_gmtime ( &ttTime, &tmy );
			tmx.tm_isdst = tmy.tm_isdst = 0;
			ttx = ansi_mktime ( &tmx );
			tty = ansi_mktime ( &tmy );
			if ( (ttx == -1) || (tty == -1) ) XMP_Throw ( "Failure from ANSI C mktime function", kXMPErr_ExternalFailure );
			diffSecs = ansi_difftime ( ttx, tty );
		#endif
	}

	if ( diffSecs > 0.0 ) {
		xmpTime->tzSign = kXMP_TimeEastOfUTC;
	} else if ( diffSecs == 0.0 ) {
		xmpTime->tzSign = kXMP_TimeIsUTC;
	} else {
		xmpTime->tzSign = kXMP_TimeWestOfUTC;
		diffSecs = -diffSecs;
	}
	xmpTime->tzHour = XMP_Int32 ( diffSecs / 3600.0 );
	xmpTime->tzMinute = XMP_Int32 ( (diffSecs / 60.0) - (xmpTime->tzHour * 60.0) );

	xmpTime->hasTimeZone = xmpTime->hasTime = true;

	// *** Save the tm_isdst flag in a qualifier?

	XMP_Assert ( (0 <= xmpTime->tzHour) && (xmpTime->tzHour <= 23) );
	XMP_Assert ( (0 <= xmpTime->tzMinute) && (xmpTime->tzMinute <= 59) );
	XMP_Assert ( (-1 <= xmpTime->tzSign) && (xmpTime->tzSign <= +1) );
	XMP_Assert ( (xmpTime->tzSign == 0) ? ((xmpTime->tzHour == 0) && (xmpTime->tzMinute == 0)) :
										  ((xmpTime->tzHour != 0) || (xmpTime->tzMinute != 0)) );

}	// SetTimeZone

// -------------------------------------------------------------------------------------------------
// ConvertToUTCTime
// ----------------

/* class static */ void
XMPUtils::ConvertToUTCTime ( XMP_DateTime * time )
{
	XMP_Assert ( time != 0 );	// ! Enforced by wrapper.

	VerifyDateTimeFlags ( time );

	if ( ! time->hasTimeZone ) return;	// Do nothing if there is no current time zone.

	XMP_Assert ( (0 <= time->tzHour) && (time->tzHour <= 23) );
	XMP_Assert ( (0 <= time->tzMinute) && (time->tzMinute <= 59) );
	XMP_Assert ( (-1 <= time->tzSign) && (time->tzSign <= +1) );
	XMP_Assert ( (time->tzSign == 0) ? ((time->tzHour == 0) && (time->tzMinute == 0)) :
									   ((time->tzHour != 0) || (time->tzMinute != 0)) );

	if ( time->tzSign == kXMP_TimeEastOfUTC ) {
		// We are before (east of) GMT, subtract the offset from the time.
		time->hour -= time->tzHour;
		time->minute -= time->tzMinute;
	} else if ( time->tzSign == kXMP_TimeWestOfUTC ) {
		// We are behind (west of) GMT, add the offset to the time.
		time->hour += time->tzHour;
		time->minute += time->tzMinute;
	}

	AdjustTimeOverflow ( time );
	time->tzSign = time->tzHour = time->tzMinute = 0;

}	// ConvertToUTCTime

// -------------------------------------------------------------------------------------------------
// ConvertToLocalTime
// ------------------

/* class static */ void
XMPUtils::ConvertToLocalTime ( XMP_DateTime * time )
{
	XMP_Assert ( time != 0 );	// ! Enforced by wrapper.

	VerifyDateTimeFlags ( time );

	if ( ! time->hasTimeZone ) return;	// Do nothing if there is no current time zone.

	XMP_Assert ( (0 <= time->tzHour) && (time->tzHour <= 23) );
	XMP_Assert ( (0 <= time->tzMinute) && (time->tzMinute <= 59) );
	XMP_Assert ( (-1 <= time->tzSign) && (time->tzSign <= +1) );
	XMP_Assert ( (time->tzSign == 0) ? ((time->tzHour == 0) && (time->tzMinute == 0)) :
									   ((time->tzHour != 0) || (time->tzMinute != 0)) );

	ConvertToUTCTime ( time );	// The existing time zone might not be the local one.
	time->hasTimeZone = false;	// ! Needed for SetTimeZone.
	SetTimeZone ( time );		// Fill in the local timezone offset, then adjust the time.

	if ( time->tzSign > 0 ) {
		// We are before (east of) GMT, add the offset to the time.
		time->hour += time->tzHour;
		time->minute += time->tzMinute;
	} else if ( time->tzSign < 0 ) {
		// We are behind (west of) GMT, subtract the offset from the time.
		time->hour -= time->tzHour;
		time->minute -= time->tzMinute;
	}

	AdjustTimeOverflow ( time );

}	// ConvertToLocalTime

// -------------------------------------------------------------------------------------------------
// CompareDateTime
// ---------------

/* class static */ int
XMPUtils::CompareDateTime ( const XMP_DateTime & _in_left,
							const XMP_DateTime & _in_right )
{
	int result = 0;

	XMP_DateTime left  = _in_left;
	XMP_DateTime right = _in_right;

	VerifyDateTimeFlags ( &left );
	VerifyDateTimeFlags ( &right );

	// Can't compare if one has a date and the other does not.
	if ( left.hasDate != right.hasDate ) return 0;	// Throw?

	if ( left.hasTimeZone & right.hasTimeZone ) {
		// If both times have zones then convert them to UTC, otherwise assume the same zone.
		ConvertToUTCTime ( &left );
		ConvertToUTCTime ( &right );
	}

	if ( left.hasDate ) {

		XMP_Assert ( right.hasDate );

		if ( left.year < right.year ) {
			result = -1;
		} else if ( left.year > right.year ) {
			result = +1;
		} else if ( left.month < right.month ) {
			result = -1;
		} else if ( left.month > right.month ) {
			result = +1;
		} else if ( left.day < right.day ) {
			result = -1;
		} else if ( left.day > right.day ) {
			result = +1;
		}

		if ( result != 0 ) return result;

	}

	if ( left.hasTime & right.hasTime ) {

		// Ignore the time parts if either value is date-only.

		if ( left.hour < right.hour ) {
			result = -1;
		} else if ( left.hour > right.hour ) {
			result = +1;
		} else if ( left.minute < right.minute ) {
			result = -1;
		} else if ( left.minute > right.minute ) {
			result = +1;
		} else if ( left.second < right.second ) {
			result = -1;
		} else if ( left.second > right.second ) {
			result = +1;
		} else if ( left.nanoSecond < right.nanoSecond ) {
			result = -1;
		} else if ( left.nanoSecond > right.nanoSecond ) {
			result = +1;
		} else {
			result = 0;
		}

	}

	return result;

}	// CompareDateTime

// =================================================================================================



std::string * XMPUtils::WhiteSpaceStrPtr = NULL;

std::string& XMPUtils::Trim( std::string& string )
{
	size_t pos = string.find_last_not_of( *WhiteSpaceStrPtr );

	if ( pos != std::string::npos ) {
		string.erase( pos + 1 );
		pos = string.find_first_not_of( *WhiteSpaceStrPtr );
		if(pos != std::string::npos) string.erase(0, pos);
	} else {
		string.erase( string.begin(), string.end() );
	}
	return string;
}

#if ENABLE_CPP_DOM_MODEL
#include "XMPCore/XMPCoreErrorCodes.h"

void XMPUtils::MapXMPErrorToIError( XMP_Int32 xmpErrorCode, IError::eErrorDomain & domain, IError::eErrorCode & code ) {

	switch ( xmpErrorCode ) {
	case kXMPErr_Unknown:
	case kXMPErr_TBD:
		code = kGECUnknownFailure;
		domain = IError_base::kEDGeneral;
		break;

	case kXMPErr_Unavailable:
	case kXMPErr_Unimplemented:
		code = kGECNotImplemented;
		domain = IError_base::kEDGeneral;
		break;

	case kXMPErr_BadObject:
	case kXMPErr_BadParam:
	case kXMPErr_BadValue:
		code = kGECParametersNotAsExpected;
		domain = IError_base::kEDGeneral;
		break;

	case kXMPErr_AssertFailure:
	case kXMPErr_EnforceFailure:
		code = kGECAssertionFailure;
		domain = IError_base::kEDGeneral;
		break;

	case kXMPErr_InternalFailure:
		code = kGECInternalFailure;
		domain = IError_base::kEDGeneral;
		break;

	case kXMPErr_Deprecated:
		code = kGECDeprecatedFunctionCall;
		domain = IError_base::kEDGeneral;
		break;

	case kXMPErr_ExternalFailure:
		code = kGECExternalFailure;
		domain = IError_base::kEDGeneral;
		break;

	case kXMPErr_UserAbort:
	case kXMPErr_ProgressAbort:
		code = kGECUserAbort;
		domain = IError_base::kEDGeneral;
		break;

	case kXMPErr_StdException:
		code = kGECStandardException;
		domain = IError_base::kEDGeneral;
		break;

	case kXMPErr_UnknownException:
		code = kGECUnknownExceptionCaught;
		domain = IError_base::kEDGeneral;
		break;

	case kXMPErr_NoMemory:
		code = kMMECAllocationFailure;
		domain = IError_base::kEDMemoryManagement;
		break;

	case kXMPErr_BadSchema:
		code = kDMECBadSchema;
		domain = IError_base::kEDDataModel;
		break;

	case kXMPErr_BadXPath:
		code = kDMECBadXPath;
		domain = IError_base::kEDDataModel;
		break;

	case kXMPErr_BadOptions:
		code = kDMECBadOptions;
		domain = IError_base::kEDDataModel;
		break;

	case kXMPErr_BadIndex:
		code = kGECIndexOutOfBounds;
		domain = IError_base::kEDGeneral;
		break;

	case kXMPErr_BadIterPosition:
		code = kDMECBadIterPosition;
		domain = IError_base::kEDDataModel;
		break;

	case kXMPErr_BadParse:
		code = kPECBadXMP;
		domain = IError_base::kEDParser;
		break;

	case kXMPErr_BadSerialize:
		code = kSECSizeExceed;
		domain = IError_base::kEDSerializer;
		break;

	case kXMPErr_BadFileFormat:
	case kXMPErr_NoFileHandler:
	case kXMPErr_TooLargeForJPEG:
	case kXMPErr_NoFile:
	case kXMPErr_FilePermission:
	case kXMPErr_DiskSpace:
	case kXMPErr_ReadError:
	case kXMPErr_WriteError:
	case kXMPErr_BadBlockFormat:
	case kXMPErr_FilePathNotAFile:
	case kXMPErr_RejectedFileExtension:
		code = kGECNotImplemented;
		domain = IError_base::kEDGeneral;
		break;

	case kXMPErr_BadXML:
		code = kPECBadXML;
		domain = IError_base::kEDParser;
		break;

	case kXMPErr_BadRDF:
		code = kPECBadRDF;
		domain = IError_base::kEDParser;
		break;

	case kXMPErr_BadXMP:
		code = kPECBadXMP;
		domain = IError_base::kEDParser;
		break;

	case kXMPErr_EmptyIterator:
		code = kDMECEmptyIterator;
		domain = IError_base::kEDDataModel;
		break;

	case kXMPErr_BadUnicode:
		code = kDMECBadUnicode;
		domain = IError_base::kEDDataModel;
		break;

	case kXMPErr_BadTIFF:
	case kXMPErr_BadJPEG:
	case kXMPErr_BadPSD:
	case kXMPErr_BadPSIR:
	case kXMPErr_BadIPTC:
	case kXMPErr_BadMPEG:
	default:
		code = kGECNotImplemented;
		domain = IError_base::kEDGeneral;
		break;

	}
}

#endif

// =================================================================================================
