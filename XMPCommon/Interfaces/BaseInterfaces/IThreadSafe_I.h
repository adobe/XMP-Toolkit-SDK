#ifndef IThreadSafe_I_h__
#define IThreadSafe_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/XMPCommonFwdDeclarations_I.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IThreadSafe.h"

namespace AdobeXMPCommon_Int {

	//!
	//! \brief Internal Interface that serves as the base interface for all the externally exposed 
	//! or internal interfaces which needs to provide client configurable thread safety.
	//! \attention In case client has disabled thread safety at the module level these functions will
	//! have no use.
	//! \note By default all the objects created are not thread safe.
	//!

	class IThreadSafe_I
		: public virtual IThreadSafe
	{
	public:

		//!
		//! Make two objects share the same mutex.
		//! Generally required in case of a DOM or where parent child relationship needs to be maintained.
		//! \param[in] mutex a shared pointer of ISharedMutex interface to be shared among the object.
		//!
		virtual void APICALL ShareMutex( const XMP_COMPONENT_INT_NAMESPACE::spISharedMutex & mutex ) = 0;
		virtual void APICALL UnShareMutex() = 0;

		virtual pIThreadSafe_I APICALL GetIThreadSafe_I() __NOTHROW__ { return this; }

		virtual uint32 APICALL isThreadSafe() const __NOTHROW__ { 
			if ( IsThreadSafe() ) return 1; 
			return 0;
		}

	#ifdef FRIEND_CLASS_DECLARATION
		REQ_FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}
#endif  // IThreadSafe_I_h__
