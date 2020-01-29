#ifndef __TAtomicTypes_Win_h__
#define __TAtomicTypes_Win_h__ 1

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

#if XMP_WinBuild

#ifndef __TAtomicTypes_h__
	#error	"This file is supposed to be included from TAtomicTypes and not directly"
#endif

#include <Windows.h>

namespace AdobeXMPCommon {

	inline int32 AtomicRead( const volatile int32 & ref ) {
		return InterlockedCompareExchange( const_cast< volatile int32 * >( &ref ), 0, 0 );
	}

	inline uint32 AtomicRead( const volatile uint32 & ref ) {
		return InterlockedCompareExchange( const_cast< volatile uint32 *> ( &ref ), 0 , 0 );
	}

	inline int64 AtomicRead( const volatile int64 & ref ) {
		return InterlockedCompareExchange64( const_cast< volatile int64 * >( &ref ), 0, 0 );
	}

	inline uint64 AtomicRead( const volatile uint64 & ref ) {
		return InterlockedCompareExchange( const_cast< volatile uint64 *> ( &ref ), 0 , 0 );
	}

	inline int32 AtomicWrite( volatile int32 & ref, int32 value ) {
		return InterlockedExchange( &ref, value );
	}

	inline uint32 AtomicWrite( volatile uint32 & ref, uint32 value ) {
		return InterlockedExchange( &ref, value );
	}

	inline int64 AtomicWrite( volatile int64 & ref, int32 value ) {
		return InterlockedExchange64( &ref, value );
	}

	inline uint64 AtomicWrite( volatile uint64 & ref, uint64 value ) {
		return InterlockedExchange( &ref, value );
	}

	inline uint32 AtomicAdd( volatile uint32 & ref, uint32 value ) {
		return InterlockedExchangeAdd( &ref, value );
	}

	inline uint64 AtomicAdd( volatile uint64 & ref, uint64 value ) {
		return InterlockedExchangeAdd( &ref, value );
	}

	inline int32 AtomicAdd( volatile int32 & ref, int32 value ) {
		return InterlockedExchangeAdd( &ref, value );
	}

	inline int64 AtomicAdd( volatile int64 & ref, int64 value ) {
		return InterlockedExchangeAdd64( &ref, value );
	}

	inline uint32 AtomicIncrement( volatile uint32 & ref ) {
		return InterlockedIncrement( &ref );
	}

	inline uint64 AtomicIncrement( volatile uint64 & ref ) {
		return InterlockedIncrement( &ref );
	}

	inline int32 AtomicIncrement( volatile int32 & ref ) {
		return InterlockedIncrement( &ref );
	}

	inline int64 AtomicIncrement( volatile int64 & ref ) {
		return InterlockedIncrement64( &ref );
	}

	inline uint32 AtomicDecrement( volatile uint32 & ref ) {
		return InterlockedDecrement( &ref );
	}

	inline uint64 AtomicDecrement( volatile uint64 & ref ) {
		return InterlockedDecrement( &ref );
	}

	inline int32 AtomicDecrement( volatile int32 & ref ) {
		return InterlockedDecrement( &ref );
	}

	inline int64 AtomicDecrement( volatile int64 & ref ) {
		return InterlockedDecrement64( &ref );
	}

}

#endif

#endif  // __TAtomicTypes_Win_h__