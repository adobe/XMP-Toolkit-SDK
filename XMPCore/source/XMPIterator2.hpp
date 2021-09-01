#ifndef __XMPIterator2_hpp__
#define __XMPIterator2_hpp__

// =================================================================================================
// Copyright 2003 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "public/include/XMP_Environment.h"
#include "public/include/XMP_Const.h"
#include "XMPCore/source/XMPMeta.hpp"
#include "XMPCore/source/XMPIterator.hpp"
#include "XMPCore/XMPCoreFwdDeclarations_I.h"
#include "XMPCore/Interfaces/IMetadata_I.h"
#include "XMPCore/Interfaces/INodeIterator_I.h"
#include <vector>
// =================================================================================================


// =================================================================================================


struct IteratorNode;

typedef std::vector< IteratorNode > IteratorOffSpring;
typedef IteratorOffSpring::iterator  IteratorPos;
typedef std::pair<IteratorPos, IteratorPos> IteratorPosPair;
typedef std::vector< IteratorPosPair> IteratorPosStack;
struct IteratorNode
{
	AdobeXMPCore::spINode						node;
	XMP_Uns8									visitStage;
	IteratorOffSpring							nodeQualifiers, nodeChildren;
	XMP_VarString								fullPath;
	size_t										leafOffset;
	XMP_OptionBits								options;



	IteratorNode() : options(0), leafOffset(0), visitStage(kIter_BeforeVisit)
	{

	};

	IteratorNode(XMP_OptionBits _options, const XMP_VarString& _fullPath, size_t _leafOffset)
		: options(_options), fullPath(_fullPath), leafOffset(_leafOffset), visitStage(kIter_BeforeVisit)
	{

	};
};



struct IteratorState
{
	
	XMP_VarString		schema;
	XMP_OptionBits		options;
		
	XMP_VarString		currSchema;
	IteratorPos			currPos, endPos;
	IteratorPosStack	ancestors;
	IteratorNode 		tree;
	


	IteratorState() : options(0)
	{

	};

	IteratorState(XMP_OptionBits _options) : options(_options)
	{
			
	};

	
	IteratorState(const AdobeXMPCore::spINode & inNode, XMP_VarString inSchema, XMP_VarString inPath) {

		
	/*	schema = inSchema;
		path = inPath;*/
	}

	IteratorState(const AdobeXMPCore::spcINode & inNode, XMP_VarString inSchema, XMP_VarString inPath) {

		/*node = inNode;
		schema = inSchema;
		path = inPath;*/
	}


};
class XMPIterator2: public XMPIterator {
	
public:
	


	XMPIterator2(const XMPMeta & xmpObj,	// Construct a property iterator.
		XMP_StringPtr	  schemaNS,
		XMP_StringPtr	  propName,
		XMP_OptionBits  options);

	XMPIterator2(XMP_StringPtr	 schemaNS,	// Construct a table iterator.
		XMP_StringPtr	 propName,
		XMP_OptionBits options);

	virtual ~XMPIterator2() RELEASE_NO_THROW;

	bool
	Next ( XMP_StringPtr *  schemaNS,
		   XMP_StringLen *  nsSize,
		   XMP_StringPtr *  propPath,
		   XMP_StringLen *  pathSize,
		   XMP_StringPtr *  propValue,
		   XMP_StringLen *  valueSize,
		   XMP_OptionBits * propOptions );

	void
	Skip ( XMP_OptionBits options );

	// ! Expose so that wrappers and file static functions can see the data.
	
	


private:

	

	void AddNodeOffSpring(IteratorNode &iterParent, const AdobeXMPCore::spINode & xmpParent);
	AdobeXMPCore::spINode GetNextNode(bool &);
	void AdvanceIteratorPosition();
	void SetCurrentSchema(XMP_VarString &fullPath);
	void SetCurrentSchema(const char * fullPath);
	void AddSchemaProperties(IteratorNode & iterSchema, const char * nameSpace);
	XMP_OptionBits							iteratorOptions;
	XMP_VarString							fullPath;
	AdobeXMPCore::spINode					currNode;
	IteratorState							info;
	AdobeXMPCore::spIMetadata				mDOM;


};

// =================================================================================================

#endif	// __XMPIterator2_hpp__
