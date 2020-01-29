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

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/CoreObjectFactoryImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"

#include "XMPCommon/Interfaces/IUTF8String_I.h"

#include "XMPCore/Interfaces/IPathSegment_I.h"
#include "XMPCore/Interfaces/IPath_I.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap_I.h"
#include "XMPCore/Interfaces/ISimpleNode_I.h"
#include "XMPCore/Interfaces/IArrayNode_I.h"
#include "XMPCore/Interfaces/IMetadataConverterUtils_I.h"
#include "XMPCore/Interfaces/IStructureNode_I.h"
#include "XMPCore/Interfaces/IMetadata_I.h"
#include "XMPCore/Interfaces/IDOMImplementationRegistry_I.h"
#include "XMPCore/Interfaces/ICoreConfigurationManager_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCore/XMPCoreFwdDeclarations.h"

#if ENABLE_CPP_DOM_MODEL

namespace AdobeXMPCore_Int {

	pcIPathSegment_base APICALL CoreObjectFactoryImpl::CreateArrayIndexPathSegment( const char * nameSpace, sizet nameSpaceLength, sizet index, pcIError_base & error ) __NOTHROW__ {
		return ReturnRawPointerFromSharedPointer< const IPathSegment, pcIPathSegment_base >(
			&IPathSegment::CreateArrayIndexPathSegment, error, __FILE__, __LINE__, nameSpace, nameSpaceLength, index );
	}

	pIArrayNode_base APICALL CoreObjectFactoryImpl::CreateArrayNode( uint32 arrayForm, const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ {
		switch ( static_cast< IArrayNode::eArrayForm >( arrayForm ) ) {
		case IArrayNode::kAFOrdered:
			return ReturnRawPointerFromSharedPointer< IArrayNode, pIArrayNode_base >(
				&IArrayNode::CreateOrderedArrayNode, error, __FILE__, __LINE__, nameSpace, nameSpaceLength, name, nameLength );
			break;

		case IArrayNode::kAFUnordered:
			return ReturnRawPointerFromSharedPointer< IArrayNode, pIArrayNode_base >(
				&IArrayNode::CreateUnorderedArrayNode, error, __FILE__, __LINE__, nameSpace, nameSpaceLength, name, nameLength );
			break;

		case IArrayNode::kAFAlternative:
			return ReturnRawPointerFromSharedPointer< IArrayNode, pIArrayNode_base >(
				&IArrayNode::CreateAlternativeArrayNode, error, __FILE__, __LINE__, nameSpace, nameSpaceLength, name, nameLength );
			break;
        default:
                return NULL;
                break;

		}
		
	}

	pIMetadata_base APICALL CoreObjectFactoryImpl::CreateMetadata( pcIError_base & error ) __NOTHROW__ {
		return ReturnRawPointerFromSharedPointer< IMetadata, pIMetadata_base >(
			&IMetadata::CreateMetadata, error, __FILE__, __LINE__ );
	}

	pINameSpacePrefixMap_base APICALL CoreObjectFactoryImpl::CreateNameSpacePrefixMap( pcIError_base & error ) __NOTHROW__ {
		return ReturnRawPointerFromSharedPointer< INameSpacePrefixMap, pINameSpacePrefixMap_base >(
			&INameSpacePrefixMap::CreateNameSpacePrefixMap, error, __FILE__, __LINE__ );
	}

	pIPath_base APICALL CoreObjectFactoryImpl::CreatePath( pcIError_base & error ) __NOTHROW__ {
		return ReturnRawPointerFromSharedPointer< IPath, pIPath_base >(
			&IPath::CreatePath, error, __FILE__, __LINE__ );
	}

	pcIPathSegment_base APICALL CoreObjectFactoryImpl::CreatePropertyPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ {
		return ReturnRawPointerFromSharedPointer< const IPathSegment, pcIPathSegment_base >(
			&IPathSegment::CreatePropertyPathSegment, error, __FILE__, __LINE__, nameSpace, nameSpaceLength, name, nameLength );
	}

	pcIPathSegment_base APICALL CoreObjectFactoryImpl::CreateQualifierPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ {
		return ReturnRawPointerFromSharedPointer< const IPathSegment, pcIPathSegment_base >(
			&IPathSegment::CreateQualifierPathSegment, error, __FILE__, __LINE__, nameSpace, nameSpaceLength, name, nameLength );
	}

	pcIPathSegment_base APICALL CoreObjectFactoryImpl::CreateQualifierSelectorPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, const char * value, sizet valueLength, pcIError_base & error ) __NOTHROW__ {
		return ReturnRawPointerFromSharedPointer< const IPathSegment, pcIPathSegment_base >(
			&IPathSegment::CreateQualifierSelectorPathSegment, error, __FILE__, __LINE__, nameSpace, nameSpaceLength, name, nameLength, value, valueLength );
	}

	pISimpleNode_base APICALL CoreObjectFactoryImpl::CreateSimpleNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, const char * value, sizet valueLength, pcIError_base & error ) __NOTHROW__ {
		return ReturnRawPointerFromSharedPointer< ISimpleNode, pISimpleNode_base >(
			&ISimpleNode::CreateSimpleNode, error, __FILE__, __LINE__, nameSpace, nameSpaceLength, name, nameLength, value, valueLength );
	}

	pIStructureNode_base APICALL CoreObjectFactoryImpl::CreateStructureNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ {
		return ReturnRawPointerFromSharedPointer< IStructureNode, pIStructureNode_base >(
			&IStructureNode::CreateStructureNode, error, __FILE__, __LINE__, nameSpace, nameSpaceLength, name, nameLength );
	}
    
	pICoreConfigurationManager_base APICALL CoreObjectFactoryImpl::GetCoreConfigurationManager( pcIError_base & error ) __NOTHROW__ {
		return ReturnRawPointerFromSharedPointer< ICoreConfigurationManager, pICoreConfigurationManager_base >(
			&ICoreConfigurationManager::GetCoreConfigurationManager, error, __FILE__, __LINE__ );
	}

	pIDOMImplementationRegistry_base CoreObjectFactoryImpl::GetDOMImplementationRegistry( pcIError_base & error ) __NOTHROW__ {
		return ReturnRawPointerFromSharedPointer< IDOMImplementationRegistry, pIDOMImplementationRegistry_base >(
			&IDOMImplementationRegistry::GetDOMImplementationRegistry, error, __FILE__, __LINE__ );
	}

	pcINameSpacePrefixMap_base CoreObjectFactoryImpl::GetDefaultNameSpacePrefixMap( pcIError_base & error ) __NOTHROW__ {
		return ReturnRawPointerFromSharedPointer< const INameSpacePrefixMap, pcINameSpacePrefixMap_base >(
			&INameSpacePrefixMap::GetDefaultNameSpacePrefixMap, error, __FILE__, __LINE__ );
	}

	pIPath_base CoreObjectFactoryImpl::ParsePath( const char * path, sizet pathLength, pcINameSpacePrefixMap_base map, pcIError_base & error ) __NOTHROW__ {
		return ReturnRawPointerFromSharedPointer< IPath, pIPath_base, const char *, sizet, const spcINameSpacePrefixMap & >(
			&IPath::ParsePath, error, __FILE__, __LINE__, path, pathLength, INameSpacePrefixMap::MakeShared( map ) );
	}

		pIError_base CoreObjectFactoryImpl::CreateError(uint32 domain, uint32 code, uint32 severity, pcIError_base & error) __NOTHROW__{
		return ReturnRawPointerFromSharedPointer< IError_I, pIError_base, IError::eErrorDomain, IError::eErrorCode, IError::eErrorSeverity >(
			&IError_I::CreateError, error, __FILE__, __LINE__, static_cast< IError::eErrorDomain >( domain ), static_cast< IError::eErrorCode >( code ), static_cast< IError::eErrorSeverity >( severity ) );
	}

		pIUTF8String_base CoreObjectFactoryImpl::CreateUTF8String(const char * buf, sizet count, pcIError_base & error) __NOTHROW__{
		return ReturnRawPointerFromSharedPointer< IUTF8String, pIUTF8String_base >(
			&IUTF8String_I::CreateUTF8String, error, __FILE__, (sizet) __LINE__, buf, count );
	}

	static pICoreObjectFactory ManageCoreObjectFactory( bool destroy = false ) {
		static CoreObjectFactoryImpl coreObjectFactoryImplObj;
		return &coreObjectFactoryImplObj;
	}
    
    pIMetadata_base CoreObjectFactoryImpl::ConvertXMPMetatoIMetadata( XMPMetaRef xmpref, pcIError_base & error ) __NOTHROW__
    {
		XMPMeta* meta = (XMPMeta*)xmpref;
		error = NULL;
		try {
			auto sp = IMetadataConverterUtils_I::convertXMPMetatoIMetadata(meta);
			sp->GetISharedObject_I()->AcquireInternal();
			return sp.get();
		}
		catch (spcIError err) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		}
		catch (...) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError(IError_v1::kESOperationFatal, __FILE__, __LINE__).get();
			err->AcquireInternal();
			error = err;
		}
		return NULL;
      /*  return ReturnRawPointerFromSharedPointer< IMetadata, pIMetadata_base, XMPMeta* >
			(&IMetadataConverterUtils_I::convertXMPMetatoIMetadata, error, __FILE__, (sizet)__LINE__, meta);*/
    }
    
    XMPMetaRef CoreObjectFactoryImpl::ConvertIMetadatatoXMPMeta( pIMetadata iMeta, pcIError_base & error ) __NOTHROW__
    {
        try {
            XMPMetaRef ref = IMetadataConverterUtils_I::convertIMetadatatoXMPMeta(iMeta);
            return ref;
        } catch ( spcIError err ) {
            error = err->GetActualIError();
            error->GetISharedObject_I()->AcquireInternal();
        } catch ( ... ) {
            pIError_I err = IError_I::CreateUnknownExceptionCaughtError( IError_v1::kESOperationFatal, __FILE__, __LINE__ ).get();
            err->AcquireInternal();
            error = err;
        }
        return NULL;

    }

}

namespace AdobeXMPCore {

	void ICoreObjectFactory_v1::SetupCoreObjectFactory() {
		AdobeXMPCore_Int::ManageCoreObjectFactory();
	}

	pICoreObjectFactory ICoreObjectFactory_v1::GetCoreObjectFactory() {
		return AdobeXMPCore_Int::ManageCoreObjectFactory();
	}

	void ICoreObjectFactory_v1::DestroyCoreObjectFactory() {
		AdobeXMPCore_Int::ManageCoreObjectFactory( true );
	}
}
#endif
extern "C" XMP_PUBLIC AdobeXMPCore::pICoreObjectFactory_base WXMPMeta_GetXMPDOMFactoryInstance_1() {
#if ENABLE_CPP_DOM_MODEL
	return AdobeXMPCore_Int::ManageCoreObjectFactory();
#endif
	
	return NULL;
}
