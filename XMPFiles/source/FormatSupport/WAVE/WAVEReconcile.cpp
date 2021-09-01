// =================================================================================================
// Copyright Adobe
// Copyright 2010 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! XMP_Environment.h must be the first included header.
#include "public/include/XMP_Const.h"

#include <string.h>

#include "XMPFiles/source/FormatSupport/WAVE/WAVEReconcile.h"
#include "XMPFiles/source/FormatSupport/WAVE/DISPMetadata.h"
#include "XMPFiles/source/FormatSupport/WAVE/INFOMetadata.h"
#include "XMPFiles/source/FormatSupport/WAVE/BEXTMetadata.h"
#include "XMPFiles/source/FormatSupport/WAVE/CartMetadata.h"
#include "XMPFiles/source/FormatSupport/WAVE/iXMLMetadata.h"

#include "XMPFiles/source/FormatSupport/TimeConversionUtils.hpp"

// cr8r is not yet required for WAVE
//#include "Cr8rMetadata.h"

#include "XMPFiles/source/NativeMetadataSupport/MetadataSet.h"

#include "XMPFiles/source/FormatSupport/Reconcile_Impl.hpp"


using namespace IFF_RIFF;

// ************** legacy Mappings ***************** //

static const char * kBWF_description = "description";
static const char * kBWF_originator = "originator";
static const char * kBWF_originatorReference = "originatorReference";
static const char * kBWF_originationDate = "originationDate";
static const char * kBWF_originationTime = "originationTime";
static const char * kBWF_timeReference = "timeReference";
static const char * kBWF_version = "version";
static const char * kBWF_umid = "umid";
static const char * kBWF_codingHistory = "codingHistory";
static const char * kBWF_timeStampSampleRate = "timeSampleRate";		// its transient should be deleted at the end.

static const MetadataPropertyInfo kBextProperties[] =
{
//	  XMP NS		XMP Property Name			Native Metadata Identifier			Native Datatype				XMP	Datatype		Delete	Priority	ExportPolicy
	{ kXMP_NS_BWF,	kBWF_description,			BEXTMetadata::kDescription,			kNativeType_StrLocal,		kXMPType_Simple,	false,	false,		kExport_Always },	// bext:description <-> BEXT:Description
	{ kXMP_NS_BWF,	kBWF_originator,			BEXTMetadata::kOriginator,			kNativeType_StrLocal,		kXMPType_Simple,	false,	false,		kExport_Always },	// bext:originator <-> BEXT:originator
	{ kXMP_NS_BWF,	kBWF_originatorReference,	BEXTMetadata::kOriginatorReference,	kNativeType_StrLocal,		kXMPType_Simple,	false,	false,		kExport_Always },	// bext:OriginatorReference <-> BEXT:OriginatorReference
	{ kXMP_NS_BWF,	kBWF_originationDate,		BEXTMetadata::kOriginationDate,		kNativeType_StrLocal,		kXMPType_Simple,	false,	false,		kExport_Always },	// bext:originationDate <-> BEXT:originationDate
	{ kXMP_NS_BWF,	kBWF_originationTime,		BEXTMetadata::kOriginationTime,		kNativeType_StrLocal,		kXMPType_Simple,	false,	false,		kExport_Always },	// bext:originationTime <-> BEXT:originationTime
	{ kXMP_NS_BWF,	kBWF_timeReference,			BEXTMetadata::kTimeReference,		kNativeType_Uns64,			kXMPType_Simple,	false,	false,		kExport_Always },	// bext:timeReference <-> BEXT:TimeReferenceLow + BEXT:TimeReferenceHigh
	// Special case: On export BEXT:version is always written as 1
	{ kXMP_NS_BWF,	kBWF_version,				BEXTMetadata::kVersion,				kNativeType_Uns16,			kXMPType_Simple,	false,	false,		kExport_Never },	// bext:version <-> BEXT:version
	// special case: bext:umid <-> BEXT:UMID
	{ kXMP_NS_BWF,	kBWF_codingHistory,			BEXTMetadata::kCodingHistory,		kNativeType_StrLocal,		kXMPType_Simple,	false,	false,		kExport_Always },	// bext:codingHistory <-> BEXT:codingHistory
	{ NULL }
};

static const char * kDM_shotNumber = "shotNumber";
static const char * kDM_audioSampleType = "audioSampleType";
static const char * kDM_scene = "scene";
static const char * kDM_tapeName = "tapeName";
static const char * kDM_logComment = "logComment";
static const char * kDM_projectName = "projectName";
static const char * kDM_audioSampleRate = "audioSampleRate";
static const char * kDM_startTimecode = "startTimecode";
static const char * kDM_timeFormat = "timeFormat";
static const char * kDM_timeValue = "timeValue";
static const char * kDM_good = "good";
static const char * kIXML_trackList = "trackList";
static const char * kIXML_channelIndex = "channelIndex";
static const char * kIXML_interleaveIndex = "interleaveIndex";
static const char * kIXML_Name = "name";
static const char * kIXML_Function = "function";

static const MetadataPropertyInfo kiXMLProperties[] =
{
//	  XMP NS		XMP Property Name			Native Metadata Identifier				Native Datatype				XMP	Datatype		Delete	Priority	ExportPolicy
	{ kXMP_NS_DM,	kDM_tapeName,				iXMLMetadata::kTape,					kNativeType_StrUTF8,		kXMPType_Simple,	false,	false,		kExport_Always },	//xmpDM:tapeName <-> iXML:TAPE
	{ kXMP_NS_DM,	kDM_shotNumber,				iXMLMetadata::kTake,					kNativeType_StrUTF8,		kXMPType_Simple,	false,	false,		kExport_Always },	//xmpDM:shotNumber <-> iXML:TAKE
	{ kXMP_NS_DM,	kDM_scene,					iXMLMetadata::kScene,					kNativeType_StrUTF8,		kXMPType_Simple,	false,	false,		kExport_Always },	//xmpDM:scene <-> iXML:SCENE
	{ kXMP_NS_DM,	kDM_logComment,				iXMLMetadata::kNote,					kNativeType_StrUTF8,		kXMPType_Simple,	false,	false,		kExport_Always },	//xmpDM:logComment <-> iXML:NOTE
	{ kXMP_NS_DM,	kDM_projectName,			iXMLMetadata::kProject,					kNativeType_StrUTF8,		kXMPType_Simple,	false,	false,		kExport_Always },	//xmpDM:project <-> iXML:PROJECT
	{ kXMP_NS_DM,	kDM_good,					iXMLMetadata::kCircled,					kNativeType_Bool,			kXMPType_Simple,	false,	false,		kExport_Always },	//xmpDM:good <-> iXML:CIRCLED
	{ kXMP_NS_DM,	kDM_audioSampleRate,		iXMLMetadata::kFileSampleRate,			kNativeType_Uns64,			kXMPType_Simple,	false,	false,		kExport_Always },	// xmpDM:audioSampleRate <-> iXML:FILE_SAMPLE_RATE
	// special case for AudioBitDepth // xmpDM:audioSampleType <-> iXML:AUDIO_BIT_DEPTH
	{ kXMP_NS_BWF,	kBWF_description,			iXMLMetadata::kBWFDescription,			kNativeType_StrUTF8,		kXMPType_Simple,	true,	false,		kExport_Always },	// bext:description <-> iXML:BWF_DESCRIPTION
	{ kXMP_NS_BWF,	kBWF_originator,			iXMLMetadata::kBWFOriginator,			kNativeType_StrUTF8,		kXMPType_Simple,	true,	false,		kExport_Always },	// bext:originator <-> iXML:BWF_ORIGINATOR
	{ kXMP_NS_BWF,	kBWF_originatorReference,	iXMLMetadata::kBWFOriginatorReference,	kNativeType_StrUTF8,		kXMPType_Simple,	true,	false,		kExport_Always },	// bext:OriginatorReference <-> iXML:BWF_ORIGINATOR_REFERENCE
	{ kXMP_NS_BWF,	kBWF_originationDate,		iXMLMetadata::kBWFOriginationDate,		kNativeType_StrUTF8,		kXMPType_Simple,	true,	false,		kExport_Always },	// bext:originationDate <-> iXML:BWF_ORIGINATION_DATE
	{ kXMP_NS_BWF,	kBWF_originationTime,		iXMLMetadata::kBWFOriginationTime,		kNativeType_StrUTF8,		kXMPType_Simple,	true,	false,		kExport_Always },	// bext:originationTime <-> iXML:BWF_ORIGINATION_TIME
	// special case for timeReference // bext:timeReference <-> iXML:BWF_TIME_REFERENCE_LOW and iXML:BWF_TIME_REFERENCE_HIGH
	// Special case: On export BEXT:version is always written as 1
	{ kXMP_NS_BWF,	kBWF_version,				iXMLMetadata::kBWFVersion,				kNativeType_Uns64,			kXMPType_Simple,	true,	false,		kExport_Never },	// bext:version <-> iXML:BWF_VERSION
	{ kXMP_NS_BWF,	kBWF_codingHistory,			iXMLMetadata::kBWFHistory,				kNativeType_StrUTF8,		kXMPType_Simple,	true,	false,		kExport_Always },	// bext:codingHistory <-> iXML:BWF_CODING_HISTORY
	{ kXMP_NS_BWF,	kBWF_umid,					iXMLMetadata::kBWFUMID,					kNativeType_StrASCII,		kXMPType_Simple,	true,	false,		kExport_Always },	// bext:codingHistory <-> iXML:BWF_CODING_HISTORY
	// special case for timeReference // bext:timeReference <-> iXML:TIMESTAMP_SAMPLES_SINCE_MIDNIGHT_HO and iXML:TIMESTAMP_SAMPLES_SINCE_MIDNIGHT_HI
	// special case for startTimeCode // xmpDM:startTimecode <-> iXML:TIMECODE_RATE, iXML:TIMECODE_FLAG and bext:timeReference.
	{ kXMP_NS_BWF,	kBWF_timeStampSampleRate,	iXMLMetadata::kTimeStampSampleRate,		kNativeType_Uns64,			kXMPType_Simple,	false,	false,		kExport_NoDelete },	// bext::timeStampSampleRate <-> iXML
	// special case for TRACK_LIST // ixml:Track_List <-> ixml:trackList
	{ NULL }
};

static const MetadataPropertyInfo kINFOProperties[] =
{
//	  XMP NS		XMP Property Name		Native Metadata Identifier		Native Datatype				XMP	Datatype		Delete	Priority	ExportPolicy
	{ kXMP_NS_DM,	"artist",				INFOMetadata::kArtist,			kNativeType_StrUTF8,		kXMPType_Simple,	false,	true,		kExport_Always },		// xmpDM:artist <-> IART
	{ kXMP_NS_DM,	"logComment",			INFOMetadata::kComments,		kNativeType_StrUTF8,		kXMPType_Simple,	false,	true,		kExport_Always },		// xmpDM:logComment <-> ICMT
	{ kXMP_NS_DC,	"rights",				INFOMetadata::kCopyright,		kNativeType_StrUTF8,		kXMPType_Localized,	false,	true,		kExport_Always },		// dc:rights <-> ICOP
	{ kXMP_NS_XMP,	"CreateDate",			INFOMetadata::kCreationDate,	kNativeType_StrUTF8,		kXMPType_Simple,	false,	true,		kExport_Always },		// xmp:CreateDate <-> ICRD
	{ kXMP_NS_DM,	"engineer",				INFOMetadata::kEngineer,		kNativeType_StrUTF8,		kXMPType_Simple,	false,	true,		kExport_Always },		// xmpDM:engineer <-> IENG
	{ kXMP_NS_DM,	"genre",				INFOMetadata::kGenre,			kNativeType_StrUTF8,		kXMPType_Simple,	false,	true,		kExport_Always },		// xmpDM:genre <-> IGNR
	{ kXMP_NS_XMP,	"CreatorTool",			INFOMetadata::kSoftware,		kNativeType_StrUTF8,		kXMPType_Simple,	false,	true,		kExport_Always },		// xmp:CreatorTool <-> ISFT
	{ kXMP_NS_DC,	"source",				INFOMetadata::kMedium,			kNativeType_StrUTF8,		kXMPType_Simple,	false,	false,		kExport_Always },		// dc:source <-> IMED, not in old digest
	{ kXMP_NS_DC,	"type",					INFOMetadata::kSourceForm,		kNativeType_StrUTF8,		kXMPType_Array,		false,	false,		kExport_Always },		// dc:type <-> ISRF, not in old digest

	// new mappings
	{ kXMP_NS_RIFFINFO,	"name",				INFOMetadata::kName,			kNativeType_StrUTF8,		kXMPType_Localized,	true,	false,		kExport_Always },		// riffinfo:name <-> INAM
	{ kXMP_NS_RIFFINFO,	"archivalLocation",	INFOMetadata::kArchivalLocation,kNativeType_StrUTF8,		kXMPType_Simple,	true,	false,		kExport_Always },		// riffinfo:archivalLocation  <-> IARL
	{ kXMP_NS_RIFFINFO,	"commissioned",		INFOMetadata::kCommissioned,	kNativeType_StrUTF8,		kXMPType_Simple,	true,	false,		kExport_Always },		// riffinfo:commissioned  <-> ICMS
	// special case, the native value is a semicolon-separated array
	// { kXMP_NS_DC,		"subject",			INFOMetadata::kKeywords,		kNativeType_StrUTF8,		kXMPType_Array,		false,	false,		kExport_Always },		// dc:subject  <-> IKEY
	{ kXMP_NS_RIFFINFO,	"product",			INFOMetadata::kProduct,			kNativeType_StrUTF8,		kXMPType_Simple,	true,	false,		kExport_Always },		// riffinfo:product <-> IPRD
	{ kXMP_NS_DC,		"description",		INFOMetadata::kSubject,			kNativeType_StrUTF8,		kXMPType_Localized,	false,	false,		kExport_Always },		// dc:description <-> ISBJ
	{ kXMP_NS_RIFFINFO,	"source",			INFOMetadata::kSource,			kNativeType_StrUTF8,		kXMPType_Simple,	true,	false,		kExport_Always },		// riffinfo:source  <-> ISRC
	{ kXMP_NS_RIFFINFO,	"technician",		INFOMetadata::kTechnican,		kNativeType_StrUTF8,		kXMPType_Simple,	true,	false,		kExport_Always },		// riffinfo:technician <-> ITCH

	{ NULL }
};

static const MetadataPropertyInfo kDISPProperties[] =
{
//	  XMP NS		XMP Property Name		Native Metadata Identifier		Datatype					Datatype			Delete	Priority	ExportPolicy
	{ kXMP_NS_DC,	"title",				DISPMetadata::kTitle,			kNativeType_StrUTF8,		kXMPType_Localized,	false,	true,		kExport_Always },		// dc:title <-> DISP
	// Special case: DISP will overwrite LIST/INFO:INAM in dc:title if existing
	{ NULL }
};

static const MetadataPropertyInfo kCartProperties[] =
{
//	  XMP NS			XMP Property Name		Native Metadata Identifier			Datatype					Datatype			Delete	Priority	ExportPolicy
	{ kXMP_NS_AEScart,	"Version",				CartMetadata::kVersion,				kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"Title",				CartMetadata::kTitle,				kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"Artist",				CartMetadata::kArtist,				kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"CutID",				CartMetadata::kCutID,				kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"ClientID",				CartMetadata::kClientID,			kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"Category",				CartMetadata::kCategory,			kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"Classification",		CartMetadata::kClassification,		kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"OutCue",				CartMetadata::kOutCue,				kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"StartDate",			CartMetadata::kStartDate,			kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"StartTime",			CartMetadata::kStartTime,			kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"EndDate",				CartMetadata::kEndDate,				kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"EndTime",				CartMetadata::kEndTime,				kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"ProducerAppID",		CartMetadata::kProducerAppID,		kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"ProducerAppVersion",	CartMetadata::kProducerAppVersion,	kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"UserDef",				CartMetadata::kUserDef,				kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"URL",					CartMetadata::kURL,					kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"TagText",				CartMetadata::kTagText,				kNativeType_StrLocal,		kXMPType_Simple,	true,	false,		kExport_Always },
	{ kXMP_NS_AEScart,	"LevelReference",		CartMetadata::kLevelReference,		kNativeType_Int32,			kXMPType_Simple,	true,	false,		kExport_Always },
	// Special case Post Timer
	{ NULL }
};

// cr8r is not yet required for WAVE
//
//static const MetadataPropertyInfo kCr8rProperties[] =
//{
////	  XMP NS				XMP Property Name							Native Metadata Identifier	Native Datatype		XMP Datatype		Delete	Priority	ExportPolicy
//	{ kXMP_NS_CreatorAtom,	"macAtom/creatorAtom:applicationCode",		Cr8rMetadata::kCreatorCode,	kNativeType_Uns32,	kXMPType_Simple,	false,	false,		kExport_Always },	// creatorAtom:macAtom/creatorAtom:applicationCode <-> Cr8r.creatorCode 
//	{ kXMP_NS_CreatorAtom,	"macAtom/creatorAtom:invocationAppleEvent",	Cr8rMetadata::kAppleEvent,	kNativeType_Uns32,	kXMPType_Simple,	false,	false,		kExport_Always },	// creatorAtom:macAtom/creatorAtom:invocationAppleEvent <-> Cr8r.appleEvent
//	{ kXMP_NS_CreatorAtom,	"windowsAtom/creatorAtom:extension",		Cr8rMetadata::kFileExt,		kNativeType_Str,	kXMPType_Simple,	false,	false,		kExport_Always },	// creatorAtom:windowsAtom/creatorAtom:extension <-> Cr8r.fileExt
//	{ kXMP_NS_CreatorAtom,	"windowsAtom/creatorAtom:invocationFlags",	Cr8rMetadata::kAppOptions,	kNativeType_Str,	kXMPType_Simple,	false,	false,		kExport_Always },	// creatorAtom:windowsAtom/creatorAtom:invocationFlags <-> Cr8r.appOptions
//	{ kXMP_NS_XMP,			"CreatorTool",								Cr8rMetadata::kAppName,		kNativeType_Str,	kXMPType_Simple,	false,	false,		kExport_Always },	// xmp:CreatorTool <-> Cr8r.appName
//	{ NULL }
//};

// ! PrmL atom has all special mappings

// ************** legacy Mappings end ***************** //

XMP_Bool WAVEReconcile::importToXMP( SXMPMeta& outXMP, const MetadataSet& inMetaData )
{
	bool changed = false;

	// the reconciliation is based on the existing outXMP packet

	//
	// ! The existence of a digest leads to prefering pre-existing XMP over legacy properties.
	//
	bool hasDigest = outXMP.GetProperty( kXMP_NS_WAV, "NativeDigest", NULL , NULL );

	if ( hasDigest )
	{
		// remove, as digests are no longer used.
		outXMP.DeleteProperty( kXMP_NS_WAV, "NativeDigest" );
	}


	if ( ! ignoreLocalText ) 
	{
		//
		// Import iXML
		//
		iXMLMetadata * iXMLMeta = inMetaData.get<iXMLMetadata>();
		if ( iXMLMeta != NULL ) {
			changed |= IReconcile::importNativeToXMP( outXMP, *iXMLMeta, kiXMLProperties, false );
			changed |= exportSpecialiXMLToXMP( *iXMLMeta, outXMP );
		}

		//
		// Import BEXT
		//
		BEXTMetadata *bextMeta = inMetaData.get<BEXTMetadata>();

		if (bextMeta != NULL)
		{
			changed |= IReconcile::importNativeToXMP( outXMP, *bextMeta, kBextProperties, false );

			// bext:umid <-> BEXT:UMID
			if ( bextMeta->valueExists( BEXTMetadata::kUMID ) )
			{
				XMP_Uns32 umidSize = 0;
				const XMP_Uns8* const umid = bextMeta->getArray<XMP_Uns8>( BEXTMetadata::kUMID, umidSize );

				std::string xmpValue;
				bool allZero = encodeToHexString( umid, xmpValue );
			
				if( ! allZero )
				{	
					outXMP.SetProperty(kXMP_NS_BWF, "umid", xmpValue.c_str());	
					changed = true;
				}
			}

		}


		//
		// Import cart
		//
		CartMetadata* cartData = inMetaData.get<CartMetadata>();
		if ( cartData != NULL ) 
		{

			if (cartData->valueExists( CartMetadata::kPostTimer ) )
			{
				// first get Array
				XMP_Uns32 size = 0;
				const CartMetadata::StoredCartTimer* timerArray = cartData->getArray<CartMetadata::StoredCartTimer> ( CartMetadata::kPostTimer, size );
				XMP_Assert (size == CartMetadata::kPostTimerLength );

				char usage [5];
				XMP_Uns32 usageBE = 0;
				char value [25];  // Unsigned has 10 dezimal digets (buffer has 25 just to be save)
				std::string  path = "";
				memset ( usage, 0, 5 );	// Fill with zeros
				memset ( value, 0, 25 );	// Fill with zeros

				outXMP.DeleteProperty( kXMP_NS_AEScart, "PostTimer");
				outXMP.AppendArrayItem( kXMP_NS_AEScart, "PostTimer", kXMP_PropArrayIsOrdered, NULL, kXMP_PropValueIsStruct );

				for ( XMP_Uns32 i = 0; i<CartMetadata::kPostTimerLength; i++ )
				{	
					// Ensure to write as Big Endian
					usageBE = MakeUns32BE(timerArray[i].usage);
					memcpy( usage, &usageBE, 4 );

					snprintf ( value, 24,	"%u", timerArray[i].value);

					SXMPUtils::ComposeArrayItemPath( kXMP_NS_AEScart, "PostTimer", i+1, &path);
					outXMP.SetStructField( kXMP_NS_AEScart,  path.c_str(), kXMP_NS_AEScart, "Usage", usage );
					outXMP.SetStructField( kXMP_NS_AEScart, path.c_str(), kXMP_NS_AEScart, "Value", value );
				}

				changed = true;
			}

			// import rest if cart properties
			changed |= IReconcile::importNativeToXMP ( outXMP, *cartData, kCartProperties, false );
		}
	
	}

	// cr8r is not yet required for WAVE

	////
	//// import cr8r
	//// Special case: If both Cr8r.AppName and LIST/INFO:ISFT are present, ISFT shall win.
	//// therefor import Cr8r first.
	////
	//Cr8rMetadata* cr8rMeta = inMetaData.get<Cr8rMetadata>();

	//if( cr8rMeta != NULL )
	//{
	//	changed |= IReconcile::importNativeToXMP( outXMP, *cr8rMeta, kCr8rProperties, false );
	//}

	//
	// Import LIST/INFO
	//
	INFOMetadata *infoMeta = inMetaData.get<INFOMetadata>();
	bool hasINAM = false;
	std::string actualLang;
	bool hasDCTitle = outXMP.GetLocalizedText( kXMP_NS_DC, "title", "" , "x-default" , &actualLang, NULL, NULL );

	if (infoMeta != NULL)
	{
		//
		// Remember if List/INFO:INAM has been imported
		//
		hasINAM = infoMeta->valueExists( INFOMetadata::kName );

		// Keywords are a ;-separated list and is therefore handled manually,
		// leveraging the XMPUtils functions
		if (infoMeta->valueExists( INFOMetadata::kKeywords ) )
		{
			std::string keywordsUTF8;
			outXMP.DeleteProperty( kXMP_NS_DC, "subject" );
			ReconcileUtils::NativeToUTF8( infoMeta->getValue<std::string>( INFOMetadata::kKeywords ), keywordsUTF8 );
			SXMPUtils::SeparateArrayItems( &outXMP, kXMP_NS_DC, "subject", kXMP_PropArrayIsUnordered, keywordsUTF8 );
			changed = true;
		}

		//
		// import properties
		//
		changed |= IReconcile::importNativeToXMP( outXMP, *infoMeta, kINFOProperties, hasDigest );
	}

	//
	// Import DISP
	// ! DISP will overwrite dc:title
	//
	bool hasDISP = false;

	DISPMetadata *dispMeta = inMetaData.get<DISPMetadata>();

	if( dispMeta != NULL && dispMeta->valueExists( DISPMetadata::kTitle) )
	{
		changed |= IReconcile::importNativeToXMP( outXMP, *dispMeta, kDISPProperties, hasDigest );
		hasDISP = true;
	}

	if( !hasDISP )
	{
		//
		// map INAM to dc:title ONLY in the case if:
		// * DISP does NOT exists
		// * dc:title does NOT exists
		// * INAM exists
		//
		if( !hasDCTitle && hasINAM )
		{
			std::string xmpValue;
			ReconcileUtils::NativeToUTF8( infoMeta->getValue<std::string>( INFOMetadata::kName ), xmpValue );
			outXMP.SetLocalizedText( kXMP_NS_DC, "title", NULL, "x-default", xmpValue.c_str() );
		}
	}

	// do timecode calculations
	if ( outXMP.DoesPropertyExist( kXMP_NS_BWF, kBWF_timeReference ) &&
		 outXMP.DoesPropertyExist( kXMP_NS_BWF, kDM_timeFormat ) &&
		 outXMP.DoesPropertyExist( kXMP_NS_BWF, kBWF_timeStampSampleRate ) )
	{
		std::string xmpValue;
		XMP_Int64 sampleRate = 0;
		XMP_Uns64 nSamples = 0;
		std::string timeFormat;
		bool ok = outXMP.GetProperty( kXMP_NS_BWF, kBWF_timeReference, &xmpValue, 0 );

		if ( ok )
		{
			int count;
			char nextCh;
			const char * strValue = xmpValue.c_str();
			count = sscanf ( strValue, "%llu%c", &nSamples, &nextCh );
		
			if ( count != 1 ) ok = false;
		}
		if ( ok )
			ok = outXMP.GetProperty_Int64( kXMP_NS_BWF, kBWF_timeStampSampleRate, &sampleRate, 0 );
		if ( ok )
			ok = outXMP.GetProperty( kXMP_NS_BWF, kDM_timeFormat, &timeFormat, 0 );

		if ( ok && sampleRate != 0 && timeFormat.size() > 0 ) {
			// compute time code from all the information we have
			std::string timecode;
			if ( TimeConversionUtils::ConvertSamplesToSMPTETimecode( timecode, nSamples, sampleRate, timeFormat ) ) {
				outXMP.SetStructField( kXMP_NS_DM, kDM_startTimecode, kXMP_NS_DM, kDM_timeValue, timecode, 0 );
				outXMP.SetStructField( kXMP_NS_DM, kDM_startTimecode, kXMP_NS_DM, kDM_timeFormat, timeFormat, 0 );
			}
		}
	}

	// delete transient properties
	outXMP.DeleteProperty( kXMP_NS_BWF, kBWF_timeStampSampleRate );
	outXMP.DeleteProperty( kXMP_NS_BWF, kDM_timeFormat );

	return changed;
}	// importToXMP


XMP_Bool WAVEReconcile::exportFromXMP( MetadataSet& outMetaData, SXMPMeta& inXMP )
{
	// tracks if anything has been exported from the XMP
	bool changed = false;

	//
	// Export DISP
	//
	DISPMetadata *dispMeta = outMetaData.get<DISPMetadata>();
	if (dispMeta != NULL)
	{
		// dc:title <-> DISP
		changed |= IReconcile::exportXMPToNative( *dispMeta, inXMP, kDISPProperties );
	}

	PropertyList propertiesToBeRemovedFromXMPMeta;
	if ( ! ignoreLocalText ) 
	{
		bool removeAllPropertiesinBextNameSpace = false;
		//
		// Export iXML
		//
		iXMLMetadata *iXMLMeta = outMetaData.get<iXMLMetadata>();
		if (iXMLMeta != NULL)
		{
			IReconcile::exportXMPToNative( *iXMLMeta, inXMP, kiXMLProperties, &propertiesToBeRemovedFromXMPMeta );
			exportSpecialXMPToiXML( inXMP, *iXMLMeta, propertiesToBeRemovedFromXMPMeta );
			changed |= iXMLMeta->hasChanged();
			removeAllPropertiesinBextNameSpace = true;
			// for maintaing backward compatibility we don't want to remove properties from xmp packet.
			propertiesToBeRemovedFromXMPMeta.clear();

			// update time code value
			if ( iXMLMeta->valueExists( iXMLMetadata::kTimeStampSampleRate ) && 
				 iXMLMeta->valueExists( iXMLMetadata::kTimeStampSampleSinceMidnightHigh ) &&
				 iXMLMeta->valueExists( iXMLMetadata::kTimeStampSampleSinceMidnightLow ) &&
				 iXMLMeta->valueExists( iXMLMetadata::kTimeCodeRate ) )
			{
				if ( iXMLMeta->valueChanged( iXMLMetadata::kTimeStampSampleRate ) ||
					 iXMLMeta->valueChanged( iXMLMetadata::kTimeStampSampleSinceMidnightHigh ) ||
					 iXMLMeta->valueChanged( iXMLMetadata::kTimeStampSampleSinceMidnightLow ) ||
					 iXMLMeta->valueChanged( iXMLMetadata::kTimeCodeRate ) ||
					 ( iXMLMeta->valueExists( iXMLMetadata::kTimeCodeFlag ) && iXMLMeta->valueChanged( iXMLMetadata::kTimeCodeFlag ) ) )
				{
					// update the time code value
					XMP_Int64 sampleRate = iXMLMeta->getValue<XMP_Uns64>( iXMLMetadata::kTimeStampSampleRate );
					XMP_Int64 nSamples = 0;
					std::string timeFormat;
					bool ok = inXMP.GetProperty_Int64( kXMP_NS_BWF, kBWF_timeReference, &nSamples, 0 );
					if ( ok )
						ok = inXMP.GetStructField( kXMP_NS_DM, kDM_startTimecode, kXMP_NS_DM, kDM_timeFormat, &timeFormat, 0 );

					if ( ok && sampleRate != 0 && timeFormat.size() > 0 ) {
						// compute time code from all the information we have
						std::string timecode;
						if ( TimeConversionUtils::ConvertSamplesToSMPTETimecode( timecode, nSamples, sampleRate, timeFormat ) ) {
							inXMP.SetStructField( kXMP_NS_DM, kDM_startTimecode, kXMP_NS_DM, kDM_timeValue, timecode, 0 );
						}
					}
					
				}
			}
		}

		//
		// Export BEXT
		//

		BEXTMetadata *bextMeta = outMetaData.get<BEXTMetadata>();

		if (bextMeta != NULL)
		{
			IReconcile::exportXMPToNative( *bextMeta, inXMP, kBextProperties );

			std::string xmpValue;

			// bext:umid  <-> RIFF:WAVE/bext.UMID
			if (inXMP.GetProperty(kXMP_NS_BWF, "umid", &xmpValue, 0))
			{
				std::string umid;
			
				if( this->decodeFromHexString( xmpValue, umid ) )
				{
					//
					// if the XMP property doesn't contain a valid hex string then
					// keep the existing value in the umid BEXT field
					//
					bextMeta->setArray<XMP_Uns8>(BEXTMetadata::kUMID, reinterpret_cast<const XMP_Uns8*>(umid.data()),((XMP_Uns32)umid.length()));
				}
			}
			else
			{
				bextMeta->deleteValue(BEXTMetadata::kUMID);
			}

			// bext:version  <-> RIFF:WAVE/bext.version
			// Special case: bext.version is always written as 1
			if (inXMP.GetProperty(kXMP_NS_BWF, "version", NULL, 0))
			{
				bextMeta->setValue<XMP_Uns16>(BEXTMetadata::kVersion, 1);
			}
			else
			{
				bextMeta->deleteValue(BEXTMetadata::kVersion);
			}

			removeAllPropertiesinBextNameSpace = true;

			changed |= bextMeta->hasChanged();
		}

		if ( removeAllPropertiesinBextNameSpace )
			SXMPUtils::RemoveProperties(&inXMP, kXMP_NS_BWF, NULL, kXMPUtil_DoAllProperties );

		//
		// Export cart
		//
		CartMetadata* cartData = outMetaData.get<CartMetadata>();

		if ( cartData != NULL ) 
		{
			IReconcile::exportXMPToNative ( *cartData, inXMP, kCartProperties );
			
			// Export PostTimer
			if ( inXMP.DoesPropertyExist( kXMP_NS_AEScart, "PostTimer" ) )
			{
				if( inXMP.CountArrayItems( kXMP_NS_AEScart, "PostTimer" ) == CartMetadata::kPostTimerLength )
				{
					
					CartMetadata::StoredCartTimer timer[CartMetadata::kPostTimerLength];
					memset ( timer, 0, sizeof(CartMetadata::StoredCartTimer)*CartMetadata::kPostTimerLength );	// Fill with zeros
					std::string path = "";
					std::string usageSt = "";
					std::string valueSt = "";
					XMP_Bool invalidArray = false;
					XMP_Uns32 usage = 0;
					XMP_Uns32 value = 0;
					XMP_Int64 tmp = 0;
					XMP_OptionBits opts;
					

					for ( XMP_Uns32 i = 0; i<CartMetadata::kPostTimerLength && !invalidArray; i++ )
					{	
						// get options of array item
						inXMP.GetArrayItem( kXMP_NS_AEScart, "PostTimer", i+1, NULL, &opts );
						// copose path to array item
						SXMPUtils::ComposeArrayItemPath( kXMP_NS_AEScart, "PostTimer", i+1, &path);						

						if ( opts == kXMP_PropValueIsStruct &&
							 inXMP.DoesStructFieldExist ( kXMP_NS_AEScart, path.c_str(), kXMP_NS_AEScart, "Usage" ) &&
							 inXMP.DoesStructFieldExist ( kXMP_NS_AEScart, path.c_str(), kXMP_NS_AEScart, "Value" ) )
						{
							inXMP.GetStructField( kXMP_NS_AEScart, path.c_str(), kXMP_NS_AEScart, "Usage", &usageSt, NULL);
							inXMP.GetStructField( kXMP_NS_AEScart, path.c_str(), kXMP_NS_AEScart, "Value", &valueSt, NULL);

							if ( stringToFOURCC( usageSt,usage ) )
							{
								// don't add if the String is not set or not exactly 4 characters
								timer[i].usage = usage;
						
								// now the value
								if ( valueSt.length() > 0 )
								{
									tmp = SXMPUtils::ConvertToInt64(valueSt);
									if ( tmp > 0  && tmp <= 0xffffffff)
									{								
										value = static_cast<XMP_Uns32>(tmp); // save because I checked that the number is positiv.
										timer[i].value = value;
									}
									// else value stays 0								
								}
								// else value stays 0
							}
						} 
						else
						{
							invalidArray = true;
						}
					}

					if (!invalidArray) // if the structure of the array is wrong don't add anything
					{
						cartData->setArray<CartMetadata::StoredCartTimer> (CartMetadata::kPostTimer, timer, CartMetadata::kPostTimerLength);
					}
				} // Array length is wrong: don't add anything
			}
			else
			{
				cartData->deleteValue( CartMetadata::kPostTimer );
			}
			SXMPUtils::RemoveProperties ( &inXMP, kXMP_NS_AEScart, 0, kXMPUtil_DoAllProperties );
			changed |= cartData->hasChanged();
		}
	}

	//
	// export LIST:INFO
	//
	INFOMetadata *infoMeta = outMetaData.get<INFOMetadata>();

	if (infoMeta != NULL)
	{
		IReconcile::exportXMPToNative( *infoMeta, inXMP, kINFOProperties );

		if ( inXMP.DoesPropertyExist(	 kXMP_NS_DC, "subject" ) )
		{
			std::string catedStr;
			SXMPUtils::CatenateArrayItems( inXMP, kXMP_NS_DC, "subject", NULL, NULL, kXMP_NoOptions, &catedStr );
			infoMeta->setValue<std::string>(INFOMetadata::kKeywords, catedStr);
		}
		else
		{
			infoMeta->deleteValue( INFOMetadata::kKeywords );
		}

		// Remove RIFFINFO properties from the XMP
		SXMPUtils::RemoveProperties(&inXMP, kXMP_NS_RIFFINFO, NULL, kXMPUtil_DoAllProperties );

		changed |= infoMeta->hasChanged();
	}

	// cr8r is not yet required for WAVE

	////
	//// export cr8r
	////
	//Cr8rMetadata* cr8rMeta = outMetaData.get<Cr8rMetadata>();

	//if( cr8rMeta != NULL )
	//{
	//	changed |= IReconcile::exportXMPToNative( *cr8rMeta, inXMP, kCr8rProperties );
	//}

	//
	// remove WAV digest
	//
	inXMP.DeleteProperty( kXMP_NS_WAV, "NativeDigest" );

	for ( PropertyList::iterator it = propertiesToBeRemovedFromXMPMeta.begin(), last = propertiesToBeRemovedFromXMPMeta.end(); it != last; it++ )
	{
		inXMP.DeleteProperty( it->first, it->second );
	}

	return changed;
}	// exportFromXMP


// ************** Helper Functions ***************** //

bool WAVEReconcile::encodeToHexString ( const XMP_Uns8* input, std::string& output )
{
	bool allZero = true; // assume for now
	XMP_Uns32 kFixedSize = 64; // Only used for UMID Bext field, which is fixed
	output.erase();

	if ( input != 0 )
	{
		output.reserve ( kFixedSize * 2 );
				
		for( XMP_Uns32 i = 0; i < kFixedSize; i++ )
		{
			// first, second nibble
			XMP_Uns8 first = input[i] >> 4;
			XMP_Uns8 second = input[i] & 0xF;

			if ( allZero && (( first != 0 ) || (second != 0)))
				allZero = false;

			output.append( 1, ReconcileUtils::kHexDigits[first] );
			output.append( 1, ReconcileUtils::kHexDigits[second] );	
		}
	}
	return allZero;
}	// encodeToHexString


bool WAVEReconcile::decodeFromHexString ( std::string input, std::string &output)
{
	if ( (input.length() % 2) != 0 )
		return false;
	output.erase();
	output.reserve ( input.length() / 2 );

	for( XMP_Uns32 i = 0; i < input.length(); )
	{	
		XMP_Uns8 upperNibble = input[i];
		if ( (upperNibble < 48) || ( (upperNibble > 57 ) && ( upperNibble < 65 ) ) || (upperNibble > 70) )
		{
			return false;
		}
		if ( upperNibble >= 65 )
		{		
			upperNibble -= 7; // shift A-F area adjacent to 0-9
		}
		upperNibble -= 48; // 'shift' to a value [0..15]
		upperNibble = ( upperNibble << 4 );
		i++;

		XMP_Uns8 lowerNibble = input[i];
		if ( (lowerNibble < 48) || ( (lowerNibble > 57 ) && ( lowerNibble < 65 ) ) || (lowerNibble > 70) )
		{
			return false;
		}
		if ( lowerNibble >= 65 )
		{		
			lowerNibble -= 7; // shift A-F area adjacent to 0-9
		}
		lowerNibble -= 48; // 'shift' to a value [0..15]
		i++;

		output.append ( 1, (upperNibble + lowerNibble) );
	}
	return true;
}	// decodeFromHexString

bool WAVEReconcile::stringToFOURCC ( std::string input, XMP_Uns32 &output )
{
	bool result = false;
	std::string asciiST = "";
	
	// convert to ACSII
	convertToASCII(input, asciiST);
	if ( asciiST.length() == 4 )
	{

		output = GetUns32BE( asciiST.c_str() );
		result = true;
	}
	
	return result;
}

struct iXMLAudioSampleTypeMapping
{
	const char *			xmpStringValue;
	XMP_Uns64				ixmlIntValue;
};

iXMLAudioSampleTypeMapping ixmlAudioSampleTypeMappings[] = {
	{ "8Int",		8 },
	{ "16Int",		16 },
	{ "24Int",		24 },
	{ "32Float",	32 },
};

struct iXMLTimeCodeRateAndFlagMapping 
{
	const char *			xmpStringValue;
	const char *			ixmlTimeCodeRateValue;
	const char *			ixmlTimeCodeFlagValue;
};

iXMLTimeCodeRateAndFlagMapping ixmlTimeCodeRateAndFlagMappings[] = {
	{ "24Timecode",				"24/1",			"NDF" },
	{ "25Timecode",				"25/1",			"NDF" },
	{ "2997DropTimecode",		"30000/1001",	"DF" },
	{ "2997NonDropTimecode",	"30000/1001",	"NDF" },
	{ "30Timecode",				"30/1",			"NDF" },
	{ "50Timecode",				"50/1",			"NDF" },
	{ "5994DropTimecode",		"60000/1001",	"DF" },
	{ "5994NonDropTimecode",	"60000/1001",	"NDF" },
	{ "60Timecode",				"60/1",			"NDF" },
	{ "23976Timecode",			"24000/1001",	"NDF" },
};

void IFF_RIFF::WAVEReconcile::exportSpecialXMPToiXML( SXMPMeta & inXMP, IMetadata & outNativeMeta, PropertyList & propertiesToBeDeleted )
{
	std::string sXmpValue;
	XMP_Int64 iXMPValue;

	// special case for NoGood and Circled // xmpDM:good ,-> iXML:NO_GOOD and iXML:CIRCLED

	// special case for AudioBitDepth // xmpDM:audioSampleType <-> iXML:AUDIO_BIT_DEPTH
	bool deleteNativeEntry = false;
	try
	{
		if ( inXMP.GetProperty( kXMP_NS_DM, kDM_audioSampleType, &sXmpValue, 0 ) )
		{
			bool matchingValueFound = false;
			XMP_Uns64 ixmlValue = 0;
			for ( size_t i = 0, total = sizeof(ixmlAudioSampleTypeMappings )/sizeof( iXMLAudioSampleTypeMapping); i < total; i++ )
			{
				if ( sXmpValue.compare( ixmlAudioSampleTypeMappings[i].xmpStringValue ) == 0 )
				{
					ixmlValue = ixmlAudioSampleTypeMappings[i].ixmlIntValue;
					matchingValueFound = true;
					break;
				}
			}

			if ( matchingValueFound )
			{
				outNativeMeta.setValue< XMP_Uns64 >( iXMLMetadata::kAudioBitDepth, ixmlValue );
				propertiesToBeDeleted.push_back( std::make_pair( kXMP_NS_DM, kDM_audioSampleType ) );
				deleteNativeEntry = false;
			}
			else
			{
				deleteNativeEntry = true;
			}
		}
		else
		{
			deleteNativeEntry = true;
		}

		if ( deleteNativeEntry )
		{
			if ( outNativeMeta.valueExists( iXMLMetadata::kAudioBitDepth ) )
			{
				XMP_Uns64 ixmlValue = outNativeMeta.getValue< XMP_Uns64 >( iXMLMetadata::kAudioBitDepth );
				bool validValue = false;
				for ( size_t i = 0, total = sizeof(ixmlAudioSampleTypeMappings )/sizeof( iXMLAudioSampleTypeMapping); i < total; i++ )
				{
					if ( ixmlValue == ixmlAudioSampleTypeMappings[i].ixmlIntValue )
					{
						validValue = true;
						break;
					}
				}
				if ( validValue )
					outNativeMeta.deleteValue( iXMLMetadata::kAudioBitDepth );
			}
		}
	}
	catch ( ... )
	{
		// do nothing
	}

	// Special case: On export BEXT:version is always written as 1
	try
	{
		if ( inXMP.GetProperty( kXMP_NS_BWF, "version", NULL, 0 ) )
		{
			outNativeMeta.setValue< XMP_Uns64 >( iXMLMetadata::kBWFVersion, 1 );
		}
		else
		{
			outNativeMeta.deleteValue( iXMLMetadata::kBWFVersion );
		}
	}
	catch( ... )
	{
		// do nothing
	}

	// special case for xmpDM:startTimecode\xmpDM:timeFormat
	try
	{
		if ( inXMP.GetStructField( kXMP_NS_DM, kDM_startTimecode, kXMP_NS_DM, kDM_timeFormat, &sXmpValue, 0 ) )
		{
			bool matchingValueFound = false;
			const char * ixmlValueForTimeCodeRate = NULL;
			const char * ixmlValueForTimeCodeFlag = NULL;
			for ( size_t i = 0, total = sizeof(ixmlTimeCodeRateAndFlagMappings)/sizeof(iXMLTimeCodeRateAndFlagMapping); i < total; i++ )
			{
				if ( sXmpValue.compare( ixmlTimeCodeRateAndFlagMappings[i].xmpStringValue ) == 0 )
				{
					ixmlValueForTimeCodeRate = ixmlTimeCodeRateAndFlagMappings[i].ixmlTimeCodeRateValue;
					ixmlValueForTimeCodeFlag = ixmlTimeCodeRateAndFlagMappings[i].ixmlTimeCodeFlagValue;
					matchingValueFound = true;
					deleteNativeEntry = false;
					break;
				}
			}

			if ( matchingValueFound )
			{
				outNativeMeta.setValue< std::string >( iXMLMetadata::kTimeCodeRate, ixmlValueForTimeCodeRate );
				outNativeMeta.setValue< std::string >( iXMLMetadata::kTimeCodeFlag, ixmlValueForTimeCodeFlag );
			}
			else
			{
				deleteNativeEntry = true;
			}
		}
		else
		{
			deleteNativeEntry = true;
		}

		if ( deleteNativeEntry )
		{
			if ( outNativeMeta.valueExists( iXMLMetadata::kTimeCodeRate ) )
			{
				std::string ixmlValueForTimecodeRate = outNativeMeta.getValue< std::string >( iXMLMetadata::kTimeCodeRate );
				bool validValue = false;
				for ( size_t i = 0, total = sizeof(ixmlTimeCodeRateAndFlagMappings)/sizeof(iXMLTimeCodeRateAndFlagMapping); i < total; i++ )
				{
					if ( ixmlValueForTimecodeRate.compare( ixmlTimeCodeRateAndFlagMappings[i].ixmlTimeCodeRateValue ) == 0 )
					{
						validValue = true;
						break;
					}
				}
				if ( validValue )
				{
					outNativeMeta.deleteValue( iXMLMetadata::kTimeCodeRate );
					outNativeMeta.deleteValue( iXMLMetadata::kTimeCodeFlag );
				}
			}
		}
	}
	catch( ... )
	{
		// do nothing
	}

	// special case for timeReference // bext:timeReference <-> iXML:BWF_TIME_REFERENCE_LOW and iXML:BWF_TIME_REFERENCE_HIGH
	bool bextTimeReferenceDataAvailable = false;
	XMP_Uns32 bextLowValue = 0;
	XMP_Uns32 bextHighValue = 0;
	bool timeCodeDataAvailable = false;
	XMP_Uns32 tcLowValue = 0;
	XMP_Uns32 tcHighValue = 0;

	try
	{
		if ( inXMP.GetProperty_Int64( kXMP_NS_BWF, "timeReference", &iXMPValue, 0 ) )
		{
			bextTimeReferenceDataAvailable = true;
			XMP_Uns64 uXmpValue = ( XMP_Uns64 )( iXMPValue );
			bextLowValue = ( XMP_Uns32 ) uXmpValue;
			bextHighValue = ( XMP_Uns32 ) ( uXmpValue >> 32 );
		}
	}
	catch ( ... )
	{
		// do nothing
	}

#if 0 // reverse calculation from timecode to samples can result in different number of samples. So ignoring for time being
	if ( outNativeMeta.valueExists( iXMLMetadata::kTimeStampSampleRate ) )
	{
		try
		{
			std::string timeFormat, timeValue;
			if ( inXMP.GetStructField( kXMP_NS_DM, kDM_startTimecode, kXMP_NS_DM, kDM_timeValue, &timeValue, 0 ) &&
				inXMP.GetStructField( kXMP_NS_DM, kDM_startTimecode, kXMP_NS_DM, kDM_timeFormat, &timeFormat, 0) )
			{
				XMP_Int64 nSamples;
				if ( TimeConversionUtils::ConvertSMPTETimecodeToSamples( nSamples, timeValue,
					outNativeMeta.getValue< XMP_Uns64 >( iXMLMetadata::kTimeStampSampleRate ), timeFormat ) )
				{
					timeCodeDataAvailable = true;
					XMP_Uns64 uXmpValue = ( XMP_Uns64 )( nSamples );
					tcLowValue = ( XMP_Uns32 ) uXmpValue;
					tcHighValue = ( XMP_Uns32 ) ( uXmpValue >> 32 );
				}
				
			}
		}
		catch ( ... )
		{
			// do nothing
		}
	}

	if ( bextTimeReferenceDataAvailable && timeCodeDataAvailable )
	{
		// pick the one which is different, if both different pick the bext one
		XMP_Uns64 
	}
	else if ( bextTimeReferenceDataAvailable )
	{
	}
	else if ( timeCodeDataAvailable )
	{
	}
	else  // none of the bextTimeReference and timeCode data is available
	{
		outNativeMeta.deleteValue( iXMLMetadata::kTimeStampSampleSinceMidnightHigh );
		outNativeMeta.deleteValue( iXMLMetadata::kTimeStampSampleSinceMidnightLow );
		outNativeMeta.deleteValue( iXMLMetadata::kBWFTimeReferenceHigh );
		outNativeMeta.deleteValue( iXMLMetadata::kBWFTimeReferenceLow );
	}
#endif

	if ( bextTimeReferenceDataAvailable ) {
		outNativeMeta.setValue< XMP_Uns64 >( iXMLMetadata::kBWFTimeReferenceHigh, bextHighValue );
		outNativeMeta.setValue< XMP_Uns64 >( iXMLMetadata::kBWFTimeReferenceLow, bextLowValue );
		outNativeMeta.setValue< XMP_Uns64 >( iXMLMetadata::kTimeStampSampleSinceMidnightHigh, bextHighValue );
		outNativeMeta.setValue< XMP_Uns64 >( iXMLMetadata::kTimeStampSampleSinceMidnightLow, bextLowValue );
	} else {
		outNativeMeta.deleteValue( iXMLMetadata::kTimeStampSampleSinceMidnightHigh );
		outNativeMeta.deleteValue( iXMLMetadata::kTimeStampSampleSinceMidnightLow );
		outNativeMeta.deleteValue( iXMLMetadata::kBWFTimeReferenceHigh );
		outNativeMeta.deleteValue( iXMLMetadata::kBWFTimeReferenceLow );
	}

	// track list
	try {
		if ( inXMP.DoesPropertyExist( kXMP_NS_iXML, kIXML_trackList ) ) {
			XMP_OptionBits options( 0 );
			if ( inXMP.GetProperty( kXMP_NS_iXML, kIXML_trackList, NULL, &options ) &&
				XMP_OptionIsSet( options, kXMP_PropArrayIsUnordered ) )
			{
				XMP_Index count = inXMP.CountArrayItems( kXMP_NS_iXML, kIXML_trackList );
				std::vector< iXMLMetadata::TrackListInfo > trackListInfo;
				XMP_Index size = count;
				for ( XMP_Index i = 0; i < count; i++ ) {
					std::string trackPath;
					SXMPUtils::ComposeArrayItemPath( kXMP_NS_iXML, kIXML_trackList, i + 1, &trackPath );
					std::string fieldPath, channelIndex, interleaveIndex, name, function;
					SXMPUtils::ComposeStructFieldPath( kXMP_NS_iXML, trackPath.c_str(), kXMP_NS_iXML, kIXML_channelIndex, &fieldPath );
					//XMP_Int64 int64Value;
					
					inXMP.GetStructField( kXMP_NS_iXML, trackPath.c_str(), kXMP_NS_iXML, kIXML_channelIndex, &channelIndex, &options );
					inXMP.GetStructField(kXMP_NS_iXML, trackPath.c_str(), kXMP_NS_iXML, kIXML_interleaveIndex, &interleaveIndex, &options);
					inXMP.GetStructField(kXMP_NS_iXML, trackPath.c_str(), kXMP_NS_iXML, kIXML_Name, &name, &options);
					inXMP.GetStructField( kXMP_NS_iXML, trackPath.c_str(), kXMP_NS_iXML, kIXML_Function, &function, &options );
					
					if (channelIndex.size() > 0 || interleaveIndex.size() > 0 || name.size() > 0 || function.size() > 0)
					{
						iXMLMetadata::TrackListInfo trackRef(channelIndex, name, function, interleaveIndex);
						trackListInfo.push_back(trackRef);
					}
					else
						size--;
				}
				outNativeMeta.setArray< iXMLMetadata::TrackListInfo >( iXMLMetadata::kTrackList, trackListInfo.data(), size );
				outNativeMeta.setValue<XMP_Uns64>(iXMLMetadata::kNativeTrackCount, (XMP_Uns64)size);
				inXMP.DeleteProperty( kXMP_NS_iXML, kIXML_trackList );
			}
		} else {
			outNativeMeta.deleteValue( iXMLMetadata::kTrackList );
			outNativeMeta.deleteValue(iXMLMetadata::kNativeTrackCount);
		}
	} catch( ... ) {
		// do nothing
	}
}

bool IFF_RIFF::WAVEReconcile::exportSpecialiXMLToXMP( IMetadata & inNativeMeta, SXMPMeta & outXMP )
{
	bool changed = false;
	// special case for AudioBitDepth // xmpDM:audioSampleType <-> iXML:AUDIO_BIT_DEPTH
	if ( inNativeMeta.valueExists( iXMLMetadata::kAudioBitDepth ) ) {
		XMP_Uns64 ixmlValue = inNativeMeta.getValue< XMP_Uns64 >( iXMLMetadata::kAudioBitDepth);
		const char * xmpValue = NULL;
		bool matchingValueFound = false;

		for ( size_t i = 0, total = sizeof(ixmlAudioSampleTypeMappings)/sizeof(iXMLAudioSampleTypeMapping); i < total; i++ )
		{
			if ( ixmlAudioSampleTypeMappings[i].ixmlIntValue == ixmlValue )
			{
				xmpValue = ixmlAudioSampleTypeMappings[i].xmpStringValue;
				matchingValueFound = true;
				break;
			}
		}
		if ( matchingValueFound )
		{
			outXMP.SetProperty( kXMP_NS_DM, kDM_audioSampleType, xmpValue );
			changed = true;
		}
	}

	// special case for timeReference // bext:timeReference <-> iXML:TIME_SAMPLES_SINCE_MIDNIGHT_LO and iXML:TIME_SAMPLES_SINCE_MIDNIGHT_HI
	if ( inNativeMeta.valueExists( iXMLMetadata::kTimeStampSampleSinceMidnightHigh ) && inNativeMeta.valueExists( iXMLMetadata::kTimeStampSampleSinceMidnightLow ) ) {
		XMP_Uns64 combinedValue = inNativeMeta.getValue< XMP_Uns64 >( iXMLMetadata::kTimeStampSampleSinceMidnightHigh );
		combinedValue = combinedValue << 32;
		combinedValue += inNativeMeta.getValue< XMP_Uns64 >( iXMLMetadata::kTimeStampSampleSinceMidnightLow );
		std::string strValue;
		SXMPUtils::ConvertFromInt64( combinedValue, "%llu", &strValue );
		outXMP.SetProperty( kXMP_NS_BWF, "timeReference", strValue );
		changed = true;
	}

	// special case for timeReference // bext:timeReference <-> iXML:BWF_TIME_REFERENCE_LOW and iXML:BWF_TIME_REFERENCE_HIGH
	if ( inNativeMeta.valueExists( iXMLMetadata::kBWFTimeReferenceHigh ) && inNativeMeta.valueExists( iXMLMetadata::kBWFTimeReferenceLow ) ) {
		XMP_Uns64 combinedValue = inNativeMeta.getValue< XMP_Uns64 >( iXMLMetadata::kBWFTimeReferenceHigh );
		combinedValue = combinedValue << 32;
		combinedValue += inNativeMeta.getValue< XMP_Uns64 >( iXMLMetadata::kBWFTimeReferenceLow );
		std::string strValue;
		SXMPUtils::ConvertFromInt64( combinedValue, "%llu", &strValue );
		outXMP.SetProperty( kXMP_NS_BWF, "timeReference", strValue );
		changed = true;
	}

	// special case for xmpDM:startTimecode\xmpDM:timeFormat
	if ( inNativeMeta.valueExists( iXMLMetadata::kTimeCodeRate ) )
	{
		std::string ixmlTimecodeRateValue = inNativeMeta.getValue<std::string>( iXMLMetadata::kTimeCodeRate );
		std::string ixmlTimecodeFlagValue = "NDF";
		const char * xmpValue = NULL;
		bool matchingValueFound = false;
		if ( inNativeMeta.valueExists( iXMLMetadata::kTimeCodeFlag ) )
		{
			ixmlTimecodeFlagValue = inNativeMeta.getValue<std::string>( iXMLMetadata::kTimeCodeFlag );
		}

		for ( size_t i = 0, total = sizeof(ixmlTimeCodeRateAndFlagMappings)/sizeof(iXMLTimeCodeRateAndFlagMapping); i < total; i++ )
		{
			if ( ( ixmlTimecodeRateValue.compare(ixmlTimeCodeRateAndFlagMappings[i].ixmlTimeCodeRateValue) == 0 ) &&
				 ( ixmlTimecodeFlagValue.compare(ixmlTimeCodeRateAndFlagMappings[i].ixmlTimeCodeFlagValue) == 0 ) )
			{
				xmpValue = ixmlTimeCodeRateAndFlagMappings[i].xmpStringValue;
				matchingValueFound = true;
				break;
			}
		}
		if ( matchingValueFound )
		{
			outXMP.SetProperty( kXMP_NS_BWF, kDM_timeFormat, xmpValue );
			changed = true;
		}
	}

	// special case for iXML:trackList
	if ( inNativeMeta.valueExists( iXMLMetadata::kTrackList ) )
	{
		XMP_Uns32 countOfTracks( 0 );
		const iXMLMetadata::TrackListInfo * trackInfoArray = 
			inNativeMeta.getArray< iXMLMetadata::TrackListInfo >( iXMLMetadata::kTrackList, countOfTracks );
		if ( countOfTracks > 0 && trackInfoArray != NULL ) {
			outXMP.DeleteProperty( kXMP_NS_iXML, kIXML_trackList );
			outXMP.SetProperty( kXMP_NS_iXML, kIXML_trackList, 0, kXMP_PropArrayIsUnordered);
			for ( XMP_Uns32 i = 0; i < countOfTracks; i++ ) {
				std::string trackPath;
				SXMPUtils::ComposeArrayItemPath( kXMP_NS_iXML, kIXML_trackList, i + 1, &trackPath );
				const iXMLMetadata::TrackListInfo & ref = trackInfoArray[i];
				std::string value;
				if(ref.mChannelIndex.size()>0)
					outXMP.SetStructField( kXMP_NS_iXML, trackPath.c_str(), kXMP_NS_iXML, kIXML_channelIndex, ref.mChannelIndex);
				if (ref.mInterleaveIndex.size()>0)
					outXMP.SetStructField(kXMP_NS_iXML, trackPath.c_str(), kXMP_NS_iXML, kIXML_interleaveIndex, ref.mInterleaveIndex);
				if (ref.mName.size()>0)
					outXMP.SetStructField( kXMP_NS_iXML, trackPath.c_str(), kXMP_NS_iXML, kIXML_Name, ref.mName );
				if (ref.mFunction.size()>0)
					outXMP.SetStructField( kXMP_NS_iXML, trackPath.c_str(), kXMP_NS_iXML, kIXML_Function, ref.mFunction );
			}
			changed = true;
		}
	}

	return changed;
}
