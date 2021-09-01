#ifndef __SVG_Handler_hpp__
#define __SVG_Handler_hpp__	1

// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
//
// This file includes implementation of SVG metadata, according to Scalable Vector Graphics (SVG) 1.1 Specification. 
// "https://www.w3.org/TR/2003/REC-SVG11-20030114/"
// Copyright  1994-2002 World Wide Web Consortium, (Massachusetts Institute of Technology, 
// Institut National de Recherche en Informatique et en Automatique, Keio University). 
// All Rights Reserved . http://www.w3.org/Consortium/Legal
//
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! XMP_Environment.h must be the first included header.

#include "public/include/XMP_Const.h"
#include "public/include/XMP_IO.hpp"

#include "XMPFiles/source/XMPFiles_Impl.hpp"
#include "XMPFiles/source/FormatSupport/SVG_Adapter.hpp"

#include "third-party/zlib/zlib.h"

extern XMPFileHandler* SVG_MetaHandlerCTor( XMPFiles* parent );

extern bool SVG_CheckFormat( XMP_FileFormat format,
	XMP_StringPtr  filePath,
	XMP_IO *       fileRef,
	XMPFiles *     parent );

static const XMP_OptionBits kSVG_HandlerFlags = (	kXMPFiles_CanInjectXMP |
													kXMPFiles_CanExpand |
													kXMPFiles_CanRewrite |
													kXMPFiles_PrefersInPlace |
													kXMPFiles_CanReconcile |
													kXMPFiles_ReturnsRawPacket |
													kXMPFiles_AllowsSafeUpdate );

class SVG_MetaHandler : public XMPFileHandler {

public:

	void CacheFileData();
	void ProcessXMP();

	void UpdateFile( bool doSafeUpdate );
	void WriteTempFile( XMP_IO* tempRef );
	
	XMP_OptionBits GetSerializeOptions();

	SVG_MetaHandler( XMPFiles* parent );
	virtual ~SVG_MetaHandler();

private:

	SVG_MetaHandler() {};
	SVG_Adapter * svgAdapter;
	XML_NodePtr svgNode;
	bool isTitleUpdateReq;
	bool isDescUpdateReq;

	RawDataBlock svgContents;
	bool isCompressed;
	gz_header compressedHeader;

	void ProcessTitle( XMP_IO* sourceRef, XMP_IO * destRef, const std::string &value, XMP_Int64 &currentOffset, const OffsetStruct & titleOffset );
	void ProcessDescription( XMP_IO* sourceRef, XMP_IO * destRef, const std::string &value, XMP_Int64 &currentOffset, const OffsetStruct & descOffset );
	void InsertNewTitle( XMP_IO * destRef, const std::string &value, const std::string &prefix );
	void InsertNewDescription( XMP_IO * destRef, const std::string &value, const std::string &prefix );
	void InsertNewMetadata( XMP_IO * destRef, const std::string &value, const std::string &prefix );

	void Write(XMP_IO* sourceFile, XMP_IO* destFile, XMP_Int64 length, XMP_Int64 currentOffset);

	XMP_Int64 CompressMemoryToFile(const RawDataBlock & dataOut, XMP_IO * fileIn);
	XMP_Int64 DecompressFileToMemory(XMP_IO * fileIn, RawDataBlock * dataOut);
	XMP_Int64 CompressFileToFile(XMP_IO * fileIn, XMP_IO * fileOut);
	
};	// SVG_MetaHandler

// =================================================================================================

#endif /* __SVG_Handler_hpp__ */
