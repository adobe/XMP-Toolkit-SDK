// =================================================================================================
// Copyright 2008 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

/**
* Tutorial solution for the Walkthrough 3 in the XMP Programmers Guide, Working with custom schema.
*
* Demonstrates how to work with a custom schema that has complex properties. It shows how to access
* and modify properties with complex paths using the path composition utilities from the XMP API
*/
#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>

// Must be defined to instantiate template classes
#define TXMP_STRING_TYPE std::string 

// Must be defined to give access to XMPFiles
#define XMP_INCLUDE_XMPFILES 0

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
//#include "XMPCore\Interfaces\IXMPCoreFwdDeclarations.h"
#include "XMPCommon/Interfaces/IUTF8String.h"


// Made up namespace URI.  Prefix will be xsdkEdit and xsdkUser
const XMP_StringPtr  kXMP_NS_SDK_EDIT = "http://ns.adobe/meta/sdk/Edit/";
const XMP_StringPtr  kXMP_NS_SDK_USERS = "http://ns.adobe/meta/sdk/User/";

using namespace std;

/**
* Writes an XMP packet in XML format to a text file
*
* rdf - a pointer to the serialized XMP
* filename - the name of the file to write to
*/
void writeRDFToFile(string * rdf, string filename)
{
	ofstream outFile;
	outFile.open(filename.c_str(), ios::out);
	outFile << *rdf;
	outFile.close();
}

/**
* Registers the namespaces that will be used with the custom schema.  Then adds several new
* properties to that schema.  The properties are complex, containing nested arrays and structures.
*
* XMPFiles is not used in this sample, hence no external resource is updated with the metadata. The
* created XMP object is serialized and written as RDF to a text file, the XMP object is dumped to
* a text file and the registered namespaces are also dumped to a text file.*
*
*/
int main(int argc, const char * argv[])
{
	
	if (!SXMPMeta::Initialize())
	{
		cout << "Could not initialize toolkit!";
		return -1;
	}

	else
	{
		try
		{
			/*
			AdobeXMPCore::spINameSpacePrefixMap map = AdobeXMPCore::INameSpacePrefixMap::CreateNameSpacePrefixMap();
			map->Insert("xsdkEdit", AdobeXMPCommon::npos, kXMP_NS_SDK_EDIT, AdobeXMPCommon::npos);
			map->Insert("xsdkUser", AdobeXMPCommon::npos, kXMP_NS_SDK_USERS, AdobeXMPCommon::npos);
			*/
			
			SXMPMeta::RegisterNamespace(kXMP_NS_SDK_EDIT, "xsdkEdit", NULL);
	    	SXMPMeta::RegisterNamespace(kXMP_NS_SDK_USERS,"xsdkUser", NULL);
			
			// Adds a user of the document
			// 1.  Add a new item onto the DocumentUsers array - 
			// 2.  Compose a path to the last element of DocumentUsers array
			// 3.  Add a value for the User field of the UserDetails structure
			// 4.  Add a qualifier to the User field.  Compose the path and set the value
			// 5.  Add a value for the DUID field of the UserDetails structure
			// 6.  Add a Contact property for the ContactDetails field of the UserDetails structure
			// 7.  Compose a path to the ContactDetails field of the UserDetails structure.
			// 8.  Create the fields of the ContactDetails structure and provide values

			// Create a top Level array node of DocumentUsers. 
			AdobeXMPCore::spIMetadata metaNode = AdobeXMPCore::IMetadata::CreateMetadata();
			AdobeXMPCore::spIArrayNode DUarrayNode = AdobeXMPCore::IArrayNode::CreateUnorderedArrayNode(kXMP_NS_SDK_EDIT, AdobeXMPCommon::npos, "DocumentUsers", AdobeXMPCommon::npos);

			// Append the UserDetails struct node as child of DocumentUsers array node
			AdobeXMPCore::spIStructureNode DUstructNode1 = AdobeXMPCore::IStructureNode::CreateStructureNode(kXMP_NS_SDK_EDIT, AdobeXMPCommon::npos, "UserDetails", AdobeXMPCommon::npos);
			DUarrayNode->AppendNode(DUstructNode1);

			// Create simple nodes with property User and DUID as fields of UserDetails struct node
			AdobeXMPCore::spINode UDsimpleNode1 = AdobeXMPCore::ISimpleNode::CreateSimpleNode(kXMP_NS_SDK_USERS, AdobeXMPCommon::npos, "User", AdobeXMPCommon::npos, "John Smith", AdobeXMPCommon::npos);
			AdobeXMPCore::spINode UDsimpleNode2 = AdobeXMPCore::ISimpleNode::CreateSimpleNode(kXMP_NS_SDK_USERS, AdobeXMPCommon::npos, "DUID", AdobeXMPCommon::npos, "2", AdobeXMPCommon::npos);

			// Create a structure ContactDetails as field of UserDetails structure
			AdobeXMPCore::spIStructureNode UDstructNode3 = AdobeXMPCore::IStructureNode::CreateStructureNode(kXMP_NS_SDK_EDIT, AdobeXMPCommon::npos, "ContactDetails", AdobeXMPCommon::npos);

			// Append all created fields as child of UserDetails struct node
			DUstructNode1->AppendNode(UDsimpleNode1);
			DUstructNode1->AppendNode(UDsimpleNode2);
			DUstructNode1->AppendNode(UDstructNode3);

			// Create an alternative Array E-mail as field of ContactDetails structure
			AdobeXMPCore::spIArrayNode CDarrayNode1 = AdobeXMPCore::IArrayNode::CreateAlternativeArrayNode(kXMP_NS_SDK_EDIT, AdobeXMPCommon::npos, "E-Mail", AdobeXMPCommon::npos);

			// Create an unordered Array Telephone as field of ContactDetails structure
			AdobeXMPCore::spIArrayNode CDarrayNode2 = AdobeXMPCore::IArrayNode::CreateUnorderedArrayNode(kXMP_NS_SDK_EDIT, AdobeXMPCommon::npos, "Telephone", AdobeXMPCommon::npos);

			// Create simple node with property BaseLocation
			AdobeXMPCore::spINode CDsimpleNode3 = AdobeXMPCore::ISimpleNode::CreateSimpleNode(kXMP_NS_SDK_USERS, AdobeXMPCommon::npos, "BaseLocation", AdobeXMPCommon::npos, "London", AdobeXMPCommon::npos);

			// Append all created fields as child of ContactDetails structure
			UDstructNode3->AppendNode(CDarrayNode1);
			UDstructNode3->AppendNode(CDarrayNode2);
			UDstructNode3->AppendNode(CDsimpleNode3);

			//Append items to E-Mail array
			CDarrayNode1->AppendNode(AdobeXMPCore::ISimpleNode::CreateSimpleNode(kXMP_NS_SDK_USERS, AdobeXMPCommon::npos, "Mail1", AdobeXMPCommon::npos, "js@adobe.xmp.com", AdobeXMPCommon::npos));
			CDarrayNode1->AppendNode(AdobeXMPCore::ISimpleNode::CreateSimpleNode(kXMP_NS_SDK_USERS, AdobeXMPCommon::npos, "Mail2", AdobeXMPCommon::npos, "js@adobe.home.com", AdobeXMPCommon::npos));

			//Append items to Telephone array
			CDarrayNode2->AppendNode(AdobeXMPCore::ISimpleNode::CreateSimpleNode(kXMP_NS_SDK_USERS, AdobeXMPCommon::npos, "Phone1", AdobeXMPCommon::npos, "89112", AdobeXMPCommon::npos));
			CDarrayNode2->AppendNode(AdobeXMPCore::ISimpleNode::CreateSimpleNode(kXMP_NS_SDK_USERS, AdobeXMPCommon::npos, "Phone2", AdobeXMPCommon::npos, "84432", AdobeXMPCommon::npos));

			metaNode->AppendNode(DUarrayNode);

			// Create unordered array DocumentEdit
			AdobeXMPCore::spIArrayNode DEarrayNode = AdobeXMPCore::IArrayNode::CreateUnorderedArrayNode(kXMP_NS_SDK_EDIT, AdobeXMPCommon::npos, "DocumentEdit", AdobeXMPCommon::npos);

			// Create structure EditDetails as arrayitem of DocumentEdit array
			AdobeXMPCore::spIStructureNode DEstructNode1 = AdobeXMPCore::IStructureNode::CreateStructureNode(kXMP_NS_SDK_EDIT, AdobeXMPCommon::npos, "EditDetails", AdobeXMPCommon::npos);
			DEarrayNode->AppendNode(DEstructNode1);

			// Obtaining current date and time 
			XMP_DateTime dt;
			SXMPUtils::CurrentDateTime(&dt);
			string  date;
			SXMPUtils::ConvertFromDate(dt, &date);

			// Creating simple node as field of EditDetails structure which will hold the current date
			AdobeXMPCore::spINode EDsimpleNode1 = AdobeXMPCore::ISimpleNode::CreateSimpleNode(kXMP_NS_SDK_EDIT, AdobeXMPCommon::npos, "EditDate", AdobeXMPCommon::npos, date.c_str(), AdobeXMPCommon::npos);

			// Creating simple node Edittool as field of EditDetails structre
			AdobeXMPCore::spINode EDsimpleNode2 = AdobeXMPCore::ISimpleNode::CreateSimpleNode(kXMP_NS_SDK_EDIT, AdobeXMPCommon::npos, "EditTool", AdobeXMPCommon::npos, "FrameXML", AdobeXMPCommon::npos);

			// Appending fields as child of EditDetails structure node
			DEstructNode1->AppendNode(EDsimpleNode1);
			DEstructNode1->AppendNode(EDsimpleNode2);
			metaNode->AppendNode(DEarrayNode);

			// Write the RDF to a file
			cout << "writing RDF to file CS_RDF.txt" << endl;
			AdobeXMPCore::spIDOMImplementationRegistry DOMRegistry = AdobeXMPCore::IDOMImplementationRegistry::GetDOMImplementationRegistry();
			AdobeXMPCore::spIDOMSerializer serializer = DOMRegistry->GetSerializer("rdf");
			std::string serializedPacket = serializer->Serialize(metaNode)->c_str();
			writeRDFToFile(&serializedPacket, "CS_RDF.txt");

		}
		catch (XMP_Error & e)
		{
			cout << "ERROR: " << e.GetErrMsg();
		}

	}

	return 0;

}
