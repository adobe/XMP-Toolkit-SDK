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

//#define ENABLE_XMP_CPP_INTERFACE 1

// Must be defined to instantiate template classes
#define TXMP_STRING_TYPE std::string

// Ensure XMP templates are instantiated
#include "public/include/XMP.incl_cpp"

// Provide access to the API
#include "public/include/XMP.hpp"

#include <iostream>
#include <fstream>

// Made up namespace URI.  Prefix will be xsdkEdit and xsdkUser
const XMP_StringPtr  kXMP_NS_SDK_EDIT = "http://ns.adobe/meta/sdk/Edit/";
const XMP_StringPtr  kXMP_NS_SDK_USERS = "http://ns.adobe/meta/sdk/User/";

using namespace std;

/**
* Client defined callback function to dump XMP to a file.  In this case an output file stream is used
* to write a buffer, of length bufferSize, to a text file.  This callback is called multiple 
* times during the DumpObject() operation.  See the XMP API reference for details of 
* XMP_TextOutputProc() callbacks.
*/
XMP_Status XMPFileDump(void * refCon, XMP_StringPtr buffer, XMP_StringLen bufferSize)
{
	XMP_Status status = 0;
	try
	{
		ofstream * outFile = static_cast<ofstream*>(refCon);
		(*outFile).write(buffer, bufferSize);
	}
	catch(XMP_Error & e)
	{
		cout << e.GetErrMsg() << endl;
		return -1;
	}
	return status;
}

/**
* Client defined callback function to dump the registered namespaces to a file.  In this case 
* an output file stream is used to write a buffer, of length bufferSize, to a text file.  This 
* callback is called multiple times during the DumpObject() operation.  See the XMP API 
* reference for details of XMP_TextOutputProc() callbacks.
*/
XMP_Status DumpNS(void * refCon, XMP_StringPtr buffer, XMP_StringLen bufferSize)
{
	XMP_Status status = 0;

	try
	{
		ofstream *outFile= static_cast<ofstream*>(refCon);
		(*outFile).write(buffer, bufferSize);
	}
	catch(XMP_Error & e)
	{
		cout << e.GetErrMsg() << endl;
		return -1;
	}
	return status;
}

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
int main()
{
	if(!SXMPMeta::Initialize())
	{
		cout << "Could not initialize Toolkit!";
	}
	else
	{
		try
		{
			// Register the namespaces
			string actualPrefix;
			SXMPMeta::RegisterNamespace(kXMP_NS_SDK_EDIT, "xsdkEdit", &actualPrefix);
			SXMPMeta::RegisterNamespace(kXMP_NS_SDK_USERS, "xsdkUser",&actualPrefix);

			SXMPMeta meta;

			// Adds a user of the document
			// 1.  Add a new item onto the DocumentUsers array - 
			// 2.  Compose a path to the last element of DocumentUsers array
			// 3.  Add a value for the User field of the UserDetails structure
			// 4.  Add a qualifier to the User field.  Compose the path and set the value
			// 5.  Add a value for the DUID field of the UserDetails structure
			// 6.  Add a Contact property for the ContactDetails field of the UserDetails structure
			// 7.  Compose a path to the ContactDetails field of the UserDetails structure.
			// 8.  Create the fields of the ContactDetails structure and provide values

			// Create/Append the top level DocumentUsers array.  If the array exists a new item will be added
			meta.AppendArrayItem(kXMP_NS_SDK_EDIT, "DocumentUsers", kXMP_PropValueIsArray, 0, kXMP_PropValueIsStruct);

			// Compose a path to the last item in the DocumentUsers array, this will point to a UserDetails structure
			string userItemPath;
			SXMPUtils::ComposeArrayItemPath(kXMP_NS_SDK_EDIT, "DocumentUsers", kXMP_ArrayLastItem, &userItemPath);

			// We now have a path to the structure, so we can set the field values
			meta.SetStructField(kXMP_NS_SDK_EDIT, userItemPath.c_str(), kXMP_NS_SDK_USERS, "User", "John Smith", 0);

			// Add a qualifier to the User field, first compose the path to the field and then add the qualifier
			string userFieldPath;
			SXMPUtils::ComposeStructFieldPath(kXMP_NS_SDK_EDIT, userItemPath.c_str(), kXMP_NS_SDK_USERS, "User", &userFieldPath);
			meta.SetQualifier(kXMP_NS_SDK_EDIT, userFieldPath.c_str(), kXMP_NS_SDK_USERS, "Role", "Dev Engineer");

			// Compose a path to the DUID and set field value
			string duidPath;
			SXMPUtils::ComposeStructFieldPath(kXMP_NS_SDK_EDIT, userItemPath.c_str(), kXMP_NS_SDK_USERS, "DUID", &duidPath);
			meta.SetProperty_Int(kXMP_NS_SDK_EDIT, duidPath.c_str(), 2, 0);

			// Add the ContactDetails field, this field is a Contact structure
			meta.SetStructField(kXMP_NS_SDK_EDIT, userItemPath.c_str(), kXMP_NS_SDK_USERS, "ContactDetails", 0, kXMP_PropValueIsStruct);

			// Compose a path to the field that has the ContactDetails structure
			string contactStructPath;
			SXMPUtils::ComposeStructFieldPath(kXMP_NS_SDK_EDIT, userItemPath.c_str(), kXMP_NS_SDK_USERS, "ContactDetails", &contactStructPath);

			// Now add the fields - all empty initially
			meta.SetStructField(kXMP_NS_SDK_EDIT, contactStructPath.c_str(), kXMP_NS_SDK_USERS, "Email", 0, kXMP_PropArrayIsAlternate);	
			meta.SetStructField(kXMP_NS_SDK_EDIT, contactStructPath.c_str(), kXMP_NS_SDK_USERS, "Telephone", 0, kXMP_PropValueIsArray);
			meta.SetStructField(kXMP_NS_SDK_EDIT, contactStructPath.c_str(), kXMP_NS_SDK_USERS, "BaseLocation", "", 0);

			// Add some values for the fields
			// Email: Get the path to the field named 'Email' in the ContactDetails structure and use it to append items
			string path;
			SXMPUtils::ComposeStructFieldPath(kXMP_NS_SDK_EDIT, contactStructPath.c_str(), kXMP_NS_SDK_USERS, "Email", &path);
			meta.AppendArrayItem(kXMP_NS_SDK_EDIT, path.c_str(), 0, "js@adobe.meta.com", 0);
			meta.AppendArrayItem(kXMP_NS_SDK_EDIT, path.c_str(), 0, "js@adobe.home.com", 0);
			
			// Telephone
			SXMPUtils::ComposeStructFieldPath(kXMP_NS_SDK_EDIT, contactStructPath.c_str(), kXMP_NS_SDK_USERS, "Telephone", &path);
			meta.AppendArrayItem(kXMP_NS_SDK_EDIT, path.c_str(), 0, "89112", 0);
			meta.AppendArrayItem(kXMP_NS_SDK_EDIT, path.c_str(), 0, "84432", 0);

			// BaseLocation
			SXMPUtils::ComposeStructFieldPath(kXMP_NS_SDK_EDIT, contactStructPath.c_str(), kXMP_NS_SDK_USERS, "BaseLocation", &path);
			meta.SetProperty(kXMP_NS_SDK_EDIT, path.c_str(), "London", 0);

			// Add a user edit
			// 1.  Add an item (a structure) to the DocumentEdit array
			// 2.  Compose a path to the last item in the DocumentEdit array
			// 3.  Add fields and values to the EditDetails structure

			// Create the array
			meta.AppendArrayItem(kXMP_NS_SDK_EDIT, "DocumentEdit", kXMP_PropArrayIsOrdered, 0, kXMP_PropValueIsStruct);

			// Compose a path to the last item of the DocumentEdit array, this gives the path to the structure
			string lastItemPath;
			SXMPUtils::ComposeArrayItemPath(kXMP_NS_SDK_EDIT, "DocumentEdit", kXMP_ArrayLastItem, &lastItemPath);

			// Add the Date field
			SXMPUtils::ComposeStructFieldPath(kXMP_NS_SDK_EDIT, lastItemPath.c_str(), kXMP_NS_SDK_EDIT, "EditDate", &path);
			XMP_DateTime dt;
			SXMPUtils::CurrentDateTime(&dt);
			meta.SetProperty_Date(kXMP_NS_SDK_EDIT, path.c_str(), dt, 0);

			// Add the DUID field
			SXMPUtils::ComposeStructFieldPath(kXMP_NS_SDK_EDIT, lastItemPath.c_str(), kXMP_NS_SDK_EDIT, "DUID", &path);
			meta.SetProperty_Int(kXMP_NS_SDK_EDIT, path.c_str(), 2, 0);
			
			// Add the EditComments field
			SXMPUtils::ComposeStructFieldPath(kXMP_NS_SDK_EDIT, lastItemPath.c_str(), kXMP_NS_SDK_EDIT, "EditComments", &path);
			meta.SetLocalizedText(kXMP_NS_SDK_EDIT, path.c_str(), "en", "en-US", "Document created.", 0);

			// Add the EditTool field
			meta.SetStructField(kXMP_NS_SDK_EDIT, lastItemPath.c_str(), kXMP_NS_SDK_EDIT, "EditTool", "FrameXML", 0);
			
			// Write the RDF to a file
			cout << "writing RDF to file CS_RDF.txt" << endl;
			string metaBuffer;
			meta.SerializeToBuffer(&metaBuffer);
			writeRDFToFile(&metaBuffer, "CS_RDF.txt");
			
			// Dump the XMP object
			cout << "dumping XMP object to file XMPDump.txt" << endl;
			ofstream dumpFile;
			dumpFile.open("XMPDump.txt", ios::out);
			meta.DumpObject(XMPFileDump, &dumpFile);
			dumpFile.close();

			// Dump the namespaces to a file
			cout << "dumping namespaces to file NameDump.txt" << endl;
			dumpFile.open("NameDump.txt", ios::out);
			meta.DumpNamespaces(XMPFileDump, &dumpFile);
			dumpFile.close();
			
		}
		catch(XMP_Error & e)
		{
			cout << "ERROR: " << e.GetErrMsg();
		}

		SXMPMeta::Terminate();
	}

	return 0;
}
