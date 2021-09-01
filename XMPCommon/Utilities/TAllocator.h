#ifndef __TAllocator_h__
#define __TAllocator_h__ 1
// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/XMPCommonDefines_I.h"
#include "XMPCommon/Interfaces/IMemoryAllocator_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {
	using AdobeXMPCommon::sizet;

	//! \cond NEVER_IN_DOCUMENTATION
	// TEMPLATE CLASS _Allocator_base
	template< typename _Ty >
	struct _Allocator_base {	// base class for generic allocators
		typedef _Ty value_type;
	};

	// TEMPLATE CLASS _Allocator_base<const _Ty>
	template<typename _Ty >
	struct _Allocator_base< const _Ty > {	// base class for generic allocators for const _Ty
		typedef _Ty value_type;
	};
	//! \endcond

	//!
	//! \brief Templated class which implements all the functionality required by std::allocators.
	//! \details It allows to use STL objects which are memory managed by library's allocator manager.
	//!
	template < typename Ty >
	class TAllocator : public std::allocator< Ty > {

	public:
		TAllocator () {}
		template <class U> TAllocator( const TAllocator<U>& ) {} 

		typedef _Allocator_base<Ty>				Mybase;
		typedef typename Mybase::value_type		value_type;
		typedef AdobeXMPCommon::sizet			size_type;
		typedef ptrdiff_t						difference_type;

		typedef value_type *					pointer;
		typedef const value_type *				const_pointer;

		typedef value_type &					reference;
		typedef const value_type &				const_reference;

		pointer address( reference r ) const				{ return &r; }
		const_pointer address( const_reference r ) const	{ return &r; }

		size_type max_size() const 	{
			// impl stolen from windows default
			size_type max_num = ( size_type )( -1 ) / sizeof( Ty );
			return max_num > 0 ? max_num : 1;
		}

		void construct( pointer p, const Ty& val ) {
			::new(p) Ty(val);
		}

		void destroy( pointer p ) {
			p->~Ty();
		}

		template< class U >
		struct rebind {
			typedef TAllocator< U > other;
		};

		pointer allocate( size_type size ) {
			void * ptr;
			ptr = IMemoryAllocator_I::GetMemoryAllocator()->allocate( size * sizeof ( value_type ) );
			return (pointer) ptr;
		}

		void deallocate( void * p, size_type ) {
			IMemoryAllocator_I::GetMemoryAllocator()->deallocate( p );
		}
	};

	template< typename Ty > bool operator==( const TAllocator< Ty > &, const TAllocator< Ty > & ) {
		return true; // any two TAllocator< T >'s are the same
	}

	template< typename Ty > bool operator!=( const TAllocator< Ty > &, const TAllocator< Ty > & ) {
		return false; // any two TAllocator< T >'s are the same
	}
	
	extern TAllocator< spIUTF8String >													gTAllocatorUTF8String;

}

#endif  // __TAllocator_h__
