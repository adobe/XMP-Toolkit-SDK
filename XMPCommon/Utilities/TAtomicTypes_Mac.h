#ifndef __TAtomicTypes_Mac_h__
#define __TAtomicTypes_Mac_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#if XMP_MacBuild || XMP_iOSBuild

#ifndef __TAtomicTypes_h__
#error	"This file is supposed to be included from TAtomicTypes and not directly"
#endif

#include <libkern/OSAtomic.h>
#include <sched.h>
#include <assert.h>

namespace AdobeXMPCommon {

	/**
	 **	Atomically sets ioTarget to inNewValue if ioTarget == inCompare
	 **	returns true if succeeded
	 */
	inline bool AtomicCompareAndSet( volatile int32 & ioTarget, int32 inCompare, int32 inNewValue ) {
		return ::OSAtomicCompareAndSwap32Barrier( inCompare, inNewValue, const_cast< int32 * >( &ioTarget ) );
	}

	inline bool AtomicCompareAndSet( volatile uint32 & ioTarget, uint32 inCompare, uint32 inNewValue ) {
		return ::OSAtomicCompareAndSwap32Barrier( static_cast< int32 >( inCompare ), static_cast< int32 >( inNewValue ),
			const_cast< int32 * >( reinterpret_cast< volatile int32 * >( &ioTarget ) ) );
	}
	
	inline bool AtomicCompareAndSet( volatile int64 & ioTarget, int64 inCompare, int64 inNewValue ) {
		return ::OSAtomicCompareAndSwap64Barrier( inCompare, inNewValue, const_cast< int64 * >( &ioTarget ) );
	}

	inline bool AtomicCompareAndSet( volatile uint64 & ioTarget, uint64 inCompare, uint64 inNewValue ) {
		return ::OSAtomicCompareAndSwap64Barrier( static_cast< int64 >( inCompare ), static_cast< int64 >( inNewValue ),
			const_cast< int64 * >( reinterpret_cast< volatile int64 * >( &ioTarget ) ) );
	}

	/**
	 **	Pointer CAS specialization - note that this needs to come before the
	 **	below specializations to be visible
	 */
	template< typename T >
	inline bool AtomicCompareAndSet( T * volatile & ioTarget, T * inCompare, T * inNewValue ) {
		assert( sizeof( T * ) == sizeof( sizet ) );
		return AtomicCompareAndSet( reinterpret_cast< sizet volatile & >( ioTarget ),
			reinterpret_cast< sizet >( inCompare ), reinterpret_cast< sizet >( inNewValue ) );
	}

	/**
	 **	Preform a non-atomic read of the value ensuring the compiler does not
	 **	optimize out the read. Typically only safe for later compare and set.
	 */
	template< typename AtomicType >
	inline AtomicType VolatileRead( const volatile AtomicType & inTarget ) {
		return inTarget;
	}

	/**
	 **	A read with an acquire memory barrier after the read.
	 */
	template< typename AtomicType >
	inline AtomicType AtomicRead( const volatile AtomicType & inTarget ) {
		AtomicType result;
		for ( ; ; ) {
			result = VolatileRead( inTarget );
			if ( AtomicCompareAndSet( const_cast< volatile AtomicType & >( inTarget ), result, result ) ) {
				break;
			}
		}
		return result;
	}

	/**
	 **	Atomically sets a new value
	 */
	template< typename AtomicType, typename ValueType >
	inline ValueType AtomicWrite( volatile AtomicType & ioTarget, ValueType inNewValue ) {
		ValueType result;
		for ( ; ; ) {
			result = static_cast< ValueType >( VolatileRead( ioTarget ) );
			if ( AtomicCompareAndSet( ioTarget, result, inNewValue ) ) {
				break;
			}
		}
		return result;
	}

	inline uint32 AtomicAdd( volatile uint32 & ref, uint32 value ) {
		return ::OSAtomicAdd32( static_cast< int32 >( value ), reinterpret_cast< volatile int32 * >( &ref ) );
	}

	inline uint64 AtomicAdd( volatile uint64 & ref, uint64 value ) {
		return ::OSAtomicAdd64( static_cast< int64 >( value ), reinterpret_cast< volatile int64 * >( &ref ) );
	}

	inline int32 AtomicAdd( volatile int32 & ref, int32 value ) {
		return ::OSAtomicAdd32( value, &ref );
	}

	inline int64 AtomicAdd( volatile int64 & ref, int64 value ) {
		return ::OSAtomicAdd64( value, &ref );
	}

	inline uint32 AtomicIncrement( volatile uint32 & ref ) {
		return ::OSAtomicIncrement32( reinterpret_cast< volatile int32 * >( &ref ) );
	}

	inline uint64 AtomicIncrement( volatile uint64 & ref ) {
		return ::OSAtomicIncrement64( reinterpret_cast< volatile int64 * >( &ref ) );
	}

	inline int32 AtomicIncrement( volatile int32 & ref ) {
		return ::OSAtomicIncrement32( &ref );
	}

	inline int64 AtomicIncrement( volatile int64 & ref ) {
		return ::OSAtomicIncrement64( &ref );
	}

	inline uint32 AtomicDecrement( volatile uint32 & ref ) {
		return ::OSAtomicDecrement32( reinterpret_cast< volatile int32 * >( &ref ) );
	}

	inline uint64 AtomicDecrement( volatile uint64 & ref ) {
		return ::OSAtomicDecrement64( reinterpret_cast< volatile int64 * >( &ref ) );
	}

	inline int32 AtomicDecrement( volatile int32 & ref ) {
		return ::OSAtomicDecrement32( &ref );
	}

	inline int64 AtomicDecrement( volatile int64 & ref ) {
		return ::OSAtomicDecrement64( &ref );
	}

}

#endif

#endif  // __TAtomicTypes_Mac_h__
