// =================================================================================================
// Copyright 2008 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
//
// =================================================================================================

/*
* all legacy metadata comes in some hierarchical form
* these routines help generate a tree, which is subsequently parseable (the "classic" dumpImage)

* as well as queriable (primitive but workable solution)
* having direct access to particluar legacy fields through unique keys (like "psir424")
* enables efficient testing. tagMap is used for that.
*
* if keys are defined multiple times, the are named sameKey, sameKey-2, sameKey-3, sameKey-4
* allowing also specific queries on those
*
*/

#include "samples/source/common/TagTree.h"
#include <stdarg.h>

// silent by default
bool TagTree::verbose = false;

#if WIN_ENV
	//should preferably be within a #if XMP_WinBuild, not doable here... 
	#pragma warning ( disable : 4996 )	// 'sprintf' was declared deprecated

	#include <stdio.h>
	#ifndef snprintf
		#define snprintf _xmp_snprintf
		#pragma warning ( disable : 4996 )	// Consider using _snprintf_s instead.

		void _xmp_snprintf(char * buffer,size_t len,const char * format, ...)
		{
		va_list args; va_start(args, format);
		   _snprintf( buffer, len, format, args); 
				// using bad windows routine from stdio.h
				// and indeed named, _snprintf, no such thing as snprintf
		   buffer[len-1] = 0; //write that trailing zero..
		va_end(args);
		}
	#endif
#endif

//private - duplicate of Utils routine
std::string itos(int i)
{
	char cs[31]; //worst to come is 64 bit <=> 21 bytes (more likely will be 32 bit forever)
	snprintf(cs,30,"%d",i); //snprintf does buffer overrun check which sprintf doesn't
	return std::string(cs);
}

TagTree::TagTree()
{
	rootNode.key="rootkey";		//all never seen
	rootNode.value="rootvalue";
	rootNode.comment="rootcomment";
	reset();
}

TagTree::~TagTree()
{

}

void TagTree::reset()
{
	tagMap.clear();
	nodeStack.clear();
	rootNode.children.clear();
	nodeStack.push_back(&rootNode);
	lastNode = NULL;
}


// verbosity control:
void TagTree::setMute()
{
	TagTree::verbose = false;
}

void TagTree::setVerbose()
{
	TagTree::verbose = true;
}


void TagTree::setKeyValue(const std::string key,const std::string value, const std::string _comment)
{
	Node* pCurNode=*nodeStack.rbegin(); //current Node
	pCurNode->children.push_back(Node(key,value, _comment));

	if(key.size()==0) {		 // standalone comment?
		if (value.size()!=0) // must have no value
			Log::error("no key but value found.");
		return;				// ==> do not add to tag-map
	}

	//find first free foo, foo-2, foo-3 (note: no foo-1)
	int i=1;
	std::string extkey=key;
	while( tagMap.count(extkey) )
	{
		i++;
		extkey = key + "-" + itos(i);
	}

	//add to Map -----------------------------------
	lastNode=&*(pCurNode->children.rbegin());
	tagMap[extkey]=lastNode;

	if ( verbose )
		Log::info( "    setKeyValue( %s |-> %s) [%s]", key.c_str(), value.c_str(), _comment.c_str() );
}

void TagTree::updateKeyValue ( const std::string key, const std::string value, const std::string _comment )
{
	Node* pCurNode = *nodeStack.rbegin ( ); //current Node
	pCurNode->children.push_back ( Node ( key, value, _comment ) );

	if ( key.size ( ) == 0 ) {		 // standalone comment?
		if ( value.size ( ) != 0 ) // must have no value
			Log::error ( "no key but value found." );
		return;				// ==> do not add to tag-map
	}

	//add to Map -----------------------------------
	lastNode = &*(pCurNode->children.rbegin ( ));
	tagMap[key] = lastNode;

	if ( verbose )
		Log::info ( "    setKeyValue( %s |-> %s) [%s]", key.c_str ( ), value.c_str ( ), _comment.c_str ( ) );
}

void TagTree::digest(LFA_FileRef file,const std::string key /*=NULL*/,
					   void* returnValue /*=""*/,
					   XMP_Int32 numOfBytes /*=0*/ )
{
	if (numOfBytes==0) {
		//0-byte requests *are* legitimate, reducing codeforks for the caller
		if ( !key.empty() )
			setKeyValue(key,"(0 bytes)");
		return;
	}

	//do we need own space or will it be provided?
	char* value;
	if (returnValue)
		value=(char*)returnValue;
	else
		value=new char[numOfBytes+1];

										// require all == false => leave the throwing to this routine
	if (numOfBytes != LFA_Read ( file, value, numOfBytes, false))	// saying 1,4 guarantes read as ordered (4,1 would not)
		Log::error("could not read %d number of files (End of File reached?)",numOfBytes);
#if !IOS_ENV && !WIN_UNIVERSAL_ENV
	char* out=new char[2 + numOfBytes*3 + 5]; //'0x12 34 45 78 '   length formula: 2 ("0x") + numOfBytes x 3 + 5 (padding)
	if (!key.empty()) {
		snprintf(out,3,"0x");
		XMP_Int64 i; // *)
		for (i = 0; i < numOfBytes; i++)
		{
			snprintf(&out[2 + i * 3], 4, "%.2X ", value[i]); //always must allow that extra 0-byte on mac (overwritten again and again)
		}
		snprintf(&out[2+i*3],1,"%c",'\0'); // *) using i one more time (needed while bug 1613297 regarding snprintf not fixed)
		setKeyValue(key,out);
	}
#else
    char* out=new char[2 + numOfBytes*9 + 5]; //'0x12 34 45 78 '   length formula: 2 ("0x") + numOfBytes x 3 + 5 (padding)
	if (!key.empty()) {
		snprintf(out,3,"0x");
		XMP_Int64 i; // *)
		for (i=0; i < numOfBytes; i++)
			snprintf(&out[2+i*9],10,"%.8X ",value[i]); //always must allow that extra 0-byte on mac (overwritten again and again)
		snprintf(&out[2+i*9],1,"%c",'\0'); // *) using i one more time (needed while bug 1613297 regarding snprintf not fixed)
		setKeyValue(key,out);
	}

#endif
    delete[] out;
	if (!returnValue) delete value; //if we own it, we delete it
}

////////////////////////////////////////////////////////////////////////////////////
// numeric digest routines
//
XMP_Int64 TagTree::digest64s(LFA_FileRef file,const std::string key /* ="" */ , bool BigEndian /*=false*/ )
{
	XMP_Int64 r;
	if (8 != LFA_Read ( file, &r, 8, false)) // require all == false => leave the throwing to this routine
		Log::error("could not read 8-byte value from file (end of file?)");
	if ( ((kBigEndianHost==1) &&  !BigEndian ) ||  ((kBigEndianHost==0) && BigEndian ))  // "XOR"
		Flip8(&r);

	if (!key.empty()) {
		char out[25]; //longest is "18446744073709551615", 21 chars ==> 25
		snprintf(out,24,"%lld",r); //signed, mind the trailing \0 on Mac btw
		setKeyValue(key,out);
	}
	return r;
}

XMP_Uns64 TagTree::digest64u(LFA_FileRef file,const std::string key /* ="" */, bool BigEndian /*=false*/,bool hexDisplay /*=false*/ )
{
	XMP_Uns64 r;
	if (8 != LFA_Read ( file, &r, 8, false)) // require all == false => leave the throwing to this routine
		Log::error("could not read 8-byte value from file (end of file?)");
	if ( ((kBigEndianHost==1) &&  !BigEndian ) ||  ((kBigEndianHost==0) && BigEndian ))  // "XOR"
		Flip8(&r);
	if (!key.empty()) {
		char out[25]; //largets 64 bit no: 18446744073709551616 -1 (20 digits)
		if (!hexDisplay)
		{
			//not working, 0x1244e7780 ==> 609122176 decimal (== 0x244e7780)
			#if WIN_ENV
				snprintf(out , 24 , "%I64u" , r);
			#else 
				// MAC, UNIX
				snprintf(out , 24 , "%llu" , r);
			#endif
		}
		else
		{	
			//not working, upper 32 bit empty:  			
			#if WIN_ENV
				snprintf( out , 24 , "0x%.16I64X" , r );
			#else
				snprintf( out , 24 , "0x%.16llX" , r );
			#endif
		}
		setKeyValue(key,out);
	}
	return r;
}


XMP_Int32 TagTree::digest32s(LFA_FileRef file,const std::string key /* ="" */ , bool BigEndian /*=false*/ )
{
	XMP_Int32 r;
	if (4 != LFA_Read ( file, &r, 4, false)) // require all == false => leave the throwing to this routine
		Log::error("could not read 4-byte value from file (end of file?)");
	if ( ((kBigEndianHost==1) &&  !BigEndian ) ||  ((kBigEndianHost==0) && BigEndian ))  // "XOR"
		Flip4(&r);
	if (!key.empty()) {
		char out[15]; //longest signed int is "-2147483648", 11 chars 	
		snprintf(out,14,"%d",r); //signed, mind the trailing \0 on Mac btw
		setKeyValue(key,out);
	}
	return r;
}

XMP_Uns32 TagTree::digest32u(LFA_FileRef file,const std::string key /* ="" */, bool BigEndian /*=false*/,bool hexDisplay /*=false*/ )
{
	XMP_Uns32 r;
	if (4 != LFA_Read ( file, &r, 4, false)) // require all == false => leave the throwing to this routine
		Log::error("could not read 4-byte value from file (end of file?)");
	if ( ((kBigEndianHost==1) &&  !BigEndian ) ||  ((kBigEndianHost==0) && BigEndian ))  // "XOR"
		Flip4(&r);
	if (!key.empty()) {
		char out[19]; //longest unsigned int is "2147483648", 10 chars resp. 0xFFFFFFFF 10 chars
		if (!hexDisplay)
			snprintf(out,18,"%u",r); //unsigned, mind the trailing \0 on Mac btw
		else
			snprintf(out,18,"0x%.8X",r); //unsigned, mind the trailing \0 on Mac btw
		setKeyValue(key,out);
	}
	return r;
}
//////////////////
XMP_Int16 TagTree::digest16s(LFA_FileRef file,const std::string key /* ="" */ , bool BigEndian /*=false*/ )
{
	XMP_Int16 r;
	if (2 != LFA_Read ( file, &r, 2, false)) // require all == false => leave the throwing to this routine
		Log::error("could not read 2-byte value from file (end of file?)");
	if ( ((kBigEndianHost==1) &&  !BigEndian ) ||  ((kBigEndianHost==0) && BigEndian ))  // "XOR"
		Flip2(&r);
	if (!key.empty()) {
		char out[10]; //longest signed int is "�32768", 6 chars 	
		snprintf(out,9,"%d",r);
		setKeyValue(key,out);
	}
	return r;
}

XMP_Uns16 TagTree::digest16u(LFA_FileRef file,const std::string key /* ="" */, bool BigEndian /*=false*/,bool hexDisplay /*=false*/ )
{
	XMP_Uns16 r;
	if (2 != LFA_Read ( file, &r, 2, false)) // require all == false => leave the throwing to this routine
		Log::error("could not read 2-byte value from file (end of file?)");
	if ( ((kBigEndianHost==1) &&  !BigEndian ) ||  ((kBigEndianHost==0) && BigEndian ))  // "XOR"
		Flip2(&r);
	if (!key.empty()) {
		char out[15]; //longest unsigned int is "65536", 5 chars resp.  0xFFFF = 6 chars
		if (!hexDisplay)
			snprintf(out,14,"%u",r);
		else
			snprintf(out,14,"0x%.4X",r);
		setKeyValue(key,out);
	}
	return r;
}


//////////////////////////////////////////////////////////////////////////////////////////
// "expected" Overrides
void TagTree::digest64s(XMP_Int64 expected, LFA_FileRef file,const std::string key /*=""*/, bool BigEndian /*=false*/ )
{
	XMP_Int64 tmp=digest64s(file,"",BigEndian);
	if ( expected != tmp )
		throw DumpFileException("'%s' was %d, expected: %d",key.c_str(),tmp,expected);
}

void TagTree::digest64u(XMP_Uns64 expected, LFA_FileRef file,const std::string key /*=""*/, bool BigEndian /*=false*/, bool hexDisplay /*=false*/)
{
	XMP_Uns64 tmp=digest64u( file,"",BigEndian, hexDisplay );
	if (expected != tmp )
	{
		if (hexDisplay)
		{
			throw DumpFileException("'%s' was 0x%.16X, expected: 0x%.16X",key.c_str(),tmp,expected);
		}
		else
		{
			throw DumpFileException("'%s' was %d, expected: %d",key.c_str(),tmp,expected);
		}
	}
}

void TagTree::digest32s(XMP_Int32 expected, LFA_FileRef file,const std::string key /*=""*/, bool BigEndian /*=false*/ )
{
	XMP_Int32 tmp=digest32s(file,"",BigEndian);
	if ( expected != tmp )
		throw DumpFileException("'%s' was %d, expected: %d",key.c_str(),tmp,expected);
}

void TagTree::digest32u(XMP_Uns32 expected, LFA_FileRef file,const std::string key /*=""*/, bool BigEndian /*=false*/, bool hexDisplay /*=false*/)
{
	XMP_Uns32 tmp=digest32u( file,"",BigEndian, hexDisplay );
	if (expected != tmp )
	{
		if (hexDisplay)
		{
			throw DumpFileException("'%s' was 0x%.8X, expected: 0x%.8X",key.c_str(),tmp,expected);
		}
		else
		{
			throw DumpFileException("'%s' was %d, expected: %d",key.c_str(),tmp,expected);
		}
	}
}

void TagTree::digest16s(XMP_Int16 expected, LFA_FileRef file,const std::string key /*=""*/, bool BigEndian /*=false*/ )
{
	XMP_Int16 tmp=digest16s(file,key,BigEndian);
	if ( expected != tmp )
		throw DumpFileException("'%s' was %d, expected: %d",key.c_str(),tmp,expected);
}

void TagTree::digest16u(XMP_Uns16 expected, LFA_FileRef file,const std::string key /*=""*/, bool BigEndian /*=false*/, bool hexDisplay /*=false*/)
{
	XMP_Uns16 tmp=digest16u( file,key,BigEndian, hexDisplay );
	if (expected != tmp )
	{
		if (hexDisplay)
		{
			throw DumpFileException("'%s' was 0x%.4X, expected: 0x%.4X",key.c_str(),tmp,expected);
		}
		else
		{
			throw DumpFileException("'%s' was %d, expected: %d",key.c_str(),tmp,expected);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// CBR Overrides
void TagTree::digest64s(XMP_Int64* returnValue, LFA_FileRef file,const std::string key /*=""*/, bool BigEndian /*=false*/ )
{
	*returnValue = digest64s(file,key,BigEndian);
}

void TagTree::digest64u(XMP_Uns64* returnValue, LFA_FileRef file,const std::string key /*=""*/, bool BigEndian /*=false*/, bool hexDisplay /*=false*/)
{
	*returnValue = digest64u( file, key,BigEndian, hexDisplay );
}

void TagTree::digest32s(XMP_Int32* returnValue, LFA_FileRef file,const std::string key /*=""*/, bool BigEndian /*=false*/ )
{
	*returnValue = digest32s(file,key,BigEndian);
}

void TagTree::digest32u(XMP_Uns32* returnValue, LFA_FileRef file,const std::string key /*=""*/, bool BigEndian /*=false*/, bool hexDisplay /*=false*/)
{
	*returnValue = digest32u( file, key,BigEndian, hexDisplay );
}

void TagTree::digest16s(XMP_Int16* returnValue, LFA_FileRef file,const std::string key /*=""*/, bool BigEndian /*=false*/ )
{
	*returnValue = digest16s(file,key,BigEndian);
}

void TagTree::digest16u(XMP_Uns16* returnValue, LFA_FileRef file,const std::string key /*=""*/, bool BigEndian /*=false*/, bool hexDisplay /*=false*/)
{
	*returnValue = digest16u( file, key,BigEndian, hexDisplay );
}

//////////////////////////////////////////////////////////////////////////////////////////

std::string TagTree::digestString(LFA_FileRef file,const std::string key /*=""*/, size_t length /* =0 */, bool verifyZeroTerm /* =false */, bool allowEarlyZeroTerm /* =false */ )
{
	std::string r(256,'\0');	//give some room in advance (performance)
	r.clear();					// safety measure (may be needed on mac)

	bool outside = false;	// toggle-flag: outside ASCII

	for ( XMP_Uns32 i = 0; ( i<length ) || (length==0) ; i++ )
	{
		XMP_Uns8 ch = (XMP_Uns8)LFA_GetChar(file);

		// allow early zero termination (useful for fixed length field that may or may not end prematurely)
		if ( allowEarlyZeroTerm && ( ch == 0 ) && ( length != 0 ) )
		{
			i++;
			LFA_Seek( file, length - i, SEEK_CUR ); // compensate for skipped bytes
			break;
		}

		if ( (0x20 <= ch) && (ch <= 0x7E) ) 
		{	//outside-case
			if ( outside )
				r.push_back('>');
			r.push_back(ch);
			outside = false;
		} else {
			if ( (length==0) && (ch == '\0' ) )
				break; // lenght zero => watch for zero termination...
			if ( !outside ) 
				r.push_back('<');	//first inside
			else if (!((length==0) && (ch =='\0')))
				r.push_back(' ');	//further inside (except very last)
			outside = true;
			char tmp[4];
			sprintf(tmp, "%.2X", ch ); 
			r+=tmp;			
		}
	}

	if ( outside ) r.push_back('>'); //last one

	if ( verifyZeroTerm )
	{
		XMP_Uns8 ch = (XMP_Uns8)LFA_GetChar(file);
		if ( ch != 0 )
			Log::error("string for key %s not terminated with zero as requested but with 0x%.2X",key.c_str(),ch);
	}


	if (!key.empty())
		setKeyValue(key,r);

	return r;
}

void TagTree::comment(const std::string _comment)
{
	setKeyValue("","", _comment);
}

void TagTree::comment(const char* format, ...)
{
	char buffer[XMPQE_BUFFERSIZE];
	va_list args;
	va_start(args, format);
		vsprintf(buffer, format, args);
	va_end(args);

	setKeyValue("","",buffer);
}

//adding a subnode to tagMap and current node
//(do "go in", pushes onto nodeStack, making this the current node)
void TagTree::pushNode(const std::string key)
{
	//TODO: adding fromArgs("offset:%s",LFA_Seek( file, offset_CD ,SEEK_SET )); <== requires file to be passed in
	setKeyValue(key,"","");
	//_and_ push reference to that one on stack
	Node* pCurNode=*nodeStack.rbegin();
	nodeStack.push_back( &*pCurNode->children.rbegin() );

	if ( verbose )
		Log::info( "pushing %d: %s",nodeStack.size(), key.c_str() );
}

//formatstring wrapper
void TagTree::pushNode(const char* format, ...)
{
	char buffer[XMPQE_BUFFERSIZE];
	va_list args;
	va_start(args, format);
		vsprintf(buffer, format, args);
	va_end(args);

	pushNode( std::string(buffer) );
}

void TagTree::addOffset(LFA_FileRef file)
{
	// 3 points for doing it here: shortness, convenience, 64bit forks needed 
	#if WIN_ENV
		addComment( "offset: 0x%I64X", LFA_Tell( file ) );
    #elif ANDROID_ENV
		addComment( "offset: 0x%llX", LFA_Tell( file ) );
	#else
		addComment( "offset: 0x%ll.16X", LFA_Tell( file ) );
	#endif
}

//takes a Node off the stack
// - addTag()'s will now go to the prior Node
void TagTree::popNode()
{
	// FOR DEBUGGING Log::info( "pop %d",nodeStack.size() );

	if (nodeStack.size() <= 1)
		Log::error("nodeStack underflow: %d",nodeStack.size());
	nodeStack.pop_back();
}

// takes all Nodes from the stack
// - the right thing to do after a parsing failure to at least dump
//   the partial tree till there
void TagTree::popAllNodes()
{
	while (nodeStack.size() > 1)
		nodeStack.pop_back();
}


void TagTree::changeValue(const std::string value)
{
	if (!lastNode)
		Log::error("lastnode NULL");
	lastNode->value=value;
}

void TagTree::changeValue(const char* format, ...)
{
	char buffer[XMPQE_BUFFERSIZE];
	va_list args;
	va_start(args, format);
		vsprintf(buffer, format, args);
	va_end(args);

	changeValue( std::string(buffer) );
}


void TagTree::addComment(const std::string _comment)
{
	if (!lastNode)
		Log::error("lastnode NULL");
	lastNode->comment=lastNode->comment +
		( (lastNode->comment.size())?",":"") //only add comma, if there already is...
		+ _comment;

	if ( verbose )
		Log::info( "    addComment: %s", _comment.c_str() );
}

void TagTree::addComment(const char* format, ...)
{
	char buffer[4096];
	va_list args;
	va_start(args, format);
		vsprintf(buffer, format, args);
	va_end(args);

	addComment( std::string(buffer) );
}

// ============================================================================
// Output functions
// ============================================================================

// this is essentiall the last function to call...
// - fileDump "end-users" should call it by just saying dumpTree()
// - dumps tree to stdout (by way of Log::info)
// - recursive calls, depth is used for indentation
void TagTree::dumpTree(bool commentsFlag /*true*/,Node* pNode /*null*/ ,unsigned int depth /*=0*/)
{
	if (!pNode) { //NULL --> initial node is rootNode
		//check (only needed on first==outermost call) that push and pop match...
		if (nodeStack.size()>1)
			Log::error("nodeStack not emptied: should be 1 but is %d",nodeStack.size());
		//no need to iterate or dump rootnode itself, 
		for( NodeList::iterator iter = rootNode.children.begin(); iter != rootNode.children.end(); iter++ )
			dumpTree ( commentsFlag,&*iter, depth); //to not iterate on first level
		return;							//...and then finally return
	}

	std::string indent(depth*2,' ');	//read: tab 4

	if (commentsFlag) {
		Log::info( "%s%s%s%s%s%s%s%s%s%s",	//fancy formatting  foo='bar' [re,do]
						indent.c_str(),
						pNode->key.c_str(),
						pNode->value.size()?" = '":"",
						pNode->value.c_str(),
						pNode->value.size()?"'":"",
						( pNode->key.size() + pNode->value.size() > 0 ) ? " ":"",
						// standalong comments don't need extra indentation:
						pNode->comment.size() && ( pNode->key.size() || pNode->value.size() )?
							((std::string("\n    ")+indent).c_str()) : "",
						pNode->comment.size() && ( pNode->key.size() || pNode->value.size() )   ?"[":"", 
							//standalone comments don't need brackets
						pNode->comment.c_str(),
						pNode->comment.size() && ( pNode->key.size() || pNode->value.size() ) ?"]":""
					);	
	} else {
		//if "NoComments" mode, then make sure, there at least is a comment to avoid blankline
		if ( pNode->key.size() || pNode->value.size())
		{
			Log::info( "%s%s%s%s%s",	//fancy formatting  foo='bar' [re,do]
					indent.c_str(),
					pNode->key.c_str(),
					pNode->value.size()?" = '":"",
					pNode->value.c_str(),
					pNode->value.size()?"'":""
				);
		}
	}
	
	//iterate over children  (gracefully covers no-children case)
	for( NodeList::iterator iter = pNode->children.begin(); iter != pNode->children.end(); iter++ )
		dumpTree ( commentsFlag, &*iter, depth+1);
}

void TagTree::dumpTagMap()
{
	for( TagMap::iterator iter = tagMap.begin(); iter != tagMap.end(); iter++ )
	{
		Node* pNode=((*iter).second);
		if (!pNode->children.size())	//supress node with children
			Log::info( "%s%s%s%s",	//(no index string this time)
					pNode->key.c_str(),
					pNode->value.size()?" = '":"",
					pNode->value.c_str(),
					pNode->value.size()?"'":""
				);
	}
}

// shrinked copy of dumpTree (which has faithfull order)
// just no commenting, indenting,...)
void TagTree::dumpTagList(Node* pNode,unsigned int depth /*=0*/)
{
	if (!pNode) { //NULL --> initial node is rootNode
		//check (only needed on first==outermost call) that push and pop match...
		if (nodeStack.size()>1)
			Log::error("nodeStack not emptied: should be 1 but is %d",nodeStack.size());
		//no need to iterate or dump rootnode itself, 
		for( NodeList::iterator iter = rootNode.children.begin(); iter != rootNode.children.end(); iter++ )
			dumpTagList ( &*iter, depth);
		return;
	}

	//make sure, there at least is a comment to avoid blankline
	if ( pNode->key.size() || pNode->value.size())
	{
		Log::info( "%s%s%s%s",	//fancy formatting  foo='bar' [re,do]
				pNode->key.c_str(),
				pNode->value.size()?" = '":"",
				pNode->value.c_str(),
				pNode->value.size()?"'":""
			);
	}
	
	for( NodeList::iterator iter = pNode->children.begin(); iter != pNode->children.end(); iter++ )
		dumpTagList ( &*iter, depth+1);

}

std::string TagTree::getValue(const std::string key)
{
	if ( !hasNode(key) )
		Log::error("key %s does not exist",key.c_str());
	return tagMap[key]->value;
}

std::string TagTree::getComment(const std::string key)
{
	if ( !hasNode(key) )
		Log::error("key %s does not exist",key.c_str());
	return tagMap[key]->comment;
}

unsigned int TagTree::getSubNodePos( const std::string nodeKey, const std::string parentKey, int skip )
{
	Node* parent = NULL;

	if( parentKey.empty() )
	{
		parent = &rootNode.children.front();
	}
	else
	{
		if ( ! hasNode( parentKey ) )
			Log::error( "parent key %s does not exist", parentKey.c_str() );

		parent = tagMap[parentKey];
	}

	unsigned int pos = 1;
	NodeListIter iter;
	for( iter = parent->children.begin(); 
		iter != parent->children.end() && ( !( (iter->key == (parentKey.empty() ? nodeKey : parentKey + nodeKey)) && skip--<=0 )) ;
		iter++, pos++ );

	if( iter == parent->children.end() ) 
		pos = 0;

	return pos;
}


XMP_Int64 TagTree::getNodeSize( const std::string nodeKey )
{
	string tmp = tagMap[nodeKey]->comment;
	size_t startpos = tmp.find( "size" ) + 5;
	if( startpos == string::npos )
		return 0;
	tmp = tmp.substr( startpos, tmp.find_first_of( ",", startpos ) );
	
	return strtol( tmp.c_str(), NULL, 0 );
}


bool TagTree::hasNode(const std::string key)
{
	if ( tagMap.count(key)==0 )
		return false; //no such node
	return true;
}

XMP_Int32 TagTree::getNodeCount(const std::string key)
{
	int count=1;
	std::string extkey=key;
	while( tagMap.count(extkey) )
	{
		count++;
		extkey = key + "-" + itos(count);
	}
	return count-1;
}
