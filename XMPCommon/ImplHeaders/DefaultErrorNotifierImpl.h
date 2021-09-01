#ifndef DefaultErrorNotifierImpl_h__
#define DefaultErrorNotifierImpl_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/Interfaces/IErrorNotifier_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {


	class DefaultErrorNotifierImpl
		: public virtual IErrorNotifier_I
	{
	public:
		virtual bool APICALL Notify( const spcIError & error );
		virtual ~DefaultErrorNotifierImpl() __NOTHROW__{}

	protected:

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION(); 
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
	
}

#endif  // DefaultErrorNotifierImpl_h__
