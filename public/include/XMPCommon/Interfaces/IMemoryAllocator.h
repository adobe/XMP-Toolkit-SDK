#ifndef IMemoryAllocator_h__
#define IMemoryAllocator_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"

#include "XMPCommon/XMPCommonFwdDeclarations.h"

namespace AdobeXMPCommon {

	//!
	//! \brief Version1 of the interface that represents an interface to be implemented by client in case
	//! he is interested in controlling the memory allocation and deallocation on the heap.
	//! \details In case client is interested in controlling the memory allocation and deallocation on
	//! the heap he can implement this interface and register the same with the
	//! \#AdobeXMPCommon::IConfigurationManager. For every request of memory allocation or deallocation on
	//! the heap corresponding function will be called by the library.
	//! \attention Support for Multi threading is under clients hand.
	//!
	class XMP_PUBLIC IMemoryAllocator_v1
	{
	public:
		//!
		//! @brief Called by the library whenever it needs some space on the heap.
		//! \param[in] size A value of type \#AdobeXMPCommon::sizet indicating the number of bytes
		//! required by the library on the heap.
		//! \return A pointer to memory location on the heap.
		//!
		virtual void * APICALL allocate( sizet size ) __NOTHROW__ = 0;

		//!
		//! @brief Called by the library whenever there is no further need for a previously allocated space on the heap.
		//! \param[in] ptr A pointer to a memory location which is no longer needed.
		//!
		virtual void APICALL deallocate( void * ptr ) __NOTHROW__ = 0;

		//!
		//! @brief Called by the library whenever it needs to expand or contract some space already allocated on
		//! the heap, preserving the contents.
		//! \param[in] ptr A pointer to a memory location which was previously allocated on the heap.
		//! \param[in] size A value of type \#AdobeXMPCommon::sizet indicating the new number of bytes
		//! required by the library on the heap.
		//! \return A pointer to memory location on the heap which is of new size and previous contents are
		//!  preserved.
		//!
		virtual void * APICALL reallocate( void * ptr, sizet size ) __NOTHROW__ = 0;
        virtual ~IMemoryAllocator_v1(){}

	protected:

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
};

#endif  // IMemoryAllocator_h__
