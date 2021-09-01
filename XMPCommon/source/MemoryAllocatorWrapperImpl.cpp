// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCommon/ImplHeaders/MemoryAllocatorWrapperImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include <cstdlib>

namespace XMP_COMPONENT_INT_NAMESPACE {

	using AdobeXMPCommon::sizet;

	static const size_t kSIZE_OF_POINTER = sizeof( void * );
	static const size_t kOFFSET = ( ( kSIZE_OF_POINTER + sizeof( sizet ) - 1 ) / sizeof( sizet ) )
		* sizeof( sizet );

	MemoryAllocatorWrapperImpl::MemoryAllocatorWrapperImpl()
		: mpMemoryAllocator( NULL ) { }

	AdobeXMPCommon::pIMemoryAllocator_base MemoryAllocatorWrapperImpl::SetMemoryAllocator( pIMemoryAllocator_base memoryAllocator ) {
		auto retValue = mpMemoryAllocator;
		mpMemoryAllocator = memoryAllocator;
		return retValue;
	}

	void * APICALL MemoryAllocatorWrapperImpl::allocate( sizet size ) __NOTHROW__ {
		size_t actualSize = size + kOFFSET;
		void * memPtr = NULL;
		if ( mpMemoryAllocator )
			memPtr = mpMemoryAllocator->allocate( actualSize );
		else
			memPtr = malloc( actualSize );

		if ( memPtr ) {
			IMemoryAllocator ** address = ( IMemoryAllocator ** ) memPtr;
			*address = mpMemoryAllocator;
			return ( XMP_Uns8 * ) memPtr + kOFFSET;
		}
		return NULL;

	}

	void APICALL MemoryAllocatorWrapperImpl::deallocate( void * ptr ) __NOTHROW__ {
		void * actualMemPtr = ( XMP_Uns8 * ) ptr - kOFFSET;
		pIMemoryAllocator allocator = *( ( IMemoryAllocator ** ) actualMemPtr );
		if ( allocator )
			allocator->deallocate( actualMemPtr );
		else
			free( actualMemPtr );
	}

	void * APICALL MemoryAllocatorWrapperImpl::reallocate( void * ptr, sizet size ) __NOTHROW__ {
		size_t actualSize = size + kOFFSET;
		void * actualMemPtr = ( XMP_Uns8 * ) ptr - kOFFSET;
		void * memPtr( NULL );
		if ( mpMemoryAllocator )
			memPtr = mpMemoryAllocator->reallocate( actualMemPtr, actualSize );
		else
			memPtr = realloc( actualMemPtr, actualSize );

		if ( memPtr ) {
			IMemoryAllocator ** address = ( IMemoryAllocator ** ) memPtr;
			*address = mpMemoryAllocator;
			return ( XMP_Uns8 * ) memPtr + kOFFSET;
		}
		return NULL;
	}

}
