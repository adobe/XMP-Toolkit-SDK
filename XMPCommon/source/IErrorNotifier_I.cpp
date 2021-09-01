// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/Interfaces/IErrorNotifier_I.h"
#include "XMPCommon/ImplHeaders/DefaultErrorNotifierImpl.h"


namespace XMP_COMPONENT_INT_NAMESPACE {

	static DefaultErrorNotifierImpl sDefaultErrorNotifier;
	static pIErrorNotifier sErrorNotifier( &sDefaultErrorNotifier );

	pIErrorNotifier IErrorNotifier_I::GetErrorNotifier() {
		return sErrorNotifier;
	}

	pIErrorNotifier IErrorNotifier_I::SetErrorNotifier( pIErrorNotifier ErrorNotifier ) {
		pIErrorNotifier old = sErrorNotifier;
		if ( ErrorNotifier ) sErrorNotifier = ErrorNotifier;
		else sErrorNotifier = &sDefaultErrorNotifier;
		return old;
	}

}
