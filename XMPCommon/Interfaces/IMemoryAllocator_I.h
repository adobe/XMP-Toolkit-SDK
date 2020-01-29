#ifndef IMemoryAllocator_I_h__
#define IMemoryAllocator_I_h__ 1

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

#include "XMPCommon/XMPCommonFwdDeclarations_I.h"
#include "XMPCommon/Interfaces/IMemoryAllocator.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	//!
	//! \brief Internal interface that represents an interface to be implemented by client in case
	//! he is interested in controlling the memory allocation and deallocation on the heap.
	//! \details In case client is interested in controlling the memory allocation and deallocation on
	//! the heap he can implement this interface and register the same with the
	//! #AdobeXMPCommon::IConfigurationManager. For every request of memory allocation or deallocation on
	//! the heap corresponding function will be called by the library.
	//! \attention Support for Multi threading is under clients hand.
	//!
	class IMemoryAllocator_I
		: public virtual IMemoryAllocator
	{
	public:

		//!
		//! Get the current error notifier.
		//!
		static pIMemoryAllocator GetMemoryAllocator() __NOTHROW__;

		//!
		//! Set the current error notifier.
		//!
		static pIMemoryAllocator SetMemoryAllocator( pIMemoryAllocator_base memoryAllocator ) __NOTHROW__;


	protected:
		~IMemoryAllocator_I() {}

	#ifdef FRIEND_CLASS_DECLARATION
		REQ_FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
}

#endif // IMemoryAllocator_I_h__
