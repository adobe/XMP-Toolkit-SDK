#ifndef XMPCommon_I_h__
#define XMPCommon_I_h__ 1

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

#include "XMPCommon/XMPCommonDefines_I.h"
#include "XMPCommon/XMPCommonFwdDeclarations_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	//!
	//! Initializes the XMP Common Framework.
	//!
	void InitializeXMPCommonFramework();

	//!
	//! Terminates the XMP Common Framework.
	void TerminateXMPCommonFramework();
}

#endif  // XMPCommon_I_h__
