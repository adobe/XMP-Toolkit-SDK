#ifndef __XMPCommonFwdDeclarations_I_h__
#define __XMPCommonFwdDeclarations_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/XMPCommonDefines_I.h"
#include "XMPCommon/XMPCommonFwdDeclarations.h"

namespace XMP_COMPONENT_INT_NAMESPACE {
	using namespace AdobeXMPCommon;

#if SUPPORT_SHARED_POINTERS_IN_STD
	using std::weak_ptr;
	using std::dynamic_pointer_cast;
	using std::static_pointer_cast;
	using std::const_pointer_cast;
	using std::bad_weak_ptr;
	using std::default_delete;
#elif SUPPORT_SHARED_POINTERS_IN_TR1
	using std::tr1::weak_ptr;
	using std::tr1::dynamic_pointer_cast;
	using std::tr1::static_pointer_cast;
	using std::tr1::const_pointer_cast;
	using std::tr1::bad_weak_ptr;
	template< typename Ty>
	struct default_delete {
		void operator()( Ty * ptr ) const __NOTHROW__{
			delete ptr;
		}
	};
#endif

	// IUTF8String_I
	class IUTF8String_I;
	typedef IUTF8String_I *																pIUTF8String_I;
	typedef const IUTF8String_I *														pcIUTF8String_I;
	typedef shared_ptr< IUTF8String_I >													spIUTF8String_I;
	typedef shared_ptr< const IUTF8String_I >											spcIUTF8String_I;

	// IError_I
	class IError_I;
	typedef IError_I *																	pIError_I;
	typedef const IError_I *															pcIError_I;
	typedef shared_ptr< IError_I >														spIError_I;
	typedef shared_ptr< const IError_I >												spcIError_I;

	// IErrorNotifier_I
	class IErrorNotifier_I;
	typedef IErrorNotifier_I *															pIErrorNotifier_I;
	typedef const IErrorNotifier_I *													pcIErrorNotifier_I;
	typedef shared_ptr< IErrorNotifier_I >												spIErrorNotifier_I;
	typedef shared_ptr< const IErrorNotifier_I >										spcIErrorNotifier_I;

	// IMemoryAllocator_I
	class IMemoryAllocator_I;
	typedef IMemoryAllocator_I *														pIMemoryAllocator_I;
	typedef const IMemoryAllocator_I *													pcIMemoryAllocator_I;
	typedef shared_ptr< IMemoryAllocator_I >											spIMemoryAllocator_I;
	typedef shared_ptr< const IMemoryAllocator_I >										spcIClientAllocator_I;

	// IConfigurationManager_I
	class IConfigurationManager_I;
	typedef IConfigurationManager_I *													pIConfigurationManager_I;
	typedef const IConfigurationManager_I *												pcIConfigurationManager_I;
	typedef shared_ptr< IConfigurationManager_I >										spIConfigurationManager_I;
	typedef shared_ptr< const IConfigurationManager_I >									spcIConfigurationManager_I;

	// IConfigurable_I
	class IConfigurable_I;
	typedef IConfigurable_I *															pIConfigurable_I;
	typedef const IConfigurable_I *														pcIConfigurable_I;
	typedef shared_ptr< IConfigurable_I >												spIConfigurable_I;
	typedef shared_ptr< const IConfigurable_I >											spcIConfigurable_I;

	// ISharedMutex
	class ISharedMutex;
	typedef ISharedMutex *																pISharedMutex;
	typedef const ISharedMutex *														pcISharedMutex;
	typedef shared_ptr< ISharedMutex >													spISharedMutex;
	typedef shared_ptr< const ISharedMutex >											spcISharedMutex;

};

#endif  // __XMPCommonFwdDeclarations_I_h__
