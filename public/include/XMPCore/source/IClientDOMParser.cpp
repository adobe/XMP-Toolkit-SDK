// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCore/Interfaces/IClientDOMParser.h"
#include "XMPCore/Interfaces/INode.h"

#if 1//!BUILDING_XMPCORE_LIB
namespace AdobeXMPCore {

	pINode_base APICALL IClientDOMParser_v1::parse( const char * buffer, sizet bufferLength, pcIConfigurable configurationParameters, ReportErrorAndContinueABISafeProc proc, pcIError_base & error, uint32 & unknownExceptionCaught ) __NOTHROW__ {
		unknownExceptionCaught = 0;
		error = NULL;
		try {
			auto node = Parse( buffer, bufferLength, configurationParameters, proc );
			if ( node ) {
				node->Acquire();
				return node->GetActualINode();
			}
			return NULL;
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->Acquire();
		} catch ( ... ) {
			unknownExceptionCaught = 1;
		}
		return NULL;
	}

	uint32 APICALL IClientDOMParser_v1::areKeysCaseSensitive( pcIError_base & error, uint32 & unknownExceptionCaught ) const __NOTHROW__ {
		unknownExceptionCaught = 0;
		error = NULL;
		try {
			return AreKeysCaseSensitive() ? 1 : 0;
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->Acquire();
		} catch ( ... ) {
			unknownExceptionCaught = 1;
		}
		return 0;
	}

	void APICALL IClientDOMParser_v1::initialize( pIConfigurable configurationParameters, pcIError_base & error, uint32 & unknownExceptionCaught ) __NOTHROW__ {
		unknownExceptionCaught = 0;
		error = NULL;
		try {
			Initialize( configurationParameters );
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->Acquire();
		} catch ( ... ) {
			unknownExceptionCaught = 1;
		}
	}

	uint32 APICALL IClientDOMParser_v1::validate( const uint64 & key, uint32 dataType, const IConfigurable::CombinedDataValue & dataValue, pcIError_base & error, uint32 & unknownExceptionCaught ) __NOTHROW__ {
		unknownExceptionCaught = 0;
		error = NULL;
		try {
			return static_cast< uint32 >( Validate( key, static_cast< IConfigurable::eDataType >( dataType ), dataValue ) );
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->Acquire();
		} catch ( ... ) {
			unknownExceptionCaught = 1;
		}
		return 0;
	}
}
#endif  // !BUILDING_XMPCORE_LIB
