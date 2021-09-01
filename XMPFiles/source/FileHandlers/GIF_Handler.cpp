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
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! Must be the first #include!

#include "XMPFiles/source/FileHandlers/GIF_Handler.hpp"
#include "source/XIO.hpp"

// =================================================================================================
/// \file GIF_Handler.hpp
/// \brief File format handler for GIF.
///
/// This handler ...
///
// =================================================================================================

// =================================================================================================
// GIF_MetaHandlerCTor
// ====================

XMPFileHandler * GIF_MetaHandlerCTor ( XMPFiles * parent )
{
	return new GIF_MetaHandler ( parent );

}	// GIF_MetaHandlerCTor

#define GIF_89_Header_LEN		6
#define GIF_89_Header_DATA	"\x47\x49\x46\x38\x39\x61" // must be GIF89a, nothing else as XMP is supported only in 89a version

#define APP_ID_LEN		11
#define XMP_APP_ID_DATA		"\x58\x4D\x50\x20\x44\x61\x74\x61\x58\x4D\x50"

#define MAGIC_TRAILER_LEN		258

// =================================================================================================
// GIF_CheckFormat
// ===============

bool GIF_CheckFormat ( XMP_FileFormat format,
					   XMP_StringPtr  filePath,
                       XMP_IO*        fileRef,
                       XMPFiles *     parent )
{
	IgnoreParam(format); IgnoreParam(fileRef); IgnoreParam(parent);
	XMP_Assert ( format == kXMP_GIFFile );

	if ( fileRef->Length() < GIF_89_Header_LEN ) return false;
	XMP_Uns8 buffer[ GIF_89_Header_LEN ];

	fileRef->Rewind();
	fileRef->Read( buffer, GIF_89_Header_LEN );
	if ( !CheckBytes( buffer, GIF_89_Header_DATA, GIF_89_Header_LEN ) ) return false;

	return true;

}	// GIF_CheckFormat

// =================================================================================================
// GIF_MetaHandler::GIF_MetaHandler
// ==================================

GIF_MetaHandler::GIF_MetaHandler( XMPFiles * _parent ) : XMPPacketOffset( 0 ), XMPPacketLength( 0 ), trailerOffset( 0 )
{
	this->parent = _parent;
	this->handlerFlags = kGIF_HandlerFlags;
	// It MUST be UTF-8.
	this->stdCharForm = kXMP_Char8Bit;

}

// =================================================================================================
// GIF_MetaHandler::~GIF_MetaHandler
// ===================================

GIF_MetaHandler::~GIF_MetaHandler()
{
}

// =================================================================================================
// GIF_MetaHandler::CacheFileData
// ===============================

void GIF_MetaHandler::CacheFileData()
{
	this->containsXMP = false;

	XMP_IO * fileRef = this->parent->ioRef;

	// Try to navigate through the blocks to find the XMP block.
	if ( this->ParseGIFBlocks( fileRef ) )
	{
		// XMP packet present
		this->xmpPacket.assign( XMPPacketLength, ' ' );

		// 13 bytes for the block size and 2 bytes for Extension ID and Label
		this->SeekFile( fileRef, XMPPacketOffset, kXMP_SeekFromStart );
		fileRef->ReadAll( ( void* )this->xmpPacket.data(), XMPPacketLength );

		this->packetInfo.offset = XMPPacketOffset;
		this->packetInfo.length = XMPPacketLength;
		this->containsXMP = true;
	}
	// else no XMP

}	// GIF_MetaHandler::CacheFileData

// =================================================================================================
// GIF_MetaHandler::ProcessXMP
// ===========================
//
// Process the raw XMP and legacy metadata that was previously cached.

void GIF_MetaHandler::ProcessXMP()
{
	this->processedXMP = true;	// Make sure we only come through here once.

	// Process the XMP packet.

	if ( ! this->xmpPacket.empty() ) {

		XMP_Assert ( this->containsXMP );
		XMP_StringPtr packetStr = this->xmpPacket.c_str();
		XMP_StringLen packetLen = (XMP_StringLen) this->xmpPacket.size();

		this->xmpObj.ParseFromBuffer ( packetStr, packetLen );

		this->containsXMP = true;

	}

}	// GIF_MetaHandler::ProcessXMP

// =================================================================================================
// GIF_MetaHandler::ParseGIFBlocks
// ===========================

bool GIF_MetaHandler::ParseGIFBlocks( XMP_IO* fileRef )
{
	fileRef->Rewind();

	// Checking for GIF header
	XMP_Uns8 buffer[ GIF_89_Header_LEN ];

	fileRef->Read( buffer, GIF_89_Header_LEN );
	XMP_Enforce( memcmp( buffer, GIF_89_Header_DATA, GIF_89_Header_LEN ) == 0 );

	bool IsXMPExists = false;
	bool IsTrailerExists = false;

	ReadLogicalScreenDesc( fileRef );

	// Parsing rest of the blocks
	while ( fileRef->Offset() != fileRef->Length() )
	{
		XMP_Uns8 blockType;

		// Read the block type byte
		fileRef->Read( &blockType, 1 );

		if ( blockType == kXMP_block_ImageDesc )
		{

			// ImageDesc is a special case, So read data just like its structure.
			long tableSize = 0;
			XMP_Uns8 fields;
			// Reading Dimesnions of image as 
			// 2 bytes = Image Left Position
			// + 2 bytes = Image Right Position
			// + 2 bytes = Image Width
			// + 2 bytes = Image Height
			// = 8 bytes
			this->SeekFile( fileRef, 8, kXMP_SeekFromCurrent );

			// Reading one byte for Packed Fields
			fileRef->Read( &fields, 1 );

			// Getting Local Table Size and skipping table size
			if ( fields & 0x80 )
			{
				tableSize = ( 1 << ( ( fields & 0x07 ) + 1 ) ) * 3;
				this->SeekFile( fileRef, tableSize, kXMP_SeekFromCurrent );
			}

			// 1 byte LZW Minimum code size
			this->SeekFile( fileRef, 1, kXMP_SeekFromCurrent );

			XMP_Uns8 subBlockSize;
			// 1 byte compressed sub-block size
			fileRef->Read( &subBlockSize, 1 );

			while ( subBlockSize != 0x00 )
			{
				// Skipping compressed data sub-block
				this->SeekFile( fileRef, subBlockSize, kXMP_SeekFromCurrent );

				// 1 byte compressed sub-block size
				fileRef->Read( &subBlockSize, 1 );
			}

		}
		else if ( blockType == kXMP_block_Extension )
		{
			XMP_Uns8 extensionLbl;
			XMP_Uns32 blockSize = 0;
			XMP_Uns64 blockOffset = fileRef->Offset();

			// Extension Label
			fileRef->Read( &extensionLbl, 1 );

			// Block or Sub-Block size
			fileRef->Read( &blockSize, 1 );

			// Checking for Application Extension label and blockSize
			if ( extensionLbl == 0xFF && blockSize == APP_ID_LEN )
			{
				XMP_Uns8 idData[ APP_ID_LEN ];
				fileRef->Read( idData, APP_ID_LEN, true );

				// Checking For XMP ID
				if ( memcmp( idData, XMP_APP_ID_DATA, APP_ID_LEN ) == 0 )
				{
					XMPPacketOffset = fileRef->Offset();
					IsXMPExists = true;
				}

				// Parsing sub-blocks
				XMP_Uns8 subBlockSize;
				fileRef->Read( &subBlockSize, 1 );
				while ( subBlockSize != 0x00 )
				{
					this->SeekFile( fileRef, subBlockSize, kXMP_SeekFromCurrent );
					fileRef->Read( &subBlockSize, 1 );
				}
				if ( IsXMPExists ) {
					XMP_Int64 packetLength = fileRef->Offset() - XMPPacketOffset - MAGIC_TRAILER_LEN;
					if( packetLength < 0 ) throw XMP_Error(kXMPErr_BadFileFormat, "corrupt GIF File.");
					XMPPacketLength = static_cast< XMP_Uns32 >( packetLength );
				}
			}
			else
			{
				// Extension block other than Application Extension
				while ( blockSize != 0x00 )
				{
					// Seeking block size or sub-block size
					this->SeekFile( fileRef, blockSize, kXMP_SeekFromCurrent );

					// Block Size
					fileRef->Read( &blockSize, 1 );
				}
			}
		}
		else if ( blockType == kXMP_block_Trailer )
		{
			// 1 byte is subtracted for block type
			trailerOffset = fileRef->Offset() - 1;
			IsTrailerExists = true;
			break;
		}
		else
			XMP_Throw( "Invaild GIF Block", kXMPErr_BadBlockFormat );
	}

	if ( !IsTrailerExists )
		XMP_Throw( "No trailer exists for GIF file", kXMPErr_BadFileFormat );

	return IsXMPExists;

}	// GIF_MetaHandler::ParseGIFBlocks

// =================================================================================================
// GIF_MetaHandler::ReadLogicalScreenDesc
// ===========================

void GIF_MetaHandler::ReadLogicalScreenDesc( XMP_IO* fileRef )
{
	XMP_Uns8 fields;

	// 2 bytes for Screen Width
	// + 2 bytes for Screen Height
	// = 4 Bytes
	this->SeekFile( fileRef, 4, kXMP_SeekFromCurrent );

	// 1 byte for Packed Fields
	fileRef->Read( &fields, 1 );

	// 1 byte for Background Color Index
	// + 1 byte for Pixel Aspect Ratio
	// = 2 bytes
	this->SeekFile( fileRef, 2, kXMP_SeekFromCurrent );

	// Look for Global Color Table if exists 
	if ( fields & 0x80 )
	{
		long tableSize = ( 1 << ( ( fields & 0x07 ) + 1 ) ) * 3;
		this->SeekFile( fileRef, tableSize, kXMP_SeekFromCurrent );
	}

}	// GIF_MetaHandler::ReadLogicalScreenDesc

// =================================================================================================
// GIF_MetaHandler::SeekFile
// ===========================

void GIF_MetaHandler::SeekFile( XMP_IO * fileRef, XMP_Int64 offset, SeekMode mode )
{
	if ( offset > fileRef->Length() || ( mode == kXMP_SeekFromCurrent && fileRef->Offset() + offset > fileRef->Length() ) )
	{
		XMP_Throw( "Out of range seek operation", kXMPErr_InternalFailure );
	}
	else
		fileRef->Seek( offset, mode );

}	// GIF_MetaHandler::SeekFile

// =================================================================================================
// GIF_MetaHandler::UpdateFile
// ===========================

void GIF_MetaHandler::UpdateFile ( bool doSafeUpdate )
{
	XMP_Assert( !doSafeUpdate );	// This should only be called for "unsafe" updates.

	if ( ! this->needsUpdate ) return;

	XMP_IO * fileRef = this->parent->ioRef;

	XMP_StringPtr packetStr = xmpPacket.c_str();
	XMP_StringLen newPacketLength = (XMP_StringLen)xmpPacket.size();

	if ( newPacketLength == XMPPacketLength )
	{
		this->SeekFile( fileRef, this->packetInfo.offset, kXMP_SeekFromStart );
		fileRef->Write( this->xmpPacket.c_str(), newPacketLength );
	}
	else
	{
		XMP_IO* tempFile = fileRef->DeriveTemp();
		if ( tempFile == 0 ) XMP_Throw( "Failure creating GIF temp file", kXMPErr_InternalFailure );

		this->WriteTempFile( tempFile );
		fileRef->AbsorbTemp();
	}

	this->needsUpdate = false;

}	// GIF_MetaHandler::UpdateFile

// =================================================================================================
// GIF_MetaHandler::WriteTempFile
// ==============================

void GIF_MetaHandler::WriteTempFile ( XMP_IO* tempRef )
{
	XMP_Assert( this->needsUpdate );

	XMP_IO* originalRef = this->parent->ioRef;
	originalRef->Rewind();

	tempRef->Truncate ( 0 );
	
	if ( XMPPacketOffset != 0 )
	{
		// Copying blocks before XMP Application Block
		XIO::Copy( originalRef, tempRef, XMPPacketOffset );

		// Writing XMP Packet
		tempRef->Write( this->xmpPacket.c_str(), (XMP_Uns32)this->xmpPacket.size() );

		// Copying Rest of the file
		originalRef->Seek( XMPPacketLength, kXMP_SeekFromCurrent );
		XIO::Copy( originalRef, tempRef, originalRef->Length() - originalRef->Offset() );

	}
	else
	{
		if ( trailerOffset == 0 )
			XMP_Throw( "Not able to write XMP packet in GIF file", kXMPErr_BadFileFormat );

		// Copying blocks before XMP Application Block
		XIO::Copy( originalRef, tempRef, trailerOffset );

		// Writing Extension Introducer 
		XIO::WriteUns8( tempRef, kXMP_block_Extension );

		// Writing Application Extension label
		XIO::WriteUns8( tempRef, 0xFF );

		// Writing Application Extension label
		XIO::WriteUns8( tempRef, APP_ID_LEN );

		// Writing Application Extension label
		tempRef->Write( XMP_APP_ID_DATA, APP_ID_LEN );

		// Writing XMP Packet
		tempRef->Write( this->xmpPacket.c_str(), (XMP_Uns32)this->xmpPacket.size() );

		// Writing Magic trailer
		XMP_Uns8 magicByte = 0x01;
		tempRef->Write( &magicByte, 1 );
		for ( magicByte = 0xFF; magicByte != 0x00; --magicByte )
			tempRef->Write( &magicByte, 1 );
		tempRef->Write( &magicByte, 1 );
		tempRef->Write( &magicByte, 1 );

		// Copying Rest of the file
		XIO::Copy( originalRef, tempRef, originalRef->Length() - originalRef->Offset() );

	}

}	// GIF_MetaHandler::WriteTempFile

// =================================================================================================
