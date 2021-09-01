// =================================================================================================
// Copyright 2003 Adobe
// All Rights Reserved.
//
// NOTICE:	Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! This must be the first include!
#include "XMPCore/source/XMPCore_Impl.hpp"

#include "XMPCore/XMPCoreDefines.h"
#include "XMPCore/source/XMPUtils.hpp"
#if ENABLE_CPP_DOM_MODEL
	#include "source/UnicodeInlines.incl_cpp"
	#include "source/UnicodeConversions.hpp"
	#include "source/ExpatAdapter.hpp"
	#include "third-party/zuid/interfaces/MD5.h"
	#include "XMPCore/Interfaces/IMetadata_I.h"
	#include "XMPCore/Interfaces/IArrayNode_I.h"
	#include "XMPCore/Interfaces/ISimpleNode_I.h"
	#include "XMPCore/Interfaces/INodeIterator_I.h"
	#include "XMPCore/Interfaces/IPathSegment_I.h"
	#include "XMPCore/Interfaces/IPath_I.h"
	#include "XMPCore/Interfaces/INameSpacePrefixMap_I.h"
	#include "XMPCore/Interfaces/IDOMImplementationRegistry_I.h"
	#include "XMPCommon/Interfaces/IUTF8String_I.h"
#endif
#include <algorithm>	// For binary_search.

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <errno.h>

#include <stdio.h>	// For snprintf.

#if XMP_WinBuild
	#pragma warning ( disable : 4800 )	// forcing value to bool 'true' or 'false' (performance warning)
#endif

// =================================================================================================
// Local Types and Constants
// ========================= 

typedef unsigned long	UniCodePoint;

enum UniCharKind {
	UCK_normal,
	UCK_space,
	UCK_comma,
	UCK_semicolon,
	UCK_quote,
	UCK_control
};
typedef enum UniCharKind	UniCharKind;

#define UnsByte(c)	((unsigned char)(c))
#define UCP(u)		((UniCodePoint)(u))
	// ! Needed on Windows (& PC Linux?) for inequalities with literals ito avoid sign extension.

#ifndef TraceMultiFile
	#define TraceMultiFile	0
#endif

// =================================================================================================
// Static Variables
// ================

// =================================================================================================
// Local Utilities
// ===============

// -------------------------------------------------------------------------------------------------
// ClassifyCharacter
// -----------------

static void
ClassifyCharacter ( XMP_StringPtr fullString, size_t offset,
					UniCharKind * charKind, size_t * charSize, UniCodePoint * uniChar )
{
	*charKind = UCK_normal; // Assume typical case.
	
	unsigned char	currByte = UnsByte ( fullString[offset] );
	
	if ( currByte < UnsByte(0x80) ) {
	
		// ----------------------------------------
		// We've got a single byte ASCII character.

		*charSize = 1;
		*uniChar = currByte;

		if ( currByte > UnsByte(0x22) ) {

			if ( currByte == UnsByte(0x2C) ) {
				*charKind = UCK_comma;
			} else if ( currByte == UnsByte(0x3B) ) {
				*charKind = UCK_semicolon;
			}
			// [2674672] Discontinue to interpret square brackets 
			//           as Asian quotes in XMPUtils::SeparateArrayItems(..))
			// *** else if ( (currByte == UnsByte(0x5B)) || (currByte == UnsByte(0x5D)) ) {
			// ***	*charKind = UCK_quote;	// ! ASCII '[' and ']' are used as quotes in Chinese and Korean.
			// *** }

		} else {	// currByte <= 0x22

			if ( currByte == UnsByte(0x22) ) {
				*charKind = UCK_quote;
			} else if ( currByte == UnsByte(0x21) ) {
				*charKind = UCK_normal;
			} else if ( currByte == UnsByte(0x20) ) {
				*charKind = UCK_space;
			} else {
				*charKind = UCK_control;
			}

		}

	} else {	// currByte >= 0x80
	
		// ---------------------------------------------------------------------------------------
		// We've got a multibyte Unicode character. The first byte has the number of bytes and the
		// highest order bits. The other bytes each add 6 more bits. Compose the UTF-32 form so we
		// can classify directly with the Unicode code points. Order the upperBits tests to be
		// fastest for Japan, probably the most common non-ASCII usage.
		
		*charSize = 0;
		*uniChar = currByte;
		while ( (*uniChar & 0x80) != 0 ) {	// Count the leading 1 bits in the byte.
			++(*charSize);
			*uniChar = *uniChar << 1;
		}
		XMP_Assert ( (offset + *charSize) <= strlen(fullString) );
		
		*uniChar = *uniChar & 0x7F;			// Put the character bits in the bottom of uniChar.
		*uniChar = *uniChar >> *charSize;
		
		for ( size_t i = (offset + 1); i < (offset + *charSize); ++i ) {
			*uniChar = (*uniChar << 6) | (UnsByte(fullString[i]) & 0x3F);
		}
		
		XMP_Uns32 upperBits = static_cast<XMP_Uns32>(*uniChar >> 8);	// First filter on just the high order 24 bits.

		if ( upperBits == 0xFF ) {			// U+FFxx

			if ( *uniChar == UCP(0xFF0C) ) {
				*charKind = UCK_comma;			// U+FF0C, full width comma.
			} else if ( *uniChar == UCP(0xFF1B) ) {
				*charKind = UCK_semicolon;		// U+FF1B, full width semicolon.
			} else if ( *uniChar == UCP(0xFF64) ) {
				*charKind = UCK_comma;			// U+FF64, half width ideographic comma.
			}

		} else if ( upperBits == 0xFE ) {	// U+FE--

			if ( *uniChar == UCP(0xFE50) ) {
				*charKind = UCK_comma;			// U+FE50, small comma.
			} else if ( *uniChar == UCP(0xFE51) ) {
				*charKind = UCK_comma;			// U+FE51, small ideographic comma.
			} else if ( *uniChar == UCP(0xFE54) ) {
				*charKind = UCK_semicolon;		// U+FE54, small semicolon.
			}

		} else if ( upperBits == 0x30 ) {	// U+30--

			if ( *uniChar == UCP(0x3000) ) {
				*charKind = UCK_space;			// U+3000, ideographic space.
			} else if ( *uniChar == UCP(0x3001) ) {
				*charKind = UCK_comma;			// U+3001, ideographic comma.
			} else if ( (UCP(0x3008) <= *uniChar) && (*uniChar <= UCP(0x300F)) ) {
				*charKind = UCK_quote;			// U+3008..U+300F, various quotes.
			} else if ( *uniChar == UCP(0x303F) ) {
				*charKind = UCK_space;			// U+303F, ideographic half fill space.
			} else if ( (UCP(0x301D) <= *uniChar) && (*uniChar <= UCP(0x301F)) ) {
				*charKind = UCK_quote;			// U+301D..U+301F, double prime quotes.
			}

		} else if ( upperBits == 0x20 ) {	// U+20--

			if ( (UCP(0x2000) <= *uniChar) && (*uniChar <= UCP(0x200B)) ) {
				*charKind = UCK_space;			// U+2000..U+200B, en quad through zero width space.
			} else if ( *uniChar == UCP(0x2015) ) {
				*charKind = UCK_quote;			// U+2015, dash quote.
			} else if ( (UCP(0x2018) <= *uniChar) && (*uniChar <= UCP(0x201F)) ) {
				*charKind = UCK_quote;			// U+2018..U+201F, various quotes.
			} else if ( *uniChar == UCP(0x2028) ) {
				*charKind = UCK_control;			// U+2028, line separator.
			} else if ( *uniChar == UCP(0x2029) ) {
				*charKind = UCK_control;			// U+2029, paragraph separator.
			} else if ( (*uniChar == UCP(0x2039)) || (*uniChar == UCP(0x203A)) ) {
				*charKind = UCK_quote;			// U+2039 and U+203A, guillemet quotes.
			}

		} else if ( upperBits == 0x06 ) {	// U+06--

			if ( *uniChar == UCP(0x060C) ) {
				*charKind = UCK_comma;			// U+060C, Arabic comma.
			} else if ( *uniChar == UCP(0x061B) ) {
				*charKind = UCK_semicolon;		// U+061B, Arabic semicolon.
			}

		} else if ( upperBits == 0x05 ) {	// U+05--

			if ( *uniChar == UCP(0x055D) ) {
				*charKind = UCK_comma;			// U+055D, Armenian comma.
			}

		} else if ( upperBits == 0x03 ) {	// U+03--

			if ( *uniChar == UCP(0x037E) ) {
				*charKind = UCK_semicolon;		// U+037E, Greek "semicolon" (really a question mark).
			}

		} else if ( upperBits == 0x00 ) {	// U+00--

			if ( (*uniChar == UCP(0x00AB)) || (*uniChar == UCP(0x00BB)) ) {
				*charKind = UCK_quote;			// U+00AB and U+00BB, guillemet quotes.
			}

		}
				
	}

}	// ClassifyCharacter


// -------------------------------------------------------------------------------------------------
// IsClosingingQuote
// -----------------

static inline bool
IsClosingingQuote ( UniCodePoint uniChar, UniCodePoint openQuote, UniCodePoint closeQuote )
{

	if ( (uniChar == closeQuote) ||
		 ( (openQuote == UCP(0x301D)) && ((uniChar == UCP(0x301E)) || (uniChar == UCP(0x301F))) ) ) {
		return true;
	} else {
		return false;
	}

}	// IsClosingingQuote


// -------------------------------------------------------------------------------------------------
// IsSurroundingQuote
// ------------------

static inline bool
IsSurroundingQuote ( UniCodePoint uniChar, UniCodePoint openQuote, UniCodePoint closeQuote )
{

	if ( (uniChar == openQuote) || IsClosingingQuote ( uniChar, openQuote, closeQuote ) ) {
		return true;
	} else {
		return false;
	}

}	// IsSurroundingQuote


// -------------------------------------------------------------------------------------------------
// GetClosingQuote
// ---------------

static UniCodePoint
GetClosingQuote ( UniCodePoint openQuote )
{
	UniCodePoint	closeQuote;
	
	switch ( openQuote ) {

		case UCP(0x0022) : closeQuote = UCP(0x0022);	// ! U+0022 is both opening and closing.
						   break;
		// *** [2674672] Discontinue to interpret square brackets 
		// *** as Asian quotes in XMPUtils::SeparateArrayItems(..))
		// *** case UCP(0x005B) : closeQuote = UCP(0x005D);
		// ***				   break;
		case UCP(0x00AB) : closeQuote = UCP(0x00BB);	// ! U+00AB and U+00BB are reversible.
						   break;
		case UCP(0x00BB) : closeQuote = UCP(0x00AB);
						   break;
		case UCP(0x2015) : closeQuote = UCP(0x2015);	// ! U+2015 is both opening and closing.
						   break;
		case UCP(0x2018) : closeQuote = UCP(0x2019);
						   break;
		case UCP(0x201A) : closeQuote = UCP(0x201B);
						   break;
		case UCP(0x201C) : closeQuote = UCP(0x201D);
						   break;
		case UCP(0x201E) : closeQuote = UCP(0x201F);
						   break;
		case UCP(0x2039) : closeQuote = UCP(0x203A);	// ! U+2039 and U+203A are reversible.
						   break;
		case UCP(0x203A) : closeQuote = UCP(0x2039);
						   break;
		case UCP(0x3008) : closeQuote = UCP(0x3009);
						   break;
		case UCP(0x300A) : closeQuote = UCP(0x300B);
						   break;
		case UCP(0x300C) : closeQuote = UCP(0x300D);
						   break;
		case UCP(0x300E) : closeQuote = UCP(0x300F);
						   break;
		case UCP(0x301D) : closeQuote = UCP(0x301F);	// ! U+301E also closes U+301D.
						   break;
		default			 : closeQuote = 0;
						   break;

	}
	
	return closeQuote;
	
}	// GetClosingQuote


// -------------------------------------------------------------------------------------------------
// CodePointToUTF8
// ---------------

static void
CodePointToUTF8 ( UniCodePoint uniChar, XMP_VarString & utf8Str )
{
	size_t i, byteCount;
	XMP_Uns8 buffer [8];
	UniCodePoint cpTemp;
	
	if ( uniChar <= 0x7F ) {

		i = 7;
		byteCount = 1;
		buffer[7] = char(uniChar);
	
	} else {

		// ---------------------------------------------------------------------------------------
		// Copy the data bits from the low order end to the high order end, include the 0x80 mask.
		
		i = 8;
		cpTemp = uniChar;
		while ( cpTemp != 0 ) {
			-- i;	// Exit with i pointing to the last byte stored.
			buffer[i] = UnsByte(0x80) | (UnsByte(cpTemp) & 0x3F);
			cpTemp = cpTemp >> 6;
		}
		byteCount = 8 - i;	// The total number of bytes needed.
		XMP_Assert ( (2 <= byteCount) && (byteCount <= 6) );

		// -------------------------------------------------------------------------------------
		// Make sure the high order byte can hold the byte count mask, compute and set the mask.
		
		size_t bitCount = 0;	// The number of data bits in the first byte.
		for ( cpTemp = (buffer[i] & UnsByte(0x3F)); cpTemp != 0; cpTemp = cpTemp >> 1 ) bitCount += 1;
		if ( bitCount > (8 - (byteCount + 1)) ) byteCount += 1;
		
		i = 8 - byteCount;	// First byte index and mask shift count.
		XMP_Assert ( (0 <= i) && (i <= 6) );
		buffer[i] |= (UnsByte(0xFF) << i) & UnsByte(0xFF);	// AUDIT: Safe, i is between 0 and 6.
	
	}
	
	utf8Str.assign ( (char*)(&buffer[i]), byteCount );
	
}	// CodePointToUTF8


// -------------------------------------------------------------------------------------------------
// ApplyQuotes
// -----------

static void
ApplyQuotes ( XMP_VarString * item, UniCodePoint openQuote, UniCodePoint closeQuote, bool allowCommas )
{
	bool	prevSpace	= false;
	size_t	charOffset, charLen;
	UniCharKind		charKind;
	UniCodePoint	uniChar;
	
	// -----------------------------------------------------------------------------------------
	// See if there are any separators in the value. Stop at the first occurrance. This is a bit
	// tricky in order to make typical typing work conveniently. The purpose of applying quotes
	// is to preserve the values when splitting them back apart. That is CatenateContainerItems
	// and SeparateContainerItems must round trip properly. For the most part we only look for
	// separators here. Internal quotes, as in -- Irving "Bud" Jones -- won't cause problems in
	// the separation. An initial quote will though, it will make the value look quoted.

	charOffset = 0;
	ClassifyCharacter ( item->c_str(), charOffset, &charKind, &charLen, &uniChar );
	
	if ( charKind != UCK_quote ) {
	
	for ( charOffset = 0; size_t(charOffset) < item->size(); charOffset += charLen ) {

			ClassifyCharacter ( item->c_str(), charOffset, &charKind, &charLen, &uniChar );

			if ( charKind == UCK_space ) {
				if ( prevSpace ) break; // Multiple spaces are a separator.
				prevSpace = true;
			} else {
				prevSpace = false;
				if ( (charKind == UCK_semicolon) || (charKind == UCK_control) ) break;
				if ( (charKind == UCK_comma) && (! allowCommas) ) break;
			}

		}
	
	}
	
	if ( size_t(charOffset) < item->size() ) {
	
		// --------------------------------------------------------------------------------------
		// Create a quoted copy, doubling any internal quotes that match the outer ones. Internal
		// quotes did not stop the "needs quoting" search, but they do need doubling. So we have
		// to rescan the front of the string for quotes. Handle the special case of U+301D being
		// closed by either U+301E or U+301F.
		
		XMP_VarString	newItem;
		size_t			splitPoint;
		
		for ( splitPoint = 0; splitPoint <= charOffset; ++splitPoint ) {
			ClassifyCharacter ( item->c_str(), splitPoint, &charKind, &charLen, &uniChar );
			if ( charKind == UCK_quote ) break;
		}
		
		CodePointToUTF8 ( openQuote, newItem );
		newItem.append ( *item, 0, splitPoint );	// Copy the leading "normal" portion.

		for ( charOffset = splitPoint; size_t(charOffset) < item->size(); charOffset += charLen ) {
			ClassifyCharacter ( item->c_str(), charOffset, &charKind, &charLen, &uniChar );
			newItem.append ( *item, charOffset, charLen );
			if ( (charKind == UCK_quote) && IsSurroundingQuote ( uniChar, openQuote, closeQuote ) ) {
				newItem.append ( *item, charOffset, charLen );
			}
		}
		
		XMP_VarString closeStr;
		CodePointToUTF8 ( closeQuote, closeStr );
		newItem.append ( closeStr );
		
		*item = newItem;
	
	}
	
}	// ApplyQuotes


// -------------------------------------------------------------------------------------------------
// IsInternalProperty
// ------------------

// *** Need static checks of the schema prefixes!

static const char * kExternalxmpDM[] =
	{ "xmpDM:album",
	  "xmpDM:altTapeName",
	  "xmpDM:altTimecode",
	  "xmpDM:artist",
	  "xmpDM:cameraAngle",
	  "xmpDM:cameraLabel",
	  "xmpDM:cameraModel",
	  "xmpDM:cameraMove",
	  "xmpDM:client",
	  "xmpDM:comment",
	  "xmpDM:composer",
	  "xmpDM:director",
	  "xmpDM:directorPhotography",
	  "xmpDM:engineer",
	  "xmpDM:genre",
	  "xmpDM:good",
	  "xmpDM:instrument",
	  "xmpDM:logComment",
	  "xmpDM:projectName",
	  "xmpDM:releaseDate",
	  "xmpDM:scene",
	  "xmpDM:shotDate",
	  "xmpDM:shotDay",
	  "xmpDM:shotLocation",
	  "xmpDM:shotName",
	  "xmpDM:shotNumber",
	  "xmpDM:shotSize",
	  "xmpDM:speakerPlacement",
	  "xmpDM:takeNumber",
	  "xmpDM:tapeName",
	  "xmpDM:trackNumber",
	  "xmpDM:videoAlphaMode",
	  "xmpDM:videoAlphaPremultipleColor",
	  0 };	// ! Must have zero sentinel!

typedef const char ** CharStarIterator;	// Used for binary search of kExternalxmpDM;
static const char ** kLastExternalxmpDM = 0;	// Set on first use.
static int CharStarLess (const char * left, const char * right )
	{ return (strcmp ( left, right ) < 0); }

#define IsExternalProperty(s,p) (! IsInternalProperty ( s, p ))

bool
IsInternalProperty ( const XMP_VarString & schema, const XMP_VarString & prop )
{
	bool isInternal = false;

	if ( schema == kXMP_NS_DC ) {
	
		if ( (prop == "dc:format")	||
			 (prop == "dc:language") ) {
			isInternal = true;
		}
	
	} else if ( schema == kXMP_NS_XMP ) {
	
		if ( (prop == "xmp:BaseURL")		||
			 (prop == "xmp:CreatorTool")	||
			 (prop == "xmp:Format")			||
			 (prop == "xmp:Locale")			||
			 (prop == "xmp:MetadataDate")	||
			 (prop == "xmp:ModifyDate") ) {
			isInternal = true;
		}
	
	} else if ( schema == kXMP_NS_PDF ) {
	
		if ( (prop == "pdf:BaseURL")	||
			 (prop == "pdf:Creator")	||
			 (prop == "pdf:ModDate")	||
			 (prop == "pdf:PDFVersion") ||
			 (prop == "pdf:Producer") ) {
			isInternal = true;
		}
	
	} else if ( schema == kXMP_NS_TIFF ) {
		
		isInternal = true;	// ! The TIFF properties are internal by default.
		if ( (prop == "tiff:ImageDescription")	||	// ! ImageDescription, Artist, and Copyright are aliased.
			 (prop == "tiff:Artist")			||
			 (prop == "tiff:Copyright") ) {
			isInternal = false;
		}

	} else if ( schema == kXMP_NS_EXIF ) {
	
		isInternal = true;	// ! The EXIF properties are internal by default.
		if ( prop == "exif:UserComment" ) isInternal = false;

	} else if ( schema == kXMP_NS_EXIF_Aux ) {
	
		isInternal = true;	// ! The EXIF Aux properties are internal by default.
	
	} else if ( schema == kXMP_NS_Photoshop ) {
	
		if ( (prop == "photoshop:ICCProfile")  ||
			 (prop == "photoshop:TextLayers") ) isInternal = true;
	
	} else if ( schema == kXMP_NS_CameraRaw ) {
	
		isInternal = true;	// All of crs: is internal, they are processing settings.

	} else if ( schema == kXMP_NS_DM ) {
	
		// ! Most of the xmpDM schema is internal, and unknown properties default to internal.
		if ( kLastExternalxmpDM == 0 ) {
			for ( kLastExternalxmpDM = &kExternalxmpDM[0]; *kLastExternalxmpDM != 0; ++kLastExternalxmpDM ) {}
		}
		isInternal = (! std::binary_search ( &kExternalxmpDM[0], kLastExternalxmpDM, prop.c_str(), CharStarLess ));

	} else if ( schema == kXMP_NS_Script ) {
	
		isInternal = true;	// ! Most of the xmpScript schema is internal, and unknown properties default to internal.
		if ( (prop == "xmpScript:action") || (prop == "xmpScript:character") || (prop == "xmpScript:dialog") || 
			 (prop == "xmpScript:sceneSetting") || (prop == "xmpScript:sceneTimeOfDay") ) {
			isInternal = false;
		}
	
	} else if ( schema == kXMP_NS_BWF ) {
	
		if ( prop == "bext:version" ) isInternal = true;

	} else if ( schema == kXMP_NS_AdobeStockPhoto ) {
	
		isInternal = true;	// ! The bmsp schema has only internal properties.

	} else if ( schema == kXMP_NS_XMP_MM ) {
	
		isInternal = true;	// ! The xmpMM schema has only internal properties.
	
	} else if ( schema == kXMP_NS_XMP_Text ) {
	
		isInternal = true;	// ! The xmpT schema has only internal properties.
	
	} else if ( schema == kXMP_NS_XMP_PagedFile ) {
	
		isInternal = true;	// ! The xmpTPg schema has only internal properties.
	
	} else if ( schema == kXMP_NS_XMP_Graphics ) {
	
		isInternal = true;	// ! The xmpG schema has only internal properties.
	
	} else if ( schema == kXMP_NS_XMP_Image ) {
	
		isInternal = true;	// ! The xmpGImg schema has only internal properties.
	
	} else if ( schema == kXMP_NS_XMP_Font ) {
	
		isInternal = true;	// ! The stFNT schema has only internal properties.
	
	}
	
	return isInternal;

}	// IsInternalProperty


// -------------------------------------------------------------------------------------------------
// RemoveSchemaChildren
// --------------------

static void
RemoveSchemaChildren ( XMP_NodePtrPos schemaPos, bool doAll )
{
	XMP_Node * schemaNode = *schemaPos;
	XMP_Assert ( XMP_NodeIsSchema ( schemaNode->options ) );
		
	// ! Iterate backwards to reduce shuffling as children are erased and to simplify the logic for
	// ! denoting the current child. (Erasing child n makes the old n+1 now be n.)

	size_t		   propCount = schemaNode->children.size();
	XMP_NodePtrPos beginPos	 = schemaNode->children.begin();
	
	for ( size_t propNum = propCount-1, propLim = (size_t)(-1); propNum != propLim; --propNum ) {
		XMP_NodePtrPos currProp = beginPos + propNum;
		if ( doAll || IsExternalProperty ( schemaNode->name, (*currProp)->name ) ) {
			delete *currProp;	// ! Both delete the node and erase the pointer from the parent.
			schemaNode->children.erase ( currProp );
		}
	}
	
	if ( schemaNode->children.empty() ) {
		XMP_Node * tree = schemaNode->parent;
		tree->children.erase ( schemaPos );
		delete schemaNode;
	}

}	// RemoveSchemaChildren


// -------------------------------------------------------------------------------------------------
// ItemValuesMatch
// ---------------
//
// Does the value comparisons for array merging as part of XMPUtils::AppendProperties.

static bool
ItemValuesMatch ( const XMP_Node * leftNode, const XMP_Node * rightNode )
{
	const XMP_OptionBits leftForm  = leftNode->options & kXMP_PropCompositeMask;
	const XMP_OptionBits rightForm = leftNode->options & kXMP_PropCompositeMask;
	
	if ( leftForm != rightForm ) return false;
	
	if ( leftForm == 0 ) {
	
		// Simple nodes, check the values and xml:lang qualifiers.
		
		if ( leftNode->value != rightNode->value ) return false;
		if ( (leftNode->options & kXMP_PropHasLang) != (rightNode->options & kXMP_PropHasLang) ) return false;
		if ( leftNode->options & kXMP_PropHasLang ) {
			if ( leftNode->qualifiers[0]->value != rightNode->qualifiers[0]->value ) return false;
		}
	
	} else if ( leftForm == kXMP_PropValueIsStruct ) {
	
		// Struct nodes, see if all fields match, ignoring order.
		
		if ( leftNode->children.size() != rightNode->children.size() ) return false;

		for ( size_t leftNum = 0, leftLim = leftNode->children.size(); leftNum != leftLim; ++leftNum ) {
			const XMP_Node * leftField	= leftNode->children[leftNum];
			const XMP_Node * rightField = FindConstChild ( rightNode, leftField->name.c_str() );
			if ( (rightField == 0) || (! ItemValuesMatch ( leftField, rightField )) ) return false;
		}
		
	} else {
	
		// Array nodes, see if the "leftNode" values are present in the "rightNode", ignoring order, duplicates,
		// and extra values in the rightNode-> The rightNode is the destination for AppendProperties.

		XMP_Assert ( leftForm & kXMP_PropValueIsArray );
		
		for ( size_t leftNum = 0, leftLim = leftNode->children.size(); leftNum != leftLim; ++leftNum ) {

			const XMP_Node * leftItem = leftNode->children[leftNum];

			size_t rightNum, rightLim;
			for ( rightNum = 0, rightLim = rightNode->children.size(); rightNum != rightLim; ++rightNum ) {
				const XMP_Node * rightItem = rightNode->children[rightNum];
				if ( ItemValuesMatch ( leftItem, rightItem ) ) break;
			}
			if ( rightNum == rightLim ) return false;

		}
	
	}

	return true;	// All of the checks passed.
	
}	// ItemValuesMatch


// -------------------------------------------------------------------------------------------------
// AppendSubtree
// -------------
//
// The main implementation of XMPUtils::AppendProperties. See the description in TXMPMeta.hpp.

static void
AppendSubtree ( const XMP_Node * sourceNode, XMP_Node * destParent,
				const bool mergeCompound, const bool replaceOld, const bool deleteEmpty )
{
	XMP_NodePtrPos destPos;
	XMP_Node * destNode = FindChildNode ( destParent, sourceNode->name.c_str(), kXMP_ExistingOnly, &destPos );
	
	bool valueIsEmpty = false;
	if ( XMP_PropIsSimple ( sourceNode->options ) ) {
		valueIsEmpty = sourceNode->value.empty();
	} else {
		valueIsEmpty = sourceNode->children.empty();
	}

	if ( valueIsEmpty ) {
		if ( deleteEmpty && (destNode != 0) ) {
			delete ( destNode );
			destParent->children.erase ( destPos );
		}
		return;	// ! Done, empty values are either ignored or cause deletions.
	}
	
	if ( destNode == 0 ) {
		// The one easy case, the destination does not exist.
		destNode = CloneSubtree ( sourceNode, destParent, true /* skipEmpty */ );
		XMP_Assert ( (destNode == 0) || (! destNode->value.empty()) || (! destNode->children.empty()) );
		return;
	}
	
	// If we get here we're going to modify an existing property, either replacing or merging.
	
	XMP_Assert ( (! valueIsEmpty) && (destNode != 0) );

	XMP_OptionBits sourceForm = sourceNode->options & kXMP_PropCompositeMask;
	XMP_OptionBits destForm	  = destNode->options & kXMP_PropCompositeMask;
	
	bool replaceThis = replaceOld;	// ! Don't modify replaceOld, it gets passed to inner calls.
	if ( mergeCompound && (! XMP_PropIsSimple ( sourceForm )) ) replaceThis = false;

	if ( replaceThis ) {

		destNode->value	  = sourceNode->value;	// *** Should use SetNode.
		destNode->options = sourceNode->options;
		destNode->RemoveChildren();
		destNode->RemoveQualifiers();
		CloneOffspring ( sourceNode, destNode, true /* skipEmpty */ );
		
		if ( (! XMP_PropIsSimple ( destNode->options )) && destNode->children.empty() ) {
			// Don't keep an empty array or struct. The source might be implicitly empty due to
			// all children being empty. In this case CloneOffspring should skip them.
			DeleteSubtree ( destPos );
		}

		return;

	}
	
	// From here on are cases for merging arrays or structs.
	
	if ( XMP_PropIsSimple ( sourceForm ) || (sourceForm != destForm) ) return;
	
	if ( sourceForm == kXMP_PropValueIsStruct ) {
	
		// To merge a struct process the fields recursively. E.g. add simple missing fields. The
		// recursive call to AppendSubtree will handle deletion for fields with empty values.

		for ( size_t sourceNum = 0, sourceLim = sourceNode->children.size(); sourceNum != sourceLim && destNode!= NULL; ++sourceNum ) {
			const XMP_Node * sourceField = sourceNode->children[sourceNum];
			AppendSubtree ( sourceField, destNode, mergeCompound, replaceOld, deleteEmpty );

			if (deleteEmpty && destNode->children.empty())
			{
				delete (destNode);
				destNode = NULL;
				destParent->children.erase(destPos);
			}
		}
		
	} else if ( sourceForm & kXMP_PropArrayIsAltText ) {
	
		// Merge AltText arrays by the xml:lang qualifiers. Make sure x-default is first. Make a
		// special check for deletion of empty values. Meaningful in AltText arrays because the
		// xml:lang qualifier provides unambiguous source/dest correspondence.
	
		XMP_Assert ( mergeCompound );

		for ( size_t sourceNum = 0, sourceLim = sourceNode->children.size(); sourceNum != sourceLim && destNode!= NULL; ++sourceNum ) {

			const XMP_Node * sourceItem = sourceNode->children[sourceNum];
			if ( sourceItem->qualifiers.empty() || (sourceItem->qualifiers[0]->name != "xml:lang") ) continue;
			
			XMP_Index destIndex = LookupLangItem ( destNode, sourceItem->qualifiers[0]->value );
			
			if ( sourceItem->value.empty() ) {

				if ( deleteEmpty && (destIndex != -1) ) {
					delete ( destNode->children[destIndex] );
					destNode->children.erase ( destNode->children.begin() + destIndex );
					if ( destNode->children.empty() ) {
						delete ( destNode );
						destParent->children.erase ( destPos );
					}
				}

			} else {
			
				if ( destIndex != -1 ) {
				
					// The source and dest arrays both have this language item.
					
					if ( replaceOld ) {	// ! Yes, check replaceOld not replaceThis!
						destNode->children[destIndex]->value = sourceItem->value;
					}
				
				} else {
				
					// The dest array does not have this language item, add it.
					
					if ( (sourceItem->qualifiers[0]->value != "x-default") || destNode->children.empty() ) {
						// Typical case, empty dest array or not "x-default". Non-empty should always have "x-default".
						CloneSubtree ( sourceItem, destNode, true /* skipEmpty */ );
					} else {
						// Edge case, non-empty dest array had no "x-default", insert that at the beginning.
						XMP_Node * destItem = new XMP_Node ( destNode, sourceItem->name, sourceItem->value, sourceItem->options );
						CloneOffspring ( sourceItem, destItem, true /* skipEmpty */ );
						destNode->children.insert ( destNode->children.begin(), destItem );
					}
				
				}
			
			}

		}
	
	} else if ( sourceForm & kXMP_PropValueIsArray ) {
	
		// Merge other arrays by item values. Don't worry about order or duplicates. Source 
		// items with empty values do not cause deletion, that conflicts horribly with merging.

		for ( size_t sourceNum = 0, sourceLim = sourceNode->children.size(); sourceNum != sourceLim; ++sourceNum ) {
			const XMP_Node * sourceItem = sourceNode->children[sourceNum];

			size_t	destNum, destLim;
			for ( destNum = 0, destLim = destNode->children.size(); destNum != destLim; ++destNum ) {
				const XMP_Node * destItem = destNode->children[destNum];
				if ( ItemValuesMatch ( sourceItem, destItem ) ) break;
			}
			if ( destNum == destLim ) CloneSubtree ( sourceItem, destNode, true /* skipEmpty */ );

		}
		
	}

}	// AppendSubtree


// =================================================================================================
// Class Static Functions
// ======================

// -------------------------------------------------------------------------------------------------
// CatenateArrayItems
// ------------------


#if ENABLE_CPP_DOM_MODEL
// -------------------------------------------------------------------------------------------------
// CatenateArrayItems_v2
// ------------------

/* class static */ void
XMPUtils::CatenateArrayItems_v2(const XMPMeta & ptr,
								XMP_StringPtr   schemaNS,
								XMP_StringPtr   arrayName,
								XMP_StringPtr   separator,
								XMP_StringPtr   quotes,
								XMP_OptionBits  options,
								XMP_VarString * catedStr)
{
	using namespace AdobeXMPCore;
	using namespace AdobeXMPCommon;

	if(sUseNewCoreAPIs) {
		const XMPMeta2 & xmpObj = dynamic_cast<const XMPMeta2 &>(ptr);
		XMP_Assert((schemaNS != 0) && (arrayName != 0)); // ! Enforced by wrapper.
		XMP_Assert((separator != 0) && (quotes != 0) && (catedStr != 0)); // ! Enforced by wrapper.

		size_t		 strLen, strPos, charLen;
		UniCharKind	 charKind;
		UniCodePoint currUCP, openQuote, closeQuote;

		const bool allowCommas = ((options & kXMPUtil_AllowCommas) != 0);

		spINode arrayNode; // ! Move up to avoid gcc complaints.
		XMP_OptionBits	 arrayForm = 0, arrayOptions = 0;
		spcINode  currItem;

		// Make sure the separator is OK. It must be one semicolon surrounded by zero or more spaces.
		// Any of the recognized semicolons or spaces are allowed.

		strPos = 0;
		strLen = strlen(separator);
		bool haveSemicolon = false;

		while (strPos < strLen) {
			ClassifyCharacter(separator, strPos, &charKind, &charLen, &currUCP);
			strPos += charLen;
			if (charKind == UCK_semicolon) {
				if (haveSemicolon) XMP_Throw("Separator can have only one semicolon", kXMPErr_BadParam);
				haveSemicolon = true;
			}
			else if (charKind != UCK_space) {
				XMP_Throw("Separator can have only spaces and one semicolon", kXMPErr_BadParam);
			}
		};
		if (!haveSemicolon) XMP_Throw("Separator must have one semicolon", kXMPErr_BadParam);

		// Make sure the open and close quotes are a legitimate pair.

		strLen = strlen(quotes);
		ClassifyCharacter(quotes, 0, &charKind, &charLen, &openQuote);
		if (charKind != UCK_quote) XMP_Throw("Invalid quoting character", kXMPErr_BadParam);

		if (charLen == strLen) {
			closeQuote = openQuote;
		}
		else {
			strPos = charLen;
			ClassifyCharacter(quotes, strPos, &charKind, &charLen, &closeQuote);
			if (charKind != UCK_quote) XMP_Throw("Invalid quoting character", kXMPErr_BadParam);
			if ((strPos + charLen) != strLen) XMP_Throw("Quoting string too long", kXMPErr_BadParam);
		}
		if (closeQuote != GetClosingQuote(openQuote)) XMP_Throw("Mismatched quote pair", kXMPErr_BadParam);

		// Return an empty result if the array does not exist, hurl if it isn't the right form.

		catedStr->erase();

		XMP_ExpandedXPath arrayPath;
		ExpandXPath(schemaNS, arrayName, &arrayPath);

		XMPUtils::FindCnstNode((xmpObj.mDOM), arrayPath, arrayNode, &arrayOptions);

		if (!arrayNode) return;

		arrayForm = arrayOptions & kXMP_PropCompositeMask;
		if ((!(arrayForm & kXMP_PropValueIsArray)) || (arrayForm & kXMP_PropArrayIsAlternate)) {
			XMP_Throw("Named property must be non-alternate array", kXMPErr_BadParam);
		}
		size_t arrayChildCount = XMPUtils::GetNodeChildCount(arrayNode);
		if (!arrayChildCount) return;

		// Build the result, quoting the array items, adding separators. Hurl if any item isn't simple.
		// Start the result with the first value, then add the rest with a preceeding separator.

		spcINodeIterator arrayIter = XMPUtils::GetNodeChildIterator(arrayNode);

		if ((XMPUtils::GetIXMPOptions(currItem) & kXMP_PropCompositeMask) != 0) XMP_Throw("Array items must be simple", kXMPErr_BadParam);

		*catedStr = arrayIter->GetNode()->ConvertToSimpleNode()->GetValue()->c_str();
		ApplyQuotes(catedStr, openQuote, closeQuote, allowCommas);

		//ArrayNodes in the new DOM are homogeneous so need to check types of other items in the arary if the first one is Simple
		for (arrayIter = arrayIter->Next(); arrayIter; arrayIter = arrayIter->Next()) {

			XMP_VarString tempStr( arrayIter->GetNode()->ConvertToSimpleNode()->GetValue()->c_str());
			ApplyQuotes(&tempStr, openQuote, closeQuote, allowCommas);
			*catedStr += separator;
			*catedStr += tempStr;
		}
	}
	else {
		return;
	}


}	// CatenateArrayItems_v2

#endif
// -------------------------------------------------------------------------------------------------
/* class static */ void
XMPUtils::CatenateArrayItems ( const XMPMeta & xmpObj,
							   XMP_StringPtr   schemaNS,
							   XMP_StringPtr   arrayName,
							   XMP_StringPtr   separator,
							   XMP_StringPtr   quotes,
							   XMP_OptionBits  options,
							   XMP_VarString * catedStr )
{

#if ENABLE_CPP_DOM_MODEL
	
	if(sUseNewCoreAPIs) {

		//dynamic_cast<const XMPMeta2 &>(xmpObj);
		CatenateArrayItems_v2(xmpObj, schemaNS, arrayName, separator, quotes, options, catedStr);
		return;

	}
	
#endif

	XMP_Assert ( (schemaNS != 0) && (arrayName != 0) ); // ! Enforced by wrapper.
	XMP_Assert ( (separator != 0) && (quotes != 0) && (catedStr != 0) ); // ! Enforced by wrapper.
	
	size_t		 strLen, strPos, charLen;
	UniCharKind	 charKind;
	UniCodePoint currUCP, openQuote, closeQuote;
	
	const bool allowCommas = ((options & kXMPUtil_AllowCommas) != 0);
	
	const XMP_Node * arrayNode = 0; // ! Move up to avoid gcc complaints.
	XMP_OptionBits	 arrayForm = 0;
	const XMP_Node * currItem  = 0;

	// Make sure the separator is OK. It must be one semicolon surrounded by zero or more spaces.
	// Any of the recognized semicolons or spaces are allowed.
	
	strPos = 0;
	strLen = strlen ( separator );
	bool haveSemicolon = false;
	
	while ( strPos < strLen ) {
		ClassifyCharacter ( separator, strPos, &charKind, &charLen, &currUCP );
		strPos += charLen;
		if ( charKind == UCK_semicolon ) {
			if ( haveSemicolon ) XMP_Throw ( "Separator can have only one semicolon",  kXMPErr_BadParam );
			haveSemicolon = true;
		} else if ( charKind != UCK_space ) {
			XMP_Throw ( "Separator can have only spaces and one semicolon",	 kXMPErr_BadParam );
		}
	};
	if ( ! haveSemicolon ) XMP_Throw ( "Separator must have one semicolon",	 kXMPErr_BadParam );
	
	// Make sure the open and close quotes are a legitimate pair.

	strLen = strlen ( quotes );
	ClassifyCharacter ( quotes, 0, &charKind, &charLen, &openQuote );
	if ( charKind != UCK_quote ) XMP_Throw ( "Invalid quoting character", kXMPErr_BadParam );

	if ( charLen == strLen ) {
		closeQuote = openQuote;
	} else {
		strPos = charLen;
		ClassifyCharacter ( quotes, strPos, &charKind, &charLen, &closeQuote );
		if ( charKind != UCK_quote ) XMP_Throw ( "Invalid quoting character", kXMPErr_BadParam );
		if ( (strPos + charLen) != strLen ) XMP_Throw ( "Quoting string too long", kXMPErr_BadParam );
	}
	if ( closeQuote != GetClosingQuote ( openQuote ) ) XMP_Throw ( "Mismatched quote pair", kXMPErr_BadParam );

	// Return an empty result if the array does not exist, hurl if it isn't the right form.
	
	catedStr->erase();

	XMP_ExpandedXPath arrayPath;
	ExpandXPath ( schemaNS, arrayName, &arrayPath );

	arrayNode = FindConstNode ( &xmpObj.tree, arrayPath );
	if ( arrayNode == 0 ) return;

	arrayForm = arrayNode->options & kXMP_PropCompositeMask;
	if ( (! (arrayForm & kXMP_PropValueIsArray)) || (arrayForm & kXMP_PropArrayIsAlternate) ) {
		XMP_Throw ( "Named property must be non-alternate array", kXMPErr_BadParam );
	}
	if ( arrayNode->children.empty() ) return;
	
	// Build the result, quoting the array items, adding separators. Hurl if any item isn't simple.
	// Start the result with the first value, then add the rest with a preceeding separator.
	
	currItem = arrayNode->children[0];
	
	if ( (currItem->options & kXMP_PropCompositeMask) != 0 ) XMP_Throw ( "Array items must be simple", kXMPErr_BadParam );
	*catedStr = currItem->value;
	ApplyQuotes ( catedStr, openQuote, closeQuote, allowCommas );
	
	for ( size_t itemNum = 1, itemLim = arrayNode->children.size(); itemNum != itemLim; ++itemNum ) {
		const XMP_Node * item = arrayNode->children[itemNum];
		if ( (item->options & kXMP_PropCompositeMask) != 0 ) XMP_Throw ( "Array items must be simple", kXMPErr_BadParam );
		XMP_VarString tempStr ( item->value );
		ApplyQuotes ( &tempStr, openQuote, closeQuote, allowCommas );
		*catedStr += separator;
		*catedStr += tempStr;
	}

}	// CatenateArrayItems


// -------------------------------------------------------------------------------------------------
// SeparateArrayItems_v2
// ------------------
#if ENABLE_CPP_DOM_MODEL
/* class static */ void
XMPUtils::SeparateArrayItems_v2(XMPMeta *	  xmpObj2,
XMP_StringPtr  schemaNS,
XMP_StringPtr  arrayName,
XMP_OptionBits options,
XMP_StringPtr  catedStr)
{

#if ENABLE_CPP_DOM_MODEL
	using namespace AdobeXMPCore;
	using namespace AdobeXMPCommon;
	XMPMeta2 * xmpObj = NULL;
	if(sUseNewCoreAPIs) {
		xmpObj = dynamic_cast<XMPMeta2 *> (xmpObj2);
	}
	
#endif
	XMP_Assert((schemaNS != 0) && (arrayName != 0) && (catedStr != 0));	// ! Enforced by wrapper.
	// TODO - check if the array item name should be arrayname one or karrayitem 
	// TODO - check the find array in case array doesn't already exist
	XMP_VarString itemValue;
	size_t itemStart, itemEnd;
	size_t nextSize, charSize = 0;
	UniCharKind	  nextKind, charKind = UCK_normal;
	UniCodePoint  nextChar, uniChar = 0;
	XMP_OptionBits arrayOptions = 0;


	bool preserveCommas = false;
	if (options & kXMPUtil_AllowCommas) {
		preserveCommas = true;
		options ^= kXMPUtil_AllowCommas;
	}

	options = VerifySetOptions(options, 0);
	if (options & ~kXMP_PropArrayFormMask) XMP_Throw("Options can only provide array form", kXMPErr_BadOptions);

	// Find the array node, make sure it is OK. Move the current children aside, to be readded later if kept.

	XMP_ExpandedXPath arrayPath;
	ExpandXPath(schemaNS, arrayName, &arrayPath);
	spINode arrayNode;
	if (XMPUtils::FindCnstNode(xmpObj->mDOM, arrayPath, arrayNode, &arrayOptions)){

		XMP_OptionBits arrayForm = arrayOptions & kXMP_PropArrayFormMask;
		if ((arrayForm == 0) || (arrayForm & kXMP_PropArrayIsAlternate)) {
			XMP_Throw("Named property must be non-alternate array", kXMPErr_BadXPath);
		}

		if ((options != 0) && (options != arrayForm)) XMP_Throw("Mismatch of specified and existing array form", kXMPErr_BadXPath);	// *** Right error?
	}
	else {
		// The array does not exist, try to create it.

		XPathStepInfo  lastPathSegment(arrayPath.back());
		XMP_VarString arrayStep = lastPathSegment.step;
		//arrayPath.pop_back();
		spINode destNode;
		XMP_Index insertIndex = 0;
		if (!XMPUtils::FindNode(xmpObj->mDOM, arrayPath, kXMP_CreateNodes, options, destNode, &insertIndex, true)) {
			XMP_Throw("Failure creating array node", kXMPErr_BadXPath);
		}
		std::string arrayNameSpace, arrayNameStr;
		auto defaultMap = INameSpacePrefixMap::GetDefaultNameSpacePrefixMap();
		arrayOptions = options;
		XMPUtils::GetNameSpaceAndNameFromStepValue(lastPathSegment.step, defaultMap, arrayNameSpace, arrayNameStr);
		//  Need to check Alternate first
		if (arrayOptions & kXMP_PropArrayIsAlternate) {
			arrayNode = IArrayNode::CreateAlternativeArrayNode( arrayNameSpace.c_str(), arrayNameSpace.size(), arrayNameStr.c_str(), arrayNameStr.size());
		}
		else if (arrayOptions & kXMP_PropArrayIsOrdered) {
			arrayNode = IArrayNode::CreateOrderedArrayNode( arrayNameSpace.c_str(), arrayNameSpace.size(), arrayNameStr.c_str(), arrayNameStr.size() );
		}
		else if (arrayOptions & kXMP_PropArrayIsUnordered) {
			arrayNode = IArrayNode::CreateUnorderedArrayNode( arrayNameSpace.c_str(), arrayNameSpace.size(), arrayNameStr.c_str(), arrayNameStr.size() );
		}

		else {
			XMP_Throw("Failure creating array node", kXMPErr_BadXPath);
		}
		if (destNode->GetNodeType() == INode::kNTStructure) {

			destNode->ConvertToStructureNode()->InsertNode(arrayNode);
		}
		else if (destNode->GetNodeType() == INode::kNTArray) {

			destNode->ConvertToArrayNode()->AppendNode(arrayNode);
		}
		else {

			XMP_Throw("Failure creating array node", kXMPErr_BadXPath);
		}

		if (!arrayNode) XMP_Throw("Failed to create named array", kXMPErr_BadXPath);
	}


	size_t oldChildCount = XMPUtils::GetNodeChildCount(arrayNode);
	std::vector<XMP_VarString> oldArrayNodes;
	std::vector<spINode> qualifiers;
	
	// used to handle duplicates
	std::vector<bool> oldArrayNodeSeen(oldChildCount, false);
	spcINodeIterator oldArrayChildIter = XMPUtils::GetNodeChildIterator(arrayNode);

	for (; oldArrayChildIter; oldArrayChildIter = oldArrayChildIter->Next()) {

		oldArrayNodes.push_back( oldArrayChildIter->GetNode()->ConvertToSimpleNode()->GetValue()->c_str());
		if (oldArrayChildIter->GetNode()->HasQualifiers()) {

			qualifiers.push_back(oldArrayChildIter->GetNode()->Clone());
			/*for ( auto it = oldArrayChildIter->GetNode()->QualifiersIterator(); it; it = it->Next() ) {
				qualifiers.push_back( it->GetNode()->Clone() );
				}*/
		}
		else {
			qualifiers.push_back(spINode());
		}
		
	}

	arrayNode->Clear(true, false);
	// used to avoid typecasting repeatedly!
	spIArrayNode tempArrayNode = arrayNode->ConvertToArrayNode();

	size_t endPos = strlen(catedStr);

	itemEnd = 0;
	while (itemEnd < endPos) {



		for (itemStart = itemEnd; itemStart < endPos; itemStart += charSize) {
			ClassifyCharacter(catedStr, itemStart, &charKind, &charSize, &uniChar);
			if ((charKind == UCK_normal) || (charKind == UCK_quote)) break;
		}
		if (itemStart >= endPos) break;

		if (charKind != UCK_quote) {



			for (itemEnd = itemStart; itemEnd < endPos; itemEnd += charSize) {

				ClassifyCharacter(catedStr, itemEnd, &charKind, &charSize, &uniChar);

				if ((charKind == UCK_normal) || (charKind == UCK_quote)) continue;
				if ((charKind == UCK_comma) && preserveCommas) continue;
				if (charKind != UCK_space) break;

				if ((itemEnd + charSize) >= endPos) break;	// Anything left?
				ClassifyCharacter(catedStr, (itemEnd + charSize), &nextKind, &nextSize, &nextChar);
				if ((nextKind == UCK_normal) || (nextKind == UCK_quote)) continue;
				if ((nextKind == UCK_comma) && preserveCommas) continue;
				break;	// Have multiple spaces, or a space followed by a separator.

			}

			itemValue.assign(catedStr, itemStart, (itemEnd - itemStart));

		}
		else {

			// Accumulate quoted values into a local string, undoubling internal quotes that
			// match the surrounding quotes. Do not undouble "unmatching" quotes.

			UniCodePoint openQuote = uniChar;
			UniCodePoint closeQuote = GetClosingQuote(openQuote);

			itemStart += charSize;	// Skip the opening quote;
			itemValue.erase();

			for (itemEnd = itemStart; itemEnd < endPos; itemEnd += charSize) {

				ClassifyCharacter(catedStr, itemEnd, &charKind, &charSize, &uniChar);

				if ((charKind != UCK_quote) || (!IsSurroundingQuote(uniChar, openQuote, closeQuote))) {

					// This is not a matching quote, just append it to the item value.
					itemValue.append(catedStr, itemEnd, charSize);

				}
				else {

					// This is a "matching" quote. Is it doubled, or the final closing quote? Tolerate
					// various edge cases like undoubled opening (non-closing) quotes, or end of input.

					if ((itemEnd + charSize) < endPos) {
						ClassifyCharacter(catedStr, itemEnd + charSize, &nextKind, &nextSize, &nextChar);
					}
					else {
						nextKind = UCK_semicolon; nextSize = 0; nextChar = 0x3B;
					}

					if (uniChar == nextChar) {
						// This is doubled, copy it and skip the double.
						itemValue.append(catedStr, itemEnd, charSize);
						itemEnd += nextSize;	// Loop will add in charSize.
					}
					else if (!IsClosingingQuote(uniChar, openQuote, closeQuote)) {
						// This is an undoubled, non-closing quote, copy it.
						itemValue.append(catedStr, itemEnd, charSize);
					}
					else {
						// This is an undoubled closing quote, skip it and exit the loop.
						itemEnd += charSize;
						break;
					}

				}

			}	// Loop to accumulate the quoted value.

		}

		// Add the separated item to the array. Keep a matching old value in case it had separators.

		size_t oldChild;

		spISimpleNode newItem;
		for (oldChild = 1; oldChild <= oldChildCount; ++oldChild) {
			if (!oldArrayNodeSeen[oldChild - 1] && itemValue == oldArrayNodes[oldChild - 1]) break;
		}


		if (oldChild == oldChildCount + 1) {
			//	newItem = new XMP_Node ( arrayNode, kXMP_ArrayItemName, itemValue.c_str(), 0 );
			newItem = ISimpleNode::CreateSimpleNode(arrayNode->GetNameSpace()->c_str(), arrayNode->GetNameSpace()->size(),
				kXMP_ArrayItemName, AdobeXMPCommon::npos, itemValue.c_str());
		}
		else {
			newItem = ISimpleNode::CreateSimpleNode(arrayNode->GetNameSpace()->c_str(), arrayNode->GetNameSpace()->size(),
				kXMP_ArrayItemName, AdobeXMPCommon::npos, oldArrayNodes[oldChild - 1].c_str());
			if (qualifiers[ oldChild - 1] && qualifiers[ oldChild - 1] ->HasQualifiers() ) {

				for (auto it = qualifiers[oldChild - 1] ->QualifiersIterator(); it; it = it->Next()) {
				
					newItem->InsertQualifier(it->GetNode()->Clone());
				}
			}
			oldArrayNodeSeen[oldChild - 1] = true;	// ! Don't match again, let duplicates be seen.
		}

		tempArrayNode->AppendNode(newItem);

	}	// Loop through all of the returned items.

	// Delete any of the old children that were not kept.


}	// SeparateArrayItems_v2
#endif

// -------------------------------------------------------------------------------------------------
// SeparateArrayItems
// ------------------

/* class static */ void
XMPUtils::SeparateArrayItems ( XMPMeta *	  xmpObj,
							   XMP_StringPtr  schemaNS,
							   XMP_StringPtr  arrayName,
							   XMP_OptionBits options,
							   XMP_StringPtr  catedStr )
{

#if ENABLE_CPP_DOM_MODEL
	if (sUseNewCoreAPIs) {
		SeparateArrayItems_v2(xmpObj, schemaNS, arrayName, options, catedStr);
		return;
	}
#endif
	XMP_Assert ( (schemaNS != 0) && (arrayName != 0) && (catedStr != 0) );	// ! Enforced by wrapper.
	
	XMP_VarString itemValue;
	size_t itemStart, itemEnd;
	size_t nextSize, charSize = 0;	// Avoid VS uninit var warnings.
	UniCharKind	  nextKind, charKind = UCK_normal;
	UniCodePoint  nextChar, uniChar = 0;
	
	// Extract "special" option bits, verify and normalize the others.
	
	bool preserveCommas = false;
	if ( options & kXMPUtil_AllowCommas ) {
		preserveCommas = true;
		options ^= kXMPUtil_AllowCommas;
	}

	options = VerifySetOptions ( options, 0 );	// Keep a zero value, has special meaning below.
	if ( options & ~kXMP_PropArrayFormMask ) XMP_Throw ( "Options can only provide array form", kXMPErr_BadOptions );
	
	// Find the array node, make sure it is OK. Move the current children aside, to be readded later if kept.
	
	XMP_ExpandedXPath arrayPath;
	ExpandXPath ( schemaNS, arrayName, &arrayPath );
	XMP_Node * arrayNode = ::FindNode( &xmpObj->tree, arrayPath, kXMP_ExistingOnly );
	
	if ( arrayNode != 0 ) {
		// The array exists, make sure the form is compatible. Zero arrayForm means take what exists.
		XMP_OptionBits arrayForm = arrayNode->options & kXMP_PropArrayFormMask;
		if ( (arrayForm == 0) || (arrayForm & kXMP_PropArrayIsAlternate) ) {
			XMP_Throw ( "Named property must be non-alternate array", kXMPErr_BadXPath );
		}
		if ( (options != 0) && (options != arrayForm) ) XMP_Throw ( "Mismatch of specified and existing array form", kXMPErr_BadXPath );	// *** Right error?
	} else {
		// The array does not exist, try to create it.
		arrayNode = ::FindNode( &xmpObj->tree, arrayPath, kXMP_CreateNodes, (options | kXMP_PropValueIsArray) );
		if ( arrayNode == 0 ) XMP_Throw ( "Failed to create named array", kXMPErr_BadXPath );
	}

	XMP_NodeOffspring oldChildren ( arrayNode->children );
	size_t oldChildCount = oldChildren.size();
	arrayNode->children.clear();
	
	// Extract the item values one at a time, until the whole input string is done. Be very careful
	// in the extraction about the string positions. They are essentially byte pointers, while the
	// contents are UTF-8. Adding or subtracting 1 does not necessarily move 1 Unicode character!
	
	size_t endPos = strlen ( catedStr );
	
	itemEnd = 0;
	while ( itemEnd < endPos ) {
		
		// Skip any leading spaces and separation characters. Always skip commas here. They can be
		// kept when within a value, but not when alone between values.
		
		for ( itemStart = itemEnd; itemStart < endPos; itemStart += charSize ) {
			ClassifyCharacter ( catedStr, itemStart, &charKind, &charSize, &uniChar );
			if ( (charKind == UCK_normal) || (charKind == UCK_quote) ) break;
		}
		if ( itemStart >= endPos ) break;
		
		if ( charKind != UCK_quote ) {
		
			// This is not a quoted value. Scan for the end, create an array item from the substring.

			for ( itemEnd = itemStart; itemEnd < endPos; itemEnd += charSize ) {

				ClassifyCharacter ( catedStr, itemEnd, &charKind, &charSize, &uniChar );

				if ( (charKind == UCK_normal) || (charKind == UCK_quote) ) continue;
				if ( (charKind == UCK_comma) && preserveCommas ) continue;
				if ( charKind != UCK_space ) break;

				if ( (itemEnd + charSize) >= endPos ) break;	// Anything left?
				ClassifyCharacter ( catedStr, (itemEnd+charSize), &nextKind, &nextSize, &nextChar );
				if ( (nextKind == UCK_normal) || (nextKind == UCK_quote) ) continue;
				if ( (nextKind == UCK_comma) && preserveCommas ) continue;
				break;	// Have multiple spaces, or a space followed by a separator.

			}		

			itemValue.assign ( catedStr, itemStart, (itemEnd - itemStart) );
		
		} else {
		
			// Accumulate quoted values into a local string, undoubling internal quotes that
			// match the surrounding quotes. Do not undouble "unmatching" quotes.
		
			UniCodePoint openQuote = uniChar;
			UniCodePoint closeQuote = GetClosingQuote ( openQuote );

			itemStart += charSize;	// Skip the opening quote;
			itemValue.erase();
			
			for ( itemEnd = itemStart; itemEnd < endPos; itemEnd += charSize ) {

				ClassifyCharacter ( catedStr, itemEnd, &charKind, &charSize, &uniChar );

				if ( (charKind != UCK_quote) || (! IsSurroundingQuote ( uniChar, openQuote, closeQuote)) ) {
				
					// This is not a matching quote, just append it to the item value.
					itemValue.append ( catedStr, itemEnd, charSize );
					
				} else {
				
					// This is a "matching" quote. Is it doubled, or the final closing quote? Tolerate
					// various edge cases like undoubled opening (non-closing) quotes, or end of input.
					
					if ( (itemEnd + charSize) < endPos ) {
						ClassifyCharacter ( catedStr, itemEnd+charSize, &nextKind, &nextSize, &nextChar );
					} else {
						nextKind = UCK_semicolon; nextSize = 0; nextChar = 0x3B;
					}
					
					if ( uniChar == nextChar ) {
						// This is doubled, copy it and skip the double.
						itemValue.append ( catedStr, itemEnd, charSize );
						itemEnd += nextSize;	// Loop will add in charSize.
					} else if ( ! IsClosingingQuote ( uniChar, openQuote, closeQuote ) ) {
						// This is an undoubled, non-closing quote, copy it.
						itemValue.append ( catedStr, itemEnd, charSize );
					} else {
						// This is an undoubled closing quote, skip it and exit the loop.
						itemEnd += charSize;
						break;
					}

				}

			}	// Loop to accumulate the quoted value.
		
		}

		// Add the separated item to the array. Keep a matching old value in case it had separators.
		
		size_t oldChild;
		for ( oldChild = 0; oldChild < oldChildCount; ++oldChild ) {
			if ( (oldChildren[oldChild] != 0) && (itemValue == oldChildren[oldChild]->value) ) break;
		}
		
		XMP_Node * newItem = 0;
		if ( oldChild == oldChildCount ) {
			newItem = new XMP_Node ( arrayNode, kXMP_ArrayItemName, itemValue.c_str(), 0 );
		} else {
			newItem = oldChildren[oldChild];
			oldChildren[oldChild] = 0;	// ! Don't match again, let duplicates be seen.
		}
		arrayNode->children.push_back ( newItem );
		
	}	// Loop through all of the returned items.

	// Delete any of the old children that were not kept.
	for ( size_t i = 0; i < oldChildCount; ++i ) {
		if ( oldChildren[i] != 0 ) delete oldChildren[i];
	}
	
}	// SeparateArrayItems


// -------------------------------------------------------------------------------------------------
// ApplyTemplate
// -------------

/* class static */ void
XMPUtils::ApplyTemplate ( XMPMeta *	      workingXMP,
						  const XMPMeta & templateXMP,
						  XMP_OptionBits  actions )
{

#if ENABLE_CPP_DOM_MODEL
	if (sUseNewCoreAPIs) {
		ApplyTemplate_v2(workingXMP, templateXMP, actions);
		return;
	}
#endif

	bool doClear   = XMP_OptionIsSet ( actions, kXMPTemplate_ClearUnnamedProperties );
	bool doAdd     = XMP_OptionIsSet ( actions, kXMPTemplate_AddNewProperties );
	bool doReplace = XMP_OptionIsSet ( actions, kXMPTemplate_ReplaceExistingProperties );
	
	bool deleteEmpty = XMP_OptionIsSet ( actions, kXMPTemplate_ReplaceWithDeleteEmpty );
	doReplace |= deleteEmpty;	// Delete-empty implies Replace.
	deleteEmpty &= (! doClear);	// Clear implies not delete-empty, but keep the implicit Replace. 

	bool doAll = XMP_OptionIsSet ( actions, kXMPTemplate_IncludeInternalProperties );
	
	// ! In several places we do loops backwards so that deletions do not perturb the remaining indices.
	// ! These loops use ordinals (size .. 1), we must use a zero based index inside the loop.
	
	if ( doClear ) {
	
		// Visit the top level working properties, delete if not in the template.

		for ( size_t schemaOrdinal = workingXMP->tree.children.size(); schemaOrdinal > 0; --schemaOrdinal ) {
	
			size_t schemaNum = schemaOrdinal-1;	// ! Convert ordinal to index!
			XMP_Node * workingSchema = workingXMP->tree.children[schemaNum];
			const XMP_Node * templateSchema = FindConstSchema ( &templateXMP.tree, workingSchema->name.c_str() );
			
			if ( templateSchema == 0 ) {
			
				// The schema is not in the template, delete all properties or just all external ones.

				if ( doAll ) {

					workingSchema->RemoveChildren();	// Remove the properties here, delete the schema below.

				} else {

					for ( size_t propOrdinal = workingSchema->children.size(); propOrdinal > 0; --propOrdinal ) {
						size_t propNum = propOrdinal-1;	// ! Convert ordinal to index!
						XMP_Node * workingProp = workingSchema->children[propNum];
						if ( IsExternalProperty ( workingSchema->name, workingProp->name ) ) {
							delete ( workingProp );
							workingSchema->children.erase ( workingSchema->children.begin() + propNum );
						}
					}

				}

			} else {
			
				// Check each of the working XMP's properties to see if it is in the template.

				for ( size_t propOrdinal = workingSchema->children.size(); propOrdinal > 0; --propOrdinal ) {
					size_t propNum = propOrdinal-1;	// ! Convert ordinal to index!
					XMP_Node * workingProp = workingSchema->children[propNum];
					if ( (doAll || IsExternalProperty ( workingSchema->name, workingProp->name )) && 
						 (FindConstChild ( templateSchema, workingProp->name.c_str() ) == 0) ) {
						delete ( workingProp );
						workingSchema->children.erase ( workingSchema->children.begin() + propNum );
					}
				}

			}
			
			if ( workingSchema->children.empty() ) {
				delete ( workingSchema );
				workingXMP->tree.children.erase ( workingXMP->tree.children.begin() + schemaNum );
			}
	
		}
		
	}
	
	if ( doAdd | doReplace ) {

		for ( size_t schemaNum = 0, schemaLim = templateXMP.tree.children.size(); schemaNum < schemaLim; ++schemaNum ) {
	
			const XMP_Node * templateSchema = templateXMP.tree.children[schemaNum];
	
			// Make sure we have an output schema node, then process the top level template properties.
			
			XMP_NodePtrPos workingSchemaPos;
			XMP_Node * workingSchema = FindSchemaNode ( &workingXMP->tree, templateSchema->name.c_str(),
														kXMP_ExistingOnly, &workingSchemaPos );
			if ( workingSchema == 0 ) {
				workingSchema = new XMP_Node ( &workingXMP->tree, templateSchema->name, templateSchema->value, kXMP_SchemaNode );
				workingXMP->tree.children.push_back ( workingSchema );
				workingSchemaPos = workingXMP->tree.children.end() - 1;
			}
			
			for ( size_t propNum = 0, propLim = templateSchema->children.size(); propNum < propLim; ++propNum ) {
				const XMP_Node * templateProp = templateSchema->children[propNum];
				if ( doAll || IsExternalProperty ( templateSchema->name, templateProp->name ) ) {
					AppendSubtree ( templateProp, workingSchema, doAdd, doReplace, deleteEmpty );
				}
			}
			
			if ( workingSchema->children.empty() ) {
				delete ( workingSchema );
				workingXMP->tree.children.erase ( workingSchemaPos );
			}
			
		}

	}

}	// ApplyTemplate


// -------------------------------------------------------------------------------------------------
// RemoveProperties
// ----------------

/* class static */ void
XMPUtils::RemoveProperties ( XMPMeta *		xmpObj,
							 XMP_StringPtr	schemaNS,
							 XMP_StringPtr	propName,
							 XMP_OptionBits options )
{

#if ENABLE_CPP_DOM_MODEL
	if (sUseNewCoreAPIs) {

		RemoveProperties_v2(xmpObj, schemaNS, propName, options);
		return;
	}
#endif

	XMP_Assert ( (schemaNS != 0) && (propName != 0) );	// ! Enforced by wrapper.
	
	const bool doAll = XMP_TestOption (options, kXMPUtil_DoAllProperties );
	const bool includeAliases = XMP_TestOption ( options, kXMPUtil_IncludeAliases );
	
	if ( *propName != 0 ) {
	
		// Remove just the one indicated property. This might be an alias, the named schema might
		// not actually exist. So don't lookup the schema node.
		
		if ( *schemaNS == 0 ) XMP_Throw ( "Property name requires schema namespace", kXMPErr_BadParam );
		
		XMP_ExpandedXPath expPath;
		ExpandXPath ( schemaNS, propName, &expPath );
		
		XMP_NodePtrPos propPos;
		XMP_Node * propNode = ::FindNode( &(xmpObj->tree), expPath, kXMP_ExistingOnly, kXMP_NoOptions, &propPos );
		if ( propNode != 0 ) {
			if ( doAll || IsExternalProperty ( expPath[kSchemaStep].step, expPath[kRootPropStep].step ) ) {
				XMP_Node * parent = propNode->parent;	// *** Should have XMP_Node::RemoveChild(pos).
				delete propNode;	// ! Both delete the node and erase the pointer from the parent.
				parent->children.erase ( propPos );
				DeleteEmptySchema ( parent );
			}
		}
	
	} else if ( *schemaNS != 0 ) {
	
		// Remove all properties from the named schema. Optionally include aliases, in which case
		// there might not be an actual schema node. 

		XMP_NodePtrPos schemaPos;
		XMP_Node * schemaNode = FindSchemaNode ( &xmpObj->tree, schemaNS, kXMP_ExistingOnly, &schemaPos );
		if ( schemaNode != 0 ) RemoveSchemaChildren ( schemaPos, doAll );
		
		if ( includeAliases ) {
		
			// We're removing the aliases also. Look them up by their namespace prefix. Yes, the
			// alias map is sorted so we could process just that portion. But that takes more code
			// and the extra speed isn't worth it. (Plus this way we avoid a dependence on the map
			// implementation.) Lookup the XMP node from the alias, to make sure the actual exists.

			XMP_StringPtr nsPrefix;
			XMP_StringLen nsLen;
			(void) XMPMeta::GetNamespacePrefix ( schemaNS, &nsPrefix, &nsLen );
			
			XMP_AliasMapPos currAlias = sRegisteredAliasMap->begin();
			XMP_AliasMapPos endAlias  = sRegisteredAliasMap->end();
			
			for ( ; currAlias != endAlias; ++currAlias ) {
				if ( strncmp ( currAlias->first.c_str(), nsPrefix, nsLen ) == 0 ) {
					XMP_NodePtrPos actualPos;
					XMP_Node * actualProp = ::FindNode( &xmpObj->tree, currAlias->second, kXMP_ExistingOnly, kXMP_NoOptions, &actualPos );
					if ( actualProp != 0 ) {
						XMP_Node * rootProp = actualProp;
						while ( ! XMP_NodeIsSchema ( rootProp->parent->options ) ) rootProp = rootProp->parent;
						if ( doAll || IsExternalProperty ( rootProp->parent->name, rootProp->name ) ) {
							XMP_Node * parent = actualProp->parent;
							delete actualProp;	// ! Both delete the node and erase the pointer from the parent.
							parent->children.erase ( actualPos );
							DeleteEmptySchema ( parent );
						}
					}
				}
			}

		}

	} else {
		
		// Remove all appropriate properties from all schema. In this case we don't have to be
		// concerned with aliases, they are handled implicitly from the actual properties.

		// ! Iterate backwards to reduce shuffling if schema are erased and to simplify the logic
		// ! for denoting the current schema. (Erasing schema n makes the old n+1 now be n.)

		size_t		   schemaCount = xmpObj->tree.children.size();
		XMP_NodePtrPos beginPos	   = xmpObj->tree.children.begin();
		
		for ( size_t schemaNum = schemaCount-1, schemaLim = (size_t)(-1); schemaNum != schemaLim; --schemaNum ) {
			XMP_NodePtrPos currSchema = beginPos + schemaNum;
			RemoveSchemaChildren ( currSchema, doAll );
		}
	
	}

}	// RemoveProperties


// -------------------------------------------------------------------------------------------------
// DuplicateSubtree
// ----------------

/* class static */ void
XMPUtils::DuplicateSubtree ( const XMPMeta & source,
							 XMPMeta *		 dest,
							 XMP_StringPtr	 sourceNS,
							 XMP_StringPtr	 sourceRoot,
							 XMP_StringPtr	 destNS,
							 XMP_StringPtr	 destRoot,
							 XMP_OptionBits	 options )
{

#if ENABLE_CPP_DOM_MODEL
	if(sUseNewCoreAPIs) {
		(void)dynamic_cast<const XMPMeta2 &>(source);
		return XMPUtils::DuplicateSubtree_v2(source, dest, sourceNS, sourceRoot, destNS, destRoot, options);
	}

#endif

	IgnoreParam(options);
	
	bool fullSourceTree = false;
	bool fullDestTree   = false;
	
	XMP_ExpandedXPath sourcePath, destPath; 

	const XMP_Node * sourceNode = 0;
	XMP_Node * destNode = 0;
	
	XMP_Assert ( (sourceNS != 0) && (*sourceNS != 0) );
	XMP_Assert ( (sourceRoot != 0) && (*sourceRoot != 0) );
	XMP_Assert ( (dest != 0) && (destNS != 0) && (destRoot != 0) );

	if ( *destNS == 0 )	  destNS   = sourceNS;
	if ( *destRoot == 0 ) destRoot = sourceRoot;
	
	if ( XMP_LitMatch ( sourceNS, "*" ) ) fullSourceTree = true;
	if ( XMP_LitMatch ( destNS, "*" ) )   fullDestTree   = true;
	
	if ( (&source == dest) && (fullSourceTree | fullDestTree) ) {
		XMP_Throw ( "Can't duplicate tree onto itself", kXMPErr_BadParam );
	}
	
	if ( fullSourceTree & fullDestTree ) XMP_Throw ( "Use Clone for full tree to full tree", kXMPErr_BadParam );

	if ( fullSourceTree ) {
	
		// The destination must be an existing empty struct, copy all of the source top level as fields.

		ExpandXPath ( destNS, destRoot, &destPath );
		destNode = ::FindNode( &dest->tree, destPath, kXMP_ExistingOnly );

		if ( (destNode == 0) || (! XMP_PropIsStruct ( destNode->options )) ) {
			XMP_Throw ( "Destination must be an existing struct", kXMPErr_BadXPath );
		}
		
		if ( ! destNode->children.empty() ) {
			if ( options & kXMP_DeleteExisting ) {
				destNode->RemoveChildren();
			} else {
				XMP_Throw ( "Destination must be an empty struct", kXMPErr_BadXPath );
			}
		}
		
		for ( size_t schemaNum = 0, schemaLim = source.tree.children.size(); schemaNum < schemaLim; ++schemaNum ) {

			const XMP_Node * currSchema = source.tree.children[schemaNum];

			for ( size_t propNum = 0, propLim = currSchema->children.size(); propNum < propLim; ++propNum ) {
				sourceNode = currSchema->children[propNum];
				XMP_Node * copyNode = new XMP_Node ( destNode, sourceNode->name, sourceNode->value, sourceNode->options );
				destNode->children.push_back ( copyNode );
				CloneOffspring ( sourceNode, copyNode );
			}

		}
	
	} else if ( fullDestTree ) {

		// The source node must be an existing struct, copy all of the fields to the dest top level.

		XMP_ExpandedXPath srcPath; 
		ExpandXPath ( sourceNS, sourceRoot, &srcPath );
		sourceNode = FindConstNode ( &source.tree, srcPath );

		if ( (sourceNode == 0) || (! XMP_PropIsStruct ( sourceNode->options )) ) {
			XMP_Throw ( "Source must be an existing struct", kXMPErr_BadXPath );
		}
		
		destNode = &dest->tree;
		
		if ( ! destNode->children.empty() ) {
			if ( options & kXMP_DeleteExisting ) {
				destNode->RemoveChildren();
			} else {
				XMP_Throw ( "Destination tree must be empty", kXMPErr_BadXPath );
			}
		}
		
		std::string   nsPrefix;
		XMP_StringPtr nsURI;
		XMP_StringLen nsLen;
		
		for ( size_t fieldNum = 0, fieldLim = sourceNode->children.size(); fieldNum < fieldLim; ++fieldNum ) {

			const XMP_Node * currField = sourceNode->children[fieldNum];

			size_t colonPos = currField->name.find ( ':' );
			if (  colonPos == std::string::npos ) continue;
			nsPrefix.assign ( currField->name.c_str(), colonPos );
			bool nsOK = XMPMeta::GetNamespaceURI ( nsPrefix.c_str(), &nsURI, &nsLen );
			if ( ! nsOK ) XMP_Throw ( "Source field namespace is not global", kXMPErr_BadSchema );
			
			XMP_Node * destSchema = FindSchemaNode ( &dest->tree, nsURI, kXMP_CreateNodes );
			if ( destSchema == 0 ) XMP_Throw ( "Failed to find destination schema", kXMPErr_BadSchema );

			XMP_Node * copyNode = new XMP_Node ( destSchema, currField->name, currField->value, currField->options );
			destSchema->children.push_back ( copyNode );
			CloneOffspring ( currField, copyNode );

		}
		
	} else {

		// Find the root nodes for the source and destination subtrees.
		
		ExpandXPath ( sourceNS, sourceRoot, &sourcePath );
		ExpandXPath ( destNS, destRoot, &destPath );
	
		sourceNode = FindConstNode ( &source.tree, sourcePath );
		if ( sourceNode == 0 ) XMP_Throw ( "Can't find source subtree", kXMPErr_BadXPath );
		
		destNode = ::FindNode ( &dest->tree, destPath, kXMP_ExistingOnly );	// Dest must not yet exist.
		if ( destNode != 0 ) XMP_Throw ( "Destination subtree must not exist", kXMPErr_BadXPath );
		
		destNode = ::FindNode ( &dest->tree, destPath, kXMP_CreateNodes );	// Now create the dest.
		if ( destNode == 0 ) XMP_Throw ( "Can't create destination root node", kXMPErr_BadXPath );
		
		// Make sure the destination is not within the source! The source can't be inside the destination
		// because the source already existed and the destination was just created.
		
		if ( &source == dest ) {
			for ( XMP_Node * testNode = destNode; testNode != 0; testNode = testNode->parent ) {
				if ( testNode == sourceNode ) {
					// *** delete the just-created dest root node
					XMP_Throw ( "Destination subtree is within the source subtree", kXMPErr_BadXPath );
				}
			}
		}
	
		// *** Could use a CloneTree util here and maybe elsewhere.
		
		destNode->value	  = sourceNode->value;	// *** Should use SetNode.
		destNode->options = sourceNode->options;
		CloneOffspring ( sourceNode, destNode );

	}

}	// DuplicateSubtree


// =================================================================================================
