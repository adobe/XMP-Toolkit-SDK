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
	#include "XMPCommon/ImplHeaders/DefaultErrorNotifierImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	bool APICALL DefaultErrorNotifierImpl::Notify( const spcIError & error ) {
		if ( error->GetSeverity() > IError_v1::kESWarning )
			return false;
		return true;
	}

}
