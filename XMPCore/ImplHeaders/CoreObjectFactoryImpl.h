#ifndef CoreObjectFactoryImpl_h__
#define CoreObjectFactoryImpl_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#if !(IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED)
	#error "Not adhering to design constraints"
	// this file should only be included from its own cpp file
#endif

#include "XMPCore/Interfaces/ICoreObjectFactory_I.h"
#include "XMPCommon/BaseClasses/MemoryManagedObject.h"
#if ENABLE_CPP_DOM_MODEL
#include "XMPCommon/ImplHeaders/SharedObjectImpl.h"
#endif

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

extern "C" XMP_PUBLIC AdobeXMPCore::pICoreObjectFactory_base WXMPMeta_GetXMPDOMFactoryInstance_1();

namespace AdobeXMPCore_Int {

	class CoreObjectFactoryImpl
		: public virtual ICoreObjectFactory_I
	{
	public:
		CoreObjectFactoryImpl() {}
		virtual ~CoreObjectFactoryImpl() __NOTHROW__ {}

		virtual pIError_base APICALL CreateError( uint32 domain, uint32 code, uint32 severity, pcIError_base & error ) __NOTHROW__;
		virtual pIUTF8String_base APICALL CreateUTF8String( const char * buf, sizet count, pcIError_base & error ) __NOTHROW__;
		virtual pcIPathSegment_base APICALL CreateArrayIndexPathSegment( const char * nameSpace, sizet nameSpaceLength, sizet index, pcIError_base & error ) __NOTHROW__;
		virtual pIArrayNode_base APICALL CreateArrayNode( uint32 arrayForm, const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__;
		virtual pIMetadata_base APICALL CreateMetadata( pcIError_base & error ) __NOTHROW__;
		virtual pINameSpacePrefixMap_base APICALL CreateNameSpacePrefixMap( pcIError_base & error ) __NOTHROW__;
		virtual pIPath_base APICALL CreatePath( pcIError_base & error ) __NOTHROW__;
		virtual pcIPathSegment_base APICALL CreatePropertyPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__;
		virtual pcIPathSegment_base APICALL CreateQualifierPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__;
		virtual pcIPathSegment_base APICALL CreateQualifierSelectorPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, const char * value, sizet valueLength, pcIError_base & error ) __NOTHROW__;
		virtual pISimpleNode_base APICALL CreateSimpleNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, const char * value, sizet valueLength, pcIError_base & error ) __NOTHROW__;
		virtual pIStructureNode_base APICALL CreateStructureNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__;
		virtual pICoreConfigurationManager_base APICALL GetCoreConfigurationManager( pcIError_base & error ) __NOTHROW__;
		virtual pIDOMImplementationRegistry_base APICALL GetDOMImplementationRegistry( pcIError_base & error ) __NOTHROW__;
		virtual pcINameSpacePrefixMap_base APICALL GetDefaultNameSpacePrefixMap( pcIError_base & error ) __NOTHROW__;
		virtual pIPath_base APICALL ParsePath( const char * path, sizet pathLength, pcINameSpacePrefixMap_base map, pcIError_base & error ) __NOTHROW__;
        virtual pIMetadata_base APICALL ConvertXMPMetatoIMetadata(XMPMetaRef xmpref, pcIError_base & error ) __NOTHROW__;
        virtual XMPMetaRef APICALL ConvertIMetadatatoXMPMeta( pIMetadata iMeta, pcIError_base & error ) __NOTHROW__;

	protected:

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();


	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}


#endif // CoreObjectFactoryImpl_h__
