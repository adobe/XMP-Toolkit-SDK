#ifndef __META_Support_hpp__
#define __META_Support_hpp__	1
// =================================================================================================
// Copyright Adobe
// Copyright 2018 Adobe
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
#include <map>
#include <unordered_map>
#include <set>


class Meta_Manager : public ISOBaseMedia_Manager {
public:
	//only version 2 and 3 allowed
	struct itemInfoEntry {

		// version 2 has item id of 16 bits, version 3 has item id of 32 bits
		// So taking type XMP_Uns32 
		// itemID=0 means Primary item, but we are not bothered with it, so we can initialize it with 0
		XMP_Uns32 itemID; 
		XMP_Uns16 itemProtectionIndex;
		XMP_Uns32 itemType;
		std::string itemName;
		// required when itemType='mime'
		std::string contentType, contenEncoding;
		// required when itemType='uri'
		std::string	itemUriType;
		XMP_Uns8 version; // version of infeBox

		itemInfoEntry() :itemID(0), itemProtectionIndex(0), itemType(0), version(0) {};
	};

	std::vector<itemInfoEntry> itemInfoEntryList;

	struct extent {
		XMP_Uns64 extent_index;  // Reqd for construction method = 2 | 0 is reserved, starts from 1
		XMP_Uns64 extent_offset; // file offset based on construction method
		XMP_Uns64 extent_length;
		XMP_Uns64 relativeOffsetInData; // =0 for only 1 extent in iloc item

		extent() :extent_index(0), extent_offset(0), extent_length(0){};
	};
	struct ilocItem {
		XMP_Uns8 constructionMethod; //In version 1 and 2
		XMP_Uns16 dataReferenceIndex;
		std::vector<extent> iExtents;
		bool changed;
		XMP_Uns32 sizeOfItem;
		ilocItem() : constructionMethod(0), dataReferenceIndex(0),changed(false), sizeOfItem(0){};
	};

	struct {
		size_t offsetSize;
		size_t baseOffsetSize;
		size_t lengthSize;
		size_t indexSize;
	} ilocByteSizesStruct;
	
	// Considering Item Id to be unique here
	std::map<XMP_Uns32, ilocItem> itemLocationMap;

	typedef std::map<XMP_Uns32, std::pair<size_t, Meta_Manager::extent>> ExtentIDMap;

	// unordered_map of <reference type, unordered_map of <fromItemId, toItemIdsVector>
	typedef std::unordered_map<XMP_Uns32, std::vector<XMP_Uns32>> RelatedItemsMap;
	typedef std::unordered_map<XMP_Uns32, RelatedItemsMap> ItemReferenceTypeMap;
	ItemReferenceTypeMap irefBox;

	void ParseMetaMemoryTree(); 
   	BoxRef GetBox(const char * boxPath, BoxInfo * info) const;
	
	BoxNode metaNode;
	
	std::set<XMP_Uns32> traversed;

	void UpdateMemoryTree();
	void UpdateIlocBoxContent(); 
private:
	
	void ParseNestedMetaBoxes(BoxNode * parentNode, const std::string & parentPath);
	XMP_Uns32 NewSubtreeSize(const BoxNode & node, const std::string & parentPath);
	XMP_Uns8 * AppendNewSubtree(const BoxNode & node, const std::string & parentPath,
		XMP_Uns8 * newPtr, XMP_Uns8 * newEnd);
};


#endif
