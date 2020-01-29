#ifndef __IMetadataConverterUtils_I_h__
#define __IMetadataConverterUtils_I_h__ 1

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


#include "XMPCore/Interfaces/IMetadataConverterUtils.h"
#include "XMPCore/XMPCoreFwdDeclarations_I.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject_I.h"
#include "XMPCore/source/XMPMeta.hpp"

namespace AdobeXMPCore_Int {
    
    //!
    //! \brief Internal interface that represents an utility functions that can convert old xmp object(SXMPMeta) to new xmp object(IMetadata) and vice versa.
    //!
    
    class IMetadataConverterUtils_I
    : public virtual IMetadataConverterUtils
    , public virtual ISharedObject_I
    {
    
    public:
       // static AdobeXMPCore::spIMetadata APICALL convertXMPMetatoIMetadata( XMPMetaRef metaRef) __NOTHROW__;
        
        static XMPMetaRef APICALL convertIMetadatatoXMPMeta(AdobeXMPCore::pIMetadata_base iMeta, const AdobeXMPCore::spcINameSpacePrefixMap & nameSpacePrefixMap = AdobeXMPCore::spcINameSpacePrefixMap()) __NOTHROW__;
        
        /* For internal use : called from RDFDOMParserImpl::ParseAsNode*/
        static AdobeXMPCore::spIMetadata APICALL convertXMPMetatoIMetadata( XMPMeta* inpMeta) __NOTHROW__;
        
        /* For internal use : called from RDFDOMSerializerImpl::Serialize and RDFDOMSerializerImpl::SerializeInternal*/
        static XMPMetaRef APICALL convertIMetadatatoXMPMeta(const AdobeXMPCore::spINode & node,XMP_OptionBits options, const AdobeXMPCore::spcINameSpacePrefixMap & nameSpacePrefixMap = AdobeXMPCore::spcINameSpacePrefixMap()) __NOTHROW__;
        
        //!
        //! return the version of the interface.
        //! \return 32 bit unsigned integer representing the version of the interface.
        //!
        static uint32 GetInterfaceVersion() { return kInternalInterfaceVersionNumber; }
        
        
        virtual pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__{ return this; }
        virtual AdobeXMPCore::pIMetadataConverterUtils APICALL GetActualIMetadataConverterUtils() __NOTHROW__ { return this; }

        virtual AdobeXMPCore_Int::pIMetadataConverterUtils_I APICALL GetIMetadataConverterUtils_I() __NOTHROW__ { return this; }

        virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );
        
    protected:
        virtual ~IMetadataConverterUtils_I() __NOTHROW__ {}

        virtual pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__;
        pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );

        #ifdef FRIEND_CLASS_DECLARATION
            FRIEND_CLASS_DECLARATION();
        #endif
        REQ_FRIEND_CLASS_DECLARATION();
    };
}

#endif /*__IMetadataConverterUtils_I_h__*/
