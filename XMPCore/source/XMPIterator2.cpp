// =================================================================================================
// Copyright 2003 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "public/include/XMP_Environment.h"	// ! This must be the first include!
#include "XMPCore/source/XMPCore_Impl.hpp"

#include "XMPCore/XMPCoreDefines.h"
#if ENABLE_CPP_DOM_MODEL
#include "XMPCore/source/XMPIterator2.hpp"
#include "XMPCore/source/XMPMeta2.hpp"
#include "XMPCore/source/XMPUtils.hpp"
#include "XMPCore/Interfaces/IMetadata_I.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap_I.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCore/Interfaces/ISimpleNode_I.h"
#include <map>
#include <string>
#include <stdio.h>	// For snprintf.

#if XMP_WinBuild
	#pragma warning ( disable : 4702 )	// unreachable code
	#pragma warning ( disable : 4800 )	// forcing value to bool 'true' or 'false' (performance warning)
	#pragma warning ( disable : 4996 )	// '...' was declared deprecated
#endif

// =================================================================================================
// Support Routines
// =================================================================================================


#ifndef TraceIterators
	#define TraceIterators 0
#endif

#if TraceIterators
	static const char * sStageNames[] = { "before", "self", "qualifiers", "children" };
#endif


static XMP_VarString GetNameSpace( const AdobeXMPCommon::spcIUTF8String & nameSpace )
{
	auto defaultMap = AdobeXMPCore::INameSpacePrefixMap::GetDefaultNameSpacePrefixMap()->GetINameSpacePrefixMap_I();
	auto prefix = defaultMap->GetPrefix( nameSpace );
	return prefix->c_str();
}


static XMP_VarString NodeFullName( const AdobeXMPCore::spcINode & node )
{
	XMP_VarString name = GetNameSpace(node->GetNameSpace()) + ":" + node->GetName()->c_str();
	return name;
}

void  XMPIterator2::AddSchemaProperties(IteratorNode & iterSchema, const char * nameSpace)
{
	using namespace  AdobeXMPCore;

	for (auto childIter = mDOM->Iterator(); childIter; childIter = childIter->Next()) {

		spINode childNode = childIter->GetNode();
		//TODO check name
		if (!strcmp(childNode->GetNameSpace()->c_str(), nameSpace)) {
			iterSchema.nodeChildren.push_back(IteratorNode( XMPUtils::GetIXMPOptions(childNode), NodeFullName(childNode), 0 ));
		}

	}

}
 void XMPIterator2::SetCurrentSchema(XMP_StringPtr schemaName)
{

	info.currSchema = schemaName;

}	// SetCurrSchema

 void XMPIterator2::SetCurrentSchema(XMP_VarString & schemaName)
{

	info.currSchema = schemaName;

}	// SetCurrSchema

void XMPIterator2::AdvanceIteratorPosition()
{

	while (true) {
	
		if (info.currPos == info.endPos) {

			if (info.ancestors.empty()) break;

			IteratorPosPair & parent = info.ancestors.back();
			info.currPos = parent.first;
			info.endPos = parent.second;
			info.ancestors.pop_back();
		}
		else {

			if (info.currPos->visitStage == kIter_BeforeVisit) {
				if (info.currPos->options & kXMP_SchemaNode) SetCurrentSchema(info.currPos->fullPath);
				break;
			}


			if (info.currPos->visitStage == kIter_VisitSelf) {

				info.currPos->visitStage = kIter_VisitQualifiers;
				if (!info.currPos->nodeQualifiers.empty()) {

					info.ancestors.push_back(IteratorPosPair(info.currPos, info.endPos));
					info.endPos = info.currPos->nodeQualifiers.end();
					info.currPos = info.currPos->nodeQualifiers.begin();
					break;
				}
			}

			if (info.currPos->visitStage == kIter_VisitQualifiers) {	// Just finished visiting the qualifiers.
				info.currPos->nodeQualifiers.clear();
				info.currPos->visitStage = kIter_VisitChildren;			// Start visiting the children.
				if (!info.currPos->nodeChildren.empty()) {
					info.ancestors.push_back(IteratorPosPair(info.currPos, info.endPos));
					info.endPos = info.currPos->nodeChildren.end();		// ! Set the parent's endPos before changing currPos!
					info.currPos = info.currPos->nodeChildren.begin();
					break;
				}
			}

			if (info.currPos->visitStage == kIter_VisitChildren) {
				info.currPos->nodeChildren.clear();
				++info.currPos;
				continue;
			}
		}
	}

	XMP_Assert((info.currPos == info.endPos) || (info.currPos->visitStage == kIter_BeforeVisit));

}

AdobeXMPCore::spINode XMPIterator2::GetNextNode(bool & isSchema)
{
	using namespace  AdobeXMPCore;
	spINode xmpNode = spINode();

	if (info.currPos->visitStage != kIter_BeforeVisit) AdvanceIteratorPosition();

	bool isSchemaNode = false;
	XMP_ExpandedXPath exPath; 

	while (info.currPos != info.endPos) {

		isSchemaNode = XMP_NodeIsSchema(info.currPos->options);
		if (isSchemaNode) {

			SetCurrentSchema(info.currPos->fullPath);

			isSchema = isSchemaNode;
		}
		else {

			ExpandXPath(info.currSchema.c_str(), info.currPos->fullPath.c_str(), &exPath);
			bool found = XMPUtils::FindCnstNode(mDOM, exPath, xmpNode);
		}
		if (xmpNode ||  isSchemaNode) break;

		info.currPos->visitStage = kIter_VisitChildren;
		info.currPos->nodeChildren.clear();
		info.currPos->nodeQualifiers.clear();

		AdvanceIteratorPosition();
	}
	
	if (info.currPos == info.endPos) return spINode();

	XMP_Assert(info.currPos->visitStage == kIter_BeforeVisit);

	if (info.currPos->visitStage == kIter_BeforeVisit) {

		if (!isSchemaNode && !(info.options & kXMP_IterJustChildren)) {
			
			AddNodeOffSpring(*info.currPos, xmpNode);
		}
		info.currPos->visitStage = kIter_VisitSelf;
	}

	return xmpNode;
}

void XMPIterator2::AddNodeOffSpring(IteratorNode &iterParent, const AdobeXMPCore::spINode & xmpParent)
{

		using namespace AdobeXMPCore;
		XMP_VarString currPath(iterParent.fullPath);
		size_t leafOffset = currPath.size();
		if (xmpParent->HasQualifiers() && (!(info.options & kXMP_IterOmitQualifiers))) {

			currPath += "/?";	// All qualifiers are named and use paths like "Prop/?Qual".
			leafOffset += 2;

			for ( auto qualIter = xmpParent->QualifiersIterator(); qualIter; qualIter = qualIter->Next() ) {

				spINode qualNode = qualIter->GetNode();
				//TOTO Add prefix too
				currPath += NodeFullName( qualNode );

				iterParent.nodeQualifiers.push_back( IteratorNode( XMPUtils::GetIXMPOptions( qualNode ), currPath, leafOffset ) );
				currPath.erase( leafOffset );
			}

			leafOffset -= 2;
			currPath.erase(leafOffset);

		}
		if (XMPUtils::GetNodeChildCount(xmpParent)) {



			if (xmpParent->GetNodeType() == INode::kNTStructure) {
				currPath += '/';
				leafOffset += 1;
			}
			size_t childIdx = 0;
			for (auto childIter = XMPUtils::GetNodeChildIterator(xmpParent); childIter; childIter = childIter->Next(), ++childIdx) {

				spcINode xmpChild = childIter->GetNode();
				if (xmpParent->GetNodeType() != INode::kNTArray) {
					//TODO Add prefix as well
					currPath += NodeFullName(xmpChild);
				}
				else {

					char buffer[32];	// AUDIT: Using sizeof(buffer) below for snprintf length is safe.
					#if XMP_WinBuild
						snprintf(buffer, sizeof(buffer), "[%Iu]", childIdx + 1);	// ! XPath indices are one-based.
					#else
						snprintf(buffer, sizeof(buffer), "[%zu]", childIdx + 1);
					#endif
					currPath += buffer;
				}

				iterParent.nodeChildren.push_back(IteratorNode(XMPUtils::GetIXMPOptions(xmpChild), currPath, leafOffset));
				currPath.erase(leafOffset);

			}


		}



	}	// AddNodeOffspring

// =================================================================================================
// Constructors
// =================================================================================================

// -------------------------------------------------------------------------------------------------
// XMPIterator
// -----------
//
// Constructor for iterations over the nodes in an XMPMeta object. This builds a tree of iteration
// nodes that caches the existing node names of the XMPMeta object. The iteration tree is a partial
// replica of the XMPMeta tree. The initial iteration tree normally has just the root node, all of
// the schema nodes for a full object iteration. Lower level nodes (children and qualifiers) are 
// added when the parent is visited. If the kXMP_IterJustChildren option is passed then the initial
// iterator includes the children and the parent is marked as done. The iteration tree nodes are
// pruned when they are no longer needed. 

XMPIterator2::XMPIterator2 ( const XMPMeta & xmpObjBase,
						   XMP_StringPtr   schemaNS,
						   XMP_StringPtr   propName,
						   XMP_OptionBits  options) :XMPIterator(xmpObjBase, schemaNS, propName, options)
{
	using namespace AdobeXMPCore;
	if ( (options & kXMP_IterClassMask) != kXMP_IterProperties ) {
		XMP_Throw ( "Unsupported iteration kind", kXMPErr_BadOptions );
	}
	iteratorOptions = options;
	info.options = options;
	
	if(sUseNewCoreAPIs) {

		const XMPMeta2 & tempPtr = dynamic_cast<const XMPMeta2 &>(xmpObjBase);
		
	}
	else {
		XMP_Throw("Unsupported iteration kind", kXMPErr_BadOptions);
	}
	
	const XMPMeta2 & xmpObj = dynamic_cast<const XMPMeta2 &>(xmpObjBase);

	spIMetadata root = xmpObj.mDOM;
	mDOM = xmpObj.mDOM;
	// *** Lock the XMPMeta object if we ever stop using a full DLL lock.

	if ( *propName != 0 ) {
		XMP_ExpandedXPath propPath;
		ExpandXPath(schemaNS, propName, &propPath);
		spINode destNode;
		XMP_OptionBits destOptions = 0;
		bool nodeFound = XMPUtils::FindCnstNode(root, propPath, destNode,&destOptions);

		if (nodeFound) {

			XMP_VarString rootName(propPath[1].step);	// The schema is [0].
			for (size_t i = 2; i < propPath.size(); ++i) {
				XMP_OptionBits stepKind = GetStepKind(propPath[i].options);
				if (stepKind <= kXMP_QualifierStep) rootName += '/';
				rootName += propPath[i].step;
			}

			propName = rootName.c_str();
			size_t leafOffset = rootName.size();
			while ((leafOffset > 0) && (propName[leafOffset] != '/') && (propName[leafOffset] != '[')) --leafOffset;
			if (propName[leafOffset] == '/') ++leafOffset;

			info.tree.nodeChildren.push_back( IteratorNode( destOptions, propName, leafOffset ) );
			SetCurrentSchema(propPath[kSchemaStep].step.c_str());
			if (options & kXMP_IterJustChildren) {

				AddNodeOffSpring(info.tree.nodeChildren.back(), destNode);
			}

		}
	
	
	} else if ( *schemaNS != 0 ) {

		info.tree.nodeChildren.push_back(IteratorNode(kXMP_SchemaNode, schemaNS, 0));
		IteratorNode & iterSchema = info.tree.nodeChildren.back();

		bool schemaFound = false;

		for (auto childIter = mDOM->Iterator(); childIter; childIter = childIter->Next()) {

			if (!strcmp(childIter->GetNode()->GetNameSpace()->c_str(), schemaNS)) {

				schemaFound = true;
				break;
			}
		}
		

		if (schemaFound) AddSchemaProperties(iterSchema, schemaNS);

		if (iterSchema.nodeChildren.empty()) {
			info.tree.nodeChildren.pop_back();	// No properties, remove the schema node.
		}
		else {
			SetCurrentSchema(schemaNS);
		}
		
	
	} else {

		std::map < XMP_VarString, bool > schemaProperties;

		for (auto childIter = mDOM->Iterator(); childIter; childIter = childIter->Next()) {

			spINode childNode = childIter->GetNode();

			schemaProperties[childNode->GetNameSpace()->c_str()] = true;
		}


		for (auto key : schemaProperties) {
			//TODO check name
			info.tree.nodeChildren.push_back(IteratorNode( kXMP_SchemaNode, key.first, 0 ));

			IteratorNode & iterSchema = info.tree.nodeChildren.back();

			if (!(info.options & kXMP_IterJustChildren)) {
				AddSchemaProperties(iterSchema, key.first.c_str());
			//	if (iterSchema.nodeChildren.empty()) info.tree.nodeChildren.pop_back();	// No properties, remove the schema node.
			}
		}




	}

	info.currPos = info.tree.nodeChildren.begin();
	info.endPos = info.tree.nodeChildren.end();

	if ((info.options & kXMP_IterJustChildren) && (info.currPos != info.endPos) && (*schemaNS != 0)) {
		info.currPos->visitStage = kIter_VisitSelf;
	}
	

	
}	// XMPIterator for XMPMeta objects

// -------------------------------------------------------------------------------------------------
// XMPIterator
// -----------
//
// Constructor for iterations over global tables such as registered namespaces or aliases.

XMPIterator2::XMPIterator2 ( XMP_StringPtr  schemaNS,
	XMP_StringPtr  propName,
	XMP_OptionBits options) : XMPIterator(schemaNS, propName, options)
{
	void * p; p = &schemaNS; p = &propName; p = &options;	// Avoid unused param warnings.
	XMP_Throw("Unimplemented XMPIterator constructor for global tables", kXMPErr_Unimplemented);

}	// XMPIterator for global tables

// -------------------------------------------------------------------------------------------------
// ~XMPIterator
// -----------

XMPIterator2::~XMPIterator2() RELEASE_NO_THROW
{
	
	// Let everything else default.
	
}	// ~XMPIterator

// =================================================================================================
// Iteration Methods
// =================================================================================================

// -------------------------------------------------------------------------------------------------
// Next
// ----
//
// Do a preorder traversal of the cached nodes.

// *** Need to document the relationships between currPos, endPos, and visitStage.

bool
XMPIterator2::Next ( XMP_StringPtr * schemaNS,
					XMP_StringLen *	 nsSize,
					XMP_StringPtr *	 propPath,
					XMP_StringLen *	 pathSize,
					XMP_StringPtr *	 propValue,
					XMP_StringLen *	 valueSize,
					XMP_OptionBits * propOptions )
{

	if (info.currPos == info.endPos) return false;
	using namespace AdobeXMPCore;
	using namespace AdobeXMPCommon;
	bool isSchema = false;
	spcINode xmpNode = GetNextNode(isSchema);
	if (!xmpNode && !isSchema) return false;
	bool isSchemaNode = isSchema;
	if (info.options & kXMP_IterJustLeafNodes) {
		while (isSchemaNode || ( xmpNode && XMPUtils::GetNodeChildCount(xmpNode)))  {
			info.currPos->visitStage = kIter_VisitQualifiers;	// Skip to this node's children.
			xmpNode = GetNextNode(isSchemaNode);
			if (xmpNode == libcppNULL && !isSchemaNode) return false;
			isSchemaNode = XMP_NodeIsSchema(info.currPos->options);
		}
	}

	*schemaNS = info.currSchema.c_str();
	*nsSize = (XMP_Uns32)info.currSchema.size();
	

	*propOptions = info.currPos->options;

	*propPath = "";
	*pathSize = 0;
	*propValue = "";
	*valueSize = 0;

	if (!(*propOptions & kXMP_SchemaNode)) {

		*propPath = info.currPos->fullPath.c_str();
		*pathSize = (XMP_Uns32)info.currPos->fullPath.size();

		if (info.options & kXMP_IterJustLeafName) {
			*propPath += info.currPos->leafOffset;
			*pathSize -= info.currPos->leafOffset;
			if (! xmpNode->IsArrayItem()) {
				*schemaNS = xmpNode->GetNameSpace()->c_str();
				*nsSize = (XMP_Uns32)xmpNode->GetNameSpace()->size();
			}
			else {
				*schemaNS = "";
				*nsSize = 0;
			}

		}

		if (!(*propOptions & kXMP_PropCompositeMask)) {
			spcIUTF8String nodeValue = xmpNode->ConvertToSimpleNode()->GetValue();
			*propValue = nodeValue->c_str();
			*valueSize = (XMP_Uns32)nodeValue->size();
		}

	}

	return true;
}	// Next

// -------------------------------------------------------------------------------------------------
// Skip
// ----
//
// Skip some portion of the traversal related to the last visited node. We skip either that node's
// children, or those children and the previous node's siblings. The implementation might look a bit
// awkward because info.currNode always points to the next node to be visited. We might already have
// moved past the things to skip, e.g. if the previous node was simple and the last of its siblings.

enum {
	kXMP_ValidIterSkipOptions = kXMP_IterSkipSubtree | kXMP_IterSkipSiblings
};


void
XMPIterator2::Skip ( XMP_OptionBits iterOptions )
{
	//	if ( (info.currPos == kIter_NullPos) )  XMP_Throw ( "No prior postion to skip from", kXMPErr_BadIterPosition );
	if (iterOptions == 0) XMP_Throw("Must specify what to skip", kXMPErr_BadOptions);
	if ((iterOptions & ~kXMP_ValidIterSkipOptions) != 0) XMP_Throw("Undefined options", kXMPErr_BadOptions);


	if (iterOptions & kXMP_IterSkipSubtree) {

		info.currPos->visitStage = kIter_VisitChildren;
	}
	else if (iterOptions & kXMP_IterSkipSiblings) {

		info.currPos = info.endPos;
		AdvanceIteratorPosition();
	}

}	// Skip



// =================================================================================================
#endif  // ENABLE_CPP_DOM_MODEL
