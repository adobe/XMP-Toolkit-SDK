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

#include "XMPCommon/Interfaces/IErrorNotifier.h"
#include "XMPCommon/Interfaces/IError.h"

namespace AdobeXMPCommon {
	uint32 APICALL IErrorNotifier_v1::notify( pcIError_base error, uint32 & exceptionThrown ) __NOTHROW__ {
		exceptionThrown = 0;
		bool retValue( false );
		try {
			retValue = Notify( IError::MakeShared( error ) );
		} catch ( ... ) {
			exceptionThrown = 1;
		}
		return retValue ? 1 : 0;
	}

}

