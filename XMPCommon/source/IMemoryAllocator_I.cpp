// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/Interfaces/IMemoryAllocator_I.h"
#include "XMPCommon/ImplHeaders/MemoryAllocatorWrapperImpl.h"


namespace XMP_COMPONENT_INT_NAMESPACE {

	static MemoryAllocatorWrapperImpl sDefaultMemoryAllocator;

	pIMemoryAllocator IMemoryAllocator_I::GetMemoryAllocator() __NOTHROW__ {
		return &sDefaultMemoryAllocator;
	}

	pIMemoryAllocator IMemoryAllocator_I::SetMemoryAllocator( pIMemoryAllocator_base memoryAllocator ) __NOTHROW__ {
		return sDefaultMemoryAllocator.SetMemoryAllocator( memoryAllocator );
	}

}
