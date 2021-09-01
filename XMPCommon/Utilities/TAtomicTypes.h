#ifndef __TAtomicTypes_h__
#define __TAtomicTypes_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCommon/XMPCommonDefines.h"

#if SUPPORT_STD_ATOMIC_IMPLEMENTATION
	#include <atomic>

	namespace AdobeXMPCommon {
		typedef std::atomic_size_t	atomic_sizet;
	}
#else

	#if XMP_WinBuild
		#include "XMPCommon/Utilities/TAtomicTypes_Win.h"
	#elif XMP_MacBuild || XMP_iOSBuild
		#include "XMPCommon/Utilities/TAtomicTypes_Mac.h"
	#elif XMP_UNIXBuild
		#include "XMPCommon/Utilities/TAtomicTypes_Linux.h"
    #elif XMP_AndroidBuild
    	#include "XMPCommon/Utilities/TAtomicTypes_Android.h"
	#endif

	#ifndef NOT_DEFINED_ATOMIC_SIZE_T
		#define NOT_DEFINED_ATOMIC_SIZE_T 1
	#endif

#if NOT_DEFINED_ATOMIC_SIZE_T

	namespace AdobeXMPCommon {

	typedef enum memory_order {
		memory_order_relaxed,
		memory_order_consume,
		memory_order_acquire,
		memory_order_release,
		memory_order_acq_rel,
		memory_order_seq_cst
	} memory_order;

	class atomic_sizet {
	public:
		atomic_sizet() __NOTHROW__
			: mValue( 0 ){ }

		atomic_sizet( sizet _Val ) __NOTHROW__
		: mValue( _Val ) { }

		void store( sizet value, memory_order = memory_order_seq_cst ) volatile __NOTHROW__ {
			AtomicWrite( this->mValue, value );
		}
		void store( sizet value, memory_order = memory_order_seq_cst ) __NOTHROW__ {
			AtomicWrite( this->mValue, value );
		}

		sizet load( memory_order = memory_order_seq_cst ) const volatile __NOTHROW__ {
			return AtomicRead( this->mValue );
		}
		sizet load( memory_order = memory_order_seq_cst ) const __NOTHROW__ {
			return AtomicRead( this->mValue );
		}

		operator sizet() const volatile __NOTHROW__ {
			return mValue;
		}
		operator sizet() const __NOTHROW__ {
			return mValue;
		}

		sizet operator++( int ) volatile __NOTHROW__ {
			sizet value = AtomicRead( this->mValue );
			AtomicIncrement( this->mValue );
			return value;
		}
		sizet operator++( int ) __NOTHROW__ {
			sizet value = AtomicRead( this->mValue );
			AtomicIncrement( this->mValue );
			return value;
		}

		sizet operator--( int ) volatile __NOTHROW__ {
			sizet value = AtomicRead( this->mValue );
			AtomicDecrement( this->mValue );
			return value;
		}
		sizet operator--( int ) __NOTHROW__ {
			sizet value = AtomicRead( this->mValue );
			AtomicDecrement( this->mValue );
			return value;
		}

		sizet operator++() volatile __NOTHROW__ {
			return AtomicIncrement( this->mValue );
		}
		sizet operator++() __NOTHROW__ {
			return AtomicIncrement( this->mValue );
		}

		sizet operator--() volatile __NOTHROW__ {
			return AtomicDecrement( this->mValue );
		}
		sizet operator--() __NOTHROW__ {
			return AtomicDecrement( this->mValue );
		}

	private:
		atomic_sizet( const atomic_sizet & );	// not defined
		//atomic_sizet & operator=( const atomic_sizet & );	// not defined
		//atomic_sizet & operator=( const atomic_sizet & ) volatile;	// not defined

	protected:
		sizet			mValue;
	};
}

#endif  // NOT_DEFINED_ATOMIC_SIZE_T

#endif  // SUPPORT_STD_ATOMIC_IMPLEMENTATION

#endif //  __TAtomicTypes_h__
