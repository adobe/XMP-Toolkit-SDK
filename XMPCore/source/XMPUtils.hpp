#ifndef __XMPUtils_hpp__
#define __XMPUtils_hpp__

// =================================================================================================
// Copyright 2003 Adobe
// All Rights Reserved.
//
// NOTICE:	Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "public/include/XMP_Environment.h"
#include "public/include/XMP_Const.h"

#include "XMPCore/source/XMPMeta.hpp"
#include "XMPCore/source/XMPCore_Impl.hpp"
#include "public/include/client-glue/WXMPUtils.hpp"
#include "XMPCore/XMPCoreDefines.h"

#if ENABLE_CPP_DOM_MODEL
#include "XMPCommon/Interfaces/IError.h"
#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPCore/source/XMPMeta2.hpp"
#endif

#include "XMPCore/source/XMPCore_Impl.hpp"
#include "source/XMLParserAdapter.hpp"
#include "XMPCore/source/XMPMeta.hpp"
#include "third-party/zuid/interfaces/MD5.h"



bool
IsInternalProperty(const XMP_VarString & schema,
				   const XMP_VarString & prop);
// -------------------------------------------------------------------------------------------------

class	XMPUtils {
public:

	static bool
		Initialize();	// ! For internal use only!

	static void
		Terminate() RELEASE_NO_THROW;	// ! For internal use only!

	// ---------------------------------------------------------------------------------------------	

#if ENABLE_CPP_DOM_MODEL
	static void SetNode(const AdobeXMPCore::spINode & node, XMP_StringPtr value, XMP_OptionBits options);
	static XMP_OptionBits ConvertNewArrayFormToOldArrayForm(const AdobeXMPCore::spcIArrayNode & arrayNode);
	static AdobeXMPCore::spINode CreateArrayChildNode(const AdobeXMPCore::spIArrayNode & arrayNode, XMP_OptionBits options);
	static void DoSetArrayItem(const AdobeXMPCore::spIArrayNode & arrayNode, XMP_Index itemIndex, XMP_StringPtr itemValue, XMP_OptionBits options);
	static void SetImplicitNodeInformation(bool & firstImplicitNodeFound, AdobeXMPCore::spINode & implicitNodeRoot, AdobeXMPCore::spINode & destNode,
		XMP_Index & implicitNodeIndex, XMP_Index index = 0);
	static void GetNameSpaceAndNameFromStepValue(const XMP_VarString & stepStr, const AdobeXMPCore::spcINameSpacePrefixMap & defaultMap,
		XMP_VarString & stepNameSpace, XMP_VarString & stepName);
	static bool FindNode(const AdobeXMPCore::spIMetadata & mDOM, XMP_ExpandedXPath & expPath, bool createNodes, XMP_OptionBits leafOptions,
		AdobeXMPCore::spINode & retNode, XMP_Index * nodeIndex = 0, bool ignoreLastStep = 0);
	static bool FindCnstNode(const AdobeXMPCore::spIMetadata & mDOM, XMP_ExpandedXPath & expPath, AdobeXMPCore::spINode & destNode, XMP_OptionBits * options = 0,
		XMP_Index * arrayIndex = 0);
	static AdobeXMPCore::spINode FindChildNode(const AdobeXMPCore::spINode & parent, XMP_StringPtr childName, XMP_StringPtr childNameSpace, bool createNodes, size_t *	pos /* = 0 */);
	static size_t GetNodeChildCount(const AdobeXMPCore::spcINode & node);
	static AdobeXMPCore::spcINodeIterator GetNodeChildIterator(const AdobeXMPCore::spcINode & node);
	static std::vector< AdobeXMPCore::spcINode > GetChildVector(const AdobeXMPCore::spINode & node);
	static XMP_OptionBits GetIXMPOptions(const AdobeXMPCore::spcINode & node);
	static bool HandleConstAliasStep(const AdobeXMPCore::spIMetadata & mDOM, AdobeXMPCore::spINode & destNode, const XMP_ExpandedXPath & expandedXPath,
		XMP_Index  * nodeIndex = 0);
	static bool HandleAliasStep(const AdobeXMPCore::spIMetadata &  mDOM, XMP_ExpandedXPath & expandedXPath, bool createNodes, XMP_OptionBits leafOptions,
		AdobeXMPCore::spINode & destNode, XMP_Index * nodeIndex, bool ignoreLastStep);
	static AdobeXMPCommon::spcIUTF8String GetNodeValue(const AdobeXMPCore::spINode & node);
	static XMP_Index LookupFieldSelector_v2(const AdobeXMPCore::spIArrayNode & arrayNode, XMP_VarString fieldName, XMP_VarString fieldValue);
	static AdobeXMPCore::spINode CreateTerminalNode(const char* nameSpace, const char * name, XMP_OptionBits options);

#endif


	static void
		ComposeArrayItemPath(XMP_StringPtr   schemaNS,
		XMP_StringPtr   arrayName,
		XMP_Index	   itemIndex,
		XMP_VarString * fullPath);

	static void
		ComposeStructFieldPath(XMP_StringPtr	 schemaNS,
		XMP_StringPtr	 structName,
		XMP_StringPtr	 fieldNS,
		XMP_StringPtr	 fieldName,
		XMP_VarString * fullPath);

	static void
		ComposeQualifierPath(XMP_StringPtr   schemaNS,
		XMP_StringPtr   propName,
		XMP_StringPtr   qualNS,
		XMP_StringPtr   qualName,
		XMP_VarString * fullPath);

	static void
		ComposeLangSelector(XMP_StringPtr	  schemaNS,
		XMP_StringPtr	  arrayName,
		XMP_StringPtr	  langName,
		XMP_VarString * fullPath);

	static void
		ComposeFieldSelector(XMP_StringPtr   schemaNS,
		XMP_StringPtr   arrayName,
		XMP_StringPtr   fieldNS,
		XMP_StringPtr   fieldName,
		XMP_StringPtr   fieldValue,
		XMP_VarString * fullPath);

	// ---------------------------------------------------------------------------------------------

	static void
		ConvertFromBool(bool			  binValue,
		XMP_VarString * strValue);

	static void
		ConvertFromInt(XMP_Int32		 binValue,
		XMP_StringPtr	 format,
		XMP_VarString * strValue);

	static void
		ConvertFromInt64(XMP_Int64	   binValue,
		XMP_StringPtr   format,
		XMP_VarString * strValue);

	static void
		ConvertFromFloat(double		   binValue,
		XMP_StringPtr   format,
		XMP_VarString * strValue);

	static void
		ConvertFromDate(const XMP_DateTime & binValue,
		XMP_VarString *	   strValue);

	// ---------------------------------------------------------------------------------------------

	static bool
		ConvertToBool(XMP_StringPtr strValue);

	static XMP_Int32
		ConvertToInt(XMP_StringPtr strValue);

	static XMP_Int64
		ConvertToInt64(XMP_StringPtr strValue);

	static double
		ConvertToFloat(XMP_StringPtr strValue);

	static void
		ConvertToDate(XMP_StringPtr	strValue,
		XMP_DateTime * binValue);

	// ---------------------------------------------------------------------------------------------

	static void
		CurrentDateTime(XMP_DateTime * time);

	static void
		SetTimeZone(XMP_DateTime * time);

	static void
		ConvertToUTCTime(XMP_DateTime * time);

	static void
		ConvertToLocalTime(XMP_DateTime * time);

	static int
		CompareDateTime(const XMP_DateTime & left,
		const XMP_DateTime & right);
	// ---------------------------------------------------------------------------------------------

	static void
		EncodeToBase64(XMP_StringPtr	 rawStr,
		XMP_StringLen	 rawLen,
		XMP_VarString * encodedStr);

	static void
		DecodeFromBase64(XMP_StringPtr   encodedStr,
		XMP_StringLen   encodedLen,
		XMP_VarString * rawStr);

	// ---------------------------------------------------------------------------------------------

	static void
		PackageForJPEG(const XMPMeta & xmpObj,
		XMP_VarString * stdStr,
		XMP_VarString * extStr,
		XMP_VarString * digestStr);


#if ENABLE_CPP_DOM_MODEL
	static void
		PackageForJPEG(const XMPMeta2 & xmpObj,
		XMP_VarString * stdStr,
		XMP_VarString * extStr,
		XMP_VarString * digestStr);
#endif
	static void
		MergeFromJPEG(XMPMeta *       fullXMP,
		const XMPMeta & extendedXMP);




	// ---------------------------------------------------------------------------------------------

	static void
		CatenateArrayItems(const XMPMeta & xmpObj,
		XMP_StringPtr	 schemaNS,
		XMP_StringPtr	 arrayName,
		XMP_StringPtr	 separator,
		XMP_StringPtr	 quotes,
		XMP_OptionBits	 options,
		XMP_VarString * catedStr);

	static void
		SeparateArrayItems(XMPMeta *		xmpObj,
		XMP_StringPtr	schemaNS,
		XMP_StringPtr	arrayName,
		XMP_OptionBits options,
		XMP_StringPtr	catedStr);

	static void
		ApplyTemplate(XMPMeta *	    workingXMP,
		const XMPMeta & templateXMP,
		XMP_OptionBits  actions);


	static void
		RemoveProperties(XMPMeta *	  xmpObj,
		XMP_StringPtr  schemaNS,
		XMP_StringPtr  propName,
		XMP_OptionBits options);


	static void
		DuplicateSubtree(const XMPMeta & source,
		XMPMeta *	   dest,
		XMP_StringPtr   sourceNS,
		XMP_StringPtr   sourceRoot,
		XMP_StringPtr   destNS,
		XMP_StringPtr   destRoot,
		XMP_OptionBits  options);


	// ---------------------------------------------------------------------------------------------

	static std::string& Trim(std::string& string);

	static std::string * WhiteSpaceStrPtr;

#if ENABLE_CPP_DOM_MODEL
	static void MapXMPErrorToIError(XMP_Int32 xmpErrorCodes, AdobeXMPCommon::IError::eErrorDomain & domain, AdobeXMPCommon::IError::eErrorCode & code);
	static bool SerializeExtensionAsJSON(const AdobeXMPCore::spINode & extensionNode, std::string & key, std::string & value);
	static bool IsExtensionValidForBackwardCompatibility(const AdobeXMPCore::spINode & extensionNode);
	static bool CreateExtensionNode(const AdobeXMPCore::spIStructureNode & xmpNode, const XMP_VarString & serializedJSON, const XMP_VarString & doubleQuotesStr);


	static void
		CatenateArrayItems_v2(const XMPMeta & xmpObj,
		XMP_StringPtr	 schemaNS,
		XMP_StringPtr	 arrayName,
		XMP_StringPtr	 separator,
		XMP_StringPtr	 quotes,
		XMP_OptionBits	 options,
		XMP_VarString * catedStr);
	static void
		SeparateArrayItems_v2(XMPMeta *		xmpObj,
		XMP_StringPtr	schemaNS,
		XMP_StringPtr	arrayName,
		XMP_OptionBits options,
		XMP_StringPtr	catedStr);

	static void
		RemoveProperties_v2(XMPMeta *		xmpMetaPtr,
		XMP_StringPtr	schemaNS,
		XMP_StringPtr	propName,
		XMP_OptionBits options);
	
	static void
		ApplyTemplate_v2(XMPMeta *	      workingXMP,
		const XMPMeta & templateXMP,
		XMP_OptionBits  actions);

	static void
		DuplicateSubtree_v2(const XMPMeta & source,
		XMPMeta *		 dest,
		XMP_StringPtr	 sourceNS,
		XMP_StringPtr	 sourceRoot,
		XMP_StringPtr	 destNS,
		XMP_StringPtr	 destRoot,
		XMP_OptionBits	 options);

#endif

	static bool CreateExtensionNode(XMP_Node ** xmpNode, const XMP_VarString & serializedJSON, const XMP_VarString & doubleQuotesString);

	static bool GetSerializedJSONForExtensionNode(const XMP_Node * xmpNode, XMP_VarString &extensionAsKey, XMP_VarString & serializedJSON);

	static bool IsSuitableForJSONSerialization(const XMP_Node * xmpNode);

};	// XMPUtils

// =================================================================================================

#endif	// __XMPUtils_hpp__
