// =================================================================================================
// Copyright 2002 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

/**
* Demonstrates syntax and usage by exercising most of the API functions of XMPCore Toolkit SDK component, 
* using a sample XMP Packet that contains all of the different property and attribute types.
*/
#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <cstdlib>
#include <cerrno>
#include <ctime>

#define TXMP_STRING_TYPE	std::string

#include "public/include/XMP.hpp"
#include "public/include/XMP.incl_cpp"

//#define ENABLE_XMP_CPP_INTERFACE 1;

using namespace std;

#if WIN_ENV
	#pragma warning ( disable : 4100 )	// ignore unused variable
	#pragma warning ( disable : 4127 )	// conditional expression is constant
	#pragma warning ( disable : 4267 )	// possible loss of data (temporary for 64-bit builds)
	#pragma warning ( disable : 4505 )	// unreferenced local function has been removed
	#pragma warning ( disable : 4996 )	// '...' was declared deprecated
#endif

// =================================================================================================

static const char * kNS1 = "ns:test1/";
static const char * kNS2 = "ns:test2/";

static const char * kRDFCoverage =
	"<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>"
	"  <rdf:Description rdf:about='Test:XMPCoreCoverage/kRDFCoverage' xmlns:ns1='ns:test1/' xmlns:ns2='ns:test2/'>"
	""
	"    <ns1:SimpleProp1>Simple1 value</ns1:SimpleProp1>"
	"    <ns1:SimpleProp2 xml:lang='x-default'>Simple2 value</ns1:SimpleProp2>"
	""
	"    <ns1:ArrayProp1>"
	"      <rdf:Bag>"
	"        <rdf:li>Item1.1 value</rdf:li>"
	"        <rdf:li>Item1.2 value</rdf:li>"
	"      </rdf:Bag>"
	"    </ns1:ArrayProp1>"
	""
	"    <ns1:ArrayProp2>"
	"      <rdf:Alt>"
	"        <rdf:li xml:lang='x-one'>Item2.1 value</rdf:li>"
	"        <rdf:li xml:lang='x-two'>Item2.2 value</rdf:li>"
	"      </rdf:Alt>"
	"    </ns1:ArrayProp2>"
	""
	"    <ns1:ArrayProp3>"
	"      <rdf:Alt>"
	"        <rdf:li xml:lang='x-one'>Item3.1 value</rdf:li>"
	"        <rdf:li>Item3.2 value</rdf:li>"
	"      </rdf:Alt>"
	"    </ns1:ArrayProp3>"
	""
	"    <ns1:ArrayProp4>"
	"      <rdf:Alt>"
	"        <rdf:li>Item4.1 value</rdf:li>"
	"        <rdf:li xml:lang='x-two'>Item4.2 value</rdf:li>"
	"      </rdf:Alt>"
	"    </ns1:ArrayProp4>"
	""
	"    <ns1:ArrayProp5>"
	"      <rdf:Alt>"
	"        <rdf:li xml:lang='x-xxx'>Item5.1 value</rdf:li>"
	"        <rdf:li xml:lang='x-xxx'>Item5.2 value</rdf:li>"
	"      </rdf:Alt>"
	"    </ns1:ArrayProp5>"
	""
	"    <ns1:StructProp rdf:parseType='Resource'>"
	"      <ns2:Field1>Field1 value</ns2:Field1>"
	"      <ns2:Field2>Field2 value</ns2:Field2>"
	"    </ns1:StructProp>"
	""
	"    <ns1:QualProp1 rdf:parseType='Resource'>"
	"      <rdf:value>Prop value</rdf:value>"
	"      <ns2:Qual>Qual value</ns2:Qual>"
	"    </ns1:QualProp1>"
	""
	"    <ns1:QualProp2 rdf:parseType='Resource'>"
	"      <rdf:value xml:lang='x-default'>Prop value</rdf:value>"
	"      <ns2:Qual>Qual value</ns2:Qual>"
	"    </ns1:QualProp2>"
	""
	"    <!-- NOTE: QualProp3 is not quite kosher. Normally a qualifier on a struct is attached to the -->"
	"    <!-- struct node in the XMP tree, and the same for an array. See QualProp4 and QualProp5. But -->"
	"    <!-- for the pseudo-struct of a qualified simple property there is no final struct node that  -->"
	"    <!-- can own the qualifier. Instead the qualifier is attached to the value. The alternative   -->"
	"    <!-- of attaching the qualifier to the value and all other qualifiers is not compelling. This -->"
	"    <!-- issue only arises for xml:lang, it is the only qualifier that RDF has as an attribute.   -->"
	""
	"    <ns1:QualProp3 xml:lang='x-default' rdf:parseType='Resource'>"
	"      <rdf:value>Prop value</rdf:value>"
	"      <ns2:Qual>Qual value</ns2:Qual>"
	"    </ns1:QualProp3>"
	""
	"    <ns1:QualProp4 xml:lang='x-default' rdf:parseType='Resource'>"
	"      <ns2:Field1>Field1 value</ns2:Field1>"
	"      <ns2:Field2>Field2 value</ns2:Field2>"
	"    </ns1:QualProp4>"
	""
	"    <ns1:QualProp5 xml:lang='x-default'>"
	"      <rdf:Bag>"
	"        <rdf:li>Item1.1 value</rdf:li>"
	"        <rdf:li>Item1.2 value</rdf:li>"
	"      </rdf:Bag>"
	"    </ns1:QualProp5>"
	""
	"    <ns2:NestedStructProp rdf:parseType='Resource'>"
	"      <ns1:Outer rdf:parseType='Resource'>"
	"        <ns1:Middle rdf:parseType='Resource'>"
	"          <ns1:Inner rdf:parseType='Resource'>"
	"            <ns1:Field1>Field1 value</ns1:Field1>"
	"            <ns2:Field2>Field2 value</ns2:Field2>"
	"          </ns1:Inner>"
	"        </ns1:Middle>"
	"      </ns1:Outer>"
	"    </ns2:NestedStructProp>"
	""
	"  </rdf:Description>"
	"</rdf:RDF>";

static const char * kSimpleRDF =
	"<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>"
	"  <rdf:Description rdf:about='Test:XMPCoreCoverage/kSimpleRDF' xmlns:ns1='ns:test1/' xmlns:ns2='ns:test2/'>"
	""
	"    <ns1:SimpleProp>Simple value</ns1:SimpleProp>"
	""
	"    <ns1:ArrayProp>"
	"      <rdf:Bag>"
	"        <rdf:li>Item1 value</rdf:li>"
	"        <rdf:li>Item2 value</rdf:li>"
	"      </rdf:Bag>"
	"    </ns1:ArrayProp>"
	""
	"    <ns1:StructProp rdf:parseType='Resource'>"
	"      <ns2:Field1>Field1 value</ns2:Field1>"
	"      <ns2:Field2>Field2 value</ns2:Field2>"
	"    </ns1:StructProp>"
	""
	"    <ns1:QualProp rdf:parseType='Resource'>"
	"      <rdf:value>Prop value</rdf:value>"
	"      <ns2:Qual>Qual value</ns2:Qual>"
	"    </ns1:QualProp>"
	""
	"    <ns1:AltTextProp>"
	"      <rdf:Alt>"
	"        <rdf:li xml:lang='x-one'>x-one value</rdf:li>"
	"        <rdf:li xml:lang='x-two'>x-two value</rdf:li>"
	"      </rdf:Alt>"
	"    </ns1:AltTextProp>"
	""
	"    <ns1:ArrayOfStructProp>"
	"      <rdf:Bag>"
	"        <rdf:li rdf:parseType='Resource'>"
	"          <ns2:Field1>Item-1</ns2:Field1>"
	"          <ns2:Field2>Field 1.2 value</ns2:Field2>"
	"        </rdf:li>"
	"        <rdf:li rdf:parseType='Resource'>"
	"          <ns2:Field1>Item-2</ns2:Field1>"
	"          <ns2:Field2>Field 2.2 value</ns2:Field2>"
	"        </rdf:li>"
	"      </rdf:Bag>"
	"    </ns1:ArrayOfStructProp>"
	""
	"  </rdf:Description>"
	"</rdf:RDF>";

static const char * kNamespaceRDF =
	"<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>"
	"  <rdf:Description rdf:about='Test:XMPCoreCoverage/kNamespaceRDF' xmlns:ns1='ns:test1/'>"
	""
	"    <ns1:NestedStructProp rdf:parseType='Resource'>"
	"      <ns2:Outer rdf:parseType='Resource' xmlns:ns2='ns:test2/' xmlns:ns3='ns:test3/'>"
	"        <ns3:Middle rdf:parseType='Resource' xmlns:ns4='ns:test4/'>"
	"          <ns4:Inner rdf:parseType='Resource' xmlns:ns5='ns:test5/' xmlns:ns6='ns:test6/'>"
	"            <ns5:Field1>Field1 value</ns5:Field1>"
	"            <ns6:Field2>Field2 value</ns6:Field2>"
	"          </ns4:Inner>"
	"        </ns3:Middle>"
	"      </ns2:Outer>"
	"    </ns1:NestedStructProp>"
	""
	"  </rdf:Description>"
	"</rdf:RDF>";

static const char * kXMPMetaRDF =
	"<x:Outermost xmlns:x='adobe:ns:meta/'>"
	""
	"<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>"
	"  <rdf:Description rdf:about='Test:XMPCoreCoverage/kBogusLeadingRDF' xmlns:ns1='ns:test1/'>"
	"    <ns1:BogusLeadingProp>bogus packet</ns1:BogusLeadingProp>"
	"  </rdf:Description>"
	"</rdf:RDF>"
	""
	"<x:xmpmeta>"
	"<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>"
	"  <rdf:Description rdf:about='Test:XMPCoreCoverage/kXMPMetaRDF' xmlns:ns1='ns:test1/'>"
	"    <ns1:XMPMetaProp>xmpmeta packet</ns1:XMPMetaProp>"
	"  </rdf:Description>"
	"</rdf:RDF>"
	"</x:xmpmeta>"
	""
	"<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>"
	"  <rdf:Description rdf:about='Test:XMPCoreCoverage/kBogusTrailingRDF' xmlns:ns1='ns:test1/'>"
	"    <ns1:BogusTrailingProp>bogus packet</ns1:BogusTrailingProp>"
	"  </rdf:Description>"
	"</rdf:RDF>"
	""
	"</x:Outermost>";

static const char * kNewlineRDF =
	"<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>"
	"  <rdf:Description rdf:about='Test:XMPCoreCoverage/kNewlineRDF' xmlns:ns1='ns:test1/'>"
	""
	"    <ns1:HasCR>ASCII &#xD; CR</ns1:HasCR>"
	"    <ns1:HasLF>ASCII &#xA; LF</ns1:HasLF>"
	"    <ns1:HasCRLF>ASCII &#xD;&#xA; CRLF</ns1:HasCRLF>"
	""
	"  </rdf:Description>"
	"</rdf:RDF>";

static const char * kInconsistentRDF =
	"<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>"
	"  <rdf:Description rdf:about='Test:XMPCoreCoverage/kInconsistentRDF'"
	"                   xmlns:pdf='http://ns.adobe.com/pdf/1.3/'"
	"                   xmlns:xmp='http://ns.adobe.com/xap/1.0/'"
	"                   xmlns:dc='http://purl.org/dc/elements/1.1/'>"
	""
	"    <pdf:Author>PDF Author</pdf:Author>"
	"    <xmp:Author>XMP Author</xmp:Author>"
	""
	"    <xmp:Authors>"
	"      <rdf:Seq>"
	"        <rdf:li>XMP Authors [1]</rdf:li>"
	"      </rdf:Seq>"
	"    </xmp:Authors>"
	""
	"    <dc:creator>"
	"      <rdf:Seq>"
	"        <rdf:li>DC Creator [1]</rdf:li>"
	"      </rdf:Seq>"
	"    </dc:creator>"
	""
	"  </rdf:Description>"
	"</rdf:RDF>";

static const char * kDateTimeRDF =
	"<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>"
	"  <rdf:Description rdf:about='Test:XMPCoreCoverage/kDateTimeRDF' xmlns:ns1='ns:test1/'>"
	""
	"    <ns1:Date1>2003</ns1:Date1>"
	"    <ns1:Date2>2003-12</ns1:Date2>"
	"    <ns1:Date3>2003-12-31</ns1:Date3>"
	""
	"    <ns1:Date4>2003-12-31T12:34Z</ns1:Date4>"
	"    <ns1:Date5>2003-12-31T12:34:56Z</ns1:Date5>"
	""
	"    <ns1:Date6>2003-12-31T12:34:56.001Z</ns1:Date6>"
	"    <ns1:Date7>2003-12-31T12:34:56.000000001Z</ns1:Date7>"
	""
	"    <ns1:Date8>2003-12-31T10:04:56-02:30</ns1:Date8>"
	"    <ns1:Date9>2003-12-31T15:49:56+03:15</ns1:Date9>"
	""
	"  </rdf:Description>"
	"</rdf:RDF>";

// =================================================================================================

#define FoundOrNot(b)	((b) ? "found" : "not found")
#define YesOrNo(b)		((b) ? "yes" : "no")

// -------------------------------------------------------------------------------------------------

static void FillDateTime ( XMP_DateTime * dateTime, XMP_Int32 year, XMP_Int32 month, XMP_Int32 day,
						   XMP_Int32 hour, XMP_Int32 minute, XMP_Int32 second,
						   XMP_Bool hasDate, XMP_Bool hasTime, XMP_Bool hasTimeZone,
						   XMP_Int8 tzSign, XMP_Int32 tzHour, XMP_Int32 tzMinute, XMP_Int32 nanoSecond )
{

	dateTime->year = year;
	dateTime->month = month;
	dateTime->day = day;
	dateTime->hour = hour;
	dateTime->minute = minute;
	dateTime->second = second;
	dateTime->hasDate = hasDate;
	dateTime->hasTime = hasTime;
	dateTime->hasTimeZone = hasTimeZone;
	dateTime->tzSign = tzSign;
	dateTime->tzHour = tzHour;
	dateTime->tzMinute = tzMinute;
	dateTime->nanoSecond = nanoSecond;

}	// FillDateTime

// -------------------------------------------------------------------------------------------------

static void WriteMajorLabel ( FILE * log, const char * title )
{

	fprintf ( log, "\n" );
	fprintf ( log, "// =============================================================================\n" );
	fprintf ( log, "// %s.\n", title );
	fprintf ( log, "// =============================================================================\n" );
	fflush ( log );

}	// WriteMajorLabel

// -------------------------------------------------------------------------------------------------

static void WriteMinorLabel ( FILE * log, const char * title )
{

	fprintf ( log, "\n// " );
	for ( size_t i = 0; i < strlen(title); ++i ) fprintf ( log, "-" );
	fprintf ( log, "--\n// %s :\n\n", title );
	fflush ( log );

}	// WriteMinorLabel

// -------------------------------------------------------------------------------------------------

static XMP_Status DumpToString ( void * refCon, XMP_StringPtr outStr, XMP_StringLen outLen )
{
	XMP_Status	status	= 0;
	std::string * dumpString = (std::string*)refCon; 
	
	try {
		dumpString->append ( outStr, outLen );
	} catch ( ... ) {
		status = -1;
	}
	return status;
	
}	// DumpToString

// -------------------------------------------------------------------------------------------------

static XMP_Status DumpToFile ( void * refCon, XMP_StringPtr outStr, XMP_StringLen outLen )
{
	XMP_Status	status	= 0; 
	size_t		count;
	FILE *		outFile	= static_cast < FILE * > ( refCon );
	
	count = fwrite ( outStr, 1, outLen, outFile );
	if ( count != outLen ) status = errno;
	fflush ( outFile );
	return status;
	
}	// DumpToFile

// -------------------------------------------------------------------------------------------------

static void DumpXMPObj ( FILE * log, SXMPMeta & meta, const char * title )
{

	WriteMinorLabel ( log, title );
	meta.DumpObject ( DumpToFile, log );

}	// DumpXMPObj

// -------------------------------------------------------------------------------------------------

static void VerifyNewlines ( FILE * log, std::string xmp, const char * newline )
{
	for ( size_t i = 0; i < xmp.size(); ++i ) {
		if ( (xmp[i] == '\x0A') || (xmp[i] == '\x0D') ) {
			if ( strncmp ( &xmp[i], newline, strlen(newline) ) != 0 ) {
				fprintf ( log, "** Wrong newline at offset %zd\n", i );
			}
			if ( strlen(newline) == 2 ) ++i;
		}
	}
}

// =================================================================================================

static void DoXMPCoreCoverage ( FILE * log )
{

	int				i;
	bool			ok;
	std::string 	tmpStr1, tmpStr2, tmpStr3, tmpStr4;
	XMP_OptionBits	options;

	#if 0
	{	// Use this to be able to move the app window away from debugger windows.
		string junk;
		cout << "Move window, type anything to continue";
		cin >> junk;
	}
	#endif

	// --------------------------------------------------------------------------------------------

	#if 0

		WriteMajorLabel ( log, "Test global XMP toolkit options" );

		fprintf ( log, "Initial global options 0x%X\n", SXMPMeta::GetGlobalOptions() );
		SXMPMeta::SetGlobalOptions ( 0 );
		fprintf ( log, "Final global options 0x%X\n", SXMPMeta::GetGlobalOptions() );

	#endif

	// --------------------------------------------------------------------------------------------

	WriteMajorLabel ( log, "Dump predefined namespaces" );

	SXMPMeta::DumpNamespaces ( DumpToFile, log );

	// --------------------------------------------------------------------------------------------

	{
		WriteMajorLabel ( log, "Test simple constructors and parsing, setting the instance ID" );

		SXMPMeta meta1;
		DumpXMPObj ( log, meta1, "Empty XMP object" );
		meta1.GetObjectName ( &tmpStr1 );
		fprintf ( log, "\nEmpty object name = \"%s\"\n", tmpStr1.c_str() );
		meta1.SetObjectName ( "New object name" );
		DumpXMPObj ( log, meta1, "Set object name" );

		SXMPMeta meta2 ( kRDFCoverage, strlen ( kRDFCoverage ) );
		DumpXMPObj ( log, meta2, "Construct and parse from buffer" );
		meta2.GetObjectName ( &tmpStr1 );
		fprintf ( log, "\nRDFCoverage object name = \"%s\"\n", tmpStr1.c_str() );

		meta2.SetProperty ( kXMP_NS_XMP_MM, "InstanceID", "meta2:Original" );
		DumpXMPObj ( log, meta2, "Add instance ID" );

		SXMPMeta meta4;
		meta4 = meta2.Clone();
		meta4.SetProperty ( kXMP_NS_XMP_MM, "InstanceID", "meta4:Clone" );
		DumpXMPObj ( log, meta4, "Clone and add instance ID" );

	#if 0

		WriteMajorLabel ( log, "Test XMPMeta object options" );

		fprintf ( log, "Initial object options 0x%X\n", meta2.GetObjectOptions() );
		meta2.SetObjectOptions ( <TBD> );
		fprintf ( log, "Final object options 0x%X\n", meta2.GetObjectOptions() );

	#endif

	}

	// --------------------------------------------------------------------------------------------
	// Static namespace functions
	// --------------------------

	WriteMajorLabel ( log, "Test static namespace functions" );
	fprintf ( log, "\n" );

	tmpStr1.erase();
	ok = SXMPMeta::RegisterNamespace ( kNS2, "ns2", &tmpStr1 );
	fprintf ( log, "RegisterNamespace ns2 : %s, %s\n", YesOrNo ( ok ), tmpStr1.c_str() );

	tmpStr1.erase();
	ok = SXMPMeta::RegisterNamespace ( kNS2, "nsx:", &tmpStr1 );
	fprintf ( log, "RegisterNamespace nsx : %s, %s\n", YesOrNo ( ok ), tmpStr1.c_str() );

	tmpStr1.erase();
	ok = SXMPMeta::GetNamespacePrefix ( kNS1, &tmpStr1 );
	fprintf ( log, "GetNamespacePrefix ns1 : %s, %s\n", FoundOrNot ( ok ), tmpStr1.c_str() );

	tmpStr1.erase();
	ok = SXMPMeta::GetNamespaceURI ( "ns1", &tmpStr1 );
	fprintf ( log, "GetNamespaceURI ns1 : %s, %s\n", FoundOrNot ( ok ), tmpStr1.c_str() );

	tmpStr1.erase();
	ok = SXMPMeta::GetNamespacePrefix ( "bogus", &tmpStr1 );
	fprintf ( log, "GetNamespacePrefix bogus : %s\n", FoundOrNot ( ok ) );

	tmpStr1.erase();
	ok = SXMPMeta::GetNamespaceURI ( "bogus", &tmpStr1 );
	fprintf ( log, "GetNamespaceURI bogus : %s\n", FoundOrNot ( ok ) );

	SXMPMeta::DumpNamespaces ( DumpToFile, log );

	#if 0
		SXMPMeta::DeleteNamespace ( kNS2 );
		SXMPMeta::DumpNamespaces ( DumpToFile, log );
		(void) SXMPMeta::RegisterNamespace ( kNS2, "ns2", 0 );
	#endif

	// --------------------------------------------------------------------------------------------
	// Basic set/get methods
	// ---------------------

	{
		SXMPMeta meta;

		WriteMajorLabel ( log, "Test SetProperty and related methods" );

		tmpStr1 = "Prop value";
		meta.SetProperty ( kNS1, "Prop", tmpStr1 );
		meta.SetProperty ( kNS1, "ns1:XMLProp", "<PropValue/>" );
		meta.SetProperty ( kNS1, "ns1:URIProp", "URI:value/", kXMP_PropValueIsURI );

		tmpStr1 = "BagItem value";
		meta.AppendArrayItem ( kNS1, "Bag", kXMP_PropValueIsArray, tmpStr1 );
		meta.AppendArrayItem ( kNS1, "ns1:Seq", kXMP_PropArrayIsOrdered, "SeqItem value" );
		meta.AppendArrayItem ( kNS1, "ns1:Alt", kXMP_PropArrayIsAlternate, "AltItem value" );

		tmpStr1 = "Field1 value";
		meta.SetStructField ( kNS1, "Struct", kNS2, "Field1", tmpStr1 );
		meta.SetStructField ( kNS1, "ns1:Struct", kNS2, "Field2", "Field2 value" );
		meta.SetStructField ( kNS1, "ns1:Struct", kNS2, "Field3", "Field3 value" );

		tmpStr1 = "BagItem 3";
		meta.SetArrayItem ( kNS1, "Bag", 1, tmpStr1 );
		meta.SetArrayItem ( kNS1, "ns1:Bag", 1, "BagItem 1", kXMP_InsertBeforeItem );
		meta.SetArrayItem ( kNS1, "ns1:Bag", 1, "BagItem 2", kXMP_InsertAfterItem );
		meta.AppendArrayItem ( kNS1, "Bag", 0, "BagItem 4" );

		DumpXMPObj ( log, meta, "A few basic Set... calls" );

		tmpStr1.erase();
		meta.SerializeToBuffer ( &tmpStr1, kXMP_OmitPacketWrapper );
		fprintf ( log, "\n%s\n", tmpStr1.c_str() );

		fprintf ( log, "CountArrayItems Bag = %d\n", meta.CountArrayItems ( kNS1, "Bag" ) );

		meta.SetProperty ( kNS1, "QualProp1", "Prop value" );
		meta.SetQualifier ( kNS1, "QualProp1", kNS2, "Qual1", "Qual1 value" );
		// *** meta.SetProperty ( kNS1, "QualProp1/Qual2", "Qual2 value", kXMP_PropIsQualifier );	invalid
		meta.SetProperty ( kNS1, "QualProp1/?ns2:Qual3", "Qual3 value" );
		meta.SetProperty ( kNS1, "QualProp1/?xml:lang", "x-qual" );

		meta.SetProperty ( kNS1, "QualProp2", "Prop value" );
		meta.SetQualifier ( kNS1, "QualProp2", kXMP_NS_XML, "lang", "en-us" );
		// *** meta.SetProperty ( kNS1, "QualProp2/xml:lang", "x-field", kXMP_PropIsQualifier );	invalid
		meta.SetProperty ( kNS1, "QualProp2/@xml:lang", "x-attr" );

		meta.SetProperty ( kNS1, "QualProp3", "Prop value" );
		meta.SetQualifier ( kNS1, "ns1:QualProp3", kXMP_NS_XML, "xml:lang", "en-us" );
		meta.SetQualifier ( kNS1, "ns1:QualProp3", kNS2, "ns2:Qual", "Qual value" );

		meta.SetProperty ( kNS1, "QualProp4", "Prop value" );
		tmpStr1 = "Qual value";
		meta.SetQualifier ( kNS1, "QualProp4", kNS2, "Qual", tmpStr1 );
		meta.SetQualifier ( kNS1, "QualProp4", kXMP_NS_XML, "lang", "en-us" );

		DumpXMPObj ( log, meta, "Add some qualifiers" );

		tmpStr1.erase();
		meta.SerializeToBuffer ( &tmpStr1, kXMP_OmitPacketWrapper );
		fprintf ( log, "\n%s\n", tmpStr1.c_str() );
		
		meta.SetProperty ( kNS1, "QualProp1", "new value" );
		meta.SetProperty ( kNS1, "QualProp2", "new value" );
		meta.SetProperty ( kNS1, "QualProp3", "new value" );
		meta.SetProperty ( kNS1, "QualProp4", "new value" );
		DumpXMPObj ( log, meta, "Change values and keep qualifiers" );

		// ----------------------------------------------------------------------------------------

		WriteMajorLabel ( log, "Test GetProperty and related methods" );

		meta.DeleteProperty ( kNS1, "QualProp1" );	// ! Start with fresh qualifiers.
		meta.DeleteProperty ( kNS1, "ns1:QualProp2" );
		meta.DeleteProperty ( kNS1, "ns1:QualProp3" );
		meta.DeleteProperty ( kNS1, "QualProp4" );

		meta.SetProperty ( kNS1, "QualProp1", "Prop value" );
		meta.SetQualifier ( kNS1, "QualProp1", kNS2, "Qual1", "Qual1 value" );

		meta.SetProperty ( kNS1, "QualProp2", "Prop value" );
		meta.SetQualifier ( kNS1, "QualProp2", kXMP_NS_XML, "lang", "en-us" );

		meta.SetProperty ( kNS1, "QualProp3", "Prop value" );
		meta.SetQualifier ( kNS1, "QualProp3", kXMP_NS_XML, "lang", "en-us" );
		meta.SetQualifier ( kNS1, "QualProp3", kNS2, "Qual", "Qual value" );

		meta.SetProperty ( kNS1, "QualProp4", "Prop value" );
		meta.SetQualifier ( kNS1, "QualProp4", kNS2, "Qual", "Qual value" );
		meta.SetQualifier ( kNS1, "QualProp4", kXMP_NS_XML, "lang", "en-us" );

		DumpXMPObj ( log, meta, "XMP object" );
		fprintf ( log, "\n" );

		tmpStr1.erase();
		ok = meta.GetProperty ( kNS1, "Prop", &tmpStr1, &options );
		fprintf ( log, "GetProperty ns1:Prop : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		try {
			tmpStr1.erase();
			ok = meta.GetProperty ( 0, "ns1:Prop", &tmpStr1, &options );
			fprintf ( log, "#ERROR: No exception for GetProperty with no schema URI : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );
		} catch ( XMP_Error & excep ) {
			fprintf ( log, "GetProperty with no schema URI - threw XMP_Error #%d : %s\n", excep.GetID(), excep.GetErrMsg() );
		} catch ( ... ) {
			fprintf ( log, "GetProperty with no schema URI - threw unknown exception\n" );
		}

		tmpStr1.erase();
		ok = meta.GetProperty ( kNS1, "ns1:XMLProp", &tmpStr1, &options );
		fprintf ( log, "GetProperty ns1:XMLProp : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		tmpStr1.erase();
		ok = meta.GetProperty ( kNS1, "ns1:URIProp", &tmpStr1, &options );
		fprintf ( log, "GetProperty ns1:URIProp : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		fprintf ( log, "\n" );

		tmpStr1.erase();
		ok = meta.GetArrayItem ( kNS1, "Bag", 2, &tmpStr1, &options );
		fprintf ( log, "GetArrayItem ns1:Bag[2] : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		try {
			tmpStr1.erase();
			ok = meta.GetArrayItem ( 0, "ns1:Bag", 1, &tmpStr1, &options );
			fprintf ( log, "#ERROR: No exception for GetArrayItem with no schema URI : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );
		} catch ( XMP_Error & excep ) {
			fprintf ( log, "GetArrayItem with no schema URI - threw XMP_Error #%d : %s\n", excep.GetID(), excep.GetErrMsg() );
		} catch ( ... ) {
			fprintf ( log, "GetArrayItem with no schema URI - threw unknown exception\n" );
		}

		tmpStr1.erase();
		ok = meta.GetArrayItem ( kNS1, "ns1:Seq", 1, &tmpStr1, &options );
		fprintf ( log, "GetArrayItem ns1:Seq[1] : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		tmpStr1.erase();
		ok = meta.GetArrayItem ( kNS1, "ns1:Alt", kXMP_ArrayLastItem, &tmpStr1, &options );
		fprintf ( log, "GetArrayItem ns1:Alt[1] : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		fprintf ( log, "\n" );

		tmpStr1.erase();
		ok = meta.GetStructField ( kNS1, "Struct", kNS2, "Field1", &tmpStr1, &options );
		fprintf ( log, "GetStructField ns1:Struct/ns2:Field1 : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		tmpStr1.erase();
		ok = meta.GetStructField ( kNS1, "ns1:Struct", kNS2, "ns2:Field2", &tmpStr1, &options );
		fprintf ( log, "GetStructField ns1:Struct/ns2:Field2 : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		tmpStr1.erase();
		ok = meta.GetStructField ( kNS1, "ns1:Struct", kNS2, "ns2:Field3", &tmpStr1, &options );
		fprintf ( log, "GetStructField ns1:Struct/ns2:Field3 : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		tmpStr1.erase();
		ok = meta.GetQualifier ( kNS1, "QualProp1", kNS2, "Qual1", &tmpStr1, &options );
		fprintf ( log, "GetQualifier ns1:QualProp1/?ns2:Qual1 : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		try {
			tmpStr1.erase();
			ok = meta.GetQualifier ( 0, "ns1:QualProp1", kNS2, "Qual1", &tmpStr1, &options );
			fprintf ( log, "#ERROR: No exception for GetQualifier with no schema URI : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );
		} catch ( XMP_Error & excep ) {
			fprintf ( log, "GetQualifier with no schema URI - threw XMP_Error #%d : %s\n", excep.GetID(), excep.GetErrMsg() );
		} catch ( ... ) {
			fprintf ( log, "GetQualifier with no schema URI - threw unknown exception\n" );
		}

		tmpStr1.erase();
		ok = meta.GetQualifier ( kNS1, "ns1:QualProp3", kXMP_NS_XML, "xml:lang", &tmpStr1, &options );
		fprintf ( log, "GetQualifier ns1:QualProp3 : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		tmpStr1.erase();
		ok = meta.GetQualifier ( kNS1, "ns1:QualProp3", kNS2, "ns2:Qual", &tmpStr1, &options );
		fprintf ( log, "GetQualifier ns1:QualProp3/?ns2:Qual : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		fprintf ( log, "\n" );

		tmpStr1 = "junk";
		ok = meta.GetProperty ( kNS1, "Bag", &tmpStr1, &options );
		fprintf ( log, "GetProperty ns1:Bag : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		tmpStr1 = "junk";
		ok = meta.GetProperty ( kNS1, "Seq", &tmpStr1, &options );
		fprintf ( log, "GetProperty ns1:Seq : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		tmpStr1 = "junk";
		ok = meta.GetProperty ( kNS1, "Alt", &tmpStr1, &options );
		fprintf ( log, "GetProperty ns1:Alt : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		tmpStr1 = "junk";
		ok = meta.GetProperty ( kNS1, "Struct", &tmpStr1, &options );
		fprintf ( log, "GetProperty ns1:Struct : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

		fprintf ( log, "\n" );

		try {
			tmpStr1 = "junk";
			ok = meta.GetProperty ( "ns:bogus/", "Bogus", &tmpStr1, &options );
			fprintf ( log, "#ERROR: No exception for GetProperty with bogus schema URI: %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );
		} catch ( XMP_Error & excep ) {
			fprintf ( log, "GetProperty with bogus schema URI - threw XMP_Error #%d : %s\n", excep.GetID(), excep.GetErrMsg() );
		} catch ( ... ) {
			fprintf ( log, "GetProperty with bogus schema URI - threw unknown exception\n" );
		}

		tmpStr1 = "junk";
		ok = meta.GetProperty ( kNS1, "Bogus", &tmpStr1, &options );
		fprintf ( log, "GetProperty ns1:Bogus : %s\n", FoundOrNot ( ok ) );

		tmpStr1 = "junk";
		ok = meta.GetArrayItem ( kNS1, "Bag", 99, &tmpStr1, &options );
		fprintf ( log, "GetArrayItem ns1:Bag[99] : %s\n", FoundOrNot ( ok ) );

		tmpStr1 = "junk";
		ok = meta.GetStructField ( kNS1, "Struct", kNS2, "Bogus", &tmpStr1, &options );
		fprintf ( log, "GetStructField ns1:Struct/ns2:Bogus : %s\n", FoundOrNot ( ok ) );

		tmpStr1 = "junk";
		ok = meta.GetQualifier ( kNS1, "Prop", kNS2, "Bogus", &tmpStr1, &options );
		fprintf ( log, "GetQualifier ns1:Prop/?ns2:Bogus : %s\n", FoundOrNot ( ok ) );

		// ----------------------------------------------------------------------------------------

		WriteMajorLabel ( log, "Test DoesPropertyExist, DeleteProperty, and related methods" );

		DumpXMPObj ( log, meta, "XMP object" );
		fprintf ( log, "\n" );

		ok = meta.DoesPropertyExist ( kNS1, "Prop" );
		fprintf ( log, "DoesPropertyExist ns1:Prop : %s\n", YesOrNo ( ok ) );

		try {
			ok = meta.DoesPropertyExist ( 0, "ns1:Bag" );
			fprintf ( log, "#ERROR: No exception for DoesPropertyExist with no schema URI: %s\n", YesOrNo ( ok ) );
		} catch ( XMP_Error & excep ) {
			fprintf ( log, "DoesPropertyExist with no schema URI - threw XMP_Error #%d : %s\n", excep.GetID(), excep.GetErrMsg() );
		} catch ( ... ) {
			fprintf ( log, "DoesPropertyExist with no schema URI - threw unknown exception\n" );
		}

		ok = meta.DoesPropertyExist ( kNS1, "ns1:Struct" );
		fprintf ( log, "DoesPropertyExist ns1:Struct : %s\n", YesOrNo ( ok ) );

		fprintf ( log, "\n" );

		ok = meta.DoesArrayItemExist ( kNS1, "Bag", 2 );
		fprintf ( log, "DoesArrayItemExist ns1:Bag[2] : %s\n", YesOrNo ( ok ) );

		ok = meta.DoesArrayItemExist ( kNS1, "ns1:Seq", kXMP_ArrayLastItem );
		fprintf ( log, "DoesArrayItemExist ns1:Seq[last] : %s\n", YesOrNo ( ok ) );

		ok = meta.DoesStructFieldExist ( kNS1, "Struct", kNS2, "Field1" );
		fprintf ( log, "DoesStructFieldExist ns1:Struct/ns2:Field1 : %s\n", YesOrNo ( ok ) );

		ok = meta.DoesQualifierExist ( kNS1, "QualProp1", kNS2, "Qual1" );
		fprintf ( log, "DoesQualifierExist ns1:QualProp1/?ns2:Qual1 : %s\n", YesOrNo ( ok ) );

		ok = meta.DoesQualifierExist ( kNS1, "QualProp2", kXMP_NS_XML, "lang" );
		fprintf ( log, "DoesQualifierExist ns1:QualProp2/?xml:lang : %s\n", YesOrNo ( ok ) );

		fprintf ( log, "\n" );

		try {
			ok = meta.DoesPropertyExist ( "ns:bogus/", "Bogus" );
			fprintf ( log, "#ERROR: No exception for DoesPropertyExist with bogus schema URI: %s\n", YesOrNo ( ok ) );
		} catch ( XMP_Error & excep ) {
			fprintf ( log, "DoesPropertyExist with bogus schema URI - threw XMP_Error #%d : %s\n", excep.GetID(), excep.GetErrMsg() );
		} catch ( ... ) {
			fprintf ( log, "DoesPropertyExist with bogus schema URI - threw unknown exception\n" );
		}

		ok = meta.DoesPropertyExist ( kNS1, "Bogus" );
		fprintf ( log, "DoesPropertyExist ns1:Bogus : %s\n", YesOrNo ( ok ) );

		ok = meta.DoesArrayItemExist ( kNS1, "Bag", 99 );
		fprintf ( log, "DoesArrayItemExist ns1:Bag[99] : %s\n", YesOrNo ( ok ) );

		try {
			ok = meta.DoesArrayItemExist ( 0, "ns1:Bag", kXMP_ArrayLastItem );
			fprintf ( log, "#ERROR: No exception for DoesArrayItemExist with no schema URI: %s\n", YesOrNo ( ok ) );
		} catch ( XMP_Error & excep ) {
			fprintf ( log, "DoesArrayItemExist with no schema URI - threw XMP_Error #%d : %s\n", excep.GetID(), excep.GetErrMsg() );
		} catch ( ... ) {
			fprintf ( log, "DoesArrayItemExist with no schema URI - threw unknown exception\n" );
		}

		ok = meta.DoesStructFieldExist ( kNS1, "Struct", kNS2, "Bogus" );
		fprintf ( log, "DoesStructFieldExist ns1:Struct/ns2:Bogus : %s\n", YesOrNo ( ok ) );

		ok = meta.DoesQualifierExist ( kNS1, "Prop", kNS2, "Bogus" );
		fprintf ( log, "DoesQualifierExist ns1:Prop/?ns2:Bogus : %s\n", YesOrNo ( ok ) );

		meta.DeleteProperty ( kNS1, "Prop" );
		meta.DeleteArrayItem ( kNS1, "Bag", 2 );
		meta.DeleteStructField ( kNS1, "Struct", kNS2, "Field1" );

		DumpXMPObj ( log, meta, "Delete Prop, Bag[2], and Struct1/Field1" );

		meta.DeleteQualifier ( kNS1, "QualProp1", kNS2, "Qual1" );
		meta.DeleteQualifier ( kNS1, "QualProp2", kXMP_NS_XML, "lang" );
		meta.DeleteQualifier ( kNS1, "QualProp3", kNS2, "Qual" );
		meta.DeleteQualifier ( kNS1, "QualProp4", kXMP_NS_XML, "lang" );

		DumpXMPObj ( log, meta, "Delete QualProp1/?ns2:Qual1, QualProp2/?xml:lang, QualProp3:/ns2:Qual, and QualProp4/?xml:lang" );

		meta.DeleteProperty ( kNS1, "Bag" );
		meta.DeleteProperty ( kNS1, "Struct" );

		DumpXMPObj ( log, meta, "Delete all of Bag and Struct" );

	}

	// --------------------------------------------------------------------------------------------
	// Localized text set/get methods
	// ------------------------------

	{
		SXMPMeta meta;

		WriteMajorLabel ( log, "Test SetLocalizedText and GetLocalizedText" );

		tmpStr1 = "default value";
		meta.SetLocalizedText ( kNS1, "AltText", "", "x-default", tmpStr1 );
		DumpXMPObj ( log, meta, "Set x-default value" );

		meta.SetLocalizedText ( kNS1, "AltText", "en", "en-us", "en-us value" );
		DumpXMPObj ( log, meta, "Set en/en-us value" );

		meta.SetLocalizedText ( kNS1, "AltText", "en", "en-uk", "en-uk value" );
		DumpXMPObj ( log, meta, "Set en/en-uk value" );

		fprintf ( log, "\n" );

		tmpStr1.erase();  tmpStr2.erase();
		ok = meta.GetLocalizedText ( kNS1, "AltText", "en", "en-ca", &tmpStr1, &tmpStr2, &options );
		fprintf ( log, "GetLocalizedText en/en-ca : %s, \'%s\' \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), tmpStr2.c_str(), options );

		tmpStr1 = "junk";
		ok = meta.GetProperty ( kNS1, "AltText", &tmpStr1, &options );
		fprintf ( log, "GetProperty ns1:AltText : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options );

	}

	// --------------------------------------------------------------------------------------------
	// Binary value set/get methods
	// ----------------------------

	{
		SXMPMeta meta ( kDateTimeRDF, strlen(kDateTimeRDF) );
		XMP_DateTime dateValue;
		bool		boolValue;
		XMP_Int32	intValue;
		double		floatValue;
		char		dateName [8];

		WriteMajorLabel ( log, "Test SetProperty... and GetProperty... methods (set/get with binary values)" );
		
		FillDateTime ( &dateValue, 2000, 1, 2, 3, 4, 5, true, true, false, 0, 0, 0, 0 );

		meta.SetProperty_Bool ( kNS1, "Bool0", false );
		meta.SetProperty_Bool ( kNS1, "Bool1", true );
		meta.SetProperty_Int ( kNS1, "Int", 42 );
		meta.SetProperty_Float ( kNS1, "Float", 4.2 );

		meta.SetProperty_Date ( kNS1, "Date10", dateValue );
		dateValue.tzSign = 1; dateValue.tzHour = 6; dateValue.tzMinute = 7;
		meta.SetProperty_Date ( kNS1, "Date11", dateValue );
		dateValue.tzSign = -1;
		meta.SetProperty_Date ( kNS1, "Date12", dateValue );
		dateValue.nanoSecond = 9;
		meta.SetProperty_Date ( kNS1, "Date13", dateValue );

		DumpXMPObj ( log, meta, "A few basic binary Set... calls" );

		fprintf ( log, "\n" );

		ok = meta.GetProperty_Bool ( kNS1, "Bool0", &boolValue, &options );
		fprintf ( log, "GetProperty_Bool Bool0 : %s, %d, 0x%X\n", FoundOrNot ( ok ), boolValue, options );

		ok = meta.GetProperty_Bool ( kNS1, "Bool1", &boolValue, &options );
		fprintf ( log, "GetProperty_Bool Bool1 : %s, %d, 0x%X\n", FoundOrNot ( ok ), boolValue, options );

		ok = meta.GetProperty_Int ( kNS1, "Int", &intValue, &options );
		fprintf ( log, "GetProperty_Int : %s, %d, 0x%X\n", FoundOrNot ( ok ), intValue, options );

		ok = meta.GetProperty_Float ( kNS1, "Float", &floatValue, &options );
		fprintf ( log, "GetProperty_Float : %s, %f, 0x%X\n", FoundOrNot ( ok ), floatValue, options );

		fprintf ( log, "\n" );

		for ( i = 1; i < 14; ++i ) {
			sprintf ( dateName, "Date%d", i );
			ok = meta.GetProperty_Date ( kNS1, dateName, &dateValue, &options );
			fprintf ( log, "GetProperty_Date (%s) : %s, %d-%02d-%02d %02d:%02d:%02d %d*%02d:%02d %d, 0x%X\n",  dateName, FoundOrNot ( ok ),
					  dateValue.year, dateValue.month, dateValue.day, dateValue.hour, dateValue.minute, dateValue.second,
					  dateValue.tzSign, dateValue.tzHour, dateValue.tzMinute, dateValue.nanoSecond, options );
			meta.SetProperty_Date ( kNS2, dateName, dateValue );
		}

		DumpXMPObj ( log, meta, "Get and re-set the dates" );

	}

	// --------------------------------------------------------------------------------------------
	// Parse and serialize methods
	// ---------------------------

	WriteMajorLabel ( log, "Test parsing with multiple buffers and various options" );

	{
		SXMPMeta meta;
		for ( i = 0; i < (long)strlen(kSimpleRDF) - 10; i += 10 ) {
			meta.ParseFromBuffer ( &kSimpleRDF[i], 10, kXMP_ParseMoreBuffers );
		}
		meta.ParseFromBuffer ( &kSimpleRDF[i], strlen(kSimpleRDF) - i );
		DumpXMPObj ( log, meta, "Multiple buffer parse" );
	}

	{
		SXMPMeta meta;
		for ( i = 0; i < (long)strlen(kSimpleRDF) - 10; i += 10 ) {
			meta.ParseFromBuffer ( &kSimpleRDF[i], 10, kXMP_ParseMoreBuffers );
		}
		meta.ParseFromBuffer ( &kSimpleRDF[i], (strlen(kSimpleRDF) - i), kXMP_ParseMoreBuffers );
		meta.ParseFromBuffer ( kSimpleRDF, 0 );
		DumpXMPObj ( log, meta, "Multiple buffer parse, empty last buffer" );
	}

	{
		SXMPMeta meta;
		for ( i = 0; i < (long)strlen(kSimpleRDF) - 10; i += 10 ) {
			meta.ParseFromBuffer ( &kSimpleRDF[i], 10, kXMP_ParseMoreBuffers );
		}
		meta.ParseFromBuffer ( &kSimpleRDF[i], (strlen(kSimpleRDF) - i), kXMP_ParseMoreBuffers );
		meta.ParseFromBuffer ( 0, 0 );
		DumpXMPObj ( log, meta, "Multiple buffer parse, null last buffer" );
	}
	
	{
		SXMPMeta meta;
		meta.ParseFromBuffer ( kSimpleRDF, strlen(kSimpleRDF), kXMP_RequireXMPMeta );
		DumpXMPObj ( log, meta, "Parse and require xmpmeta element, which is missing" );
	}
	
	{
		SXMPMeta meta;
		meta.ParseFromBuffer ( kNamespaceRDF, strlen(kNamespaceRDF) );
		DumpXMPObj ( log, meta, "Parse RDF with multiple nested namespaces" );
	}
	
	{
		SXMPMeta meta;
		meta.ParseFromBuffer ( kXMPMetaRDF, strlen(kXMPMetaRDF), kXMP_RequireXMPMeta );
		DumpXMPObj ( log, meta, "Parse and require xmpmeta element, which is present" );
	}
	
	{
		SXMPMeta meta;
		meta.ParseFromBuffer ( kInconsistentRDF, strlen(kInconsistentRDF) );
		DumpXMPObj ( log, meta, "Parse and reconcile inconsistent aliases" );
	}

	try {
		SXMPMeta meta;
		meta.ParseFromBuffer ( kInconsistentRDF, strlen(kInconsistentRDF), kXMP_StrictAliasing );
		DumpXMPObj ( log, meta, "ERROR: Parse and do not reconcile inconsistent aliases - should have thrown an exception" );
	} catch ( XMP_Error & excep ) {
		fprintf ( log, "\nParse and do not reconcile inconsistent aliases - threw XMP_Error #%d : %s\n", excep.GetID(), excep.GetErrMsg() );
	} catch ( ... ) {
		fprintf ( log, "\nParse and do not reconcile inconsistent aliases - threw unknown exception\n" );
	}

	{
		WriteMajorLabel ( log, "Test CR and LF in values" );
		
		const char *	kValueWithCR	= "ASCII \x0D CR";
		const char *	kValueWithLF	= "ASCII \x0A LF";
		const char *	kValueWithCRLF	= "ASCII \x0D\x0A CRLF";

		SXMPMeta meta ( kNewlineRDF, kXMP_UseNullTermination );

		meta.SetProperty ( kNS2, "HasCR", kValueWithCR );
		meta.SetProperty ( kNS2, "HasLF", kValueWithLF );
		meta.SetProperty ( kNS2, "HasCRLF", kValueWithCRLF );

		tmpStr1.erase();
		meta.SerializeToBuffer ( &tmpStr1, kXMP_OmitPacketWrapper );
		fprintf ( log, "\n%s\n", tmpStr1.c_str() );
		
		tmpStr1.erase();  tmpStr2.erase();
		ok = meta.GetProperty ( kNS1, "HasCR", &tmpStr1, 0 );
		ok = meta.GetProperty ( kNS2, "HasCR", &tmpStr2, 0 );
		if ( (tmpStr1 != kValueWithCR) || (tmpStr2 != kValueWithCR) ) fprintf ( log, "\n ## HasCR values are bad\n" );
		
		tmpStr1.erase();  tmpStr2.erase();
		ok = meta.GetProperty ( kNS1, "HasLF", &tmpStr1, 0 );
		ok = meta.GetProperty ( kNS2, "HasLF", &tmpStr2, 0 );
		if ( (tmpStr1 != kValueWithLF) || (tmpStr2 != kValueWithLF) ) fprintf ( log, "\n ## HasLF values are bad\n" );
		
		tmpStr1.erase();  tmpStr2.erase();
		ok = meta.GetProperty ( kNS1, "HasCRLF", &tmpStr1, 0 );
		ok = meta.GetProperty ( kNS2, "HasCRLF", &tmpStr2, 0 );
		if ( (tmpStr1 != kValueWithCRLF) || (tmpStr2 != kValueWithCRLF) ) fprintf ( log, "\n ## HasCRLF values are bad\n" );
	}
	
	{
		WriteMajorLabel ( log, "Test serialization with various options" );

		SXMPMeta meta ( kSimpleRDF, strlen(kSimpleRDF) );
		meta.SetProperty ( kNS2, "Another", "Something in another schema" );
		meta.SetProperty ( kNS2, "Yet/pdf:More", "Yet more in another schema" );

		DumpXMPObj ( log, meta, "Parse simple RDF, serialize with various options" );

		tmpStr1.erase();
		meta.SerializeToBuffer ( &tmpStr1 );
		WriteMinorLabel ( log, "Default serialize" );
		fprintf ( log, "%s\n", tmpStr1.c_str() );  fflush ( log );
		VerifyNewlines ( log, tmpStr1, "\x0A" );
		
		SXMPMeta meta2 ( tmpStr1.c_str(), tmpStr1.size() );
		DumpXMPObj ( log, meta2, "Reparse default serialization" );

		tmpStr1.erase();
		meta.SerializeToBuffer ( &tmpStr1, kXMP_OmitPacketWrapper | kXMP_UseCompactFormat );
		WriteMinorLabel ( log, "Compact RDF, no packet serialize" );
		fprintf ( log, "%s\n", tmpStr1.c_str() );
		
		SXMPMeta meta3 ( tmpStr1.c_str(), tmpStr1.size() );
		DumpXMPObj ( log, meta3, "Reparse compact serialization" );

		{
			SXMPMeta meta2;

			meta2.SetProperty ( kXMP_NS_PDF, "Author", "PDF Author" );
			
			tmpStr1.erase();
			meta2.SerializeToBuffer ( &tmpStr1, kXMP_ReadOnlyPacket );
			WriteMinorLabel ( log, "Read-only serialize with alias comments" );
			fprintf ( log, "%s\n", tmpStr1.c_str() );

			meta2.SetProperty ( kXMP_NS_PDF, "Actual", "PDF Actual" );
			meta2.SetProperty ( kXMP_NS_XMP, "Actual", "XMP Actual" );
			
			tmpStr1.erase();
			meta2.SerializeToBuffer ( &tmpStr1, kXMP_ReadOnlyPacket );
			WriteMinorLabel ( log, "Read-only serialize with alias comments (more actuals)" );
			fprintf ( log, "%s\n", tmpStr1.c_str() );
		}

		tmpStr1.erase();
		meta.SerializeToBuffer ( &tmpStr1, kXMP_OmitPacketWrapper, 0, "\x0D" );
		WriteMinorLabel ( log, "CR newline serialize" );
		fprintf ( log, "%s\n", tmpStr1.c_str() );
		VerifyNewlines ( log, tmpStr1, "\x0D" );

		tmpStr1.erase();
		meta.SerializeToBuffer ( &tmpStr1, kXMP_OmitPacketWrapper, 0, "\x0D\x0A" );
		WriteMinorLabel ( log, "CRLF newline serialize" );
		fprintf ( log, "%s\n", tmpStr1.c_str() );
		VerifyNewlines ( log, tmpStr1, "\x0D\x0A" );

		tmpStr1.erase();
		meta.SerializeToBuffer ( &tmpStr1, kXMP_OmitPacketWrapper, 0, "<->" );
		WriteMinorLabel ( log, "Alternate newline serialize" );
		fprintf ( log, "%s\n", tmpStr1.c_str() );

		tmpStr1.erase();
		meta.SerializeToBuffer ( &tmpStr1, kXMP_OmitPacketWrapper, 0, "", "#", 3 );
		WriteMinorLabel ( log, "Alternate indent serialize" );
		fprintf ( log, "%s\n", tmpStr1.c_str() );

		tmpStr1.erase();
		meta.SerializeToBuffer ( &tmpStr1, 0, 10 );
		WriteMinorLabel ( log, "Small padding serialize" );
		fprintf ( log, "%s\n", tmpStr1.c_str() );

		tmpStr1.erase();
		tmpStr2.erase();
		meta.SerializeToBuffer ( &tmpStr1 );
		meta.SerializeToBuffer ( &tmpStr2, kXMP_IncludeThumbnailPad );
		fprintf ( log, "Thumbnailpad adds %zd bytes\n", tmpStr2.size()-tmpStr1.size() );
		
		tmpStr1.erase();
		meta.SerializeToBuffer ( &tmpStr1, kXMP_ReadOnlyPacket );
		size_t minSize = tmpStr1.size();
		fprintf ( log, "Minimum packet size is %zd bytes\n", minSize );
		
		tmpStr1.erase();
		meta.SerializeToBuffer ( &tmpStr1, kXMP_ExactPacketLength, minSize+1234 );
		fprintf ( log, "Minimum+1234 packet size is %zd bytes\n", tmpStr1.size() );
		if ( tmpStr1.size() != (minSize + 1234) ) fprintf ( log, "** Bad packet length **\n" );
		
		tmpStr1.erase();
		meta.SerializeToBuffer ( &tmpStr1, kXMP_ExactPacketLength, minSize );
		fprintf ( log, "Minimum+0 packet size is %zd bytes\n", tmpStr1.size() );
		if ( tmpStr1.size() != minSize ) fprintf ( log, "** Bad packet length **\n" );

		try {
			tmpStr1.erase();
			meta.SerializeToBuffer ( &tmpStr1, kXMP_ExactPacketLength, minSize-1 );
			fprintf ( log, "#ERROR: No exception for minimum-1, size is %zd bytes **\n", tmpStr1.size() );
		} catch ( XMP_Error & excep ) {
			fprintf ( log, "Serialize in minimum-1 - threw XMP_Error #%d : %s\n", excep.GetID(), excep.GetErrMsg() );
		} catch ( ... ) {
			fprintf ( log, "Serialize in minimum-1 - threw unknown exception\n" );
		}
		
		// *** UTF-16 and UTF-32 encodings

	}

	// --------------------------------------------------------------------------------------------
	// Iteration methods
	// -----------------

	{
		WriteMajorLabel ( log, "Test iteration methods" );

		SXMPMeta meta ( kRDFCoverage, strlen ( kRDFCoverage ) );
		XMP_OptionBits opt2;

		meta.SetProperty ( kNS2, "Prop", "Prop value" );

		meta.SetProperty ( kNS2, "Bag", 0, kXMP_PropValueIsArray );
		meta.SetArrayItem ( kNS2, "Bag", 1, "BagItem 2" );
		meta.SetArrayItem ( kNS2, "Bag", 1, "BagItem 1", kXMP_InsertBeforeItem );
		meta.SetArrayItem ( kNS2, "Bag", 2, "BagItem 3", kXMP_InsertAfterItem );

		DumpXMPObj ( log, meta, "Parse \"coverage\" RDF, add Bag items out of order" );

		{
			SXMPIterator iter ( meta );
			WriteMinorLabel ( log, "Default iteration" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
				if ( ! (options & kXMP_SchemaNode) ) {
					tmpStr4.erase();
					ok = meta.GetProperty ( tmpStr1.c_str(), tmpStr2.c_str(), &tmpStr4, &opt2 );
					if ( (! ok) || (tmpStr4 != tmpStr3) || (opt2 != options) ) {
						fprintf ( log, "    ** GetProperty failed: %s, \"%s\", 0x%X\n", FoundOrNot(ok), tmpStr4.c_str(), opt2 );
					}
				}
			}
		}

		{
			SXMPIterator iter ( meta, kXMP_IterOmitQualifiers );
			WriteMinorLabel ( log, "Iterate omitting qualifiers" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
				if ( ! (options & kXMP_SchemaNode) ) {
					tmpStr4.erase();
					ok = meta.GetProperty ( tmpStr1.c_str(), tmpStr2.c_str(), &tmpStr4, &opt2 );
					if ( (! ok) || (tmpStr4 != tmpStr3) || (opt2 != options) ) {
						fprintf ( log, "    ** GetProperty failed: %s, \"%s\", 0x%X\n", FoundOrNot(ok), tmpStr4.c_str(), opt2 );
					}
				}
			}
		}

		{
			SXMPIterator iter ( meta, kXMP_IterJustLeafName );
			WriteMinorLabel ( log, "Iterate with just leaf names" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
			}
		}

		{
			SXMPIterator iter ( meta, kXMP_IterJustLeafNodes );
			WriteMinorLabel ( log, "Iterate just the leaf nodes" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
				if ( ! (options & kXMP_SchemaNode) ) {
					tmpStr4.erase();
					ok = meta.GetProperty ( tmpStr1.c_str(), tmpStr2.c_str(), &tmpStr4, &opt2 );
					if ( (! ok) || (tmpStr4 != tmpStr3) || (opt2 != options) ) {
						fprintf ( log, "    ** GetProperty failed: %s, \"%s\", 0x%X\n", FoundOrNot(ok), tmpStr4.c_str(), opt2 );
					}
				}
			}
		}

		{
			SXMPIterator iter ( meta, kXMP_IterJustChildren );
			WriteMinorLabel ( log, "Iterate just the schema nodes" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
			}
		}

		{
			SXMPIterator iter ( meta, kNS2 );
			WriteMinorLabel ( log, "Iterate the ns2: namespace" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
				if ( ! (options & kXMP_SchemaNode) ) {
					tmpStr4.erase();
					ok = meta.GetProperty ( tmpStr1.c_str(), tmpStr2.c_str(), &tmpStr4, &opt2 );
					if ( (! ok) || (tmpStr4 != tmpStr3) || (opt2 != options) ) {
						fprintf ( log, "    ** GetProperty failed: %s, \"%s\", 0x%X\n", FoundOrNot(ok), tmpStr4.c_str(), opt2 );
					}
				}
			}
		}

		{
			SXMPIterator iter ( meta, kNS2, "Bag" );
			WriteMinorLabel ( log, "Start at ns2:Bag" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
				if ( ! (options & kXMP_SchemaNode) ) {
					tmpStr4.erase();
					ok = meta.GetProperty ( tmpStr1.c_str(), tmpStr2.c_str(), &tmpStr4, &opt2 );
					if ( (! ok) || (tmpStr4 != tmpStr3) || (opt2 != options) ) {
						fprintf ( log, "    ** GetProperty failed: %s, \"%s\", 0x%X\n", FoundOrNot(ok), tmpStr4.c_str(), opt2 );
					}
				}
			}
		}

		{
			SXMPIterator iter ( meta, kNS2, "NestedStructProp/ns1:Outer" );
			WriteMinorLabel ( log, "Start at ns2:NestedStructProp/ns1:Outer" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
				if ( ! (options & kXMP_SchemaNode) ) {
					tmpStr4.erase();
					ok = meta.GetProperty ( tmpStr1.c_str(), tmpStr2.c_str(), &tmpStr4, &opt2 );
					if ( (! ok) || (tmpStr4 != tmpStr3) || (opt2 != options) ) {
						fprintf ( log, "    ** GetProperty failed: %s, \"%s\", 0x%X\n", FoundOrNot(ok), tmpStr4.c_str(), opt2 );
					}
				}
			}
		}

		{
			SXMPIterator iter ( meta, "ns:empty/" );
			WriteMinorLabel ( log, "Iterate an empty namespace" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
				if ( ! (options & kXMP_SchemaNode) ) {
					tmpStr4.erase();
					ok = meta.GetProperty ( tmpStr1.c_str(), tmpStr2.c_str(), &tmpStr4, &opt2 );
					if ( (! ok) || (tmpStr4 != tmpStr3) || (opt2 != options) ) {
						fprintf ( log, "    ** GetProperty failed: %s, \"%s\", 0x%X\n", FoundOrNot(ok), tmpStr4.c_str(), opt2 );
					}
				}
			}
		}

		{
			SXMPIterator iter ( meta, kNS2, "", kXMP_IterJustChildren | kXMP_IterJustLeafName );
			WriteMinorLabel ( log, "Iterate the top of the ns2: namespace with just leaf names" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
			}
		}

		{
			SXMPIterator iter ( meta, kNS2, "", kXMP_IterJustChildren | kXMP_IterJustLeafNodes );
			WriteMinorLabel ( log, "Iterate the top of the ns2: namespace visiting just leaf nodes" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
				if ( ! (options & kXMP_SchemaNode) ) {
					tmpStr4.erase();
					ok = meta.GetProperty ( tmpStr1.c_str(), tmpStr2.c_str(), &tmpStr4, &opt2 );
					if ( (! ok) || (tmpStr4 != tmpStr3) || (opt2 != options) ) {
						fprintf ( log, "    ** GetProperty failed: %s, \"%s\", 0x%X\n", FoundOrNot(ok), tmpStr4.c_str(), opt2 );
					}
				}
			}
		}

		{
			SXMPIterator iter ( meta, kNS2, "Bag", kXMP_IterJustChildren );
			WriteMinorLabel ( log, "Iterate just the children of ns2:Bag" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
				if ( ! (options & kXMP_SchemaNode) ) {
					tmpStr4.erase();
					ok = meta.GetProperty ( tmpStr1.c_str(), tmpStr2.c_str(), &tmpStr4, &opt2 );
					if ( (! ok) || (tmpStr4 != tmpStr3) || (opt2 != options) ) {
						fprintf ( log, "    ** GetProperty failed: %s, \"%s\", 0x%X\n", FoundOrNot(ok), tmpStr4.c_str(), opt2 );
					}
				}
			}
		}

		{
			SXMPIterator iter ( meta, kNS2, "Bag", kXMP_IterJustChildren | kXMP_IterJustLeafName );
			WriteMinorLabel ( log, "Iterate just the children of ns2:Bag with just leaf names" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
			}
		}

		{
			SXMPIterator iter ( meta, kNS2, "NestedStructProp/ns1:Outer/ns1:Middle", kXMP_IterJustChildren );
			WriteMinorLabel ( log, "Iterate just the children of ns2:NestedStructProp/ns1:Outer/ns1:Middle" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
				if ( ! (options & kXMP_SchemaNode) ) {
					tmpStr4.erase();
					ok = meta.GetProperty ( tmpStr1.c_str(), tmpStr2.c_str(), &tmpStr4, &opt2 );
					if ( (! ok) || (tmpStr4 != tmpStr3) || (opt2 != options) ) {
						fprintf ( log, "    ** GetProperty failed: %s, \"%s\", 0x%X\n", FoundOrNot(ok), tmpStr4.c_str(), opt2 );
					}
				}
			}
		}

		{
			SXMPIterator iter ( meta );
			WriteMinorLabel ( log, "Skip children of ArrayProp2, and siblings after StructProp" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
				if ( ! (options & kXMP_SchemaNode) ) {
					tmpStr4.erase();
					ok = meta.GetProperty ( tmpStr1.c_str(), tmpStr2.c_str(), &tmpStr4, &opt2 );
					if ( (! ok) || (tmpStr4 != tmpStr3) || (opt2 != options) ) {
						fprintf ( log, "    ** GetProperty failed: %s, \"%s\", 0x%X\n", FoundOrNot(ok), tmpStr4.c_str(), opt2 );
					}
				}
				if ( tmpStr2 == "ns1:ArrayProp2" ) iter.Skip ( kXMP_IterSkipSubtree );
				if ( tmpStr2 == "ns1:StructProp" ) iter.Skip ( kXMP_IterSkipSiblings );
			}
		}
		
		{
			SXMPMeta meta;
			
			meta.SetProperty ( kXMP_NS_PDF, "Author", "PDF Author" );
			meta.SetProperty ( kXMP_NS_PDF, "PDFProp", "PDF Prop" );
			meta.SetProperty ( kXMP_NS_XMP, "XMPProp", "XMP Prop" );
			meta.SetProperty ( kXMP_NS_DC, "DCProp", "DC Prop" );

			SXMPIterator iter1 ( meta );
			WriteMinorLabel ( log, "Iterate without showing aliases" );
			while ( true ) {
				tmpStr1.erase();  tmpStr2.erase();  tmpStr3.erase();
				if ( ! iter1.Next ( &tmpStr1, &tmpStr2, &tmpStr3, &options ) ) break;
				fprintf ( log, "  %s %s = \"%s\", 0x%X\n", tmpStr1.c_str(), tmpStr2.c_str(), tmpStr3.c_str(), options );
				if ( ! (options & kXMP_SchemaNode) ) {
					tmpStr4.erase();
					options &= kXMP_PropHasAliases;	// So the comparison below works.
					ok = meta.GetProperty ( tmpStr1.c_str(), tmpStr2.c_str(), &tmpStr4, &opt2 );
					if ( (! ok) || (tmpStr4 != tmpStr3) || (opt2 != options) ) {
						fprintf ( log, "    ** GetProperty failed: %s, \"%s\", 0x%X\n", FoundOrNot(ok), tmpStr4.c_str(), opt2 );
					}
				}
			}
		}

	}

	// --------------------------------------------------------------------------------------------
	// XPath composition utilities
	// ---------------------------

	{
		WriteMajorLabel ( log, "Test XPath composition utilities" );

		SXMPMeta meta ( kSimpleRDF, strlen(kSimpleRDF) );
		DumpXMPObj ( log, meta, "Parse simple RDF" );
		fprintf ( log, "\n" );
	
		tmpStr1.erase();
		SXMPUtils::ComposeArrayItemPath ( kNS1, "ArrayProp", 2, &tmpStr1 );
		fprintf ( log, "ComposeArrayItemPath ns1:ArrayProp[2] : %s\n", tmpStr1.c_str() );
		meta.SetProperty ( kNS1, tmpStr1.c_str(), "new ns1:ArrayProp[2] value" );

		fprintf ( log, "\n" );

		tmpStr1.erase();
		SXMPUtils::ComposeStructFieldPath ( kNS1, "StructProp", kNS2, "Field3", &tmpStr1 );
		fprintf ( log, "ComposeStructFieldPath ns1:StructProp/ns2:Field3 : %s\n", tmpStr1.c_str() );
		meta.SetProperty ( kNS1, tmpStr1.c_str(), "new ns1:StructProp/ns2:Field3 value" );

		tmpStr1.erase();
		SXMPUtils::ComposeQualifierPath ( kNS1, "QualProp", kNS2, "Qual", &tmpStr1 );
		fprintf ( log, "ComposeQualifierPath ns1:QualProp/?ns2:Qual : %s\n", tmpStr1.c_str() );
		meta.SetProperty ( kNS1, tmpStr1.c_str(), "new ns1:QualProp/?ns2:Qual value" );

		fprintf ( log, "\n" );

		tmpStr1.erase();
		SXMPUtils::ComposeQualifierPath ( kNS1, "AltTextProp", kXMP_NS_XML, "lang", &tmpStr1 );
		fprintf ( log, "ComposeQualifierPath ns1:AltTextProp/?xml:lang : %s\n", tmpStr1.c_str() );
		meta.SetProperty ( kNS1, tmpStr1.c_str(), "new ns1:AltTextProp/?xml:lang value" );

		tmpStr1.erase();
		tmpStr2 = "x-two";
		SXMPUtils::ComposeLangSelector ( kNS1, "AltTextProp", tmpStr2, &tmpStr1 );
		fprintf ( log, "ComposeLangSelector ns1:AltTextProp['x-two'] : %s\n", tmpStr1.c_str() );
		meta.SetProperty ( kNS1, tmpStr1.c_str(), "new ns1:AltTextProp['x-two'] value" );

		fprintf ( log, "\n" );
		
		fprintf ( log, "Check field selector usage\n" ); fflush ( log );

		tmpStr1.erase();
		ok = meta.GetProperty ( kNS1, "ArrayOfStructProp[ns2:Field1='Item-2']", &tmpStr1, &options );
		fprintf ( log, "GetProperty ArrayOfStructProp[ns2:Field1='Item-2'] : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options ); fflush ( log );

		tmpStr1.erase();
		ok = meta.GetProperty ( kNS1, "ArrayOfStructProp[ns2:Field1='Item-2']/ns2:Field2", &tmpStr1, &options );
		fprintf ( log, "GetProperty ArrayOfStructProp[ns2:Field1='Item-2']/ns2:Field2 : %s, \"%s\", 0x%X\n", FoundOrNot ( ok ), tmpStr1.c_str(), options ); fflush ( log );

		tmpStr1.erase();
		tmpStr2 = "Item-2";
		SXMPUtils::ComposeFieldSelector ( kNS1, "ArrayOfStructProp", kNS2, "Field1", tmpStr2, &tmpStr1 );
		fprintf ( log, "ComposeFieldSelector ns1:ArrayOfStructProp[ns2:Field1=Item-2] : %s\n", tmpStr1.c_str() );

		tmpStr2.erase();
		SXMPUtils::ComposeStructFieldPath ( kNS1, tmpStr1.c_str(), kNS2, "Field2", &tmpStr2 );
		fprintf ( log, "ComposeStructFieldPath ns1:ArrayOfStructProp[ns2:Field1=Item-2]/ns2:Field2 : %s\n", tmpStr2.c_str() );
		meta.SetProperty ( kNS1, tmpStr2.c_str(), "new ns1:ArrayOfStructProp[ns2:Field1=Item-2]/ns2:Field2 value" );

		DumpXMPObj ( log, meta, "Modified simple RDF" );
	
	}

	// --------------------------------------------------------------------------------------------
	// Value conversion utilities
	// --------------------------

	WriteMajorLabel ( log, "Test value conversion utilities" );
	fprintf ( log, "\n" );

	tmpStr1.erase();
	SXMPUtils::ConvertFromBool ( true, &tmpStr1 );
	fprintf ( log, "ConverFromBool true : %s\n", tmpStr1.c_str() );
	tmpStr1.erase();
	SXMPUtils::ConvertFromBool ( false, &tmpStr1 );
	fprintf ( log, "ConverFromBool false : %s\n", tmpStr1.c_str() );

	fprintf ( log, "\n" );

	ok = SXMPUtils::ConvertToBool ( kXMP_TrueStr );
	fprintf ( log, "ConverToBool kXMP_TrueStr : %d\n", (int)ok );
	ok = SXMPUtils::ConvertToBool ( kXMP_FalseStr );
	fprintf ( log, "ConverToBool kXMP_FalseStr : %d\n", (int)ok );

	fprintf ( log, "\n" );

	tmpStr1 = "true";
	ok = SXMPUtils::ConvertToBool ( tmpStr1 );
	fprintf ( log, "ConverToBool true : %d\n", (int)ok );
	ok = SXMPUtils::ConvertToBool ( "TRUE" );
	fprintf ( log, "ConverToBool TRUE : %d\n", (int)ok );
	ok = SXMPUtils::ConvertToBool ( "t" );
	fprintf ( log, "ConverToBool t : %d\n", (int)ok );
	ok = SXMPUtils::ConvertToBool ( "1" );
	fprintf ( log, "ConverToBool 1 : %d\n", (int)ok );

	fprintf ( log, "\n" );

	ok = SXMPUtils::ConvertToBool ( "false" );
	fprintf ( log, "ConverToBool false : %d\n", (int)ok );
	ok = SXMPUtils::ConvertToBool ( "FALSE" );
	fprintf ( log, "ConverToBool FALSE : %d\n", (int)ok );
	ok = SXMPUtils::ConvertToBool ( "f" );
	fprintf ( log, "ConverToBool f : %d\n", (int)ok );
	ok = SXMPUtils::ConvertToBool ( "0" );
	fprintf ( log, "ConverToBool 0 : %d\n", (int)ok );

	fprintf ( log, "\n" );

	tmpStr1.erase();
	SXMPUtils::ConvertFromInt ( 0, 0, &tmpStr1 );
	fprintf ( log, "ConverFromInt 0 : %s\n", tmpStr1.c_str() );
	tmpStr1.erase();
	SXMPUtils::ConvertFromInt ( 42, 0, &tmpStr1 );
	fprintf ( log, "ConverFromInt 42 : %s\n", tmpStr1.c_str() );
	tmpStr1.erase();
	SXMPUtils::ConvertFromInt ( -42, 0, &tmpStr1 );
	fprintf ( log, "ConverFromInt -42 : %s\n", tmpStr1.c_str() );
	tmpStr1.erase();
	SXMPUtils::ConvertFromInt ( 0x7FFFFFFF, 0, &tmpStr1 );
	fprintf ( log, "ConverFromInt 0x7FFFFFFF : %s\n", tmpStr1.c_str() );
	tmpStr1.erase();
	SXMPUtils::ConvertFromInt ( 0x80000000, 0, &tmpStr1 );
	fprintf ( log, "ConverFromInt 0x80000000 : %s\n", tmpStr1.c_str() );
	tmpStr1.erase();
	SXMPUtils::ConvertFromInt ( 0x7FFFFFFF, "%X", &tmpStr1 );
	fprintf ( log, "ConverFromInt 0x7FFFFFFF as hex : %s\n", tmpStr1.c_str() );
	tmpStr1.erase();
	SXMPUtils::ConvertFromInt ( 0x80000000, "%X", &tmpStr1 );
	fprintf ( log, "ConverFromInt 0x80000000 as hex : %s\n", tmpStr1.c_str() );

	fprintf ( log, "\n" );

	long	int1;

	tmpStr1 = "0";
	int1 = SXMPUtils::ConvertToInt ( tmpStr1 );
	fprintf ( log, "ConvertToInt 0 : %ld\n", int1 );
	int1 = SXMPUtils::ConvertToInt ( "42" );
	fprintf ( log, "ConvertToInt 42 : %ld\n", int1 );
	int1 = SXMPUtils::ConvertToInt ( "-42" );
	fprintf ( log, "ConvertToInt -42 : %ld\n", int1 );
	int1 = SXMPUtils::ConvertToInt ( "0x7FFFFFFF" );
	fprintf ( log, "ConvertToInt 0x7FFFFFFF : %ld\n", int1 );
	int1 = SXMPUtils::ConvertToInt ( "0x80000000" );
	fprintf ( log, "ConvertToInt 0x80000000 : %ld\n", int1 );
	int1 = SXMPUtils::ConvertToInt ( "0x7FFFFFFF" );
	fprintf ( log, "ConvertToInt 0x7FFFFFFF as hex : %lX\n", int1 );
	int1 = SXMPUtils::ConvertToInt ( "0x80000000" );
	fprintf ( log, "ConvertToInt 0x80000000 as hex : %lX\n", int1 );

	fprintf ( log, "\n" );

	tmpStr1.erase();
	SXMPUtils::ConvertFromFloat ( 0, 0, &tmpStr1 );
	fprintf ( log, "ConverFromFloat 0 : %s\n", tmpStr1.c_str() );
	tmpStr1.erase();
	SXMPUtils::ConvertFromFloat ( 4.2, 0, &tmpStr1 );
	fprintf ( log, "ConverFromFloat 4.2 : %s\n", tmpStr1.c_str() );
	tmpStr1.erase();
	SXMPUtils::ConvertFromFloat ( -4.2, 0, &tmpStr1 );
	fprintf ( log, "ConverFromFloat -4.2 : %s\n", tmpStr1.c_str() );
	tmpStr1.erase();
	SXMPUtils::ConvertFromFloat ( (int)0x7FFFFFFF, 0, &tmpStr1 );
	fprintf ( log, "ConverFromFloat 0x7FFFFFFF : %s\n", tmpStr1.c_str() );
	tmpStr1.erase();
	SXMPUtils::ConvertFromFloat ( (int)0x80000000, 0, &tmpStr1 );
	fprintf ( log, "ConverFromFloat 0x80000000 : %s\n", tmpStr1.c_str() );
	tmpStr1.erase();
	SXMPUtils::ConvertFromFloat ( (int)0x7FFFFFFF, "%f", &tmpStr1 );
	fprintf ( log, "ConverFromFloat 0x7FFFFFFF as f : %s\n", tmpStr1.c_str() );
	tmpStr1.erase();
	SXMPUtils::ConvertFromFloat ( (int)0x80000000, "%f", &tmpStr1 );
	fprintf ( log, "ConverFromFloat 0x80000000 as f : %s\n", tmpStr1.c_str() );

	fprintf ( log, "\n" );

	double	float1;

	tmpStr1 = "0";
	float1 = SXMPUtils::ConvertToFloat ( tmpStr1 );
	fprintf ( log, "ConvertToFloat 0 : %f\n", float1 );
	float1 = SXMPUtils::ConvertToFloat ( "4.2" );
	fprintf ( log, "ConvertToFloat 4.2 : %f\n", float1 );
	float1 = SXMPUtils::ConvertToFloat ( "-4.2" );
	fprintf ( log, "ConvertToFloat -4.2 : %f\n", float1 );

	fprintf ( log, "\n" );

	XMP_DateTime date1, date2;
	FillDateTime ( &date1, 2000, 1, 31, 12, 34, 56, true, true, true, -1, 8, 0, 0 );


	tmpStr1.erase();
	SXMPUtils::ConvertFromDate ( date1, &tmpStr1 );
	fprintf ( log, "ConvertFromDate 2000 Jan 31 12:34:56 PST : %s\n", tmpStr1.c_str() );

	SXMPUtils::ConvertToDate ( tmpStr1, &date2 );
	fprintf ( log, "ConvertToDate : %d-%02d-%02d %02d:%02d:%02d %d*%02d:%02d %d\n",
			  date2.year, date2.month, date2.day, date2.hour, date2.minute, date2.second,
			  date2.tzSign, date2.tzHour, date2.tzMinute, date2.nanoSecond );

	// --------------------------------------------------------------------------------------------
	// Date/Time utilities
	// -------------------

	{
		WriteMajorLabel ( log, "Test date/time utilities and special values" );
		fprintf ( log, "\n" );
		
		XMP_DateTime utcNow, localNow;
		
		SXMPUtils::SetTimeZone ( &utcNow );
		fprintf ( log, "SetTimeZone : %d-%02d-%02d %02d:%02d:%02d %d*%02d:%02d %d\n",
				  utcNow.year, utcNow.month, utcNow.day, utcNow.hour, utcNow.minute, utcNow.second,
				  utcNow.tzSign, utcNow.tzHour, utcNow.tzMinute, utcNow.nanoSecond );
		
		SXMPUtils::CurrentDateTime ( &utcNow );
		fprintf ( log, "CurrentDateTime : %d-%02d-%02d %02d:%02d:%02d %d*%02d:%02d %d\n",
				  utcNow.year, utcNow.month, utcNow.day, utcNow.hour, utcNow.minute, utcNow.second,
				  utcNow.tzSign, utcNow.tzHour, utcNow.tzMinute, utcNow.nanoSecond );
		
		localNow = utcNow;
		SXMPUtils::ConvertToLocalTime ( &localNow );
		fprintf ( log, "ConvertToLocalTime : %d-%02d-%02d %02d:%02d:%02d %d*%02d:%02d %d\n",
				  localNow.year, localNow.month, localNow.day, localNow.hour, localNow.minute, localNow.second,
				  localNow.tzSign, localNow.tzHour, localNow.tzMinute, localNow.nanoSecond );
		
		utcNow = localNow;
		SXMPUtils::ConvertToUTCTime ( &utcNow );
		fprintf ( log, "ConvertToUTCTime : %d-%02d-%02d %02d:%02d:%02d %d*%02d:%02d %d\n",
				  utcNow.year, utcNow.month, utcNow.day, utcNow.hour, utcNow.minute, utcNow.second,
				  utcNow.tzSign, utcNow.tzHour, utcNow.tzMinute, utcNow.nanoSecond );

		fprintf ( log, "\n" );

		i = SXMPUtils::CompareDateTime ( utcNow, localNow );
		fprintf ( log, "CompareDateTime with a == b : %d\n", i );

		utcNow.second = 0;
		localNow.second = 30;
		i = SXMPUtils::CompareDateTime ( utcNow, localNow );
		fprintf ( log, "CompareDateTime with a < b : %d\n", i );

		utcNow.second = 59;
		i = SXMPUtils::CompareDateTime ( utcNow, localNow );
		fprintf ( log, "CompareDateTime with a > b : %d\n", i );

	}

	// --------------------------------------------------------------------------------------------
	// Miscellaneous utilities
	// -----------------------

	{
		WriteMajorLabel ( log, "Test CatenateArrayItems and SeparateArrayItems" );
		fprintf ( log, "\n" );

		SXMPMeta meta;
		
		meta.AppendArrayItem ( kNS1, "Array1", kXMP_PropValueIsArray, "one" );
		meta.AppendArrayItem ( kNS1, "Array1", 0, "two" );
		meta.AppendArrayItem ( kNS1, "Array1", kXMP_PropValueIsArray, "3, three" );
		meta.AppendArrayItem ( kNS1, "Array1", 0, "4; four" );

		DumpXMPObj ( log, meta, "Initial array" );
		fprintf ( log, "\n" );
		
		tmpStr1.erase();
		SXMPUtils::CatenateArrayItems ( meta, kNS1, "Array1", "; ", "\"", kXMP_NoOptions, &tmpStr1 );
		fprintf ( log, "CatenateArrayItems, no commas : %s\n", tmpStr1.c_str() );
		
		tmpStr2.erase();
		SXMPUtils::CatenateArrayItems ( meta, kNS1, "Array1", " ; ", "\"", kXMPUtil_AllowCommas, &tmpStr2 );
		fprintf ( log, "CatenateArrayItems, allow commas : %s\n", tmpStr2.c_str() );

		SXMPUtils::SeparateArrayItems ( &meta, kNS1, "Array2-1", kXMP_NoOptions, tmpStr1.c_str() );
		SXMPUtils::SeparateArrayItems ( &meta, kNS1, "Array2-2", kXMPUtil_AllowCommas, tmpStr1.c_str() );
		
		SXMPUtils::SeparateArrayItems ( &meta, kNS1, "Array3-1", kXMP_PropArrayIsOrdered, tmpStr2 );
		SXMPUtils::SeparateArrayItems ( &meta, kNS1, "Array3-2", (kXMP_PropArrayIsOrdered | kXMPUtil_AllowCommas), tmpStr2 );

		DumpXMPObj ( log, meta, "Set Array1, cat and split into others" );

		SXMPUtils::SeparateArrayItems ( &meta, kNS1, "Array2-2", kXMP_NoOptions, tmpStr1.c_str() );	// Repeat into existing arrays.
		SXMPUtils::SeparateArrayItems ( &meta, kNS1, "Array3-2", kXMP_PropArrayIsOrdered, tmpStr2.c_str() );

	}

	// --------------------------------------------------------------------------------------------
	
	{
		WriteMajorLabel ( log, "Test RemoveProperties and AppendProperties" );

		SXMPMeta meta1 ( kSimpleRDF, strlen(kSimpleRDF) );

		meta1.SetProperty ( kNS2, "Prop", "value" );
		DumpXMPObj ( log, meta1, "Parse simple RDF, add ns2:Prop" );
		
		SXMPUtils::RemoveProperties ( &meta1, kNS1, "ArrayOfStructProp" );
		DumpXMPObj ( log, meta1, "Remove ns1:ArrayOfStructProp" );
		
		SXMPUtils::RemoveProperties ( &meta1, kNS1 );
		DumpXMPObj ( log, meta1, "Remove all of ns1:" );
		
		meta1.SetProperty ( kXMP_NS_XMP, "CreatorTool", "XMPCoverage" );
		meta1.SetProperty ( kXMP_NS_XMP, "Nickname", "TXMP test" );
		DumpXMPObj ( log, meta1, "Set xmp:CreatorTool (internal) and xmp:Nickname (external)" );
		
		SXMPUtils::RemoveProperties ( &meta1 );
		DumpXMPObj ( log, meta1, "Remove all external properties" );
		
		SXMPUtils::RemoveProperties ( &meta1, 0, 0, kXMPUtil_DoAllProperties );
		DumpXMPObj ( log, meta1, "Remove all properties, including internal" );
		
		meta1.SetProperty ( kXMP_NS_XMP, "CreatorTool", "XMPCoverage" );
		meta1.SetProperty ( kXMP_NS_XMP, "Nickname", "TXMP test" );
		DumpXMPObj ( log, meta1, "Set xmp:CreatorTool and xmp:Nickname again" );
		
		SXMPMeta meta2 ( kSimpleRDF, strlen(kSimpleRDF) );
		
		meta2.SetProperty ( kXMP_NS_XMP, "CreatorTool", "new CreatorTool" );
		meta2.SetProperty ( kXMP_NS_XMP, "Nickname", "new Nickname" );
		meta2.SetProperty ( kXMP_NS_XMP, "Format", "new Format" );
		DumpXMPObj ( log, meta2, "Create 2nd XMP object with new values" );
		
		SXMPUtils::ApplyTemplate ( &meta1, meta2, kXMPTemplate_AddNewProperties );
		DumpXMPObj ( log, meta1, "Append 2nd to 1st, keeping old values, external only" );
		
		meta2.SetProperty ( kXMP_NS_XMP, "CreatorTool", "newer CreatorTool" );
		meta2.SetProperty ( kXMP_NS_XMP, "Nickname", "newer Nickname" );
		meta2.SetProperty ( kXMP_NS_XMP, "Format", "newer Format" );
		SXMPUtils::ApplyTemplate ( &meta1, meta2, kXMPTemplate_AddNewProperties | kXMPTemplate_IncludeInternalProperties );
		DumpXMPObj ( log, meta1, "Append 2nd to 1st, keeping old values, internal also" );

		meta2.SetProperty ( kXMP_NS_XMP, "CreatorTool", "newest CreatorTool" );
		meta2.SetProperty ( kXMP_NS_XMP, "Nickname", "newest Nickname" );
		meta2.SetProperty ( kXMP_NS_XMP, "Format", "newest Format" );
		SXMPUtils::ApplyTemplate ( &meta1, meta2, kXMPTemplate_AddNewProperties | kXMPTemplate_ReplaceExistingProperties );
		DumpXMPObj ( log, meta1, "Append 2nd to 1st, replacing old values, external only" );
		
		meta2.SetProperty ( kXMP_NS_XMP, "CreatorTool", "final CreatorTool" );
		meta2.SetProperty ( kXMP_NS_XMP, "Nickname", "final Nickname" );
		meta2.SetProperty ( kXMP_NS_XMP, "Format", "final Format" );
		SXMPUtils::ApplyTemplate ( &meta1, meta2, kXMPTemplate_AddNewProperties | kXMPTemplate_ReplaceExistingProperties | kXMPTemplate_IncludeInternalProperties );
		DumpXMPObj ( log, meta1, "Append 2nd to 1st, replacing old values, internal also" );

	}

	// --------------------------------------------------------------------------------------------
	
	{		
		WriteMajorLabel ( log, "Test DuplicateSubtree" );

		SXMPMeta meta1 ( kSimpleRDF, strlen(kSimpleRDF) );
		SXMPMeta meta2;
		
		SXMPUtils::DuplicateSubtree ( meta1, &meta2, kNS1, "ArrayOfStructProp" );
		DumpXMPObj ( log, meta2, "DuplicateSubtree to default destination" );

		#if 1	// The underlying old toolkit does not support changing the schema namespace.
				
			SXMPUtils::DuplicateSubtree ( meta1, &meta2, kNS1, "ArrayOfStructProp", kNS2, "NewAoS" );
			DumpXMPObj ( log, meta2, "DuplicateSubtree to different destination" );
			
			SXMPUtils::DuplicateSubtree ( meta1, &meta1, kNS1, "ArrayOfStructProp", kNS2, "NewAoS" );
			DumpXMPObj ( log, meta1, "DuplicateSubtree to different destination in same object" );
		
		#else
				
			SXMPUtils::DuplicateSubtree ( meta1, &meta2, kNS1, "ArrayOfStructProp", kNS1, "NewAoS" );
			DumpXMPObj ( log, meta2, "DuplicateSubtree to different destination" );
			
			SXMPUtils::DuplicateSubtree ( meta1, &meta1, kNS1, "ArrayOfStructProp", kNS1, "NewAoS" );
			DumpXMPObj ( log, meta1, "DuplicateSubtree to different destination in same object" );
		
		#endif

	}

	// --------------------------------------------------------------------------------------------
	
	{
		WriteMajorLabel ( log, "Test EncodeToBase64 and DecodeFromBase64" );
		fprintf ( log, "\n" );
		
		unsigned long m;

		#if UseStringPushBack
			#define PushBack(s,c)	s.push_back ( c )
		#else
			#define PushBack(s,c)	s.insert ( s.end(), c );
		#endif
	
		tmpStr1.erase();
		for ( i = 0; i < 64; i += 4 ) {
			m = (i << 18) + ((i+1) << 12) + ((i+2) << 6) + (i+3);
			PushBack ( tmpStr1, ((char) (m >> 16)) );
			PushBack ( tmpStr1, ((char) ((m >> 8) & 0xFF)) );
			PushBack ( tmpStr1, ((char) (m & 0xFF)) );
		}

		tmpStr2.erase();
		SXMPUtils::EncodeToBase64 ( tmpStr1, &tmpStr2 );
		fprintf ( log, "Encoded sequence (should be A-Za-z0-9+/) : %s\n", tmpStr2.c_str() );

		tmpStr3.erase();
		SXMPUtils::DecodeFromBase64 ( tmpStr2, &tmpStr3 );
		if ( tmpStr1 != tmpStr3 ) fprintf ( log, "** Error in base 64 round trip\n" );

	}

	// --------------------------------------------------------------------------------------------

	WriteMajorLabel ( log, "XMPCoreCoverage done" );
	fprintf ( log, "\n" );

}	// DoXMPCoreCoverage

// =================================================================================================

extern "C" int main ( int /*argc*/, const char * argv [] )
{
	int result = 0;

	char   logName[256];
	int    nameLen = strlen ( argv[0] );
	if ( (nameLen >= 4) && (strcmp ( argv[0]+nameLen-4, ".exe" ) == 0) ) nameLen -= 4;
	memcpy ( logName, argv[0], nameLen );
	memcpy ( &logName[nameLen], "Log.txt", 8 );	// Include final null.
	FILE * log = fopen ( logName, "wb" );

	time_t now = time(0);
	fprintf ( log, "XMPCoreCoverage starting %s", ctime(&now) );

	XMP_VersionInfo version;
	SXMPMeta::GetVersionInfo ( &version );
	fprintf ( log, "Version : %s\n" , version.message );

	try {

// *** Add memory leak checking for both DLL and static builds ***

		if ( ! SXMPMeta::Initialize() ) {
			fprintf ( log, "## XMPMeta::Initialize failed!\n" );
			return -1;
		}
		DoXMPCoreCoverage ( log );

	} catch ( XMP_Error & excep ) {

		fprintf ( log, "\nCaught XMP_Error %d : %s\n", excep.GetID(), excep.GetErrMsg() );
		result = -2;

	} catch ( ... ) {

		fprintf ( log, "## Caught unknown exception\n" );
		result = -3;

	}

	SXMPMeta::Terminate();

	now = time(0);
	fprintf ( log, "XMPCoreCoverage finished %s", ctime(&now) );
	fprintf ( log,	"Final status = %d\n", result );
	fclose ( log );
	
	printf( "results have been logged into %s\n", logName );
	
	return result;

}
