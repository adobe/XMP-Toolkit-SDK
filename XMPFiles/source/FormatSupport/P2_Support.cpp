
// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! XMP_Environment.h must be the first included header.

#include <sstream>

#include "public/include/XMP_Const.h"
#include "public/include/XMP_IO.hpp"

#include "XMPFiles/source/XMPFiles_Impl.hpp"
#include "source/XMPFiles_IO.hpp"
#include "source/XIO.hpp"
#include "source/ExpatAdapter.hpp"

#include "source/IOUtils.hpp"

#include "XMPFiles/source/FormatSupport/P2_Support.hpp"
#include "third-party/zuid/interfaces/MD5.h"

P2_Clip::P2_Clip(const std::string & p2ClipMetadataFilePath)
	:p2XMLParser(0), p2Root(0), headContentCached(false)
	, p2ClipContent(0), filePath(p2ClipMetadataFilePath)
{
	PrepareForExpatParser(p2ClipMetadataFilePath);
}



void P2_Clip::PrepareForExpatParser(const std::string &p2ClipMetadataFilePath)
{
	try {


		Host_IO::FileRef hostRef = Host_IO::Open(p2ClipMetadataFilePath.c_str(), Host_IO::openReadOnly);
		XMPFiles_IO xmlFile(hostRef, p2ClipMetadataFilePath.c_str(), Host_IO::openReadOnly);
		CreateExpatParser(xmlFile);
		xmlFile.Close();
	}
	catch (...)
	{
		DestroyExpatParser();
		throw;
	}

}
P2_Clip::~P2_Clip()
{
	DestroyExpatParser();
}

void P2_Clip::CreateExpatParser(XMPFiles_IO &xmlFile)
{
	this->p2XMLParser = XMP_NewExpatAdapter ( ExpatAdapter::kUseLocalNamespaces );
	if ( this->p2XMLParser == 0 ) XMP_Throw ( "P2_MetaHandler: Can't create Expat adapter", kXMPErr_NoMemory );

	XMP_Uns8 buffer [64*1024];
	while ( true ) {
		XMP_Int32 ioCount = xmlFile.Read ( buffer, sizeof(buffer) );
		if ( ioCount == 0 ) break;
		this->p2XMLParser->ParseBuffer ( buffer, ioCount, false /* not the end */ );
	}
	this->p2XMLParser->ParseBuffer ( 0, 0, true );	
}

void P2_Clip::DestroyExpatParser()
{
	 delete this->p2XMLParser; 
	 this->p2XMLParser = 0; 
	 p2Root=0;
	 headContent.reset();
	 headContentCached = false;
}

XML_NodePtr P2_Clip::GetP2RootNode()
{
	if (p2Root!=0) return p2Root;
	// The root element should be P2Main in some namespace. At least 2 different namespaces are in
	// use (ending in "v3.0" and "v3.1"). Take whatever this file uses.

	XML_Node & xmlTree = this->p2XMLParser->tree;
	XML_NodePtr rootElem = 0;

	for ( size_t i = 0, limit = xmlTree.content.size(); i < limit; ++i ) {
		if ( xmlTree.content[i]->kind == kElemNode ) {
			rootElem = xmlTree.content[i];
		}
	}

	if ( rootElem == 0 ) return 0;
	XMP_StringPtr rootLocalName = rootElem->name.c_str() + rootElem->nsPrefixLen;
	if ( ! XMP_LitMatch ( rootLocalName, "P2Main" ) ) return 0;

	this->p2Root = rootElem;
	return p2Root;
}
static void  GetElementLocation(XML_NodePtr p2node,std::string*& elemLoc ) 
{
	if ( p2node != 0 && p2node->IsLeafContentNode() ) 
	{
		elemLoc= p2node->GetLeafContentPtr();
	}
}

static void GetElementValue(XML_NodePtr p2node, XMP_Uns32 &value) 
{
	if ( p2node != 0 && p2node->IsLeafContentNode() ) 
	{
		value =atoi(p2node->GetLeafContentValue());
	}
}
void P2_Clip::CacheClipContent()
{
	if (headContentCached) return;
	headContentCached = true;
	XML_NodePtr p2RootNode = GetP2RootNode();
	if( p2RootNode == 0 ) return;
	XMP_StringPtr p2NameSpace = p2RootNode->ns.c_str();

	p2ClipContent = GetP2RootNode()->GetNamedElement ( p2NameSpace, "ClipContent" );
	if ( p2ClipContent == 0 )  return;
	XML_NodePtr  p2node;

	p2node= p2ClipContent->GetNamedElement ( p2NameSpace, "GlobalClipID" );
	GetElementLocation(p2node,headContent.clipId );

	p2node= p2ClipContent->GetNamedElement ( p2NameSpace, "ClipName" );
	GetElementLocation(p2node,headContent.clipTitle );

	p2node= p2ClipContent->GetNamedElement ( p2NameSpace, "Duration" );
	GetElementValue(p2node,headContent.duration );

	p2node= p2ClipContent->GetNamedElement ( p2NameSpace, "EditUnit" );
	GetElementLocation(p2node,headContent.scaleUnit );

	headContent.clipMetadata= p2ClipContent->GetNamedElement ( p2NameSpace, "ClipMetadata" );
	headContent.essenceList= p2ClipContent->GetNamedElement ( p2NameSpace, "EssenceList" );

	p2node= p2ClipContent->GetNamedElement ( p2NameSpace, "Relation" );
	if ( p2node != 0 )
	{
		XML_NodePtr p2Offset= p2node->GetNamedElement ( p2NameSpace, "OffsetInShot" );
		GetElementValue(p2Offset,headContent.OffsetInShot );		
		p2Offset= p2node->GetNamedElement ( p2NameSpace, "GlobalShotID" );
		GetElementLocation(p2Offset,headContent.shotId );
		XML_NodePtr p2connection= p2node->GetNamedElement ( p2NameSpace, "Connection" );
		if ( p2connection != 0 )
		{
			p2node= p2connection->GetNamedElement ( p2NameSpace, "Top" );
			if ( p2node != 0 )
			{
				p2node= p2node->GetNamedElement ( p2NameSpace, "GlobalClipID" );
				GetElementLocation(p2node,headContent.topClipId );
			}			
			p2node= p2connection->GetNamedElement ( p2NameSpace, "Next" );
			if ( p2node != 0 )
			{
				p2node= p2node->GetNamedElement ( p2NameSpace, "GlobalClipID" );
				GetElementLocation(p2node,headContent.nextClipId );
			}			
			p2node= p2connection->GetNamedElement ( p2NameSpace, "Previous" );
			if ( p2node != 0 )
			{
				p2node= p2node->GetNamedElement ( p2NameSpace, "GlobalClipID" );
				GetElementLocation(p2node,headContent.prevClipId );
			}
		}
	}
}

bool P2_Clip::IsValidClip()
{
	this->CacheClipContent();
	return headContent.clipId != 0;
}
bool P2_Clip::IsSpannedClip()
{
	return IsValidClip() && headContent.topClipId != 0 &&( headContent.prevClipId != 0 || headContent.nextClipId!=0 );

}

bool P2_Clip::IsTopClip()
{
	return IsValidClip() && headContent.topClipId != 0  &&  *(headContent.topClipId) == *(headContent.clipId);
}

XMP_Uns32 P2_Clip::GetOffsetInShot() 
{
	this->CacheClipContent();
	return this->headContent.OffsetInShot;
}

XMP_Uns32 P2_Clip::GetDuration()
{
	this->CacheClipContent();
	return this->headContent.duration;
}

std::string* P2_Clip::GetClipId()
{
	this->CacheClipContent();
	return this->headContent.clipId;
}

std::string* P2_Clip::GetClipName()
{
  if ( this->clipName == "" )
  {
    std::string tempPath = this->filePath;		
    XIO::SplitLeafName(&tempPath, &this->clipName);
    std::string ext;
    XIO::SplitFileExtension(&this->clipName, &ext);		
  }
  return &this->clipName;
}

std::string P2_Clip::GetClipTitle()
{  
  this->CacheClipContent();
  if ( ! this->headContent.clipTitle ) return std::string("");
  return *this->headContent.clipTitle;
}
std::string* P2_Clip::GetNextClipId()
{
	this->CacheClipContent();
	return this->headContent.nextClipId;
}

std::string* P2_Clip::GetPreviousClipId()
{
	this->CacheClipContent();
	return this->headContent.prevClipId;
}

std::string* P2_Clip::GetTopClipId()
{
	this->CacheClipContent();
	return this->headContent.topClipId;
}

std::string* P2_Clip::GetShotId()
{
	this->CacheClipContent();
	return this->headContent.shotId;
}

std::string* P2_Clip::GetEditUnit()
{
	this->CacheClipContent();
	return this->headContent.scaleUnit;
}

XML_NodePtr P2_Clip::GetClipContentNode()
{
	this->CacheClipContent();
	return this->p2ClipContent;
}

XML_NodePtr P2_Clip::GetClipMetadataNode()
{
	this->CacheClipContent();
	return this->headContent.clipMetadata;
}

XML_NodePtr P2_Clip::GetEssenceListNode()
{
	this->CacheClipContent();
	return this->headContent.essenceList;
}

std::string P2_Clip::GetXMPFilePath()
{
	std::string ClipMetadataPath =  this->GetClipPath();
	std::string ignoreext;
	XIO::SplitFileExtension(&ClipMetadataPath,&ignoreext);
	return ClipMetadataPath+ ".XMP";
}

void P2_Clip::CreateDigest ( std::string * digestStr )
{
	return;
}

void  P2_Clip::SerializeP2ClipContent(std::string& xmlContentData) 
{
	this->p2XMLParser->tree.Serialize ( &xmlContentData );
}


P2_SpannedClip::P2_SpannedClip(const std::string & p2ClipMetadataFilePath):
	P2_Clip(p2ClipMetadataFilePath)
{
	P2_Clip* p2Clip= dynamic_cast<P2_Clip*>(this);
	spannedP2Clip.insert(p2Clip);
	if (p2Clip->GetClipId())
		addedClipIds.insert(*p2Clip->GetClipId());
}

bool P2_SpannedClip::AddIfRelated(P2_Clip* newClip)
{
	std::string* tClipId = newClip->GetTopClipId();
	if( tClipId != 0 && *(tClipId)==*this->GetTopClipId() && 
		newClip->IsValidClip() && addedClipIds.find(*newClip->GetClipId()) == addedClipIds.end())
	{
		spannedP2Clip.insert(newClip);
		addedClipIds.insert(*newClip->GetClipId());
		return true;
	}
	return false;
}

void P2_SpannedClip::checkSpannedClipIsComplete()
{
	RelatedP2ClipList::iterator iter = spannedP2Clip.begin();
	if (!(*iter)->IsTopClip()) 
		completeSpannedClip = false;
		
	std::string* next = (*iter)->GetNextClipId();
	while (++iter != spannedP2Clip.end() &&
		next != 0 && (*iter)->IsValidClip() &&
		*next == *((*iter)->GetClipId())
		)
		next = (*iter)->GetNextClipId();
	if (iter != spannedP2Clip.end() || next != 0)
	{
		iter = spannedP2Clip.begin();
		std::string* prev = (*iter)->GetClipId();
		while (++iter != spannedP2Clip.end() &&
			prev != 0 && (*iter)->GetPreviousClipId() != 0 &&
			*prev == *((*iter)->GetPreviousClipId())
			)
			prev = (*iter)->GetClipId();
		if (iter != spannedP2Clip.end()) completeSpannedClip = false;
	}
	completeSpannedClip = true;
}

bool P2_SpannedClip::IsComplete()
{
	return completeSpannedClip;
}

std::string P2_SpannedClip::GetXMPFilePath()
{	
	if (  this->IsComplete() )
	{
		std::string ClipMetadataPath =  (*spannedP2Clip.begin())->GetClipPath();
		std::string ignoreext;
		XIO::SplitFileExtension(&ClipMetadataPath,&ignoreext);
		return ClipMetadataPath+ ".XMP";
	}else
	{
		return P2_Clip::GetXMPFilePath();
	}
}

void P2_SpannedClip::DigestElement( MD5_CTX & md5Context, XML_NodePtr legacyContext, XMP_StringPtr legacyPropName )
{
	XML_NodePtr legacyProp = legacyContext->GetNamedElement ( this->GetP2RootNode()->ns.c_str(), legacyPropName );

	if ( (legacyProp != 0) && legacyProp->IsLeafContentNode() && (! legacyProp->content.empty()) ) {
		const XML_Node * xmlValue = legacyProp->content[0];
		MD5Update ( &md5Context, (XMP_Uns8*)xmlValue->value.c_str(), (unsigned int)xmlValue->value.size() );
	}

}	// P2_MetaHandler::DigestLegacyItem
#define kHexDigits "0123456789ABCDEF"

void P2_SpannedClip::CreateDigest ( std::string * digestStr )
{
	digestStr->erase();
	if ( this->headContent.clipMetadata == 0 ) return;	// Bail if we don't have any legacy XML.

	XML_NodePtr p2RootNode = this->GetP2RootNode(); // Return if there is no root node.
	if( p2RootNode == 0 ) return;
	XMP_StringPtr p2NS = p2RootNode->ns.c_str();
	XML_NodePtr legacyContext;
	MD5_CTX    md5Context;
	unsigned char digestBin [16];
	MD5Init ( &md5Context );
	
	MD5Update ( &md5Context, (XMP_Uns8*)this->GetClipTitle().c_str(), (unsigned int)this->GetClipTitle().size() );
	if ( headContent.clipId )
		MD5Update ( &md5Context, (XMP_Uns8*)headContent.clipId->c_str(), (unsigned int)headContent.clipId->size() );

	XMP_Uns32 totalDuration=this->GetDuration();
	std::ostringstream ostr;
	ostr << totalDuration;
	if ( totalDuration )
		MD5Update ( &md5Context, (XMP_Uns8*)ostr.str().c_str(), (unsigned int)ostr.str().size() );
	if ( headContent.scaleUnit )
		MD5Update ( &md5Context, (XMP_Uns8*)headContent.scaleUnit->c_str(), (unsigned int)headContent.scaleUnit->size() );

	if ( headContent.shotId )
		MD5Update ( &md5Context, (XMP_Uns8*)headContent.shotId->c_str(), (unsigned int)headContent.shotId->size() );
	if ( headContent.topClipId )
		MD5Update ( &md5Context, (XMP_Uns8*)headContent.topClipId->c_str(), (unsigned int)headContent.topClipId->size() );
	if ( headContent.prevClipId )
		MD5Update ( &md5Context, (XMP_Uns8*)headContent.prevClipId->c_str(), (unsigned int)headContent.prevClipId->size() );
	if ( headContent.nextClipId )
		MD5Update ( &md5Context, (XMP_Uns8*)headContent.nextClipId->c_str(), (unsigned int)headContent.nextClipId->size() );

	if ( this->headContent.essenceList != 0 ) {

		XML_NodePtr videoContext = this->headContent.essenceList->GetNamedElement ( p2NS, "Video" );

		if ( videoContext != 0 ) {
			this->DigestElement ( md5Context, videoContext, "AspectRatio" );
			this->DigestElement ( md5Context, videoContext, "Codec" );
			this->DigestElement ( md5Context, videoContext, "FrameRate" );
			this->DigestElement ( md5Context, videoContext, "StartTimecode" );
		}

		XML_NodePtr audioContext = this->headContent.essenceList->GetNamedElement ( p2NS, "Audio" );

		if ( audioContext != 0 ) {
			this->DigestElement ( md5Context, audioContext, "SamplingRate" );
			this->DigestElement ( md5Context, audioContext, "BitsPerSample" );
		}

	}

	legacyContext = this->headContent.clipMetadata;
	this->DigestElement ( md5Context, legacyContext, "UserClipName" );
	this->DigestElement ( md5Context, legacyContext, "ShotMark" );

	legacyContext = this->headContent.clipMetadata->GetNamedElement ( p2NS, "Access" );
	/* Rather return than create the digest because the "Access" element is listed as "required" in the P2 spec.
	So a P2 file without an "Access" element does not follow the spec and might be corrupt.*/
	if ( legacyContext == 0 ) return;

	this->DigestElement ( md5Context, legacyContext, "Creator" );
	this->DigestElement ( md5Context, legacyContext, "CreationDate" );
	this->DigestElement ( md5Context, legacyContext, "LastUpdateDate" );

	legacyContext = this->headContent.clipMetadata->GetNamedElement ( p2NS, "Shoot" );

	if ( legacyContext != 0 ) {
		this->DigestElement ( md5Context, legacyContext, "Shooter" );

		legacyContext = legacyContext->GetNamedElement ( p2NS, "Location" );

		if ( legacyContext != 0 ) {
			this->DigestElement ( md5Context, legacyContext, "PlaceName" );
			this->DigestElement ( md5Context, legacyContext, "Longitude" );
			this->DigestElement ( md5Context, legacyContext, "Latitude" );
			this->DigestElement ( md5Context, legacyContext, "Altitude" );
		}
	}

	legacyContext = this->headContent.clipMetadata->GetNamedElement ( p2NS, "Scenario" );

	if ( legacyContext != 0 ) {
		this->DigestElement ( md5Context, legacyContext, "SceneNo." );
		this->DigestElement ( md5Context, legacyContext, "TakeNo." );
	}

	legacyContext = this->headContent.clipMetadata->GetNamedElement ( p2NS, "Device" );

	if ( legacyContext != 0 ) {
		this->DigestElement ( md5Context, legacyContext, "Manufacturer" );
		this->DigestElement ( md5Context, legacyContext, "SerialNo." );
		this->DigestElement ( md5Context, legacyContext, "ModelName" );
	}

	MD5Final ( digestBin, &md5Context );

	char buffer [40];
	for ( int in = 0, out = 0; in < 16; in += 1, out += 2 ) {
		XMP_Uns8 byte = digestBin[in];
		buffer[out]   = kHexDigits [ byte >> 4 ];
		buffer[out+1] = kHexDigits [ byte & 0xF ];
	}
	buffer[32] = 0;
	digestStr->append ( buffer );

}

P2_SpannedClip::~P2_SpannedClip()
{
	RelatedP2ClipList::iterator iter = spannedP2Clip.begin();
	for(;iter!=spannedP2Clip.end();iter++)
	{
		if (GetClipPath() != (*iter)->GetClipPath())
			delete *iter;
	}
	spannedP2Clip.clear();
}

P2_Clip* P2_SpannedClip::TopP2Clip()
{
	if ( this->IsComplete() && spannedP2Clip.size() > 1 )
	{
		return *spannedP2Clip.begin();
	}
	return this;	
}

XMP_Uns32 P2_SpannedClip::GetDuration()
{	
	if ( IsComplete() )
	{
		RelatedP2ClipList::iterator iter = this->spannedP2Clip.begin();
		XMP_Uns32 totalDuration=0;
		for(;iter!=spannedP2Clip.end();iter++)
			totalDuration+=(*iter)->GetDuration();
		return totalDuration;
	}
	return P2_Clip::GetDuration();
}

void P2_SpannedClip::GetAllClipNames(std::vector <std::string> & clipNameList)
{
	clipNameList.clear();
	if ( IsComplete() )
	{
		RelatedP2ClipList::iterator iter = this->spannedP2Clip.begin();
		for(;iter!=spannedP2Clip.end();iter++)
       clipNameList.push_back(*( (*iter)->GetClipName() ) );
	}else
	{
		clipNameList.push_back(*( this->GetClipName() ) );
	}
}

P2_Manager::P2_Manager():spannedClips(0)
{

}

P2_Manager::~P2_Manager()
{
	delete this->spannedClips;
	this->spannedClips    = 0;
}

void P2_Manager::ProcessClip(std::string & clipPath)
{
	this->spannedClips = new P2_SpannedClip(clipPath);
	if ( this->spannedClips->IsSpannedClip())
	{
		std::string clipFolder,filename,regExp;
		XMP_StringVector  clipFileList,regExpVec;
		clipFolder=clipPath;
		XIO::SplitLeafName ( &clipFolder, &filename );
		regExp = "^\\d\\d\\d\\d\\d\\d.XML$";
		regExpVec.push_back ( regExp );
		regExp = "^\\d\\d\\d\\d\\W\\W.XML$";
		regExpVec.push_back ( regExp );
		regExp = "^\\d\\d\\d\\d\\d\\W.XML$";
		regExpVec.push_back ( regExp );
		regExp = "^\\d\\d\\d\\d\\W\\d.XML$";
		regExpVec.push_back ( regExp );
		IOUtils::GetMatchingChildren ( clipFileList,  clipFolder, regExpVec, false, true, true );
		for(XMP_StringVector::iterator iter=clipFileList.begin();
			iter!=clipFileList.end();iter++)
		{ 
			P2_Clip * tempClip= new P2_Clip(*iter);
			if ( ! spannedClips->AddIfRelated(tempClip) )
				delete tempClip;
		}
		spannedClips->checkSpannedClipIsComplete();
	}
}

bool P2_Manager::IsValidP2()
{
	return spannedClips!= 0;
}

P2_Clip* P2_Manager::GetManagedClip()
{
	return spannedClips->TopP2Clip();
}
	
P2_SpannedClip* P2_Manager::GetSpannedClip()
{
	return spannedClips;
}

