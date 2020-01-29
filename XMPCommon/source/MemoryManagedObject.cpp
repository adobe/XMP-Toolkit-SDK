// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================


#include "XMPCommon/BaseClasses/MemoryManagedObject.h"
#include "XMPCommon/Interfaces/IMemoryAllocator_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	void * MemoryManagedObject::operator new( std::size_t size ) {
		void * ptr = operator new ( size, std::nothrow_t() );
		if ( ptr )
			return ptr;
		throw std::bad_alloc();
	}

	void * MemoryManagedObject::operator new( std::size_t size, const std::nothrow_t & nothrow ) throw () {
		return IMemoryAllocator_I::GetMemoryAllocator()->allocate( size );
	}

	void * MemoryManagedObject::operator new( std::size_t size, void * ptr ) throw () {
		return ptr;
	}

	void * MemoryManagedObject::operator new[]( std::size_t size ) {
		return operator new( size );
	}

	void * MemoryManagedObject::operator new[]( std::size_t size, const std::nothrow_t & nothrow ) throw () {
		return operator new( size, nothrow );
	}

	void * MemoryManagedObject::operator new[]( std::size_t size, void * ptr ) throw () {
		return operator new( size, ptr );
	}

	void MemoryManagedObject::operator delete( void * ptr ) throw () {
		IMemoryAllocator_I::GetMemoryAllocator()->deallocate( ptr );
	}

	void MemoryManagedObject::operator delete( void * ptr, const std::nothrow_t & nothrow ) throw () {
		return operator delete( ptr );
	}

	void MemoryManagedObject::operator delete( void * ptr, void * voidptr2 ) throw () {
		return;
	}

	void MemoryManagedObject::operator delete[]( void * ptr ) throw () {
		return operator delete( ptr );
	}

	void MemoryManagedObject::operator delete[]( void * ptr, const std::nothrow_t & nothrow ) throw () {
		return operator delete( ptr, nothrow );
	}

	void MemoryManagedObject::operator delete[]( void * ptr, void * voidptr2 ) throw () {
		return operator delete( ptr, voidptr2 );
	}

}

