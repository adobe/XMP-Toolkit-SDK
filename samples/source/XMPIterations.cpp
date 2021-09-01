// =================================================================================================
// Copyright 2008 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

/**
 * Demonstrates how to use the iteration utility in the XMPCore component to walk through property trees.
 */

#include <cstdio>
#include <vector>
#include <string>
#include <cstring>

//#define ENABLE_XMP_CPP_INTERFACE 1;

// Must be defined to instantiate template classes
#define TXMP_STRING_TYPE std::string

// Must be defined to give access to XMPFiles
#define XMP_INCLUDE_XMPFILES 1 

// Ensure XMP templates are instantiated
#include "public/include/XMP.incl_cpp"

// Provide access to the API
#include "public/include/XMP.hpp"

#include <iostream>

using namespace std;

// Provide some custom XMP
static const char * rdf =
"<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>"
"  <rdf:Description rdf:about='' xmlns:xmpTest='http://ns.adobe.com/xmpTest/'>"
""
"	 <xmpTest:MySimpleProp rdf:parseType='Resource'>"
"		<rdf:value>A Value</rdf:value>"
"		<xmpTest:MyQual>Qual Value</xmpTest:MyQual>"
"  </xmpTest:MySimpleProp>"
""
"		<xmpTest:MyTopStruct rdf:parseType='Resource'>"
"			<xmpTest:MySecondStruct rdf:parseType='Resource'>"
"				<xmpTest:MyThirdStruct rdf:parseType='Resource'>"
"					<xmpTest:MyThirdStructField>Field Value 3</xmpTest:MyThirdStructField>"
"       </xmpTest:MyThirdStruct>"
"				<xmpTest:MySecondStructField>Field Value 2</xmpTest:MySecondStructField>"
"     </xmpTest:MySecondStruct>"
"    <xmpTest:MyTopStructField>Field Value 1</xmpTest:MyTopStructField>"
"   </xmpTest:MyTopStruct>"

"   <xmpTest:MyArrayWithNestedArray>"
"			<rdf:Bag>"
"				<rdf:li>"
"					<rdf:Seq>"
"						<rdf:li>Item 1</rdf:li>"
"           <rdf:li>Item 2</rdf:li>"
"         </rdf:Seq>"
"				</rdf:li>"
"			</rdf:Bag>"
"   </xmpTest:MyArrayWithNestedArray>"

"   <xmpTest:MyArrayWithStructures>"
"			<rdf:Seq>"
"				<rdf:li rdf:parseType='Resource'>"
"					<rdf:value>Field Value 1</rdf:value>"
"						<xmpTest:FirstQual>Qual Value 1</xmpTest:FirstQual>"
"           <xmpTest:SecondQual>Qual Value 2</xmpTest:SecondQual>"
"        </rdf:li>"
"        <rdf:li rdf:parseType='Resource'>"
"					<rdf:value>Field Value 2</rdf:value>"
"						<xmpTest:FirstQual>Qual Value 3</xmpTest:FirstQual>"
"           <xmpTest:SecondQual>Qual Value 4</xmpTest:SecondQual>"
"        </rdf:li>"
"     </rdf:Seq>"
"   </xmpTest:MyArrayWithStructures>"
""
"		<xmpTest:MyStructureWithArray rdf:parseType='Resource'>"
"			<xmpTest:NestedArray>"
"				<rdf:Bag>"
"					<rdf:li>Item 3</rdf:li>"
"         <rdf:li>Item 4</rdf:li>"
"					<rdf:li>Item 5</rdf:li>"
"					<rdf:li>Item 6</rdf:li>"
"       </rdf:Bag>"
"     </xmpTest:NestedArray>"
"			<xmpTest:NestedArray2>"
"				<rdf:Bag>"
"					<rdf:li>Item 66</rdf:li>"
"         <rdf:li>Item 46</rdf:li>"
"					<rdf:li>Item 56</rdf:li>"
"					<rdf:li>Item 66</rdf:li>"
"       </rdf:Bag>"
"     </xmpTest:NestedArray2>"
"   </xmpTest:MyStructureWithArray>"
""
" </rdf:Description>"
"</rdf:RDF>";

// The namespace to be used.  This will be automatically registered
// when the RDF is parsed.
const XMP_StringPtr kXMP_NS_SDK = "http://ns.adobe.com/xmpTest/";

/**
 * Reads some metadata from a file and appends some custom XMP to it.  Then does several 
 * iterations, using various iterators.  Each iteration is displayed in the console window.
 */
int main()
{
	if(SXMPMeta::Initialize())
	{
		XMP_OptionBits options = 0;
#if UNIX_ENV
        options |= kXMPFiles_ServerMode;
#endif
		if ( SXMPFiles::Initialize ( options ) ) {
			bool ok;
			SXMPFiles myFile;
            
			XMP_OptionBits opts = kXMPFiles_OpenForRead | kXMPFiles_OpenUseSmartHandler;
#if MAC_ENV
            ok = myFile.OpenFile("../../../../testfiles/Image1.jpg", kXMP_UnknownFile, opts);
#else
            ok = myFile.OpenFile("../../../testfiles/Image1.jpg", kXMP_UnknownFile, opts);
#endif
			if(ok)
			{
				SXMPMeta xmp;
				myFile.GetXMP(&xmp);
                
				// Add some custom metadata to the XMP object
				SXMPMeta custXMP(rdf, (XMP_StringLen) strlen(rdf));
				SXMPUtils::ApplyTemplate(&xmp, custXMP, kXMPTemplate_AddNewProperties);
                
				// Store any details from the iter.Next() call
				string schemaNS, propPath, propVal;
                
				// Only visit the immediate children that are leaf properties of the Dublin Core schema
				SXMPIterator dcLeafIter(xmp, kXMP_NS_DC, (kXMP_IterJustChildren | kXMP_IterJustLeafNodes));
				while(dcLeafIter.Next(&schemaNS, &propPath, &propVal))
				{
					cout << schemaNS << "  " << propPath << " = " << propVal << endl;
				}
                
				cout << "----------------------------------" << endl;
                
				// Visit one property from the XMP Basic schema
				SXMPIterator xmpKeywordsIter(xmp, kXMP_NS_XMP, "Keywords", kXMP_IterJustLeafNodes);
				while(xmpKeywordsIter.Next(&schemaNS, &propPath, &propVal))
				{
					cout << schemaNS << "  " << propPath << " = " << propVal << endl;
				}
                
				cout << "----------------------------------" << endl;
                
				// Visit the Dublin Core schema, omit any quailifiers and only
				// show the leaf properties
				SXMPIterator dcIter(xmp, kXMP_NS_DC, (kXMP_IterOmitQualifiers | kXMP_IterJustLeafNodes));
				while(dcIter.Next(&schemaNS, &propPath, &propVal))
				{
					cout << schemaNS << "  " << propPath << " = " << propVal << endl;
				}
                
				cout << "----------------------------------" << endl;
                
				// Visit the Dublin Core schema, omit any quailifiers, 
				// show the leaf properties but only return the leaf name and not the full path
				SXMPIterator dcIter2(xmp, kXMP_NS_DC, (kXMP_IterOmitQualifiers | kXMP_IterJustLeafNodes | kXMP_IterJustLeafName));
				while(dcIter2.Next(&schemaNS, &propPath, &propVal))
				{
					cout << schemaNS << "  " << propPath << " = " << propVal << endl;
				}
                
				cout << "----------------------------------" << endl;
                
				// Iterate over a single namespace.  Show all properties within
				// the Photoshop schema
				SXMPIterator exifIter(xmp, kXMP_NS_Photoshop);
				while(exifIter.Next(&schemaNS, &propPath, &propVal))
				{
					cout << schemaNS << "  " << propPath << " = " << propVal << endl;
				}
                
				cout << "----------------------------------" << endl;
                
				// Just visit the leaf nodes of EXIF properties. That is just 
				// properties that may have values.
				SXMPIterator exifLeafIter(xmp, kXMP_NS_EXIF, kXMP_IterJustLeafNodes);
				while(exifLeafIter.Next(&schemaNS, &propPath, &propVal))
				{
					cout << schemaNS << "  " << propPath << " = " << propVal << endl;
				}
                
				cout << "----------------------------------" << endl;
                
				// Iterate over all properties but skip the EXIF schema and skip the custom schema
				// and continue visiting nodes
				SXMPIterator skipExifIter (xmp);
				while(skipExifIter.Next(&schemaNS, &propPath, &propVal))
				{
					if(schemaNS == kXMP_NS_EXIF || schemaNS == kXMP_NS_SDK)
					{
						skipExifIter.Skip(kXMP_IterSkipSubtree);
					}
					else
					{ 
						cout << schemaNS << "  " << propPath << " = " << propVal << endl;
					}
				}
                
				cout << "----------------------------------" << endl;
                
				// Iterate over all properties but skip the EXIF schema
				// and any remaining siblings of the current node.
				SXMPIterator stopAfterExifIter ( xmp );
				while(stopAfterExifIter.Next(&schemaNS, &propPath, &propVal))
				{
					if(schemaNS == kXMP_NS_EXIF || schemaNS == kXMP_NS_SDK)
					{
						stopAfterExifIter.Skip(kXMP_IterSkipSiblings);
					}
					else
					{
						cout << schemaNS << "  " << propPath << " = " << propVal << endl;
					}
				}
                
				cout << "----------------------------------" << endl;
                
				//////////////////////////////////////////////////////////////////////////////////////
                
				// Iterate over the custom XMP
                
				// Visit the immediate children of this node. 
				// No qualifiers are visisted as they are below the property being visisted.
				SXMPIterator justChildrenIter(xmp, kXMP_NS_SDK, kXMP_IterJustChildren);
				while(justChildrenIter.Next(&schemaNS, &propPath, &propVal))
				{
					cout << propPath << " = " << propVal << endl;
				}
                
				cout << "----------------------------------" << endl;
                
				// Visit the immediate children of this node but only those that may have values.
				// No qualifiers are visisted as they are below the property being visisted.
				SXMPIterator justChildrenAndLeafIter(xmp, kXMP_NS_SDK, (kXMP_IterJustChildren | kXMP_IterJustLeafNodes));
				while(justChildrenAndLeafIter.Next(&schemaNS, &propPath, &propVal))
				{
					cout << propPath << " = " << propVal << endl;
				}
                
				cout << "----------------------------------" << endl;
                
				// Visit the leaf nodes of TopStructProperty
				SXMPIterator myTopStructIter(xmp, kXMP_NS_SDK, "MyTopStruct", kXMP_IterJustLeafNodes);
				while(myTopStructIter.Next(&schemaNS, &propPath, &propVal))
				{
					cout << propPath << " = " << propVal << endl;
				}
                
				cout << "----------------------------------" << endl;
                
				// Visit the leaf nodes of the TopStructProperty but only return the names for 
				// the leaf components and not the full path
				SXMPIterator xmyTopStructIterShortNames(xmp, kXMP_NS_SDK, "MyTopStruct", (kXMP_IterJustLeafNodes | kXMP_IterJustLeafName));
				while(xmyTopStructIterShortNames.Next(&schemaNS, &propPath, &propVal))
				{
					cout << propPath << " = " << propVal << endl;
				}
                
				cout << "----------------------------------" << endl;
                
				// Visit a property and all of the qualifiers
				SXMPIterator iterArrayProp (xmp, kXMP_NS_SDK, "ArrayWithStructures", kXMP_IterJustLeafNodes );
				while(iterArrayProp.Next(&schemaNS, &propPath, &propVal))
				{
					cout << propPath << " = " << propVal << endl;
				}
                
				cout << "----------------------------------" << endl;
                
				// Visit a property and omit all of the qualifiers
				SXMPIterator iterArrayPropNoQual (xmp, kXMP_NS_SDK, "ArrayWithStructures", (kXMP_IterJustLeafNodes | kXMP_IterOmitQualifiers));
				while(iterArrayPropNoQual.Next(&schemaNS, &propPath, &propVal))
				{
					cout << propPath << " = " << propVal << endl;
				}
                
				cout << "----------------------------------" << endl;
                
				// Skip a subtree and continue onwards.  Once 'Item 4' is found then the we can skip all of the
				// siblings of the current node.  If the the current node were a top level node the iteration 
				// would be complete as all siblings would be skipped.  However, when 'Item 4' is found the current 
				// node is not at the top level so there are other nodes further up the tree that still need to be
				// visited.
				SXMPIterator skipIter (xmp, kXMP_NS_SDK, (kXMP_IterJustLeafNodes | kXMP_IterOmitQualifiers | kXMP_IterJustLeafName));
				while(skipIter.Next(&schemaNS, &propPath, &propVal))
				{
					if(propVal == "Item 4")
					{
						skipIter.Skip(kXMP_IterSkipSiblings);
					}
					else
					{
						cout << schemaNS << "  " << propPath << " = " << propVal << endl;
					}
				}
                
				/*
                 // Visit all properties and qualifiers
                 SXMPIterator allPropsIter(xmp);
                 while(allPropsIter.Next(&schemaNS, &propPath, &propVal))
                 {
                 cout << schemaNS << "  " << propPath << " = " << propVal << endl;
                 }
                 */
			}
		}
	}
	
	SXMPFiles::Terminate();
	SXMPMeta::Terminate();
    
	return 0;
}

