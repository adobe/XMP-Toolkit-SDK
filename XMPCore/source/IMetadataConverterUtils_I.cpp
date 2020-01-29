// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#include "XMPCore/Interfaces/IMetadataConverterUtils_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"

namespace AdobeXMPCore_Int {
    
    pvoid APICALL IMetadataConverterUtils_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
        return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
    }
    
    pvoid APICALL IMetadataConverterUtils_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
        return CallUnSafeFunction< IMetadataConverterUtils_I, pvoid, pvoid, uint64, uint32 >(error, this, NULL, &IMetadataConverterUtils_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
    }
    
    pvoid APICALL IMetadataConverterUtils_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
        if ( interfaceID == AdobeXMPCore::kIMetadataConverterUtilsID ) {
            switch ( interfaceVersion ) {
                case 1:
                    return static_cast< IMetadataConverterUtils_v1 * >( this );
                    break;
                    
                case kInternalInterfaceVersionNumber:
                    return this;
                    break;
                    
                default:
                    throw IError_I::CreateInterfaceVersionNotAvailableError(IError_v1::kESOperationFatal, interfaceID, interfaceVersion, __FILE__, __LINE__ );
                    break;
            }
        }
        if ( isTopLevel )
            throw IError_I::CreateInterfaceNotAvailableError(IError_v1::kESOperationFatal, AdobeXMPCore::kIMetadataConverterUtilsID, interfaceID, __FILE__, __LINE__ );
        return NULL;
    }

    
}
