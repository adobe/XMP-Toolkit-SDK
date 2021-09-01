#ifndef __GIF_Handler_hpp__
#define __GIF_Handler_hpp__	1

// =================================================================================================
// Copyright Adobe
// Copyright 2008 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// 
// This file includes implementation of GIF file metadata, according to GIF89a Specification. 
// https://www.w3.org/Graphics/GIF/spec-gif89a.txt
// The Graphics Interchange Format(c) is the Copyright property of CompuServe Incorporated. 
// GIF(sm) is a Service Mark property of CompuServe Incorporated.
// All Rights Reserved . http://www.w3.org/Consortium/Legal
//
//Derived from PNG_Handler.hpp by Ian Jacobi
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! Must be the first #include!

#include "public/include/XMP_Const.h"
#include "public/include/XMP_IO.hpp"

#include "XMPFiles/source/XMPFiles_Impl.hpp"

// =================================================================================================
/// \file GIF_Handler.hpp
/// \brief File format handler for GIF.
///
/// This header ...
///
// =================================================================================================

// *** Could derive from Basic_Handler - buffer file tail in a temp file.

extern XMPFileHandler* GIF_MetaHandlerCTor ( XMPFiles* parent );

extern bool GIF_CheckFormat ( XMP_FileFormat format,
							   XMP_StringPtr filePath,
                               XMP_IO*       fileRef,
                               XMPFiles*     parent );

static const XMP_OptionBits kGIF_HandlerFlags = ( kXMPFiles_CanInjectXMP |
												  kXMPFiles_CanExpand |
												  kXMPFiles_PrefersInPlace |
												  kXMPFiles_AllowsOnlyXMP |
												  kXMPFiles_ReturnsRawPacket |
												  kXMPFiles_NeedsReadOnlyPacket
													);

class GIF_MetaHandler : public XMPFileHandler
{
public:

	void CacheFileData();
	void ProcessXMP();

	void UpdateFile ( bool doSafeUpdate );
	void WriteTempFile ( XMP_IO* tempRef );

	bool SafeWriteFile ();

	GIF_MetaHandler ( XMPFiles* parent );
	virtual ~GIF_MetaHandler();

private:

	enum GIFBlockType 
	{
		kXMP_block_ImageDesc = 0x2C,
		kXMP_block_Extension = 0x21,
		kXMP_block_Trailer = 0x3B,
		kXMP_block_Header = 0x47
	};

	XMP_Uns64 XMPPacketOffset;
	XMP_Uns32 XMPPacketLength;
	XMP_Uns64 trailerOffset;

	bool ParseGIFBlocks( XMP_IO * fileRef );
	void ReadLogicalScreenDesc( XMP_IO* fileRef );
	void SeekFile( XMP_IO * fileRef, XMP_Int64 offset, SeekMode mode );

};	// GIF_MetaHandler

// =================================================================================================

#endif /* __GIF_Handler_hpp__ */
