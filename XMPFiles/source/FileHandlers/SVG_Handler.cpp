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
// Copyright © 1994-2002 World Wide Web Consortium, (Massachusetts Institute of Technology, 
// Institut National de Recherche en Informatique et en Automatique, Keio University). 
// All Rights Reserved . http://www.w3.org/Consortium/Legal
//
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! XMP_Environment.h must be the first included header.

#include "public/include/XMP_Const.h"
#include "public/include/XMP_IO.hpp"

#include "XMPFiles/source/XMPFiles_Impl.hpp"
#include "source/XMPFiles_IO.hpp"
#include "source/XIO.hpp"

#include "XMPFiles/source/FileHandlers/SVG_Handler.hpp"

using namespace std;

/*
	 Currently supporting only UTF-8 encoded SVG
*/

// =================================================================================================
// AppendData
// ===============

static inline void AppendData(RawDataBlock * dataOut, XMP_Uns8 * buffer, size_t count) {

	size_t prevSize = dataOut->size();	// ! Don't save a pointer, there might be a reallocation.
	dataOut->insert(dataOut->end(), count, 0);	// Add space to the RawDataBlock.
	memcpy(&( ( *dataOut ) [prevSize] ), buffer, count);

}	// AppendData

// =================================================================================================
// ReplaceData
// ===============

static inline void ReplaceData(RawDataBlock * dataOut, size_t pos, size_t deleteCount, const XMP_Uns8 * buffer, size_t count) {

	RawDataBlock::iterator iter = dataOut->begin() + pos;
	iter = dataOut->erase(iter, iter + deleteCount);
	dataOut->insert(iter, count, 0);
	memcpy(&( ( *dataOut ) [pos] ), buffer, count);

}	// ReplaceData

// =================================================================================================
//DecompressBuffer
// ==============================
// This function will decompress the buffer contents
static XMP_Uns64 DecompressBuffer(XMP_Uns8 * buffer, const XMP_Uns32 ioCount, RawDataBlock * dataOut)
{
	// Provided buffer is of size 1024 and contains compressed bytes, therefore buffer size is doubled 
	// do that decompressing can be done in single step.

	const size_t bufferSize = 2 * 1024;
	XMP_Uns8 bufferOut [bufferSize];

	z_stream zipState;
	memset(&zipState, 0, sizeof(zipState));

	// To decompress a gzip format file use windowBits as 16 + MAX_WBITS with inflateInit2
	int err = inflateInit2(&zipState, 16 + MAX_WBITS);
	if(err != Z_OK)
		return 0;

	// Initial input and output conditions. 
	zipState.next_out = &bufferOut [0];
	zipState.avail_out = bufferSize;
	zipState.next_in = &buffer [0];
	zipState.avail_in = ioCount;

	// Process all of this input, writing as needed.
	while(zipState.avail_in > 0)
	{
		XMP_Assert(zipState.avail_out > 0);	// Sanity check for output buffer space. pppp

		err = inflate(&zipState, Z_NO_FLUSH);
		if(err != Z_OK && err != Z_STREAM_END)
			return 0;
		if(zipState.avail_out == 0) {
			AppendData(dataOut, bufferOut, bufferSize);
			zipState.next_out = &bufferOut [0];
			zipState.avail_out = bufferSize;
		}
	}

	// Write the final output if any remaining.
	XMP_Uns32 remainingSize = bufferSize - zipState.avail_out;
	if(remainingSize > 0)
	{
		AppendData(dataOut, bufferOut, remainingSize);
		zipState.next_out = &bufferOut [0];
		zipState.avail_out = bufferSize;
	}

	// Finish the decompression 
	inflateEnd(&zipState);
	return zipState.total_out;
}	// DecompressBuffer

// =================================================================================================
// SVG_MetaHandler::DecompressFileToMemory
// ==============================
// This function will decompress gzip contents of a file into a buffer
XMP_Int64 SVG_MetaHandler::DecompressFileToMemory(XMP_IO * fileIn, RawDataBlock * dataOut)
{
	fileIn->Rewind();
	dataOut->clear();

	// Buffer size of 64K is efficient for faster compression and decompression
	static const size_t bufferSize = 64 * 1024;
	XMP_Uns8 bufferIn [bufferSize];
	XMP_Uns8 bufferOut [bufferSize];

	int err;
	z_stream zipState;
	memset(&zipState, 0, sizeof(zipState));

	// To decompress a gzip format file use windowBits as 16 + MAX_WBITS with inflateInit2
	err = inflateInit2(&zipState, 16 + MAX_WBITS);

	// Any information contained in the gzip header is not retained unless inflateGetHeader() is used.
	// Saving header for writing back compressed file 
	inflateGetHeader(&zipState, &compressedHeader);
	XMP_Enforce(err == Z_OK);

	XMP_Int32 ioCount;
	XMP_Int64 offsetIn = 0;
	const XMP_Int64 lengthIn = fileIn->Length();

	// Initial output conditions. Must be set before the input loop!
	zipState.next_out = &bufferOut [0];
	zipState.avail_out = bufferSize;

	while(offsetIn < lengthIn) {

		// Read the next chunk of input.
		ioCount = fileIn->Read(bufferIn, bufferSize);
		XMP_Enforce(ioCount > 0);
		offsetIn += ioCount;
		zipState.next_in = &bufferIn [0];
		zipState.avail_in = ioCount;

		// Process all of this input, writing as needed.

		err = Z_OK;
		while(( zipState.avail_in > 0 ) && ( err == Z_OK )) {

			XMP_Assert(zipState.avail_out > 0);	// Sanity check for output buffer space.
			err = inflate(&zipState, Z_NO_FLUSH);
			XMP_Enforce(( err == Z_OK ) || ( err == Z_STREAM_END ));

			if(zipState.avail_out == 0) {
				AppendData(dataOut, bufferOut, bufferSize);
				zipState.next_out = &bufferOut [0];
				zipState.avail_out = bufferSize;
			}
		}
	}

	// Finish the decompression and write the final output.

	do {

		ioCount = bufferSize - zipState.avail_out;	// Make sure there is room for inflate to do more.
		if(ioCount > 0) {
			AppendData(dataOut, bufferOut, ioCount);
			zipState.next_out = &bufferOut [0];
			zipState.avail_out = bufferSize;
		}

		err = inflate(&zipState, Z_NO_FLUSH);
		XMP_Enforce(( err == Z_OK ) || ( err == Z_STREAM_END ) || ( err == Z_BUF_ERROR ));

	} while(err == Z_OK);

	ioCount = bufferSize - zipState.avail_out;	// Write any final output.
	if(ioCount > 0) {
		AppendData(dataOut, bufferOut, ioCount);
		zipState.next_out = &bufferOut [0];
		zipState.avail_out = bufferSize;
	}

	// Done. Make sure the file header has the true decompressed size.
	XMP_Int64 lengthOut = zipState.total_out;
	inflateEnd(&zipState);
	return lengthOut;

}	// SVG_MetaHandler::DecompressFileToMemory

// =================================================================================================
// SVG_MetaHandler::CompressMemoryToFile
// ==============================
// This function will compress gzip contents of a buffer into a file
XMP_Int64 SVG_MetaHandler::CompressMemoryToFile(const RawDataBlock & dataIn, XMP_IO * fileOut)
{
	fileOut->Rewind();
	fileOut->Truncate(0);

	// 64K buffer is ideal for faster compression
	static const size_t bufferSize = 64 * 1024;
	XMP_Uns8 bufferOut [bufferSize];

	int err;
	z_stream zipState;
	memset(&zipState, 0, sizeof(zipState));
	err = deflateInit2(&zipState, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
	XMP_Enforce(err == Z_OK);

	// It provides gzip header information for when a gzip stream is requested by deflateInit2().
	// The gz_header structure are written to the gzip header which would have been saved during decompressing.
	err = deflateSetHeader(&zipState, &compressedHeader);
	XMP_Enforce(err == Z_OK);

	XMP_Int32 ioCount;
	const size_t lengthIn = dataIn.size();

	// Feed the input to the compression engine in one step, write the output as available.

	zipState.next_in = (Bytef*) &dataIn [0];
	zipState.avail_in = static_cast<XMP_Uns32>(lengthIn);
	zipState.next_out = &bufferOut [0];
	zipState.avail_out = bufferSize;

	while(zipState.avail_in > 0) {

		XMP_Assert(zipState.avail_out > 0);	// Sanity check for output buffer space.
		err = deflate(&zipState, Z_NO_FLUSH);
		XMP_Enforce(err == Z_OK);

		if(zipState.avail_out == 0) {
			fileOut->Write(bufferOut, bufferSize);
			zipState.next_out = &bufferOut [0];
			zipState.avail_out = bufferSize;
		}

	}

	// Finish the compression and write the final output.
	do {

		err = deflate(&zipState, Z_FINISH);
		XMP_Enforce(( err == Z_OK ) || ( err == Z_STREAM_END ));
		ioCount = bufferSize - zipState.avail_out;	// See if there is output to write.

		if(ioCount > 0) {
			fileOut->Write(bufferOut, ioCount);
			zipState.next_out = &bufferOut [0];
			zipState.avail_out = bufferSize;
		}

	} while(err != Z_STREAM_END);

	// Done.
	XMP_Int64 lengthOut = zipState.total_out;

	deflateEnd(&zipState);
	return lengthOut;
}	// SVG_MetaHandler::CompressMemoryToFile

// =================================================================================================
// SVG_MetaHandler::CompressFileToFile
// ==============================
// This function will compress gzip contents of a file into another file
XMP_Int64 SVG_MetaHandler::CompressFileToFile(XMP_IO * fileIn, XMP_IO * fileOut)
{
	fileIn->Rewind();
	fileOut->Truncate(0);

	// 64K buffer is ideal for faster compression
	static const size_t bufferSize = 64 * 1024;
	XMP_Uns8 bufferIn [bufferSize];
	XMP_Uns8 bufferOut [bufferSize];

	int err;
	z_stream zipState;
	memset(&zipState, 0, sizeof(zipState));
	err = deflateInit2(&zipState, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
	XMP_Enforce(err == Z_OK);
	// Setting the header which would have been saved during decompressing
	err = deflateSetHeader(&zipState, &compressedHeader);
	XMP_Enforce(err == Z_OK);

	XMP_Int32 ioCount;
	XMP_Int64 offsetIn = 0;
	const XMP_Int64 lengthIn = fileIn->Length();

	// Read the input file, feed it to the compression engine, writing as needed.
	// Initial output conditions. Must be set before the input loop!
	zipState.next_out = &bufferOut [0];
	zipState.avail_out = bufferSize;

	while(offsetIn < lengthIn) {

		// Read the next chunk of input.
		ioCount = fileIn->Read(bufferIn, bufferSize);
		XMP_Enforce(ioCount > 0);
		offsetIn += ioCount;
		zipState.next_in = &bufferIn [0];
		zipState.avail_in = ioCount;

		// Process all of this input, writing as needed. Yes, we need a loop. Compression means less
		// output than input, but a previous read has probably left partial compression results.

		while(zipState.avail_in > 0) {

			XMP_Assert(zipState.avail_out > 0);	// Sanity check for output buffer space.
			err = deflate(&zipState, Z_NO_FLUSH);
			XMP_Enforce(err == Z_OK);

			if(zipState.avail_out == 0) {
				fileOut->Write(bufferOut, bufferSize);
				zipState.next_out = &bufferOut [0];
				zipState.avail_out = bufferSize;
			}
		}
	}

	// Finish the compression and write the final output.

	do {

		err = deflate(&zipState, Z_FINISH);
		XMP_Enforce(( err == Z_OK ) || ( err == Z_STREAM_END ));
		ioCount = bufferSize - zipState.avail_out;	// See if there is output to write.

		if(ioCount > 0) {
			fileOut->Write(bufferOut, ioCount);
			zipState.next_out = &bufferOut [0];
			zipState.avail_out = bufferSize;
		}

	} while(err != Z_STREAM_END);

	// Done.

	XMP_Int64 lengthOut = zipState.total_out;
	deflateEnd(&zipState);
	return lengthOut;
}	// SVG_MetaHandler::CompressFileToFile


// =================================================================================================
// SVG_CheckFormat
// ===============

bool SVG_CheckFormat( XMP_FileFormat format,
	XMP_StringPtr  filePath,
	XMP_IO *       fileRef,
	XMPFiles *     parent )
{
	// 8K buffer is provided just to handle maximum SVG files
	// We can't check for SVG element in whole file which could take a lot of time for valid XML files
	IgnoreParam( filePath ); IgnoreParam( parent );

	XMP_Assert( format == kXMP_SVGFile );

	fileRef->Rewind();

	XMP_Uns8 buffer[ 1024 ];
	
	// Reading 4 bytes for BOM
	XMP_Uns32 bytesRead = fileRef->Read( buffer, 4 );
	if ( bytesRead != 4 )
		return false;

	// Checking for UTF-16 BOM and UTF-32 BOM
	if ( ( buffer[ 0 ] == 0xFF && buffer[ 1 ] == 0xFE ) || ( buffer[ 0 ] == 0xFE && buffer[ 1 ] == 0xFF ) || ( buffer[ 0 ] == buffer[ 1 ] == 0x00 && buffer[ 2 ] == 0xFE && buffer[ 3 ] == 0xFF ) )
	{
		return false;
	}

	// Initially we are intersted only in "svg" element.
	SVG_Adapter * svgChecker = new ( std::nothrow ) SVG_Adapter();
	if ( svgChecker == 0 )
		return false;

	svgChecker->SetErrorCallback(&parent->errorCallback);

	bool isSVG = false;
	bool isCompressed = false;

	// Checking for compressed header of gzip i.e. ID1 = (0x1F) , ID2 = (0x8B) & CM = (0x08)
	if(buffer [0] == 0x1f && buffer [1] == 0x8b && buffer [2] == 0x08)
		isCompressed = true;

	fileRef->Rewind();
	for ( XMP_Uns8 index = 0; index < 8; ++index )
	{
		XMP_Uns64 ioCount = fileRef->Read( buffer, sizeof( buffer ) );
		if ( ioCount == 0 ) break;

		// If compressed then first decompress 1024 bytes.
		if(isCompressed)
		{
			RawDataBlock block;
			ioCount = DecompressBuffer(buffer, static_cast<XMP_Uns32>(ioCount), &block);
			if(ioCount == 0)
				break;
			if(!svgChecker->ParseBufferNoThrow(block.data(), ioCount, false /* not the end */))
				break;
		}
		else
		{
			// Checking for well formed XML
			if(!svgChecker->ParseBufferNoThrow(buffer, ioCount, false /* not the end */))
				break;
		}

		if ( svgChecker->tree.GetNamedElement( kURI_SVG , "svg" ) )
		{
			isSVG = true;
			break;
		}
	}

	if ( svgChecker )
		delete ( svgChecker );

	return isSVG;

}	// SVG_CheckFormat

// =================================================================================================
// SVG_MetaHandlerCTor
// ===================

XMPFileHandler * SVG_MetaHandlerCTor( XMPFiles * parent )
{
	return new SVG_MetaHandler( parent );

}	// SVG_MetaHandlerCTor

// =================================================================================================
// SVG_MetaHandler::SVG_MetaHandler
// ================================

SVG_MetaHandler::SVG_MetaHandler(XMPFiles * _parent) : svgNode(0), svgAdapter(0), isTitleUpdateReq(false), isDescUpdateReq(false), isCompressed(false), compressedHeader()
{
	this->parent = _parent;
	this->handlerFlags = kSVG_HandlerFlags;
	this->stdCharForm = kXMP_Char8Bit;

}

// =================================================================================================
// SVG_MetaHandler::~SVG_MetaHandler
// =================================

SVG_MetaHandler::~SVG_MetaHandler()
{

	if ( this->svgAdapter != 0 ) 
	{
		delete ( this->svgAdapter );
		this->svgAdapter = 0;
	}
}

// =================================================================================================
// SVG_MetaHandler::GetSerializeOptions
// ===================================
//
// Override default implementation to ensure Canonical packet.

XMP_OptionBits SVG_MetaHandler::GetSerializeOptions()
{

	return ( kXMP_UseCanonicalFormat );

} // SVG_MetaHandler::GetSerializeOptions

// =================================================================================================
// SVG_MetaHandler::CacheFileData
// ==============================

void SVG_MetaHandler::CacheFileData() 
{
	XMP_Assert( !this->containsXMP );

	XMP_IO * fileRef = this->parent->ioRef;
	
	XMP_Uns8 marker[ 4 ];
	fileRef->Rewind();
	fileRef->Read( marker, 4 );

	// Checking for UTF-16 BOM and UTF-32 BOM
	if ( ( marker[ 0 ] == 0xFF && marker[ 1 ] == 0xFE ) || ( marker[ 0 ] == 0xFE && marker[ 1 ] == 0xFF ) || ( marker[ 0 ] == marker[ 1 ] == 0x00 && marker[ 2 ] == 0xFE && marker[ 3 ] == 0xFF ) )
	{
		XMP_Error error( kXMPErr_BadXML, "Invalid SVG file" );
		this->NotifyClient( &this->parent->errorCallback, kXMPErrSev_OperationFatal, error );
	}
		
	// Creating a new SVG Parser
	svgAdapter = new ( std::nothrow ) SVG_Adapter();
	if ( svgAdapter == 0 )
		XMP_Throw( "SVG_MetaHandler: Can't create SVG adapter", kXMPErr_NoMemory );
	svgAdapter->SetErrorCallback( &this->parent->errorCallback );

	// Registering all the required tags to SVG Parser
	svgAdapter->RegisterElement("svg", "");
	svgAdapter->RegisterPI( "xpacket" );
	svgAdapter->RegisterElement( "metadata", "svg" );
	svgAdapter->RegisterElement( "xmpmeta", "metadata" );
	svgAdapter->RegisterElement( "RDF", "metadata" );
	svgAdapter->RegisterElement( "title", "svg" );
	svgAdapter->RegisterElement( "desc", "svg" );


	// Checking for compressed header of gzip i.e. ID1 = (0x1F) , ID2 = (0x8B) & CM = (0x08)
	if(marker [0] == 0x1f && marker [1] == 0x8b && marker [2] == 0x08) {

		isCompressed = true;

		//Acc. to rfc1952 ISIZE (last 4 bytes) contains the size of the original(uncompressed) input data modulo 2 ^ 32.
		const XMP_Int64 maxSize = 0xFFFFFFFFUL;
		fileRef->Seek(-4, kXMP_SeekFromEnd);
		fileRef->Read(marker, 4);
		XMP_Uns32 expectedFullSize = GetUns32LE(marker);
		XMP_Enforce(expectedFullSize <= maxSize);

		svgContents.reserve(expectedFullSize); // Try to avoid reallocations.

		fileRef->Rewind(); 					   // Read the input file from start.

		// Filling svgContents with decompressed data
		this->DecompressFileToMemory(fileRef, &svgContents);
		
		// Parsing the whole buffer
		svgAdapter->ParseBuffer(this->svgContents.data(), this->svgContents.size(), true);	// End the parse.

	}
	else {
		// Parsing the whole file
		fileRef->Rewind();
		XMP_Uns8 buffer [64 * 1024];
		while(true) {
			XMP_Int32 ioCount = fileRef->Read(buffer, sizeof(buffer));
			if(ioCount == 0 || !svgAdapter->IsParsingRequire()) break;
			svgAdapter->ParseBuffer(buffer, ioCount, false /* not the end */);
		}
		svgAdapter->ParseBuffer(0, 0, true);	// End the parse.
	}

	XML_Node & xmlTree = this->svgAdapter->tree;
	XML_NodePtr rootElem = 0;

	for ( size_t i = 0, limit = xmlTree.content.size(); i < limit; ++i )
	{
		if ( xmlTree.content[ i ]->kind == kElemNode ) {
			rootElem = xmlTree.content[ i ];
		}
	}
	if ( rootElem == 0 )
		XMP_Throw( "Not a valid SVG File", kXMPErr_BadFileFormat );

	XMP_StringPtr rootLocalName = rootElem->name.c_str() + rootElem->nsPrefixLen;

	if ( ! XMP_LitMatch( rootLocalName, "svg" ) )
		XMP_Throw( "Not able to parse such SVG File", kXMPErr_BadFileFormat );
	
	// Making SVG node as Root Node
	svgNode = rootElem;

	bool FoundPI = false;
	bool FoundWrapper = false;
	XML_NodePtr metadataNode = svgNode->GetNamedElement( rootElem->ns.c_str(), "metadata" );

	// We are intersted only in the Metadata tag of outer SVG element
	// XMP should be present only in metadata Node of SVG
	if ( metadataNode != NULL )
	{
		XMP_Int64 packetLength = -1;
		XMP_Int64 packetOffset = -1;
		XMP_Int64 PIOffset = svgAdapter->GetPIOffset( "xpacket", 1 );
		OffsetStruct wrapperOffset = svgAdapter->GetElementOffsets( "xmpmeta" );
		OffsetStruct rdfOffset = svgAdapter->GetElementOffsets( "RDF" );
		
		// Checking XMP PI's position
		if ( PIOffset != -1 )
		{
			if ( wrapperOffset.startOffset != -1 && wrapperOffset.startOffset < PIOffset )
				packetOffset = wrapperOffset.startOffset;
			else
			{
				XMP_Int64 trailerOffset = svgAdapter->GetPIOffset( "xpacket", 2 );
				XML_NodePtr trailerNode = metadataNode->GetNamedElement( "", "xpacket", 1 );
				if (trailerOffset != -1 && trailerNode != 0)
				{
					packetLength = 2;								// "<?" = 2
					packetLength += trailerNode->name.length();		// Node's name
					packetLength += 1;								// Empty Space after Node's name
					packetLength += trailerNode->value.length();	// Value
					packetLength += 2;								// "?>" = 2
					packetLength += ( trailerOffset - PIOffset );
					packetOffset = PIOffset;
				}
			}
		}
		else if ( wrapperOffset.startOffset != -1 )		// XMP Wrapper is present without PI
		{
			XML_NodePtr wrapperNode = metadataNode->GetNamedElement( "adobe:ns:meta/", "xmpmeta" );
			if ( wrapperNode != 0 )
			{
				std::string trailerWrapper = "</x:xmpmeta>";
				packetLength = trailerWrapper.length();
				packetLength += ( wrapperOffset.endOffset - wrapperOffset.startOffset );
				packetOffset = wrapperOffset.startOffset;
			}
		}
		else		// RDF packet is present without PI and wrapper
		{
			XML_NodePtr rdfNode = metadataNode->GetNamedElement( "http://www.w3.org/1999/02/22-rdf-syntax-ns#", "RDF" );
			if ( rdfNode != 0 )
			{
				std::string rdfTrailer = "</rdf:RDF>";
				packetLength = rdfTrailer.length();
				packetLength += ( rdfOffset.endOffset - rdfOffset.startOffset );
				packetOffset = rdfOffset.startOffset;
			}
		}

		// Fill the necesarry information and packet with XMP data
		if ( packetOffset != -1 )
		{
			this->packetInfo.offset = packetOffset;
			this->packetInfo.length = ( XMP_Int32 ) packetLength;
			this->xmpPacket.assign(this->packetInfo.length, ' ');

			if(isCompressed)
				this->xmpPacket.assign((const char *) ( &this->svgContents.at(packetOffset) ), packetLength);
			else {
				fileRef->Seek(packetOffset, kXMP_SeekFromStart);
				fileRef->ReadAll(( void* )this->xmpPacket.data(), this->packetInfo.length);
			}
			FillPacketInfo( this->xmpPacket, &this->packetInfo );
			this->containsXMP = true;
			return;
		}
	}
	this->containsXMP = false;	

}	// SVG_MetaHandler::CacheFileData

// =================================================================================================
// SVG_MetaHandler::ProcessXMP
// ==============================

void SVG_MetaHandler::ProcessXMP()
{
	//
	// Here we are intersted in Only 2 childs, title and desc
	//
	this->processedXMP = true;	// Make sure we only come through here once.
	
	if ( svgNode == NULL )
		return;

	if ( !this->xmpPacket.empty() ) {
		XMP_Assert( this->containsXMP );
		this->xmpObj.ParseFromBuffer( this->xmpPacket.c_str(), ( XMP_StringLen )this->xmpPacket.size() );
	}
	
	// Description
	XML_NodePtr descNode = svgNode->GetNamedElement( svgNode->ns.c_str(), "desc" );
	if ( descNode != 0 && descNode->content.size() == 1 && descNode->content[0]->kind == kCDataNode )
	{
		this->xmpObj.SetLocalizedText( kXMP_NS_DC, "description", "", "x-default", descNode->content[0]->value, kXMP_DeleteExisting );
		this->containsXMP = true;
	}

	// Title
	XML_NodePtr titleNode = svgNode->GetNamedElement( svgNode->ns.c_str(), "title" );
	if ( titleNode != 0 && titleNode->content.size() == 1 && titleNode->content[ 0 ]->kind == kCDataNode )
	{
		this->xmpObj.SetLocalizedText( kXMP_NS_DC, "title", "", "x-default", titleNode->content[0]->value, kXMP_DeleteExisting );
		this->containsXMP = true;
	}
	
}	// SVG_MetaHandler::ProcessXMP

// =================================================================================================
// SVG_MetaHandler::ProcessTitle
// ===========================
// It is handling the updation and deletion case
void SVG_MetaHandler::ProcessTitle( XMP_IO* sourceRef, XMP_IO * destRef, const std::string &value, XMP_Int64 &currentOffset, const OffsetStruct & titleOffset )
{
	//sourceRef will be NULL in case read from svgContents i.e compressed case
	if ( value.empty() )
	{
		if(sourceRef != NULL) {
			XIO::Copy(sourceRef, destRef, titleOffset.startOffset - currentOffset);
			sourceRef->Seek(titleOffset.nextOffset, kXMP_SeekFromStart);
		}
		else
			destRef->Write(&( this->svgContents.data() [currentOffset] ), static_cast<XMP_Uns32>(titleOffset.startOffset - currentOffset));

		currentOffset = titleOffset.nextOffset;
	}
	else
	{
		char *tempStr = new char[titleOffset.endOffset - titleOffset.startOffset + 1];

		tempStr [titleOffset.endOffset - titleOffset.startOffset] = '\0';

		if(sourceRef != NULL) {
			sourceRef->Seek(titleOffset.startOffset, kXMP_SeekFromStart);
			sourceRef->Read(tempStr, static_cast<XMP_Uns32>(titleOffset.endOffset - titleOffset.startOffset));
			sourceRef->Seek(currentOffset, kXMP_SeekFromStart);
			const char *pos = strchr(tempStr, '>');
			XIO::Copy(sourceRef, destRef, titleOffset.startOffset - currentOffset + ( pos - tempStr + 1 ));
			sourceRef->Seek(titleOffset.endOffset, kXMP_SeekFromStart);
		}
		else{
			memcpy( tempStr, &( this->svgContents.data() [titleOffset.startOffset] ), titleOffset.endOffset - titleOffset.startOffset);
			tempStr [titleOffset.endOffset - titleOffset.startOffset] = '\0';
			const char *pos = strchr(tempStr, '>');
			destRef->Write(&( this->svgContents.data() [currentOffset] ), static_cast<XMP_Uns32>(titleOffset.startOffset - currentOffset + ( pos - tempStr + 1 )));
		}

		destRef->Write( value.c_str(), static_cast< int >( value.length() ) );
		currentOffset = titleOffset.endOffset;
		delete[] tempStr;
		tempStr = NULL;
	}
}	// SVG_MetaHandler::ProcessTitle

// =================================================================================================
// SVG_MetaHandler::ProcessDescription
// ===========================
// It is handling the updation and deletion case
void SVG_MetaHandler::ProcessDescription( XMP_IO* sourceRef, XMP_IO * destRef, const std::string &value, XMP_Int64 &currentOffset, const OffsetStruct & descOffset )
{
	if ( value.empty() )
	{
		if(sourceRef != NULL) {
			XIO::Copy(sourceRef, destRef, descOffset.startOffset - currentOffset);
			sourceRef->Seek(descOffset.nextOffset, kXMP_SeekFromStart);
		}
		currentOffset = descOffset.nextOffset;
	}
	else
	{
		char *tempStr = new char[descOffset.endOffset - descOffset.startOffset + 1];

		tempStr [descOffset.endOffset - descOffset.startOffset] = '\0';
		if(sourceRef != NULL) {
			sourceRef->Seek(descOffset.startOffset, kXMP_SeekFromStart);
			sourceRef->Read(tempStr, static_cast<XMP_Uns32>(descOffset.endOffset - descOffset.startOffset));
			sourceRef->Seek(currentOffset, kXMP_SeekFromStart);
			const char *pos = strchr(tempStr, '>');
			XIO::Copy(sourceRef, destRef, descOffset.startOffset - currentOffset + ( pos - tempStr + 1 ));
			sourceRef->Seek(descOffset.endOffset, kXMP_SeekFromStart);
		}
		else{
			memcpy( tempStr, &( this->svgContents.data() [descOffset.startOffset] ), descOffset.endOffset - descOffset.startOffset);
			const char *pos = strchr(tempStr, '>');
			destRef->Write(&( this->svgContents.data() [currentOffset] ), static_cast<XMP_Uns32>(descOffset.startOffset - currentOffset + ( pos - tempStr + 1 )));
		}
		destRef->Write( value.c_str(), static_cast< int >( value.length() ) );
		currentOffset = descOffset.endOffset;
		delete[] tempStr;
		tempStr = NULL;
	}

}	// SVG_MetaHandler::ProcessDescription

// =================================================================================================
// SVG_MetaHandler::InsertNewTitle
// ===========================
// It is handling the insertion case
void SVG_MetaHandler::InsertNewTitle( XMP_IO * destRef, const std::string &value, const std::string &prefix )
{
	std::string titleElement = "<" + prefix + "title>";
	destRef->Write( titleElement.c_str(), static_cast< int >( titleElement.length() ) );
	destRef->Write( value.c_str(), static_cast< int >( value.length() ) );
	titleElement = "</" + prefix + "title>\n";
	destRef->Write( titleElement.c_str(), static_cast< int >( titleElement.length() ) );

}	// SVG_MetaHandler::InsertNewTitle

// =================================================================================================
// SVG_MetaHandler::InsertNewDescription
// ===========================
// It is handling the insertion case
void SVG_MetaHandler::InsertNewDescription( XMP_IO * destRef, const std::string &value, const std::string &prefix )
{
	std::string descElement = "<" + prefix + "desc>";
	destRef->Write( descElement.c_str(), static_cast< int >( descElement.length() ) );
	destRef->Write( value.c_str(), static_cast< int >( value.length() ) );
	descElement = "</" + prefix + "desc>\n";
	destRef->Write( descElement.c_str(), static_cast< int >( descElement.length() ) );

}	// SVG_MetaHandler::InsertNewDescription

// =================================================================================================
// SVG_MetaHandler::InsertNewMetadata
// ===========================
// It is handling the insertion case
void SVG_MetaHandler::InsertNewMetadata( XMP_IO * destRef, const std::string &value, const std::string &prefix )
{

	std::string metadataElement = "<" + prefix + "metadata>";
	destRef->Write( metadataElement.c_str(), static_cast< int >( metadataElement.length() ) );
	destRef->Write( value.c_str(), static_cast< int >( value.length() ) );
	metadataElement = "</" + prefix + "metadata>\n";
	destRef->Write( metadataElement.c_str(), static_cast< int >( metadataElement.length() ) );

}	// SVG_MetaHandler::InsertNewMetadata

// =================================================================================================
// SVG_MetaHandler::UpdateFile
// ===========================

void SVG_MetaHandler::UpdateFile( bool doSafeUpdate )
{
	XMP_Assert( !doSafeUpdate );	// This should only be called for "unsafe" updates.
	
	XMP_IO* sourceRef = this->parent->ioRef;

	if ( sourceRef == NULL || svgNode == NULL )
		return;

	// Checking whether Title updation requires or not
	std::string title;
	XML_NodePtr titleNode = svgNode->GetNamedElement( svgNode->ns.c_str(), "title" );
	(void) this->xmpObj.GetLocalizedText( kXMP_NS_DC, "title", "", "x-default", 0, &title, 0 );
	if ( ( titleNode == NULL ) == ( title.empty() ) )
	{
		if ( titleNode != NULL && titleNode->content.size() == 1 && titleNode->content[ 0 ]->kind == kCDataNode && !XMP_LitMatch( titleNode->content[ 0 ]->value.c_str(), title.c_str() ) )
			isTitleUpdateReq = true;
	}
	else
		isTitleUpdateReq = true;

	// Checking whether Description updation requires or not
	std::string description;
	XML_NodePtr descNode = svgNode->GetNamedElement( svgNode->ns.c_str(), "desc" );
	( void ) this->xmpObj.GetLocalizedText( kXMP_NS_DC, "description", "", "x-default", 0, &description, 0 );
	if ( ( descNode == NULL ) == ( description.empty() ) )
	{
		if ( descNode != NULL && descNode->content.size() == 1 && descNode->content[ 0 ]->kind == kCDataNode && !XMP_LitMatch( descNode->content[ 0 ]->value.c_str(), description.c_str() ) )
			isDescUpdateReq = true;
	}
	else
		isDescUpdateReq = true;

	//	If any updation is required then don't do inplace replace
	bool isUpdateRequire = isTitleUpdateReq | isDescUpdateReq | (this->packetInfo.offset == kXMPFiles_UnknownOffset);

	// Inplace Updation of XMP
	if ( !isUpdateRequire && ((XMP_Int32)this->xmpPacket.size() == this->packetInfo.length) )
	{
		if(this->isCompressed)
		{
			ReplaceData(&this->svgContents, this->packetInfo.offset, this->packetInfo.length, ( const XMP_Uns8 * )this->xmpPacket.data(), this->packetInfo.length);
			CompressMemoryToFile(this->svgContents, sourceRef);
		}
		else {
			sourceRef->Seek(this->packetInfo.offset, kXMP_SeekFromStart);
			sourceRef->Write(this->xmpPacket.c_str(), static_cast<int>( this->xmpPacket.size() ));
		}
	}
	else
	{
		
		// Inplace is not possibe, So perform full updation
		try
		{
			XMP_IO* tempRef = sourceRef->DeriveTemp();
			this->WriteTempFile(tempRef);
		}
		catch(...)
		{
			sourceRef->DeleteTemp();
			throw;
		}

		sourceRef->AbsorbTemp();
	}

	this->needsUpdate = false;

}	// SVG_MetaHandler::UpdateFile

// =================================================================================================
// SVG_MetaHandler::WriteTempFile
// ==============================
//
void SVG_MetaHandler::WriteTempFile( XMP_IO* tempRef )
{
	XMP_Assert( this->needsUpdate );

	XMP_IO* sourceRef = this->parent->ioRef;
	if ( sourceRef == NULL || svgNode == NULL )
		return;

	tempRef->Rewind();
	sourceRef->Rewind();

	if(isCompressed)
		sourceRef = NULL;

	XMP_Int64 currentOffset = svgAdapter->firstSVGElementOffset;
	if(currentOffset == -1) {
		currentOffset = svgAdapter->GetElementOffsets("svg").endOffset;
	}
	Write(sourceRef, tempRef, currentOffset, 0);

	OffsetStruct titleOffset = svgAdapter->GetElementOffsets( "title" );
	OffsetStruct descOffset = svgAdapter->GetElementOffsets( "desc" );
	OffsetStruct metadataOffset = svgAdapter->GetElementOffsets( "metadata" );

	std::string title;
	std::string description;

	XML_NodePtr titleNode = svgNode->GetNamedElement( svgNode->ns.c_str(), "title" );
	( void ) this->xmpObj.GetLocalizedText( kXMP_NS_DC, "title", "", "x-default", 0, &title, 0 );

	XML_NodePtr descNode = svgNode->GetNamedElement( svgNode->ns.c_str(), "desc" );
	( void ) this->xmpObj.GetLocalizedText( kXMP_NS_DC, "description", "", "x-default", 0, &description, 0 );

	// Need to cover the case of both workflows
	// This would have been called after inplace is not possible
	// This would have called for safe update
	if ( !isTitleUpdateReq )
	{
		if ( ( titleNode == NULL ) == ( title.empty() ) )
		{
			if ( titleNode != NULL && titleNode->content.size() == 1 && titleNode->content[ 0 ]->kind == kCDataNode && !XMP_LitMatch( titleNode->content[ 0 ]->value.c_str(), title.c_str() ) )
				isTitleUpdateReq = true;
		}
		else
			isTitleUpdateReq = true;
	}
	if ( !isDescUpdateReq )
	{
		if ( ( descNode == NULL ) == ( description.empty() ) )
		{
			if ( descNode != NULL && descNode->content.size() == 1 && descNode->content[ 0 ]->kind == kCDataNode &&  !XMP_LitMatch( descNode->content[ 0 ]->value.c_str(), description.c_str() ) )
				isDescUpdateReq = true;
		}
		else
			isDescUpdateReq = true;
	}

	// Initial Insertion/Updation

	//Find the prefix of URI "http://www.w3.org/2000/svg"
	//It is not possible that this uri is not present , as it is checked in checkFileFormat

	XMP_StringPtr prefix;
	XMP_StringLen prefixLen;

	std::string nameSpacePrefix = "";
	bool found = svgAdapter->registeredNamespaces->GetPrefix ( kURI_SVG , &prefix , &prefixLen );
	if( !found ) {
		XMP_Error error ( kXMPErr_ExternalFailure , "SVG URI not present in svg file" );
		svgAdapter->NotifyClient ( kXMPErrSev_OperationFatal , error );
	}
	
	if( strcmp(prefix,"_dflt_:") != 0 ) 
		nameSpacePrefix.assign ( prefix, prefixLen );

	// Insert/Update Title if requires
	// Don't insert/update it if Metadata or desc child comes before title child
	bool isTitleWritten = !isTitleUpdateReq;
	if ( isTitleUpdateReq )
	{
		// Insertion Case
		if ( titleNode == NULL )
		{
			InsertNewTitle( tempRef, title, nameSpacePrefix );
			isTitleWritten = true;
		}
		else if ( ( descOffset.startOffset == -1 || titleOffset.startOffset < descOffset.startOffset )	// Updation/Deletion Case
			&& ( metadataOffset.startOffset == -1 || titleOffset.startOffset < metadataOffset.startOffset ) )
		{
			ProcessTitle( sourceRef, tempRef, title, currentOffset, titleOffset );
			isTitleWritten = true;
		}
	}

	// Insert/Update Description if requires
	// Don't insert/update it if Metadata child comes before desc child
	bool isDescWritten = !isDescUpdateReq;
	if ( isDescUpdateReq )
	{
		if ( descNode == NULL )
		{
			if ( titleOffset.nextOffset != -1 )
			{
				Write(sourceRef, tempRef, titleOffset.nextOffset - currentOffset, currentOffset);
				currentOffset = titleOffset.nextOffset;
			}
			InsertNewDescription( tempRef, description, nameSpacePrefix );
			isDescWritten = true;
		}
		else if ( metadataOffset.startOffset == -1 || descOffset.startOffset < metadataOffset.startOffset )
		{
			ProcessDescription( sourceRef, tempRef, description, currentOffset, descOffset );
			isDescWritten = true;
		}
	}

	// Insert/Update Metadata if requires
	// Don't insert/update it if case is DTM
	bool isMetadataWritten = false;
	if ( metadataOffset.startOffset == -1 )
	{
		if ( descOffset.nextOffset != -1 )
		{
			Write(sourceRef, tempRef, descOffset.nextOffset - currentOffset, currentOffset);
			currentOffset = descOffset.nextOffset;
		}
		else if ( titleOffset.nextOffset != -1 )
		{
			Write(sourceRef, tempRef, titleOffset.nextOffset - currentOffset, currentOffset);
			currentOffset = titleOffset.nextOffset;
		}
		InsertNewMetadata( tempRef, this->xmpPacket, nameSpacePrefix ); 
		isMetadataWritten = true;
	}
	else if ( !( !isTitleWritten && isDescWritten && titleOffset.startOffset < metadataOffset.startOffset ) )		// Not DTM
	{
		// No XMP packet was present in the file
		if ( this->packetInfo.offset == kXMPFiles_UnknownOffset )
		{
			/*
			std::string metadataElement = "<" + nameSpacePrefix+ "metadata>"; 
			XIO::Copy( sourceRef, tempRef, metadataOffset.startOffset - currentOffset + metadataElement.length() );
			currentOffset = sourceRef->Offset();
			tempRef->Write( this->xmpPacket.c_str(), static_cast< int >( this->xmpPacket.length() ) );
			*/

			Write(sourceRef, tempRef, metadataOffset.startOffset - currentOffset, currentOffset);
			InsertNewMetadata(tempRef, this->xmpPacket.c_str(), nameSpacePrefix);
			if(sourceRef != NULL){
				sourceRef->Seek(metadataOffset.nextOffset, kXMP_SeekFromStart);
			}
			currentOffset = metadataOffset.nextOffset;
		}
		else	// Replace XMP Packet
		{
			Write(sourceRef, tempRef, this->packetInfo.offset - currentOffset, currentOffset);
			if(sourceRef != NULL) {
				sourceRef->Seek(this->packetInfo.offset + this->packetInfo.length, kXMP_SeekFromStart);
			}
			tempRef->Write(this->xmpPacket.c_str(), static_cast< int >( this->xmpPacket.length() ));
			currentOffset = this->packetInfo.offset + this->packetInfo.length;
		}
		isMetadataWritten = true;
	}

	// If simple cases was followed then copy rest file
	if ( isTitleWritten && isDescWritten && isMetadataWritten )
	{
		Write(sourceRef, tempRef, ( (sourceRef ? sourceRef->Length() : this->svgContents.size() ) - currentOffset ), currentOffset);
		return;
	}

	// If the case is not Simple (TDM) then perform these operations
	if ( isDescWritten )		// TDM, DTM, DMT
	{
		if ( !isTitleWritten )		// DTM, DMT
		{
			if ( titleOffset.startOffset < metadataOffset.startOffset )		// DTM
			{
				ProcessTitle( sourceRef, tempRef, title, currentOffset, titleOffset );
				isTitleWritten = true;

				if ( this->packetInfo.offset == kXMPFiles_UnknownOffset )
				{
					std::string metadataElement = "<" + nameSpacePrefix + "metadata>";
					Write(sourceRef, tempRef, metadataOffset.startOffset - currentOffset + metadataElement.length(), currentOffset);
					currentOffset = metadataOffset.startOffset + metadataElement.length();
					tempRef->Write(this->xmpPacket.c_str(), static_cast< int >( this->xmpPacket.length() ));
				}
				else
				{
					Write(sourceRef, tempRef, this->packetInfo.offset - currentOffset, currentOffset);
					tempRef->Write(this->xmpPacket.c_str(), static_cast< int >( this->xmpPacket.length() ));
					if(sourceRef != NULL)
						sourceRef->Seek(this->packetInfo.offset + this->packetInfo.length, kXMP_SeekFromStart);
					currentOffset = this->packetInfo.offset + this->packetInfo.length;
				}
				isMetadataWritten = true;

			}
			else	// DMT
			{
				ProcessTitle( sourceRef, tempRef, title, currentOffset, titleOffset );
				isTitleWritten = true;
			}
		}
		// Else
		// Would have already covered this case: TDM

	}
	else		//  TMD, MDT, MTD
	{
		if ( isTitleWritten )		// TMD
		{
			ProcessDescription( sourceRef, tempRef, description, currentOffset, descOffset );
			isDescWritten = true;
		}
		else		// MDT or MTD
		{
			if ( titleOffset.startOffset < descOffset.startOffset )	// MTD
			{
				ProcessTitle( sourceRef, tempRef, title, currentOffset, titleOffset );
				isTitleWritten = true;

				ProcessDescription( sourceRef, tempRef, description, currentOffset, descOffset );
				isDescWritten = true;
			}
			else		// MDT
			{
				ProcessDescription( sourceRef, tempRef, description, currentOffset, descOffset );
				isDescWritten = true;

				ProcessTitle( sourceRef, tempRef, title, currentOffset, titleOffset );
				isTitleWritten = true;
			}
		}
	}

	// Finally Everything would have been written
	XMP_Enforce(isTitleWritten && isDescWritten && isMetadataWritten);
	Write(sourceRef, tempRef, ( ( sourceRef ? sourceRef->Length() : this->svgContents.size() ) - currentOffset ), currentOffset);
	this->needsUpdate = false;

	if(this->isCompressed)
	{
		try
		{
			XMP_IO* tempCompressedRef = tempRef->DeriveTemp();
			this->CompressFileToFile(tempRef, tempCompressedRef);
		}
		catch(...)
		{
			tempRef->DeleteTemp();
			throw;
		}
		tempRef->AbsorbTemp();
	}
}	// SVG_MetaHandler::WriteTempFile

// =================================================================================================
// SVG_MetaHandler::Write 
// ==============================
// This function will write from buffer(svgContents i.e uncompressed contents) to destFile if sourceFile is NULL , 
// otherwise from sourceFile to DestFile. 
void SVG_MetaHandler::Write(XMP_IO* sourceFile, XMP_IO* destFile, XMP_Int64 length, XMP_Int64 currentOffset) {

	if(sourceFile != NULL) {
		XIO::Copy(sourceFile, destFile, length);
	}
	else {
		//assuming currentOffset and length will never be negative
		if(this->svgContents.capacity() - static_cast<decltype(this->svgContents.capacity())>(currentOffset) < static_cast<decltype(this->svgContents.capacity())>(length))
			this->svgContents.reserve(currentOffset + length);
		destFile->Write(&( this->svgContents.data() [currentOffset] ), static_cast<XMP_Uns32>(length));
	}
}	//SVG_MetaHandler::Write
