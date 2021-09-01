#ifndef __ISharedObject_h__
#define __ISharedObject_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/XMPCommonFwdDeclarations.h"

namespace AdobeXMPCommon {

	//!
	//! \brief Interface that serves as the base interface of all the externally exposed interfaces.
	//! \details This allows all interfaces to be used as shared pointers so as to reduce the burden of
	//! scope management from the client or library code. It makes the heap allocated object to be self manged in
	//! in terms of memory and life. This provides functions so as to inform the actual object when a shared pointer
	//! is created or destroyed and appropriately release the memory during the last call to Release.
	//! \attention Supports Multi-threading at object level through use of Atomic Variables.
	//! \note Any interface which inherits from this needs to make sure that its destructor is declared protected
	//!  so that unknowingly also client of this object cannot call delete on the object.
	//!

	class XMP_PUBLIC ISharedObject {
	public:

		//!
		//! @brief Called by the clients of the object to indicate that he has acquired the shared ownership of the object.
		//!
		virtual void APICALL Acquire() const __NOTHROW__ = 0;

		//!
		//! @brief Called by the clients of the object to indicate he has released his shared ownership of the object.
		//! If this being the last client than this function should call Destroy to delete and release the memory.
		//!
		virtual void APICALL Release() const __NOTHROW__ = 0;

		//! @{
		//! \cond XMP_INTERNAL_DOCUMENTATION
		//! Return the pointer to the internal Shared Object interface
		//! \return either a const or non const pointer to internal ISharedObject_I interface.
		virtual AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPCommon_Int::pcISharedObject_I GetISharedObject_I() const __NOTHROW__ {
			return const_cast< ISharedObject * >( this )->GetISharedObject_I();
		}
		//! \endcond
		//! @}

	protected:

		//!
		//! protected virtual destructor.
		//!
		virtual ~ISharedObject() __NOTHROW__ = 0;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif

		REQ_FRIEND_CLASS_DECLARATION();
	};

	inline ISharedObject::~ISharedObject() __NOTHROW__ { }

};

#endif  // __ISharedObject_h__
