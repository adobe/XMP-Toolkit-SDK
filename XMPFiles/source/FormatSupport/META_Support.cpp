// =================================================================================================
// Copyright Adobe
// Copyright 2018 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================
#include "XMPFiles/source/FormatSupport/META_Support.hpp"

#include "XMPFiles/source/FormatSupport/ISOBaseMedia_Support.hpp"
#include<algorithm>

// =================================================================================================
// Meta_Manager::ParseMetaMemoryTree
// ==============================
void Meta_Manager::ParseMetaMemoryTree() {
	//this->fileMode = _fileMode;
	this->subtreeRootNode.offset = this->subtreeRootNode.boxType = 0;
	this->subtreeRootNode.headerSize = this->subtreeRootNode.contentSize = 0;
	this->subtreeRootNode.children.clear();
	this->subtreeRootNode.changedContent.clear();
	this->subtreeRootNode.changed = false;

	if (this->fullSubtree.empty()) return;

	ISOMedia::BoxInfo metaInfo;
	const XMP_Uns8 * metaOrigin = &this->fullSubtree[0];
	const XMP_Uns8 * metaLimit = metaOrigin + this->fullSubtree.size();

	(void)ISOMedia::GetBoxInfo(metaOrigin, metaLimit, &metaInfo);
	XMP_Enforce(metaInfo.boxType == ISOMedia::k_meta);

	XMP_Uns64 fullMetaSize = metaInfo.headerSize + metaInfo.contentSize;

	if (fullMetaSize > TopBoxSizeLimit) {	// From here on we know 32-bit offsets are safe.  
		XMP_Throw("Oversize 'meta' box", kXMPErr_EnforceFailure);
	}

	this->subtreeRootNode.boxType = ISOMedia::k_meta;
	this->subtreeRootNode.headerSize = metaInfo.headerSize;
	this->subtreeRootNode.contentSize = (XMP_Uns32)metaInfo.contentSize;

	///this->subtreeRootNode = BoxNode(0, metaInfo.boxType, metaInfo.headerSize, (XMP_Uns32)metaInfo.contentSize);
	ParseNestedMetaBoxes(& (this->subtreeRootNode), "meta");
	
}

// =================================================================================================
// Meta_Manager::ParseNestedMetaBoxes
// ==============================
void Meta_Manager::ParseNestedMetaBoxes(BoxNode * parentNode, const std::string & parentPath) {


	const XMP_Uns8 * metaOrigin = &this->fullSubtree[0];
	ISOMedia::BoxInfo isoInfo;
	BoxInfo mmgrInfo; 
	
	const XMP_Uns8 * childOrigin = metaOrigin + parentNode->offset + parentNode->headerSize;
	const XMP_Uns8 * childLimit = childOrigin + parentNode->contentSize;

	const XMP_Uns8 * nextChild;

	parentNode->contentSize = 0;

	//meta box, iinf, dref is a full box
	if (parentPath == "meta" || parentPath == "iref"){
		childOrigin += 4; parentNode->contentSize = 4;
	}
	else if (parentPath == "dref"){
		childOrigin += 8; // Adding size(4 bytes) of entry count
		parentNode->contentSize = 8;
	}
	else if (parentPath == "iinf") {
		if(childOrigin[0] == 0){	
			childOrigin += 6; // 2 bytes in iinf box are for entry count 
			parentNode->contentSize = 6;
		}
		else {
			childOrigin += 8; // 4 bytes in iinf box are for entry count
			parentNode->contentSize = 8;
		}
			 
	}

	for (const XMP_Uns8 * currChild = childOrigin; currChild < childLimit; currChild = nextChild) {
		nextChild = ISOMedia::GetBoxInfo(currChild, childLimit, &isoInfo);
		if ((isoInfo.headerSize < 8) && 
			(isoInfo.contentSize == 0)) continue;
		
		XMP_Uns32 childOffset = (XMP_Uns32)(currChild - metaOrigin);

		//currently no uuid box is used in HEIF files
		if (isoInfo.boxType == ISOMedia::k_uuid)
			parentNode->children.push_back(BoxNode(childOffset, isoInfo.boxType, isoInfo.headerSize, (XMP_Uns8 *)isoInfo.idUUID, (XMP_Uns32)isoInfo.contentSize));
		else
			parentNode->children.push_back(BoxNode(childOffset, isoInfo.boxType, isoInfo.headerSize, (XMP_Uns32)isoInfo.contentSize));
		BoxNode * newChild = &parentNode->children.back();
		
		switch (isoInfo.boxType) {

			case ISOMedia::k_iinf: this->ParseNestedMetaBoxes(newChild, "iinf"); break;
			case ISOMedia::k_dinf: this->ParseNestedMetaBoxes(newChild, "dinf"); break;
			case ISOMedia::k_dref: this->ParseNestedMetaBoxes(newChild, "dref"); break;
			case ISOMedia::k_iref: this->ParseNestedMetaBoxes(newChild, "iref"); break;
		}
		
		
	}
}

// Find a box given the type path. Pick the first child of each type.
ISOBaseMedia_Manager::BoxRef Meta_Manager::GetBox(const char * boxPath, BoxInfo * info) const
{
	size_t pathLen = strlen(boxPath);
	XMP_Assert((pathLen >= 4) && XMP_LitNMatch(boxPath, "meta", 4));
	if (info != 0) memset(info, 0, sizeof(BoxInfo));

	const char * pathPtr = boxPath + 5;	// Skip the "meta/" portion.
	const char * pathEnd = boxPath + pathLen;

	BoxRef currRef = &this->subtreeRootNode;

	while (pathPtr < pathEnd) {

		XMP_Assert((pathEnd - pathPtr) >= 4);
		XMP_Uns32 boxType = GetUns32BE(pathPtr);
		pathPtr += 5;	// ! Don't care that the last step goes 1 too far.

		currRef = this->GetTypeChild(currRef, boxType, 0);
		if (currRef == 0) return 0;

	}

	this->FillBoxInfo(*((BoxNode*)currRef), info);
	return currRef;

}

// =================================================================================================
// Meta_Manager::NewSubtreeSize
// ============================
//
// Determine the new (changed) size of a subtree. Ignore 'free' and 'wide' boxes.
XMP_Uns32 Meta_Manager::NewSubtreeSize(const BoxNode & node, const std::string & parentPath)
{
	XMP_Uns32 subtreeSize;
	if (node.contentSize != 0)
		subtreeSize = 8 + node.contentSize;	// All boxes will have 8 byte headers.
	else
		subtreeSize = 8;

	if (node.boxType == ISOMedia::k_uuid)
		subtreeSize += 16;				// id of uuid is 16 bytes long
	

		for (size_t i = 0, limit = node.children.size(); i < limit; ++i) {

			char suffix[6];
			suffix[0] = '/';
			PutUns32BE(node.boxType, &suffix[1]);
			suffix[5] = 0;
			std::string nodePath = parentPath + suffix;

			subtreeSize += this->NewSubtreeSize(node.children[i], nodePath);
			XMP_Enforce(subtreeSize < TopBoxSizeLimit);

		}

		return subtreeSize;

}// Meta_Manager::NewSubtreeSize

// =================================================================================================
// Meta_Manager::UpdateMemoryTree
// ==============================
void Meta_Manager::UpdateMemoryTree()
{
	if (!this->IsChanged()) return;

	XMP_Uns32 newSize = this->NewSubtreeSize(this->subtreeRootNode, "");
	XMP_Enforce(newSize < TopBoxSizeLimit);

	RawDataBlock newData;
	newData.assign(newSize, 0);	// Prefill with zeroes, can't append multiple items to a vector.

	XMP_Uns8 * newPtr = &newData[0];
	XMP_Uns8 * newEnd = newPtr + newSize;
	
	XMP_Uns8 * trueEnd = this->AppendNewSubtree(this->subtreeRootNode, "", newPtr, newEnd); //meta is a fullbox
	XMP_Enforce(trueEnd == newEnd);

	this->fullSubtree.swap(newData);
	this->ParseMetaMemoryTree();

}	// Meta_Manager::UpdateMemoryTree

#define IncrNewPtr(count)	{ newPtr += count; XMP_Enforce ( newPtr <= newEnd ); }

XMP_Uns8 * Meta_Manager::AppendNewSubtree(const BoxNode & node, const std::string & parentPath,
	XMP_Uns8 * newPtr, XMP_Uns8 * newEnd)
{
	
	/*XMP_Assert ( (node.boxType != ISOMedia::k_meta) ? (node.children.empty() || (node.contentSize == 0)) :
	(node.children.empty() || (node.contentSize == 4)) );*/

	XMP_Enforce((XMP_Uns32)(newEnd - newPtr) >= (8 + node.contentSize));

	//XMP_Uns8 * ilocOrigin;
	
	// Leave the size as 0 for now, append the type and content.
	XMP_Uns8 * boxOrigin = newPtr;	// Save origin to fill in the final size.
	PutUns32BE(node.boxType, (newPtr + 4));
	IncrNewPtr(8); 
	if (node.boxType == ISOMedia::k_uuid)		// For uuid, additional 16 bytes is stored for ID 
	{
		XMP_Enforce((XMP_Uns32)(newEnd - newPtr) >= (16 + node.contentSize));
		memcpy(newPtr, node.idUUID, 16);
		IncrNewPtr(16);
	}
	/*if (node.boxType == ISOMedia::k_iloc)
		ilocOrigin = newPtr;*/

	if (node.contentSize != 0 ) { 
		const XMP_Uns8 * content = PickContentPtr(node);
		memcpy(newPtr, content, node.contentSize);
		IncrNewPtr(node.contentSize);
	}

	// Append the nested boxes.

	if (!node.children.empty()) {

		char suffix[6];
		suffix[0] = '/';
		PutUns32BE(node.boxType, &suffix[1]);
		suffix[5] = 0;
		std::string nodePath = parentPath + suffix;

		for (size_t i = 0, limit = node.children.size(); i < limit; ++i) {
			newPtr = this->AppendNewSubtree(node.children[i], nodePath, newPtr, newEnd);
		}

	}

	// Fill in the final size.

	PutUns32BE((XMP_Uns32)(newPtr - boxOrigin), boxOrigin);
	//ilocOfset = (XMP_Uns32)(ilocOrigin - boxOrigin);
	return newPtr;

}	// MOOV_Manager::AppendNewSubtree

// =================================================================================================
// Meta_Manager::UpdateIlocBoxContent
// ==============================
void Meta_Manager::UpdateIlocBoxContent() {
	ISOBaseMedia_Manager::BoxInfo ilocInfo;
	ISOBaseMedia_Manager::BoxRef  ilocRef = this->GetTypeChild(&subtreeRootNode, ISOMedia::k_iloc, &ilocInfo);

	ISOBaseMedia_Manager::BoxNode * ilocNode = (ISOBaseMedia_Manager::BoxNode*)ilocRef;

	XMP_Uns8 ilocVersion = ilocInfo.content[0];
	if (ilocVersion > 2) //other versions not allowed
		return;
	
	// To know the size of changed content
	XMP_Uns32 newSize = 0;
	if (ilocVersion < 2)
		newSize += 8;
	else 
		newSize += 10;

	for (auto const& itr : itemLocationMap)
	{
		newSize += itr.second.sizeOfItem;
	}
	ilocNode->changedContent.assign(newSize, 0);
	memcpy(&(ilocNode->changedContent[0]), ilocInfo.content, ilocInfo.contentSize);

	XMP_Uns32 vecIndex = 4;//iloc is a FullBox
	//XMP_Uns16 sizeValue = GetUns16BE(ilocInfo.content + 4);
	vecIndex += 2; //sizeValue bytes

	if (ilocVersion < 2) {
		PutUns16BE((XMP_Uns16)itemLocationMap.size(),&( ilocNode->changedContent[vecIndex]));
		vecIndex += 2;
		
	}
	else {
		PutUns32BE((XMP_Uns32)itemLocationMap.size(), &(ilocNode->changedContent[vecIndex]));
		vecIndex += 4;
	}
	
	bool flag = 0;
	for (auto const& itr : itemLocationMap)
	{
		ilocItem currItem = itr.second;
		if (!currItem.changed &&  flag == 0) {
			//memcpy(&(ilocNode->changedContent[vecIndex]), ilocInfo.content + vec, currItem.sizeOfItem);
			vecIndex += currItem.sizeOfItem;
			continue;
		}
		flag = 1;
		if (ilocVersion < 2) {
			XMP_Uns16 itemId = (XMP_Uns16)itr.first;
			PutUns16BE(itemId, &(ilocNode->changedContent[vecIndex]));
			vecIndex += 2;
		}
		else if (ilocVersion == 2) {
			XMP_Uns32 itemId = itr.first;
			PutUns32BE(itemId, &(ilocNode->changedContent[vecIndex]));
			vecIndex += 4;
		}
		if (ilocVersion == 1 || ilocVersion == 2) {
			XMP_Uns16 res = 0;
			res &= 0xFFF0;
			res |= (currItem.constructionMethod & 0x000F);
			PutUns16BE(res, &(ilocNode->changedContent[vecIndex]));
			vecIndex += 2;// reserved + construction method = 0
		}
		PutUns16BE(0, &(ilocNode->changedContent[vecIndex])); //data refrence index
		vecIndex += 2;


		if (ilocByteSizesStruct.baseOffsetSize == 32) { PutUns32BE(0, &(ilocNode->changedContent[vecIndex])); vecIndex += 4; }
		else if (ilocByteSizesStruct.baseOffsetSize == 64) { PutUns64BE(0, &(ilocNode->changedContent[vecIndex])); vecIndex += 8; }
		XMP_Uns16 nExtents = (XMP_Uns16)currItem.iExtents.size();
		PutUns16BE(nExtents , &(ilocNode->changedContent[vecIndex]));
		vecIndex += 2;

		for (size_t j = 0; j < nExtents; j++) { 
			extent ex = currItem.iExtents[j];
	
			if ((ilocVersion == 1 || ilocVersion == 2) && ilocByteSizesStruct.indexSize > 0) { // part4 = index_size // && byteSizes.part4 > 0 	
				if (ilocByteSizesStruct.indexSize == 32) { PutUns32BE((XMP_Uns32)ex.extent_index, &(ilocNode->changedContent[vecIndex])); vecIndex += 4; }
				else if (ilocByteSizesStruct.indexSize == 64) { PutUns64BE(ex.extent_index, &(ilocNode->changedContent[vecIndex])); vecIndex += 8; }
			}

			if (ilocByteSizesStruct.offsetSize == 32) { PutUns32BE((XMP_Uns32)ex.extent_offset, &(ilocNode->changedContent[vecIndex])); vecIndex += 4; }
			else if (ilocByteSizesStruct.offsetSize == 64) { PutUns64BE(ex.extent_offset, &(ilocNode->changedContent[vecIndex])); vecIndex += 8; }

			if (ilocByteSizesStruct.lengthSize == 32) { PutUns32BE((XMP_Uns32)ex.extent_length, &(ilocNode->changedContent[vecIndex])); vecIndex += 4; }
			else if (ilocByteSizesStruct.lengthSize == 64) { PutUns64BE(ex.extent_length, &(ilocNode->changedContent[vecIndex])); vecIndex += 8; }
		}
	}

	ilocNode->changed = true;
	ilocNode->contentSize = newSize;
	NoteChange();
}

