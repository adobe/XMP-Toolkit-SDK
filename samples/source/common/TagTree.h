// =================================================================================================
// Copyright 2008 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
//
// =================================================================================================

#ifndef TagTree_H
#define TagTree_H

#include <map>
#include <list>
#include <string>

#include "source/EndianUtils.hpp"
#include "public/include/XMP_Const.h" //needed for setKV convenience functions

#include "samples/source/common/globals.h"
#include "samples/source/common/Log.h"
#include "samples/source/common/LargeFileAccess.hpp"
void LFA_Throw ( const char* msg, int id );

using namespace std;

// TagTree routines must be able to throw these..
class DumpFileException : public std::runtime_error {
	public:
		DumpFileException(const char* format, ...);

		const char* what_dumpfile_reason(); //no override, but almost..
	private:
		static const unsigned int DUMPFILE_MAX_ERROR_LENGTH=2048;
		char buffer [DUMPFILE_MAX_ERROR_LENGTH+1];
};

class TagTree {
private:
	struct Node; //forward-looking declaration
	typedef std::list<Node> NodeList;
	typedef std::list<Node>::iterator NodeListIter;

	struct Node {
		std::string key;		// node structure name resp. tag name
		std::string value;		// value if applicable/of relevance
		std::string comment;	// comment string, add-on as many as you like
		NodeList children;		// children of this tag, if any
	public:
		//default-constructor is an std::container requirement. DO NOT ACTUALLY USE.
		Node() {
			key=std::string("unnamed"); // should be overridden at all times
			value=std::string("no value");
			comment=std::string("no comment");
			children.clear();			// be safe (std::mac-issue..) clear beforehand
		}

		//the one to use
		Node(std::string _key, std::string _value, std::string _comment) {
			this->key = _key;
			this->value = _value;
			this->comment = _comment;
			children.clear();
		}
	};

	// the map (always alphabetic) to collect key-value pairs
	// - Node* rather than string to have access to value and comment info
	typedef std::map<std::string,Node*> TagMap;
	TagMap tagMap;

	//used for changeValue and addComment
	//(NB: not null-ed or such on push+pop, thus stretches beyond)
	Node* lastNode;

	//we need a stack to iterate in and out
	// during build-up and dump recursion
	typedef std::list<Node*> NodeStack;

	NodeStack nodeStack;
	Node rootNode; //TODO: ("root","");

	// control verbosity to ease debugging:
	static bool verbose;

public:
	TagTree();
	~TagTree();

	void reset();

	// verbosity control (mute by default ) ===================================
	void setMute();
	void setVerbose();

	//input functions =========================================================

	void pushNode(const std::string key);
	void pushNode(const char* format, ...);

	// add file offset as comment -> own routine to better output 64 bit offsets...
	void addOffset(LFA_FileRef file);

	void popNode();
	void popAllNodes();

	//sets a key-value pair and optinal comment. value is also optional and may be set at a later time
	//can also be used to set pure, standalone comments (using key==value=="")
	void setKeyValue(const std::string key,const std::string value="", const std::string comment="");

	//updates the value of key without creating new key, value pairs.
	void updateKeyValue ( const std::string key, const std::string value, const std::string comment = "" );
	
	// convenience functions //////////////////////////////////////////////////////////////////
	// these functions read bytes (assert in file-length), dump them to screen (as hex or as number)
	// and optionally return the values for further processing

	//read, convert endianess, dump certain values all in one go:
	// * key - may be NULL if you just want to obtain the values but not make a KV entry
	// * returnValue - returns the bytes digested
	// * numOfBytes - 0-byte requests *are* legitimate, as they may reduce codeforks for the client
	void digest(LFA_FileRef file,const std::string key="",
							void* returnValue=NULL,
							XMP_Int32 numOfBytes=0);

	////////////////////////////////////////////////////////////////////////////////////
	// numeric digest routines
	//
	// same parameters as above, plus:
	// * bigEndian -  set to false, if the number is in the file as little endian
	//                ( correct return value according to machine type is taken care of for either setting)
	// overload signed and unsigned, 32 and 16 bit
	// Note, again: key may be NULL if you just want to obtain the values but not make a KV entry
	XMP_Int64 digest64s(LFA_FileRef file,const std::string key="", bool BigEndian=false);
	XMP_Uns64 digest64u(LFA_FileRef file,const std::string key="", bool BigEndian=false,bool hexDisplay=false);
	XMP_Int32 digest32s(LFA_FileRef file,const std::string key="", bool BigEndian=false);
	XMP_Uns32 digest32u(LFA_FileRef file,const std::string key="", bool BigEndian=false,bool hexDisplay=false);
	XMP_Int16 digest16s(LFA_FileRef file,const std::string key="", bool BigEndian=false);
	XMP_Uns16 digest16u(LFA_FileRef file,const std::string key="", bool BigEndian=false,bool hexDisplay=false);

	// "expected" Overrides
	void digest64s(XMP_Int64 expected, LFA_FileRef file,const std::string key="", bool BigEndian=false);
	void digest64u(XMP_Uns64 expected, LFA_FileRef file,const std::string key="", bool BigEndian=false,bool hexDisplay=false);
	void digest32s(XMP_Int32 expected, LFA_FileRef file,const std::string key="", bool BigEndian=false);
	void digest32u(XMP_Uns32 expected, LFA_FileRef file,const std::string key="", bool BigEndian=false,bool hexDisplay=false);
	void digest16s(XMP_Int16 expected, LFA_FileRef file,const std::string key="", bool BigEndian=false);
	void digest16u(XMP_Uns16 expected, LFA_FileRef file,const std::string key="", bool BigEndian=false,bool hexDisplay=false);

	//CBR Overrides
	void digest64s(XMP_Int64* returnValue, LFA_FileRef file,const std::string key="", bool BigEndian=false);
	void digest64u(XMP_Uns64* returnValue, LFA_FileRef file,const std::string key="", bool BigEndian=false,bool hexDisplay=false);
	void digest32s(XMP_Int32* returnValue, LFA_FileRef file,const std::string key="", bool BigEndian=false);
	void digest32u(XMP_Uns32* returnValue, LFA_FileRef file,const std::string key="", bool BigEndian=false,bool hexDisplay=false);
	void digest16s(XMP_Int16* returnValue, LFA_FileRef file,const std::string key="", bool BigEndian=false);
	void digest16u(XMP_Uns16* returnValue, LFA_FileRef file,const std::string key="", bool BigEndian=false,bool hexDisplay=false);


	//8-bit string (whichever encoding -> "buginese") to std::string
	//use length==0 to indicate zero-termination,
	//otherwise indicated lenght will be grabbed also accross \0 's
	//(length is counted w/o trailing zero termination, i.e. length("hans")==4 )
	// TODO: length default = 0 not yet implemented
	// verifyZeroTerm
	//  - has an effect only if a length!=0 is given (otherwise things go up to the 0 anyway)
	//  - in this case, asserts that the string has a terminating zero 
	//   (_after_ <length> bytes, otherwise that byte is not read which has an impact on the filepointer!)
	//    would throw if any zero (\0) is encountered prior to that
	std::string digestString(LFA_FileRef file,const std::string key="", size_t length=0, bool verifyZeroTerm=false, bool allowEarlyZeroTerm=false );

	// (wrappers)
	// standalone comment
	void comment(const std::string comment);
	// standalone comment
	// be aware of bug1741056, feeding 64bit numbers might not output correctly
	void comment(const char* format, ...);

	//sometimes its worth changing (or actually setting for the first time) they current
	//(aka last set) key/value at a later time. includes correction in tagmap.
	void changeValue(const std::string value);
	void changeValue(const char* format, ...);

	//adds a comment to last prior entry ( which could be KeyValue, Node or standalone comment...)
	void addComment(const std::string comment);
	//adds a comment to last prior entry ( which could be KeyValue, Node or standalone comment...)
	void addComment(const char* format, ...);

	//output functions ===========================================================
	void dumpTree(bool commentsFlag=true, Node* pNode = NULL,unsigned int depth=0);

	void dumpTagMap();
	void dumpTagList(Node* pNode = NULL,unsigned int depth=0);

	std::string getValue(const std::string key);
	std::string getComment(const std::string key);
	unsigned int getSubNodePos( const std::string nodeKey, const std::string parentKey = "", int skip = 0 );
	XMP_Int64 getNodeSize( const std::string nodeKey );

	//returns true if there is such a node, false if not, error if it happens to be key-value pair
	bool hasNode(const std::string key);
	XMP_Int32 getNodeCount(const std::string key);
};

#endif
