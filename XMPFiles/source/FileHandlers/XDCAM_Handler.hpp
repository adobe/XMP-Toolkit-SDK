#ifndef __XDCAM_Handler_hpp__
#define __XDCAM_Handler_hpp__	1

// =================================================================================================
// Copyright Adobe
// Copyright 2007 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! This must be the first include.

#include "XMPFiles/source/XMPFiles_Impl.hpp"

#include "source/ExpatAdapter.hpp"

// =================================================================================================
/// \file XDCAM_Handler.hpp
/// \brief Folder format handler for XDCAM.
///
/// This header ...
///
// =================================================================================================

inline bool IsDigit( char c )
{
	return c >= '0' && c <= '9';
}

class XDCAM_MetaHandler : public XMPFileHandler
{
public:

	bool GetFileModDate ( XMP_DateTime * modDate );

	virtual void FillAssociatedResources ( std::vector<std::string> * resourceList ) {};
	bool IsMetadataWritable ( ) ;

	void CacheFileData();
	void ProcessXMP();

	void UpdateFile ( bool doSafeUpdate );
    void WriteTempFile ( XMP_IO* tempRef );

	XMP_OptionBits GetSerializeOptions()	// *** These should be standard for standalone XMP files.
		{ return (kXMP_UseCompactFormat | kXMP_OmitPacketWrapper); };

	XDCAM_MetaHandler ( XMPFiles * _parent );
	virtual ~XDCAM_MetaHandler();

protected:

	XDCAM_MetaHandler() : expat(0), clipMetadata(0) {};	// Hidden on purpose.

	virtual bool MakeClipFilePath ( std::string * path, XMP_StringPtr suffix, bool checkFile = false ) { return false; }
	virtual void SetPathVariables ( const std::string & clientPath )  { }
	virtual bool GetMediaProMetadata ( SXMPMeta * xmpObjPtr, const std::string& clipUMID, bool digestFound ) {
		return false;
	}
	bool MakeMediaproPath ( std::string * path, bool checkFile = false );
	virtual bool GetClipUmid ( std::string &clipUmid ) { return false; }
	void readXMLFile( XMP_StringPtr filePath,ExpatAdapter* &expat );
	bool RefersClipUmid ( std::string clipUmid , XMP_StringPtr editInfoPath )  ;
	std::string rootPath, clipName, sidecarPath;
	
	std::string mNRTFilePath;
	std::string oldSidecarPath;

private:

	void FillMetadataFiles ( std::vector<std::string> * metadataFiles );
	void MakeLegacyDigest ( std::string * digestStr );
	void CleanupLegacyXML();

	std::string xdcNS, legacyNS;

	ExpatAdapter * expat;
	XML_Node * clipMetadata;	// ! Don't delete, points into the Expat tree.

};	// XDCAM_MetaHandler

// =================================================================================================

#endif /* __XDCAM_Handler_hpp__ */
