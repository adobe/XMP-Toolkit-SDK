#ifndef __TSmartPointers_I_h__
#define __TSmartPointers_I_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#include "XMPCommon/XMPCommonDefines_I.h"
#include "XMPCommon/Utilities/TAllocator.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCommon/XMPCommonErrorCodes_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	//!
	//! Template function to create a shared pointer of the same type from the raw pointer of an object implementing
	//! #AdobeXMPCommon::ISharedObject interface.
	//! \param[in] ptr ptr of template type Ty.
	//! \param[in] fileName pointer to a constant null terminated char buffer containing fileName in which error
	//! originated.
	//! \param[in] lineNumber a value of #AdobeXMPCommon::uint32 type respresenting the lineNumber at which error
	//! originated.
	//! \param[in] throwErrorOnNullPointer a boolean value indicating whether to treat NULL pointer as an error
	//! condition. Default is false.
	//! \return a shared pointer of object type Ty.
	//! \attention throws a pointer in case
	//! 	- throwErrorOnNullPointer is true and ptr is NULL.
	//!
	template< typename Ty >
	shared_ptr< Ty > MakeUncheckedSharedPointer( Ty * ptr, const char * fileName, sizet lineNumber, bool throwErrorOnNullPointer = false ) {
		if ( throwErrorOnNullPointer && ptr == NULL ) {
			auto err = IError_I::CreateError( IError_v1::kEDGeneral, kGECInternalFailure, IError_v1::kESOperationFatal );
			err->SetLocation( fileName, lineNumber );
			err->SetMessage( "NULL pointer provided for creating shared pointer" );
			throw err;
		}
		ptr->Acquire();
#if SUPPORT_SHARED_POINTERS_WITH_ALLOCATORS
		return shared_ptr< Ty >( ptr, mem_fn(&Ty::Release), TAllocator< Ty >() );
#else
		return shared_ptr< Ty >( ptr, mem_fn(&Ty::Release) );
#endif
	}

	//!
	//! Template function to create a shared pointer of the same type from the raw pointer of a class not
	//! inheriting from #AdobeXMPCommon::ISharedObject.
	//! \param[in] ptr ptr of template type Ty.
	//! \param[in] fileName pointer to a constant null terminated char buffer containing fileName in which error
	//! originated.
	//! \param[in] lineNumber a value of #AdobeXMPCommon::uint32 type respresenting the lineNumber at which error
	//! originated.
	//! \param[in] throwErrorOnNullPointer a boolean value indicating whether to treat NULL pointer as an error
	//! condition. Default is false.
	//! \return a shared pointer of object type Ty.
	//! \attention throws a pointer in case
	//! 	- throwErrorOnNullPointer is true and ptr is NULL.
	//!
	template< typename Ty >
	shared_ptr< Ty > MakeUncheckedSharedPointerWithDefaultDelete( Ty * ptr, const char * fileName, sizet lineNumber, bool throwErrorOnNullPointer = false ) {
		if ( throwErrorOnNullPointer && ptr == NULL ) {
			auto err = IError_I::CreateError( IError_v1::kEDGeneral, kGECInternalFailure, IError_v1::kESOperationFatal );
			err->SetLocation( fileName, lineNumber );
			err->SetMessage( "NULL pointer provided for creating shared pointer" );
			spcIError cError = err;
			throw cError;
		}
#if SUPPORT_SHARED_POINTERS_WITH_ALLOCATORS
		return shared_ptr< Ty >( ptr, default_delete< Ty >(), TAllocator< Ty >() );
#else
		return shared_ptr< Ty >( ptr, default_delete< Ty >() );
#endif
	}

}

#endif  // __TSmartPointers_I_h__
