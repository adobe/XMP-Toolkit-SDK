#include "XMPCore/XMPCoreDefines.h"
#if ENABLE_CPP_DOM_MODEL
#ifndef __XMPMeta2_hpp__
#define __XMPMeta2_hpp__


#include "public/include/XMP_Environment.h"
#include "public/include/XMP_Const.h"
#include "XMPCore/source/XMPCore_Impl.hpp"
#include "source/XMLParserAdapter.hpp"
#include "XMPCore/source/XMPMeta.hpp"
#include "XMPCore/XMPCoreFwdDeclarations_I.h"

#ifndef DumpXMLParseTree
	#define DumpXMLParseTree 0
#endif

extern XMP_VarString * xdefaultName;	// Needed in XMPMeta-Parse.cpp, MoveExplicitAliases.

class XMPIterator;
class XMPUtils;

class XMPMeta2 : public XMPMeta {
public:
	

	

	// ---------------------------------------------------------------------------------------------

	XMPMeta2();
	
	virtual ~XMPMeta2() RELEASE_NO_THROW;

	// ---------------------------------------------------------------------------------------------
	
	virtual bool
	GetProperty ( XMP_StringPtr	   schemaNS,
				  XMP_StringPtr	   propName,
				  XMP_StringPtr *  propValue,
				  XMP_StringLen *  valueSize,
				  XMP_OptionBits * options ) const;

	// ---------------------------------------------------------------------------------------------
	
	virtual void
	SetProperty ( XMP_StringPtr	 schemaNS,
				  XMP_StringPtr	 propName,
				  XMP_StringPtr	 propValue,
				  XMP_OptionBits options );
	
	
	virtual void
	SetArrayItem ( XMP_StringPtr  schemaNS,
				   XMP_StringPtr  arrayName,
				   XMP_Index	  itemIndex,
				   XMP_StringPtr  itemValue,
				   XMP_OptionBits options );
	
	virtual void
	AppendArrayItem ( XMP_StringPtr	 schemaNS,
					  XMP_StringPtr	 arrayName,
					  XMP_OptionBits arrayOptions,
					  XMP_StringPtr	 itemValue,
					  XMP_OptionBits options );
	
	virtual void
	SetQualifier ( XMP_StringPtr  schemaNS,
				   XMP_StringPtr  propName,
				   XMP_StringPtr  qualNS,
				   XMP_StringPtr  qualName,
				   XMP_StringPtr  qualValue,
				   XMP_OptionBits options );
	/*
	virtual void
	SetStructField ( XMP_StringPtr	schemaNS,
					 XMP_StringPtr	structName,
					 XMP_StringPtr	fieldNS,
					 XMP_StringPtr	fieldName,
					 XMP_StringPtr	fieldValue,
					 XMP_OptionBits options );
	
	
	
		
	
	
	*/
	/*
	
	
	// ---------------------------------------------------------------------------------------------
	*/
	virtual bool
	GetLocalizedText ( XMP_StringPtr	schemaNS,
					   XMP_StringPtr	altTextName,
					   XMP_StringPtr	genericLang,
					   XMP_StringPtr	specificLang,
					   XMP_StringPtr *	actualLang,
					   XMP_StringLen *	langSize,
					   XMP_StringPtr *	itemValue,
					   XMP_StringLen *	valueSize,
					   XMP_OptionBits * options ) const;
	virtual void
	SetLocalizedText ( XMP_StringPtr  schemaNS,
					   XMP_StringPtr  altTextName,
					   XMP_StringPtr  genericLang,
					   XMP_StringPtr  specificLang,
					   XMP_StringPtr  itemValue,
					   XMP_OptionBits options );
	
	virtual void
	DeleteLocalizedText (	XMP_StringPtr	schemaNS,
							XMP_StringPtr	altTextName,
							XMP_StringPtr	genericLang,
							XMP_StringPtr	specificLang);
	virtual void
	GetObjectName ( XMP_StringPtr * namePtr,
					XMP_StringLen * nameLen ) const;

	virtual void
	SetObjectName ( XMP_StringPtr name );

	
	virtual void
	ParseFromBuffer ( XMP_StringPtr	 buffer,
					  XMP_StringLen	 bufferSize,
					  XMP_OptionBits options );
	virtual void
	SerializeToBuffer ( XMP_VarString * rdfString,
						XMP_OptionBits	options,
						XMP_StringLen	padding,
						XMP_StringPtr	newline,
						XMP_StringPtr	indent,
						XMP_Index		baseIndent ) const;
	virtual void
	Clone ( XMPMeta * clone, XMP_OptionBits options ) const;
	virtual bool
	DoesPropertyExist ( XMP_StringPtr schemaNS,
						XMP_StringPtr propName ) const;
	virtual void
	Erase();
	virtual void
	Sort();
	virtual XMP_Index
	CountArrayItems ( XMP_StringPtr schemaNS,
					  XMP_StringPtr arrayName ) const;
	virtual void
	DeleteProperty ( XMP_StringPtr schemaNS,
					 XMP_StringPtr propName );
	virtual void
	DumpObject ( XMP_TextOutputProc outProc,
				 void *				refCon ) const;

	void
		SetErrorCallback(XMPMeta_ErrorCallbackWrapper wrapperProc,
		XMPMeta_ErrorCallbackProc    clientProc,
		void *    context,
		XMP_Uns32 limit);

	void
		ResetErrorCallbackLimit(XMP_Uns32 limit);

	
	// ---------------------------------------------------------------------------------------------
	
	AdobeXMPCore::spIMetadata mDOM;
	AdobeXMPCore::spIDOMImplementationRegistry spRegistry;
	AdobeXMPCore::spIDOMParser spParser;

	friend class XMPIterator;
	friend class XMPUtils;

private:
  
	AdobeXMPCommon::spIUTF8String mBuffer;
	XMPMeta2 ( const XMPMeta2 & )
		{ XMP_Throw ( "Call to hidden constructor", kXMPErr_InternalFailure ); };
	void operator= ( const XMPMeta2 & )  
		{ XMP_Throw ( "Call to hidden operator=", kXMPErr_InternalFailure ); };

	void ProcessXMLTree ( XMP_OptionBits options );
	bool ProcessXMLBuffer ( XMP_StringPtr buffer, XMP_StringLen xmpSize, bool lastClientCall );
	void ProcessRDF ( const XML_Node & xmlTree, XMP_OptionBits options );


};

#endif 
#endif
