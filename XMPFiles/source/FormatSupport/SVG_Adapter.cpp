// =================================================================================================
// Copyright 2015 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
//
// This file includes implementation of SVG metadata, according to Scalable Vector Graphics (SVG) 1.1 Specification.
// "https://www.w3.org/TR/2003/REC-SVG11-20030114/"
// Copyright © 1994-2002 World Wide Web Consortium, (Massachusetts Institute of Technology,
// Institut National de Recherche en Informatique et en Automatique, Keio University).
// All Rights Reserved . http://www.w3.org/Consortium/Legal
//
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! Must be the first #include!

#include "XMPFiles/source/FormatSupport/SVG_Adapter.hpp"

#include "third-party/expat/lib/expat.h"
#include <string.h>

using namespace std;

#if XMP_WinBuild
#pragma warning ( disable : 4996 )	// '...' was declared deprecated
#endif

#define FullNameSeparator	'@'

// =================================================================================================

static void StartNamespaceDeclHandler( void * userData, XMP_StringPtr prefix, XMP_StringPtr uri );
static void EndNamespaceDeclHandler( void * userData, XMP_StringPtr prefix );
static void StartElementHandler( void * userData, XMP_StringPtr name, XMP_StringPtr* attrs );
static void EndElementHandler( void * userData, XMP_StringPtr name );
static void CharacterDataHandler( void * userData, XMP_StringPtr cData, int len );
static void ProcessingInstructionHandler( void * userData, XMP_StringPtr target, XMP_StringPtr data );
static void DeclarationHandler( void *userData, const XML_Char  *version, const XML_Char  *encoding, int standalone );

static bool isRequireData = false;
static XMP_Uns32 reqDepth = 0;

// Flag is provided to support behaviour like Expat Adapter
#if BanAllEntityUsage

// For now we do this by banning DOCTYPE entirely. This is easy and consistent with what is
// available in recent Java XML parsers. Another, somewhat less drastic, approach would be to 
// ban all entity declarations. We can't allow declarations and ban references, Expat does not
// call the SkippedEntityHandler for references in attribute values.

// ! Standard entities (&amp;, &lt;, &gt;, &quot;, &apos;, and numeric character references) are
// ! not banned. Expat handles them transparently no matter what.

static void StartDoctypeDeclHandler( void * userData, XMP_StringPtr doctypeName,
	XMP_StringPtr sysid, XMP_StringPtr pubid, int has_internal_subset );

#endif

// =================================================================================================

SVG_Adapter::SVG_Adapter() : parser(0), registeredNamespaces(0), firstSVGElementOffset(-1), depth(0)
{
	
	this->parser = XML_ParserCreateNS( 0, FullNameSeparator );

	if ( this->parser == 0 ) {
		XMP_Error error( kXMPErr_NoMemory, "Failure creating Expat parser" );
		this->NotifyClient( kXMPErrSev_ProcessFatal, error );
	}
	else
	{
		this->registeredNamespaces = new XMP_NamespaceTable();

		XML_SetUserData( this->parser, this );
		XML_SetNamespaceDeclHandler( this->parser, StartNamespaceDeclHandler, EndNamespaceDeclHandler );
		XML_SetElementHandler( this->parser, StartElementHandler, EndElementHandler );
		XML_SetCharacterDataHandler( this->parser, CharacterDataHandler );
		XML_SetProcessingInstructionHandler( this->parser, ProcessingInstructionHandler );
		XML_SetXmlDeclHandler( this->parser, DeclarationHandler );

#if BanAllEntityUsage
		XML_SetStartDoctypeDeclHandler( this->parser, StartDoctypeDeclHandler );
		isAborted = false;
#endif

		this->parseStack.push_back( &this->tree );	// Push the XML root node.
	}
}	// SVG_Adapter::SVG_Adapter

// =================================================================================================

SVG_Adapter::~SVG_Adapter()
{
	if ( this->parser != 0 ) XML_ParserFree( this->parser );
	this->parser = 0;

	if ( this->registeredNamespaces != 0 ) delete ( this->registeredNamespaces );
	this->registeredNamespaces = 0;

}	// SVG_Adapter::~SVG_Adapter

// =================================================================================================

OffsetStruct SVG_Adapter::GetElementOffsets( std::string elementName )
{
	IteratorStringOffsetStruct iterator = this->mOffsetsMap.find( elementName );
	if ( iterator != mOffsetsMap.end() )
		return iterator->second;
	
	return OffsetStruct();
}   // SVG_Adapter::GetElementOffset

// =================================================================================================

void SVG_Adapter::RegisterElement( std::string elementName, std::string reqParent )
{
	IteratorStringOffsetStruct iterator = this->mOffsetsMap.find( elementName );
	if ( iterator == mOffsetsMap.end() )
	{
		this->mOffsetsMap.insert( iterator, std::pair<std::string, OffsetStruct>( elementName, OffsetStruct( reqParent ) ) );
	}
}   // SVG_Adapter::RegisterElement

// =================================================================================================

XMP_Int64 SVG_Adapter::GetPIOffset( std::string PIName, XMP_Uns32 requiredIndex /* = 1 */ )
{
	if ( this->parser != 0 )
	{
		std::pair<IteratorStringXMP_Int64, IteratorStringXMP_Int64> iterator = this->mPIWithOffsetMap.equal_range( PIName );
		if ( iterator.first != iterator.second )
		{
			XMP_Uns32 index = 0;
			IteratorStringXMP_Int64 indexIterator = iterator.first;
			for ( ; index < ( requiredIndex - 1 ) && indexIterator != iterator.second; ++indexIterator, ++index );
			if ( indexIterator != this->mPIWithOffsetMap.end() && index == requiredIndex - 1 )
				return indexIterator->second;
		}
	}
	return -1;
}   // SVG_Adapter::GetPIOffset

// =================================================================================================

void SVG_Adapter::RegisterPI( std::string PIName )
{
	IteratorStringXMP_Int64 iterator = this->mPIWithOffsetMap.find( PIName );
	if ( iterator == mPIWithOffsetMap.end() )
	{
		this->mPIWithOffsetMap.insert( iterator, std::pair<std::string, XMP_Int64>( PIName, -1 ) );
	}
}   // SVG_Adapter::RegisterPI

// =================================================================================================

XMP_Bool SVG_Adapter::IsParsingRequire( )
{
	for ( IteratorStringOffsetStruct iterator = this->mOffsetsMap.begin(); iterator != this->mOffsetsMap.end(); ++iterator )
	{
		if ( iterator->second.startOffset == -1 || iterator->second.endOffset == -1 || iterator->second.nextOffset == -1 )
			return true;
	}
	return false;
}   // SVG_Adapter::IsParsingRequire

// =================================================================================================

// This version of parsing throw an error
void SVG_Adapter::ParseBuffer( const void * buffer, size_t length, bool last /* = true */ )
{
	enum XML_Status status;

	if ( length == 0 ) {	// Expat does not like empty buffers.
		if ( !last ) return;
		const char * kOneSpace = " ";
		buffer = kOneSpace;
		length = 1;
	}

	status = XML_Parse( this->parser, ( const char * ) buffer, static_cast< XMP_StringLen >( length ), last );

#if BanAllEntityUsage
	if ( this->isAborted ) {
		XMP_Error error( kXMPErr_BadXML, "DOCTYPE is not allowed" );
			this->NotifyClient( kXMPErrSev_Recoverable, error );
	}
#endif

	if ( status != XML_STATUS_OK ) {

		XMP_Error error( kXMPErr_BadXML, "Invalid SVG file" );
		this->NotifyClient( kXMPErrSev_OperationFatal, error );

	}

}	// SVG_Adapter::ParseBuffer

// =================================================================================================

// This version of parsing doesn't throw error but returns false if any error is encountered
// This is required just for checkformat
XMP_Bool SVG_Adapter::ParseBufferNoThrow( const void * buffer, size_t length, bool last /* = true */ )
{
	enum XML_Status status;

	if ( length == 0 ) {	// Expat does not like empty buffers.
		if ( !last ) return false;
		const char * kOneSpace = " ";
		buffer = kOneSpace;
		length = 1;
	}

	try
	{
		status = XML_Parse(this->parser, (const char *)buffer, static_cast<XMP_StringLen>(length), last);
	}
	catch (XMP_Error &e)
	{
		return false; //Don't let one failure abort checking other file formats , this api is called only from checkFileFormat
	}

#if BanAllEntityUsage
	if ( this->isAborted ) {
		XMP_Error error( kXMPErr_BadXML, "DOCTYPE is not allowed" );
			this->NotifyClient( kXMPErrSev_Recoverable, error );
	}
#endif

	if ( status != XML_STATUS_OK )
		return false;
	else
		return true;

}	// SVG_Adapter::ParseBufferNoThrow

// =================================================================================================

static void ParseFullNS( XMP_StringPtr fullName, string & NS, string &localName )
{
	// Expat delivers the full name as a catenation of namespace URI, separator, and local name.
	size_t sepPos = strlen( fullName );
	if (!sepPos)
		return; //Throw?
	for ( --sepPos; sepPos > 0; --sepPos ) {
		if ( fullName[ sepPos ] == FullNameSeparator ) break;
	}

	if ( fullName[ sepPos ] == FullNameSeparator )
	{
		localName = fullName + sepPos + 1;
		NS.assign( fullName, sepPos );
	}
	else
		localName = fullName;
	
}	// ParseFullNS

// =================================================================================================

static void StartNamespaceDeclHandler( void * userData, XMP_StringPtr prefix, XMP_StringPtr uri )
{
	
	SVG_Adapter * thiz = ( SVG_Adapter* ) userData;

	if ( prefix == 0 ) prefix = "_dflt_";	// Have default namespace.
	if ( uri == 0 ) return;	                // Ignore, have xmlns:pre="", no URI to register.

	( void ) thiz->registeredNamespaces->Define( uri, prefix, 0, 0 );

}	// StartNamespaceDeclHandler

// =================================================================================================

static void EndNamespaceDeclHandler( void * userData, XMP_StringPtr prefix )
{
	IgnoreParam( userData );
	IgnoreParam( prefix );
	if ( prefix == 0 ) prefix = "_dflt_";	// Have default namespace.

}	// EndNamespaceDeclHandler

// =================================================================================================

static void StartElementHandler( void * userData, XMP_StringPtr name, XMP_StringPtr* attrs )
{
	// In case, if name is NULL then ParseBuffer would return with error status
	SVG_Adapter * thiz = ( SVG_Adapter* ) userData;
	thiz->depth++;
	if ( thiz->depth > 3 )
		return;
	else if ( thiz->firstSVGElementOffset == -1 && thiz->depth == 2 )
		thiz->firstSVGElementOffset = XML_GetCurrentByteIndex( thiz->parser );
	else
	{
		if ( !thiz->mPrevRequiredElement.empty() )
		{
			IteratorStringOffsetStruct iterator = thiz->mOffsetsMap.find( thiz->mPrevRequiredElement );
			if ( iterator != thiz->mOffsetsMap.end() )
				iterator->second.nextOffset = XML_GetCurrentByteIndex( thiz->parser );
			thiz->mPrevRequiredElement.clear();
		}
	}

	string NS, localName;
	ParseFullNS( name, NS, localName );

	IteratorStringOffsetStruct iterator = thiz->mOffsetsMap.find( localName );
	if ( iterator == thiz->mOffsetsMap.end() && localName != "svg" )
		return;
		
	if( thiz->depth > 2  && ( localName == "metadata" || localName == "title" || localName == "desc" ) )
		return;

	XML_Node * parentNode = thiz->parseStack.back();
	XML_Node * elemNode = new XML_Node( parentNode, "", kElemNode );

	if ( strncmp( localName.c_str(), name, localName.length() ) != 0 )
	{
		XMP_StringPtr prefix;
		XMP_StringLen prefixLen;
		bool found = thiz->registeredNamespaces->GetPrefix( NS.c_str(), &prefix, &prefixLen );
		if ( !found ) {
			XMP_Error error( kXMPErr_ExternalFailure, "Unknown URI in Expat full name" );
			thiz->NotifyClient( kXMPErrSev_OperationFatal, error );
		}
		elemNode->ns = NS;
		elemNode->nsPrefixLen = prefixLen;	// ! Includes the ':'.

		if ( strcmp( prefix, "_dflt_:" ) == 0 || ( (localName == "svg" || localName == "metadata") && NS == kURI_SVG) )
		{
			elemNode->name = localName;
			elemNode->nsPrefixLen = 0;
		}
		else
		{
			elemNode->name = prefix;
			elemNode->name += localName;
		}
	}
	else
	{
		elemNode->name = localName;	// The name is not in a namespace.
	}

	parentNode->content.push_back( elemNode );
	thiz->parseStack.push_back( elemNode );

	if ( iterator != thiz->mOffsetsMap.end() && iterator->second.parent == parentNode->name )
	{
		reqDepth = thiz->depth;
		isRequireData = true;
		if ( iterator->second.startOffset == -1 )
			iterator->second.startOffset = XML_GetCurrentByteIndex( thiz->parser );
	}
	else
	{
		isRequireData = false;
	}

}	// StartElementHandler

// =================================================================================================

static void EndElementHandler( void * userData, XMP_StringPtr name )
{
	SVG_Adapter * thiz = ( SVG_Adapter* ) userData;

	thiz->depth--;
	if ( thiz->depth > 2 )
		return;

	string NS, localName;
	ParseFullNS( name, NS, localName );

	if(thiz->depth > 1 && ( localName == "metadata" || localName == "title" || localName == "desc" ))
		return;

	IteratorStringOffsetStruct iterator = thiz->mOffsetsMap.find( localName );
	if ( iterator != thiz->mOffsetsMap.end() )
	{
		// StartOffset flag is provided to reject the elements of non-required namespace 
		// Endoffset flag is provided to maintain state of first available element
		// Depth flag is provided to support for workflow like <title><title>...</title></title>
		if ( iterator->second.startOffset != -1 && iterator->second.endOffset == -1 && thiz->depth == reqDepth - 1 )
		{
			iterator->second.endOffset = XML_GetCurrentByteIndex( thiz->parser );
			thiz->mPrevRequiredElement = localName;
		}
	}
	else if ( localName != "svg" )
		return;

	( void ) thiz->parseStack.pop_back();

}	// EndElementHandler

// =================================================================================================

static void CharacterDataHandler( void * userData, XMP_StringPtr cData, int len )
{
	if ( !isRequireData )
		return;
	isRequireData = false;
	SVG_Adapter * thiz = ( SVG_Adapter* ) userData;

	if ( ( cData == 0 ) || ( len == 0 ) ) { cData = ""; len = 0; }

	XML_Node * parentNode = thiz->parseStack.back();
	XML_Node * cDataNode = new XML_Node( parentNode, "", kCDataNode );

	cDataNode->value.assign( cData, len );
	parentNode->content.push_back( cDataNode );

}	// CharacterDataHandler

// =================================================================================================

static void ProcessingInstructionHandler( void * userData, XMP_StringPtr target, XMP_StringPtr data )
{
	
	if ( target == NULL || strncmp( target, "xpacket", 7 ) != 0 )
		return;	// Ignore all PIs except the XMP packet wrapper.
	SVG_Adapter * thiz = ( SVG_Adapter* ) userData;
	XML_Node * parentNode = thiz->parseStack.back();
	
	XMP_VarString nsURI = parentNode->ns;
	XMP_VarString elementName = parentNode->name;
	//remove prefix from name if any
	size_t colonPos = elementName.find ( ':' );
	XMP_VarString parentNodeName ( elementName.substr ( colonPos + 1 , elementName.size() - colonPos ) );

	if ( nsURI != kURI_SVG || parentNodeName != "metadata" )
		return;
	IteratorStringXMP_Int64 iterator = thiz->mPIWithOffsetMap.find( target );
	if ( iterator != thiz->mPIWithOffsetMap.end() )
	{
		if ( iterator->second == -1 )
			iterator->second = XML_GetCurrentByteIndex( thiz->parser );
		else
			thiz->mPIWithOffsetMap.insert( std::pair<std::string, XMP_Int64>( target, XML_GetCurrentByteIndex( thiz->parser ) ) );
	}
	
	if ( data == 0 ) data = "";
	XML_Node * piNode = new XML_Node( parentNode, target, kPINode );

	piNode->value.assign( data );
	parentNode->content.push_back( piNode );

}	// ProcessingInstructionHandler

// =================================================================================================

static void DeclarationHandler( void *userData, const XML_Char  *version, const XML_Char  *encoding, int standalone )
{
	if ( encoding == NULL || strlen( encoding ) != 5 ||
		( tolower( encoding[ 0 ] ) == 'u'
		&& tolower( encoding[ 1 ] ) == 't'
		&& tolower( encoding[ 2 ] ) == 'f'
		&& encoding[ 3 ] == '-'
		&& encoding[ 4 ] == '8' ) )
		return;
	else
	{
		SVG_Adapter * thiz = ( SVG_Adapter* ) userData;
		( void ) ( XML_StopParser( thiz->parser, false ) );
	}
}	// DeclarationHandler

// =================================================================================================

#if BanAllEntityUsage
static void StartDoctypeDeclHandler( void * userData, XMP_StringPtr doctypeName,
	XMP_StringPtr sysid, XMP_StringPtr pubid, int has_internal_subset )
{
	IgnoreParam( userData );

	SVG_Adapter * thiz = ( SVG_Adapter* ) userData;

	thiz->isAborted = true;	// ! Can't throw an exception across the plain C Expat frames.
	( void ) XML_StopParser( thiz->parser, XML_FALSE /* not resumable */ );

}	// StartDoctypeDeclHandler
#endif

// =================================================================================================
