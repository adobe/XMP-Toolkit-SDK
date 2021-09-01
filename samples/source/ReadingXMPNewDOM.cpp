// =================================================================================================
// Copyright 2008 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

/**
* Tutorial solution for the Walkthrough 1 in the XMP Programmers Guide, Opening files and reading XMP.
* Demonstrates the basic use of the XMPFiles and XMPCore components, obtaining read-only XMP from a file
* and examining it through the XMP object.
*/

#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>

//#define ENABLE_XMP_CPP_INTERFACE 1;

// Must be defined to instantiate template classes
#define TXMP_STRING_TYPE std::string 

// Must be defined to give access to XMPFiles
#define XMP_INCLUDE_XMPFILES 1 

#define ENABLE_NEW_DOM_MODEL 1

// Ensure XMP templates are instantiated
#include "public/include/XMP.incl_cpp"

// Provide access to the API
#include "public/include/XMP.hpp"

#include "XMPCore/Interfaces/IDOMImplementationRegistry.h"
#include "XMPCore/Interfaces/IDOMParser.h"
#include "XMPCore/Interfaces/IDOMSerializer.h"
#include "XMPCore/Interfaces/IMetadata.h"
#include "XMPCore/Interfaces/ICoreObjectFactory.h"
#include "XMPCore/Interfaces/ISimpleNode.h"
#include "XMPCore/Interfaces/IStructureNode.h"
#include "XMPCore/Interfaces/IArrayNode.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCore/Interfaces/INodeIterator.h"

using namespace std;
using namespace AdobeXMPCore;


void GetLocalizedText(spIArrayNode titleNode, const char* specificLang, const char* genericLang, string lang)
{
	AdobeXMPCore::spINode currItem;
	const size_t itemLim = titleNode->ChildCount();
	size_t itemNum;

	spISimpleNode xmlLangQualifierNode, currItemNode;
	for (itemNum = 1; itemNum <= itemLim; ++itemNum)
	{
		currItem = titleNode->GetNodeAtIndex(itemNum);
		if (currItem != NULL)
		{
			xmlLangQualifierNode = currItem->QualifiersIterator()->GetNode()->ConvertToSimpleNode();
			if (!strcmp(xmlLangQualifierNode->GetValue()->c_str(), specificLang)) {
				currItemNode = currItem->ConvertToSimpleNode();
				cout << "dc:title in" <<" " << lang <<" " << currItemNode->GetValue()->c_str() << endl;
				return;
			}
		}
	}

	if (*genericLang != 0)
	{
		// Look for the first partial match with the generic language.
		const size_t genericLen = strlen(genericLang);
		for (itemNum = 1; itemNum <= itemLim; ++itemNum) {
			currItem = titleNode->GetNodeAtIndex(itemNum);
			xmlLangQualifierNode = currItem->QualifiersIterator()->GetNode()->ConvertToSimpleNode();
			XMP_StringPtr currLang = xmlLangQualifierNode->GetValue()->c_str();
			const size_t currLangSize = xmlLangQualifierNode->GetValue()->size();
			if ((currLangSize >= genericLen) &&
				!strncmp(currLang, genericLang, genericLen) &&
				((currLangSize == genericLen) || (currLang[genericLen] == '-')))
			{
				currItemNode = currItem->ConvertToSimpleNode();
				cout << "dc:title in" <<" " << lang << " " << currItemNode->GetValue()->c_str() << endl;
				return;
			}
		}
	}

	// Look for an 'x-default' item.
	for (itemNum = 1; itemNum <= itemLim; ++itemNum) {
		currItem = titleNode->GetNodeAtIndex(itemNum);
		xmlLangQualifierNode = currItem->QualifiersIterator()->GetNode()->ConvertToSimpleNode();
		if (!strcmp(xmlLangQualifierNode->GetValue()->c_str(), "x-default")) {
			currItemNode = currItem->ConvertToSimpleNode();
			cout << "dc:title in" <<" " << lang <<" " << currItemNode->GetValue()->c_str() << endl;
			return;
		}
	}

	// Everything failed, choose the first item.
	currItem = titleNode->GetNodeAtIndex(1);
	currItemNode = currItem->ConvertToSimpleNode();
	cout << "dc:title in" <<" " <<lang << " "<< currItemNode->GetValue() << endl;
	return;

}



void writeRDFToFile(string * rdf, string filename)
{
	ofstream outFile;
	outFile.open(filename.c_str(), ios::out);
	outFile << *rdf;
	outFile.close();
}


/**
*	Initializes the toolkit and attempts to open a file for reading metadata.Initially
* an attempt to open the file is done with a handler, if this fails then the file is opened with
* packet scanning.Once the file is open several properties are read and displayed in the console.
* The XMP object is then dumped to a text file and the resource file is closed.
*/

int main(int argc, const char * argv[])
{

	if (argc != 2) // 2 := command and 1 parameter
	{
		cout << "usage: ReadingXMP (filename)" << endl;
		return 0;
	}

	string filename = string(argv[1]);

	if (!SXMPMeta::Initialize())
	{
		cout << "Could not initialize toolkit!";
		return -1;
	}
	XMP_OptionBits options = 0;
#if UNIX_ENV
	options |= kXMPFiles_ServerMode;
#endif
	// Must initialize SXMPFiles before we use it
	if (!SXMPFiles::Initialize(options))
	{
		cout << "Could not initialize SXMPFiles.";
		return -1;
	}

	try
	{
		// Options to open the file with - read only and use a file handler
		XMP_OptionBits opts = kXMPFiles_OpenForRead | kXMPFiles_OpenUseSmartHandler;

		bool ok;
		SXMPFiles myFile;
		std::string status = "";

		// First we try and open the file
		ok = myFile.OpenFile(filename, kXMP_UnknownFile, opts);
		if (!ok)
		{
			status += "No smart handler available for " + filename + "\n";
			status += "Trying packet scanning.\n";

			// Now try using packet scanning
			opts = kXMPFiles_OpenForUpdate | kXMPFiles_OpenUsePacketScanning;
			ok = myFile.OpenFile(filename, kXMP_UnknownFile, opts);
		}
		
		// If the file is open then read the metadata
		if (ok)
		{
			
			cout << status << endl;
			cout << filename << " is opened successfully" << endl;
			// Create the xmp object and get the xmp data
			SXMPMeta meta;
			myFile.GetXMP(&meta);
			string buffer;
			meta.SerializeToBuffer(&buffer);
			writeRDFToFile(&buffer, "Image1RDF.txt");
			AdobeXMPCore::spIDOMImplementationRegistry DOMRegistry = AdobeXMPCore::IDOMImplementationRegistry::GetDOMImplementationRegistry();
			AdobeXMPCore:: spIDOMParser parser = DOMRegistry->GetParser("rdf");
			AdobeXMPCore ::spIMetadata metaNode = parser->Parse(buffer.c_str(), buffer.size());

			// Read a simple property
			AdobeXMPCore::spISimpleNode simpleNode = metaNode->GetSimpleNode(kXMP_NS_XMP, AdobeXMPCommon::npos, "CreatorTool", AdobeXMPCommon::npos);
			if (simpleNode != NULL)
			{
				string simpleNodeValue = simpleNode->GetValue()->c_str();
				cout << "CreatorTool = " << simpleNodeValue << endl;
			}

			// Get the first element in the dc:creator array
			AdobeXMPCore::spIArrayNode arrayNode = metaNode->GetArrayNode(kXMP_NS_DC, AdobeXMPCommon::npos, "creator", AdobeXMPCommon::npos);
			if (arrayNode != NULL)
			{
				AdobeXMPCore::spISimpleNode arrayNodeChild = arrayNode->GetSimpleNodeAtIndex(1);
				if (arrayNodeChild != NULL)
				{
					string arrayNodeChildValue = arrayNodeChild->GetValue()->c_str();
					cout << "dc:creator = " << arrayNodeChildValue << endl;
				}
			}

			// Get the the entire dc:subject array 
			AdobeXMPCore::spIArrayNode subjectArray = metaNode->GetArrayNode(kXMP_NS_DC, AdobeXMPCommon::npos, "subject", AdobeXMPCommon::npos);
			if (subjectArray != NULL)
			{
				sizet arraySize = subjectArray->ChildCount();
				for (sizet i = 1; i <= arraySize; i++)
				{
					AdobeXMPCore::spISimpleNode subjectChild = subjectArray->GetSimpleNodeAtIndex(i);
					if (subjectChild != NULL)
					{
						string propValue = subjectChild->GetValue()->c_str();
						cout << "dc:subject[" << i << "] = " << propValue << endl;
					}
				}
			}
			
		    // Get the dc:title for English and French
			
			
			AdobeXMPCore::spIArrayNode titleNode = metaNode->GetArrayNode(kXMP_NS_DC, AdobeXMPCommon::npos, "title", AdobeXMPCommon::npos);
			if (titleNode != NULL)
			{
				GetLocalizedText(titleNode, "en-US", "en", "English");
				GetLocalizedText(titleNode, "fr-FR", "fr", "French");
			}
			
			
		    
			// Get dc:MetadataDate			
			AdobeXMPCore::spISimpleNode dateNode = metaNode->GetSimpleNode(kXMP_NS_XMP, AdobeXMPCommon::npos, "MetadataDate", AdobeXMPCommon::npos);
			if(dateNode != NULL)
			{
				string date = dateNode->GetValue()->c_str();
				cout << "meta:MetadataDate = " << date << endl;
			}
			

			// See if the flash struct exists and see if it was used
			AdobeXMPCore::spIStructureNode flashNode = metaNode->GetStructureNode(kXMP_NS_EXIF, AdobeXMPCommon::npos, "Flash", AdobeXMPCommon::npos);
			if (flashNode != NULL)
			{
				AdobeXMPCore::spISimpleNode field = flashNode->GetSimpleNode(kXMP_NS_EXIF, AdobeXMPCommon::npos, "Fired", AdobeXMPCommon::npos);
				if (field != NULL)
				{
					string fieldValue = field->GetValue()->c_str();
					cout << "Flash Used = " << fieldValue << endl;
				}
			}
			// Close the SXMPFile.  The resource file is already closed if it was
			// opened as read only but this call must still be made.
			myFile.CloseFile();

		}
		else
		{
			cout << "Unable to open " << filename << endl;
		}
	}
	catch (XMP_Error & e)
	{
		cout << "ERROR: " << e.GetErrMsg() << endl;
	}

	// Terminate the toolkit
	SXMPFiles::Terminate();
	SXMPMeta::Terminate();

	return 0;
}
