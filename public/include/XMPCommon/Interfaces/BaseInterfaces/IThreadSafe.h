//! @file IThreadSafe.h
#ifndef IThreadSafe_h__
#define IThreadSafe_h__ 1

//
// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================
//

#include "XMPCommon/XMPCommonFwdDeclarations.h"

namespace AdobeXMPCommon {
	
	//!
	//! \brief Interface that serves as the base interface for all the externally exposed interfaces which needs to provide client configurable thread safety.
	//!          
	//! \attention In case client has disabled thread safety at the module level these functions will
	//!  have no use.
	//! \note By default all the objects created are not thread safe.
	//!
	class XMP_PUBLIC IThreadSafe
	{
	public:

		//!
		//! @brief Enables the thread safety on an object.
		//! @details After calling this function the object can be used across multiple threads.
		//!
		virtual void APICALL EnableThreadSafety() const __NOTHROW__ = 0;

		//!
		//! @brief Disables the thread safety on an object.
		//! @details After calling this function the object should not be used across multiple threads.
		//!
		virtual void APICALL DisableThreadSafety() const __NOTHROW__ = 0;

		//!
		//! @brief Informs whether object can be used across multiple threads or not.
		//! \returns bool value; true in case object can be used across multiple threads, false
		//!  otherwise.
		//!
		virtual bool APICALL IsThreadSafe() const = 0;

		//! @{
		//! \cond XMP_INTERNAL_DOCUMENTATION
		//!  @brief Return the pointer to the internal Thread Safe interface
		//! \return either a const or non const pointer to internal IThreadSafe_I interface.
		virtual AdobeXMPCommon_Int::pIThreadSafe_I APICALL GetIThreadSafe_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPCommon_Int::pcIThreadSafe_I GetIThreadSafe_I() const __NOTHROW__ {
			return const_cast< IThreadSafe * >( this )->GetIThreadSafe_I();
		}
        virtual ~IThreadSafe(){}
		//! \endcond
		//! @}

	protected:
		//! \cond XMP_INTERNAL_DOCUMENTATION
		//! @brief all safe functions
		virtual uint32 APICALL isThreadSafe() const __NOTHROW__ = 0;
		//! \endcond

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};

}
#endif  // IThreadSafe_h__
