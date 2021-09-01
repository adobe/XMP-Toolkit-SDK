// =================================================================================================
// Copyright 2008 Adobe Systems Incorporated
// All Rights Reserved. 
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
//
// =================================================================================================
//
// DumpFile is suitable for both dumping an entire file structure to screen _as well as_ access to 
// specific to specific legacy fields (as much required for auto-testing). 
// 
//  Currently supports
//     - JPEG
//     - TIFF 
//     - PHOTOSHOP
//     - JPEG2K
//     - WMAV (ASF/WMA/WMV)
//     - IFF/RIFF (AVI/WAV/RF64/AIFF/AIFF-C)
//     - PNG
//     - InDesign
//     - MP3
//     - MOV (Quicktime)
//     - UCF (done, including commented zips and zip64 (>4GB))
//     - SWF
//     - FLV
//     - MPEG-4
//
// DumpFile does depend on XMPCore and the packetscanner from XMPFiles. 


#include <stdarg.h>

#include "source/ExpatAdapter.hpp"

#include "samples/source/common/globals.h"
#include "samples/source/common/DumpFile.h"
#include "samples/source/common/LargeFileAccess.hpp"
static const XMP_Uns32 CONST_INFINITE = (XMP_Uns32)(-1);

// converts a (supposed) 8Bit-encoded String to Buginese
// - suitable for UTF-8 or any other encoding
// - stopOnNull does just that, exceeding length is declared as |R:1234 at the end of string
// - len is the max parsing size, defaults to unlimited
// - having set neither stopOnNull nor max parsing size is a bad idea (checked and yields error)
std::string convert8Bit(void* str, bool stopOnNull, XMP_Uns32 byteLen)
{
	std::string r;	//result
	r.clear();	// ...I have become cautious... :-)

	if (byteLen == 0)
		return r; //nothing to do

				  //provoke access violation:
				  //if invalid length leads to access violation, I want it here and now...
	if (byteLen != CONST_INFINITE) { //if not "CONST_INFINITE"
		char tmp = ((char*)str)[byteLen - 1];
	}
	if (!stopOnNull && (byteLen == CONST_INFINITE))
		Log::error("must set either stopOnNULL or specify length of string");

	bool outside = false;	// toggle-flag: outside ASCII ?
	XMP_Uns32 remainder = 0;
	char buffer[200];  //changed from 20 to 200 (whatever reason there was to have it so small)

	for (XMP_Uns32 i = 0;
		i<byteLen;  //either byteLen==0 or run forever (read: till 'break')
		i++)
	{
		XMP_Uns8 ch = ((char*)str)[i];
		if ((0x20 <= ch) && (ch <= 0x7E))
		{	//outside-case
			if (outside)
				r += ">";
			r += ch;
			outside = false;
		}
		else {
			if (!outside)
				r += "<";	//first inside-case
			else
				r += " ";
			sprintf(buffer, "%.2X", ch);
			r += buffer;
			outside = true;
		}

		if (stopOnNull && (ch == 0)) {
			if (byteLen != CONST_INFINITE) remainder = byteLen - i - 2;
			break;
		}
	}

	if (outside) r += ">";
	if (remainder>0) {
		sprintf(buffer, "|R:%d", remainder);
		r += buffer;
	}
	return r;
}


//same story, but for (UTF-)16BE characters
//note: length still to be specified in byte, thus must be even (verified)
std::string convert16Bit(bool bigEndian, XMP_Uns8* str, bool stopOnNull, XMP_Uns32 byteLen)
{
	//if invalid length leads to access violation, I want it here and now...
	if (byteLen != CONST_INFINITE) {
		char tmp = str[byteLen - 1];
	}

	if (!stopOnNull && (byteLen == CONST_INFINITE))
		Log::error("must set either stopOnNULL or specify length of string");
	if ((byteLen != CONST_INFINITE) && (byteLen % 2 != 0))  //if neither CONST_INFINITE or even...#
		Log::error("convert16BitToBuginese: byteLen must be even");


	bool outside = false;	// toggle-flag: outside ASCII ?
	XMP_Uns32 remainder = 0;
	char buffer[20];

	std::string r;	//result
	r.clear();	// ...I have become cautious... :-)

	for (XMP_Uns32 i = 0;
		i<byteLen;  //either byteLen==0 or run forever (read: till 'break')
		i = i + 2)
	{
		XMP_Uns16 ch = (bigEndian) ? GetUns16BE(&str[i]) : GetUns16LE(&str[i]);

		if ((0x20 <= ch) && (ch <= 0x7E))
		{	//outside-case
			if (outside)
				r += ">";
			r += (char)ch;
			outside = false;
		}
		else {
			if (!outside)
				r += "<";	//first inside-case
			else
				r += " ";
			//I want to reflect actual byte order when dumping, thus byte-wise here and no endian-fork
			sprintf(buffer, "%.2X %.2X", str[i], str[i + 1]);
			r += buffer;
			outside = true;
		}

		if (stopOnNull && (ch == 0)) {
			if (byteLen != CONST_INFINITE) remainder = byteLen - i - 2;
			break;
		}
	}

	if (outside) r += ">";
	if (remainder>0) {
		sprintf(buffer, "|R:%d", remainder);
		r += buffer;
	}
	return r;
}


std::string fromArgs(const char* format, ...)
{
	//note: format and ... are somehow "used up", i.e. dumping them 
	//      via vsprintf _and_ via printf brought up errors on Mac (only)
	//      i.e. %d %X stuff looking odd (roughly like signed vs unsigned...)
	//      buffer reuse is fine, just dont use format/... twice.

	char buffer[4096];  //should be big enough but no guarantees..
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	return std::string(buffer);
}

#include <iostream>
#include <stdarg.h>	//fpr va_list et al

DumpFileException::DumpFileException(const char *format, ...) : std::runtime_error("dumpfile exception")
{
	va_list args; va_start(args, format);
	vsnprintf(buffer, XMPQE_MAX_ERROR_LENGTH, format, args);
	va_end(args);
}

// overriding, since the buffer needs to be constructed first...
const char* DumpFileException::what_dumpfile_reason()
{
	return buffer;
}

// REMOVED ON PURPOSE: #include <assert.h>
// define two assert macros, /w and w/o msg
// (we don't want to slip malformed files through. Neither in release mode.)
#undef assertMsg
#undef assert

//TODO: integrate file pos in parse failure description (LFA_Tell ()...)

// this method just 
#define assertNoThrowMsg(msg,c)                                                             \
try { c }                                                                                   \
catch ( ... ) {                                                                             \
	throw DumpFileException( "- assert:   %s\n- message:  %s\n- location: " __FILE__ ":%u", \
                            #c, std::string( msg ).c_str(), __LINE__ );                     \
}

#define assertMsg(msg,c)																				\
if ( ! (c) ) {																							\
	throw DumpFileException( "- assert:   %s\n- message:  %s\n- location: " __FILE__ ":%u", #c, std::string( msg ).c_str(), __LINE__ );		\
}

#define assert(c)																				\
if ( ! (c) ) {																					\
	throw DumpFileException( "- assert:   %s\n- location: " __FILE__ ":%u", #c, __LINE__ );		\
}

#define assertEOF(file)		\
if ( ! LFA_isEof(file) ) {	\
	throw DumpFileException( "- assert:   feof(file)\n- message:  end of file not reached, still at 0x%X\n- location: " __FILE__ ":%u", LFA_Tell (file), __LINE__ );		\
}

#define fail(msg)																				\
	throw DumpFileException( "- failure\n- message:  %s\n- location: " __FILE__ ":%u", std::string( msg ).c_str(), __LINE__ );

using namespace std;

//XMPCore related
//! no use of XMPFiles
//! no "XMP.incl_cpp" here, happens in Dumpfile/main.cpp resp. CppUnit/main.cpp
#define TXMP_STRING_TYPE std::string
#include "public/include/XMP.hpp"
#include "public/include/XMP_Const.h"

#include "samples/source/common/XMPScanner.hpp"
#include "samples/source/common/Log.h"
//disabled warning (take-over)
#if XMP_WinBuild
#pragma warning (disable : 4996)	// '...' was declared deprecated
#pragma warning (disable : 4244)	// conversion from '__w64 int' to 'XMP_Uns32', possible loss of data
#pragma warning (disable : 4267)	// conversion from 'size_t' to 'int', possible loss of data
#endif

#pragma pack (1)

//the tag tree to be build up, 
//  then dumped (dumpfile.exe) resp.
//  resp. queried (testrunner)
static TagTree* tree;

// specifc 'state machine' for QT/MPEG4 dumping
// * false by default (set in DumpISO() stub)
static bool TimeCodeTrack;

// =================================================================================================

long kOne = 1;
char firstByte = *((char*)&kOne);

const bool sBigEndianHost = (firstByte == 0);
const bool sLittleEndianHost = (firstByte == 1);
static bool beTIFF;

typedef const char * ChPtr;
#define CheckBytes(left,right,len)	(memcmp (((ChPtr)(left)), ((ChPtr)(right)), len) == 0)

static XMP_Uns8* sDataPtr = 0;	// Used via CaptureFileData for variable length data.
static XMP_Uns32 sDataMax = 0;
static XMP_Uns32 sDataLen = 0;

// storing XMP Info 'globally' for a later dump...
static XMP_Uns8* sXMPPtr = 0;	// Used via CaptureXMP for the main XMP.
static XMP_Uns32 sXMPMax = 0;
static XMP_Uns32 sXMPLen = 0;
static XMP_Int64 sXMPPos = 0;

typedef XMP_Uns16(*GetUns16_Proc) (const void * addr);
typedef XMP_Uns32(*GetUns32_Proc) (const void * addr);
typedef XMP_Uns64(*GetUns64_Proc) (const void * addr);

static XMP_Uns16 GetUns16BE(const void * addr);
static XMP_Uns16 GetUns16LE(const void * addr);
static XMP_Uns32 GetUns32BE(const void * addr);
static XMP_Uns32 GetUns32LE(const void * addr);
static XMP_Uns64 GetUns64BE(const void * addr);
static XMP_Uns64 GetUns64LE(const void * addr);

#define High32(u64) ((XMP_Uns32)((u64) >> 32))
#define Low32(u64)  ((XMP_Uns32)((u64) & 0xFFFFFFFFUL))

// =================================================================================================

// ahead declarations
struct JpegMarker {
	XMP_Uns8 * jpegMarkerPtr;
	XMP_Uns16 jpegMarkerLen;
};
typedef std::vector<JpegMarker> JpegMarkers;

static void DumpTIFF(XMP_Uns8 * tiffContent, XMP_Uns32 tiffLen, XMP_Uns32 fileOffset, const char * label, std::string path, bool isHeaderAbsent = false);
static void DumpTIFF(const JpegMarkers& psirMarkers, XMP_Uns8 * dataStart, const char * label, std::string path);
static void DumpIPTC(XMP_Uns8 * iptcOrigin, XMP_Uns32 iptcLen, XMP_Uns32 fileOffset, const char * label);
static void DumpImageResources(XMP_Uns8 * psirOrigin, XMP_Uns32 psirLen, XMP_Uns32 fileOffset, const char * label);
static void DumpImageResources(const JpegMarkers& psirMarkers, XMP_Uns8 * dataStart, const char * label);
static void DumpIFDChain(XMP_Uns8 * startPtr, XMP_Uns8 * endPtr, XMP_Uns8 * tiffContent, XMP_Uns32 fileOffset, const char * label, std::string path, bool isHeaderAbsent = false);

// =================================================================================================

static GetUns16_Proc TIFF_GetUns16 = 0;	// Keeps endian procs for the "current" TIFF.
static GetUns32_Proc TIFF_GetUns32 = 0;
static GetUns64_Proc TIFF_GetUns64 = 0;

enum {	// Special TIFF tags
	kTIFF_XMP = 700,
	kTIFF_IPTC = 33723,
	kTIFF_PSIR = 34377,
	kTIFF_Exif = 34665,
	kTIFF_GPS = 34853,
	kTIFF_MakerNote = 37500,
	kTIFF_Interop = 40965
};

enum {	// Special Photoshop image resource IDs
	kPSIR_OldCaption = 1008,
	kPSIR_PrintCaption = 1020,
	kPSIR_IPTC = 1028,
	kPSIR_CopyrightFlag = 1034,
	kPSIR_CopyrightURL = 1035,
	kPSIR_Exif_1 = 1058,
	kPSIR_Exif_3 = 1059,
	kPSIR_XMP = 1060,
	kPSIR_IPTC_Digest = 1061
};

struct IPTC_DataSet {	// The 5 byte header of an IPTC DataSet.
	XMP_Uns8 tagMarker;
	XMP_Uns8 recordNumber;
	XMP_Uns8 dataSetNumber;
	XMP_Uns8 octetCountHigh;
	XMP_Uns8 octetCountLow;
};

enum {	// IPTC DataSet IDs
	kIPTC_IntellectualGenre = 4,
	kIPTC_Title = 5,
	kIPTC_Urgency = 10,
	kIPTC_SubjectCode = 12,
	kIPTC_Category = 15,
	kIPTC_SuppCategory = 20,
	kIPTC_Keyword = 25,
	kIPTC_Instructions = 40,
	kIPTC_DateCreated = 55,
	kIPTC_TimeCreated = 60,
	kIPTC_DigitalCreationDate = 62,
	kIPTC_DigitalCreationTime = 63,
	kIPTC_Creator = 80,
	kIPTC_CreatorJobtitle = 85,
	kIPTC_City = 90,
	kIPTC_Location = 92,
	kIPTC_State = 95,
	kIPTC_CountryCode = 100,
	kIPTC_Country = 101,
	kIPTC_JobID = 103,
	kIPTC_Headline = 105,
	kIPTC_Provider = 110,
	kIPTC_Source = 115,
	kIPTC_CopyrightNotice = 116,
	kIPTC_Description = 120,
	kIPTC_DescriptionWriter = 122
};

struct DataSetInfo {
	XMP_Uns8 id;
	const char * name;
};

static const DataSetInfo kDataSetNames[] =
{ { kIPTC_IntellectualGenre, "Intellectual Genre" },
{ kIPTC_Title, "Title" },
{ kIPTC_Urgency, "Urgency" },
{ kIPTC_SubjectCode, "Subject Code" },
{ kIPTC_Category, "Category" },
{ kIPTC_SuppCategory, "Supplemental Category" },
{ kIPTC_Keyword, "Keyword" },
{ kIPTC_Instructions, "Instructions" },
{ kIPTC_DateCreated, "Date Created" },
{ kIPTC_TimeCreated, "Time Created" },
{ kIPTC_DigitalCreationDate, "Digital Creation Date" },
{ kIPTC_DigitalCreationTime, "Digital Creation Time" },
{ kIPTC_Creator, "Creator" },
{ kIPTC_CreatorJobtitle, "Creator Jobtitle" },
{ kIPTC_City, "City" },
{ kIPTC_Location, "Location" },
{ kIPTC_State, "Province-State" },
{ kIPTC_CountryCode, "Country Code" },
{ kIPTC_Country, "Country" },
{ kIPTC_JobID, "Job ID" },
{ kIPTC_Headline, "Headline" },
{ kIPTC_Provider, "Provider" },
{ kIPTC_Source, "Source" },
{ kIPTC_CopyrightNotice, "Copyright Notice" },
{ kIPTC_Description, "Description" },
{ kIPTC_DescriptionWriter, "Description Writer" },
{ 0, 0 } };

enum {
	kTIFF_Uns8 = 1,
	kTIFF_ASCII = 2,
	kTIFF_Uns16 = 3,
	kTIFF_Uns32 = 4,
	kTIFF_URational = 5,
	kTIFF_Int8 = 6,
	kTIFF_Undef8 = 7,
	kTIFF_Int16 = 8,
	kTIFF_Int32 = 9,
	kTIFF_SRational = 10,
	kTIFF_Float = 11,
	kTIFF_Double = 12,
	kTIFF_IFD = 13,
	kTIFF_TypeEnd = kTIFF_IFD
};

static const int sTIFF_TypeSizes[] = { 0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8, 4 };
static const char * sTIFF_TypeNames[] = { "", "BYTE", "ASCII", "SHORT", "LONG", "RATIONAL",
"SBYTE", "UNDEFINED", "SSHORT", "SLONG", "SRATIONAL",
"FLOAT", "DOUBLE" };

struct TagNameInfo {
	long	tag;
	const char * name;
};

static const TagNameInfo sTIFF_TagNames[] =
{ { 256, "ImageWidth" },
{ 257, "ImageLength" },
{ 258, "BitsPerSample" },
{ 259, "Compression" },
{ 262, "PhotometricInterpretation" },
{ 270, "ImageDescription" },
{ 271, "Make" },
{ 272, "Model" },
{ 274, "Orientation" },
{ 282, "XResolution" },
{ 283, "YResolution" },
{ 284, "PlanarConfiguration" },
{ 296, "ResolutionUnit" },
{ 301, "TransferFunction" },
{ 305, "Software" },
{ 306, "DateTime" },
{ 315, "Artist" },
{ 318, "WhitePoint" },
{ 319, "PrimaryChromaticities" },
{ 529, "YCbCrCoefficients" },
{ 530, "YCbCrSubSampling" },
{ 531, "YCbCrPositioning" },
{ 532, "ReferenceBlackWhite" },
{ 33432, "Copyright" },
{ 33434, "ExposureTime" },
{ 33437, "FNumber" },
{ 34850, "ExposureProgram" },
{ 34852, "SpectralSensitivity" },
{ 34855, "ISOSpeedRatings" },
{ 34856, "OECF" },
{ 36864, "ExifVersion" },
{ 36867, "DateTimeOriginal" },
{ 36868, "DateTimeDigitized" },
{ 37121, "ComponentsConfiguration" },
{ 37122, "CompressedBitsPerPixel" },
{ 37377, "ShutterSpeedValue" },
{ 37378, "ApertureValue" },
{ 37379, "BrightnessValue" },
{ 37380, "ExposureBiasValue" },
{ 37381, "MaxApertureValue" },
{ 37382, "SubjectDistance" },
{ 37383, "MeteringMode" },
{ 37384, "LightSource" },
{ 37385, "Flash" },
{ 37386, "FocalLength" },
{ 37396, "SubjectArea" },
{ 37500, "MakerNote" },
{ 37510, "UserComment" },
{ 37520, "SubSecTime" },
{ 37521, "SubSecTimeOriginal" },
{ 37522, "SubSecTimeDigitized" },
{ 40960, "FlashpixVersion" },
{ 40961, "ColorSpace" },
{ 40962, "PixelXDimension" },
{ 40963, "PixelYDimension" },
{ 40964, "RelatedSoundFile" },
{ 41483, "FlashEnergy" },
{ 41484, "SpatialFrequencyResponse" },
{ 41486, "FocalPlaneXResolution" },
{ 41487, "FocalPlaneYResolution" },
{ 41488, "FocalPlaneResolutionUnit" },
{ 41492, "SubjectLocation" },
{ 41493, "ExposureIndex" },
{ 41495, "SensingMethod" },
{ 41728, "FileSource" },
{ 41729, "SceneType" },
{ 41730, "CFAPattern" },
{ 41985, "CustomRendered" },
{ 41986, "ExposureMode" },
{ 41987, "WhiteBalance" },
{ 41988, "DigitalZoomRatio" },
{ 41989, "FocalLengthIn35mmFilm" },
{ 41990, "SceneCaptureType" },
{ 41991, "GainControl" },
{ 41992, "Contrast" },
{ 41993, "Saturation" },
{ 41994, "Sharpness" },
{ 41995, "DeviceSettingDescription" },
{ 41996, "SubjectDistanceRange" },
{ 42016, "ImageUniqueID" },
{ 50706, "DNGVersion" },
{ 50707, "DNGBackwardVersion" },
{ 50708, "DNG UniqueCameraModel" },
{ 0, "" } };

// =================================================================================================

struct ASF_GUID {

	XMP_Uns32 part1;	// Written little endian.
	XMP_Uns16 part2;	// Written little endian.
	XMP_Uns16 part3;	// Written little endian.
	XMP_Uns16 part4;	// Written big endian.
	XMP_Uns8  part5[6];	// Written in order.

	ASF_GUID() {};
	ASF_GUID(XMP_Uns32 p1, XMP_Uns16 p2, XMP_Uns16 p3, XMP_Uns16 p4, const void* p5)
	{
		part1 = GetUns32LE(&p1);
		part2 = GetUns16LE(&p2);
		part3 = GetUns16LE(&p3);
		part4 = GetUns16BE(&p4);
		memcpy(&part5, p5, 6);
	};

};

enum {	// Objects for which we have special knowledge.
	kASFObj_Unknown = 0,
	kASFObj_Header,			// Special top level objects.
	kASFObj_Data,
	kASFObj_XMP,
	kASFObj_FileProperties,	// Children of the Header Object.
	kASFObj_ContentDesc,
	kASFObj_ContentBrand,
	kASFObj_ContentEncrypt,
	kASFObj_HeaderExtension,
	kASFObj_Padding

};

static const ASF_GUID kASF_HeaderGUID(0x75B22630, 0x668E, 0x11CF, 0xA6D9, "\x00\xAA\x00\x62\xCE\x6C");

struct ASF_ObjectInfo {
	ASF_GUID     guid;
	const char * name;
	XMP_Uns8     kind;
};

static const ASF_ObjectInfo kASF_KnownObjects[] =
{
	{ ASF_GUID(0x75B22630, 0x668E, 0x11CF, 0xA6D9, "\x00\xAA\x00\x62\xCE\x6C"), "Header", kASFObj_Header },
{ ASF_GUID(0x75B22636, 0x668E, 0x11CF, 0xA6D9, "\x00\xAA\x00\x62\xCE\x6C"), "Data", kASFObj_Data },
{ ASF_GUID(0xBE7ACFCB, 0x97A9, 0x42E8, 0x9C71, "\x99\x94\x91\xE3\xAF\xAC"), "XMP", kASFObj_XMP },

{ ASF_GUID(0x33000890, 0xE5B1, 0x11CF, 0x89F4, "\x00\xA0\xC9\x03\x49\xCB"), "Simple_Index", 0 },
{ ASF_GUID(0xD6E229D3, 0x35DA, 0x11D1, 0x9034, "\x00\xA0\xC9\x03\x49\xBE"), "Index", 0 },
{ ASF_GUID(0xFEB103F8, 0x12AD, 0x4C64, 0x840F, "\x2A\x1D\x2F\x7A\xD4\x8C"), "Media_Object_Index", 0 },
{ ASF_GUID(0x3CB73FD0, 0x0C4A, 0x4803, 0x953D, "\xED\xF7\xB6\x22\x8F\x0C"), "Timecode_Index", 0 },

{ ASF_GUID(0x8CABDCA1, 0xA947, 0x11CF, 0x8EE4, "\x00\xC0\x0C\x20\x53\x65"), "File_Properties", kASFObj_FileProperties },
{ ASF_GUID(0xB7DC0791, 0xA9B7, 0x11CF, 0x8EE6, "\x00\xC0\x0C\x20\x53\x65"), "Stream_Properties", 0 },
{ ASF_GUID(0x5FBF03B5, 0xA92E, 0x11CF, 0x8EE3, "\x00\xC0\x0C\x20\x53\x65"), "Header_Extension", kASFObj_HeaderExtension },
{ ASF_GUID(0x86D15240, 0x311D, 0x11D0, 0xA3A4, "\x00\xA0\xC9\x03\x48\xF6"), "Codec_List", 0 },
{ ASF_GUID(0x1EFB1A30, 0x0B62, 0x11D0, 0xA39B, "\x00\xA0\xC9\x03\x48\xF6"), "Script_Command", 0 },
{ ASF_GUID(0xF487CD01, 0xA951, 0x11CF, 0x8EE6, "\x00\xC0\x0C\x20\x53\x65"), "Marker", 0 },
{ ASF_GUID(0xD6E229DC, 0x35DA, 0x11D1, 0x9034, "\x00\xA0\xC9\x03\x49\xBE"), "Bitrate_Mutual_Exclusion", 0 },
{ ASF_GUID(0x75B22635, 0x668E, 0x11CF, 0xA6D9, "\x00\xAA\x00\x62\xCE\x6C"), "Error_Correction", 0 },
{ ASF_GUID(0x75B22633, 0x668E, 0x11CF, 0xA6D9, "\x00\xAA\x00\x62\xCE\x6C"), "Content_Description", kASFObj_ContentDesc },
{ ASF_GUID(0xD2D0A440, 0xE307, 0x11D2, 0x97F0, "\x00\xA0\xC9\x5E\xA8\x50"), "Extended_Content_Description", 0 },
{ ASF_GUID(0x2211B3FA, 0xBD23, 0x11D2, 0xB4B7, "\x00\xA0\xC9\x55\xFC\x6E"), "Content_Branding", kASFObj_ContentBrand },
{ ASF_GUID(0x7BF875CE, 0x468D, 0x11D1, 0x8D82, "\x00\x60\x97\xC9\xA2\xB2"), "Stream_Bitrate_Properties", 0 },
{ ASF_GUID(0x2211B3FB, 0xBD23, 0x11D2, 0xB4B7, "\x00\xA0\xC9\x55\xFC\x6E"), "Content_Encryption", kASFObj_ContentEncrypt },
{ ASF_GUID(0x298AE614, 0x2622, 0x4C17, 0xB935, "\xDA\xE0\x7E\xE9\x28\x9C"), "Extended_Content_Encryption", 0 },
{ ASF_GUID(0x2211B3FC, 0xBD23, 0x11D2, 0xB4B7, "\x00\xA0\xC9\x55\xFC\x6E"), "Digital_Signature", 0 },
{ ASF_GUID(0x1806D474, 0xCADF, 0x4509, 0xA4BA, "\x9A\xAB\xCB\x96\xAA\xE8"), "Padding", kASFObj_Padding },

{ ASF_GUID(0x14E6A5CB, 0xC672, 0x4332, 0x8399, "\xA9\x69\x52\x06\x5B\x5A"), "Extended_Stream_Properties", 0 },
{ ASF_GUID(0xA08649CF, 0x4775, 0x4670, 0x8A16, "\x6E\x35\x35\x75\x66\xCD"), "Advanced_Mutual_Exclusion", 0 },
{ ASF_GUID(0xD1465A40, 0x5A79, 0x4338, 0xB71B, "\xE3\x6B\x8F\xD6\xC2\x49"), "Group_Mutual_Exclusion", 0 },
{ ASF_GUID(0xD4FED15B, 0x88D3, 0x454F, 0x81F0, "\xED\x5C\x45\x99\x9E\x24"), "Stream_Prioritization", 0 },
{ ASF_GUID(0xA69609E6, 0x517B, 0x11D2, 0xB6AF, "\x00\xC0\x4F\xD9\x08\xE9"), "Bandwidth_Sharing", 0 },
{ ASF_GUID(0x7C4346A9, 0xEFE0, 0x4BFC, 0xB229, "\x39\x3E\xDE\x41\x5C\x85"), "Language_List", 0 },
{ ASF_GUID(0xC5F8CBEA, 0x5BAF, 0x4877, 0x8467, "\xAA\x8C\x44\xFA\x4C\xCA"), "Metadata", 0 },
{ ASF_GUID(0x44231C94, 0x9498, 0x49D1, 0xA141, "\x1D\x13\x4E\x45\x70\x54"), "Metadata_Library", 0 },
{ ASF_GUID(0xD6E229DF, 0x35DA, 0x11D1, 0x9034, "\x00\xA0\xC9\x03\x49\xBE"), "Index_Parameters", 0 },
{ ASF_GUID(0x6B203BAD, 0x3F11, 0x48E4, 0xACA8, "\xD7\x61\x3D\xE2\xCF\xA7"), "Media_Object_Index_Parameters", 0 },
{ ASF_GUID(0xF55E496D, 0x9797, 0x4B5D, 0x8C8B, "\x60\x4D\xFE\x9B\xFB\x24"), "Timecode_Index_Parameters", 0 },
{ ASF_GUID(0x75B22630, 0x668E, 0x11CF, 0xA6D9, "\x00\xAA\x00\x62\xCE\x6C"), "Compatibility", 0 },
{ ASF_GUID(0x43058533, 0x6981, 0x49E6, 0x9B74, "\xAD\x12\xCB\x86\xD5\x8C"), "Advanced_Content_Encryption", 0 },

{ ASF_GUID(0x00000000, 0x0000, 0x0000, 0x0000, "\x00\x00\x00\x00\x00\x00"), 0, 0 }
};

struct ASF_ObjHeader {
	ASF_GUID  guid;
	XMP_Int64 size;
};

struct ASF_FileProperties {
	ASF_GUID  guid;
	XMP_Int64 size;
	ASF_GUID  fileID;
	XMP_Int64 fileSize;
	XMP_Int64 creationDate;	// Number of 100-nanosecond intervals since January 1, 1601.
	XMP_Int64 dataPacketsCount;
	XMP_Int64 playDuration;
	XMP_Int64 sendDuration;
	XMP_Int64 preroll;
	XMP_Uns32 flags;	// The Broadcast flag is bit 0 (lsb).
	XMP_Uns32 minDataPacketSize;
	XMP_Uns32 maxDataPacketSize;
	XMP_Uns32 maxBitrate;
};
#define kASF_FilePropertiesSize	(16 + 8 + 16 + 6*8 + 4*4)

struct ASF_ContentDescription {
	ASF_GUID  guid;
	XMP_Int64 size;
	XMP_Uns16 titleLen;
	XMP_Uns16 authorLen;
	XMP_Uns16 copyrightLen;
	XMP_Uns16 descriptionLen;
	XMP_Uns16 ratingLen;
	// Little endian UTF-16 strings follow, no BOM, possible nul terminator.
};
#define kASF_ContentDescriptionSize	(16 + 8 + 5*2)

#if 0	// ! Has embedded variable length fields!
struct ASF_ContentBranding {
	ASF_GUID  guid;
	XMP_Int64 size;
	XMP_Uns32 bannerType;
	XMP_Uns32 bannerDataSize;
	// The banner data is here.
	XMP_Uns32 bannerURLSize;
	// The banner URL string is here, an ASCII string.
	XMP_Uns32 copyrightURLSize;
	// The copyright URL string is here, an ASCII string.
};
#endif

#if 0	// ! Has embedded variable length fields!
struct ASF_ContentEncryption {
	ASF_GUID  guid;
	XMP_Int64 size;
	XMP_Uns32 secretDataSize;
	// The secret data is here.
	XMP_Uns32 protectionTypeSize;
	// The protection type is here, an ASCII string.
	XMP_Uns32 keyIDSize;
	// The key ID is here, an ASCII string.
	XMP_Uns32 licenseURLSize;
	// The licensed URL is here, an ASCII string.
};
#endif

struct ASF_HeaderExtension {
	ASF_GUID  guid;
	XMP_Int64 size;
	ASF_GUID  reserved1;
	XMP_Uns16 reserved2;
	XMP_Uns32 dataLen;
	// The header extension data is a sequence of nested objects.
};
#define kASF_HeaderExtensionSize (16 + 8 + 16 + 2 + 4)

// =================================================================================================

enum {
	kINDD_PageSize = 4096,
	kINDD_LittleEndian = 1,
	kINDD_BigEndian = 2,
	kInDesignGUIDSize = 16
};

struct InDesignMasterPage {
	XMP_Uns8  fGUID[kInDesignGUIDSize];
	XMP_Uns8  fMagicBytes[8];
	XMP_Uns8  fObjectStreamEndian;
	XMP_Uns8  fIrrelevant1[239];
	XMP_Uns64 fSequenceNumber;
	XMP_Uns8  fIrrelevant2[8];
	XMP_Uns32 fFilePages;
	XMP_Uns8  fIrrelevant3[3812];
};

static const XMP_Uns8 kInDesign_MasterPageGUID[kInDesignGUIDSize] =
{ 0x06, 0x06, 0xED, 0xF5, 0xD8, 0x1D, 0x46, 0xE5, 0xBD, 0x31, 0xEF, 0xE7, 0xFE, 0x74, 0xB7, 0x1D };

struct InDesignContigObjMarker {
	XMP_Uns8  fGUID[kInDesignGUIDSize];
	XMP_Uns32 fObjectUID;
	XMP_Uns32 fObjectClassID;
	XMP_Uns32 fStreamLength;
	XMP_Uns32 fChecksum;
};

static const XMP_Uns8 kINDDContigObjHeaderGUID[kInDesignGUIDSize] =
{ 0xDE, 0x39, 0x39, 0x79, 0x51, 0x88, 0x4B, 0x6C, 0x8E, 0x63, 0xEE, 0xF8, 0xAE, 0xE0, 0xDD, 0x38 };

// =================================================================================================

struct FileExtMapping {
	XMP_StringPtr  ext;
	XMP_FileFormat format;
};

const FileExtMapping kFileExtMap[] =	// Add all known mappings, multiple mappings (tif, tiff) are OK.
{ { "pdf",  kXMP_PDFFile },
{ "ps",   kXMP_PostScriptFile },
{ "eps",  kXMP_EPSFile },

{ "jpg",  kXMP_JPEGFile },
{ "jpeg", kXMP_JPEGFile },
{ "jpx",  kXMP_JPEG2KFile },
{ "tif",  kXMP_TIFFFile },
{ "tiff", kXMP_TIFFFile },
{ "gif",  kXMP_GIFFile },
{ "giff", kXMP_GIFFile },
{ "png",  kXMP_PNGFile },

{ "swf",  kXMP_SWFFile },
{ "flv",  kXMP_FLVFile },

{ "aif",  kXMP_AIFFFile },

{ "mov",  kXMP_MOVFile },
{ "avi",  kXMP_AVIFile },
{ "cin",  kXMP_CINFile },
{ "wav",  kXMP_WAVFile },
{ "mp3",  kXMP_MP3File },
{ "mp4",  kXMP_MPEG4File },
{ "ses",  kXMP_SESFile },
{ "cel",  kXMP_CELFile },
{ "wma",  kXMP_WMAVFile },
{ "wmv",  kXMP_WMAVFile },

{ "mpg",  kXMP_MPEGFile },
{ "mpeg", kXMP_MPEGFile },
{ "mp2",  kXMP_MPEGFile },
{ "mod",  kXMP_MPEGFile },
{ "m2v",  kXMP_MPEGFile },
{ "mpa",  kXMP_MPEGFile },
{ "mpv",  kXMP_MPEGFile },
{ "m2p",  kXMP_MPEGFile },
{ "m2a",  kXMP_MPEGFile },
{ "m2t",  kXMP_MPEGFile },
{ "mpe",  kXMP_MPEGFile },
{ "vob",  kXMP_MPEGFile },
{ "ms-pvr", kXMP_MPEGFile },
{ "dvr-ms", kXMP_MPEGFile },

{ "html", kXMP_HTMLFile },
{ "xml",  kXMP_XMLFile },
{ "txt",  kXMP_TextFile },
{ "text", kXMP_TextFile },

{ "psd",  kXMP_PhotoshopFile },
{ "ai",   kXMP_IllustratorFile },
{ "indd", kXMP_InDesignFile },
{ "indt", kXMP_InDesignFile },
{ "aep",  kXMP_AEProjectFile },
{ "aet",  kXMP_AEProjTemplateFile },
{ "ffx",  kXMP_AEFilterPresetFile },
{ "ncor", kXMP_EncoreProjectFile },
{ "prproj", kXMP_PremiereProjectFile },
{ "prtl", kXMP_PremiereTitleFile },

{ 0, kXMP_UnknownFile } };	// ! Must be last as a sentinel.

							// Vector of keys in quicktime file in the order in which they appear
vector <string> ISOMetaKeys;

XMP_Int32 entryCount_dref;
XMP_Uns16 exif_item_id;
XMP_Uns16 mime_item_id;
std::string item_name;
XMP_Uns32 item_type;
std::string content_type;
std::string url_location;
std::string content_encoding;


// File convenience wrappers (now LFA-based) ====================================

// skip forward by <size> bytes and verify not beyond EOF
void static Skip(LFA_FileRef file, XMP_Int64 size)
{
	// assert no more, since LFA_Seek does not return 0 to say o.k., but actual filePos
	// OLD	assertMsg("unexpected end of file", 0 == LFA_Seek (file, size, SEEK_CUR) );
	LFA_Seek(file, size, SEEK_CUR);
}

// going back in the file (use positive values!)
// (yes redundant to above but "makes a better read")
void static Rewind(LFA_FileRef file, XMP_Int64 size)
{
	assertMsg("use positive values", size > 0);
	LFA_Seek(file, -size, SEEK_CUR); // ditto to above
}

// overload, no size parameter, rewinds to start
void static Rewind(LFA_FileRef file)
{
	LFA_Seek(file, 0, SEEK_SET);
}

XMP_Uns32 static Peek32u(LFA_FileRef file, bool bigEndian = false)
{
	XMP_Uns32 value = tree->digest32u(file, "", bigEndian);
	Rewind(file, 4);
	return value;
}


// =================================================================================================

static XMP_FileFormat
LookupFileExtMapping(const char * filePath)
{
	std::string fileExt;
	size_t extPos = strlen(filePath);
	for (--extPos; extPos > 0; --extPos) if (filePath[extPos] == '.') break;

	if (filePath[extPos] != '.') return kXMP_UnknownFile;

	++extPos;
	fileExt.assign(&filePath[extPos]);
	for (size_t i = 0; i < fileExt.size(); ++i) {
		if (('A' <= fileExt[i]) && (fileExt[i] <= 'Z')) fileExt[i] += 0x20;
	}

	size_t mapPos;
	for (mapPos = 0; kFileExtMap[mapPos].ext != 0; ++mapPos) {
		if (fileExt == kFileExtMap[mapPos].ext) break;
	}

	return kFileExtMap[mapPos].format;

}	// LookupFileExtMapping

	// =================================================================================================

	//*** used by in-RAM code? needs replacement?
static void
CaptureFileData(LFA_FileRef file, XMP_Int64 offset, XMP_Uns32 length)
{

	if (length > sDataMax) {
		if (sDataPtr != 0) free(sDataPtr);
		sDataPtr = (XMP_Uns8*)malloc(length);
		sDataMax = length;
	}

	if (offset != 0) LFA_Seek(file, (long)offset, SEEK_SET);
	LFA_Read(file, sDataPtr, length, true);
	sDataLen = length;

}	// CaptureFileData

	// -------------------------------------------------------------------------------------------------

	//*** used by in-RAM code? needs replacement !!!
static void
CaptureXMPF(LFA_FileRef file, XMP_Int64 offset, XMP_Uns32 length)
{

	if (length > sXMPMax) {
		if (sXMPPtr != 0) free(sXMPPtr);
		sXMPPtr = (XMP_Uns8*)malloc(length);
		sXMPMax = length;
	}

	if (offset != 0) LFA_Seek(file, (long)offset, SEEK_SET);
	LFA_Read(file, sXMPPtr, length, true);
	sXMPLen = length;
	sXMPPos = offset;

}	// CaptureXMPF

	// -------------------------------------------------------------------------------------------------

static void
CaptureXMP(const XMP_Uns8 * xmpPtr, const XMP_Uns32 xmpLen, XMP_Int64 fileOffset)
{

	if (xmpLen > sXMPMax) {
		if (sXMPPtr != 0) free(sXMPPtr);
		sXMPPtr = (XMP_Uns8*)malloc(xmpLen);
		sXMPMax = xmpLen;
	}

	memcpy(sXMPPtr, xmpPtr, xmpLen);
	sXMPLen = xmpLen;
	sXMPPos = fileOffset;

}	// CaptureXMP

	// -------------------------------------------------------------------------------------------------

static void PrintOnlyASCII_8(XMP_Uns8 * strPtr, XMP_Uns32 strLen, bool stopOnNUL = true)
{
	//wrapping to QEBuginese
	// - NB: remainder (zero termination earlier then length) is catered for...

	tree->addComment(convert8Bit(strPtr, stopOnNUL, strLen));
}

// -------------------------------------------------------------------------------------------------

// this wrap and the LE counterpart can only be inferior, since
// its always added as a comment, even if value was more appropriate.
// ==> callers should make use of convert16Bit directly.
static void PrintOnlyASCII_16BE(XMP_Uns16 * u16Ptr, XMP_Uns32 u16Bytes, bool stopOnNUL = true)
{
	tree->addComment(convert16Bit(true, (XMP_Uns8*)u16Ptr, stopOnNUL, u16Bytes));
}	// PrintOnlyASCII_16BE

	// -------------------------------------------------------------------------------------------------

static void PrintOnlyASCII_16LE(XMP_Uns16 * u16Ptr, XMP_Uns32 u16Bytes, bool stopOnNUL = true)
{
	tree->addComment(convert16Bit(false, (XMP_Uns8*)u16Ptr, stopOnNUL, u16Bytes));
}	// PrintOnlyASCII_16LE

	// =================================================================================================

static const XMP_Int64 kJPEGMinSize = 4;	// At least the SOI and EOI markers.
static const XMP_Uns8  kJPEGStart[] = { 0xFF, 0xD8, 0xFF };	// 0xFFD8 is SOI, plus 0xFF for next marker.

static const XMP_Int64 kPhotoshopMinSize = 26 + 4 * 4;	// At least the file header and 4 section lengths.
static const XMP_Uns8  kPhotoshopV1Start[] = { 0x38, 0x42, 0x50, 0x53, 0x00, 0x01 };	// 0x38425053 is "8BPS".
static const XMP_Uns8  kPhotoshopV2Start[] = { 0x38, 0x42, 0x50, 0x53, 0x00, 0x02 };

static const XMP_Int64 kTIFFMinSize = 8 + 2 + 12 + 4;	// At least the header plus 1 minimal IFD.
static const XMP_Uns8  kTIFFBigStart[] = { 0x4D, 0x4D, 0x00, 0x2A };	// 0x4D is 'M', 0x2A is 42.
static const XMP_Uns8  kTIFFLittleStart[] = { 0x49, 0x49, 0x2A, 0x00 };	// 0x49 is 'I'.

static const XMP_Int64 kJPEG2KMinSize = 12 + 16;	// At least the signature and minimal file type boxes.
static const XMP_Uns8  kJPEG2KStart[] = { 0x00, 0x00, 0x00, 0x0C, 0x6A, 0x50, 0x20, 0x20, 0x0D, 0x0A, 0x87, 0x0A };

static const XMP_Int64 kPNGMinSize = 8 + (12 + 13) + 12;	// At least the file header plus IHDR and IEND chunks.
static const XMP_Uns8  kPNGStart[] = { 137, 80, 78, 71, 13, 10, 26, 10 };

static const XMP_Int64 kASFMinSize = 16;	// ! Not really accurate, but covers the header GUID.

static const XMP_Int64 kRIFFMinSize = 12;

static const XMP_Int64 kPostScriptMinSize = 49;


static const XMP_Int64 kInDesignMinSize = 2 * kINDD_PageSize;	// Two master pages.

static const XMP_Int64 kISOMediaMinSize = 16;	// At least a minimal file type box.
static const XMP_Uns8  kISOMediaFTyp[] = { 0x66, 0x74, 0x79, 0x70 };	// "ftyp"
static const XMP_Uns32 kISOTag_ftyp = 0x66747970UL;
static const XMP_Uns32 kISOBrand_mp41 = 0x6D703431UL;
static const XMP_Uns32 kISOBrand_mp42 = 0x6D703432UL;
static const XMP_Uns32 kISOBrand_avc1 = 0x61766331UL;
static const XMP_Uns32 kISOBrand_f4v = 0x66347620UL;
static const XMP_Uns32 kISOBrand_isom = 0x69736F6DUL;
static const XMP_Uns32 kISOBrand_3gp4 = 0x33677034UL;
static const XMP_Uns32 kISOBrand_3g2a = 0x33673261UL;
static const XMP_Uns32 kISOBrand_3g2b = 0x33673262UL;
static const XMP_Uns32 kISOBrand_3g2c = 0x33673263UL;
static const XMP_Uns32 kISOBrand_mif1 = 0x6D696631UL;


static const XMP_Uns32 kQTTag_XMP_ = 0x584D505FUL;

static const XMP_Int64 kSWFMinSize = (8 + 2 + 4 + 2);	// Header with minimal rectangle and an End tag.

static const XMP_Int64 kFLVMinSize = 9;	// Header with zero length data.

static const XMP_Uns8  kPostScriptStart[] = { 0xC5, 0xD0, 0xD3, 0xC6 };

static XMP_FileFormat
CheckFileFormat(const char * filePath, XMP_Uns8 * fileContent, XMP_Int64 fileSize)
{
	// ! The buffer passed to CheckFileFormat is just the first 4K bytes of the file.

	if ((fileSize >= kJPEGMinSize) && CheckBytes(fileContent, kJPEGStart, 3)) {
		return kXMP_JPEGFile;
	}

	if ((fileSize >= kPhotoshopMinSize) &&
		(CheckBytes(fileContent, kPhotoshopV1Start, 6) || CheckBytes(fileContent, kPhotoshopV2Start, 6))) {
		return kXMP_PhotoshopFile;
	}

	if ((fileSize >= kTIFFMinSize) &&
		(CheckBytes(fileContent, kTIFFBigStart, 4) || CheckBytes(fileContent, kTIFFLittleStart, 4))) {
		return kXMP_TIFFFile;
	}

	if ((fileSize >= kJPEG2KMinSize) && CheckBytes(fileContent, kJPEG2KStart, 12)) {
		return kXMP_JPEG2KFile;
	}

	if ((fileSize >= kASFMinSize) && CheckBytes(fileContent, &kASF_HeaderGUID, 16)) {
		return kXMP_WMAVFile;
	}

	if ((fileSize >= kPNGMinSize) && CheckBytes(fileContent, kPNGStart, 8)) {
		return kXMP_PNGFile;
	}

	if ((fileSize >= kRIFFMinSize) && CheckBytes(fileContent, "RIFF", 4)) {
		if (CheckBytes(fileContent + 8, "AVI ", 4)) return kXMP_AVIFile;
		if (CheckBytes(fileContent + 8, "WAVE", 4)) return kXMP_WAVFile;
	}

	if ((fileSize >= kRIFFMinSize) && CheckBytes(fileContent, "RF64", 4)) {
		if (CheckBytes(fileContent + 8, "WAVE", 4)) return kXMP_WAVFile;
	}

	if ((fileSize >= kRIFFMinSize) && CheckBytes(fileContent, "FORM", 4)) {
		if (CheckBytes(fileContent + 8, "AIFF ", 4)) return kXMP_AIFFFile;
		if (CheckBytes(fileContent + 8, "AIFC", 4)) return kXMP_AIFFFile;
	}

	if ((fileSize >= kPostScriptMinSize) && CheckBytes(fileContent, kPostScriptStart, 4)) {
		return kXMP_PostScriptFile;
	}

	if ((fileSize >= kInDesignMinSize) && CheckBytes(fileContent, kInDesign_MasterPageGUID, kInDesignGUIDSize)) {
		return kXMP_InDesignFile;
	}

	if ((fileSize >= kSWFMinSize) &&
		(CheckBytes(fileContent, "FWS", 3) || CheckBytes(fileContent, "CWS", 3)) &&
		(fileContent[3] <= 10 /* 2007 latest is 8 */)) {
		return kXMP_SWFFile;
	}

	if ((fileSize >= kFLVMinSize) &&
		CheckBytes(fileContent, "FLV", 3) &&
		(fileContent[3] <= 10 /* 2007 latest is 1 */)) {
		return kXMP_FLVFile;
	}

	if ((fileSize >= kISOMediaMinSize) && CheckBytes(fileContent + 4, kISOMediaFTyp, 4)) {

		XMP_Uns32 ftypLen = GetUns32BE(fileContent);
		if (ftypLen == 0) ftypLen = fileSize;
		if ((ftypLen < kISOMediaMinSize) || (ftypLen > fileSize) || (ftypLen > 4096)) return kXMP_UnknownFile;

		XMP_Uns8 * compatPtr = fileContent + 16;
		XMP_Uns8 * compatEnd = fileContent + ftypLen;

		for (; compatPtr < compatEnd; compatPtr += 4) {
			XMP_Uns32 compatBrand = GetUns32BE(compatPtr);
			switch (compatBrand) {
			case kISOBrand_mp41:
			case kISOBrand_mp42:
			case kISOBrand_avc1:
			case kISOBrand_isom:
			case kISOBrand_3gp4:
			case kISOBrand_3g2a:
			case kISOBrand_3g2b:
			case kISOBrand_3g2c:
				return kXMP_MPEG4File;
				break;
			case kISOBrand_mif1:
				return kXMP_HEIFFile;
				break;
			default:
				break;

			}

		}

	}

	if ((fileSize > 30) && CheckBytes(fileContent, "\x50\x4B\x03\x04", 4)) {  // "PK 03 04"
		return kXMP_UCFFile;
	}

	// ! Do MP3 next to last. It uses the file extension if there is no ID3.
	if (CheckBytes(fileContent, "ID3", 3) ||
		(LookupFileExtMapping(filePath) == kXMP_MP3File)) return kXMP_MP3File;

	// ! Do MPEG (MP2) and MOV last. They use just the file extension, not content.
	if (LookupFileExtMapping(filePath) == kXMP_MPEGFile) return kXMP_MPEGFile;
	if (LookupFileExtMapping(filePath) == kXMP_MOVFile) return kXMP_MOVFile;

	std::string fileData = (char*)fileContent;
	if ((fileSize > 30) && (int)fileData.find("<svg") >= 0) {
		return kXMP_SVGFile;
	}

	return kXMP_UnknownFile;

}	// CheckFileFormat

	// =================================================================================================
	// DumpXMP
	// =======

static void
DumpXMP(XMP_Uns8 * xmpPtr, XMP_Uns32 xmpLen, XMP_Int64 xmpOffset, const char * label)
{
	if (xmpOffset <= 0xFFFFFFFFL) {
		tree->pushNode("XMP");
		tree->addComment("from %s, offset %u (0x%X), size %d",
			label, (XMP_Uns32)xmpOffset, (XMP_Uns32)xmpOffset, xmpLen);
	}
	else {
		tree->pushNode("XMP");
		tree->addComment("from %s, offset %ll (0x%X-%.8X), size %d",
			label, High32(xmpOffset), Low32(xmpOffset), xmpLen);
	}

	//bool atStart = true;
	SXMPMeta xmp((XMP_StringPtr)xmpPtr, xmpLen);
	xmp.Sort();

	//FNO: could be reactived, but makes the dump naturally very long - harder to read for dev work
	//xmp.DumpObject( (DumpCallback, &atStart);
	tree->popNode();
}

// =================================================================================================
// DumpXMP
// =======

// an (old) wrapper for above function relying on static, "global" variables
static void
DumpXMP(const char * label)
{
	DumpXMP(sXMPPtr, sXMPLen, sXMPPos, label);
}	// DumpXMP

	// =================================================================================================
	// DumpIPTC
	// ========
	//
	// The IPTC (IIM, NAA) values are in a sequence of "data sets". Each has a 5 byte header followed
	// by the value. There is no overall length in the sequence itself. Photoshop writes this in TIFF
	// as LONGs (4 byte chunks), so there might be padding at the end.

static void
DumpIPTC(XMP_Uns8 * iptcOrigin, XMP_Uns32 iptcLen, XMP_Uns32 fileOffset, const char * label)
{
	tree->pushNode("IPTC data");
	tree->addComment("from %s, offset %d (0x%X), size %d",
		label, fileOffset, fileOffset, iptcLen);

	// ** Compute and print the MD5 digest.
	XMP_Uns8 * iptcPtr = iptcOrigin;
	XMP_Uns8 * iptcEnd = iptcPtr + iptcLen;
	XMP_Uns8 * valuePtr;
	XMP_Uns32 valueLen;

	while (iptcPtr < (iptcEnd - 4)) {	// ! The -4 is to skip terminal padding.
		IPTC_DataSet * currDS = (IPTC_DataSet*)iptcPtr;
		if (currDS->tagMarker != 0x1C) {
			tree->comment("** invalid IPTC marker **");
			break;
		}
		valuePtr = iptcPtr + 5;
		valueLen = (currDS->octetCountHigh << 8) + currDS->octetCountLow;

		if ((valueLen >> 15) == 1) {
			int count = valueLen & 0x7FFF;
			valueLen = 0;
			for (int i = 0; i < count; ++i) valueLen = (valueLen << 8) + valuePtr[i];
			valuePtr += count;
		}

		XMP_Uns32 dsOffset = fileOffset + (iptcPtr - iptcOrigin);

		//key come here ===================
		tree->setKeyValue(
			fromArgs("IPTC:%d:%d", currDS->recordNumber, currDS->dataSetNumber), "");
		tree->addComment("offset %d (0x%X), size %d", dsOffset, dsOffset, valueLen);


		if ((currDS->recordNumber != 1) && (currDS->recordNumber != 2)) {

			//LF only 1:** and 2:** bother us

		}
		else if (currDS->recordNumber == 1) {

			switch (currDS->dataSetNumber) {
			case 0:
			{
				XMP_Uns16 version = GetUns16BE(valuePtr);
				tree->addComment("version = 0x%.4X", version);
				break;
			}
			case 90:
				if (valueLen == 3) {
					tree->addComment("encoding = 0x%.2X%.2X%.2X", valuePtr[0], valuePtr[1], valuePtr[2]);
					if (memcmp(valuePtr, "\x1B\x25\x47", 3) == 0) tree->addComment(" (UTF-8)");
				}
				break;
			default:
				break;
			}

		}
		else if (currDS->dataSetNumber == 0) {

			XMP_Uns16 version = GetUns16BE(valuePtr);
			tree->addComment(",Version = 0x%.4X", version);

		}
		else {

			int ds;
			for (ds = 0; kDataSetNames[ds].name != 0; ++ds) {
				if (currDS->dataSetNumber == kDataSetNames[ds].id) break;
			}
			if (kDataSetNames[ds].name == 0) {
				//LF
			}
			else {
				tree->addComment("%s", kDataSetNames[ds].name);
				tree->changeValue(convert8Bit(valuePtr, false, valueLen));
			}

		}

		iptcPtr = valuePtr + valueLen;

	}

	//LF
	if (iptcPtr > iptcEnd) {
		tree->comment("** Too much IPTC data, delta %d", (long)(iptcEnd - iptcPtr));
	}
	else {
		while ((iptcPtr < iptcEnd) && (*iptcPtr == 0)) ++iptcPtr;
		if (iptcPtr != iptcEnd) tree->comment("** Too little IPTC data, delta %d", (long)(iptcPtr - iptcEnd));
	}

	tree->popNode();
}	// DumpIPTC

	// =================================================================================================

static void
DumpImageResources(const JpegMarkers& psirMarkers, XMP_Uns8 * dataStart, const char * label)
{

	XMP_Uns32 i = 0, size = psirMarkers.size();
	std::string combinedPSIRData;
	for (i = 0; i < size; i++) {
		combinedPSIRData.append((const char *)psirMarkers[i].jpegMarkerPtr, psirMarkers[i].jpegMarkerLen);
	}


	XMP_Uns8 * psirPtr = (XMP_Uns8 *)combinedPSIRData.data();
	XMP_Uns8 * psirEnd = psirPtr + combinedPSIRData.size();

	XMP_Uns8 * irPtr;
	XMP_Uns32  irLen, irOffset; //irType replaced by irTypeStr below

	XMP_Uns8 * iptcPtr = 0;
	XMP_Uns8 * xmpPtr = 0;
	XMP_Uns8 * exif1Ptr = 0;
	XMP_Uns8 * exif3Ptr = 0;
	XMP_Uns32  iptcLen, xmpLen, exif1Len, exif3Len;
	XMP_Int32 lastIndexUsed = -1;
	while (psirPtr < psirEnd) {
		// calculate fileOffset and psirOrigin
		size_t currentOffset = (const char *)psirPtr - combinedPSIRData.data();
		XMP_Uns32 length = 0;
		for (i = 0; i < size; i++) {
			length += psirMarkers[i].jpegMarkerLen;
			if (currentOffset <= length)
				break;
		}
		if (lastIndexUsed != (XMP_Int32)i) {
			if (lastIndexUsed != -1)
				tree->popNode();
			// time to push a new node
			tree->pushNode("Photoshop Image Resources %d", i + 1);
			XMP_Uns32 fileOffset = psirMarkers[i].jpegMarkerPtr - dataStart;
			tree->addComment("from %s, offset %d (0x%X), size %d",
				label, fileOffset, fileOffset, psirMarkers[i].jpegMarkerLen);
			lastIndexUsed = i;
		}
		XMP_Uns32 fileOffset = psirMarkers[i].jpegMarkerPtr - dataStart;
		XMP_Uns8 * psirOrigin = psirMarkers[i].jpegMarkerPtr;

		std::string irTypeStr = convert8Bit(psirPtr, false, 4); //get in an endian neutral way
		XMP_Uns16 irID = GetUns16BE(psirPtr + 4);	// The image resource ID.

		const char* irName = (XMP_StringPtr)psirPtr + 6;	// A Pascal string.
		irOffset = 6 + ((*irName + 2) & 0xFFFFFFFE);	// Offset to the image resource data length.
		irLen = GetUns32BE(psirPtr + irOffset);
		irPtr = psirPtr + irOffset + 4;

		irOffset = fileOffset + ((psirPtr - (XMP_Uns8 *)combinedPSIRData.data()) - (length - psirMarkers[i].jpegMarkerLen));

		if (irTypeStr != "8BIM") {
			tree->setKeyValue(fromArgs("PSIR:%s:#%u", irTypeStr.c_str(), irID), "");
			tree->comment("(non-8BIM encountered and tolerated, see bug 1454756)");
		}
		else if (irID == kPSIR_IPTC) {			//****************
			tree->setKeyValue("PSIR:IPTC", "");
			iptcPtr = irPtr;
			iptcLen = irLen;
			if (iptcPtr != 0) {
				XMP_Uns32 offset = fileOffset + ((iptcPtr - (XMP_Uns8 *)combinedPSIRData.data()) - (length - psirMarkers[i].jpegMarkerLen));
				DumpIPTC(iptcPtr, iptcLen, offset, "PSIR #1028");
			}
		}
		else if (irID == kPSIR_XMP) {				//****************
			tree->setKeyValue("PSIR:XMP", "");
			xmpPtr = irPtr;
			xmpLen = irLen;
			if (xmpPtr != 0) {
				XMP_Uns32 offset = fileOffset + ((xmpPtr - (XMP_Uns8 *)combinedPSIRData.data()) - (length - psirMarkers[i].jpegMarkerLen));
				DumpXMP(xmpPtr, xmpLen, offset, "PSIR #1060");
			}
		}
		else if (irID == kPSIR_Exif_1) {			//****************
			tree->setKeyValue("PSIR:Exif-1", "");
			exif1Ptr = irPtr;
			exif1Len = irLen;
			XMP_Uns32 offset = fileOffset + ((exif1Ptr - (XMP_Uns8 *)combinedPSIRData.data()) - (length - psirMarkers[i].jpegMarkerLen));
			DumpTIFF(exif1Ptr, exif1Len, offset, "PSIR #1058 (Exif 1)", "PSIR:Exif-1");
		}
		else if (irID == kPSIR_Exif_3) {			//****************
			tree->setKeyValue("PSIR:Exif-3", "");
			exif3Ptr = irPtr;
			exif3Len = irLen;
			XMP_Uns32 offset = fileOffset + ((exif3Ptr - (XMP_Uns8 *)combinedPSIRData.data()) - (length - psirMarkers[i].jpegMarkerLen));
			if (exif3Ptr != 0) DumpTIFF(exif3Ptr, exif3Len, offset, "PSIR #1059 (Exif 3)", "PSIR:Exif-3");
		}
		else if (irID == kPSIR_IPTC_Digest) {
			tree->setKeyValue("PSIR:IPTC digest",
				fromArgs("%.8X-%.8X-%.8X-%.8X",
					GetUns32BE(irPtr),
					GetUns32BE(irPtr + 4),
					GetUns32BE(irPtr + 8),
					GetUns32BE(irPtr + 12))
			);
		}
		else if (irID == kPSIR_CopyrightFlag) {
			bool copyrighted = (*irPtr != 0);
			tree->setKeyValue("PSIR:copyrighted", (copyrighted ? "yes" : "no"));
		}
		else if (irID == kPSIR_CopyrightURL) {
			tree->setKeyValue("PSIR:copyright URL", convert8Bit(irPtr, true, irLen));
		}
		else if (irID == kPSIR_OldCaption) {
			tree->setKeyValue("PSIR:old caption", convert8Bit(irPtr, true, irLen));
		}
		else if (irID == kPSIR_PrintCaption) {
			tree->comment("** obsolete print caption **");
		}
		else {
			tree->setKeyValue(
				fromArgs("PSIR:%s:#%d", irTypeStr.c_str(), irID),
				""
			);
		}
		if (irOffset + irLen > (psirMarkers[i].jpegMarkerPtr - dataStart) + psirMarkers[i].jpegMarkerLen) {
			//merged from two markers
			tree->addComment("offset %d (0x%X), size %d - split in multiple markers", irOffset, irOffset, irLen);
		}
		else {
			tree->addComment("offset %d (0x%X), size %d", irOffset, irOffset, irLen);
		}
		if (*irName != 0) tree->addComment("\"%.*s\"", (int)(*irName), (irName + 1));
		psirPtr = irPtr + ((irLen + 1) & 0xFFFFFFFE);	// Round the length to be even.
	} //while-loop

	if (psirPtr != psirEnd) {
		tree->addComment("** Unexpected end of image resources, delta %d", (long)(psirPtr - psirEnd));
	}

	//NB: dump routines moved up into if-else's	
	tree->popNode();
}	// DumpImageResources

	// =================================================================================================

static void
DumpImageResources(XMP_Uns8 * psirOrigin, XMP_Uns32 psirLen, XMP_Uns32 fileOffset, const char * label)
{
	tree->pushNode("Photoshop Image Resources");
	tree->addComment("from %s, offset %d (0x%X), size %d",
		label, fileOffset, fileOffset, psirLen);

	XMP_Uns8 * psirPtr = psirOrigin;
	XMP_Uns8 * psirEnd = psirPtr + psirLen;
	XMP_Uns8 * irPtr;
	XMP_Uns32  irLen, irOffset; //irType replaced by irTypeStr below

	XMP_Uns8 * iptcPtr = 0;
	XMP_Uns8 * xmpPtr = 0;
	XMP_Uns8 * exif1Ptr = 0;
	XMP_Uns8 * exif3Ptr = 0;
	XMP_Uns32  iptcLen, xmpLen, exif1Len, exif3Len;

	while (psirPtr < psirEnd) {
		std::string irTypeStr = convert8Bit(psirPtr, false, 4); //get in an endian neutral way
		XMP_Uns16 irID = GetUns16BE(psirPtr + 4);	// The image resource ID.

		const char* irName = (XMP_StringPtr)psirPtr + 6;	// A Pascal string.
		irOffset = 6 + ((*irName + 2) & 0xFFFFFFFE);	// Offset to the image resource data length.
		irLen = GetUns32BE(psirPtr + irOffset);
		irPtr = psirPtr + irOffset + 4;

		irOffset = fileOffset + (psirPtr - psirOrigin);

		if (irTypeStr != "8BIM") {
			tree->setKeyValue(fromArgs("PSIR:%s:#%u", irTypeStr.c_str(), irID), "");
			tree->comment("(non-8BIM encountered and tolerated, see bug 1454756)");
		}
		else if (irID == kPSIR_IPTC) {			//****************
			tree->setKeyValue("PSIR:IPTC", "");
			iptcPtr = irPtr;
			iptcLen = irLen;
			if (iptcPtr != 0) DumpIPTC(iptcPtr, iptcLen, (fileOffset + (iptcPtr - psirOrigin)), "PSIR #1028");
		}
		else if (irID == kPSIR_XMP) {				//****************
			tree->setKeyValue("PSIR:XMP", "");
			xmpPtr = irPtr;
			xmpLen = irLen;
			if (xmpPtr != 0) DumpXMP(xmpPtr, xmpLen, (fileOffset + (xmpPtr - psirOrigin)), "PSIR #1060");
		}
		else if (irID == kPSIR_Exif_1) {			//****************
			tree->setKeyValue("PSIR:Exif-1", "");
			exif1Ptr = irPtr;
			exif1Len = irLen;
			DumpTIFF(exif1Ptr, exif1Len, (fileOffset + (exif1Ptr - psirOrigin)), "PSIR #1058 (Exif 1)", "PSIR:Exif-1");
		}
		else if (irID == kPSIR_Exif_3) {			//****************
			tree->setKeyValue("PSIR:Exif-3", "");
			exif3Ptr = irPtr;
			exif3Len = irLen;
			if (exif3Ptr != 0) DumpTIFF(exif3Ptr, exif3Len, (fileOffset + (exif3Ptr - psirOrigin)), "PSIR #1059 (Exif 3)", "PSIR:Exif-3");
		}
		else if (irID == kPSIR_IPTC_Digest) {
			tree->setKeyValue("PSIR:IPTC digest",
				fromArgs("%.8X-%.8X-%.8X-%.8X",
					GetUns32BE(irPtr),
					GetUns32BE(irPtr + 4),
					GetUns32BE(irPtr + 8),
					GetUns32BE(irPtr + 12))
			);
		}
		else if (irID == kPSIR_CopyrightFlag) {
			bool copyrighted = (*irPtr != 0);
			tree->setKeyValue("PSIR:copyrighted", (copyrighted ? "yes" : "no"));
		}
		else if (irID == kPSIR_CopyrightURL) {
			tree->setKeyValue("PSIR:copyright URL", convert8Bit(irPtr, true, irLen));
		}
		else if (irID == kPSIR_OldCaption) {
			tree->setKeyValue("PSIR:old caption", convert8Bit(irPtr, true, irLen));
		}
		else if (irID == kPSIR_PrintCaption) {
			tree->comment("** obsolete print caption **");
		}
		else {
			tree->setKeyValue(
				fromArgs("PSIR:%s:#%d", irTypeStr.c_str(), irID),
				""
			);
		}
		tree->addComment("offset %d (0x%X), size %d", irOffset, irOffset, irLen);
		if (*irName != 0) tree->addComment("\"%.*s\"", (int)(*irName), (irName + 1));
		psirPtr = irPtr + ((irLen + 1) & 0xFFFFFFFE);	// Round the length to be even.
	} //while-loop

	if (psirPtr != psirEnd) {
		tree->addComment("** Unexpected end of image resources, delta %d", (long)(psirPtr - psirEnd));
	}

	//NB: dump routines moved up into if-else's	

	tree->popNode();
}	// DumpImageResources

	// =================================================================================================

static void
DumpOneIFD(int ifdIndex, XMP_Uns8 * ifdPtr, XMP_Uns8 * endPtr,
	XMP_Uns8 * tiffContent, XMP_Uns32 fileOffset, const char * label, std::string path)
{
	XMP_Uns8 * exifPtr = 0;
	XMP_Uns8 * gpsPtr = 0;
	XMP_Uns8 * interopPtr = 0;
	XMP_Uns8 * makerNotePtr = 0;
	XMP_Uns8 * psirPtr = 0;
	XMP_Uns8 * iptcPtr = 0;
	XMP_Uns8 * xmpPtr = 0;
	XMP_Uns32 psirLen = 0;
	XMP_Uns32 iptcLen = 0;
	XMP_Uns32 xmpLen = 0;

	XMP_Uns32 ifdOffset = ifdPtr - tiffContent;
	XMP_Uns16 fieldCount = TIFF_GetUns16(ifdPtr);
	XMP_Uns32 ifdLen = 2 + (12 * fieldCount) + 4;
	XMP_Uns32 nextIFD = TIFF_GetUns32(ifdPtr + ifdLen - 4);

	tree->pushNode("TIFF IFD #%d from %s", ifdIndex, label);
	tree->addComment("offset %d (0x%X), tag count %d",
		(ifdOffset + fileOffset), (ifdOffset + fileOffset), fieldCount);

	if (nextIFD == 0) {
		tree->comment("end of IFD chain");
	}
	else {
		tree->comment("next IFD offset %d (0x%X)", (nextIFD + fileOffset), (nextIFD + fileOffset));
	}

	XMP_Uns16 prevTag = 0;
	XMP_Uns8 * fieldPtr = tiffContent + ifdOffset + 2;

	if (!path.empty())
		path.append("/");

	path.append(fromArgs("IFD%d", ifdIndex));

	for (int i = 0; i < fieldCount; ++i, fieldPtr += 12) {

		XMP_Uns16 fieldTag = TIFF_GetUns16(fieldPtr);
		XMP_Uns16 fieldType = TIFF_GetUns16(fieldPtr + 2);
		XMP_Uns32 valueCount = TIFF_GetUns32(fieldPtr + 4);
		XMP_Uns32 valueOffset = TIFF_GetUns32(fieldPtr + 8);

		XMP_Uns8 * valuePtr = ifdPtr - ifdOffset + valueOffset;
		XMP_Uns32 valueLen = 0;
		if (fieldType < kTIFF_TypeEnd) valueLen = valueCount * sTIFF_TypeSizes[fieldType];
		if (valueLen <= 4) valuePtr = fieldPtr + 8;

		//===================== adding key here
		tree->setKeyValue(fromArgs("%s/TIFF:%d", path.c_str(), fieldTag));

		if ((fieldType < 1) || (fieldType >= kTIFF_TypeEnd)) {
			tree->addComment("type %d", fieldType);
		}
		else {
			tree->addComment("%s", sTIFF_TypeNames[fieldType]);
		}

		tree->addComment("count %d, value size %d", valueCount, valueLen);

		if (valueLen > 4) {
			tree->addComment("value offset %d (0x%X)", (valueOffset + fileOffset), (valueOffset + fileOffset));
		}
		else {
			XMP_Uns32 rawValue = GetUns32BE(fieldPtr + 8);
			tree->addComment("value in IFD (0x%.8X)", rawValue);
		}

		if (fieldTag == kTIFF_Exif) {
			tree->addComment("Exif IFD offset");
			exifPtr = tiffContent + TIFF_GetUns32(valuePtr);	// Value is Exif IFD offset.
		}
		else if (fieldTag == kTIFF_GPS) {
			tree->addComment("GPS IFD offset");
			gpsPtr = tiffContent + TIFF_GetUns32(valuePtr);	// Value is GPS IFD offset.
		}
		else if (fieldTag == kTIFF_Interop) {
			tree->addComment("Interoperability IFD offset");
			interopPtr = tiffContent + TIFF_GetUns32(valuePtr);	// Value is Interoperability IFD offset.
		}
		else if (fieldTag == kTIFF_MakerNote) {	// Decide if the Maker Note might be formatted as an IFD.
			tree->addComment("Maker Note");
			XMP_Uns32 itemCount = (valueLen - 6) / 12;
			if ((valueLen >= 18) && (valueLen == (6 + itemCount * 12)) &&
				(itemCount == TIFF_GetUns16(valuePtr)) &&
				(TIFF_GetUns32(valuePtr + 2 + (12 * itemCount)) == 0)) {
				makerNotePtr = valuePtr;
			}
		}
		else if (fieldTag == kTIFF_PSIR) {
			tree->addComment("PSIR");
			psirPtr = valuePtr;
			psirLen = valueLen;
		}
		else if (fieldTag == kTIFF_IPTC) {
			tree->addComment("IPTC");
			iptcPtr = valuePtr;
			iptcLen = valueLen;
		}
		else if (fieldTag == kTIFF_XMP) {
			tree->addComment("XMP");
			if (fieldType == kTIFF_ASCII) fieldType = kTIFF_Uns8;	// Avoid displaying the raw packet for mis-typed XMP.
			xmpPtr = valuePtr;
			xmpLen = valueLen;
		}
		else {
			for (int j = 0; sTIFF_TagNames[j].tag != 0; ++j) {
				if (sTIFF_TagNames[j].tag == fieldTag) {
					tree->addComment("%s", sTIFF_TagNames[j].name);
					break;
				}
			}
		}

		XMP_Uns8 value8;
		XMP_Uns16 value16;
		XMP_Uns32 value32;
		XMP_Uns32 denom;
		std::string tempStr;
		char cs[31];

		switch (fieldType) {

		case kTIFF_Uns8:
			if (valueCount == 1) {
				value8 = *valuePtr;
				tree->addComment("hex value = 0x%.2X", value8);
				tree->changeValue("%u", value8);
			}
			break;

		case kTIFF_ASCII:
			tree->changeValue(convert8Bit(valuePtr, false /* internal NULs OK*/, valueLen));
			break;

		case kTIFF_Uns16:
			if (valueCount == 1) {
				value16 = TIFF_GetUns16(valuePtr);
				tree->addComment("hex value = 0x%.4X", value16);
				tree->changeValue("%u", value16);
			}
			break;

		case kTIFF_Uns32:
			if (valueCount == 1) {
				value32 = TIFF_GetUns32(valuePtr);
				tree->addComment("hex value = 0x%.8X", value32);
				tree->changeValue("%u", value32);
			}
			break;

		case kTIFF_URational:
			for (unsigned int j = 0; j < valueCount; j++) {
				value32 = TIFF_GetUns32(valuePtr + (j * 8));
				denom = TIFF_GetUns32(valuePtr + (j * 8) + 4);
				snprintf(cs, 30, "%u", value32);
				tempStr += cs;
				tempStr += "/";
				snprintf(cs, 30, "%u", denom);
				tempStr += cs;
				if (j < valueCount - 1)
					tempStr += ";";
			}
			if (tempStr.length() > 0)
				tree->changeValue(tempStr);
			break;

		case kTIFF_Int8:
			if (valueCount == 1) {
				value8 = *valuePtr;
				//fno: show the hex value unsigned (memory representation) and the decimal signed
				tree->addComment("hex value 0x%.2X", value8);
				tree->changeValue("%d", *((XMP_Int8*)&value8));
			}
			break;

		case kTIFF_Undef8:
			if (valueCount == 1) {
				value8 = *valuePtr;
				tree->changeValue("0x%.2X", value8);
			}
			else if (fieldTag == 36864) {	// ExifVersion
				tree->changeValue("%.*s", valueCount, valuePtr);
			}
			else if (fieldTag == 37510) {	// UserComment
				XMP_Uns8 * encPtr = valuePtr;
				valuePtr += 8;
				valueCount -= 8;
				sprintf(cs, "encoding = %.8s", encPtr);
				tempStr += cs;
				if (!CheckBytes(encPtr, "UNICODE\0", 8)) {
					tree->changeValue(convert8Bit(valuePtr, false, valueCount));
				}
				else {
					bool doBE = beTIFF;
					if (CheckBytes(valuePtr, "\xFE\xFF", 2)) {
						doBE = true;
						valuePtr += 2;
						valueCount -= 2;
						tempStr += ", BE BOM";
					}
					if (CheckBytes(valuePtr, "\xFF\xFE", 2)) {
						doBE = false;
						valuePtr += 2;
						valueCount -= 2;
						tempStr += ", LE BOM";
					}
					if (doBE) {
						tree->changeValue(convert16Bit(true, (XMP_Uns8*)valuePtr, false, valueCount));
						//PrintOnlyASCII_16BE ( (XMP_Uns16*)valuePtr, valueCount, ", value =", false /* ! stopOnNUL */ );
					}
					else {
						tree->changeValue(convert16Bit(false, (XMP_Uns8*)valuePtr, false, valueCount));
						//PrintOnlyASCII_16LE ( (XMP_Uns16*)valuePtr, valueCount, ", value =", false /* ! stopOnNUL */ );
					}
				}
			}

			if (tempStr.length() > 0)
				tree->addComment(tempStr);
			break;

		case kTIFF_Int16:
			if (valueCount == 1) {
				value16 = TIFF_GetUns16(valuePtr);
				tree->changeValue("%d (0x%.4X)", *((XMP_Int16*)&value16), value16);
			}
			break;

		case kTIFF_Int32:
			if (valueCount == 1) {
				value32 = TIFF_GetUns32(valuePtr);
				tree->changeValue("%d (0x%.8X)", *((XMP_Int32*)&value32), value32);
			}
			break;

		case kTIFF_SRational:
			if (valueCount == 1) {
				value32 = TIFF_GetUns32(valuePtr);
				denom = TIFF_GetUns32(valuePtr + 4);
				tree->changeValue("%d/%d", *((XMP_Int32*)&value32), *((XMP_Int32*)&denom));
			}
			break;

		case kTIFF_Float:
			break;

		case kTIFF_Double:
			break;

		case kTIFF_IFD:
			if (valueCount == 1) {
				value32 = TIFF_GetUns32(valuePtr);
				tree->addComment("hex value = 0x%.8X", value32);
				tree->changeValue("%u", value32);
			}
			break;

		default:
			tree->addComment("** unknown type **");
			break;

		}

		if (fieldTag == prevTag) {
			tree->addComment("** Repeated tag **");
		}
		else if (fieldTag < prevTag) {
			tree->addComment("** Out of order tag **");
		}

		prevTag = fieldTag;

	}

	if (exifPtr != 0) {
		DumpIFDChain(exifPtr, endPtr, tiffContent,
			(fileOffset + (exifPtr - tiffContent)), "TIFF tag #34665 (Exif IFD)", path + "/TIFF:34665");
	}

	if (gpsPtr != 0) {
		DumpIFDChain(gpsPtr, endPtr, tiffContent,
			(fileOffset + (gpsPtr - tiffContent)), "TIFF tag #34853 (GPS Info IFD)", path + "/TIFF:34853");
	}

	if (interopPtr != 0) {
		DumpIFDChain(interopPtr, endPtr, tiffContent,
			(fileOffset + (interopPtr - tiffContent)), "TIFF tag #40965 (Interoperability IFD)", path + "/TIFF:40965");
	}

	if (makerNotePtr != 0) {
		DumpIFDChain(makerNotePtr, endPtr, tiffContent,
			(fileOffset + (makerNotePtr - tiffContent)), "TIFF tag #37500 (Maker Note)", path + "/TIFF:37500");
	}

	if (iptcPtr != 0) {
		DumpIPTC(iptcPtr, iptcLen, (fileOffset + (iptcPtr - tiffContent)), "TIFF tag #33723");
	}

	if (psirPtr != 0) {
		DumpImageResources(psirPtr, psirLen, (fileOffset + (psirPtr - tiffContent)), "TIFF tag #34377");
	}

	if (xmpPtr != 0) {
		DumpXMP(xmpPtr, xmpLen, (fileOffset + (xmpPtr - tiffContent)), "TIFF tag #700");
	}

	tree->popNode();
}	// DumpOneIFD

	// =================================================================================================


static void
DumpIFDChain(XMP_Uns8 * startPtr, XMP_Uns8 * endPtr,
	XMP_Uns8 * tiffContent, XMP_Uns32 fileOrigin, const char * label, std::string path, bool isHeaderAbsent)
{
	XMP_Uns8 * ifdPtr = startPtr;
	XMP_Uns32  ifdOffset = startPtr - tiffContent;

	if (isHeaderAbsent) // It's a kind of hack to iterate all the ifdboxes at least once.
		ifdOffset = 1;

	for (size_t ifdIndex = 0; ifdOffset != 0; ++ifdIndex) {

		if ((ifdPtr < tiffContent) || (ifdPtr >= endPtr)) {
			ifdOffset = fileOrigin + (ifdPtr - tiffContent);
			tree->comment("** Invalid IFD offset, %d (0x%X) tree.", ifdOffset, ifdOffset);
			return;
		}

		XMP_Uns16 fieldCount = TIFF_GetUns16(ifdPtr);
		DumpOneIFD(ifdIndex, ifdPtr, endPtr, tiffContent, fileOrigin, label, path);
		ifdOffset = TIFF_GetUns32(ifdPtr + 2 + (12 * fieldCount));
		ifdPtr = tiffContent + ifdOffset;

	}

}	// DumpIFDChain

	// =================================================================================================

static void
DumpTIFF(XMP_Uns8 * tiffContent, XMP_Uns32 tiffLen, XMP_Uns32 fileOffset, const char * label, std::string path, bool isHeaderAbsent)
{
	tree->pushNode("TIFF content from %s", label);
	// ! TIFF can be nested because of the Photoshop 6 weiredness. Save and restore the procs.
	GetUns16_Proc save_GetUns16 = TIFF_GetUns16;
	GetUns32_Proc save_GetUns32 = TIFF_GetUns32;
	GetUns64_Proc save_GetUns64 = TIFF_GetUns64;

	XMP_Uns32 ifdOffset = 0;

	if (!isHeaderAbsent)
	{
		if (CheckBytes(tiffContent, "II\x2A\x00", 4)) {
			beTIFF = false;
			TIFF_GetUns16 = GetUns16LE;
			TIFF_GetUns32 = GetUns32LE;
			TIFF_GetUns64 = GetUns64LE;
			tree->addComment("Little endian ");
		}
		else if (CheckBytes(tiffContent, "MM\x00\x2A", 4)) {
			beTIFF = true;
			TIFF_GetUns16 = GetUns16BE;
			TIFF_GetUns32 = GetUns32BE;
			TIFF_GetUns64 = GetUns64BE;
			tree->addComment("Big endian ");
		}
		else {
			tree->comment("** Missing TIFF image file header tree.");
			return;
		}

		tree->addComment("TIFF from %s, offset %d (0x%X), size %d", label, fileOffset, fileOffset, tiffLen);

		ifdOffset = TIFF_GetUns32(tiffContent + 4);
	}
	else
	{
		beTIFF = false;
		TIFF_GetUns16 = GetUns16LE;
		TIFF_GetUns32 = GetUns32LE;
		TIFF_GetUns64 = GetUns64LE;
		tree->addComment("Little endian ");
	}

	DumpIFDChain(tiffContent + ifdOffset, tiffContent + tiffLen, tiffContent, fileOffset, label, path, isHeaderAbsent);

	TIFF_GetUns16 = save_GetUns16;
	TIFF_GetUns32 = save_GetUns32;
	TIFF_GetUns64 = save_GetUns64;

	tree->popNode();
}	// DumpTIFF


	// =================================================================================================

static void DumpTIFF(const JpegMarkers& exifMarkers, XMP_Uns8 * dataStart, const char * label, std::string path)
{
	XMP_Uns32 i = 0, size = exifMarkers.size();
	std::string combinedExifData;
	tree->pushNode("Combined EXIF Markers from %s", path.c_str());
	for (i = 0; i < size; i++) {
		tree->pushNode("EXIF Marker %d", i + 1);
		tree->addComment("offset %d (0x%X), size %d", exifMarkers[i].jpegMarkerPtr - dataStart,
			exifMarkers[i].jpegMarkerPtr - dataStart, exifMarkers[i].jpegMarkerLen);
		combinedExifData.append((const char *)exifMarkers[i].jpegMarkerPtr, exifMarkers[i].jpegMarkerLen);
		tree->popNode();
	}

	DumpTIFF((XMP_Uns8 *)combinedExifData.data(), combinedExifData.length(), exifMarkers[0].jpegMarkerPtr - dataStart, label, path);

	tree->popNode();
}	// DumpTIFF

	// =================================================================================================

static void
DumpPhotoshop(XMP_Uns8 * psdContent, XMP_Uns32 psdLen)
{
	psdLen = psdLen;	// Avoid unused parameter warning.

	XMP_Uns32  psirOffset = 26 + 4 + GetUns32BE(psdContent + 26);
	XMP_Uns8 * psirSect = psdContent + psirOffset;
	XMP_Uns8 * psirPtr = psirSect + 4;
	XMP_Uns32  psirLen = GetUns32BE(psirSect);

	DumpImageResources(psirPtr, psirLen, (psirPtr - psdContent), "Photoshop file");

}	// DumpPhotoshop

	// =================================================================================================

static void
DumpJPEG(XMP_Uns8 * jpegContent, XMP_Uns32 jpegLen)
{
	XMP_Uns8 * endPtr = jpegContent + jpegLen;
	XMP_Uns8 * segPtr = jpegContent;
	XMP_Uns32  segOffset;

	XMP_Uns8 * xmpPtr = 0;
	XMP_Uns16 xmpLen = 0;

	JpegMarkers psirMarkers, exifMarkers;

	while (segPtr < endPtr) { // ----------------------------------------------------------------

		XMP_Uns16  segMark = GetUns16BE(segPtr);
		if (segMark == 0xFFFF) {
			segPtr += 1;	// Skip leading 0xFF pad byte.
			continue;
		}

		XMP_Uns16 minorKind = segMark & 0x000F;
		segOffset = segPtr - jpegContent;

		tree->pushNode("JPEG:%.4X", segMark);
		tree->addComment("offset %d (0x%X)", segOffset, segOffset);

		if (((segMark >> 8) != 0xFF) || (segMark == 0xFF00)) {
			tree->addComment("** invalid JPEG marker **");
			tree->popNode();
			break;
		}

		// Check for standalone markers first, only fetch the length for marker segments.

		if (segMark == 0xFF01) {
			tree->addComment("** TEM **");
			segPtr += 2;	// A standalone marker.
			tree->popNode();
			continue;
		}
		else if ((0xFFD0 <= segMark) && (segMark <= 0xFFD7)) {
			tree->addComment(fromArgs("RST%d  ** unexpected **", minorKind));
			segPtr += 2;	// A standalone marker.
			tree->popNode();
			continue;
		}
		else if (segMark == 0xFFD8) {
			tree->addComment("SOI");
			segPtr += 2;	// A standalone marker.
			tree->popNode();
			continue;
		}
		else if (segMark == 0xFFD9) {
			tree->addComment("EOI");
			segPtr += 2;	// A standalone marker.
			tree->popNode();
			break;	// Exit on EOI.
		}

		XMP_Uns16 segLen = GetUns16BE(segPtr + 2);

		// figure out Exif vs PSIR vs XMP
		if ((0xFFE0 <= segMark) && (segMark <= 0xFFEF)) {
			const char* segName = (const char *)(segPtr + 4);
			tree->addComment(fromArgs("size %d, APP%d, \"%s\"", segLen, minorKind, segName));
			if ((minorKind == 1) &&
				((memcmp(segName, "Exif\0\0", 6) == 0) || (memcmp(segName, "Exif\0\xFF", 6) == 0))) {
				tree->addComment("EXIF");
				tree->changeValue("EXIF");
				JpegMarker exifMarker;
				exifMarker.jpegMarkerPtr = segPtr + 4 + 6;
				exifMarker.jpegMarkerLen = segLen - 2 - 6;
				exifMarkers.push_back(exifMarker);
			}
			else if ((minorKind == 13) && (strcmp(segName, "Photoshop 3.0") == 0)) {
				tree->addComment("PSIR");
				tree->changeValue("PSIR");
				JpegMarker psirMarker;
				psirMarker.jpegMarkerPtr = segPtr + 4 + strlen(segName) + 1;
				psirMarker.jpegMarkerLen = (XMP_Uns16)(segLen - 2 - strlen(segName) - 1);
				psirMarkers.push_back(psirMarker);
			}
			else if ((minorKind == 1) && (strcmp(segName, "http://ns.adobe.com/xap/1.0/") == 0)) {
				tree->addComment("XMP");
				tree->changeValue("XMP");
				xmpPtr = segPtr + 4 + strlen(segName) + 1;
				xmpLen = (XMP_Uns16)(segLen - 2 - strlen(segName) - 1);
			}
			segPtr += 2 + segLen;
			tree->popNode();
			continue;
		}

		if (segMark == 0xFFDA) {
			tree->addComment(fromArgs("size %d, SOS", segLen));
			segPtr += 2 + segLen;	// Skip the SOS marker segment itself
			long rstCount = 0;
			while (segPtr < endPtr) {	// Skip the entropy-coded data and RSTn markers.
				if (*segPtr != 0xFF) {
					segPtr += 1;	// General data byte.
				}
				else {
					segMark = GetUns16BE(segPtr);
					if (segMark == 0xFF00) {
						segPtr += 2;	// Padded 0xFF data byte.
					}
					else if ((segMark < 0xFFD0) || (segMark > 0xFFD7)) {
						segLen = 0;
						segPtr -= 2;	// Prepare for the increment in the outer loop.
						break;	// Exit, non-RSTn marker.
					}
					else {
						++rstCount;
						segPtr += 2;
					}
				}
			}
			tree->addComment(fromArgs("%d restart markers", rstCount));

			segPtr += 2 + segLen;
			tree->popNode();
			continue;
		}


		if ((0xFF02 <= segMark) && (segMark <= 0xFFBF)) {
			tree->addComment(fromArgs("size %d, ** RES **", segLen));
		}
		else if ((0xFFC0 <= segMark) && (segMark <= 0xFFC3)) {
			tree->addComment(fromArgs("size %d, SOF%d", segLen, minorKind));
		}
		else if (segMark == 0xFFC4) {
			tree->addComment(fromArgs("size %d, DHT", segLen));
		}
		else if ((0xFFC5 <= segMark) && (segMark <= 0xFFC7)) {
			tree->addComment(fromArgs("size %d, SOF%d", segLen, minorKind));
		}
		else if (segMark == 0xFFC8) {
			tree->addComment(fromArgs("size %d, JPG", segLen));
		}
		else if ((0xFFC9 <= segMark) && (segMark <= 0xFFCB)) {
			tree->addComment(fromArgs("size %d, SOF%d", segLen, minorKind));
		}
		else if (segMark == 0xFFCC) {
			tree->addComment(fromArgs("size %d, DAC", segLen));
		}
		else if ((0xFFCD <= segMark) && (segMark <= 0xFFCF)) {
			tree->addComment(fromArgs("size %d, SOF%d", segLen, minorKind));
		}
		else if (segMark == 0xFFDB) {
			tree->addComment(fromArgs("size %d, DQT", segLen));
		}
		else if (segMark == 0xFFDC) {
			tree->addComment(fromArgs("size %d, DNL", segLen));
		}
		else if (segMark == 0xFFDD) {
			tree->addComment(fromArgs("size %d, DRI", segLen));
		}
		else if (segMark == 0xFFDE) {
			tree->addComment(fromArgs("size %d, DHP", segLen));
		}
		else if (segMark == 0xFFDF) {
			tree->addComment(fromArgs("size %d, EXP", segLen));
		}
		else if ((0xFFF0 <= segMark) && (segMark <= 0xFFFD)) {
			tree->addComment(fromArgs("size %d, JPG%d", segLen, minorKind));
		}
		else if (segMark == 0xFFFE) {
			tree->addComment(fromArgs("size %d, COM", segLen));
		}
		else {
			tree->addComment("** UNRECOGNIZED MARKER **");
		}

		segPtr += 2 + segLen;

		tree->popNode();
	} // ------------------------------------------------------------------------------------

	if (segPtr != endPtr) {
		segOffset = segPtr - jpegContent;
		tree->addComment(fromArgs(
			"** Unexpected end of JPEG markers at offset %d (0x%X), delta %d tree.",
			segOffset, segOffset, (long)(endPtr - segPtr)
		));
	}

	if (exifMarkers.size() > 0) DumpTIFF(exifMarkers, jpegContent, "JPEG Exif APP1", "JPEG:APP1");
	if (psirMarkers.size() > 0) DumpImageResources(psirMarkers, jpegContent, "JPEG Photoshop APP13");
	if (xmpPtr != 0) DumpXMP(xmpPtr, xmpLen, (xmpPtr - jpegContent), "JPEG XMP APP1");

}	// DumpJPEG

	// =================================================================================================
	//#if !IOS_ENV
static const XMP_Uns8 kUUID_XMP[16] =
{ 0xBE, 0x7A, 0xCF, 0xCB, 0x97, 0xA9, 0x42, 0xE8, 0x9C, 0x71, 0x99, 0x94, 0x91, 0xE3, 0xAF, 0xAC };
/*#else
static const XMP_Uns8 kUUID_XMP[16]  =
{ 0xFFFFFFBE, 0x0000007A, 0xFFFFFFCF, 0xFFFFFFCB, 0xFFFFFF97, 0xFFFFFFA9, 0x00000042, 0xFFFFFFE8, 0xFFFFFF9C, 0x00000071, 0xFFFFFF99, 0xFFFFFF94, 0xFFFFFF91, 0xFFFFFFE3, 0xFFFFFFAF, 0xFFFFFFAC };
#endif */
static const XMP_Uns8 kUUID_Exif[16] =
{ 0x05, 0x37, 0xCD, 0xAB, 0x9D, 0x0C, 0x44, 0x31, 0xA7, 0x2A, 0xFA, 0x56, 0x1F, 0x2A, 0x11, 0x3E };
static const XMP_Uns8 kUUID_IPTC[16] =
{ 0x09, 0xA1, 0x4E, 0x97, 0xC0, 0xB4, 0x42, 0xE0, 0xBE, 0xBF, 0x36, 0xDF, 0x6F, 0x0C, 0xE3, 0x6F };
static const XMP_Uns8 kUUID_PSIR[16] =
{ 0x2C, 0x4C, 0x01, 0x00, 0x85, 0x04, 0x40, 0xB9, 0xA0, 0x3E, 0x56, 0x21, 0x48, 0xD6, 0xDF, 0xEB };
// -------------------------------------------------------------------------------------------------

/**
* helper routine to get past the version and flags field...
*/
static void
digestISOFullBoxExtension(LFA_FileRef file, std::string isoPath, XMP_Int64& remainingSize, XMP_Uns8& version, XMP_Uns32& flags)
{
	version = LFA_ReadUns8(file);
	flags = 0;

	LFA_Read(file, &flags, 3, true); // read only 3 byte!
	flags = flags >> 8; // (move to bit 0-23)
	remainingSize -= 4;

	tree->setKeyValue(isoPath + "version", fromArgs("%d", version));
	tree->setKeyValue(isoPath + "flags", fromArgs("0x%.8X", flags));
}

static void
digestInternationalTextSequence(LFA_FileRef file, std::string isoPath, XMP_Int64* remainingSize)
{
	XMP_Int64 miniBoxStringSize = tree->digest16u(file, isoPath + "size", true, true);
	tree->digest16u(file, isoPath + "language code", true, true);
	(*remainingSize) -= 4;
	if ((*remainingSize) != miniBoxStringSize)
		tree->addComment("WARNING: boxSize and miniBoxSize differ!");
	tree->digestString(file, isoPath + "value", miniBoxStringSize, false);
}

/**
* dumps one *or several* (ohter while loop) ISO Boxes within the indicated space:
*
* maxBoxLen is :== fileLen on top-level, otherwise available length of outer box (exluding header size naturally)
*
* (NB: reading (and displaying) box types, compat brands and other 4-letter stuff
*  as LE is somehow easier (might need adjustment for PPC though)
*
* practices:
* compensate endianess using MakeUns32BE() prior to use as string, NOT for numeric compare
*/
static void
DumpISOBoxes(LFA_FileRef file, XMP_Uns32 maxBoxLen, std::string _isoPath)
{
	XMP_Int64 endOfThisLevel = LFA_Tell(file) + maxBoxLen;
	std::string origIsoPath(_isoPath);
	std::string isoPath(_isoPath);
	std::list<int> keys;
	while (LFA_Tell(file) < endOfThisLevel)
	{
		XMP_Int64 boxHeaderSize = 8;

		//assertMsg("No space for ISO box header", boxHeaderSize <= maxBoxLen ); 

		//// certainly not enough room for another box?
		// could be a 32bit zero trailing a udta
		// or, uhm, something garbage-ish...
		if (LFA_Tell(file) + boxHeaderSize > endOfThisLevel)
		{
			XMP_Int64 numUnusedBytes = (endOfThisLevel - LFA_Tell(file));
			tree->digestString(file, isoPath + "unused", numUnusedBytes, false);
			tree->addComment("'free' since too small for a box");

			bool ok;
			LFA_Seek(file, endOfThisLevel, SEEK_SET, &ok);
			assertMsg("skippind to-small space failed (truncated file?)", ok);
			continue; // could just as well: return
		}

		XMP_Int64 boxPos = LFA_Tell(file); // store here, output below
		XMP_Int64 boxSize = tree->digest32u(file, "", true); // NB: 32bit <- 64bit
		XMP_Uns32 boxType = tree->digest32u(file, "", false);

		switch (boxSize)
		{
		case 0:
			// A value of zero says that the box extends to the end of the file.
			boxSize = (maxBoxLen - boxPos); // *** could be errorneous below top-level
			break;
		case 1:
			// A value of 1 says that a 64-bit big endian size is written after the box type field, the data follows.
			boxSize = LFA_ReadUns64_BE(file);
			boxHeaderSize += 8;
			break;
		default:
			break;
		}

		XMP_Uns32 tempBoxType = GetUns32LE(&boxType);
		std::string boxString(fromArgs("%.4s", &tempBoxType));

		if (boxString.size() != 0)
		{
			// substitute mac-copyright signs with an easier-to-handle "(c)"
#if WIN_UNIVERSAL_ENV
			if (boxString.at(0) == 0xffffffffffffffa9)
#elif !IOS_ENV
			if (boxString.at(0) == 0xa9)
#else
			if (boxString.at(0) == 0xffffffa9)
#endif
				boxString = std::string("(c)") + boxString.substr(1);
		}
		else
			break;

		isoPath = origIsoPath + boxString + "/";

		// TEMP
		// Log::info("pushing %s, endOfThisLevel: 0x%X", isoPath.c_str(), endOfThisLevel );
		// printf ("%s \n", isoPath.c_str());
		tree->pushNode(isoPath);
#if ANDROID
		tree->addComment("offset 0x%llX, size 0x%llX", boxPos, boxSize);
#else
		tree->addComment("offset 0x%I64X, size 0x%I64X", boxPos, boxSize);
#endif


		// endOfBoxPos saves the hassle of keeping the remainingSize up-to-date
		// (which is only needed and only done, if usefull for the specific box)
		XMP_Int64 remainingSize = boxSize - boxHeaderSize;
		XMP_Int64 endOfBoxPos = LFA_Tell(file) + remainingSize;

		// ---------------------------------------------
		// for FullBoxes:
		XMP_Uns8 version = 255;
		XMP_Uns32 flags = 0xFFFFFF;

		switch (boxType)
		{
			// container boxes (FULL), that contain (relevant) boxes:
		case 0x6174656D: // meta, FULLBOX
			if (isoPath == "moov/udta/meta/")
			{
				digestISOFullBoxExtension(file, isoPath, remainingSize, version, flags);
				DumpISOBoxes(file, remainingSize, isoPath);
				break;
			}
			else if (isoPath == "moov/meta/")
			{
				DumpISOBoxes(file, remainingSize, isoPath);
				break;
			}
			else if (isoPath == "meta/") // for HEIF
			{
				digestISOFullBoxExtension(file, isoPath, remainingSize, version, flags);
				DumpISOBoxes(file, remainingSize, isoPath);
				break;
			}
			else
				break; //no area of interest (and navigate around some malformed files)

					   // container boxes (all non-FULL), that contain (relevant) boxes:
		case 0x666E6964: //dinf
			if (isoPath == "meta/dinf/")
			{
				bool ok;
				XMP_Int64 keep = LFA_Tell(file);
				DumpISOBoxes(file, remainingSize, isoPath);
				LFA_Seek(file, keep, SEEK_SET, &ok);
				assertMsg("seek failed", ok);
			}
			break;
		case 0x66657264: // dref
			digestISOFullBoxExtension(file, isoPath, remainingSize, version, flags);
			tree->digest32u(file, isoPath + "entry_count", true, true);
			DumpISOBoxes(file, remainingSize, isoPath);
			break;
		case 0x206C7275: // for 'url' field, for 'urn' support to be added later
			digestISOFullBoxExtension(file, isoPath, remainingSize, version, flags);
			if (remainingSize > 0)
			{
				tree->digestString(file, isoPath + "location", 0);
			}
			break;
		case 0x6D746970: //pitm
			digestISOFullBoxExtension(file, isoPath, remainingSize, version, flags);
			tree->digest16u(file, isoPath + "item_ID", true, true);
			break;
		case 0x666E6969: //iinf
			digestISOFullBoxExtension(file, isoPath, remainingSize, version, flags);
			if (version == 0)
			{
				tree->digest16u(file, isoPath + "entry_count", true, true);
			}
			else
			{
				tree->digest32u(file, isoPath + "entry_count", true, true);
			}
			
			DumpISOBoxes(file, remainingSize, isoPath);
			break;
		case 0x65666E69: //infe
			XMP_Uns16 item_id;
			digestISOFullBoxExtension(file, isoPath, remainingSize, version, flags);
			if ((version == 0) || (version == 1))
			{
				item_id = tree->digest16u(file, isoPath + "item_id", true, false);
				remainingSize -= 2;
				tree->digest16u(file, isoPath + "item_protection_index", true, true);
				remainingSize -= 2;
				item_name = tree->digestString(file, isoPath + "item_name", 0);
				if (0 == item_name.compare(0, 4, "Exif"))
				{
					exif_item_id = item_id;
				}
				if (0 == item_name.compare(0, 4, "mime"))
				{
					mime_item_id = item_id;
				}
				remainingSize -= (item_name.size() + 1);
				if (remainingSize == 0)
				{
					//there are no fields to process further
				}
				else if (remainingSize > 0)
				{
					//check for other fields here. If control reaches here, it means we have content_type field also
					content_type = tree->digestString(file, isoPath + "content_type", 0);
					remainingSize -= (content_type.size() + 1);
					if (remainingSize == 0)
					{
						//there are no fields to process further
					}
					else if (remainingSize > 0)
					{
						// we have content_encoding present also
						content_encoding = tree->digestString(file, isoPath + "content_encoding", 0);
					}
					else
					{
						//cout << "Some probem occurred when parsing infe box (content_encoding).Please check." << endl;
					}
					break;

				}
				else
				{
					//cout << "Some probem occurred when parsing infe box.Please check." << endl;
				}
			}
			if (version >= 2)
			{
				if (version == 2)
				{
					item_id = tree->digest16u(file, isoPath + "item_id", true, false);
					remainingSize -= 2;
				}
				else if (version == 3)
				{
					item_id = tree->digest32u(file, isoPath + "item_id", true, false);
					remainingSize -= 4;
				}
				tree->digest16u(file, isoPath + "item_protection_index", true, true);
				remainingSize -= 2;
				item_type=tree->digest32u(file, isoPath + "item_type", true, true);
				remainingSize -= 4;
				item_name = tree->digestString(file, isoPath + "item_name", 0);
				remainingSize -= (item_name.size() + 1);

				if (0x45786966 == item_type)
				{
					exif_item_id = item_id; 
				}
				if (0x6D696D65 == item_type)
				{
					mime_item_id = item_id;
				}
				else if (0x75726900 == item_type)
				{
					string item_uri_type=tree->digestString(file, isoPath + "item_uri_type", 0);
					remainingSize -= (item_uri_type.size() + 1);
				}


				if (remainingSize == 0)
				{
					//there are no fields to process further
				}
				else if (remainingSize > 0)
				{
					//check for other fields here. If control reaches here, it means we have content_type field also
					content_type = tree->digestString(file, isoPath + "content_type", 0);
					remainingSize -= (content_type.size() + 1);
					if (remainingSize == 0)
					{
						//there are no fields to process further
					}
					else if (remainingSize > 0)
					{
						// we have content_encoding present also
						content_encoding = tree->digestString(file, isoPath + "content_encoding", 0);
					}
					else
					{
						cout << "Some probem occurred when parsing infe box (content_encoding).Please check." << endl;
					}
				}
			}

			//Get the data for exif - using value from exif_item_id
			// Dump the exif metadata here
			{
				if (exif_item_id != 0 && tree->hasNode("meta/iloc/item[" + to_string(exif_item_id) + "]/extent_offset"))
				{
					bool ok;
					XMP_Int64 keep = LFA_Tell(file);
					XMP_Uns32 offset = stoi(tree->getValue("meta/iloc/item[" + to_string(exif_item_id) + "]/extent_offset"));	// Here tiff stream starts after 16476 ( 16388 + 84 + 4 ) bytes from starting of PANA atom's content
																																//XMP_Uns8 *ExifSize = (XMP_Uns8*)malloc(4);
																																//Now point file to offset value
					LFA_Seek(file, offset, SEEK_SET, &ok);
					XMP_Uns32 blocksize = LFA_ReadUns32_BE(file);
					LFA_Seek(file, offset + 4 + blocksize, SEEK_SET, &ok);
					XMP_Uns64 tiffLength1 = (stoi(tree->getValue("meta/iloc/item[" + to_string(exif_item_id) + "]/extent_length")));
					XMP_Uns64 tiffLength = (stoi(tree->getValue("meta/iloc/item[" + to_string(exif_item_id) + "]/extent_length")) - blocksize - 4);
					XMP_Uns8 *tiffContent = (XMP_Uns8*)malloc(tiffLength);
					LFA_Read(file, tiffContent, tiffLength, true);
					DumpTIFF(tiffContent, tiffLength, offset, "HEIF Exif", "HEIF:Exif");
					LFA_Seek(file, keep, SEEK_SET, &ok);
					assertMsg("seek failed", ok);
					exif_item_id = NULL;
				}
			}
			//Get the data for xmp - using value from mime_item_id
			// Dump the xmp metadata here
			{
				if (mime_item_id != 0  && tree->hasNode("meta/iloc/item[" + to_string(mime_item_id) + "]/extent_offset"))
				{
					bool ok;
					XMP_Int64 keep = LFA_Tell(file);
					XMP_Uns32 offset = stoi(tree->getValue("meta/iloc/item[" + to_string(mime_item_id) + "]/extent_offset"));
					XMP_Uns64 xmpLength = stoi(tree->getValue("meta/iloc/item[" + to_string(mime_item_id) + "]/extent_length"));
					LFA_Seek(file, offset, SEEK_SET, &ok);
					XMP_Uns8 *xmpContent = (XMP_Uns8*)malloc(xmpLength);
					LFA_Read(file, xmpContent, xmpLength, true);
					DumpXMP(xmpContent, xmpLength, offset, "XMP");
					LFA_Seek(file, keep, SEEK_SET, &ok);
					assertMsg("seek failed", ok);
					mime_item_id = NULL;
				}
			}
			break;
		case 0x636F6C69: //iloc
			XMP_Uns16 deriveValues, item_count, extent_count;
			XMP_Uns8 offset_size, length_size, base_offset_size, index_size;
			digestISOFullBoxExtension(file, isoPath, remainingSize, version, flags);
			// Read the contents here, do not add them to tree, move the seek to 16bits forward
			deriveValues = LFA_ReadInt16_BE(file);
			//add these values in the tree
			offset_size = (deriveValues & 0xF000) >> 12;
			length_size = (deriveValues & 0x0F00) >> 8;
			base_offset_size = (deriveValues & 0x00F0) >> 4;
			tree->setKeyValue(isoPath + "offset_size", fromArgs("%d", offset_size));
			tree->setKeyValue(isoPath + "length_size", fromArgs("%d", length_size));
			tree->setKeyValue(isoPath + "base_offset_size", fromArgs("%d", base_offset_size));
			if (version == 1 || version == 2)
			{
				index_size = (deriveValues & 0x000F);
				tree->setKeyValue(isoPath + "index_size", fromArgs("%d", index_size));
			}
			else
				index_size = 0;


			if (version < 2)
			{
				item_count = tree->digest16u(file, isoPath + "item_count", true, true);
			}
			else if (version == 2) {
				item_count = tree->digest32u(file, isoPath + "item_count", true, true);
			}
			for (int i = 0; i < item_count; i++)
			{
				XMP_Uns32 item_id;
				bool ok;
				XMP_Int64 keep = LFA_Tell(file);
				if (version < 2)
				{
					item_id = LFA_ReadUns16_BE(file);
				}
				else if (version == 2) {
					item_id = LFA_ReadUns32_BE(file);
				}
				LFA_Seek(file, keep, SEEK_SET, &ok);
				assertMsg("seek failed", ok);
				
				string modifiedisoPath = isoPath + "item[" + to_string(item_id) + "]/";
				if (version < 2)
				{
					item_id=tree->digest16u(file, modifiedisoPath + "item_ID", true, true);
				}
				else if (version == 2) {
					item_id=tree->digest32u(file, modifiedisoPath + "item_ID", true, true);
				}

				if (version == 1 || version == 2)
				{
					//construction method
					XMP_Uns16 deriveConstructionMethod = LFA_ReadInt16_BE(file);
					XMP_Uns8 constructionMethod;
					constructionMethod = (deriveConstructionMethod & 0xF000) >> 12;
					tree->setKeyValue(isoPath + "construction_method", fromArgs("%d", constructionMethod));
				}
				tree->digest16u(file, modifiedisoPath + "data_reference_index", true, true);
				if (base_offset_size != 0)
				{
					if (base_offset_size == 4) {
						tree->digest32u(file, modifiedisoPath + "base_offset", true, true);
					}
					else if (base_offset_size == 8)
					{
						tree->digest64u(file, modifiedisoPath + "base_offset", true, true);
					}
					else
					{
						cout << "wrong value of base_offset received!! It should be either 0 or 4 or 8" << endl;
					}
				}

				extent_count = tree->digest16u(file, modifiedisoPath + "extent_count", true, false);
				for (int j = 0; j < extent_count; j++)
				{
					if ((version == 1 || version == 2) && (index_size > 0))
					{

						if (index_size == 4) {
							tree->digest32u(file, modifiedisoPath + "index_size", true, false);
						}
						else if (index_size == 8)
						{
							tree->digest64u(file, modifiedisoPath + "index_size", true, false);
						}
						else
						{
							cout << "wrong value of index_size received!! It should be either 0 or 4 or 8" << endl;
						}
					}

					if (offset_size > 0)
					{
						if (offset_size == 4) {
							tree->digest32u(file, modifiedisoPath + "extent_offset", true, false);
						}
						else if (offset_size == 8)
						{
							tree->digest64u(file, modifiedisoPath + "extent_offset", true, false);
						}
						else
						{
							cout << "wrong value of offset_size received!! It should be either 0 or 4 or 8" << endl;
						}
					}

					if (length_size > 0)
					{
						if (length_size == 4) {
							tree->digest32u(file, modifiedisoPath + "extent_length", true, false);
						}
						else if (length_size == 8)
						{
							tree->digest64u(file, modifiedisoPath + "extent_length", true, false);
						}
						else
						{
							cout << "wrong value of length_size received!! It should be either 0 or 4 or 8" << endl;
						}
					}

				}
			}
			//Get the data for exif - using value from exif_item_id
			// Dump the exif metadata here
			{
				if (exif_item_id != 0 && tree->hasNode("meta/iloc/item[" + to_string(exif_item_id) + "]/extent_offset"))
				{
					bool ok;
					XMP_Int64 keep = LFA_Tell(file);
					XMP_Uns32 offset = stoi(tree->getValue("meta/iloc/item[" + to_string(exif_item_id) + "]/extent_offset"));	// Here tiff stream starts after 16476 ( 16388 + 84 + 4 ) bytes from starting of PANA atom's content
																																//XMP_Uns8 *ExifSize = (XMP_Uns8*)malloc(4);
																																//Now point file to offset value
					LFA_Seek(file, offset, SEEK_SET, &ok);
					XMP_Uns32 blocksize = LFA_ReadUns32_BE(file);
					LFA_Seek(file, offset + 4 + blocksize, SEEK_SET, &ok);
					XMP_Uns64 tiffLength1 = (stoi(tree->getValue("meta/iloc/item[" + to_string(exif_item_id) + "]/extent_length")));
					XMP_Uns64 tiffLength = (stoi(tree->getValue("meta/iloc/item[" + to_string(exif_item_id) + "]/extent_length")) - blocksize - 4);
					XMP_Uns8 *tiffContent = (XMP_Uns8*)malloc(tiffLength);
					LFA_Read(file, tiffContent, tiffLength, true);
					DumpTIFF(tiffContent, tiffLength, offset, "HEIF Exif", "HEIF:Exif");
					LFA_Seek(file, keep, SEEK_SET, &ok);
					assertMsg("seek failed", ok);
					exif_item_id = NULL;
				}
			}
			//Get the data for xmp - using value from mime_item_id
			// Dump the xmp metadata here
			{
				if (mime_item_id != 0 && tree->hasNode("meta/iloc/item[" + to_string(exif_item_id) + "]/extent_offset"))
				{
					bool ok;
					XMP_Int64 keep = LFA_Tell(file);
					XMP_Uns32 offset = stoi(tree->getValue("meta/iloc/item[" + to_string(mime_item_id) + "]/extent_offset"));
					XMP_Uns64 xmpLength = stoi(tree->getValue("meta/iloc/item[" + to_string(mime_item_id) + "]/extent_length"));
					LFA_Seek(file, offset, SEEK_SET, &ok);
					XMP_Uns8 *xmpContent = (XMP_Uns8*)malloc(xmpLength);
					LFA_Read(file, xmpContent, xmpLength, true);
					DumpXMP(xmpContent, xmpLength, offset, "XMP");
					LFA_Seek(file, keep, SEEK_SET, &ok);
					assertMsg("seek failed", ok);
					mime_item_id = NULL;
				}
			}

			break;
		case 0x66657269: //iref
			digestISOFullBoxExtension(file, isoPath, remainingSize, version, flags);
			DumpISOBoxes(file, remainingSize, isoPath);
			break;
		case 0x676D6964: //dimg
			XMP_Uns16 refCount;
			tree->digest16u(file, isoPath + "from_item_ID", true, true);
			refCount = tree->digest16u(file, isoPath + "reference_count", true, true);
			for (int i = 0; i < refCount; i++)
			{
				tree->digest16u(file, isoPath + "to_item_ID", true, true);
			}
			break;
		case 0x626D6874: //thmb
			XMP_Uns16 refCount_thmb;
			tree->digest16u(file, isoPath + "from_item_ID", true, true);
			refCount_thmb = tree->digest16u(file, isoPath + "reference_count", true, true);
			for (int i = 0; i < refCount_thmb; i++)
			{
				tree->digest16u(file, isoPath + "to_item_ID", true, true);
			}
			break;
		case 0x63736463: //cdsc
			XMP_Uns16 refCount_cdsc;
			tree->digest16u(file, isoPath + "from_item_ID", true, true);
			refCount_cdsc = tree->digest16u(file, isoPath + "reference_count", true, true);
			for (int i = 0; i < refCount_cdsc; i++)
			{
				tree->digest16u(file, isoPath + "to_item_ID", true, true);
			}
			DumpISOBoxes(file, remainingSize, isoPath);
			break;
		case 0x70727069: //iprp
			DumpISOBoxes(file, remainingSize, isoPath);
			break;
			/*case 0x6F637069: //ipco
			DumpISOBoxes(file, remainingSize, isoPath);
			break;
			case 0x726c6f63: //colr
			XMP_Uns32 colour_type;
			colour_type=tree->digest32u(file, isoPath + "colour_type", true, true);
			if (colour_type == 'nclx')
			{
			tree->digest16u(file, isoPath + "colour_primaries", true, true);
			tree->digest16u(file, isoPath + "transfer_characteristics", true, true);
			tree->digest16u(file, isoPath + "matrix_coefficients", true, true);
			XMP_Uns8 full_range_flag = LFA_ReadUns8(file);
			full_range_flag = ((full_range_flag & 0xF) >> 7);
			tree->setKeyValue(isoPath + "full_range_flag", fromArgs("%d", full_range_flag));
			}
			else if (colour_type == 'rICC')
			{

			}
			else if (colour_type == 'prof')
			{
			//unrestricted icc profile handling

			}

			break;*/
		case 0x666E696D: // minf - "simple container, no direct content"
			if (boxString == "minf" && isoPath != "moov/trak/mdia/minf/")
				break;
		case 0x6C627473: // stbl is a simple container, no direct content
		{
			TimeCodeTrack = false; // assume until we known better by a relevant
								   // moov/trak/mdia/minf/stbl/stsd/ of format tmcd
			if (boxString == "stbl" && isoPath != "moov/trak/mdia/minf/stbl/")
				break;
		}
		case 0x766F6F6D: // moov
		case 0x6169646D: // mdia
		case 0x61746475: // udta - user data
		case 0x6B617274: // trak - track
		case 0x74736C69: // ilst (contains cprt box)
		{
			// store and restore current position to not depend
			// on sub-level mischief...
			if (isoPath == "moov/meta/ilst/")
			{
				while (remainingSize > 0)
				{
					XMP_Uns32 metaItemSize = tree->digest32u(file, "", true);
					XMP_Uns32 keyIndex = tree->digest32u(file, "", true);
					string key_name = ISOMetaKeys[keyIndex - 1];
					string temp_Path = isoPath + key_name + "/";
					metaItemSize -= 4 + 4;
					DumpISOBoxes(file, metaItemSize, temp_Path);
					remainingSize -= (metaItemSize + 8);
				}
			}
			else
			{
				bool ok;
				XMP_Int64 keep = LFA_Tell(file);
				DumpISOBoxes(file, remainingSize, isoPath);
				LFA_Seek(file, keep, SEEK_SET, &ok);
				assertMsg("seek failed", ok);
			}
		}
		break;

		// known boxes, that need content extraction
		case 0x70797466: // ftyp - file type
		{
			XMP_Uns32 majorBrand = LFA_ReadUns32_LE(file);
			XMP_Uns32 minorVersion = LFA_ReadUns32_LE(file);

			//data has been read in LE make it in BE
			majorBrand = GetUns32LE(&majorBrand);
			minorVersion = GetUns32LE(&minorVersion);

			//Log::info( fromArgs( "major Brand:   '%.4s' (0x%.8X)" , &majorBrand, MakeUns32BE(majorBrand) ));
			//Log::info( fromArgs( "minor Version: 0x%.8X" , MakeUns32BE(minorVersion) ) );
			tree->setKeyValue(isoPath + "majorBrand",
				fromArgs("%.4s", &majorBrand),
				fromArgs("0x%.8X", MakeUns32BE(majorBrand)));
			tree->setKeyValue(isoPath + "minorVersion",
				fromArgs("0x%.8X", MakeUns32BE(minorVersion)));

			remainingSize -= 4 + 4;
			//Log::info( fromArgs( "remaining Size: %d" , remainingSize ) );

			while (remainingSize >= 4)
			{
				LFA_ReadUns32_LE(file);
				// TODO: Concatenate for KeyValue...
				//XMP_Uns32 compatVersion = LFA_ReadUns32_LE( file );
				//Log::info( fromArgs( "compatible brand: '%.4s' (0x%.8X)" , &compatVersion, MakeUns32BE(compatVersion) ));
				remainingSize -= 4;
			}

			// odd bytes left?
			if (remainingSize > 0)
				tree->addComment("WARNING: %d bytes left, considering FREE", remainingSize);

		}
		break;
		case 0x61746164: // data (within itunes Metadata)
						 // all data atoms start with two common fields: a type indicator, and a locale indicator.
						 // each of these fields is four bytes long:
			tree->digest32u(file, isoPath + "type", true, true);
			tree->digest32u(file, isoPath + "locale", true, true);
			remainingSize -= 8;
			// rest is actual contents:
			tree->digestString(file, isoPath + "value", remainingSize, false);
			break;
		case 0x64697575: // uuid
			XMP_Uns8 uid[16];
			tree->digest(file, isoPath + "uuidValue", uid, 16);
			if (!strncmp((const char*)kUUID_XMP, (const char*)uid, 16))
				tree->addComment(" - the XMP UUID !");

			break;

		case 0x65657266: // free
			tree->addComment("free space");
			break;

			// FULL BOXES (w/o container boxes, above) **********************************
		case 0x6468766D: // mvhd, FULLBOX, movie-header-box
		{
			digestISOFullBoxExtension(file, isoPath, remainingSize, version, flags);
			if (version == 1)
			{
				tree->digest64u(file, isoPath + "creation_time", true, true);
				tree->digest64u(file, isoPath + "modification_time", true, true);
				tree->digest32u(file, isoPath + "timescale", true, true);
				tree->digest64u(file, isoPath + "duration", true, true);
			}
			else if (version == 0)
			{
				tree->digest32u(file, isoPath + "creation_time", true, true);
				tree->digest32u(file, isoPath + "modification_time", true, true);
				tree->digest32u(file, isoPath + "timescale", true, true);
				tree->digest32u(file, isoPath + "duration", true, true);
			}
			else
			{
				tree->addComment("WARNING: unknown mvhd version!");
			}
			// COULDDO more fields, but not needed right now.
		}
		break;
		case 0x726C6468: // hdlr - handler reference
		{
			if (isoPath == "moov/trak/mdia/hdlr/")
			{
				if (remainingSize < 4 * 4)
					break; // box too small...

				digestISOFullBoxExtension(file, isoPath, remainingSize, version, flags);

				tree->digestString(file, isoPath + "quickTimeType", 4, false);	// expecting: 'mhlr' - media handler
				tree->digestString(file, isoPath + "subType", 4, false);		// expecting: 'tmcd' - timecode
				tree->digestString(file, isoPath + "manufacturer", 4, false);		// e.g. 'appl'
				break;
			}
			else if (isoPath == "moov/meta/hdlr/")
			{
				assertMsg("hdlr: version and flags must be zero", 0 == tree->digest32u(file, "", true));
				LFA_Seek(file, 4, SEEK_CUR);

				XMP_Uns32 hndlrTyp = LFA_ReadUns32_BE(file);
				if (hndlrTyp == 0x6D647461)
					tree->addComment("Handler Type = mdta");
				else
					break;

				remainingSize -= 12;
				XMP_Uns32 pos = LFA_Tell(file);
				LFA_Seek(file, pos + remainingSize, SEEK_SET);
				break;
			}
			else if (isoPath == "meta/hdlr/")
			{
				digestISOFullBoxExtension(file, isoPath, remainingSize, version, flags);
				LFA_Seek(file, 4, SEEK_CUR);
				tree->digestString(file, isoPath + "handler_type", 4, false);
				break;
			}
			// rest doesn't bother us...
		}
		case 0x64737473: // stsd - timecode sample description table
		{
			if (isoPath != "moov/trak/mdia/minf/stbl/stsd/")
				break;

			// version (1 byte), flags (3 byte) - must be 0
			assertMsg("stbl: version and flags must be zero", 0 == tree->digest32u(file, "", true));
			// entryCount - must be 0
			assertMsg("stbl: at least one entry needed", 1 <= tree->digest32u(file, "", true));
			remainingSize -= 8;

			// only dump first occurence 
			// ensure there's enough bytes for at least on stbl:
			if (remainingSize < 29)
				break;	// MPEG4/al_sbr_twi_22_1_fsaac22.mv4 has a box that is smaller than this...
						// hence can only break, not throw
			XMP_Int64 entrySize = (XMP_Int64)tree->digest32u(file, "", true, true);
			std::string format = tree->digestString(file, "", 4, false); // must be 'tmcd'
			tree->pushNode(isoPath + format + "/");
			if (format != "tmcd")
			{
				tree->addComment("irrelevant node");
				tree->popNode();
				break; // different party...
			}

			TimeCodeTrack = true; // we're in the right track

			Skip(file, 6); // [6] reserved bytes
			tree->digest16u(file, isoPath + "dataReferenceIndex", true, true); // (ignored)
			tree->addOffset(file);
			Skip(file, 4); // uint32 reserved
			tree->digest32u(file, isoPath + "flags", true, true);
			tree->digest32u(file, isoPath + "timeScale", true, true);
			tree->digest32u(file, isoPath + "frameDuration", true, true);
			Skip(file, 2); // skip ignored frame count, reserved

						   // ////////////////////////////// dig out 'trailing boxes'
						   // comparing "atom remains" vs. "entry" (probably must be '>='

						   // deduct the already digested...
			entrySize -= 34; // ( 4+4+6+2+4+4+4+4+1+1 )
			remainingSize -= 34; // (the atom-level value)

			assertMsg("entry Size must be 0 or positive", entrySize >= 0);
			assertMsg("must not overreach atom", entrySize <= remainingSize);

			XMP_Int64 endOfTrailingBoxes = LFA_Tell(file) + remainingSize;
			while (LFA_Tell(file) < endOfTrailingBoxes)
			{
				LFA_Tell(file);
				DumpISOBoxes(file, entrySize, isoPath);
				LFA_Tell(file);
			}

			assertMsg("did not boil down to zero", LFA_Tell(file) == endOfTrailingBoxes);

			tree->popNode();
			break;
		}

		case 0x63737473: // stsc - timecode sample description table
		{
			if (isoPath != "moov/trak/mdia/minf/stbl/stsc/")
				break;
			if (!TimeCodeTrack)
			{
				tree->addComment("not tcmd -> not of interest");
				break; // not of interest
			}

			// version (1 byte), flags (3 byte) - must be 0
			assertMsg("stbl: version and flags must be zero", 0 == tree->digest32u(file, "", true));
			// entryCount - must be 0
			assertMsg("stbl: at least one entry needed", 1 <= tree->digest32u(file, "", true));
			remainingSize -= 8;

			LFA_Tell(file);

			tree->digest32u(file, isoPath + "firstChunkNo", true, true);
			tree->digest32u(file, isoPath + "numSamplesPerChunk", true, true);
			tree->digest32u(file, isoPath + "sampleDescriptionID", true, true);

			break;
		}

		case 0x6F637473: // stco - timecode sample description table
		{
			if (isoPath != "moov/trak/mdia/minf/stbl/stco/")
				break;
			if (!TimeCodeTrack)
				break; // not of interest

					   // version (1 byte), flags (3 byte) - must be 0
			assertMsg("stbl: version and flags must be zero", 0 == tree->digest32u(file, "", true));
			// entryCount - must be 0
			assertMsg("stbl: at least one entry needed", 1 <= tree->digest32u(file, "", true));
			remainingSize -= 8;

			XMP_Int64 absOffset = tree->digest32u(file, isoPath + "absFileOffset32", true, true);

			// recklessly navigate to that timecode media sample, grab value, return to old position...
			XMP_Int64 oldPos = LFA_Tell(file);
			LFA_Seek(file, absOffset, SEEK_SET, 0);
			tree->digest32u(file, isoPath + "timecodeMediaSample", true, true);
			LFA_Seek(file, oldPos, SEEK_SET, 0);

		}
		case 0x34366F63: // co64 - timecode sample description table -> 64 bit offset
		{
			if (isoPath != "moov/trak/mdia/minf/stbl/co64/")
				break;

			tree->digest64u(file, isoPath + "absFileOffset64", true, true);
			break;
		}

		case 0x74727063: // cprt, FULLBOX
			if (isoPath == "moov/udta/cprt/" || isoPath == "moov/uuid/udta/cprt/")
			{

				digestISOFullBoxExtension(file, isoPath, remainingSize, version, flags);

				// 1/8 byte ISO language padding = 1-bit value set to 0
				// 1 7/8 bytes content language = 3 * 5-bits ISO 639-2 language code less 0x60
				// - example code for english = 0x15C7
				tree->digest16u(file, isoPath + "content language", true, true);
				tree->addComment("(0x15C7 == english)");
				// zero-terminated, actual string:
				tree->digestString(file, isoPath + "value", 0);
			}
			else
			{
				// ISO - copyright (?)					
				// a container box, hunt for 'data' atom by recursion:
				bool ok;
				XMP_Int64 keep = LFA_Tell(file);
				DumpISOBoxes(file, remainingSize, isoPath);
				LFA_Seek(file, keep, SEEK_SET, &ok);
				assertMsg("seek failed", ok);
			}
			break;
		case 0x64686B74: // tkhd, FULLBOX
		case 0x6E61656D: // mean, FULLBOX
		case 0x656D616E: // name, FULLBOX
		{
			if (isoPath == "moov/trak/mdia/minf/stbl/stsd/name/") // this regrettably is a diffrent animal (international text sequence)
				digestInternationalTextSequence(file, isoPath, &remainingSize);
			else
				digestISOFullBoxExtension(file, isoPath, remainingSize, version, flags);
		}
		break;
		case 0x7379656B:    //moov/meta/keys 
		{
			LFA_Seek(file, 4, SEEK_CUR);
			XMP_Uns32 keysCount = tree->digest32u(file, "", true);
			tree->addComment("Number of Keys = %d", keysCount);
			XMP_Uns32 index;
			for (index = 1; index <= keysCount; index++)
			{
				XMP_Uns32 keySize = tree->digest32u(file, "", true);
				keySize -= 4 + 4;  // 4 bytes of key_size and key_namespace
				string key_namespace = tree->digestString(file, "", 4);
				if ((strcmp(key_namespace.c_str(), "mdta") == 0) || (strcmp(key_namespace.c_str(), "udta") == 0))
				{
					string key_value = tree->digestString(file, "", keySize);
					ISOMetaKeys.push_back(key_value);
					tree->pushNode(isoPath + key_value);
					tree->addComment("Key Namespace = %s", key_namespace.c_str());
					tree->popNode();
				}
				else
					LFA_Seek(file, keySize, SEEK_CUR);
			}
		}
		break;
			// (c)-style quicktime boxes and boxes of no interest:
		default:
			if ((boxType & 0xA9) == 0xA9) // (c)something
			{
					if (0 == isoPath.compare(0, 10, "moov/udta/"))
					{ // => Quicktime metadata "international text sequence" ( size, language code, value )
						digestInternationalTextSequence(file, isoPath, &remainingSize);
					}
					else
					{
						tree->addComment("WARNING: unknown flavor of (c)*** boxes, neither QT nor iTunes");
					}
				break;
			}
			//boxes of no interest:
			break;
		}

		bool ok;
		LFA_Seek(file, endOfBoxPos, SEEK_SET, &ok);
		assertMsg("End-of-Box Seek failed (truncated file?)", ok);

		tree->popNode();

	} // while

} // DumpISOBoxes()

  // attempt to combine dumping of mpeg-4 and quicktime (mov) into one routine...
static void
DumpISO(LFA_FileRef file, XMP_Uns32 fileLen)
{
	TimeCodeTrack = false;

	// see specificition at https://zerowing.corp.adobe.com/display/XMP/Embedding+Spec+MPEG4
	DumpISOBoxes(file, fileLen, "");
	assertMsg("truncated file/last box reached beyond end?", LFA_Tell(file) == fileLen);
}

// =================================================================================================

static size_t GetASFObjectInfo(LFA_FileRef file, XMP_Uns32 objOffset, ASF_ObjHeader* objHeader, size_t nesting)
{
	LFA_Seek(file, objOffset, SEEK_SET);
	LFA_Read(file, objHeader, 24, true);

	objHeader->size = GetUns64LE(&objHeader->size);
	XMP_Uns32 size32 = (XMP_Uns32)objHeader->size;

	if (objHeader->size > 0xFFFFFFFF) {
		tree->addComment("** ASF Object at offset 0x%X is over 4GB: 0x%.8X 0x%.8X",
			objOffset, High32(objHeader->size), Low32(objHeader->size));
	}

	size_t infoIndex;
	for (infoIndex = 0; kASF_KnownObjects[infoIndex].name != 0; ++infoIndex) {
		if (memcmp(&objHeader->guid, &kASF_KnownObjects[infoIndex].guid, 16) == 0) break;
	}

	std::string indent(3 * nesting, ' ');

	if (kASF_KnownObjects[infoIndex].name != 0) {
		tree->addComment("%s   %s Object, offset %u (0x%X), size %u",
			indent.c_str(), kASF_KnownObjects[infoIndex].name, objOffset, objOffset, size32);
	}
	else {
		tree->addComment("%s   <<unknown object>>, offset %u (0x%X), size %u",
			indent.c_str(), objOffset, objOffset, size32);
		ASF_GUID guid;
		guid.part1 = GetUns32LE(&objHeader->guid.part1);
		guid.part2 = GetUns16LE(&objHeader->guid.part2);
		guid.part3 = GetUns16LE(&objHeader->guid.part3);
		guid.part4 = GetUns16LE(&objHeader->guid.part4);
		tree->addComment("GUID %.8X-%.4X-%.4X-%.4X-%.4X%.8X",
			guid.part1, guid.part2, guid.part3, guid.part4,
			*(XMP_Uns16*)(&guid.part5[0]), *(XMP_Uns32*)(&guid.part5[2]));
	}

	if (objOffset != 0) tree->addComment("");	// Don't print newline for the real header.

	return infoIndex;

}	// GetASFObjectInfo

	// =================================================================================================

static void PrinfASF_UTF16(LFA_FileRef file, XMP_Uns16 byteCount, const char * label)
{
	size_t filePos = LFA_Tell(file);
	//FNO: note: has sideeffect on sDataPtr
	CaptureFileData(file, 0, byteCount);

	tree->setKeyValue(
		label,
		convert16Bit(false, sDataPtr, false, byteCount),
		fromArgs("offset %d (0x%X), size %d", filePos, filePos, byteCount)
	);
}

static void DumpASFFileProperties(LFA_FileRef file, XMP_Uns32 objOffset, XMP_Uns32 objLen)
{
	ASF_FileProperties fileProps;

	if (objLen < kASF_FilePropertiesSize) {
		tree->comment("** File Properties Object is too short");
		return;
	}

	LFA_Seek(file, objOffset, SEEK_SET);
	LFA_Read(file, &fileProps, kASF_FilePropertiesSize, true);

	fileProps.flags = GetUns32LE(&fileProps.flags);	// Only care about flags and create date.
	fileProps.creationDate = GetUns64LE(&fileProps.creationDate);

	bool bcast = (bool)(fileProps.flags & 1);
	tree->setKeyValue("ASF:broadcast",
		(bcast ? "set" : "not set")
	);

	XMP_Int64 totalSecs = fileProps.creationDate / (10 * 1000 * 1000);
	XMP_Int32 nanoSec = ((XMP_Int32)(fileProps.creationDate - (totalSecs * 10 * 1000 * 1000))) * 100;
	XMP_Int32 days = (XMP_Int32)(totalSecs / 86400);
	totalSecs -= ((XMP_Int64)days * 86400);
	XMP_Int32 hour = (XMP_Int32)(totalSecs / 3600);
	totalSecs -= ((XMP_Int64)hour * 3600);
	XMP_Int32 minute = (XMP_Int32)(totalSecs / 60);
	totalSecs -= ((XMP_Int64)minute * 60);
	XMP_Int32 second = (XMP_Int32)totalSecs;
	XMP_DateTime binDate;
	memset(&binDate, 0, sizeof(binDate));

	binDate.year = 1601;
	binDate.month = 1;
	binDate.day = 1;

	binDate.day += days;
	binDate.hour = hour;
	binDate.minute = minute;
	binDate.second = second;
	binDate.nanoSecond = nanoSec;

	SXMPUtils::ConvertToUTCTime(&binDate);
	std::string strDate;
	SXMPUtils::ConvertFromDate(binDate, &strDate);

	tree->setKeyValue("ASF:creation date",
		fromArgs("%s (0x%.8X-%.8X)",
			strDate.c_str(), High32(fileProps.creationDate), Low32(fileProps.creationDate))
	);
}	// DumpASFFileProperties

	// =================================================================================================

static void DumpASFContentDescription(LFA_FileRef file, XMP_Uns32 objOffset, XMP_Uns32 objLen)
{
	ASF_ContentDescription contentDesc;	// ! The lengths are in bytes.

	if (objLen < kASF_ContentDescriptionSize) {
		tree->comment("** Content Description Object is too short");
		return;
	}

	LFA_Seek(file, objOffset, SEEK_SET);
	LFA_Read(file, &contentDesc, kASF_ContentDescriptionSize, true);

	contentDesc.titleLen = GetUns16LE(&contentDesc.titleLen);
	contentDesc.authorLen = GetUns16LE(&contentDesc.authorLen);
	contentDesc.copyrightLen = GetUns16LE(&contentDesc.copyrightLen);
	contentDesc.descriptionLen = GetUns16LE(&contentDesc.descriptionLen);
	contentDesc.ratingLen = GetUns16LE(&contentDesc.ratingLen);

	PrinfASF_UTF16(file, contentDesc.titleLen, "ASF:title");
	PrinfASF_UTF16(file, contentDesc.authorLen, "ASF:author");
	PrinfASF_UTF16(file, contentDesc.copyrightLen, "ASF:copyright");
	PrinfASF_UTF16(file, contentDesc.descriptionLen, "ASF:description");
	PrinfASF_UTF16(file, contentDesc.ratingLen, "ASF:rating");

}	// DumpASFContentDescription

	// =================================================================================================

static void DumpASFContentBranding(LFA_FileRef file, XMP_Uns32 objOffset, XMP_Uns32 objLen)
{
	XMP_Uns32 fieldSize;

	if (objLen < (16 + 8 + 4 * 4)) {
		tree->comment("** Content Branding Object is too short");
		return;
	}

	XMP_Uns32 fieldOffset = objOffset + 16 + 8 + 4;
	LFA_Seek(file, fieldOffset, SEEK_SET);

	LFA_Read(file, &fieldSize, 4, true);
	fieldSize = GetUns32LE(&fieldSize);
	fieldOffset += fieldSize;
	LFA_Seek(file, fieldSize, SEEK_CUR);	// Skip the banner data.

	LFA_Read(file, &fieldSize, 4, true);
	fieldSize = GetUns32LE(&fieldSize);
	fieldOffset += fieldSize;
	tree->setKeyValue("ASF:banner URL",
		"",
		fromArgs("offset %d (0x%X), size %d", fieldOffset, fieldOffset, fieldSize)
	);

	if (fieldSize != 0) {
		CaptureFileData(file, 0, fieldSize);
		//NB: not yet tested..., not sure if stopOnNull needed, thus using false
		tree->changeValue(convert8Bit(sDataPtr, false, fieldSize));
	}

	LFA_Read(file, &fieldSize, 4, true);
	fieldSize = GetUns32LE(&fieldSize);
	fieldOffset += fieldSize;
	tree->setKeyValue("ASF:copyright URL",
		"",
		fromArgs("offset %d (0x%X), size %d", fieldOffset, fieldOffset, fieldSize)
	);

	if (fieldSize != 0) {
		CaptureFileData(file, 0, fieldSize);
		//NB: not yet tested..., not sure if stopOnNull needed, thus using false
		tree->changeValue(convert8Bit(sDataPtr, false, fieldSize));
	}

}	// DumpASFContentBranding

	// =================================================================================================

static void DumpASFContentEncryption(LFA_FileRef file, XMP_Uns32 objOffset, XMP_Uns32 objLen)
{
	XMP_Uns32 fieldSize;

	if (objLen < (16 + 8 + 4 * 4)) {
		tree->addComment("** Content Encryption Object is too short");
		return;
	}

	LFA_Seek(file, (objOffset + 16 + 8), SEEK_SET);

	LFA_Read(file, &fieldSize, 4, true);
	fieldSize = GetUns32LE(&fieldSize);
	LFA_Seek(file, fieldSize, SEEK_CUR);	// Skip the secret data.

	LFA_Read(file, &fieldSize, 4, true);
	fieldSize = GetUns32LE(&fieldSize);
	LFA_Seek(file, fieldSize, SEEK_CUR);	// Skip the protection type.

	LFA_Read(file, &fieldSize, 4, true);
	fieldSize = GetUns32LE(&fieldSize);
	CaptureFileData(file, 0, fieldSize);
	PrintOnlyASCII_8(sDataPtr, fieldSize);

	LFA_Read(file, &fieldSize, 4, true);
	fieldSize = GetUns32LE(&fieldSize);
	CaptureFileData(file, 0, fieldSize);
	PrintOnlyASCII_8(sDataPtr, fieldSize);

}	// DumpASFContentEncryption

	// =================================================================================================

static void DumpASFHeaderExtension(LFA_FileRef file, XMP_Uns32 extOffset, XMP_Uns32 extLen)
{
	// The Header Extension Object is a child of the Header Object and the parent of nested objects.

	XMP_Uns32 extEnd = extOffset + extLen;

	XMP_Uns32 childLen;
	XMP_Uns32 childOffset;

	ASF_ObjHeader childHeader;

	for (childOffset = (extOffset + kASF_HeaderExtensionSize); childOffset < extEnd; childOffset += childLen) {
		(void)GetASFObjectInfo(file, childOffset, &childHeader, 2);
		childLen = (XMP_Uns32)childHeader.size;
	}

	if (childOffset != extEnd) {
		tree->addComment("** Invalid end to nested Header Extension objects, offset %u", childOffset);
	}

}	// DumpASFHeaderExtension

	// =================================================================================================

static void
DumpASF(LFA_FileRef file, XMP_Uns32 asfLen)
{
	// An ASF file contains objects of the form:
	//   A 16 byte GUID giving the object's type and use
	//   A little endian 64-bit object length, includes the GUID and length (== 24 + data size)
	//   The object's data
	// Objects can be nested. The top level of a file is a Header Object, followed by a Data Object,
	// followed by any number of "other" top level objects, followed by any number of index objects.
	// There is legacy metadata in certain nested objects within the Header Object. The XMP is an
	// "other" top level object.

	size_t infoIndex;

	XMP_Uns32  objLen;
	XMP_Uns32  objOffset;
	ASF_ObjHeader objHeader;

	tree->comment("ASF file Object layout");

	// Dump the Header Object's content, looking for legacy metadata.
	infoIndex = GetASFObjectInfo(file, 0, &objHeader, 0);
	XMP_Uns32 headerLen = (XMP_Uns32)objHeader.size;

	if (kASF_KnownObjects[infoIndex].kind != kASFObj_Header) {
		tree->comment("** First object is not the Header Object");
		return;
	}

	XMP_Uns32 nestedCount;
	LFA_Seek(file, 24, SEEK_SET);
	LFA_Read(file, &nestedCount, 4, true);
	nestedCount = GetUns32LE(&nestedCount);
	tree->addComment("%u nested objects", nestedCount);

	for (objOffset = (16 + 8 + 4 + 2); objOffset < headerLen; objOffset += objLen, --nestedCount) {
		infoIndex = GetASFObjectInfo(file, objOffset, &objHeader, 1);
		objLen = (XMP_Uns32)objHeader.size;

		switch (kASF_KnownObjects[infoIndex].kind) {
		case kASFObj_FileProperties:
			DumpASFFileProperties(file, objOffset, objLen);
			break;
		case kASFObj_ContentDesc:
			DumpASFContentDescription(file, objOffset, objLen);
			break;
		case kASFObj_ContentBrand:
			DumpASFContentBranding(file, objOffset, objLen);
			break;
		case kASFObj_ContentEncrypt:
			DumpASFContentEncryption(file, objOffset, objLen);
			break;
		case kASFObj_HeaderExtension:
			DumpASFHeaderExtension(file, objOffset, objLen);
			break;
		default:
			break;
		}
	}

	if ((objOffset != headerLen) || (nestedCount != 0)) {
		tree->comment("** Invalid end to nested Header objects, offset %u, count %u",
			objOffset, nestedCount);
		objOffset = headerLen;
	}

	// Dump the basic info for the remaining objects, looking for the XMP along the way.
	infoIndex = GetASFObjectInfo(file, objOffset, &objHeader, 0);
	objLen = (XMP_Uns32)objHeader.size;

	if (kASF_KnownObjects[infoIndex].kind != kASFObj_Data) {
		tree->addComment("** Second object is not the Data Object");
		if (kASF_KnownObjects[infoIndex].kind == kASFObj_XMP) {
			if (sXMPPtr == 0)
				CaptureXMPF(file, (objOffset + 24), (objLen - 24));
			else
				tree->addComment("** Multiple XMP objects");
		}
	}

	for (objOffset += objLen; objOffset < asfLen; objOffset += objLen) {
		GetASFObjectInfo(file, objOffset, &objHeader, 0);
		objLen = (XMP_Uns32)objHeader.size;

		if (kASF_KnownObjects[infoIndex].kind == kASFObj_XMP) {
			if (sXMPPtr == 0)
				CaptureXMPF(file, (objOffset + 24), (objLen - 24));
			else
				tree->addComment("** Multiple XMP objects");
		}
	}

	if (objOffset != asfLen) tree->addComment("** Invalid end to top level objects: %u", objOffset);
	if (sXMPPtr != 0) DumpXMP("ASF XMP object");

}	// DumpASF

	// =================================================================================================

static void
DumpUCF(LFA_FileRef file, XMP_Int64 len)
{
	//////////////////////////////////////////////////////////////////////
	// constants
	const static XMP_Uns32 UCF_HS_CONTENTFILE = 0x04034b50;
	const static XMP_Uns32 UCF_CD_FILE_HEADER = 0x02014b50;		//central directory - file header
	const static XMP_Uns32 UCF_ZIP64_END_OF_CD_RECORD = 0x06064b50;
	const static XMP_Uns32 UCF_ZIP64_END_OF_CD_LOCATOR = 0x07064b50;
	const static XMP_Uns32 UCF_CD_END = 0x06054b50;

	const static XMP_Int32 UCF_COMMENT_MAX = 0xFFFF;
	const static XMP_Int32 UCF_EOD_FIXED_SIZE = 22;

	const static XMP_Int32 UCF_ZIP64_LOCATOR_FIXED_SIZE = 20;
	const static XMP_Int32 UCF_ZIP64_RECORD_FIXED_SIZE = 56;

	//////////////////////////////////////////////////////////////////////
	// variables:
	XMP_Int64 curPos = 0;
	bool isZip64;

	XMP_Uns32 numDirEntries;

	XMP_Uns64 size_CD;
	XMP_Uns64 offset_Zip64_EndOfCD_Record = 0;
	XMP_Uns64 offset_CD;

	typedef std::list<XMP_Int64> OffsetStack;
	OffsetStack contentFiles;
	contentFiles.clear(); //precaution for mac

						  //////////////////////////////////////////////////////////////////////
						  // prolog:
	tree->comment("len is 0x%I64X", len);
	tree->comment("inherently parsing bottom-up");
	if (len > 0xFFFFFFFFl)
		tree->comment("info: >4GB ==> most like zip64 !");

	//////////////////////////////////////////////////////////////////////
	// parse bottom up:

	/////////////////////////////////////////////////////////////////////
	// zip comment:
	XMP_Int32 zipCommentLen = 0;
	for (; zipCommentLen <= UCF_COMMENT_MAX; zipCommentLen++)
	{
		LFA_Seek(file, -zipCommentLen - 2, SEEK_END);
		if (LFA_ReadUns16_LE(file) == zipCommentLen)	//found it?
		{
			//double check, might just look like comment length (actually be 'evil' comment)
			LFA_Seek(file, -UCF_EOD_FIXED_SIZE, SEEK_CUR);
			if (LFA_ReadUns32_LE(file) == UCF_CD_END) break; //heureka, directory ID
															 // 'else': just go on
		}
	}
	tree->comment(fromArgs("zip Comment length: %d", zipCommentLen));

	//was it a break or just not found ?
	assertMsg("zip broken near end or invalid comment", zipCommentLen < UCF_COMMENT_MAX);

	/////////////////////////////////////////////////////////////////////
	// End of CDR:
	LFA_Seek(file, -UCF_EOD_FIXED_SIZE - zipCommentLen, SEEK_END);
	curPos = LFA_Tell(file);
	tree->pushNode("End of Central Directory");
	tree->addOffset(file);
	{
		assertMsg("expected 'end of central directory record'", UCF_CD_END == tree->digest32u(file));
		assertMsg("UCF allow single-volume zips only", 0 == tree->digest16u(file)); //volume number (0,1,..)
		assertMsg("UCF allow single-volume zips only(thus directory must be in 0)", 0 == tree->digest16u(file, "")); //volume number (0,1,..)

		numDirEntries = tree->digest16u(file, "number of directory entries");
		tree->digest16u(numDirEntries, file, "number of total directory entries");

		size_CD = tree->digest32u(file, "size of central directory", false, true);
		offset_CD = tree->digest32u(file, "offset of central directory", false, true);
		if (offset_CD == 0xFFFFFFFF) tree->addComment("apparently zip-64");

		XMP_Uns16 zipCommentLengReverify = tree->digest16u(file, "zip comment length");
		assertMsg("zipCommentLengReverify failed", zipCommentLengReverify == zipCommentLen);
	}
	tree->popNode();
	/////////////////////////////////////////////////////////////////////
	// Zip64 End Of CD Locator
	LFA_Seek(file, curPos - UCF_ZIP64_LOCATOR_FIXED_SIZE, SEEK_SET);

	//tree->comment("offset is %X", LFA_Tell(file) );
	//tree->comment("peek is %X", Peek32u(file) );

	if (Peek32u(file) != UCF_ZIP64_END_OF_CD_LOCATOR)
	{
		tree->comment("no Zip64 CDL -> no Zip64");
		assertMsg("offset FFFF FFFF indicates zip-64, but no Zip64 CDL found", offset_CD != 0xFFFFFFFF);
		isZip64 = false;
	}
	else
	{
		isZip64 = true;
		tree->pushNode("Zip64 End-Of-CD Locator");
		tree->addOffset(file);

		tree->digest32u(file, "sig", false, true);
		assertMsg("'numOfDisk with central start dir' must be 0",
			0 == tree->digest32u(file, "disk with start dir"));
		tree->digest64u(&offset_Zip64_EndOfCD_Record, file, "Zip64 End Of CD Offset", false, true);

		tree->digest32u( /* deactived while bug #1742179:   1,*/ file, "total num of disks", false, true);
		tree->popNode();
	}

	/////////////////////////////////////////////////////////////////////
	// Zip64 End of CD Record
	if (isZip64)
	{
		XMP_Uns64 size_Zip64_EndOfCD_Record;
		tree->pushNode("Zip64 End of CD Record");
		LFA_Seek(file, offset_Zip64_EndOfCD_Record, SEEK_SET);
		tree->addOffset(file);

		tree->digest32u(UCF_ZIP64_END_OF_CD_RECORD, file, "sig", false, true);
		tree->digest64u(&size_Zip64_EndOfCD_Record, file, "size of zip64 CDR", false, true);
		tree->digest16u(file, "made by", false, true);
		tree->digest16u(file, "needed to extract", false, true);
		tree->digest32u((XMP_Uns32)0, file, "number of this disk", false, true);
		tree->digest32u((XMP_Uns32)0, file, "disk that contains start of CD", false, true);
		tree->digest64u((XMP_Uns64)numDirEntries, file, "total Num of Entries This Disk", false, false);
		tree->digest64u((XMP_Uns64)numDirEntries, file, "total Num of Entries", false, false);
		//TODO assert agtainst each other and above
		tree->digest64u(&size_CD, file, "size_CD", false, true);
		tree->digest64u(&offset_CD, file, "offset_CD", false, true);
		XMP_Int64 lenExtensibleSector = UCF_ZIP64_RECORD_FIXED_SIZE - size_Zip64_EndOfCD_Record;
		tree->comment("zip64 extensible data sector (%d bytes)", lenExtensibleSector);
		//sanity test:
		Skip(file, lenExtensibleSector);
		assertMsg("numbers don't add up", Peek32u(file) != UCF_ZIP64_END_OF_CD_LOCATOR);

		tree->popNode();
	}

	/////////////////////////////////////////////////////////////////////
	// parse Central directory structure: content file 1..n
	tree->pushNode("Central directory structure:");
	LFA_Seek(file, offset_CD, SEEK_SET);
	tree->addOffset(file);

	for (XMP_Uns32 contentFileNo = 1; contentFileNo <= numDirEntries; contentFileNo++)
	{
		tree->pushNode("File Header No %d:", contentFileNo);
		tree->addOffset(file);

		XMP_Uns16 version, flags, cmethod;
		XMP_Uns32 crc, compressed_size, uncompressed_size32;
		XMP_Uns64 offsetLocalHeader = 0;
		bool usesDescriptionHeader;

		tree->digest32u(UCF_CD_FILE_HEADER, file, "sig", false, true);

		tree->digest16u(file, "version made by", false, true);
		tree->digest16u(&version, file, "version needed to extract");
		assertMsg(fromArgs("illegal 'version needed to extract' (must be 10,20 or 45, was %u)", version),
			(version == 10 || version == 20 || version == 45));

		tree->digest16u(&flags, file, "general purpose bit flags", false, true);
		assertMsg("no zip encryption must be used", (flags & 0x1) == 0);
		usesDescriptionHeader = ((flags & 0x8) != 0);
		if (usesDescriptionHeader) tree->addComment("uses description header");

		tree->digest16u(&cmethod, file, "compression method");
		assertMsg("illegal compression method (must be 0 or 8(flate))!", (cmethod == 0 || cmethod == 8));

		tree->digest(file, "last mod file time", 0, 2);
		tree->digest(file, "last mod file date", 0, 2);

		tree->digest32u(&crc, file); //crc-32
		tree->digest32u(&compressed_size, file, "compressed size");
		tree->digest32u(&uncompressed_size32, file, "uncompressed size");

		XMP_Uns16 size_filename, size_extra, size_comment;

		tree->digest16u(&size_filename, file, "size filename");
		assertMsg("unusual name length length (broken file?)", size_filename>0 && size_filename < 500); //discover parsing nonsense...
		tree->digest16u(&size_extra, file, "size extra field");
		tree->digest16u(&size_comment, file, "size file comment");
		tree->digest16u((XMP_Uns16)0, file, "disk start no");

		tree->digest16u(file, "internal attribs");
		tree->digest32u(file, "external attribs");

		offsetLocalHeader = tree->digest32u(file, "relative offset local header", false, true); // Int64 <== Uns32

																								// name of file, optional relative path, strictly forward slashes.
		assert(size_filename != 0);
		std::string filename = tree->digestString(file, "filename", size_filename); //NOT zero-terminated

		if (contentFileNo == 1)
		{
			assert(size_extra == 0); //spec guarantes mimetype content at 38 <=> extraFieldLen == 0
			assertMsg(
				fromArgs("first file in UCF must be called mimetype, was %s", filename.c_str()),
				(size_filename == 8) && (filename == "mimetype"));
		}

		if (size_extra != 0)
		{
			tree->pushNode("extraField");
			XMP_Int32 remaining = size_extra;
			while (remaining > 0)
			{
				assertMsg("need 4 bytes for next header ID+len", remaining >= 4);
				XMP_Uns16 headerID = tree->digest16u(file, "headerID", false, true);
				XMP_Uns16 dataSize = tree->digest16u(file, "data size", false, true);
				remaining -= 4;
				assertMsg("actual field lenght not given", remaining >= dataSize);
				if (headerID == 0x1) //we only care about "Zip64 extended information extra field"
				{
					tree->digest64u(&offsetLocalHeader, file, "64bit offset", false, true);
					remaining -= 8;
				}
				else
				{
					Skip(file, dataSize);
					remaining -= dataSize;
				}

			}
			tree->popNode();
		}

		//now that regular 32 bit and zip-64 is through...
		if (contentFileNo == 1)
		{
			assertMsg("first header offset (aka content file offset) must (naturally) be 0", offsetLocalHeader == 0);
		}
		else
		{
			assertMsg("local header offset (aka content file offset) must not be 0", offsetLocalHeader != 0);
		}
		contentFiles.push_back(offsetLocalHeader);

		if (size_comment != 0)
		{
			tree->digest(file, "file comment", 0, size_comment);
		}
		tree->popNode(); //file header
	}
	tree->popNode(); //central directory structure

					 /////////////////////////////////////////////////////////////////////
					 // Content Files (incl. Headers, etc)
	for (XMP_Uns16 cfNo = 1; cfNo <= numDirEntries; cfNo++)
	{
		//vars
		XMP_Uns32 compressed_size, uncompressed_size, crc32;
		XMP_Uns16 version, nameLen, extraFieldLen;

		tree->pushNode("Content File %d:", cfNo);

		assert(!contentFiles.empty());
		XMP_Int64 fileHeaderOffset = contentFiles.front();
		contentFiles.pop_front();

		bool ok;
		LFA_Seek(file, fileHeaderOffset, SEEK_SET, &ok);
		tree->addOffset(file);
		assert(ok);

		//local file header
		tree->digest32u(UCF_HS_CONTENTFILE, file, "sig", false, true);
		tree->digest16u(&version, file, "version");
		assertMsg("illegal 'version needed to extract' (must be 10,20 or 45, was %u)",
			(version == 10 || version == 20 || version == 45));
		Skip(file, 8);
		tree->digest32u(&crc32, file, "crc-32", false, true);
		tree->digest32u(&compressed_size, file, "compressed", false, false);
		tree->digest32u(&uncompressed_size, file, "uncompressed", false, false);
		tree->digest16u(&nameLen, file, "file name length", false, false);
		tree->digest16u(&extraFieldLen, file, "extra field length", false, false);

		assert(nameLen != 0);
		assertMsg("unusual name length length (broken file?)", nameLen>0 && nameLen < 500); //discover parsing nonsense...

																							// name of file, optional relative path, strictly forward slashes.
		std::string filename = tree->digestString(file, "filename", nameLen); //NOT zero-terminated
		if (cfNo == 1)
		{
			assertMsg("first file in UCF muste be called mimetype", filename == "mimetype");
			assert(extraFieldLen == 0); //spec guarantes mimetype content at 38 <=> extraFieldLen == 0
			assert(LFA_Tell(file) == 38);
			tree->digestString(file, "file data (mimetype)", compressed_size);
		}
		else //not the first mimetype file thing
		{
			// FILE DATA =============================================================
			if (extraFieldLen != 0)		// may indeed not exist, and lenght=0 must not be passed into digestString()
				tree->digestString(file, "extra field", extraFieldLen); //NOT zero-terminated
			tree->setKeyValue("file data", "", fromArgs("skipping %u bytes", compressed_size));
			Skip(file, compressed_size);
		}
		tree->popNode();
	}

	tree->pushNode("");
	tree->popNode();
}	// DumpUCF

	// =================================================================================================

	// AVI and WAV files are RIFF based. Although they have usage differences, we can have a common
	// dumper. This might need changes for other RIFF-based files, e.g. for specific legacy
	// metadata. RIFF is a chunky format. AVI and WAV have an outermost RIFF chunk. The XMP is in a
	// top level "_PMX" chunk. Legacy metadata for WAV is in a top level LIST/INFO chunk.  Legacy
	// metadata for AVI is in a variety of places, don't have specs at present. Free space can be
	// JUNK or JUNQ.
	//
	// A RIFF chunk contains:
	//   - A 4 byte chunk ID, typically ASCII letters
	//   - A little endian UInt32 size of the chunk data
	//   - The chunk data
	//	 - Pad byte if the chunk data length is odd (added on 2007-03-22)

	// The ID is written in "reading order", e.g. the 'R' in "RIFF" is first in the file. Chunks
	// must start on even offsets. A pad byte of 0 is written after the data if necessary. The size
	// does not include the pad, nor the ID and size fields. Some chunks contain nested chunks,
	// notably the RIFF and LIST chunks do. These have the layout:
	//   - A 4 byte chunk ID, typically ASCII letters
	//   - A little endian UInt32 size of the chunk data
	//   - A 4 byte usage ID, typically ASCII letters
	//   - The nested chunks

	// reads maxSize bytes from file (not "up to", exactly fullSize)
	// puts it into a string, sets respective tree property
static void setFixedBEXTField(LFA_FileRef file, std::string propName, XMP_Int64 fullSize)
{
	char* descriptionBuffer = new char[fullSize + 2];
	LFA_Read(file, descriptionBuffer, fullSize, true);
	descriptionBuffer[fullSize] = '\0'; // tack on, in case not contained
										// parse till first \0
	std::string description(descriptionBuffer);
	tree->setKeyValue(propName, description);
	delete[] descriptionBuffer;
}

struct ChunkSize64 // declare ChunkSize64 structure
{
	XMP_Uns32 chunkId; // chunk ID (i.e. "big1" - this chunk is a big one)
	XMP_Uns64 chunkSize; //
};

struct DataSize64Chunk // declare DataSize64Chunk structure
{
	XMP_Uns32 chunkId; // ds64
	XMP_Uns32 chunkSize; // 4 byte size of the ds64 chunk
	XMP_Uns64 riffSize; // size of RF64 block
	XMP_Uns64 dataSize; // size of data chunk
	XMP_Uns64 sampleCount; // sample count of fact chunk	
	XMP_Uns32 tableLength; // number of valid entries in array "table"
	std::vector< ChunkSize64 > table;
};



static XMP_Uns64 parseRF64(LFA_FileRef file, DataSize64Chunk* rf64Sizes)
{
	XMP_Int64 chunkPos = LFA_Tell(file);
	rf64Sizes->chunkId = tree->digest32u(file, "", false);
	std::string ds64ChunkID_ST(fromArgs("%.4s", &rf64Sizes->chunkId));
	assertMsg("Not a valid RF64 file!", ds64ChunkID_ST == "ds64");

	rf64Sizes->chunkSize = tree->digest32u(file, "", false);

	XMP_Uns32 bitCnt = 0;
	rf64Sizes->riffSize = tree->digest64u(file, "", false);
	rf64Sizes->dataSize = tree->digest64u(file, "", false);
	rf64Sizes->sampleCount = tree->digest64u(file, "", false);

	rf64Sizes->tableLength = tree->digest32u(file, "", false);

	bitCnt = 28;

	for (XMP_Uns32 i = 0; i < rf64Sizes->tableLength; i++)
	{
		ChunkSize64 tmp;
		tmp.chunkId = tree->digest32u(file, "", false);
		tmp.chunkSize = tree->digest64u(file, "", false);
		rf64Sizes->table.push_back(tmp);

		bitCnt += 12;
	}

	// is there a rest to skip?
	XMP_Uns32 rest = rf64Sizes->chunkSize - bitCnt;
	if (rest != 0)
	{
		Skip(file, rest);
	}
	// return correct RIFF size
	return rf64Sizes->riffSize;

}

static XMP_Uns64 getRealSize(bool isOutermost, std::string chunkID, LFA_FileRef file, DataSize64Chunk* sizeChunk)
{
	if (isOutermost)
	{
		return parseRF64(file, sizeChunk);
	}
	else
	{
		if (chunkID == "data")
		{
			return sizeChunk->dataSize;
		}
		else
		{
			// search table
			for (XMP_Uns32 i = 0; i < sizeChunk->tableLength; i++)
			{
				std::string idString(fromArgs("%.4s", &sizeChunk->table[i].chunkId));
				if (idString == chunkID)
				{
					return sizeChunk->table[i].chunkSize;
				}
			}
		}
	}
	return 0;
}

static void
DumpRIFFChunk(LFA_FileRef file, XMP_Int64 parentEnd, std::string origChunkPath, bool bigEndian = false, DataSize64Chunk* rf64Sizes = NULL)
{

	while (LFA_Tell(file) < parentEnd)
	{
		bool isOutermost = origChunkPath.empty();

		XMP_Int64 chunkPos = LFA_Tell(file);
		XMP_Int64 fileSize = LFA_Measure(file);
		XMP_Int64 fileTail = fileSize - chunkPos;

		if (fileTail < 8)
		{
			tree->pushNode("** unknown bytes **");
			tree->addOffset(file);
			tree->addComment("size: 0x%llX", fileTail);
			Skip(file, fileTail);	// Already read the 8 byte header.
			tree->popNode();
		}
		else
		{
			XMP_Uns32 tmp = tree->digest32u(file, "", true);
			XMP_Uns32 chunkID = GetUns32BE(&tmp); // flip if necessary for LE systems

			std::string idString(fromArgs("%.4s", &chunkID));

			XMP_Int64 chunkSizeWOHeader = tree->digest32u(file, "", bigEndian);
			XMP_Int64 validRF64Header = chunkSizeWOHeader;
			XMP_Uns32 chunkType = 0;
			std::string typeString = "";
			// only RIFF and LIST contain subchunks...
			bool hasSubChunks = (idString == "RIFF") || (idString == "RF64") || (idString == "FORM") || (idString == "LIST") || (idString == "APPL");

			if (hasSubChunks)
			{
				XMP_Uns32 tmp = tree->digest32u(file, "", true);
				chunkType = GetUns32BE(&tmp); // flip if necessary for LE systems
				typeString = fromArgs("%.4s", &chunkType);
			}
			//get inner ID 'type' as in 'listType', 'fileType', ...
			//XMP_Uns32 chunkType = tree->digest32u( file );

			if (chunkSizeWOHeader == 0xFFFFFFFF)  //RF64 size for children
			{

				chunkSizeWOHeader = getRealSize(isOutermost, idString, file, rf64Sizes);
			}

			XMP_Int64 chunkSize = chunkSizeWOHeader + 8;// NB: XMPInt64 <- XMPUns32
														//adding size of id and length field itself

														// calculate size if size field seems broken
			if (chunkSize > parentEnd)
				chunkSize = parentEnd - chunkPos;


			std::string chunkPath = isOutermost ? (idString) : (origChunkPath + "/" + idString);


			// check special case of trailing bytes not in a valid RIFF structure
			if (isOutermost && idString != "RIFF"&& idString != "FORM" && idString != "RF64")
			{
				//dump undefined bytes till the end of the file
				tree->pushNode("** unknown bytes **");
				chunkSize = parentEnd - chunkPos; // get size through calculation (and not from size bytes) 
				tree->addComment("offset 0x%llX, size: 0x%llX", chunkPos, chunkSize);
				Skip(file, chunkSize - 8);	// Already read the 8 byte header.
				tree->popNode();
			}
			else
			{

				bool skipper = false;
				if (hasSubChunks)
				{
					if (isOutermost)
					{
						assertMsg("level-0 chunk must be AVI, AVIX, WAVE, AIFF, AIFC",
							(typeString == "AVI ") || (typeString == "AVIX") || (typeString == "WAVE")
							|| (typeString == "AIFF") || (typeString == "AIFC"));
					}

					chunkPath = chunkPath + ":" + typeString;
					XMP_Uns64 dsHeaderSize = 0xFFFFFFFF;
					if (chunkSize != fileSize && idString == "RF64")
					{
						XMP_Uns32 tmp = tree->digest32u(file, "", true);
						XMP_Uns32 chunkID = GetUns32BE(&tmp); // flip if necessary for LE systems

						std::string dsString(fromArgs("%.4s", &chunkID));
						if (dsString == "ds64")
						{
							//skip 4 bytes to read size of file in ds64 header
							tmp = tree->digest32u(file, "", true);
							XMP_Uns64 dsHeader = tree->digest64u(file, "", true);
							dsHeaderSize = GetUns64BE(&dsHeader);
						}
						tree->pushNode(chunkPath);
						tree->addComment("offset 0x%llX, size 0x%llX, size(w/o header) 0x%llX", chunkPos, chunkSize, chunkSizeWOHeader);

						tree->setKeyValue("dsChunkSize", fromArgs("0x%llX", dsHeaderSize + 8));

					}
					else
					{
						tree->pushNode(chunkPath);
						tree->addComment("offset 0x%llX, size 0x%llX, size(w/o header) 0x%llX", chunkPos, chunkSize, chunkSizeWOHeader);

					}
					tree->setKeyValue("fileSize", fromArgs("0x%llX", fileSize));
					tree->setKeyValue("validRF64Header", fromArgs("0x%llX", validRF64Header));


					if (isOutermost && idString == "RF64")
					{
						tree->pushNode("RF64/ds64");
						tree->addComment("offset 0x%llX, size 0x%X, size(w/o header) 0x%X", chunkPos + 12, rf64Sizes->chunkSize + 8, rf64Sizes->chunkSize);
						tree->setKeyValue("riffSize", fromArgs("0x%llX", rf64Sizes->riffSize));
						tree->setKeyValue("dataSize", fromArgs("0x%llX", rf64Sizes->dataSize));
						tree->setKeyValue("sampleCount", fromArgs("0x%llX", rf64Sizes->sampleCount));
						tree->setKeyValue("tableLength", fromArgs("0x%X", rf64Sizes->tableLength));
						tree->popNode();

						DumpRIFFChunk(file, LFA_Tell(file) + chunkSize - 12 - rf64Sizes->chunkSize - 8 /* filesize + riff chunk size - riff header(12) - rf64 header(8) */, chunkPath, bigEndian, rf64Sizes);	// recurse!
					}
					if ((idString + ":" + typeString == "LIST:INFO") ||
						(idString + ":" + typeString == "LIST:Tdat") ||
						(idString + ":" + typeString == "RIFF:AVI ") ||
						(idString + ":" + typeString == "RIFF:AVIX") ||
						(idString + ":" + typeString == "RIFF:WAVE") ||
						(idString + ":" + typeString == "FORM:AIFF") ||
						(idString + ":" + typeString == "FORM:AIFC") ||
						(idString + ":" + typeString == "LIST:hdrl") ||
						(idString + ":" + typeString == "LIST:strl") ||
						(idString + ":" + typeString == "LIST:movi")
						)
					{
						DumpRIFFChunk(file, LFA_Tell(file) + chunkSize - 12, chunkPath, bigEndian, rf64Sizes);	// recurse!
					}
					else
					{
						Skip(file, chunkSize - 12); // skip it !
					}
					tree->popNode();
				}
				else if (idString.length() == 4) // check that we got a valid idString
				{

					// now that LIST:movi gets dumped,
					// skip some very frequent, irrelevant chunks, 
					// otherwise the dump becomes unusably long...
					std::string firstTwo = idString.substr(0, 2);
					std::string secondTwo = idString.substr(2, 2);
					if (secondTwo == "db" || secondTwo == "dc" || secondTwo == "wb") // nb: _could_ colidde, requiring additional numeric test
					{
						skipper = true;
					}

					if (!skipper)
					{
						tree->pushNode(chunkPath);
						//Log::info( chunkPath );
						tree->addComment("offset 0x%llX, size 0x%llX, size(w/o header) 0x%llX", chunkPos, chunkSize, chunkSizeWOHeader);
					}

					// tackle chunks of interest //////////////////////////////////////////////
					bool isListInfo =
						((origChunkPath == "RIFF:WAVE/LIST:INFO" || origChunkPath == "RIFF:AVI /LIST:INFO")
							&& idString.at(0) == 'I'); // so far all mapping relevant props begin with "I"

					bool isListTdat = (origChunkPath == "RIFF:WAVE/LIST:Tdat" || origChunkPath == "RIFF:AVI /LIST:Tdat")
						&& idString.at(0) != 'J'; // just exclude JUNK/Q

					bool isDispChunk =
						((origChunkPath == "RIFF:WAVE" || origChunkPath == "RIFF:AVI ")
							&& idString == "DISP");

					bool isBextChunk =
						((origChunkPath == "RIFF:WAVE" || origChunkPath == "RIFF:AVI ")
							&& idString == "bext");

					bool isIXMLChunk =
						((origChunkPath == "RIFF:WAVE")
							&& idString == "iXML");

					bool isXMPchunk = false; //assume beforehand
					if (idString == "_PMX")
					{	// detour first, to detect xmp in wrong places
						assertMsg("XMP packet found in wrong place!",
							(origChunkPath == "RIFF:WAVE" || "RIFF:AVI" || "RIFF:AVIX")); //be very linient here.
						isXMPchunk = true;
					}

					bool isIDITChunk =
						((origChunkPath == "RIFF:AVI/LIST:hdrl" || origChunkPath == "RIFF:AVI /LIST:hdrl")
							&& idString == "IDIT");

					// deal with chunks of interest /////////////////////////////////////////////
					// a little prelude for disp chunk
					if (isDispChunk)
					{
						XMP_Uns32 dispChunkType = LFA_ReadUns32_LE(file);
						// only dispChunks starting with a 0x0001 are of interest to us.
						// others do exist and are not an error

						if (dispChunkType != 0x0001)
							isDispChunk = false;

						chunkSize -= 4;
					}

					if (isListInfo || isListTdat || isDispChunk || isIDITChunk)
					{
						// dump that string:
						std::string value;

						if (chunkSize > 8) // aka skip for empty chunks
						{
							// first check if the string is zero terminated
							LFA_Seek(file, chunkSize - 8 - 1, SEEK_CUR); // jump to last char
							bool zeroTerm = (LFA_ReadUns8(file) == 0);
							LFA_Seek(file, -(chunkSize - 8), SEEK_CUR); //jump back
																		// some strings are zero-terminated (so despite initial length they are "c-strings"
																		// others are not ( "pascal strings" if you will.
																		// must cater to both: zero-terminated-ness should not affect resulting value.
							if (zeroTerm)
							{
								// read string without zero (last char)
								value = tree->digestString(file, "", chunkSize - 8 - 1, false);
								tree->addComment(" zero terminated");
								LFA_ReadUns8(file); // skip the zero
							}
							else
							{
								// read string including last char
								value = tree->digestString(file, "", chunkSize - 8, false);
								tree->addComment(" not zero terminated");
							}
							tree->changeValue(value);
						}

						tree->changeValue(value);
					}
					else if (isXMPchunk)
					{
						tree->pushNode("XMP packet");

						tree->addOffset(file);
						tree->addComment("packet size: 0x%llX", chunkSize - 8);
						Skip(file, chunkSize - 8);
						tree->addComment("packet end: 0x%llX", LFA_Tell(file));

						tree->popNode();
					}
					else if (isBextChunk)
					{
						tree->pushNode("bext chunk");
						tree->addOffset(file);
						tree->addComment("packet size: 0x%llX", chunkSize - 8);

						// I assume that the minimum BEXT chunk size is 602:
						// > 8 + ( 256+32+32+10+8+4+4+2+64+190+0 )
						// ans = 610
						const XMP_Int64 MIN_BEXT_SIZE = 610;
						assertMsg("minimum Berx Chunk Size", chunkSize >= MIN_BEXT_SIZE);
						XMP_Int64 BEXT_CodingHistorySize = chunkSize - MIN_BEXT_SIZE;

						setFixedBEXTField(file, chunkPath + ".Description", 256);
						setFixedBEXTField(file, chunkPath + ".Originator", 32);
						setFixedBEXTField(file, chunkPath + ".OriginatorReference", 32);
						setFixedBEXTField(file, chunkPath + ".OriginationDate", 10);
						setFixedBEXTField(file, chunkPath + ".OriginationTime", 8);

						tree->digest32u(file, chunkPath + ".TimeReferenceLow", false, true);  // DWORD == 32 Bit
						tree->digest32u(file, chunkPath + ".TimeReferenceHigh", false, true); // DWORD == 32 Bit

						tree->digest16u(file, chunkPath + ".Version", false, true);

						// UMID has 64 bytes:
						tree->digestString(file, chunkPath + ".UMID", 64);
						//tree->digest32u( file, chunkPath+".UMID_0-4", false, true );
						//tree->setKeyValue( "UMID_5-59" );
						//Skip( file, 64 - 4 - 4 );
						//tree->digest32u( file, chunkPath+".UMID_60-63", false, true );

						tree->setKeyValue(chunkPath + ".Reserved");
						Skip(file, 190);

						if (BEXT_CodingHistorySize)
						{
							setFixedBEXTField(file, chunkPath + ".CodingHistory", BEXT_CodingHistorySize);

							//tree->setKeyValue( chunkPath+".CodingHistory" ); // not bothering details.
							tree->addComment("( 0x%llx bytes ) ", BEXT_CodingHistorySize);
							//Skip( file, BEXT_CodingHistorySize );
						}

						tree->addComment("packet end: 0x%llX", LFA_Tell(file));
						tree->popNode();
					}
					else if (isIXMLChunk) {
						tree->pushNode("iXML packet");

						tree->addOffset(file);
						tree->addComment("packet size: 0x%llX", chunkSize - 8);
						//Skip( file, chunkSize - 8 );

						size_t sizeofIXMLValue = chunkSize - 8;
						char* descriptionBuffer = new char[sizeofIXMLValue + 2];
						LFA_Read(file, descriptionBuffer, sizeofIXMLValue, true);
						descriptionBuffer[sizeofIXMLValue] = '\0'; // tack on, in case not contained
																   // parse till first \0
						std::string description(descriptionBuffer);

						// Dumping the iXML chunk. Needed for testing
						// Add iXML chunk as a node to tree
						tree->setKeyValue(chunkPath + ".ValueOfIXMLChunk", description);

						delete[] descriptionBuffer;
						tree->addComment("packet end: 0x%llX", LFA_Tell(file));

						tree->popNode();

					}
					else
					{
						Skip(file, chunkSize - 8); // skip remainder of chunk ( id, length already digested )
						assertMsg(fromArgs("inner chunk size too big, curPos:0x%llx, parentEnd:0x%llx",
							LFA_Tell(file),
							parentEnd),
							LFA_Tell(file) <= parentEnd);
					}

					if (!skipper)
						tree->popNode();
				}
				else
				{
					//dump undefined bytes in LIST
					tree->pushNode("** unknown bytes **");
					tree->addOffset(file);
					tree->addComment("size: 0x%llX", chunkSize);
					Skip(file, chunkSize - 8);
					tree->popNode();
				}

				if (LFA_Tell(file) % 2 == 1) // if odd file position, add pad byte.
				{
					if (LFA_Tell(file) == parentEnd)
					{
						// last pad byte is missing
						tree->addComment(" (pad byte missing [bug 1521093])");
					}
					else
					{
						XMP_Uns8 padByte = LFA_ReadUns8(file);
						if (!skipper)
						{
							if (0 != padByte)
								tree->addComment(" (non-zero pad byte!)");
							else
								tree->addComment(" (pad byte)");
						}
					}
				}
			}
		}
	} // while


}	// DumpRIFFChunk

	// =================================================================================================

static void
DumpRIFF(LFA_FileRef file, XMP_Int64 fileLen)
{
	DataSize64Chunk rf64Sizes;
	DumpRIFFChunk(file, fileLen, "", false, &rf64Sizes);
}

static void
DumpAIFF(LFA_FileRef file, XMP_Int64 fileLen)
{
	DumpRIFFChunk(file, fileLen, "", true);
}
// =================================================================================================

static XMP_Uns32 crcTable[256];
static bool crcTableInited = false;

static XMP_Uns32 ComputeCRCforPNG(LFA_FileRef file, XMP_Uns32 crcOffset, XMP_Uns32 crcLen)
{
	if (!crcTableInited) {
		for (int n = 0; n < 256; ++n) {
			XMP_Uns32 c = n;
			for (int k = 0; k < 8; ++k) {
				XMP_Uns32 lowBit = c & 1;
				c = c >> 1;
				if (lowBit != 0) c = c ^ 0xEDB88320;
			}
			crcTable[n] = c;
		}
		crcTableInited = true;
	}

	XMP_Uns32 crc = 0xFFFFFFFF;
	CaptureFileData(file, crcOffset, crcLen);

	for (XMP_Uns32 i = 0; i < crcLen; ++i) {	// ! The CRC includes the chunk type and data.
		XMP_Uns8 byte = sDataPtr[i];
		XMP_Uns8 index = (XMP_Uns8)((crc ^ byte) & 0xFF);
		crc = crcTable[index] ^ (crc >> 8);
	}

	return crc ^ 0xFFFFFFFF;

}	// ComputeCRCforPNG

	// =================================================================================================

static const XMP_Uns32 kPNG_iTXt = 0x69545874;
static const XMP_Uns32 kPNG_tEXt = 0x74455874;
static const XMP_Uns32 kPNG_zTXt = 0x7A545874;

static XMP_Uns32
DumpPNGChunk(LFA_FileRef file, XMP_Uns32 pngLen, XMP_Uns32 chunkOffset)
{
	// A PNG chunk contains:
	//   A big endian UInt32 length for the data portion. Zero is OK.
	//   A 4 byte chunk type, should be 4 ASCII letters, lower or upper case.
	//   The chunk data.
	//   A big endian UInt32 CRC.
	// There are no alignment constraints.
	//
	// Chunks of type tEXt, iTXt, and zTXt have text values. Each text form has a leading "usage
	// keyword" followed by the data string. The keywords must be visible Latin-1, 0x20..0x7E and
	// 0xA1..0xFF. They are limited to 1 to 79 characters, plus a terminating nul.
	//
	// A tEXt chunk has 0 or more bytes of Latin-1 characters. The data is not nul terminated, and
	// embedded nuls are not allowed. A zTXt chunk is like tEXt but the data string is zlib compressed.
	// An iTXt chunk has a variety of "info tags" followed by a UTF-8 data string.
	//
	// The XMP is in an iTXt chunk with the keyword XML:com.adobe.xmp and 4 bytes of 0 for the info.

	XMP_Uns32 chunkLen;
	XMP_Uns32 chunkType;
	XMP_Uns32 chunkCRC;

	if ((pngLen - chunkOffset) < 12) {
		tree->addComment("** Unexpected end of PNG file, %ul bytes remaining **", (pngLen - chunkOffset));
		return (pngLen - chunkOffset);
	}

	LFA_Seek(file, chunkOffset, SEEK_SET);
	LFA_Read(file, &chunkLen, 4, true);
	chunkLen = GetUns32BE(&chunkLen);

	if (chunkLen > (pngLen - chunkOffset)) {
		tree->addComment("** No room for PNG chunk, need %u, have %u **", chunkLen, pngLen - chunkOffset);
		return (pngLen - chunkOffset);	// ! Not chunkLen, might be bad and cause wrap-around.
	}

	LFA_Read(file, &chunkType, 4, true);	// After read, memory is in file order.

	LFA_Seek(file, (chunkOffset + 8 + chunkLen), SEEK_SET);
	LFA_Read(file, &chunkCRC, 4, true);
	chunkCRC = GetUns32BE(&chunkCRC);

	tree->addComment("   '%.4s', offset %u (0x%X), size %d, CRC 0x%.8X",
		&chunkType, chunkOffset, chunkOffset, chunkLen, chunkCRC);

	XMP_Uns32 newCRC = ComputeCRCforPNG(file, (chunkOffset + 4), (chunkLen + 4));

	if (chunkCRC != newCRC) tree->addComment("** CRC should be 0x%.8X **", newCRC);

	chunkType = GetUns32BE(&chunkType);	// Reorder the type to compare with constants.

	if ((chunkType == kPNG_iTXt) || (chunkType == kPNG_tEXt) || (chunkType == kPNG_zTXt)) {

		CaptureFileData(file, (chunkOffset + 8), chunkLen);

		XMP_Uns8 * keywordPtr = sDataPtr;
		size_t     keywordLen = strlen((char*)keywordPtr);

		PrintOnlyASCII_8(keywordPtr, keywordLen);

		if ((chunkType == kPNG_iTXt) && (keywordLen == 17) && CheckBytes(keywordPtr, "XML:com.adobe.xmp", 18)) {

			if (sXMPPtr != 0) {
				tree->addComment("      ** Redundant XMP **");
			}
			else {
				CaptureXMP((keywordPtr + 22), (chunkLen - 22), (chunkOffset + 8 + 22));
				XMP_Uns32 otherFlags = GetUns32BE(keywordPtr + 18);
				if (otherFlags != 0) tree->addComment("** bad flags %.8X **", otherFlags);
			}

		}

	}

	return (8 + chunkLen + 4);

}	// DumpPNGChunk

	// =================================================================================================

static void
DumpPS(LFA_FileRef file, XMP_Uns32 fileLen)
{
	XMP_Int32 psOffset;
	size_t psLength;

	LFA_Seek(file, 4, SEEK_SET); // skip fileheader bytes
	LFA_Read(file, &psOffset, 4, true);
	LFA_Read(file, &psLength, 4, true);

	tree->addComment(" psOffset: %d, psLength: %d", psOffset, psLength);

	// jump to psOffset
	Skip(file, (psOffset - 12));

	// get the header (everything till first %

	XMP_Int64 offset = LFA_Tell(file);
	std::string key, value;
	char byte = LFA_GetChar(file);
	bool eof = false;
	while (!eof)
	{
		key.clear();
		key += byte; // add the first %
		byte = LFA_GetChar(file);

		while (byte != ' ' && byte != '\r') // get everthing until next space or LF
		{
			key += byte;
			byte = LFA_GetChar(file);

		}

		//if (CheckBytes( key.c_str(), "%%EOF", 5))
		if (key == "%%EOF")
		{
			eof = true;
		}
		else
		{
			byte = LFA_GetChar(file);
			value.clear();
			while (byte != '%') // get everthing until next %
			{
				value += byte;
				byte = LFA_GetChar(file);
			}
		}
		tree->pushNode(key);
		tree->addOffset(file);

		//for now only store value for header 
		if (key == "%!PS-Adobe-3.0")
		{
			tree->changeValue(value);
		}

		tree->addComment("offset: %d", offset);
		tree->addComment("size: 0x%llX", LFA_Tell(file) - offset);
		tree->popNode();

		offset = LFA_Tell(file);
	}
	// Now just get everything else and store all keys that start with %


	// get the key 
	// start of the PostScript DSC header comment

	/*XMP_Uns8 buffer [11];
	LFA_Read ( file,  &buffer, sizeof(buffer), true );

	if (!CheckBytes( buffer, "%!PS-Adobe-", 11))
	{
	tree->comment ( "** Invalid PS, unknown PS file tag." );
	return;
	}

	// Check the PostScript DSC major version number.
	XMP_Uns8 byte;
	LFA_Read ( file,  &byte, sizeof(byte), true );

	psMajorVer = 0;
	while ( IsNumeric( byte ) )
	{
	psMajorVer = (psMajorVer * 10) + (byte - '0');
	if ( psMajorVer > 1000 ) {
	tree->comment ( "** Invalid PS, Overflow." );
	return;
	};	// Overflow.
	LFA_Read ( file,  &byte, sizeof(byte), true );
	}
	if ( psMajorVer < 3 ){
	tree->comment ( "** Invalid PS, The version must be at least 3.0." );
	return;
	};	// The version must be at least 3.0.

	if ( byte != '.' ){
	tree->comment ( "** Invalid PS, No minor number" );
	return;
	};	// No minor number.
	LFA_Read ( file,  &byte, sizeof(byte), true );

	// Check the PostScript DSC minor version number.

	psMinorVer = 0;
	while ( IsNumeric( byte ) )
	{
	psMinorVer = (psMinorVer * 10) + (byte - '0');
	if ( psMinorVer > 1000 ) {
	tree->comment ( "** Invalid PS, Overflow." );
	return;
	};	// Overflow.
	LFA_Read ( file,  &byte, sizeof(byte), true );
	}

	tree->addComment(" psMajor Version: %d, psMinor Version: %d", psMajorVer, psMinorVer);*/
}

// =================================================================================================

static void
DumpPNG(LFA_FileRef file, XMP_Uns32 pngLen)
{
	// A PNG file contains an 8 byte signature followed by a sequence of chunks.

	XMP_Uns32  chunkOffset = 8;

	while (chunkOffset < pngLen) {
		XMP_Uns32 chunkLen = DumpPNGChunk(file, pngLen, chunkOffset);
		chunkOffset += chunkLen;
	}

	if (sXMPPtr != 0) DumpXMP("PNG XMP 'iTXt' chunk");

}	// DumpPNG

	// =================================================================================================

static void
DumpInDesign(LFA_FileRef file, XMP_Uns32 inddLen)
{
	InDesignMasterPage masters[2];
	size_t	 dbPages;
	XMP_Uns8 cobjEndian;

	// FIgure out which master page to use.

	LFA_Seek(file, 0, SEEK_SET);
	LFA_Read(file, &masters, sizeof(masters), true);

	XMP_Uns64 seq0 = GetUns64LE((XMP_Uns8 *)&masters[0].fSequenceNumber);
	XMP_Uns64 seq1 = GetUns64LE((XMP_Uns8 *)&masters[1].fSequenceNumber);

	if (seq0 > seq1) {
		dbPages = GetUns32LE((XMP_Uns8 *)&masters[0].fFilePages);
		cobjEndian = masters[0].fObjectStreamEndian;
		tree->addComment("   Using master page 0");
	}
	else {
		dbPages = GetUns32LE((XMP_Uns8 *)&masters[1].fFilePages);
		cobjEndian = masters[1].fObjectStreamEndian;
		tree->addComment("   Using master page 1");
	}

	bool bigEndian = (cobjEndian == kINDD_BigEndian);

	tree->addComment("%d pages, %s endian", dbPages, (bigEndian ? "big" : "little"));

	// Look for the XMP contiguous object.

	// *** XMP_Int64 cobjPos = (XMP_Int64)dbPages * kINDD_PageSize;	// ! Use a 64 bit multiply!
	XMP_Uns32 cobjPos = dbPages * kINDD_PageSize;
	XMP_Uns32 cobjLen;

	for (; cobjPos < inddLen; cobjPos += cobjLen) {

		InDesignContigObjMarker cobjHead;

		LFA_Seek(file, cobjPos, SEEK_SET);
		LFA_Read(file, &cobjHead, sizeof(cobjHead), true);

		if (!CheckBytes(&cobjHead.fGUID, kINDDContigObjHeaderGUID, kInDesignGUIDSize)) {

			// No Contiguous Object header. Could be in zero padding for the last page.

			XMP_Uns8 fileTail[kINDD_PageSize];
			size_t   tailLen = inddLen - cobjPos;
			bool endOK = (tailLen < kINDD_PageSize);

			if (endOK) {
				LFA_Seek(file, cobjPos, SEEK_SET);
				LFA_Read(file, fileTail, sizeof(fileTail), true);
				for (size_t i = 0; i < tailLen; ++i) {
					if (fileTail[i] != 0) {
						endOK = false;
						break;
					}
				}
			}

			if (endOK) break;
			tree->addComment("   ** No Contiguous Object GUID at offset %u (0x%X) tree.", cobjPos, cobjPos);
			return;

		}

		cobjHead.fObjectUID = GetUns32LE(&cobjHead.fObjectUID);
		cobjHead.fObjectClassID = GetUns32LE(&cobjHead.fObjectClassID);
		cobjHead.fStreamLength = GetUns32LE(&cobjHead.fStreamLength);
		cobjHead.fChecksum = GetUns32LE(&cobjHead.fChecksum);

		cobjLen = cobjHead.fStreamLength + (2 * sizeof(InDesignContigObjMarker));

		tree->addComment("   ContigObj offset %d (0x%X), size %d, Object UID %.8X, class ID %.8X, checksum %.8X",
			cobjPos, cobjPos, cobjHead.fStreamLength, cobjHead.fObjectUID, cobjHead.fObjectClassID, cobjHead.fChecksum);

		if ((cobjHead.fObjectClassID & 0x40000000) == 0) tree->addComment("read only");

		XMP_Uns32 xmpLen;
		LFA_Read(file, &xmpLen, 4, true);
		if (bigEndian) {
			xmpLen = GetUns32BE(&xmpLen);
		}
		else {
			xmpLen = GetUns32LE(&xmpLen);
		}

		XMP_Uns8 xmpStart[16];	// Enough for "<?xpacket begin=".
		LFA_Read(file, &xmpStart, sizeof(xmpStart), true);

		if ((cobjHead.fStreamLength > (4 + 16)) && ((xmpLen + 4) == cobjHead.fStreamLength) &&
			CheckBytes(xmpStart, "<?xpacket begin=", 16)) {

			if (sXMPPtr != 0) {
				tree->addComment("** redundant XMP **");
			}
			else {
				tree->addComment("XMP");
				CaptureXMPF(file, (cobjPos + sizeof(InDesignContigObjMarker) + 4), xmpLen);
			}

		}

	}

	if (sXMPPtr != 0) DumpXMP("InDesign XMP Contiguous Object");

}	// DumpInDesign

	// =================================================================================================

static void
DumpSVGTag(std::string basePath, XML_NodePtr currentNode)
{
	if (currentNode)
	{
		tree->pushNode(basePath + currentNode->name);

		// Iterating over all XML children.
		XML_NodeVector currNodeVector = currentNode->content;
		for (size_t i = 0; i < currNodeVector.size(); i++)
		{
			// Dump all children who are element nodes.
			if (currNodeVector[i]->kind == kElemNode)
				DumpSVGTag(basePath + currentNode->name + "/", currNodeVector[i]);

			// Extract the value from datanodes and put in TagMap if it's not yet available.
			if (currNodeVector[i]->kind == kCDataNode && tree->getValue(basePath + currentNode->name) == "")
				tree->updateKeyValue(basePath + currentNode->name, currNodeVector[i]->value);
		}
	}

}	// DumpSVGTag

	// =================================================================================================

static void
DumpSVG(LFA_FileRef file, XMP_Uns32 svgLen)
{
	// SVG is an XML based format.We consider any file as SVG file if the given file contains a SVG tag.
	// Hence CheckFileFormat looks for presence of "<svg" in the file.
	//
	// For Dumping SVG elements we are using ExpatAdapter. Below code will parse given file using this 
	// adapter and add different tags in the TagMap tree.
	//
	// Below is the currently supported structure of known tags for this format.
	//	<svg>
	//		<title/>
	//		<desc/>
	//		<metadata>
	//			<x:xmpmeta/>
	//			<...>
	//			<...>
	//		</metadata>
	//		<...>
	//		<...>
	//	</svg>

	ExpatAdapter * pExpatAdapter = XMP_NewExpatAdapter(false);

	if (pExpatAdapter == 0)
	{
		tree->comment("ExpatAdapter initialization failed. Cann't parse SVG file.");
		return;
	}

	// Allocating big enough memory on heap to read file contents.
	XMP_Uns8 *fileContent = new XMP_Uns8[svgLen + 1];
	memset(fileContent, 0, (svgLen + 1) * sizeof(XMP_Uns8));

	// Reading total file in buffer (fileContent)
	LFA_Seek(file, 0, SEEK_SET);
	LFA_Read(file, fileContent, svgLen + 1, false);

	// Parsing the file with ExpatAdapter
	pExpatAdapter->ParseBuffer(fileContent, svgLen + 1, false /* not the end */);

	// Finding <svg> element and adding to TagMap tree.
	XML_NodePtr svgNode = pExpatAdapter->tree.GetNamedElement("http://www.w3.org/2000/svg", "svg");
	DumpSVGTag("", svgNode);

	// De-allocating all the resources.
	if (fileContent)
	{
		delete[] fileContent;
		fileContent = NULL;
	}

	if (pExpatAdapter)
	{
		delete pExpatAdapter;
		pExpatAdapter = NULL;
	}

}	// DumpSVG

	// =================================================================================================

#define kSWF_FileAttributesTag	69
#define kSWF_MetadataTag		77

static void
DumpSWF(LFA_FileRef file, XMP_Uns32 swfLen)
{
	// SWF is a chunky format, the chunks are called tags. The data of a tag cannot contain an
	// offset to another tag, so tags can generally be freely inserted, removed, etc. Each tag has a
	// header followed by data. There are short (2 byte) and long (6 byte) headers. A short header
	// is a UInt16 with a type code in the upper 10 bits and data length in the lower 6 bits. The
	// length does not include the header. A length of 63 (0x3F) indicates a long header. This adds
	// an SInt32 data length.
	//
	// All multi-byte integers in SWF are little endian. Strings use byte storage and are null
	// terminated. In SWF 5 or earlier strings use ASCII or shift-JIS encoding with no indication in
	// the file. In SWF 6 or later strings use UTF-8.
	//
	// The overall structure of a SWF file:
	//   File header
	//   FileAttributes tag, optional before SWF 8
	//   other tags
	//   End tag
	//
	// The End tag is #0. No data is defined, but a reader should process the length normally. The
	// last tag must be an End tag, but End tags can also be used elsewhere (e.g. to end a sprite
	// definition). There is no standard tag for free or unused space.
	//
	// SWF file header:
	//   0  3 - signature, "FWS"=uncompressed, 'CWS'=compressed (zlib, SWF6 or later)
	//   3  1 - UInt8 major version
	//   4  4 - UInt32 uncompressed file length
	//   8  v - frame rectangle, variable size
	//   ?  2 - UInt16 frame rate
	//   ?  2 - UInt16 frame count
	//
	// FileAttributes tag, #69:
	//   0  3 - reserved, must be 0
	//   3  1 - HasMetadata, 0/1 Boolean
	//   4  3 - reserved, must be 0
	//   7  1 - UseNetwork, 0/1 Boolean
	//   8 24 - reserved, must be 0
	//
	// The Metadata tag is #77. If present, the FileAttributes tag must also be present and
	// HasMetadata must be set. The data is a string, must be XMP, should be as compact as possible.
	//
	// The frame rectangle is a packed sequence of 5 bit fields, with zero bits add as padding to a
	// byte boundary. The first field is 5 bits long and gives the number of bits in each of the
	// other 4 fields (0..31). The others are signed integers for the X min/max and Y min/max
	// coordinates. The frame rectangle field is at least 2 bytes long, and at most 17 bytes long.

	XMP_Uns8 buffer[100];	// Big enough, need 32 for file header and 38 for FileAttributes.
	size_t   ioCount;

	// Dump the file header.

	bool isCompressed = false;
	bool hasMetadata = false;

	XMP_Uns8  fileVersion;
	XMP_Uns32 fullLength;
	XMP_Uns8  rectBits;
	XMP_Uns16 frameRate, frameCount;

	ioCount = LFA_Read(file, buffer, sizeof(buffer), false);
	if (ioCount < 14) {
		tree->comment("** Invalid SWF, file header is too short.");
		return;
	}

	if (CheckBytes(buffer, "CWS", 3)) {
		isCompressed = true;
	}
	else if (!CheckBytes(buffer, "FWS", 3)) {
		tree->comment("** Invalid SWF, unknown file header signature.");
		return;
	}

	fileVersion = buffer[3];
	fullLength = GetUns32LE(&buffer[4]);
	rectBits = buffer[8] >> 3;

	XMP_Uns32 rectBytes = ((5 + (4 * rectBits)) / 8) + 1;
	XMP_Uns32 headerBytes = 8 + rectBytes + 4;

	if (ioCount < headerBytes) {
		tree->comment("** Invalid SWF, file header is too short.");
		return;
	}

	frameRate = GetUns16LE(&buffer[8 + rectBytes]);
	frameCount = GetUns16LE(&buffer[8 + rectBytes + 2]);

	// *** Someday decode the frame rectangle.

	tree->pushNode("File Header");
	tree->addComment("%scompressed, version %d, full length %d, frame rate %d, frame count %d",
		(isCompressed ? "" : "un"), fileVersion, fullLength, frameRate, frameCount);
	tree->popNode();
	if (isCompressed) {
		// *** Add support to decompress into a temp file.
		tree->comment("** Ignoring compressed SWF contents.");
		return;
	}

	// Dump the tags in the body of the file.

	XMP_Uns16 tagType;
	XMP_Uns32 tagOffset, tagLength, headerLength, dataLength;

	for (tagOffset = headerBytes; (tagOffset < swfLen); tagOffset += tagLength) {

		// Read the tag header, extract the type and data length.

		LFA_Seek(file, tagOffset, SEEK_SET);
		ioCount = LFA_Read(file, buffer, sizeof(buffer), false);


		if (ioCount < 2) {
			tree->comment("** Invalid SWF, tag header is too short at offset %u (0x%X).", tagOffset, tagOffset);
			break;
		}

		tagType = GetUns16LE(&buffer[0]);
		dataLength = tagType & 0x3F;
		tagType = tagType >> 6;

		if (dataLength < 63) {
			headerLength = 2;
		}
		else {
			if (ioCount < 6) {
				tree->comment("** Invalid SWF, tag header is too short at offset %u (0x%X).", tagOffset, tagOffset);
				break;
			}
			headerLength = 6;
			dataLength = GetUns32LE(&buffer[2]);
		}
		tagLength = headerLength + dataLength;

		// Make sure the tag fits in the file, being careful about arithmetic overflow.

		if (tagLength > (swfLen - tagOffset)) {
			tree->comment("** Invalid SWF, tag is too long at offset %u (0x%X).", tagOffset, tagOffset);
			break;
		}

		// See if this is the FileAttributes tag or the Metadata tag.

		if ((tagOffset == headerBytes) && (tagType != kSWF_FileAttributesTag) && (fileVersion >= 8)) {
			tree->comment("** Invalid SWF, first tag is not FileAttributes.");
		}

		if (tagType == kSWF_FileAttributesTag) {

			if (dataLength < 4) {
				tree->comment("** Invalid SWF, FileAttributes tag is too short at offset %u (0x%X).", tagOffset, tagOffset);
				continue;
			}

			XMP_Uns32 xmpFlag = GetUns32LE(&(buffer[headerLength])) & 0x10;
			if (xmpFlag != 0) {
				hasMetadata = true;
			}

			tree->pushNode("FileAttributes tag");
			tree->addComment("Offset %d (0x%X), %s XMP", tagOffset, tagOffset, (hasMetadata ? "has" : "no"));
			tree->popNode();

		}
		else if (tagType == kSWF_MetadataTag) {

			if (!hasMetadata) {
				tree->comment("** Invalid SWF, Metadata tag without HasMetadata flag at offset %u (0x%X).", tagOffset, tagOffset);
				continue;
			}

			tree->pushNode("Metadata tag");
			tree->addComment("Offset %d (0x%X)", tagOffset, tagOffset);
			tree->popNode();

			if (sXMPPtr != 0) {
				tree->comment("  ** Redundant Metadata tag");
			}
			else {
				CaptureXMPF(file, (tagOffset + headerLength), dataLength);
			}

			//if ( sXMPPtr != 0 ) DumpXMP ( "SWF Metadata tag (#77) XMP" );

		}
		else {
			tree->pushNode("tag #%d", tagType);
			tree->addComment("Offset %d (0x%X)", tagOffset, tagOffset);
			tree->popNode();
		}

	}

}	// DumpSWF

	// =================================================================================================

static XMP_Uns32 DumpScriptDataArray(LFA_FileRef file, XMP_Uns32 sdOffset, XMP_Uns32 count,
	bool isStrict, bool isOnXMP = false);
static XMP_Uns32 DumpScriptDataObject(LFA_FileRef file, XMP_Uns32 sdOffset);
static XMP_Uns32 DumpScriptDataObjectList(LFA_FileRef file, XMP_Uns32 sdOffset);

static inline XMP_Uns32 GetUns24BE(XMP_Uns8 * ptr)
{
	return (GetUns32BE(ptr) >> 8);
}

#define ReadSDValue(len)	\
	ioCount = LFA_Read ( file,  buffer, len,true);									\
	if ( ioCount != len ) {																\
		tree->comment ( "** Failure reading ScriptDataValue, ioCount = %d", ioCount );	\
		return (sdOffset + 1 + ioCount);												\
	}


static XMP_Uns32 DumpScriptDataValue(LFA_FileRef file, XMP_Uns32 sdOffset, bool isOnXMP = false)
{
	XMP_Uns8 buffer[64 * 1024];
	size_t   ioCount;

	XMP_Uns8  kind;
	XMP_Uns16 u16;
	XMP_Uns32 u32;
	XMP_Uns64 u64;

	LFA_Seek(file, sdOffset, SEEK_SET);
	ioCount = LFA_Read(file, &kind, 1, true);
	if (ioCount != 1) {
		tree->comment("** Failure reading ScriptDataValue kind, ioCount = %d", ioCount);
		return sdOffset;
	}

	if (isOnXMP) {
		if ((kind != 8) && (kind != 2) && (kind != 0xC)) {
			tree->comment("** Invalid kind for onXMPData tag **");
		}
	}

	XMP_Uns64 time;
	XMP_Int16 tz;

	switch (kind) {

	case 0x00:	// A number, IEEE double.
		ReadSDValue(8);
		u64 = GetUns64BE(&buffer[0]);
		tree->addComment("float = %f", *((double*)(&u64)));
		return (sdOffset + 1 + 8);

	case 0x01:	// A 0/1 Boolean. (??? general Uns8?)
		ReadSDValue(1);
		tree->addComment("bool = %d", buffer[0]);
		return (sdOffset + 1 + 1);

	case 0x02:	// A short UTF-8 string, leading 2 byte count.
		ReadSDValue(2);
		u16 = GetUns16BE(&buffer[0]);
		ReadSDValue(u16);
		if (int(u16) < 4096)
		{
			tree->addComment("string (%d) = \"%.*s\"", u16, u16, buffer);
		}
		else {
			tree->addComment("string (%d) ", u16);
		}
		if (buffer[u16 - 1] != 0) tree->addComment("value lacks trailing nul");
		if (isOnXMP) CaptureXMPF(file, (sdOffset + 1 + 2), u16);
		return (sdOffset + 1 + 2 + u16);

	case 0x03:	// An object list, triples of 0x02/key/value, ends at 0x02000009.
		tree->addComment("object list");
		return DumpScriptDataObjectList(file, sdOffset + 1);

	case 0x04:	// A movie clip path as short UTF-8 string
		ReadSDValue(2);
		u16 = GetUns16BE(&buffer[0]);
		ReadSDValue(u16);
		tree->addComment("movie (%d) = \"%.*s\"", u16, u16, buffer);
		if (buffer[u16 - 1] != 0) tree->addComment("value lacks trailing nul");
		return (sdOffset + 1 + 2 + u16);

	case 0x05:	// A null, single byte.
		tree->addComment("null");
		return (sdOffset + 1);

	case 0x06:	// A undefined, single byte.
		tree->addComment("undefined");
		return (sdOffset + 1);

	case 0x07:	// A reference, Uns16.
		ReadSDValue(2);
		u16 = GetUns16BE(&buffer[0]);
		tree->addComment("reference = %d", u16);
		return (sdOffset + 1 + 2);

	case 0x08:	// An ECMA array, 32-bit count then any number of key/value pairs. Has 0x000009 terminator.
		ReadSDValue(4);
		u32 = GetUns32BE(&buffer[0]);
		tree->addComment("ECMA array [%d]", u32);
		return DumpScriptDataArray(file, sdOffset + 1 + 4, u32, false, isOnXMP);

	case 0x09:	// End of object or array. Should not see this here!
		tree->addComment("** end **");
		return (sdOffset + 1);

	case 0x0A:	// A strict array, count then that many key/value pairs, no 0x000009 terminator.
		ReadSDValue(4);
		u32 = GetUns32BE(&buffer[0]);
		tree->addComment("strict array [%d]", u32);
		return DumpScriptDataArray(file, sdOffset + 1 + 4, u32, true);

	case 0x0B:	// A date, Uns64 milliseconds since Jan 1 1970, Int16 TZ offset in minutes.
		ReadSDValue(10);
		time = GetUns64BE(&buffer[0]);
		tz = (XMP_Int16)GetUns16BE(&buffer[8]);
		tree->addComment("date, time=%ULL, tz=%d", time, tz);
		return (sdOffset + 1 + 10);

	case 0x0C:	// A long UTF-8 string, leading 4 byte count.
		ReadSDValue(4);
		u32 = GetUns32BE(&buffer[0]);
		if (u32 < sizeof(buffer)) {
			ReadSDValue(u32);
			tree->addComment("long string (%d) = \"%.*s\"", u32, u32, buffer);
			if (buffer[u32 - 1] != 0) tree->addComment("value lacks trailing nul");
		}
		else {
			ReadSDValue(sizeof(buffer));
			tree->addComment("long string (%d) = \"%.*s\"", u32, sizeof(buffer), buffer);
			tree->comment("** truncated long string output **");
		}
		if (isOnXMP) CaptureXMPF(file, (sdOffset + 1 + 4), u32);
		return (sdOffset + 1 + 4 + u32);

	case 0x0D:	// Unsupported, single byte.
		tree->addComment("unsupported");
		return (sdOffset + 1);

	case 0x0E:	// A RecordSet. (???)
		tree->addComment("** record set ?? **");
		return (sdOffset + 1);

	case 0x0F:	// XML as a long UTF-8 string
		ReadSDValue(4);
		u32 = GetUns32BE(&buffer[0]);
		if (u32 < sizeof(buffer)) {
			ReadSDValue(u32);
			tree->addComment("XML (%d) = \"%.*s\"", u32, u32, buffer);
			if (buffer[u32 - 1] != 0) tree->addComment("value lacks trailing nul");
		}
		else {
			ReadSDValue(sizeof(buffer));
			tree->addComment("XML (%d) = \"%.*s\"", u32, sizeof(buffer), buffer);
			tree->comment("** truncated long string output **");
		}
		if (isOnXMP) CaptureXMPF(file, (sdOffset + 1 + 4), u32);
		return (sdOffset + 1 + 4 + u32);

	case 0x10:	// A typed object list, short string class name, object list (like case 0x03).
		ReadSDValue(2);
		u16 = GetUns16BE(&buffer[0]);
		ReadSDValue(u16);
		tree->addComment("class, name = %.*s", u16, u16, buffer);
		if (buffer[u16 - 1] == 0) tree->addComment("name has trailing nul");
		return DumpScriptDataObjectList(file, (sdOffset + 1 + 2 + u16));

	case 0x11:	// AMF 3 data. (???)
		tree->addComment("** AMF 3 data ?? **");
		return (sdOffset + 1);

	default:
		tree->addComment("** unknown kind = %d **", kind);
		return (sdOffset + 1);

	}

}	// DumpScriptDataValue

	// =================================================================================================

static XMP_Uns32 DumpScriptVariable(LFA_FileRef file, XMP_Uns32 sdOffset, bool isOnXMP = false)
{
	XMP_Uns8 buffer[64 * 1024];
	size_t   ioCount;

	LFA_Seek(file, sdOffset, SEEK_SET);
	ioCount = LFA_Read(file, buffer, 2, true);
	if (ioCount != 2) {
		tree->comment("** Failure reading DumpScriptVariable start, ioCount = %d", ioCount);
		return (sdOffset + ioCount);
	}

	XMP_Uns16 nameLen = GetUns16BE(&buffer[0]);
	ioCount = LFA_Read(file, buffer, nameLen, true);
	if (ioCount != nameLen) {
		tree->comment("** Failure reading ScriptDataObject name, ioCount = %d", ioCount);
		return (sdOffset + 3 + ioCount);
	}

	tree->pushNode("%.*s @ 0x%X", nameLen, buffer, sdOffset);
	if (buffer[nameLen - 1] == 0) tree->addComment("name has trailing nul");
	if (strncmp((char*)buffer, "liveXML", nameLen) != 0) isOnXMP = false;	// ! Else keep the input value.
	XMP_Uns32 nextOffset = DumpScriptDataValue(file, (sdOffset + 2 + nameLen), isOnXMP);
	tree->popNode();

	return nextOffset;

}	// DumpScriptVariable

	// =================================================================================================

static XMP_Uns32 DumpScriptDataArray(LFA_FileRef file, XMP_Uns32 sdOffset, XMP_Uns32 headerCount,
	bool isStrict, bool isOnXMP /* = false */)
{
	XMP_Uns8 buffer[3];
	size_t   ioCount;

	XMP_Uns32 actualCount = 0;
	XMP_Uns32 currOffset = sdOffset;

	if (isStrict) {

		for (; headerCount > 0; --headerCount) {
			XMP_Uns32 nextOffset = DumpScriptVariable(file, currOffset);
			if (nextOffset == currOffset) {
				tree->comment("** Failure reading DumpScriptDataArray, no progress");
				return currOffset;
			}
			currOffset = nextOffset;
		}

	}
	else {

		while (true) {

			LFA_Seek(file, currOffset, SEEK_SET);
			ioCount = LFA_Read(file, buffer, 3, true);
			if (ioCount != 3) {
				tree->comment("** Failure check DumpScriptDataArray, ioCount = %d", ioCount);
				return (currOffset + ioCount);
			}
			if (GetUns24BE(&buffer[0]) == 9) break;

			XMP_Uns32 nextOffset = DumpScriptVariable(file, currOffset, isOnXMP);
			if (nextOffset == currOffset) {
				tree->comment("** Failure reading DumpScriptDataArray, no progress");
				return currOffset;
			}

			++actualCount;
			currOffset = nextOffset;

		}

		if ((headerCount != (XMP_Uns32)(-1)) && (headerCount != actualCount)) {
			tree->comment("Count mismatch, actual = %d", actualCount);	// ! Not an error!
		}

		currOffset += 3;	// ! Include the 0x000009 terminator.

	}

	return currOffset;

}	// DumpScriptDataArray

	// =================================================================================================

static XMP_Uns32 DumpScriptDataObject(LFA_FileRef file, XMP_Uns32 sdOffset)
{
	XMP_Uns8 buffer[64 * 1024];
	size_t   ioCount;

	LFA_Seek(file, sdOffset, SEEK_SET);
	ioCount = LFA_Read(file, buffer, 2, true);
	if (ioCount != 2) {
		tree->comment("** Failure reading ScriptDataObject name length, ioCount = %d", ioCount);
		return (sdOffset + ioCount);
	}

	XMP_Uns16 nameLen = GetUns16BE(&buffer[1]);
	ioCount = LFA_Read(file, buffer, nameLen, true);
	if (ioCount != nameLen) {
		tree->comment("** Failure reading ScriptDataObject name, ioCount = %d", ioCount);
		return (sdOffset + 2 + ioCount);
	}

	tree->pushNode("%.*s @ 0x%X", nameLen, buffer, sdOffset);
	if (buffer[nameLen - 1] == 0) tree->addComment("name has trailing nul");
	XMP_Uns32 nextOffset = DumpScriptDataValue(file, (sdOffset + 2 + nameLen));
	tree->popNode();

	return nextOffset;

}	// DumpScriptDataObject

	// =================================================================================================

static XMP_Uns32 DumpScriptDataObjectList(LFA_FileRef file, XMP_Uns32 sdOffset)
{
	XMP_Uns8 buffer[3];
	size_t   ioCount;

	XMP_Uns32 currOffset = sdOffset;

	while (true) {

		LFA_Seek(file, currOffset, SEEK_SET);
		ioCount = LFA_Read(file, buffer, 3, true);
		if (ioCount != 3) {
			tree->comment("** Failure check ScriptDataObjectList, ioCount = %d", ioCount);
			return (currOffset + ioCount);
		}

		XMP_Uns32 endFlag = GetUns24BE(&buffer[0]);
		if (endFlag == 9) return (currOffset + 3);

		XMP_Uns32 nextOffset = DumpScriptDataObject(file, currOffset);
		if (nextOffset == currOffset) {
			tree->comment("** Failure reading ScriptDataObjectList, no progress");
			return currOffset;
		}

		currOffset = nextOffset;

	}

}	// DumpScriptDataObjectList

	// =================================================================================================

static XMP_Uns32 DumpScriptDataTagContent(LFA_FileRef file, XMP_Uns32 sdOffset, XMP_Uns32 tagTime)
{
	XMP_Uns8 buffer[64 * 1024];
	size_t   ioCount;

	LFA_Seek(file, sdOffset, SEEK_SET);
	ioCount = LFA_Read(file, buffer, 3, true);
	if ((ioCount != 3) || (buffer[0] != 2)) {
		tree->comment("** Failure reading ScriptDataObject start, ioCount = %d, buffer[0]=0x%X", ioCount, buffer[0]);
		return (sdOffset + ioCount);
	}

	XMP_Uns16 nameLen = GetUns16BE(&buffer[1]);
	ioCount = LFA_Read(file, buffer, nameLen, true);
	if (ioCount != nameLen) {
		tree->comment("** Failure reading ScriptDataObject name, ioCount = %d", ioCount);
		return (sdOffset + 3 + ioCount);
	}

	tree->addComment("%.*s @ 0x%X", nameLen, buffer, sdOffset);
	if (buffer[nameLen - 1] == 0) tree->addComment("name has trailing nul");

	bool isOnXMP = (tagTime == 0) && (nameLen == 9) && (strncmp((char*)buffer, "onXMPData", 9) == 0);
	return DumpScriptDataValue(file, (sdOffset + 1 + 2 + nameLen), isOnXMP);

}	// DumpScriptDataTagContent

	// =================================================================================================

static void
DumpFLV(LFA_FileRef file, XMP_Uns32 flvLen)
{
	// FLV must not be confused with SWF, they are quite different internally. FLV is a chunky
	// format, the chunks are called tags. All multi-byte integers in FLV are stored in big endian
	// order. An FLV file begins with a variable length file header followed by a sequence of tags
	// (the file body).
	//
	// Each tag contains a header, content, and back-pointer. The size in a tag header is just the
	// data size. The back-pointer is the full size of the preceeding tag, not just the data length.
	// The first tag is preceeded by a 0 back-pointer (not by the length of the header). The last
	// tagk is followed by a back pointer.
	//
	// The FLV file header:
	//   0  3 - signature, "FLV"
	//   3  1 - UInt8 major version
	//   4  1 - UInt8 flags
	//   5  4 - UInt32 size of header (offset to body)
	//
	// The FLV tag header:
	//   0  1 - UInt8 tag type
	//   1  3 - UInt24 length of data
	//   4  3 - UInt24 timestamp, milliseconds into the playback
	//   7  1 - UInt8 timestamp high, for a full UInt32 playback time
	//   8  3 - UInt24 stream ID, must be 0
	//
	// Only 3 tag types are defined by SWF-FLV-8, 8 = audio, 9 = video, 18 = script data. There is
	// confusion or missing information in the spec about script data tags. In one place it uses the
	// term "script data", in another SCRIPTDATAOBJECT for type 18. Then within the "Data Tags"
	// section it talks about SCRIPTDATAOBJECT, SCRIPTDATAOBJECTEND, SCRIPTDATASTRING,
	// SCRIPTDATALONGSTRING, SCRIPTDATAVALUE, SCRIPTDATAVARIABLE, SCRIPTDATAVARIABLEEND, and
	// SCRIPTDATADATE. It isn't clear if these SCRIPTDATA* things are FLV tags, or substructure
	// within the data of tag type 18.

	XMP_Uns8 buffer[100];
	size_t   ioCount;
	XMP_Uns32 size, time, stream, backSize;

	ioCount = LFA_Read(file, buffer, 9 + 4, true);
	if (ioCount != 9 + 4) {
		tree->comment("** Failure reading FLV header, ioCount = %d", ioCount);
		return;
	}

	size = GetUns32BE(&buffer[5]);
	tree->addComment("FLV header: \"%.3s\", version %d, flags 0x%.2X, size %d (0x%X)",
		&buffer[0], buffer[3], buffer[4], size);

	LFA_Seek(file, size, SEEK_SET);
	ioCount = LFA_Read(file, buffer, 4, true);
	if (ioCount != 4) {
		tree->comment("** Failure reading leading backSize, ioCount = %d", ioCount);
		return;
	}
	backSize = GetUns32BE(&buffer[0]);
	if (backSize != 0) {
		tree->comment("** Bad leading backSize = %d", backSize);
		return;
	}

	for (XMP_Uns32 tagOffset = (size + 4); tagOffset < flvLen; tagOffset += (11 + size + 4)) {

		LFA_Seek(file, tagOffset, SEEK_SET);
		ioCount = LFA_Read(file, buffer, 11, true);	// Back pointer plus tag header.
		if (ioCount != 11) {
			tree->comment("** Failure reading FLV tag, ioCount = %d", ioCount);
			return;
		}

		size = GetUns24BE(&buffer[1]);
		time = GetUns24BE(&buffer[4]);
		time += ((XMP_Uns32)buffer[7] << 24);
		stream = GetUns24BE(&buffer[8]);

		if (time != 0) break;	// ! Just do the time 0 tags for this tool.

		char label[100];
		char comment[1000];

		XMP_Uns8 kind = buffer[0];
		if (kind == 8) {
			if (time == 0) sprintf(label, "Audio");	// Don't normally print, there are too many.
		}
		else if (kind == 9) {
			if (time == 0) sprintf(label, "Video");	// Don't normally print, there are too many.
		}
		else if (kind == 18) {
			sprintf(label, "ScriptData");
		}
		else {
			sprintf(label, "<other-%d>", kind);
		}
		sprintf(comment, "%s @ 0x%X, size=%d, time=%d, stream=%d", label, tagOffset, size, time, stream);

		tree->pushNode(label);
		tree->addComment(comment);

		LFA_Seek(file, (tagOffset + 11 + size), SEEK_SET);
		ioCount = LFA_Read(file, buffer, 4, true);	// Back pointer plus tag header.
		if (ioCount != 4) {
			tree->comment("** Failure reading backSize, ioCount = %d", ioCount);
			return;
		}

		backSize = GetUns32BE(&buffer[0]);
		if (backSize != (11 + size)) tree->comment("** Bad backSize= %d", backSize);

		if (kind == 18) {
			XMP_Uns32 endOffset = DumpScriptDataTagContent(file, (tagOffset + 11), time);
			if (endOffset != (tagOffset + 11 + size)) {
				tree->comment("** Bad endOffset = 0x%X", endOffset);
			}
		}

		tree->popNode();

	}

	if (sXMPPtr != 0) DumpXMP("FLV onXMPData tag");

}	// DumpFLV

	// =================================================================================================

static bool
PrintID3Encoding(XMP_Uns8 encoding, XMP_Uns8 * strPtr)
{
	bool bigEndian = true;

	switch (encoding) {
	case 0:
		tree->addComment("Latin1");
		break;
	case 1:
		if (*strPtr == 0xFF) bigEndian = false;
		tree->addComment("UTF-16 with BOM, %s)", (bigEndian ? "BE" : "LE"));
		break;
	case 2:
		tree->addComment("UTF-16 BE");
		break;
	case 3:
		tree->addComment("UTF-8");
		break;
	default:
		tree->addComment("** unknown **");
		break;
	}
	return bigEndian;
}	// PrintID3Encoding

	// =================================================================================================

	//static void
	//PrintID3EncodedText (XMP_Uns8 encoding, void * _strPtr, const char * label)
	//{
	//}	// PrintID3EncodedText

	// =================================================================================================

struct ID3_Header {
	char id3[3];
	XMP_Uns8 vMajor, vMinor, flags;
	XMP_Uns8 splitSize[4];
};

struct ID3_v22_FrameHeader {
	char id[3];
	XMP_Uns8 sizeHigh;
	XMP_Uns16 sizeLow;
};

struct ID3_v23_FrameHeader {
	char id[4];
	XMP_Uns32 size;
	XMP_Uns16 flags;
};

#define _U32b(ptr,n) ((XMP_Uns32) (((XMP_Uns8*)(ptr))[n]))
#define GetSyncSafe32(ptr)	((_U32b(ptr,0) << 21) | (_U32b(ptr,1) << 14) | (_U32b(ptr,2) <<  7) | _U32b(ptr,3))

#define GetID3Size(ver,ptr)	((ver == 3) ? GetUns32BE(ptr) : GetSyncSafe32(ptr))

// =================================================================================================

static void DumpID3v22Frames(LFA_FileRef file, XMP_Uns8 vMajor, XMP_Uns32 framePos, XMP_Uns32 frameEnd) {

	// Dump the frames in an ID3 v2.2 tag.

	while ((framePos < frameEnd) && ((frameEnd - framePos) >= 6)) {

		ID3_v22_FrameHeader frameHead;
		LFA_Seek(file, framePos, SEEK_SET);
		LFA_Read(file, &frameHead, sizeof(frameHead), true);

		if (CheckBytes(frameHead.id, "\x0", 1)) break;	// Assume into padding.
														// FIXED: there could be just 1 or 2 or 3 bytes of padding total !!

		XMP_Uns32 frameSize = ((XMP_Uns32)frameHead.sizeHigh << 16) + GetUns16BE(&frameHead.sizeLow);

		tree->setKeyValue(
			fromArgs("ID3v2:%.3s", frameHead.id), "",	//no value yet, tree->changeValue() below
			fromArgs("offset %d (0x%X), size %d", framePos, framePos, frameSize));

		if (frameSize == 0) {

			// NOTHING TO DO HERE.
			// i.e. on 0-byte frames, including known ones...
			// ( i.e. the testcase of a (errorneous) TCON 0 byte frame )

		}
		else if ((frameHead.id[0] == 'T') || (frameHead.id[0] == 'W')) { // Text and URL fields

			CaptureFileData(file, 0, frameSize);
			XMP_Uns8 encoding = 0;
			XMP_Uns8 skip = 0;
			if (frameHead.id[0] == 'T') {	// URL field has no encoding byte
				encoding = sDataPtr[0];
				skip = 1;
			}

			bool bigEndian = PrintID3Encoding(encoding, (sDataPtr + skip));
			if (encoding == 0) {
				tree->changeValue(convert8Bit(sDataPtr + skip, false, frameSize - skip));
			}
			else {
				tree->changeValue(convert16Bit(bigEndian, sDataPtr + skip, false, (frameSize - skip)));
			}

		}
		else if (CheckBytes(frameHead.id, "PRV", 3) && (frameSize >= 4)) {

			// checking on the XMP packet
			CaptureFileData(file, 0, frameSize); //NB: has side effect: sDataLen, sDataMax, sDataPtr
			tree->changeValue(convert8Bit(sDataPtr, false, strlen((char*)sDataPtr)));
			if (CheckBytes(sDataPtr, "XMP\x0", 4)) {
				CaptureXMPF(file, (framePos + sizeof(frameHead) + 4), (frameSize - 4));
			}

		}
		else if (CheckBytes(frameHead.id, "COM", 3) || CheckBytes(frameHead.id, "ULT", 3)) {

			const char * descrLabel = "ID3v2:COM-descr";
			if (CheckBytes(frameHead.id, "ULT", 3)) descrLabel = "ID3v2:ULT-descr";

			CaptureFileData(file, 0, frameSize);
			XMP_Uns8 * frameEnd2 = sDataPtr + frameSize;

			XMP_Uns8   encoding = sDataPtr[0];
			char *     lang = (char*)(sDataPtr + 1);

			tree->addComment("lang '%.3s'", lang);
			bool bigEndian = PrintID3Encoding(encoding, (sDataPtr + 4));

			if (encoding == 0) {

				XMP_Uns8 * descrPtr = sDataPtr + 4;
				XMP_Uns8 * valuePtr = descrPtr;

				while (*valuePtr != 0) ++valuePtr;
				++valuePtr;

				size_t descrBytes = valuePtr - descrPtr - 1;
				tree->changeValue(convert8Bit(valuePtr, false, frameEnd2 - valuePtr));
				tree->setKeyValue(descrLabel, convert8Bit(descrPtr, false, descrBytes).c_str());

			}
			else {

				XMP_Uns16 * descrPtr = (XMP_Uns16*)(sDataPtr + 4);
				XMP_Uns16 * valuePtr = descrPtr;
				while (*valuePtr != 0) ++valuePtr;
				++valuePtr;

				size_t descrBytes = 2 * (valuePtr - descrPtr - 1);
				size_t valueBytes = 2 * ((XMP_Uns16*)frameEnd2 - valuePtr);

				tree->changeValue(convert16Bit(bigEndian, (XMP_Uns8*)valuePtr, false, valueBytes));
				tree->setKeyValue(descrLabel, convert16Bit(bigEndian, (XMP_Uns8*)descrPtr, false, descrBytes).c_str());

			}

		}

		framePos += (sizeof(frameHead) + frameSize);

	}

	if (framePos < frameEnd) {
		tree->setKeyValue("", "",
			fromArgs("Padding assumed, offset %d (0x%X), size %d", framePos, framePos, (frameEnd - framePos)));
	}

}	// DumpID3v22Frames

	// =================================================================================================

static void DumpID3v23Frames(LFA_FileRef file, XMP_Uns8 vMajor, XMP_Uns32 framePos, XMP_Uns32 frameEnd) {

	// Dump the frames in an ID3 v2.3 or v2.4 tag.
	int iIterator = 0;

	while ((framePos < frameEnd) && ((frameEnd - framePos) >= 10)) {

		ID3_v23_FrameHeader frameHead;
		LFA_Seek(file, framePos, SEEK_SET);
		LFA_Read(file, &frameHead, sizeof(frameHead), true);

		if (CheckBytes(frameHead.id, "\x0", 1)) break;	// Assume into padding.
														// FIXED: there could be just 1 or 2 or 3 bytes of padding total !!

		frameHead.size = GetID3Size(vMajor, &frameHead.size);
		frameHead.flags = GetUns16BE(&frameHead.flags);

		tree->setKeyValue(
			fromArgs("ID3v2:%.4s", frameHead.id), "",	//no value yet, tree->changeValue() below
			fromArgs("offset %d (0x%X), size %d, flags 0x%.2X", framePos, framePos, frameHead.size, frameHead.flags));

		if (frameHead.size == 0) {

			// NOTHING TO DO HERE.
			// i.e. on 0-byte frames, including known ones...
			// ( i.e. the testcase of a (errorneous) TCON 0 byte frame )

		}
		else if ((frameHead.id[0] == 'T') || (frameHead.id[0] == 'W')) { // Text and URL fields

			CaptureFileData(file, 0, frameHead.size);
			XMP_Uns8 encoding = 0;
			XMP_Uns8 skip = 0;
			if (frameHead.id[0] == 'T') {	// URL field has no encoding byte
				encoding = sDataPtr[0];
				skip = 1;
			}

			bool bigEndian = PrintID3Encoding(encoding, (sDataPtr + skip));
			if ((encoding == 0) || (encoding == 3)) {
				tree->changeValue(convert8Bit(sDataPtr + skip, false, frameHead.size - skip));
			}
			else if ((encoding == 1) || (encoding == 2)) {
				tree->changeValue(convert16Bit(bigEndian, sDataPtr + skip, false, (frameHead.size - skip)));
			}

		}
		else if (CheckBytes(frameHead.id, "PRIV", 4) && (frameHead.size >= 4)) {

			// checking on the XMP packet
			CaptureFileData(file, 0, frameHead.size); //NB: has side effect: sDataLen, sDataMax, sDataPtr
			tree->changeValue(convert8Bit(sDataPtr, false, strlen((char*)sDataPtr)));
			if (CheckBytes(sDataPtr, "XMP\x0", 4)) {
				CaptureXMPF(file, (framePos + sizeof(frameHead) + 4), (frameHead.size - 4));
			}

		}
		else if (CheckBytes(frameHead.id, "COMM", 4) || CheckBytes(frameHead.id, "USLT", 4)) {

			const char * descrLabel = "ID3v2:COMM-descr";
			if (CheckBytes(frameHead.id, "USLT", 4)) descrLabel = "ID3v2:USLT-descr";

			CaptureFileData(file, 0, frameHead.size);
			XMP_Uns8 * frameEnd2 = sDataPtr + frameHead.size;

			XMP_Uns8   encoding = sDataPtr[0];
			char *     lang = (char*)(sDataPtr + 1);

			tree->addComment("lang '%.3s'", lang);
			bool bigEndian = PrintID3Encoding(encoding, (sDataPtr + 4));

			if ((encoding == 0) || (encoding == 3)) {

				XMP_Uns8 * descrPtr = sDataPtr + 4;
				XMP_Uns8 * valuePtr = descrPtr;

				while (*valuePtr != 0) ++valuePtr;
				++valuePtr;

				size_t descrBytes = valuePtr - descrPtr - 1;
				tree->changeValue(convert8Bit(valuePtr, false, frameEnd2 - valuePtr));
				tree->setKeyValue(descrLabel, convert8Bit(descrPtr, false, descrBytes).c_str());

			}
			else if ((encoding == 1) || (encoding == 2)) {

				XMP_Uns16 * descrPtr = (XMP_Uns16*)(sDataPtr + 4);
				XMP_Uns16 * valuePtr = descrPtr;
				while (*valuePtr != 0) ++valuePtr;
				++valuePtr;

				size_t descrBytes = 2 * (valuePtr - descrPtr - 1);
				size_t valueBytes = 2 * ((XMP_Uns16*)frameEnd2 - valuePtr);

				tree->changeValue(convert16Bit(bigEndian, (XMP_Uns8*)valuePtr, false, valueBytes));
				tree->setKeyValue(descrLabel, convert16Bit(bigEndian, (XMP_Uns8*)descrPtr, false, descrBytes).c_str());

			}

		}

		else if (CheckBytes(frameHead.id, "APIC", 4)) {

			++iIterator;
			unsigned int iOffset = 0;
			CaptureFileData(file, 0, frameHead.size);

			char encoding[2];
			memset(encoding, 0x0, 2);
			encoding[0] = sDataPtr[iOffset++];
			tree->setKeyValue(fromArgs("ID3v2:APIC-encodingType_%d", iIterator), encoding);

			char * mimeType = (char*)(sDataPtr + iOffset);
			iOffset += strlen(mimeType) + 1;	//1 is for null termination 
			tree->setKeyValue(fromArgs("ID3v2:APIC-mimeType_%d", iIterator), mimeType);

			char pictureType[2];
			memset(pictureType, 0x0, 2);
			pictureType[0] = sDataPtr[iOffset++];
			tree->setKeyValue(fromArgs("ID3v2:APIC-pictureType_%1d", iIterator), pictureType);

			bool bigEndian = PrintID3Encoding(encoding[0], (sDataPtr + iOffset));
			if (encoding[0] == 0x00) {

				XMP_Uns8 * descrPtr = sDataPtr + iOffset;
				XMP_Uns8 * valuePtr = descrPtr;

				while (*valuePtr != 0) ++valuePtr;
				++valuePtr;	//Null termination

				size_t descrBytes = valuePtr - descrPtr;
				tree->setKeyValue(fromArgs("ID3v2:APIC-descr_%d", iIterator), convert8Bit(descrPtr, false, descrBytes - 1).c_str());
				iOffset += descrBytes;
			}
			else if (encoding[0] == 0x01) {

				XMP_Uns16 * descrPtr = (XMP_Uns16*)(sDataPtr + iOffset);
				XMP_Uns16 * valuePtr = descrPtr;

				while (*valuePtr != 0) ++valuePtr;
				++valuePtr;	//Null termination

				size_t descrBytes = 2 * (valuePtr - descrPtr);
				tree->setKeyValue(fromArgs("ID3v2:APIC-descr_%d", iIterator), convert16Bit(bigEndian, (XMP_Uns8*)(descrPtr + 1), false, descrBytes - 4).c_str());
				iOffset += descrBytes;
			}

			XMP_Uns8 *picPtr = (sDataPtr + iOffset);
			unsigned long size_PictureData = frameHead.size - iOffset;

			char picDataSize[8];
			memset(picDataSize, 0x0, 8);
			sprintf(picDataSize, "%lu", size_PictureData);

			std::string picData;
			picData.assign((char*)picPtr, size_PictureData);

			tree->setKeyValue(fromArgs("ID3v2:APIC-pictureData_%d", iIterator), picData);
			tree->setKeyValue(fromArgs("ID3v2:APIC-pictureDataSize_%d", iIterator), picDataSize);
		}

		framePos += (sizeof(frameHead) + frameHead.size);

	}

	if (iIterator) {
		char noOfAPICs[2];
		memset(noOfAPICs, 0x0, 2);
		sprintf(noOfAPICs, "%d", iIterator);
		tree->setKeyValue("ID3v2:NoOfAPIC", noOfAPICs);
	}

	if (framePos < frameEnd) {
		tree->setKeyValue("", "",
			fromArgs("Padding assumed, offset %d (0x%X), size %d", framePos, framePos, (frameEnd - framePos)));
	}

}	// DumpID3v23Frames

	// =================================================================================================

static void
DumpMP3(LFA_FileRef file, XMP_Uns32 /*mp3Len*/)
{
	// ** We're ignoring the effects of the unsync flag, and not checking the CRC (if present).
	assert(sizeof(ID3_Header) == 10);
	assert(sizeof(ID3_v23_FrameHeader) == 10);

	// Detect ID3v1 header:
	if (LFA_Measure(file) > 128)
	{
		LFA_Seek(file, -128, SEEK_END);
		XMP_Uns32 tagID = 0xFFFFFF00 & LFA_ReadUns32_BE(file);
		if (tagID == 0x54414700) // must be "TAG"
		{
			// Dump ID3v1 header:
			tree->pushNode("ID3v1");
			Rewind(file, 1); // read one byte too many...

			tree->digestString(file, "ID3v1:title", 30, false, true);
			tree->digestString(file, "ID3v1:artist", 30, false, true);
			tree->digestString(file, "ID3v1:album", 30, false, true);
			tree->digestString(file, "ID3v1:year", 4, false, true);
			tree->digestString(file, "ID3v1:comment", 30, false, true);
			tree->digest(file, "ID3v1:genreNo", 0, 1);

			// ID3v1.1 trackNo byte dance:
			Rewind(file, 3);
			LFA_Tell(file);
			if (LFA_ReadUns8(file) == 0)
				tree->digest(file, "ID3v1:trackNo", 0, 1);

			tree->popNode();
		}
	}

	// Dump ID3v2 header:
	ID3_Header id3Head;
	LFA_Seek(file, 0, SEEK_SET);
	LFA_Read(file, &id3Head, sizeof(id3Head), true);

	if (!CheckBytes(id3Head.id3, "ID3", 3)) {
		tree->setKeyValue("No ID3v2 tag");
		return;
	}

	XMP_Uns32 id3Len = GetSyncSafe32(id3Head.splitSize);
	XMP_Uns32 framePos = sizeof(id3Head);	// The offset of the next (first) ID3 frame.
	XMP_Uns32 frameEnd = framePos + id3Len;

	tree->pushNode("ID3v2.%d.%d, size %d, flags 0x%.2X", id3Head.vMajor, id3Head.vMinor, id3Len, id3Head.flags);

	if (id3Head.flags != 0) {
		tree->addComment("%s%s%s%s",
			((id3Head.flags & 0x80) ? ", unsync" : ""),
			((id3Head.flags & 0x40) ? ", extended header" : ""),
			((id3Head.flags & 0x20) ? ", experimental" : ""),
			((id3Head.flags & 0x10) ? ", has footer" : ""));
	}

	if ((id3Head.vMajor < 2) || (id3Head.vMajor > 4)) {
		tree->addComment("   ** Unrecognized major version tree.");
		tree->popNode();
		return;
	}

	bool hasExtHeader = ((id3Head.flags & 0x40) != 0);

	// Dump the extended header if present.
	if (hasExtHeader) {
		XMP_Uns32 extHeaderLen;

		extHeaderLen = tree->digest32u(file);
		extHeaderLen = GetID3Size(id3Head.vMajor, &extHeaderLen);
		framePos += (4 + extHeaderLen);

		switch (id3Head.vMajor) {

		case 2: {
			// #error "implement"
			break;
		}

		case 3: {
			XMP_Uns16 extHeaderFlags;
			LFA_Read(file, &extHeaderFlags, 2, true);
			extHeaderFlags = GetUns16BE(&extHeaderFlags);

			XMP_Uns32 padLen;
			LFA_Read(file, &padLen, 4, true);
			padLen = GetUns32BE(&padLen);

			frameEnd -= padLen;

			tree->pushNode("Extended header MajorV3 size %d, flags 0x%.4X, pad size %d",
				extHeaderLen, extHeaderFlags, padLen);

			if (extHeaderFlags & 0x8000) {
				XMP_Uns32 crc;
				LFA_Read(file, &crc, 4, true);
				crc = GetUns32BE(&crc);
				tree->setKeyValue("CRC", fromArgs("0x%.8X", crc));
			}
			tree->popNode();
			break;
		}

		case 4: {
			XMP_Uns8 flagCount;
			LFA_Read(file, &flagCount, 1, true);

			tree->pushNode("Extended header MajorV4 size %d, flag count %d", extHeaderLen, flagCount);

			for (size_t i = 0; i < flagCount; ++i) {
				XMP_Uns8 flag;
				LFA_Read(file, &flag, 1, true);
				tree->setKeyValue(fromArgs("Flag %.2d", flag), fromArgs("0x%.2X", flag));
			}
			tree->popNode();
			break;
		}

		default:
			tree->addComment("unknown major version !");
			break;

		}

	}

	////////////////////////////////////////////////////
	// Dump the ID3 frames

	if (id3Head.vMajor == 2) {
		DumpID3v22Frames(file, id3Head.vMajor, framePos, frameEnd);
	}
	else {
		DumpID3v23Frames(file, id3Head.vMajor, framePos, frameEnd);
	}

	if (sXMPPtr != 0) DumpXMP("ID3 'PRIV' \"XMP\" frame");

	tree->popNode();
}	// DumpMP3

	// =================================================================================================

static void
PacketScan(LFA_FileRef file, XMP_Int64 fileLen)
{
	try {

		XMPScanner scanner(fileLen);
		LFA_Seek(file, 0, SEEK_SET);

		XMP_Uns8 buffer[64 * 1024];
		XMP_Uns32 filePos, readLen;

		for (filePos = 0; filePos < fileLen; filePos += readLen) {
			readLen = LFA_Read(file, buffer, sizeof(buffer), false);
			if (readLen == 0) throw std::logic_error("Empty read");
			scanner.Scan(buffer, filePos, readLen);
		}

		size_t snipCount = scanner.GetSnipCount();
		XMPScanner::SnipInfoVector snips(snipCount);
		scanner.Report(snips);

		size_t packetCount = 0;

		for (size_t s = 0; s < snipCount; ++s) {
			if (snips[s].fState == XMPScanner::eValidPacketSnip) {
				++packetCount;
				CaptureXMPF(file, (XMP_Uns32)snips[s].fOffset, (XMP_Uns32)snips[s].fLength);
				DumpXMP("packet scan");
			}
		}

		if (packetCount == 0) tree->addComment("   No packets found");

	}
	catch (...) {

		tree->addComment("** Scanner failure tree.");

	}

}	// PacketScan

	// =================================================================================================
	// External Routines

namespace DumpFile_NS {
	// ! Xcode compiler warns about normal offsetof macro.
#define SafeOffsetOf(type,field)	((size_t)(&(((type*)1000)->field)) - 1000)

	//assure that packing is 100% tight
	//see above:
	// - #pragma pack (1)
	// - SafeOffsetOf  macro definition
	//
	// calling this at least once is an extremly good idea, 
	// because among other reasons, the #pragma pack directive
	// is not ANSI-C thus things could go wrong on one platform or another...
	//
	// returns nothing, but asserts will be triggered if something is wrong.
	static bool selfTestDone = false;
	void selfTest() {
		//only very first call at each runtime runs the selfTest (mostly verify about structPacking etc...)
		if (DumpFile_NS::selfTestDone) return;

		assert(sizeof(ASF_GUID) == 16);
		assert(SafeOffsetOf(ASF_GUID, part1) == 0);

		assert(SafeOffsetOf(ASF_GUID, part2) == 4);
		assert(SafeOffsetOf(ASF_GUID, part3) == 6);
		assert(SafeOffsetOf(ASF_GUID, part4) == 8);
		assert(SafeOffsetOf(ASF_GUID, part5) == 10);

		assert(sizeof(ASF_ObjHeader) == (16 + 8));
		assert(SafeOffsetOf(ASF_ObjHeader, guid) == 0);
		assert(SafeOffsetOf(ASF_ObjHeader, size) == 16);

		assert(sizeof(ASF_FileProperties) == kASF_FilePropertiesSize);
		assert(SafeOffsetOf(ASF_FileProperties, guid) == 0);
		assert(SafeOffsetOf(ASF_FileProperties, size) == 16);
		assert(SafeOffsetOf(ASF_FileProperties, fileID) == 24);
		assert(SafeOffsetOf(ASF_FileProperties, fileSize) == 40);
		assert(SafeOffsetOf(ASF_FileProperties, creationDate) == 48);
		assert(SafeOffsetOf(ASF_FileProperties, dataPacketsCount) == 56);
		assert(SafeOffsetOf(ASF_FileProperties, playDuration) == 64);
		assert(SafeOffsetOf(ASF_FileProperties, sendDuration) == 72);
		assert(SafeOffsetOf(ASF_FileProperties, preroll) == 80);
		assert(SafeOffsetOf(ASF_FileProperties, flags) == 88);
		assert(SafeOffsetOf(ASF_FileProperties, minDataPacketSize) == 92);
		assert(SafeOffsetOf(ASF_FileProperties, maxDataPacketSize) == 96);
		assert(SafeOffsetOf(ASF_FileProperties, maxBitrate) == 100);

		assert(sizeof(ASF_ContentDescription) == kASF_ContentDescriptionSize);
		assert(SafeOffsetOf(ASF_ContentDescription, guid) == 0);
		assert(SafeOffsetOf(ASF_ContentDescription, size) == 16);
		assert(SafeOffsetOf(ASF_ContentDescription, titleLen) == 24);
		assert(SafeOffsetOf(ASF_ContentDescription, authorLen) == 26);
		assert(SafeOffsetOf(ASF_ContentDescription, copyrightLen) == 28);
		assert(SafeOffsetOf(ASF_ContentDescription, descriptionLen) == 30);
		assert(SafeOffsetOf(ASF_ContentDescription, ratingLen) == 32);

		assert(sizeof(InDesignMasterPage) == kINDD_PageSize);
		assert(SafeOffsetOf(InDesignMasterPage, fGUID) == 0);
		assert(SafeOffsetOf(InDesignMasterPage, fMagicBytes) == 16);
		assert(SafeOffsetOf(InDesignMasterPage, fObjectStreamEndian) == 24);
		assert(SafeOffsetOf(InDesignMasterPage, fIrrelevant1) == 25);
		assert(SafeOffsetOf(InDesignMasterPage, fSequenceNumber) == 264);
		assert(SafeOffsetOf(InDesignMasterPage, fIrrelevant2) == 272);
		assert(SafeOffsetOf(InDesignMasterPage, fFilePages) == 280);
		assert(SafeOffsetOf(InDesignMasterPage, fIrrelevant3) == 284);

		assert(sizeof(InDesignContigObjMarker) == 32);
		assert(SafeOffsetOf(InDesignContigObjMarker, fGUID) == 0);
		assert(SafeOffsetOf(InDesignContigObjMarker, fObjectUID) == 16);
		assert(SafeOffsetOf(InDesignContigObjMarker, fObjectClassID) == 20);
		assert(SafeOffsetOf(InDesignContigObjMarker, fStreamLength) == 24);
		assert(SafeOffsetOf(InDesignContigObjMarker, fChecksum) == 28);

		selfTestDone = true;
	} // selfTest

} /*namespace DumpFile_NS*/

  // -------------------------------------------------------------------------------------------------

void DumpFile::Scan(std::string filename, TagTree &tagTree, bool resetTree)
{
	DumpFile_NS::selfTest(); //calls selftest (will happen only once per runtime, optimization done)

	if (resetTree)
	{
		tagTree.reset();
	}

	tree = &tagTree; // static "global" helper to avoid looping throug 'tree' 24x7

					 // Read the first 4K of the file into a local buffer and determine the file format.
					 // ! We're using ANSI C calls that don't handle files over 2GB.
					 // ! Should switch to copies of the "LFA" routines used inside XMP.

	LFA_FileRef fileRef = LFA_Open(filename.c_str(), 'r');

	assertMsg(std::string("can't open ") + filename, fileRef != 0);

	LFA_Seek(fileRef, 0, SEEK_END);
	XMP_Int64 fileLen = LFA_Tell(fileRef);

	XMP_Uns8 first4K[4096];

	LFA_Seek(fileRef, 0, SEEK_SET);
	LFA_Read(fileRef, first4K, 4096, false);

	LFA_Seek(fileRef, 0, SEEK_SET); //rewinds
									// (remains rewinded behind CheckFileDFormat, since that call does not get the fileRef handle)

	XMP_FileFormat format = CheckFileFormat(filename.c_str(), first4K, fileLen);

	if (sXMPPtr != 0) free(sXMPPtr);
	sXMPPtr = 0;
	sXMPMax = 0;
	sXMPLen = 0;
	sXMPPos = 0;

	//TODO refactor-out
	XMP_Uns8 * fileContent = 0;	// *** Hack for old file-in-RAM code.

	if (format == kXMP_JPEGFile) {

		tagTree.pushNode("Dumping JPEG file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		{
			fileContent = (XMP_Uns8*)malloc(fileLen);
			LFA_Seek(fileRef, 0, SEEK_SET);
			LFA_Read(fileRef, fileContent, fileLen, true);
			DumpJPEG(fileContent, fileLen);
		}
		tagTree.popNode();

	}
	else if (format == kXMP_PhotoshopFile) {

		tagTree.pushNode("Dumping Photoshop file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		{
			fileContent = (XMP_Uns8*)malloc(fileLen);
			LFA_Seek(fileRef, 0, SEEK_SET);
			LFA_Read(fileRef, fileContent, fileLen, true);
			DumpPhotoshop(fileContent, fileLen);
		}
		tagTree.popNode();

	}
	else if (format == kXMP_TIFFFile) {

		tagTree.pushNode("Dumping TIFF file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		{
			fileContent = (XMP_Uns8*)malloc(fileLen);
			LFA_Seek(fileRef, 0, SEEK_SET);
			LFA_Read(fileRef, fileContent, fileLen, true);
			DumpTIFF(fileContent, fileLen, 0, "TIFF file", "");
		}
		tagTree.popNode();

	}
	else if (format == kXMP_WMAVFile) {

		tagTree.pushNode("Dumping ASF (WMA/WMV) file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		DumpASF(fileRef, fileLen);
		tagTree.popNode();

	}
	else if (format == kXMP_AVIFile) {

		tagTree.pushNode("Dumping AVI file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		DumpRIFF(fileRef, fileLen);
		tagTree.popNode();

	}
	else if (format == kXMP_WAVFile) {

		tagTree.pushNode("Dumping WAV file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		DumpRIFF(fileRef, fileLen);
		tagTree.popNode();

	}
	else if (format == kXMP_AIFFFile) {

		tagTree.pushNode("Dumping AIFF file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		DumpAIFF(fileRef, fileLen);
		tagTree.popNode();

	}
	else if (format == kXMP_MPEG4File || format == kXMP_MOVFile || format == kXMP_JPEG2KFile || format == kXMP_HEIFFile) {
		// all ISO formats ( MPEG4, MOV, JPEG2000,HEIF) handled jointly,
		// - no longer relying on any advance "isQT" flagging
		tagTree.pushNode("ISO file");
		Log::info("size: %d", fileLen);
		//	tagTree.addComment ( "size %I64d (0x%I64X)", fileLen, fileLen );
		tagTree.addComment("size %lld (0x%llX)", fileLen, fileLen);
		ISOMetaKeys.clear();
		DumpISO(fileRef, fileLen);
		tagTree.popNode();

	}
	else if (format == kXMP_PNGFile) {

		tagTree.pushNode("Dumping PNG file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		DumpPNG(fileRef, fileLen);
		tagTree.popNode();

	}
	else if (format == kXMP_InDesignFile) {

		tagTree.pushNode("Dumping InDesign file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		DumpInDesign(fileRef, fileLen);
		tagTree.popNode();

	}
	else if (format == kXMP_SWFFile) {

		tagTree.pushNode("Dumping SWF file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		DumpSWF(fileRef, fileLen);
		tagTree.popNode();

	}
	else if (format == kXMP_FLVFile) {

		tagTree.pushNode("Dumping FLV file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		DumpFLV(fileRef, fileLen);
		tagTree.popNode();

	}
	else if (format == kXMP_MP3File) {

		tagTree.pushNode("Dumping MP3 file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		DumpMP3(fileRef, fileLen);
		tagTree.popNode();

	}
	else if (format == kXMP_UCFFile) {

		tagTree.pushNode("Dumping UCF (Universal Container Format) file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		DumpUCF(fileRef, fileLen);
		tagTree.popNode();

	}
	else if (format == kXMP_MPEGFile) {

		tagTree.comment("** Recognized MPEG-2 file type, but this is a pure sidecar solution. No legacy dump available at this time.");

	}
	else if (format == kXMP_PostScriptFile) {

		tagTree.pushNode("Dumping PostScript file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		DumpPS(fileRef, fileLen);
		tagTree.popNode();

	}
	else if (format == kXMP_SVGFile) {

		tagTree.pushNode("Dumping SVG file");
		tagTree.addComment("size %lld (0x%llx)", fileLen, fileLen);
		DumpSVG(fileRef, fileLen);
		tagTree.popNode();
	}
	else if (format == kXMP_UnknownFile) {

		tagTree.pushNode("Unknown format. packet scanning, size %d (0x%X)", fileLen, fileLen);
		PacketScan(fileRef, fileLen);
		tagTree.popNode();

	}
	else {
		tagTree.comment("** Recognized file type, '%.4s', but no smart dumper for it.", &format);
	}

	if (fileContent != 0) free(fileContent);
	LFA_Close(fileRef);

}	// DumpFile


void DumpFile::dumpFile(std::string filename)
{
	TagTree localTree;
	DumpFile::Scan(filename, localTree); // (important test in itself for validity)
	localTree.dumpTree();
}

