// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/XMPCommon_I.h"
#include "XMPCommon/Interfaces/IMemoryAllocator_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	void InitializeXMPCommonFramework() {
		IMemoryAllocator_I::SetMemoryAllocator( NULL );
	}

	void TerminateXMPCommonFramework() {
		IMemoryAllocator_I::SetMemoryAllocator( NULL );
	}
}
