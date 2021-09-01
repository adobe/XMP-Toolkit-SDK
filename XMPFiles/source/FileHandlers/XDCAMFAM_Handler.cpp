// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

// =================================================================================================
//
// This handler will handle FAM/FTP variant of XDCAM.
// More information could be found in XDCAM_Handler.cpp
//
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! XMP_Environment.h must be the first included header.

#include "public/include/XMP_Const.h"
#include "public/include/XMP_IO.hpp"

#include "XMPFiles/source/XMPFiles_Impl.hpp"
#include "source/XMPFiles_IO.hpp"
#include "source/XIO.hpp"
#include "source/IOUtils.hpp"

#include "XMPFiles/source/FileHandlers/XDCAMFAM_Handler.hpp"
#include "XMPFiles/source/FormatSupport/XDCAM_Support.hpp"
#include "XMPFiles/source/FormatSupport/PackageFormat_Support.hpp"

bool XDCAMFAM_CheckFormat ( XMP_FileFormat format,
						 const std::string & rootPath,
						 const std::string & groupName,
						 const std::string & parentName,
						 const std::string & leafName,
						 XMPFiles * parent )
{
	/* isXDStyle   = true Means SxS Memory or XDStyle
	             , = false Means Professional Disk */ 
	bool isXDStyle = false;
	if ( (format != kXMP_XDCAM_FAMFile) && (format != kXMP_UnknownFile) ) return false;
	if ( groupName.empty() != parentName.empty() ) return false;

	if ( groupName.empty() && ( Host_IO::GetChildMode ( rootPath.c_str(), "PROAV" ) == Host_IO::kFMode_IsFolder ) ) return false;

	std::string tempPath = rootPath;

	if ( !parentName.empty() )
	{
		// Real Absolute Path exists
		if ( ! ( parentName == "CLIP" ||  parentName == "SUB" ||  parentName == "LOCAL"  ) )
			return false;
		tempPath += kDirChar + groupName;
	}

	// Some basic Checks
	if ( Host_IO::GetChildMode ( tempPath.c_str(), "DISCMETA.XML" ) != Host_IO::kFMode_IsFile ) return false;
	if ( Host_IO::GetChildMode( tempPath.c_str(), "MEDIAPRO.XML" ) != Host_IO::kFMode_IsFile ) return false;
	if ( ( Host_IO::GetChildMode( tempPath.c_str(), "Take" ) == Host_IO::kFMode_IsFolder ) || ( Host_IO::GetChildMode( tempPath.c_str(), "Local" ) == Host_IO::kFMode_IsFolder ) )
		isXDStyle = true;
	
	// XDStyle can't have INDEX.XML
	if ( isXDStyle && ( Host_IO::GetChildMode( tempPath.c_str(), "INDEX.XML" ) == Host_IO::kFMode_IsFile ) )
			return false;
	// XDStyle can't have ALIAS.XML
	if( isXDStyle && ( Host_IO::GetChildMode( tempPath.c_str(), "ALIAS.XML" ) == Host_IO::kFMode_IsFile ) )
		return false;
	// Non-XDStyle can't have CUEUP.XML file
	if( ( !isXDStyle ) && ( Host_IO::GetChildMode( tempPath.c_str(), "CUEUP.XML" ) == Host_IO::kFMode_IsFile ) )
		return false;

	// We will get metadata from NRT file inside Clip folder only
	tempPath += kDirChar;
	tempPath += "Clip";
	tempPath += kDirChar;

	std::string clipName = leafName;
	size_t length = clipName.length();
	
	// Proxy file support
	if ( ( parentName == "SUB" ) )
	{
		if( clipName.at( length - 3 ) != 'S' || ( ! IsDigit( clipName.at( length - 2 ) ) ) || ( ! IsDigit( clipName.at( length - 1 ) ) ) )
			return false;
		clipName.erase( clipName.begin() + length - 3, clipName.end() );
	}

	tempPath += clipName;
	
	// .MXF file Existence with case sensitive is the new check inserted
	std::string mxfPath = tempPath + ".MXF";
	if ( Host_IO::GetFileMode ( mxfPath.c_str() ) != Host_IO::kFMode_IsFile )
	{
		mxfPath = tempPath + ".mxf";
		if ( Host_IO::GetFileMode ( mxfPath.c_str() ) != Host_IO::kFMode_IsFile )
			return false;
	}

	tempPath += "M01.XML";
	if ( Host_IO::GetFileMode ( tempPath.c_str() ) != Host_IO::kFMode_IsFile )
		return false;
	return true;

}	// XDCAMFAM_CheckFormat

XMPFileHandler * XDCAMFAM_MetaHandlerCTor ( XMPFiles * parent )
{
	return new XDCAMFAM_MetaHandler ( parent );

}	// XDCAM_MetaHandlerCTor

// =================================================================================================
// XDCAMFAM_MetaHandler::XDCAMFAM_MetaHandler
// ====================================
XDCAMFAM_MetaHandler::XDCAMFAM_MetaHandler ( XMPFiles * _parent ) : XDCAM_MetaHandler(_parent), isXDStyle( false )
{
	this->handlerFlags = kXDCAMFAM_HandlerFlags;
	// Setting the various path variables
	this->SetPathVariables ( this->parent->GetFilePath() );
}	// XDCAMFAM_MetaHandler::XDCAMFAM_MetaHandler


// =================================================================================================
// XDCAMFAM_MetaHandler::SetPathVariables
// ====================================
void XDCAMFAM_MetaHandler::SetPathVariables ( const std::string & clientPath )
{
	// No need to check for existing or non existing as would have been done at check file format if ForceGivenHandler flag is not provided
	std::string tempPath = clientPath;
	std::string parentName, GroupName;
	std::string ignored;

	XIO::SplitLeafName ( &tempPath, &this->clipName );

	this->rootPath = tempPath;

	if ( ! Host_IO::Exists( clientPath.c_str() ) )
	{
		// Logical Path exists
		// No need to extract extension as clipname is given without extension
		if ( ( Host_IO::GetChildMode( tempPath.c_str(), "INDEX.XML" ) != Host_IO::kFMode_IsFile ) )
			this->isXDStyle = true;
		tempPath += kDirChar;
		tempPath += "Clip";
		XMP_Assert( Host_IO::GetFileMode( tempPath.c_str() ) == Host_IO::kFMode_IsFolder );
	}
	else
	{
		// Real Absolute Path exists
		XIO::SplitFileExtension ( &this->clipName, &ignored );
		XIO::SplitLeafName ( &tempPath, &parentName );
		if ( ( Host_IO::GetChildMode( tempPath.c_str(), "INDEX.XML" ) != Host_IO::kFMode_IsFile ) )
			this->isXDStyle = true;
		this->rootPath = tempPath;
		
		size_t length = this->clipName.length();
		
		// Proxy file support
		if ( parentName == "Sub" )
		{
			XMP_Assert( IsDigit( clipName.at( length - 2 ) ) && IsDigit( clipName.at( length - 1 ) ) );
			XMP_Assert( this->clipName.at( length - 3 ) == 'S' );
			this->clipName.erase( this->clipName.begin() + length - 3, clipName.end() );
			tempPath += kDirChar ;
			tempPath += "Clip";
		}
		else
			tempPath += kDirChar + parentName;
	}

	// Checks for Clip folder in XDCAM
	XMP_Assert ( Host_IO::GetChildMode ( rootPath.c_str(), "Clip" ) == Host_IO::kFMode_IsFolder );

	tempPath += kDirChar;
	tempPath += this->clipName;
	std::string mxfPath;
	
	// Case sensitive Extension support to check for clipname.MXF or clipname.mxf as already cover in Checkformat
	if ( !( MakeClipFilePath( &mxfPath, ".MXF", true ) || MakeClipFilePath( &mxfPath, ".mxf", true ) ) )
	{
		XMP_Error error( kXMPErr_FilePathNotAFile, "Clip MXF file must be exist" );
		NotifyClient( &this->parent->errorCallback, kXMPErrSev_FileFatal, error );
	}
	
	// NRT file Check as already cover in checkformat
	if ( ! MakeClipFilePath ( &this->mNRTFilePath, "M01.XML", true ) )
	{
		XMP_Error error( kXMPErr_FilePathNotAFile, "Clip NRT XML file must be exist" );
		NotifyClient( &this->parent->errorCallback, kXMPErrSev_FileFatal, error );
	}

	// Setting correct sidecar path
	if ( this->isXDStyle || (Host_IO::GetChildMode ( rootPath.c_str(), "UserData" ) == Host_IO::kFMode_IsFolder ) )
	{
		if ( ! ( MakeClipFilePath( &this->sidecarPath, ".xmp", true ) || MakeClipFilePath( &this->sidecarPath, ".XMP", true ) ) )
			this->sidecarPath = mxfPath + ".xmp";
	}
	else
	{
		if ( ! ( MakeClipFilePath( &this->sidecarPath, "M01.XMP", true ) || MakeClipFilePath( &this->sidecarPath, "M01.xmp", true ) ) )
			this->sidecarPath = tempPath + "M01.XMP";
	}
	
}	// XDCAMFAM_MetaHandler::SetPathVariables

// =================================================================================================
// XDCAMFAM_MetaHandler::FillAssociatedResources
// ====================================
void XDCAMFAM_MetaHandler::FillAssociatedResources(  std::vector<std::string> * resourceList  )
{
	//Add RootPath
	std::string filePath = rootPath + kDirChar;
	PackageFormat_Support::AddResourceIfExists( resourceList, filePath );

	// Get the files present directly inside root folder.
	filePath = rootPath + kDirChar + "ALIAS.XML";
	PackageFormat_Support::AddResourceIfExists(resourceList, filePath);

	// INDEX.XML doesn't exist for XDStyle
	if( ! this->isXDStyle )
	{
		filePath = rootPath + kDirChar + "INDEX.XML";
		PackageFormat_Support::AddResourceIfExists(resourceList, filePath);
	}

	filePath = rootPath + kDirChar + "DISCMETA.XML";
	PackageFormat_Support::AddResourceIfExists(resourceList, filePath);

	filePath = rootPath + kDirChar + "MEDIAPRO.XML";
	PackageFormat_Support::AddResourceIfExists(resourceList, filePath);
	filePath = rootPath + kDirChar + "MEDIAPRO.BUP";
	PackageFormat_Support::AddResourceIfExists(resourceList, filePath);

	// CUEUP.XML don't exist for Professional Disk XDCAM 
	if( this->isXDStyle )
	{
		filePath = rootPath + kDirChar + "CUEUP.XML";
		PackageFormat_Support::AddResourceIfExists(resourceList, filePath);
		filePath = rootPath + kDirChar + "CUEUP.BUP";
		PackageFormat_Support::AddResourceIfExists(resourceList, filePath);
	}

	// Add the UserData folder
	filePath = rootPath + kDirChar + "UserData" + kDirChar;
	PackageFormat_Support::AddResourceIfExists(resourceList, filePath);


	// Get the files present inside clip folder.
	XMP_VarString clipPath = rootPath + kDirChar + "Clip" + kDirChar ;
	size_t oldCount = resourceList->size();

	XMP_VarString regExp;
	XMP_StringVector regExpVec;
	
	regExp = "^" + clipName + ".MXF$";
	regExpVec.push_back ( regExp );
	regExp = "^" + clipName + "M\\d\\d.XML$";
	regExpVec.push_back ( regExp );
	if ( this->isXDStyle )
	{
		regExp = "^" + clipName + "R\\d\\d.BIM$";
		regExpVec.push_back ( regExp );
	}
	else
	{
		regExp = "^" + clipName + "M\\d\\d.KLV$";
		regExpVec.push_back ( regExp );
	}
	IOUtils::GetMatchingChildren ( *resourceList, clipPath, regExpVec, false, true, true ); 
	PackageFormat_Support::AddResourceIfExists( resourceList, this->sidecarPath);
	if ( resourceList->size() <= oldCount )
	{
		PackageFormat_Support::AddResourceIfExists(resourceList, clipPath);
	}

	//Get the files Under Sub folder
	clipPath = rootPath + kDirChar + "Sub" + kDirChar ;
	regExpVec.clear();
	regExp = "^" + clipName + "S\\d\\d.MXF$";
	regExpVec.push_back ( regExp );
	oldCount = resourceList->size();
	IOUtils::GetMatchingChildren ( *resourceList, clipPath, regExpVec, false, true, true );
	// Add Sub folder if no file inside this, was added.
	if ( resourceList->size() <= oldCount )
	{
		PackageFormat_Support::AddResourceIfExists(resourceList, clipPath);
	}

	// Get the files Under Local folder if it is XDStyle
	if ( isXDStyle )
	{
		clipPath = rootPath + kDirChar + "Local" + kDirChar ;
		regExpVec.clear();
		// ClipInfo file
		regExp = "^" + clipName + "C\\d\\d.SMI$";
		regExpVec.push_back ( regExp );
		// Picture pointer file
		regExp = "^" + clipName + "I\\d\\d.PPN$";
		regExpVec.push_back ( regExp );
		oldCount = resourceList->size();
		IOUtils::GetMatchingChildren ( *resourceList, clipPath, regExpVec, false, true, true );
		// Add Local folder if no file inside this, was added.
		if ( resourceList->size() <= oldCount )
		{
			PackageFormat_Support::AddResourceIfExists(resourceList, clipPath);
		}
	}

	// Add the Edit lists associated to this clip
	XMP_StringVector editInfoList;
	bool atLeastOneFileAdded = false;
	clipPath = rootPath + kDirChar + "Edit" + kDirChar ;
	if ( GetInfoFiles ( editInfoList , clipPath ) )
	{
		size_t noOfEditInfoFiles = editInfoList.size() ;
		for( size_t count = 0; count < noOfEditInfoFiles; count++ )
		{
			atLeastOneFileAdded = PackageFormat_Support::AddResourceIfExists(resourceList, editInfoList[count]) ? true : atLeastOneFileAdded;

			XMP_VarString editNRTFile = editInfoList[count] ;
			size_t filenamelen = editNRTFile.length() ;
			if ( editNRTFile[ filenamelen - 7 ] == 'E' 
				&& IsDigit( editNRTFile[ filenamelen - 6 ] ) 
				&& IsDigit( editNRTFile[ filenamelen - 5 ] ) )
			{
				editNRTFile.erase( editNRTFile.begin() + filenamelen - 7, editNRTFile.end() ) ;
			}
			else
			{
				editNRTFile.erase( editNRTFile.begin() + filenamelen - 4, editNRTFile.end() ) ;
			}

			XMP_VarString fileName;
			size_t pos = editNRTFile.find_last_of ( kDirChar );
			fileName = editNRTFile.substr ( pos + 1 );
			XMP_VarString regExpStr = "^" + fileName + "M\\d\\d.XML$";
			oldCount = resourceList->size();
			IOUtils::GetMatchingChildren ( *resourceList, clipPath, regExpStr, false, true, true );
			atLeastOneFileAdded = resourceList->size() > oldCount;

		}
	}
	// Add Edit folder if no file inside this, was added.
	if ( !atLeastOneFileAdded )
	{
		PackageFormat_Support::AddResourceIfExists(resourceList, clipPath);
	}

	atLeastOneFileAdded = false;

	// Add the Takes associated to this clip,
	// Take folder exists only for XDStyle
	if( this->isXDStyle )
	{
		XMP_StringVector takeList;
		clipPath = rootPath + kDirChar + "Take" + kDirChar ;
		if( GetInfoFiles ( takeList  , clipPath ) )
		{
			size_t noOfTakes = takeList.size() ;
			for( size_t count = 0; count < noOfTakes; count++ )
			{
				atLeastOneFileAdded = PackageFormat_Support::AddResourceIfExists(resourceList, takeList[count]) ? true : atLeastOneFileAdded;
				XMP_VarString takeNRTFile = takeList[count] ;
				size_t filenamelen = takeList[count].length() ;
				if ( takeNRTFile[ filenamelen - 7 ] == 'U' 
					&& IsDigit( takeNRTFile[ filenamelen - 6 ] ) 
					&& IsDigit( takeNRTFile[ filenamelen - 5 ] ) )
				{
					takeNRTFile.erase( takeNRTFile.begin() + filenamelen - 7, takeNRTFile.end() ) ;
				}
				else
				{
					takeNRTFile.erase( takeNRTFile.begin() + filenamelen - 4, takeNRTFile.end() ) ;
				}

				XMP_VarString fileName;
				size_t pos = takeNRTFile.find_last_of ( kDirChar );
				fileName = takeNRTFile.substr ( pos + 1 );
				XMP_VarString regExpStr = "^" + fileName + "M\\d\\d.XML$";
				oldCount = resourceList->size();
				IOUtils::GetMatchingChildren ( *resourceList, clipPath, regExpStr, false, true, true );
				atLeastOneFileAdded = resourceList->size() > oldCount;
			}
		}
		// Add Take folder if no file inside this, was added.
		if(!atLeastOneFileAdded)
		{
			filePath = rootPath + kDirChar + "Take" + kDirChar;
			PackageFormat_Support::AddResourceIfExists(resourceList, filePath);
		}
	}
	
	// Add the Planning Metadata Files associated to this clip
	// Planning Metadata exist for both SxS and Professional Disk
	XMP_StringVector planList;
	clipPath = rootPath + kDirChar + "General" + kDirChar + "Sony" + kDirChar+ "Planning" + kDirChar;
	if( GetPlanningFiles ( planList  , clipPath ) )
	{
		size_t noOfPlans = planList.size() ;
		for( size_t count = 0; count < noOfPlans; count++ )
		{
			resourceList->push_back( planList[count] );
		}
	}
}	//  XDCAMFAM_MetaHandler::FillAssociatedResources

// =================================================================================================
// XDCAMFAM_MetaHandler::MakeClipFilePath
// ====================================
bool XDCAMFAM_MetaHandler::MakeClipFilePath ( std::string * path, XMP_StringPtr suffix, bool checkFile /* = false */ )
{

	*path = this->rootPath;
	*path += kDirChar; 

	*path += "Clip";	// ! Yes, mixed case.
	
	*path += kDirChar;
	*path += this->clipName;
	*path += suffix;
	
	if ( ! checkFile ) return true;
	return Host_IO::Exists ( path->c_str() );

}	// XDCAMFAM_MetaHandler::MakeClipFilePath

// =================================================================================================
// XDCAMFAM_MetaHandler::MakeLocalFilePath
// ====================================
bool XDCAMFAM_MetaHandler::MakeLocalFilePath ( std::string * path, XMP_Uns8 fileType, bool checkFile /* = false */ )
{

	*path = this->rootPath;
	*path += kDirChar; 

	*path += "Local";	// ! Yes, mixed case.
	
	*path += kDirChar;
	*path += this->clipName;

	if( fileType == k_LocalPPNFile )
		*path += "I01.PPN";
	else if ( fileType == k_LocalClipInfoFile )
		*path += "S01.SMI";
	else
		return false;

	if ( ! checkFile ) return true;
	return Host_IO::Exists ( path->c_str() );

}	// XDCAMFAM_MetaHandler::MakeLocalFilePath


// =================================================================================================
// XDCAMFAM_MetaHandler::GetMediaProMetadata
// ====================================
bool XDCAMFAM_MetaHandler::GetMediaProMetadata ( SXMPMeta * xmpObjPtr,
											  const std::string& clipUMID,
											  bool digestFound )
{
	// Build a directory string to the MEDIAPRO file.
	std::string mediaproPath;
	MakeMediaproPath ( &mediaproPath );
	return XDCAM_Support::GetMediaProLegacyMetadata ( xmpObjPtr, clipUMID, mediaproPath, digestFound );
}	// XDCAMFAM_MetaHandler::GetMediaProMetadata

// =================================================================================================
// XDCAMFAM_MetaHandler::GetPlanningFiles
// ====================================
bool XDCAMFAM_MetaHandler::GetPlanningFiles ( std::vector<std::string> &planInfoList, std::string pathToFolder) 
{
	std::string clipUmid;
	bool found = false;

	if( GetClipUmid ( clipUmid ) )
	{
		if ( Host_IO::Exists( pathToFolder.c_str() ) && 
			Host_IO::GetFileMode( pathToFolder.c_str() ) == Host_IO::kFMode_IsFolder 
			)
		{
			Host_IO::AutoFolder planFolder;
			std::string  listChild;

			planFolder.folder = Host_IO::OpenFolder ( pathToFolder.c_str() );
			while (  Host_IO::GetNextChild ( planFolder.folder, &listChild ) ) {
				size_t filenamelen = listChild.size();
				std::string listFilePath = pathToFolder + listChild ;
				if ( ! ( filenamelen > 4 && 
					( listChild.compare ( filenamelen - 4, 4 , ".XML" ) == 0 
					|| 
					listChild.compare ( filenamelen - 4, 4 , ".xml" ) == 0
					)
					&&
					Host_IO::GetFileMode( listFilePath.c_str() ) == Host_IO::kFMode_IsFile
				) ) continue;
				if( IsClipsPlanning ( clipUmid , listFilePath.c_str() )  )
				{
					found = true ;
					planInfoList.push_back( listFilePath );
				}
			}
		}
	}
	return found;
}	// XDCAMFAM_MetaHandler::GetPlanningFiles

// =================================================================================================
// XDCAMFAM_MetaHandler::IsClipsPlanning
// ====================================
bool XDCAMFAM_MetaHandler::IsClipsPlanning ( std::string clipUmid , XMP_StringPtr planPath ) 
{
	ExpatAdapter* planniingExpat = 0 ;
	XMP_StringPtr nameSpace = 0 ;
	try {
		readXMLFile( planPath, planniingExpat );
		if ( planniingExpat != 0 )
		{	
			XML_Node & xmlTree = planniingExpat->tree;
			XML_NodePtr rootElem = 0;

			for ( size_t i = 0, limit = xmlTree.content.size(); i < limit; ++i ) {
				if ( xmlTree.content[i]->kind == kElemNode ) {
					rootElem = xmlTree.content[i];
				}
			}
			if ( rootElem != 0 )
			{
				XMP_StringPtr rootLocalName = rootElem->name.c_str() + rootElem->nsPrefixLen;
				
				if ( XMP_LitMatch ( rootLocalName, "PlanningMetadata" ) ) 
				{
					nameSpace = rootElem->ns.c_str() ;
					size_t noOfMaterialGroups = rootElem->CountNamedElements ( nameSpace, "MaterialGroup" ) ;
					while( noOfMaterialGroups-- )
					{
						XML_NodePtr mgNode = rootElem->GetNamedElement(  nameSpace, "MaterialGroup" );
						size_t noOfMaterialElements = mgNode->CountNamedElements ( nameSpace, "Material" ) ;
						while( noOfMaterialElements-- )
						{
							XML_NodePtr materialNode = mgNode->GetNamedElement(  nameSpace, "Material" );
							XMP_StringPtr materialType = materialNode->GetAttrValue ( "type" );
							if ( materialType  && XMP_LitMatch( materialType , "clip" ) ) 
							{
								XMP_StringPtr umidValue = materialNode->GetAttrValue ( "umidRef" );
								if ( umidValue != 0 &&  XMP_LitMatch( umidValue , clipUmid.c_str()  ) )
								{
									delete ( planniingExpat ) ;
									return true;
								}
							}
							
						}
					}
				}
			}
		}
		
	} catch ( ... ) {
	}
	delete ( planniingExpat ) ;
	return false;
}	// XDCAMFAM_MetaHandler::IsClipsPlanning

// =================================================================================================
// XDCAMFAM_MetaHandler::GetInfoFiles
// ====================================
bool XDCAMFAM_MetaHandler::GetInfoFiles ( std::vector<std::string> &infoList, std::string pathToFolder) 
{
	std::string clipUmid;
	bool found = false;

	if( GetClipUmid ( clipUmid ) )
	{
		if ( Host_IO::Exists( pathToFolder.c_str() ) && 
			Host_IO::GetFileMode( pathToFolder.c_str() ) == Host_IO::kFMode_IsFolder 
			)
		{
			Host_IO::AutoFolder infoFolder;
			std::string  listChild;

			infoFolder.folder = Host_IO::OpenFolder ( pathToFolder.c_str() );
			while (  Host_IO::GetNextChild ( infoFolder.folder, &listChild ) ) {
				size_t filenamelen = listChild.size();
				std::string listFilePath = pathToFolder + listChild ;
				if ( ! ( filenamelen > 7 && 
					listChild.compare ( filenamelen - 4, 4 , ".SMI" ) == 0 &&
					Host_IO::GetFileMode( listFilePath.c_str() ) == Host_IO::kFMode_IsFile
				) ) continue;
				if( RefersClipUmid ( clipUmid , listFilePath.c_str() )  )
				{
					found = true ;
					infoList.push_back( listFilePath );
				}
			}
		}
	}
	return found;
}	// XDCAMFAM_MetaHandler::GetInfoFiles

// =================================================================================================
// XDCAMFAM_MetaHandler::GetClipUmid
// ==============================
bool XDCAMFAM_MetaHandler::GetClipUmid ( std::string &clipUmid ) 
{
	std::string clipInfoPath;
	ExpatAdapter* clipInfoExpat = 0 ;
	bool umidFound = false;
	XMP_StringPtr nameSpace = 0;
	try {
		if ( this->MakeLocalFilePath ( &clipInfoPath, k_LocalClipInfoFile, true ) ) 
		{
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
}	// XDCAMFAM_MetaHandler::GetClipUmid

// =================================================================================================
