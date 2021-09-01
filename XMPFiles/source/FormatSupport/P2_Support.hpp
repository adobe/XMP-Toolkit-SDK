#ifndef __P2_Support_hpp__
#define __P2_Support_hpp__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "public/include/XMP_Environment.h"	
#include "public/include/XMP_Const.h"

#include "XMPFiles/source/XMPFiles_Impl.hpp"
#include "source/XIO.hpp"
#include "third-party/zuid/interfaces/MD5.h"
#include <set>

class P2_Clip {
public:
	P2_Clip(const std::string & p2ClipMetadataFilePath);
	bool IsSpannedClip() ;
	bool IsTopClip() ;
	bool IsValidClip() ;
	virtual void CreateDigest ( std::string * digestStr );
	XMP_Uns32 GetOffsetInShot();
	XMP_Uns32 GetDuration();
	std::string* GetClipName();
	std::string GetClipTitle();
	std::string* GetClipId();
	std::string* GetNextClipId();
	std::string* GetPreviousClipId();
	std::string* GetTopClipId();
	std::string* GetShotId();
	std::string* GetEditUnit();
	std::string GetClipPath(){return filePath;}
	virtual std::string GetXMPFilePath();
	XML_NodePtr GetClipContentNode();
	XML_NodePtr GetClipMetadataNode();
	XML_NodePtr GetEssenceListNode();
	XML_NodePtr GetP2RootNode() ;
	void  SerializeP2ClipContent(std::string& xmlContentData) ;
	virtual ~P2_Clip();
protected:
	class ClipContent
	{ 
	public:
		ClipContent():clipId(0),scaleUnit(0),
			duration(0),OffsetInShot(0),topClipId(0),nextClipId(0),
			prevClipId(0),shotId(0),clipMetadata(0),essenceList(0),clipTitle(0){}
		std::string* clipTitle;
		std::string* clipId;
		std::string* scaleUnit;
		XMP_Uns32    duration;
		XMP_Uns32    OffsetInShot;
		std::string* topClipId;
		std::string* nextClipId;
		std::string* prevClipId;
		std::string* shotId;
		XML_NodePtr  clipMetadata;
		XML_NodePtr  essenceList;
		void reset(){*this=ClipContent();}
	};
	ClipContent    headContent;
private:
	void DestroyExpatParser();
	void PrepareForExpatParser(const std::string & p2ClipMetadataFilePath);
	void CreateExpatParser(XMPFiles_IO &xmlFile);
	void CacheClipContent();

	bool headContentCached;
	ExpatAdapter * p2XMLParser;
	XML_NodePtr    p2Root;
	XML_NodePtr    p2ClipContent;
	std::string    filePath;
	std::string    clipName;

}; // class P2_Clip
struct P2SpannedClip_Order
{
	bool operator()( P2_Clip* lhs,  P2_Clip* rhs) const  
	{  
		return lhs->GetOffsetInShot() < rhs->GetOffsetInShot();
	}

};

class P2_SpannedClip : public  P2_Clip{
public:
	P2_SpannedClip(const std::string & p2ClipMetadataFilePath);
	bool AddIfRelated(P2_Clip* openedClip);
	bool IsComplete();
	void checkSpannedClipIsComplete();
	XMP_Uns32 GetDuration();
	P2_Clip* TopP2Clip() ;
	std::string GetXMPFilePath();
	void CreateDigest ( std::string * digestStr );
	void GetAllClipNames(std::vector <std::string> & clipNameList);
	virtual ~P2_SpannedClip();
private:
	P2_SpannedClip(const P2_SpannedClip &);
	P2_SpannedClip operator=(const P2_SpannedClip &);

	void DigestElement( MD5_CTX & md5Context, XML_NodePtr legacyContext, XMP_StringPtr legacyPropName );

	typedef std::multiset<P2_Clip*,P2SpannedClip_Order> RelatedP2ClipList;
	std::set<std::string> addedClipIds;
	RelatedP2ClipList spannedP2Clip;
	bool completeSpannedClip;

}; // class P2_SpannedClip

// =================================================================================================
class P2_Manager {
public:
	P2_Manager();
	void ProcessClip(std::string & clipPath);
	P2_Clip* GetManagedClip();
	P2_SpannedClip* GetSpannedClip();
	bool IsValidP2();
	~P2_Manager();

private:

	P2_SpannedClip* spannedClips;

}; // class P2_Manager


// =================================================================================================



// =================================================================================================

#endif	// __P2_Support_hpp__
