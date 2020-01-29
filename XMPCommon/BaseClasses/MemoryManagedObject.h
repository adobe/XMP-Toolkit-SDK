#ifndef MemoryManagedObject_h__
#define MemoryManagedObject_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#include "XMPCommon/XMPCommonDefines_I.h"
#include "XMPCommon/XMPCommonFwdDeclarations.h"

namespace XMP_COMPONENT_INT_NAMESPACE {
	using AdobeXMPCommon::pIMemoryAllocator;
	//!
	//! \brief class which serves as the base class for all internal concrete classes.
	//! \details Provides all the concrete classes with a set of new/delete functions which internally
	//! calls appropriate client or inbuilt library functions for all memory allocation and deallocation
	//! on heap.
	//! \attention no state maintained so no need to worry about multi threading.
	//!
	class MemoryManagedObject {

	public:
		void * operator new( std::size_t );
		void * operator new( std::size_t, const std::nothrow_t & ) __NOTHROW__;
		void * operator new( std::size_t, void * ptr ) __NOTHROW__;

		void * operator new[]( std::size_t );
		void * operator new[]( std::size_t, const std::nothrow_t & ) __NOTHROW__;
		void * operator new[]( std::size_t, void * ptr ) __NOTHROW__;

		void operator delete( void * ptr ) throw ();
		void operator delete( void * ptr, const std::nothrow_t & ) __NOTHROW__;
		void operator delete( void * ptr, void * voidptr2 ) __NOTHROW__;

		void operator delete[]( void * ptr ) throw ();
		void operator delete[]( void * ptr, const std::nothrow_t & ) __NOTHROW__;
		void operator delete[]( void * ptr, void * voidptr2 ) __NOTHROW__;

	};
};

#endif  // MemoryManagedObject_h__
