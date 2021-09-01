// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! XMP_Environment.h must be the first included header.

#include "public/include/XMP_Const.h"
#include "public/include/XMP_IO.hpp"

#include "XMPFiles/source/XMPFiles_Impl.hpp"
#include "source/XMPFiles_IO.hpp"
#include "source/XIO.hpp"
#include "source/IOUtils.hpp"

#include "XMPFiles/source/FileHandlers/XDCAMSAM_Handler.hpp"
#include "XMPFiles/source/FormatSupport/XDCAM_Support.hpp"
#include "XMPFiles/source/FormatSupport/PackageFormat_Support.hpp"

bool XDCAMSAM_CheckFormat ( XMP_FileFormat format,
						 const std::string & rootPath,
						 const std::string & groupName,
						 const std::string & parentName,
						 const std::string & leafName,
						 XMPFiles * parent )
{	
	// We only support file in CLPR folder not in other folders

	if ( ( format != kXMP_XDCAM_SAMFile ) && ( format != kXMP_UnknownFile ) ) return false;
	
	// parentName or groupName empty means Logical path exists 
	if ( groupName.empty() != parentName.empty() ) return false;
	
	std::string tempPath = rootPath;
	std::string clipName = leafName;

	if ( groupName.empty() )
	{
		// Logical clip exists
		tempPath += kDirChar;
		tempPath += "PROAV";

		// Simple checks to ensure presence or absence of Management files or CLPR folder
		if ( Host_IO::GetChildMode ( tempPath.c_str(), "INDEX.XML" ) != Host_IO::kFMode_IsFile ) return false;
		if ( Host_IO::GetChildMode ( tempPath.c_str(), "DISCMETA.XML" ) != Host_IO::kFMode_IsFile ) return false;
		if ( Host_IO::GetChildMode ( tempPath.c_str(), "DISCINFO.XML" ) != Host_IO::kFMode_IsFile ) return false;
		if ( Host_IO::GetChildMode ( tempPath.c_str(), "CLPR" ) != Host_IO::kFMode_IsFolder ) return false;
		if ( Host_IO::GetChildMode( tempPath.c_str(), "MEDIAPRO.XML" ) == Host_IO::kFMode_IsFile ) return false;
		tempPath += kDirChar;
		tempPath += "CLPR";
		tempPath += kDirChar + leafName;
	}
	else if ( groupName == "CLPR" )
	{
		// XMP provides support only to files inside CLPR 
		// Simple checks to ensure presence or absence of Management files
		if ( Host_IO::GetChildMode ( tempPath.c_str(), "INDEX.XML" ) != Host_IO::kFMode_IsFile ) return false;
		if ( Host_IO::GetChildMode ( tempPath.c_str(), "DISCMETA.XML" ) != Host_IO::kFMode_IsFile ) return false;
		if ( Host_IO::GetChildMode ( tempPath.c_str(), "DISCINFO.XML" ) != Host_IO::kFMode_IsFile ) return false;
		if ( ( Host_IO::GetChildMode( tempPath.c_str(), "MEDIAPRO.XML" ) == Host_IO::kFMode_IsFile ) ) return false;
		
		tempPath += kDirChar + groupName;
		tempPath += kDirChar + parentName;
		size_t length = clipName.length();
		const char fileType = clipName.at ( length - 3 );
		if ( IsDigit( clipName.at( length - 1 ) ) && IsDigit( clipName.at( length - 2 ) )  )
		{
			// Last 3rd characater shows what is the file type
			switch ( fileType )
			{
				case 'A' :	// Audio 
				case 'C' :	// ClipInfo
				case 'I' :	// Picture pointer
				case 'M' :	// NRT
				case 'R' :	// Real Time
				case 'S' :	// Sub (Proxy)
				case 'V' :	// Video
					break;
				default:	// Unknown
					return false;
			}
			clipName.erase ( clipName.begin() + length - 3, clipName.end() );
		}
	}
	else
		return false;
	tempPath += kDirChar + clipName;
	tempPath += "M01.XML";

	// Checking for NRT file
	if ( Host_IO::GetFileMode ( tempPath.c_str() ) != Host_IO::kFMode_IsFile )
		return false;
	
	return true;

}	// XDCAMSAM_CheckFormat

XMPFileHandler * XDCAMSAM_MetaHandlerCTor ( XMPFiles * parent )
{
	return new XDCAMSAM_MetaHandler ( parent );
}	// XDCAMSAM_MetaHandlerCTor


// =================================================================================================
// XDCAMSAM_MetaHandler::XDCAMSAM_MetaHandler
// ====================================
XDCAMSAM_MetaHandler::XDCAMSAM_MetaHandler ( XMPFiles * _parent ) : XDCAM_MetaHandler(_parent)
{
	this->handlerFlags = kXDCAMSAM_HandlerFlags;
	// Setting the various path variables
	this->SetPathVariables ( this->parent->GetFilePath() );
	
}	// XDCAMSAM_MetaHandler::XDCAMSAM_MetaHandler

// =================================================================================================
// XDCAMSAM_MetaHandler::SetPathVariables
// ====================================
void XDCAMSAM_MetaHandler::SetPathVariables ( const std::string & clientPath )
{
	// No need to check for existing or non existing as would have been done at check file format if ForceGivenHandler flag is not provided
	std::string tempPath = clientPath;
	std::string parentName, groupName;
	std::string ignored;

	std::string leafName;
	XIO::SplitLeafName ( &tempPath, &leafName );
	
	if ( ! Host_IO::Exists( clientPath.c_str() ) )
	{
		// logical path exists
		// No need to extract extension as clipname is given without extension
		this->rootPath = tempPath;
		tempPath += kDirChar;
		tempPath += "PROAV";
		
		XMP_Assert ( Host_IO::GetChildMode( tempPath.c_str(), "MEDIAPRO.XML" ) != Host_IO::kFMode_IsFile );

		tempPath += kDirChar;
		tempPath += "CLPR";
		tempPath += kDirChar + leafName;
	}
	else
	{
		// Real Absolute Path exists
		XIO::SplitFileExtension ( &leafName, &ignored );

		XIO::SplitLeafName ( &tempPath, &parentName );
		XIO::SplitLeafName ( &tempPath, &groupName );

		std::string proav;
		XIO::SplitLeafName ( &tempPath, &proav );
		XMP_Assert ( proav == "PROAV" );

		this->rootPath = tempPath;


		XMP_Assert ( groupName == "CLPR" );
		
		// Real Path may be ..PROAV//CLPR//clipName//clipname.MXF
		// So XMP check for ..PROAV//CLPR//clipName//clipNameM01.xml
		// XNP sidecar file will be ..PROAV//CLPR//clipName//clipNameM01.XMP
		size_t length = leafName.length();

		XMP_Assert ( IsDigit( leafName.at( length - 2 ) ) && IsDigit( leafName.at( length - 1 ) ) );
		// Last 3rd character of file will inform us about its type
		const char fileType = leafName.at( length - 3 );

		// A = Audio, C = ClipInfo, I = Picture Pointer, M = Non-Realtime, R = Realtime, S = Sub (Proxy), V = Video
		XMP_Assert ( fileType == 'A' || fileType == 'C' || fileType == 'I' || fileType == 'M' || fileType == 'R' || fileType == 'S' || fileType == 'V' );

		leafName.erase ( leafName.begin() + length - 3, leafName.end() );
		
		tempPath += kDirChar + proav;
		tempPath += kDirChar + groupName;
		tempPath += kDirChar + parentName;
	}

	this->clipName = leafName;


	tempPath += kDirChar;
	tempPath += leafName;

	// NRT file Check as already cover in checkformat
	if ( ! MakeClipFilePath( &this->mNRTFilePath, "M01.XML", true ) )
	{
		XMP_Error error( kXMPErr_FilePathNotAFile, "Clip NRT XML file must be exist" );
		NotifyClient( &this->parent->errorCallback, kXMPErrSev_FileFatal, error );
	}

	// Setting correct sidecar path covering both .XMP and .xmp
	if ( ! ( MakeClipFilePath( &this->sidecarPath, "M01.XMP", true ) || MakeClipFilePath( &this->sidecarPath, "M01.xmp", true ) ) )
		this->sidecarPath = tempPath + "M01.XMP";

}	// XDCAMSAM_MetaHandler::SetPathVariables

// =================================================================================================
// XDCAMSAM_MetaHandler::FillAssociatedResources
// ====================================
void XDCAMSAM_MetaHandler::FillAssociatedResources(  std::vector<std::string> * resourceList  )
{

	std::string proavPath = rootPath + kDirChar + "PROAV" + kDirChar;
	std::string filePath;
	
	//Add RootPath
	filePath = rootPath + kDirChar;
	PackageFormat_Support::AddResourceIfExists( resourceList, filePath );

	// Get the files present directly inside PROAV folder.
	filePath = proavPath + "INDEX.XML";
	PackageFormat_Support::AddResourceIfExists(resourceList, filePath);
	filePath = proavPath + "INDEX.BUP";
	PackageFormat_Support::AddResourceIfExists(resourceList, filePath);

	filePath = proavPath + "DISCINFO.XML";
	PackageFormat_Support::AddResourceIfExists(resourceList, filePath);
	filePath = proavPath + "DISCINFO.BUP";
	PackageFormat_Support::AddResourceIfExists(resourceList, filePath);

	filePath = proavPath + "DISCMETA.XML";
	PackageFormat_Support::AddResourceIfExists(resourceList, filePath);

	// Covering files in clipname folder in CLPR folder
	XMP_VarString clipPath = proavPath + "CLPR" + kDirChar + clipName + kDirChar;
	XMP_VarString regExp;
	XMP_StringVector regExpVec;

	// ClipInfo file
	regExp = "^" + clipName + "C\\d\\d.SMI$";
	regExpVec.push_back ( regExp );
	// Non-Real time metadata file
	regExp = "^" + clipName + "M\\d\\d.XML$";
	regExpVec.push_back ( regExp );
	// Video file
	regExp = "^" + clipName + "V\\d\\d.MXF$";
	regExpVec.push_back ( regExp );
	// Audio File
	regExp = "^" + clipName + "A\\d\\d.MXF$";
	regExpVec.push_back ( regExp );
	// Real time metadata file
	regExp = "^" + clipName + "R\\d\\d.BIM$";
	regExpVec.push_back ( regExp );
	// Picture pointer file
	regExp = "^" + clipName + "I\\d\\d.PPN$";
	regExpVec.push_back ( regExp );
	// Sub(Proxy) files
	regExp = "^" + clipName + "S\\d\\d.MXF$";
	regExpVec.push_back ( regExp );
	IOUtils::GetMatchingChildren ( *resourceList, clipPath, regExpVec, false, true, true );
	
	// Adding sidecar file if exixts
	PackageFormat_Support::AddResourceIfExists(resourceList, this->sidecarPath);
	
	// Add the Edit lists that refer this clip
	std::vector<std::string> editInfoList;
	if( GetEditInfoFiles ( editInfoList ) )
	{
		size_t noOfEditInfoFiles = editInfoList.size() ;
		for( size_t count = 0; count < noOfEditInfoFiles; count++ )
		{
			PackageFormat_Support::AddResourceIfExists(resourceList, editInfoList[count]);
			std::string editNRTFile = editInfoList[count].c_str() ;
			size_t filenamelen = editInfoList[count].length() ;
			editNRTFile[ filenamelen - 7 ] = 'M';
			editNRTFile[ filenamelen - 3 ] = 'X';
			editNRTFile[ filenamelen - 2 ] = 'M';
			editNRTFile[ filenamelen - 1 ] = 'L';
			PackageFormat_Support::AddResourceIfExists(resourceList, editNRTFile );
		}
	}
}	// XDCAMSAM_MetaHandler::FillAssociatedResources

// =================================================================================================
// XDCAMSAM_MetaHandler::MakeClipFilePath
// ====================================
bool XDCAMSAM_MetaHandler::MakeClipFilePath ( std::string * path, XMP_StringPtr suffix, bool checkFile /* = false */ )
{
	*path = this->rootPath;
	*path += kDirChar; 
	*path += "PROAV";
	*path += kDirChar; 

	*path += "CLPR";	// ! Yes, mixed case.
	
	*path += kDirChar;
	*path += this->clipName;
	*path += kDirChar;
	*path += this->clipName;
	*path += suffix;
	
	if ( ! checkFile ) return true;
	return Host_IO::Exists ( path->c_str() );

}	// XDCAMSAM_MetaHandler::MakeClipFilePath

// =================================================================================================
// XDCAMSAM_MetaHandler::GetEditInfoFiles
// ====================================
bool XDCAMSAM_MetaHandler::GetEditInfoFiles ( std::vector<std::string> &editInfoList ) 
{
	std::string clipUmid;
	bool found = false;

	if( GetClipUmid ( clipUmid ) )
	{
		std::string editFolderPath = this->rootPath + kDirChar + "PROAV" + kDirChar + "EDTR"  + kDirChar ;
		if ( Host_IO::Exists( editFolderPath.c_str() ) && 
			Host_IO::GetFileMode( editFolderPath.c_str() ) == Host_IO::kFMode_IsFolder 
			)
		{
			Host_IO::AutoFolder edtrFolder, editFolder;
			std::string edtrChildName, edlistChild;

			edtrFolder.folder = Host_IO::OpenFolder ( editFolderPath.c_str() );
			while (  Host_IO::GetNextChild ( edtrFolder.folder, &edtrChildName ) ) {
				size_t childLen = edtrChildName.size();
				std::string editListFolderPath = editFolderPath + edtrChildName + kDirChar ;
				if ( ! ( childLen == 5 &&
					edtrChildName[0] == 'E' &&
					IsDigit( edtrChildName[1] ) &&
					IsDigit( edtrChildName[2] ) &&
					IsDigit( edtrChildName[3] ) &&
					IsDigit( edtrChildName[4] ) &&
					Host_IO::GetFileMode( editListFolderPath.c_str() ) == Host_IO::kFMode_IsFolder
					) ) continue;
				
				editFolder.folder = Host_IO::OpenFolder ( editListFolderPath.c_str() );
				while (  Host_IO::GetNextChild ( editFolder.folder, &edlistChild ) ) {
					size_t filenamelen = edlistChild.size();
					std::string editListFilePath = editListFolderPath + edlistChild ;
					if ( ! ( filenamelen == 12 &&
						edlistChild.compare ( filenamelen - 4, 4 , ".SMI" ) == 0 &&
						edlistChild.compare ( 0, edtrChildName.size(), edtrChildName ) == 0 &&
						Host_IO::GetFileMode( editListFilePath.c_str() ) == Host_IO::kFMode_IsFile
					) ) continue;
					if( RefersClipUmid ( clipUmid , editListFilePath.c_str() )  )
					{
						found = true ;
						editInfoList.push_back( editListFilePath );
					}
				}
			}
		}
	}
	return found;
}	// XDCAMSAM_MetaHandler::GetEditInfoFiles

// =================================================================================================
// XDCAMSAM_MetaHandler::GetClipUmid
// ==============================
bool XDCAMSAM_MetaHandler::GetClipUmid ( std::string &clipUmid ) 
{
	std::string clipInfoPath;
	ExpatAdapter* clipInfoExpat = 0 ;
	bool umidFound = false;
	XMP_StringPtr nameSpace = 0;
	try {
		this->MakeClipFilePath ( &clipInfoPath, "C01.SMI" ) ;
		readXMLFile( clipInfoPath.c_str(), clipInfoExpat );
		if ( clipInfoExpat != 0 )
		{	
			XML_Node & xmlTree = clipInfoExpat->tree;
			XML_NodePtr rootElem = 0;

			for ( size_t i = 0, limit = xmlTree.content.size(); i < limit; ++i ) {
				if ( xmlTree.content[i]->kind == kElemNode ) {
					rootElem = xmlTree.content[i];
				}
			}
			if ( rootElem != 0 )
			{
				XMP_StringPtr rootLocalName = rootElem->name.c_str() + rootElem->nsPrefixLen;
				
				if ( XMP_LitMatch ( rootLocalName, "smil" ) ) 
				{
					XMP_StringPtr umidValue = rootElem->GetAttrValue ( "umid" );
					if ( umidValue != 0 ) {
						clipUmid = umidValue;
						umidFound = true;
					}
				}
			}
		}
		if( ! umidFound )
		{	//try to get the umid from the NRT metadata
			delete ( clipInfoExpat ) ; clipInfoExpat = 0;
			this->MakeClipFilePath ( &clipInfoPath, "M01.XML" ) ;
			readXMLFile( clipInfoPath.c_str(), clipInfoExpat ) ;		
			if ( clipInfoExpat != 0 )
			{	
				XML_Node & xmlTree = clipInfoExpat->tree;
				XML_NodePtr rootElem = 0;
				for ( size_t i = 0, limit = xmlTree.content.size(); i < limit; ++i ) {
					if ( xmlTree.content[i]->kind == kElemNode ) {
						rootElem = xmlTree.content[i];
					}
				}
				if ( rootElem != 0 )
				{
					XMP_StringPtr rootLocalName = rootElem->name.c_str() + rootElem->nsPrefixLen;
					
					if ( XMP_LitMatch ( rootLocalName, "NonRealTimeMeta" ) ) 
					{	
						nameSpace = rootElem->ns.c_str() ;
						XML_NodePtr targetProp = rootElem->GetNamedElement ( nameSpace, "TargetMaterial" );
						if ( (targetProp != 0) && targetProp->IsEmptyLeafNode() ) {
							XMP_StringPtr umidValue = targetProp->GetAttrValue ( "umidRef" );
							if ( umidValue != 0 ) {
								clipUmid = umidValue;
								umidFound = true;
							}
						}
					}
				}
			}
		}
	} catch ( ... ) {
	}
	delete ( clipInfoExpat ) ;
	return umidFound;
}	// XDCAMSAM_MetaHandler::GetClipUmid

// =================================================================================================

