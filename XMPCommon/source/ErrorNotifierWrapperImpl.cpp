// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/XMPCommonFwdDeclarations_I.h"
namespace XMP_COMPONENT_INT_NAMESPACE {
	class ErrorNotifierWrapperImpl_v1;
}
#define FRIEND_CLASS_DECLARATION() friend class XMP_COMPONENT_INT_NAMESPACE::ErrorNotifierWrapperImpl_v1;
#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCommon/ImplHeaders/ErrorNotifierWrapperImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED
#undef FRIEND_CLASS_DECLARATION

#include "XMPCommon/Utilities/TWrapperFunctions_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	bool APICALL ErrorNotifierWrapperImpl_v1::Notify( const spcIError & err ) {
		uint32 exceptionCaught( 0 );
		uint32 returnValue = mErrorNotifier->notify( err.get(), exceptionCaught );
		if ( exceptionCaught ) {
			return false;
		}
		return returnValue != 0 ? true : false;
	}

	ErrorNotifierWrapperImpl_v1::ErrorNotifierWrapperImpl_v1( IErrorNotifier_v1 * errorNotifier )
		: mErrorNotifier( errorNotifier ) {}

	pIErrorNotifier CreateErrorNotifierWrapperImpl( pIErrorNotifier_base errorNotifier ) {
		if ( errorNotifier == NULL )
			return NULL;
		// start with the highest available version and keep on going down
		if ( IErrorNotifier_v1 * ptr = dynamic_cast< IErrorNotifier_v1 * >( errorNotifier ) ) {
			return new ErrorNotifierWrapperImpl_v1( errorNotifier );
		} else {
			return NULL;
		}
	}
}
