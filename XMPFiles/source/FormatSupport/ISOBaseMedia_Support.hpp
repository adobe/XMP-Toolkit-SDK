#ifndef __ISOBaseMedia_Support_hpp__
#define __ISOBaseMedia_Support_hpp__     1

// =================================================================================================
// Copyright Adobe
// Copyright 2007 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "public/include/XMP_Environment.h"     // ! This must be the first include.

#include "public/include/XMP_Const.h"
#include "public/include/XMP_IO.hpp"

#include "XMPFiles/source/XMPFiles_Impl.hpp"

#include "source/XMLParserAdapter.hpp"
#include "source/ExpatAdapter.hpp"
#include <set>

// =================================================================================================
/// \file ISOBaseMedia_Support.hpp
/// \brief XMPFiles support for the ISO Base Media File Format.
///
/// \note These classes are for use only when directly compiled and linked. They should not be
/// packaged in a DLL by themselves. They do not provide any form of C++ ABI protection.
// =================================================================================================

namespace ISOMedia {

#define ISOBoxList \
	ISOboxType(k_ftyp,0x66747970UL)SEPARATOR /* File header Box, no version/flags.*/ \
	\
	ISOboxType(k_mp41,0x6D703431UL)SEPARATOR /* Compatible brand codes*/             \
	ISOboxType(k_mp42,0x6D703432UL)SEPARATOR \
	ISOboxType(k_f4v ,0x66347620UL)SEPARATOR \
	ISOboxType(k_avc1,0x61766331UL)SEPARATOR \
	ISOboxType(k_qt  ,0x71742020UL)SEPARATOR \
	ISOboxType(k_isom,0x69736F6DUL)SEPARATOR \
	ISOboxType(k_3gp4,0x33677034UL)SEPARATOR \
	ISOboxType(k_3g2a,0x33673261UL)SEPARATOR \
	ISOboxType(k_3g2b,0x33673262UL)SEPARATOR \
	ISOboxType(k_3g2c,0x33673263UL)SEPARATOR \
	ISOboxType(k_mif1,0x6d696631UL)SEPARATOR \
	ISOboxType(k_heic,0x68656963UL)SEPARATOR \
	ISOboxType(k_jpeg,0x6a706567UL)SEPARATOR \
	ISOboxType(k_heix,0x68656978UL)SEPARATOR \
	ISOboxType(k_avci,0x61766369UL)SEPARATOR \
	\
	ISOboxType(k_moov,0x6D6F6F76UL)SEPARATOR /* Container Box, no version/flags. */ \
	ISOboxType(k_mvhd,0x6D766864UL)SEPARATOR /* Data FullBox, has version/flags. */ \
	ISOboxType(k_hdlr,0x68646C72UL)SEPARATOR \
	ISOboxType(k_udta,0x75647461UL)SEPARATOR /* Container Box, no version/flags. */ \
	ISOboxType(k_cprt,0x63707274UL)SEPARATOR /* Data FullBox, has version/flags. */ \
	ISOboxType(k_uuid,0x75756964UL)SEPARATOR /* Data Box, no version/flags.      */ \
	ISOboxType(k_free,0x66726565UL)SEPARATOR /* Free space Box, no version/flags.*/ \
	ISOboxType(k_mdat,0x6D646174UL)SEPARATOR  /* Media data Box, no version/flags.*/ \
	\
	ISOboxType(k_xml,0x786D6C20UL)SEPARATOR  /*xml box, has version/flags */ \
	ISOboxType(k_trak,0x7472616BUL)SEPARATOR /* Types for the QuickTime timecode track.*/ \
	ISOboxType(k_tkhd,0x746B6864UL)SEPARATOR \
	ISOboxType(k_edts,0x65647473UL)SEPARATOR \
	ISOboxType(k_elst,0x656C7374UL)SEPARATOR \
	ISOboxType(k_mdia,0x6D646961UL)SEPARATOR \
	ISOboxType(k_mdhd,0x6D646864UL)SEPARATOR \
	ISOboxType(k_tmcd,0x746D6364UL)SEPARATOR \
	ISOboxType(k_mhlr,0x6D686C72UL)SEPARATOR \
	ISOboxType(k_minf,0x6D696E66UL)SEPARATOR \
	ISOboxType(k_stbl,0x7374626CUL)SEPARATOR \
	ISOboxType(k_stsd,0x73747364UL)SEPARATOR \
	ISOboxType(k_stsc,0x73747363UL)SEPARATOR \
	ISOboxType(k_stco,0x7374636FUL)SEPARATOR \
	ISOboxType(k_co64,0x636F3634UL)SEPARATOR \
	ISOboxType(k_dinf,0x64696E66UL)SEPARATOR \
	ISOboxType(k_dref,0x64726566UL)SEPARATOR \
	ISOboxType(k_alis,0x616C6973UL)SEPARATOR \
	\
	ISOboxType(k_keys,0x6B657973UL)SEPARATOR /* Type for the QuickTime metadata box containing keys related to metadata item.*/ \
	\
	ISOboxType(k_meta,0x6D657461UL)SEPARATOR /* Types for the iTunes metadata boxes.*/ \
	ISOboxType(k_ilst,0x696C7374UL)SEPARATOR \
	ISOboxType(k_mdir,0x6D646972UL)SEPARATOR \
	ISOboxType(k_mean,0x6D65616EUL)SEPARATOR \
	ISOboxType(k_name,0x6E616D65UL)SEPARATOR \
	ISOboxType(k_data,0x64617461UL)SEPARATOR \
	ISOboxType(k_hyphens,0x2D2D2D2DUL)SEPARATOR \
	\
	ISOboxType(k_skip,0x736B6970UL)SEPARATOR /* Additional classic QuickTime top level boxes.*/ \
	ISOboxType(k_wide,0x77696465UL)SEPARATOR \
	ISOboxType(k_pnot,0x706E6F74UL)SEPARATOR \
	\
	ISOboxType(k_iloc,0x696c6f63UL)SEPARATOR  /*For HEIF files*/\
	ISOboxType(k_iinf,0x69696e66UL)SEPARATOR \
	ISOboxType(k_infe,0x696e6665UL)SEPARATOR \
	ISOboxType(k_idat,0x69646174UL)SEPARATOR \
	ISOboxType(k_pict,0x70696374UL)SEPARATOR \
	ISOboxType(k_url,0x75726c20UL)SEPARATOR \
	ISOboxType(k_urn,0x75726e20UL)SEPARATOR \
	ISOboxType(k_uri,0x75726920UL)SEPARATOR \
	ISOboxType(k_Exif,0x45786966UL)SEPARATOR \
	ISOboxType(k_mime,0x6d696d65UL)SEPARATOR \
	ISOboxType(k_iref,0x69726566UL)SEPARATOR \
	ISOboxType(k_pitm,0x7069746dUL)SEPARATOR \
	ISOboxType(k_cdsc,0x63647363UL)SEPARATOR \
	\
	ISOboxType(k_XMP_,0x584D505FUL) /* The QuickTime variant XMP box.*/ 


#define ISOBoxPrivateList 
#define ISOboxType(x,y) x=y
#define SEPARATOR ,
	enum {
		ISOBoxList
		ISOBoxPrivateList
	};
#undef ISOboxType
#undef SEPARATOR

	bool IsKnownBoxType(XMP_Uns32 boxType) ;
	void TerminateGlobals();

	static XMP_Uns8 k_xmpUUID [16] = { 0xBE, 0x7A, 0xCF, 0xCB, 0x97, 0xA9, 0x42, 0xE8, 0x9C, 0x71, 0x99, 0x94, 0x91, 0xE3, 0xAF, 0xAC };

	struct BoxInfo {
		XMP_Uns32 boxType;         // In memory as native endian!
		XMP_Uns32 headerSize;      // Normally 8 or 16, less than 8 if available space is too small.
		XMP_Uns64 contentSize;     // Always the real size, never 0 for "to EoF".
		XMP_Uns8 idUUID[16];		   // ID of the uuid atom if present
		BoxInfo() : boxType(0), headerSize(0), contentSize(0)
		{
			memset( idUUID, 0, 16 );
		};
	};

	// Get basic info about a box in memory, returning a pointer to the following box.
	const XMP_Uns8 * GetBoxInfo ( const XMP_Uns8 * boxPtr, const XMP_Uns8 * boxLimit,
		                          BoxInfo * info, bool throwErrors = false );

	// Get basic info about a box in a file, returning the offset of the following box. The I/O
	// pointer is left at the start of the box's content. Returns the offset of the following box.
	XMP_Uns64 GetBoxInfo ( XMP_IO* fileRef, const XMP_Uns64 boxOffset, const XMP_Uns64 boxLimit,
		                   BoxInfo * info, bool doSeek = true, bool throwErrors = false );

	//     XMP_Uns32 CountChildBoxes ( XMP_IO* fileRef, const XMP_Uns64 childOffset, const XMP_Uns64 childLimit );

}      // namespace ISO_Media

// Same for moov(MOOV_Support.hpp) and meta box(Meta_Support.hpp) for now,
// If this changes in future then define this before this file is included. 
#ifndef TopBoxSizeLimit
	#define TopBoxSizeLimit 100*1024*1024
#endif
// =================================================================================================

class ISOBaseMedia_Manager {
public:
	virtual ~ISOBaseMedia_Manager(){}
	typedef const void * BoxRef;	// Valid until a sibling or higher box is added or deleted.

	struct BoxInfo {
		XMP_Uns32 boxType;			// In memory as native endian, compares work with ISOMedia::k_* constants.
		XMP_Uns32 childCount;		// ! A 'meta' box has both content (version/flags) and children!
		XMP_Uns32 contentSize;		// Does not include the size of nested boxes.
		const XMP_Uns8 * content;	// Null if contentSize is zero.
		XMP_Uns8 idUUID[16];		// ID of the uuid atom if present
		BoxInfo() : boxType(0), childCount(0), contentSize(0), content(0)
		{ 
			memset(idUUID, 0, 16);
		};
		
	};
#pragma pack (push, 1)	// ! These must match the file layout!
	struct Content_hdlr {	// An 'hdlr' box as defined by ISO 14496-12. Maps OK to the QuickTime box.
		XMP_Uns32 versionFlags;	//  0
		XMP_Uns32 preDef;		//  4
		XMP_Uns32 handlerType;	//  8
		XMP_Uns32 reserved[3];	// 12
								// Plus optional component name string, null terminated UTF-8.
	};							// 24
#pragma pack( pop )
	// ---------------------------------------------------------------------------------------------
	// GetBox - Pick a box given a '/' separated list of box types. Picks the 1st of each type.
	// GetBoxInfo - Get the info if we already have the ref.
	// GetNthChild - Pick the overall n-th child of the parent, zero based.
	// GetTypeChild - Pick the first child of the given type.
	// GetParsedOffset - Get the box's offset in the parsed tree, 0 if changed since parsing.
	// GetHeaderSize - Get the box's header size in the parsed tree, 0 if changed since parsing.

	//BoxRef GetBox(const char * boxPath, BoxInfo * info) const;

	void GetBoxInfo(const BoxRef ref, BoxInfo * info) const;

	BoxRef GetNthChild(BoxRef parentRef, size_t childIndex, BoxInfo * info) const;
	BoxRef GetTypeChild(BoxRef parentRef, XMP_Uns32 childType, BoxInfo * info) const;

	XMP_Uns32 GetParsedOffset(BoxRef ref) const;
	XMP_Uns32 GetHeaderSize(BoxRef ref) const;

	// ---------------------------------------------------------------------------------------------
	// NoteChange - Note overall change, value was directly replaced.
	virtual void NoteChange();


	// SetBox(ref) - Replace the content with a copy of the given data.
	// SetBox(path) - Like above, but creating path to the box if necessary.
	// AddChildBox - Add a child of the given type, using a copy of the given data (may be null)
	void SetBox(BoxRef theBox, const void* dataPtr, XMP_Uns32 size, const XMP_Uns8 * idUUID = 0);
	//void SetBox(const char * boxPath, const void* dataPtr, XMP_Uns32 size, const XMP_Uns8 * idUUID = 0);

	BoxRef AddChildBox(BoxRef parentRef, XMP_Uns32 childType, const void * dataPtr, XMP_Uns32 size, const XMP_Uns8 * idUUID = 0);

	// ---------------------------------------------------------------------------------------------
	// DeleteNthChild - Delete the overall n-th child, return true if there was one.
	// DeleteTypeChild - Delete the first child of the given type, return true if there was one.

	bool DeleteNthChild(BoxRef parentRef, size_t childIndex);
	bool DeleteTypeChild(BoxRef parentRef, XMP_Uns32 childType);

	bool IsChanged() const { return this->subtreeRootNode.changed; };

	struct SpaceInfo {
		XMP_Uns64 offset, size;
		SpaceInfo() : offset(0), size(0) {};
		SpaceInfo(XMP_Uns64 _offset, XMP_Uns64 _size) : offset(_offset), size(_size) {};
	};

	typedef std::vector<SpaceInfo> SpaceList;

	void CreateFreeSpaceList(XMP_IO* fileRef, XMP_Uns64 fileSize,
		XMP_Uns64 oldOffset, XMP_Uns32 oldSize, SpaceList * spaceList);

	void WipeBoxFree(XMP_IO* fileRef, XMP_Uns64 boxOffset, XMP_Uns32 boxSize);
	void WriteBoxHeader(XMP_IO* fileRef, XMP_Uns32 boxType, XMP_Uns64 boxSize);

	struct BoxNode;
	typedef std::vector<BoxNode> BoxList;
	typedef BoxList::iterator    BoxListPos;

	struct BoxNode {
		// ! Don't have a parent link, it will get destroyed by vector growth!

		XMP_Uns32 offset;		// The offset in the fullSubtree, 0 if not in the parse.
		XMP_Uns32 boxType;
		XMP_Uns32 headerSize;	// The actual header size in the fullSubtree, 0 if not in the parse.
		XMP_Uns32 contentSize;	// The current content size, does not include nested boxes or id.
		BoxList   children;
		XMP_Uns8 idUUID[16];
		RawDataBlock changedContent;	// Might be empty even if changed is true.
		bool changed;	// If true, the content is in changedContent, else in fullSubtree.

		BoxNode() : offset(0), boxType(0), headerSize(0), contentSize(0), changed(false)
		{
			memset(idUUID, 0, 16);
		};
		BoxNode(XMP_Uns32 _offset, XMP_Uns32 _boxType, XMP_Uns32 _headerSize, XMP_Uns32 _contentSize)
			: offset(_offset), boxType(_boxType), headerSize(_headerSize), contentSize(_contentSize), changed(false)
		{
			memset(idUUID, 0, 16);
		};
		BoxNode(XMP_Uns32 _offset, XMP_Uns32 _boxType, XMP_Uns32 _headerSize, const XMP_Uns8 * _idUUID, XMP_Uns32 _contentSize)
			: offset(_offset), boxType(_boxType), headerSize(_headerSize), contentSize(_contentSize), changed(false)
		{
			memcpy(idUUID, _idUUID, 16);
		};
	};
	BoxNode subtreeRootNode;
	// subtree node
	// ---------------------------------------------------------------------------------------------
	// The client is expected to fill in fullSubtree before calling ParseMemoryTree, and directly
	// use fullSubtree after calling UpdateMemoryTree.
	//
	// IMPORTANT: We only support cases where the subtree('moov' or 'meta') is significantly less than 4 GB, in
	// particular with a threshold of probably 100 MB. This has 2 big impacts: we can safely use
	// 32-bit offsets and sizes, and comfortably assume everything will fit in available heap space.

	RawDataBlock fullSubtree;	// The entire box, straight from the file or from UpdateMemoryTree.

	//virtual void ParseMemoryTree(XMP_Uns8 fileMode) {} //make this virtual
	//virtual void UpdateMemoryTree() {}
	void FillBoxInfo(const BoxNode & node, BoxInfo * info) const;
	XMP_Uns8 * PickContentPtr(const BoxNode & node) const;

};

#endif // __ISOBaseMedia_Support_hpp__
