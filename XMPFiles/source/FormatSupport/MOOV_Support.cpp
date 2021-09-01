// =================================================================================================
// Copyright Adobe
// Copyright 2009 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#ifndef TraceParseMoovTree
#define TraceParseMoovTree 0
#endif

#ifndef TraceUpdateMoovTree
#define TraceUpdateMoovTree 0
#endif


#include "XMPFiles/source/FormatSupport/MOOV_Support.hpp"

#include "XMPFiles/source/FormatSupport/ISOBaseMedia_Support.hpp"

#include <string.h>

// =================================================================================================
/// \file MOOV_Support.cpp
/// \brief XMPFiles support for the 'moov' box in MPEG-4 and QuickTime files.
// =================================================================================================

// =================================================================================================
// =================================================================================================
// MOOV_Manager - The parsing and reading routines are all commmon
// =================================================================================================
// =================================================================================================


// =================================================================================================
// MOOV_Manager::GetBox
// ====================
//
// Find a box given the type path. Pick the first child of each type.

MOOV_Manager::BoxRef MOOV_Manager::GetBox ( const char * boxPath, BoxInfo * info ) const
{
	size_t pathLen = strlen(boxPath);
	XMP_Assert ( (pathLen >= 4) && XMP_LitNMatch ( boxPath, "moov", 4 ) );
	if ( info != 0 ) memset ( info, 0, sizeof(BoxInfo) );
	
	const char * pathPtr = boxPath + 5;	// Skip the "moov/" portion.
	const char * pathEnd = boxPath + pathLen;
	
	BoxRef currRef = &this->subtreeRootNode;
	
	while ( pathPtr < pathEnd ) {
	
		XMP_Assert ( (pathEnd - pathPtr) >= 4 );		
		XMP_Uns32 boxType = GetUns32BE ( pathPtr );
		pathPtr += 5;	// ! Don't care that the last step goes 1 too far.
		
		currRef = this->GetTypeChild ( currRef, boxType, 0 );
		if ( currRef == 0 ) return 0;
	
	}

	this->FillBoxInfo ( *((BoxNode*)currRef), info );
	return currRef;
	
}	// MOOV_Manager::GetBox


// =================================================================================================
// MOOV_Manager::ParseMemoryTree
// =============================
//
// Parse the fullSubtree data, building the BoxNode tree for the stuff that we care about. Tolerate
// errors like content ending too soon, make a best effoert to parse what we can.

void MOOV_Manager::ParseMemoryTree ( XMP_Uns8 _fileMode )
{
	this->fileMode = _fileMode;
	
	this->subtreeRootNode.offset = this->subtreeRootNode.boxType = 0;
	this->subtreeRootNode.headerSize = this->subtreeRootNode.contentSize = 0;
	this->subtreeRootNode.children.clear();
	this->subtreeRootNode.changedContent.clear();
	this->subtreeRootNode.changed = false;

	if ( this->fullSubtree.empty() ) return;
	
	ISOMedia::BoxInfo moovInfo;
	const XMP_Uns8 * moovOrigin = &this->fullSubtree[0];
	const XMP_Uns8 * moovLimit  = moovOrigin + this->fullSubtree.size();

	(void) ISOMedia::GetBoxInfo ( moovOrigin, moovLimit, &moovInfo );
	XMP_Enforce ( moovInfo.boxType == ISOMedia::k_moov );
	
	XMP_Uns64 fullMoovSize = moovInfo.headerSize + moovInfo.contentSize;
	if ( fullMoovSize > TopBoxSizeLimit) {	// From here on we know 32-bit offsets are safe.
		XMP_Throw ( "Oversize 'moov' box", kXMPErr_EnforceFailure );
	}
	
	this->subtreeRootNode.boxType = ISOMedia::k_moov;
	this->subtreeRootNode.headerSize = moovInfo.headerSize;
	this->subtreeRootNode.contentSize = (XMP_Uns32)moovInfo.contentSize;

	bool ignoreMetaBoxes = (_fileMode == kFileIsTraditionalQT);	// ! Don't want, these don't follow ISO spec.
	#if TraceParseMoovTree
		fprintf ( stderr, "Parsing 'moov' subtree, subtreeRootNode @ 0x%X, ignoreMetaBoxes = %d\n",
				  &this->subtreeRootNode, ignoreMetaBoxes );
	#endif
	this->ParseNestedBoxes ( &this->subtreeRootNode, "moov", ignoreMetaBoxes );
	
}	// MOOV_Manager::ParseMemoryTree

// =================================================================================================
// MOOV_Manager::ParseNestedBoxes
// ==============================
//
// Add the current level of child boxes to the parent node, recurse as appropriate.

void MOOV_Manager::ParseNestedBoxes ( BoxNode * parentNode, const std::string & parentPath, bool ignoreMetaBoxes )
{
	ISOMedia::BoxInfo isoInfo;
	const XMP_Uns8 * moovOrigin = &this->fullSubtree[0];
	const XMP_Uns8 * childOrigin = moovOrigin + parentNode->offset + parentNode->headerSize;
	const XMP_Uns8 * childLimit  = childOrigin + parentNode->contentSize;
	const XMP_Uns8 * nextChild;
	
	parentNode->contentSize = 0;	// Exclude nested box size.

	if ( parentNode->boxType == ISOMedia::k_meta && parentPath != "moov/meta") {	// ! The 'meta' box is a FullBox.
		parentNode->contentSize = 4;
		childOrigin += 4;
	}

	for ( const XMP_Uns8 * currChild = childOrigin; currChild < childLimit; currChild = nextChild ) {
	
		nextChild = ISOMedia::GetBoxInfo ( currChild, childLimit, &isoInfo );
		if ( (isoInfo.boxType == 0) &&
			 (isoInfo.headerSize < 8) &&
			 (isoInfo.contentSize == 0) ) continue;	// Skip trailing padding that QT sometimes writes.
		
		XMP_Uns32 childOffset = (XMP_Uns32) (currChild - moovOrigin);
		if( isoInfo.boxType == ISOMedia::k_uuid )
			parentNode->children.push_back ( BoxNode ( childOffset, isoInfo.boxType, isoInfo.headerSize, (XMP_Uns8 *)isoInfo.idUUID, (XMP_Uns32)isoInfo.contentSize ) );
		else
			parentNode->children.push_back ( BoxNode ( childOffset, isoInfo.boxType, isoInfo.headerSize, (XMP_Uns32)isoInfo.contentSize ) );
		BoxNode * newChild = &parentNode->children.back();
		
		#if TraceParseMoovTree
			size_t depth = (parentPath.size()+1) / 5;
			for ( size_t i = 0; i < depth; ++i ) fprintf ( stderr, "  " );
			XMP_Uns32 be32 = MakeUns32BE ( newChild->boxType );
			XMP_Uns32 addr32 = (XMP_Uns32) this->PickContentPtr ( *newChild );
			fprintf ( stderr, "  Parsed %s/%.4s, offset 0x%X, size %d, content @ 0x%X, BoxNode @ 0x%X\n",
					  parentPath.c_str(), &be32, newChild->offset, newChild->contentSize, addr32, newChild );
		#endif
		
		const char * pathSuffix = 0;	// Set to non-zero for boxes of interest.
		char buffer[6];	buffer[0] = 0;
		
			switch ( isoInfo.boxType ) {	// Want these boxes regardless of parent.
				case ISOMedia::k_udta : pathSuffix = "/udta"; break;
				case ISOMedia::k_meta : pathSuffix = "/meta"; break;
				case ISOMedia::k_ilst : pathSuffix = "/ilst"; break;
				case ISOMedia::k_trak : pathSuffix = "/trak"; break;
				case ISOMedia::k_edts : pathSuffix = "/edts"; break;
				case ISOMedia::k_mdia : pathSuffix = "/mdia"; break;
				case ISOMedia::k_minf : pathSuffix = "/minf"; break;
				case ISOMedia::k_dinf : pathSuffix = "/dinf"; break;
				case ISOMedia::k_stbl : pathSuffix = "/stbl"; break;
			}
		if ( pathSuffix != 0 ) {
			this->ParseNestedBoxes ( newChild, (parentPath + pathSuffix), ignoreMetaBoxes );
		}
	
	}

}	// MOOV_Manager::ParseNestedBoxes




// =================================================================================================
// MOOV_Manager::SetBox
// ====================
//
// Like above, but create the path to the box if necessary.

void MOOV_Manager::SetBox ( const char * boxPath, const void* dataPtr, XMP_Uns32 size , const XMP_Uns8 * idUUID )
{
	XMP_Enforce ( size < TopBoxSizeLimit);

	size_t pathLen = strlen(boxPath);
	XMP_Assert ( (pathLen >= 4) && XMP_LitNMatch ( boxPath, "moov", 4 ) );
	
	const char * pathPtr = boxPath + 5;	// Skip the "moov/" portion.
	const char * pathEnd = boxPath + pathLen;
	
	BoxRef parentRef = 0;
	BoxRef currRef   = &this->subtreeRootNode;
	
	while ( pathPtr < pathEnd ) {
	
		XMP_Assert ( (pathEnd - pathPtr) >= 4 );		
		XMP_Uns32 boxType = GetUns32BE ( pathPtr );
		pathPtr += 5;	// ! Don't care that the last step goes 1 too far.
		
		parentRef = currRef;
		currRef = this->GetTypeChild ( parentRef, boxType, 0 );
		if ( currRef == 0 ) currRef = this->AddChildBox ( parentRef, boxType, 0, 0 , idUUID );
	
	}

	this->ISOBaseMedia_Manager::SetBox ( currRef, dataPtr, size, idUUID );
	
}	// MOOV_Manager::SetBox


// =================================================================================================
// MOOV_Manager::NewSubtreeSize
// ============================
//
// Determine the new (changed) size of a subtree. Ignore 'free' and 'wide' boxes.

XMP_Uns32 MOOV_Manager::NewSubtreeSize ( const BoxNode & node, const std::string & parentPath )
{
	XMP_Uns32 subtreeSize = 8 + node.contentSize;	// All boxes will have 8 byte headers.

	if( node.boxType == ISOMedia::k_uuid )
		subtreeSize += 16;				// id of uuid is 16 bytes long
	if ( (node.boxType == ISOMedia::k_free) || (node.boxType == ISOMedia::k_wide) ) {
	}

	for ( size_t i = 0, limit = node.children.size(); i < limit; ++i ) {

		char suffix[6];
		suffix[0] = '/';
		PutUns32BE ( node.boxType, &suffix[1] );
		suffix[5] = 0;
		std::string nodePath = parentPath + suffix;
		
		subtreeSize += this->NewSubtreeSize ( node.children[i], nodePath );
		XMP_Enforce ( subtreeSize < TopBoxSizeLimit);

	}

	return subtreeSize;
	
}	// MOOV_Manager::NewSubtreeSize

// =================================================================================================
// MOOV_Manager::AppendNewSubtree
// ==============================
//
// Append this node's header, content, and children. Because the 'meta' box is a FullBox with nested
// boxes, there can be both content and children. Ignore 'free' and 'wide' boxes.

#define IncrNewPtr(count)	{ newPtr += count; XMP_Enforce ( newPtr <= newEnd ); }

#if TraceUpdateMoovTree
	static XMP_Uns8 * newOrigin;
#endif

XMP_Uns8 * MOOV_Manager::AppendNewSubtree ( const BoxNode & node, const std::string & parentPath,
											XMP_Uns8 * newPtr, XMP_Uns8 * newEnd )
{
	if ( (node.boxType == ISOMedia::k_free) || (node.boxType == ISOMedia::k_wide) ) {
	}
	
	/*XMP_Assert ( (node.boxType != ISOMedia::k_meta) ? (node.children.empty() || (node.contentSize == 0)) :
													  (node.children.empty() || (node.contentSize == 4)) );*/
	
	XMP_Enforce ( (XMP_Uns32)(newEnd - newPtr) >= (8 + node.contentSize) );

	#if TraceUpdateMoovTree
		XMP_Uns32 be32 = MakeUns32BE ( node.boxType );
		XMP_Uns32 newOffset = (XMP_Uns32) (newPtr - newOrigin);
		XMP_Uns32 addr32 = (XMP_Uns32) this->PickContentPtr ( node );
		fprintf ( stderr, "  Appending %s/%.4s @ 0x%X, size %d, content @ 0x%X\n",
				  parentPath.c_str(), &be32, newOffset, node.contentSize, addr32 );
	#endif

	// Leave the size as 0 for now, append the type and content.
	
	XMP_Uns8 * boxOrigin = newPtr;	// Save origin to fill in the final size.
	PutUns32BE ( node.boxType, (newPtr + 4) );
	IncrNewPtr ( 8 );
	if( node.boxType == ISOMedia::k_uuid )		// For uuid, additional 16 bytes is stored for ID 
	{
		XMP_Enforce ( (XMP_Uns32)(newEnd - newPtr) >= ( 16 + node.contentSize ) );
		memcpy( newPtr, node.idUUID, 16 );
		IncrNewPtr ( 16 );
	}
	if ( node.contentSize != 0 ) {
		const XMP_Uns8 * content = PickContentPtr( node );
		memcpy ( newPtr, content, node.contentSize );
		IncrNewPtr ( node.contentSize );
	}
	
	// Append the nested boxes.
	
	if ( ! node.children.empty() ) {

		char suffix[6];
		suffix[0] = '/';
		PutUns32BE ( node.boxType, &suffix[1] );
		suffix[5] = 0;
		std::string nodePath = parentPath + suffix;
		
		for ( size_t i = 0, limit = node.children.size(); i < limit; ++i ) {
			newPtr = this->AppendNewSubtree ( node.children[i], nodePath, newPtr, newEnd );
		}

	}
	
	// Fill in the final size.
	
	PutUns32BE ( (XMP_Uns32)(newPtr - boxOrigin), boxOrigin );
	
	return newPtr;
	
}	// MOOV_Manager::AppendNewSubtree

// =================================================================================================
// MOOV_Manager::UpdateMemoryTree
// ==============================

void MOOV_Manager::UpdateMemoryTree()
{
	if ( ! this->IsChanged() ) return;
	
	XMP_Uns32 newSize = this->NewSubtreeSize ( this->subtreeRootNode, "" );
	XMP_Enforce ( newSize < TopBoxSizeLimit);
	
	RawDataBlock newData;
	newData.assign ( newSize, 0 );	// Prefill with zeroes, can't append multiple items to a vector.
	
	XMP_Uns8 * newPtr = &newData[0];
	XMP_Uns8 * newEnd = newPtr + newSize;

	#if TraceUpdateMoovTree
		fprintf ( stderr, "Starting MOOV_Manager::UpdateMemoryTree\n" );
		newOrigin = newPtr;
	#endif
	
	XMP_Uns8 * trueEnd = this->AppendNewSubtree ( this->subtreeRootNode, "", newPtr, newEnd );
	XMP_Enforce ( trueEnd == newEnd );
	
	this->fullSubtree.swap ( newData );
	this->ParseMemoryTree ( this->fileMode );
	
}	// MOOV_Manager::UpdateMemoryTree
