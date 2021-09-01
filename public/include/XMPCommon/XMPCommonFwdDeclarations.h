#ifndef __XMPCommonFwdDeclarations_h__
#define __XMPCommonFwdDeclarations_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/XMPCommonDefines.h"
#include "XMPCommon/XMPCommonLatestInterfaceVersions.h"
#include <vector>

#if SUPPORT_SHARED_POINTERS_IN_STD
	#include <memory>
	#include <functional>
#elif SUPPORT_SHARED_POINTERS_IN_TR1
	#if XMP_WinBuild
		#include <memory>
	#else
		#include <tr1/memory>
        #include <tr1/functional>
	#endif
#else
	#error "location of shared pointer stuff is unknown"
#endif

namespace AdobeXMPCommon {

	#if SUPPORT_SHARED_POINTERS_IN_STD
		using std::shared_ptr;
		using std::enable_shared_from_this;
		using std::mem_fn;
	#elif SUPPORT_SHARED_POINTERS_IN_TR1
		using std::tr1::shared_ptr;
		using std::tr1::enable_shared_from_this;
    using std::tr1::mem_fn;
	#endif

	// void
	typedef void *																pvoid;
	typedef const void *														pcvoid;
	typedef shared_ptr< void >													spvoid;
	typedef shared_ptr< const void * >											spcvoid;

	// IObjectFactory
	class IObjectFactory_v1;
	typedef IObjectFactory_v1													IObjectFactory_base;
	typedef IObjectFactory_v1 *													pIObjectFactory_base;
	typedef const IObjectFactory_v1 *											pcIObjectFactory_base;
	typedef BASE_CLASS( IObjectFactory, IOBJECTFACTORY_VERSION )				IObjectFactory;
	typedef IObjectFactory *													pIObjectFactory;
	typedef const IObjectFactory *												pcIObjectFactory;

	// IError
	class IError_v1;
	typedef IError_v1															IError_base;
	typedef IError_v1 *															pIError_base;
	typedef const IError_v1 *													pcIError_base;
	typedef BASE_CLASS( IError, IERROR_VERSION )								IError;
	typedef IError *															pIError;
	typedef const IError *														pcIError;
	typedef shared_ptr< IError >												spIError;
	typedef shared_ptr< const IError >											spcIError;

	// IUTF8String
	class IUTF8String_v1;
	typedef IUTF8String_v1														IUTF8String_base;
	typedef IUTF8String_v1 *													pIUTF8String_base;
	typedef const IUTF8String_v1 *												pcIUTF8String_base;
	typedef BASE_CLASS( IUTF8String, IUTF8STRING_VERSION )						IUTF8String;
	typedef IUTF8String *														pIUTF8String;
	typedef const IUTF8String *													pcIUTF8String;
	typedef shared_ptr< IUTF8String >											spIUTF8String;
	typedef shared_ptr< const IUTF8String >										spcIUTF8String;

	// IMemoryAllocator
	class IMemoryAllocator_v1;
	typedef IMemoryAllocator_v1													IMemoryAllocator_base;
	typedef IMemoryAllocator_v1 *												pIMemoryAllocator_base;
	typedef const IMemoryAllocator_v1 *											pcIMemoryAllocator_base;
	typedef BASE_CLASS( IMemoryAllocator, IMEMORYALLOCATOR_VERSION )			IMemoryAllocator;
	typedef IMemoryAllocator *													pIMemoryAllocator;
	typedef const IMemoryAllocator *											pcIMemoryAllocator;
	typedef shared_ptr< IMemoryAllocator >										spIMemoryAllocator;
	typedef shared_ptr< const IMemoryAllocator >								spcIMemoryAllocator;

	// IErrorNotifier
	class IErrorNotifier_v1;
	typedef IErrorNotifier_v1													IErrorNotifier_base;
	typedef IErrorNotifier_v1 *													pIErrorNotifier_base;
	typedef const IErrorNotifier_v1 *											pcIErrorNotifier_base;
	typedef BASE_CLASS( IErrorNotifier, IERRORNOTIFIER_VERSION )				IErrorNotifier;
	typedef IErrorNotifier *													pIErrorNotifier;
	typedef const IErrorNotifier *												pcIErrorNotifier;
	typedef shared_ptr< IErrorNotifier >										spIErrorNotifier;
	typedef shared_ptr< const IErrorNotifier >									spcIErrorNotifier;

	// IConfigurationManager
	class IConfigurationManager_v1;
	typedef IConfigurationManager_v1											IConfigurationManager_base;
	typedef IConfigurationManager_v1 *											pIConfigurationManager_base;
	typedef const IConfigurationManager_v1 *									pcIConfigurationManager_base;
	typedef BASE_CLASS( IConfigurationManager, ICONFIGURATIONMANAGER_VERSION )	IConfigurationManager;
	typedef IConfigurationManager *												pIConfigurationManager;
	typedef const IConfigurationManager *										pcIConfigurationManager;
	typedef shared_ptr< IConfigurationManager >									spIConfigurationManager;
	typedef shared_ptr< const IConfigurationManager >							spcIConfigurationManager;

	// IConfigurable
	class IConfigurable;
	typedef IConfigurable *														pIConfigurable;
	typedef const IConfigurable *												pcIConfigurable;

	// typedefs for vectors and their corresponding shared pointers.
	typedef std::vector< spIUTF8String >										IUTF8Strings;
	typedef std::vector< spcIUTF8String >										cIUTF8Strings;
	typedef shared_ptr< IUTF8Strings >											spIUTF8Strings;
	typedef shared_ptr< cIUTF8Strings >											spcIUTF8Strings;
	typedef shared_ptr< const IUTF8Strings >									spIUTF8Strings_const;
	typedef shared_ptr< const cIUTF8Strings >									spcIUTF8Strings_const;

	//!
	//! @brief A function pointer to get the memory allocated from the library.
    //! \param[in] size a value indicating the number of bytes to be allocated.
	//! \return a pointer to memory allocated by the library.
	//! \note NULL value is returned in case memory allocation fails.
	//!
	typedef void * ( *MemAllocateProc )( sizet size );
	void * MemAllocate( sizet size ) __NOTHROW__;

	//!
	//! @brief A function pointer to get the memory freed from the library.
	//! \param[in] ptr address of the memory location to be freed.
	//!
	typedef void( *MemReleaseProc )( void * ptr );
	void MemRelease( void * ptr ) __NOTHROW__;

}  // namespace AdobeXMPCommon

namespace AdobeXMPCommon_Int {

	// ISharedObject_I
	class ISharedObject_I;
	typedef ISharedObject_I *													pISharedObject_I;
	typedef const ISharedObject_I *												pcISharedObject_I;

	// IThreadSafe_I
	class IThreadSafe_I;
	typedef IThreadSafe_I *														pIThreadSafe_I;
	typedef const IThreadSafe_I *												pcIThreadSafe_I;

}

#endif  // __XMPCommonFwdDeclarations_h__
