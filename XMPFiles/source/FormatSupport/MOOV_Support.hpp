#ifndef __MOOV_Support_hpp__
#define __MOOV_Support_hpp__	1

// =================================================================================================
// Copyright Adobe
// Copyright 2009 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! This must be the first include.
#include "public/include/XMP_Const.h"

#include "source/EndianUtils.hpp"

#include "XMPFiles/source/XMPFiles_Impl.hpp"
#include "XMPFiles/source/FormatSupport/ISOBaseMedia_Support.hpp"
#include <vector>

#define TopBoxSizeLimit 100*1024*1024

// =================================================================================================
// MOOV_Manager
// ============

class MOOV_Manager : public ISOBaseMedia_Manager {
public:

	// ---------------------------------------------------------------------------------------------
	// Types and constants

	enum {	// Values for fileMode.
		kFileIsNormalISO = 0,		// A "normal" MPEG-4 file, no 'qt  ' compatible brand.
		kFileIsModernQT  = 1,		// Has an 'ftyp' box and 'qt  ' compatible brand.
		kFileIsTraditionalQT = 2	// Old QuickTime, no 'ftyp' box.
	};

	

	// ---------------------------------------------------------------------------------------------
	// GetBox - Pick a box given a '/' separated list of box types. Picks the 1st of each type.

	BoxRef GetBox ( const char * boxPath, BoxInfo * info ) const;
	

	// ---------------------------------------------------------------------------------------------
	// NoteChange - Note overall change, value was directly replaced.
	// SetBox(path) - Like above, but creating path to the box if necessary.

	void SetBox ( const char * boxPath, const void* dataPtr, XMP_Uns32 size , const XMP_Uns8 * idUUID = 0 );


	// ---------------------------------------------------------------------------------------------


	void ParseMemoryTree ( XMP_Uns8 fileMode );
	void UpdateMemoryTree();

	// ---------------------------------------------------------------------------------------------

	#pragma pack (push, 1)	// ! These must match the file layout!

	struct Content_mvhd_0 {
		XMP_Uns32 vFlags;			//   0
		XMP_Uns32 creationTime;		//   4
		XMP_Uns32 modificationTime;	//   8
		XMP_Uns32 timescale;		//  12
		XMP_Uns32 duration;			//  16
		XMP_Int32 rate;				//  20
		XMP_Int16 volume;			//  24
		XMP_Uns16 pad_1;			//  26
		XMP_Uns32 pad_2, pad_3;		//  28
		XMP_Int32 matrix [9];		//  36
		XMP_Uns32 preDef [6];		//  72
		XMP_Uns32 nextTrackID;		//  96
	};								// 100

	struct Content_mvhd_1 {
		XMP_Uns32 vFlags;			//   0
		XMP_Uns64 creationTime;		//   4
		XMP_Uns64 modificationTime;	//  12
		XMP_Uns32 timescale;		//  20
		XMP_Uns64 duration;			//  24
		XMP_Int32 rate;				//  32
		XMP_Int16 volume;			//  36
		XMP_Uns16 pad_1;			//  38
		XMP_Uns32 pad_2, pad_3;		//  40
		XMP_Int32 matrix [9];		//  48
		XMP_Uns32 preDef [6];		//  84
		XMP_Uns32 nextTrackID;		// 108
	};								// 112


	struct Content_stsd_entry {
		XMP_Uns32 entrySize;		//  0
		XMP_Uns32 format;			//  4
		XMP_Uns8  reserved_1 [6];	//  8
		XMP_Uns16 dataRefIndex;		// 14
		XMP_Uns32 reserved_2;		// 16
		XMP_Uns32 flags;			// 20
		XMP_Uns32 timeScale;		// 24
		XMP_Uns32 frameDuration;	// 28
		XMP_Uns8  frameCount;		// 32
		XMP_Uns8  reserved_3;		// 33
		// Plus optional trailing ISO boxes.
	};								// 34

	struct Content_stsc_entry {
		XMP_Uns32 firstChunkNumber;	//  0
		XMP_Uns32 samplesPerChunk;	//  4
		XMP_Uns32 sampleDescrID;	//  8
	};								// 12

	#pragma pack( pop )

#if SUNOS_SPARC || XMP_IOS_ARM || XMP_ANDROID_ARM
	#pragma pack( )
#endif //#if SUNOS_SPARC || XMP_IOS_ARM || XMP_ANDROID_ARM

	// ---------------------------------------------------------------------------------------------

	MOOV_Manager() : fileMode(0)
	{
		XMP_Assert ( sizeof ( Content_mvhd_0 ) == 100 );	// Make sure the structs really are packed.
		XMP_Assert ( sizeof ( Content_mvhd_1 ) == 112 );
		XMP_Assert ( sizeof ( Content_hdlr ) == 24 );
		XMP_Assert ( sizeof ( Content_stsd_entry ) == 34 );
		XMP_Assert ( sizeof ( Content_stsc_entry ) == 12 );
	};

	virtual ~MOOV_Manager() {};

private:

	
	XMP_Uns8 fileMode;
	
	void ParseNestedBoxes ( BoxNode * parentNode, const std::string & parentPath, bool ignoreMetaBoxes );

	XMP_Uns32  NewSubtreeSize ( const BoxNode & node, const std::string & parentPath );
	XMP_Uns8 * AppendNewSubtree ( const BoxNode & node, const std::string & parentPath,
										 XMP_Uns8 * newPtr, XMP_Uns8 * newEnd );

};	// MOOV_Manager

#endif	// __MOOV_Support_hpp__
