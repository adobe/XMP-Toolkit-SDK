// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
    class IMetadataConverterUtilsProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::IMetadataConverterUtilsProxy;

#include "XMPCore/Interfaces/IMetadataConverterUtils.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCore/Interfaces/IMetadata.h"
#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCore/Interfaces/ICoreObjectFactory.h"
#include "XMPCore/XMPCoreFwdDeclarations.h"

#include <assert.h>
namespace AdobeXMPCore {
    
    class IMetadataConverterUtilsProxy
    : public virtual IMetadataConverterUtils
    {
    private:
        pIMetadataConverterUtils mRawPtr;
        
    public:
        IMetadataConverterUtilsProxy( pIMetadataConverterUtils ptr ):
        mRawPtr( ptr )
        {
            mRawPtr->Acquire();
        }
        
        virtual ~IMetadataConverterUtilsProxy() __NOTHROW__ { mRawPtr->Release(); }
        
        pIMetadataConverterUtils APICALL GetActualIMetadataConverterUtils() __NOTHROW__ { return mRawPtr; }
        
        void APICALL Acquire() const __NOTHROW__ { assert( false ); }
        
        void APICALL Release() const __NOTHROW__ { assert( false ); }
        
        AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
            return mRawPtr->GetISharedObject_I();
        }
        
        AdobeXMPCore_Int::pIMetadataConverterUtils_I APICALL GetIMetadataConverterUtils_I() __NOTHROW__ {
            return mRawPtr->GetIMetadataConverterUtils_I();
        }
        
        pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
            return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
        }
        
        pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
            assert( false );
            return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
        }
    };
    
    spIMetadataConverterUtils IMetadataConverterUtils_v1::MakeShared( pIMetadataConverterUtils_base ptr ) {
        if ( !ptr ) return spIMetadataConverterUtils();
        pIMetadataConverterUtils p = IMetadataConverterUtils::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IMetadataConverterUtils >() : ptr;
        return shared_ptr< IMetadataConverterUtils >( new IMetadataConverterUtilsProxy( p ) );
    }

    AdobeXMPCore::spIMetadata IMetadataConverterUtils_v1::ConvertXMPMetatoIMetadata(const SXMPMeta* inOldXMP)
    {
        if(!inOldXMP) return  AdobeXMPCore::spIMetadata();
        return CallSafeFunctionReturningPointer< ICoreObjectFactory, pIMetadata, IMetadata, XMPMetaRef >(
        ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::ConvertXMPMetatoIMetadata ,inOldXMP->GetInternalRef());
    }
    
    SXMPMeta IMetadataConverterUtils_v1::ConvertIMetadatatoXMPMeta(AdobeXMPCore::spIMetadata inNewXMP)
    {
        if(!inNewXMP) return SXMPMeta();
        pcIError_base error( NULL );
        XMPMetaRef returnValue = ICoreObjectFactory::GetCoreObjectFactory()->ConvertIMetadatatoXMPMeta(inNewXMP->GetActualIMetadata(),error);
        if ( error ) throw IError::MakeShared( error );
        return SXMPMeta( returnValue );
    }
    
}

#endif  // BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB
