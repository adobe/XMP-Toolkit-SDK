#ifndef TWrapperFunctions_h__
#define TWrapperFunctions_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/Interfaces/IError.h"

#if SUPPORT_VARIADIC_TEMPLATES
//! \cond XMP_INTERNAL_DOCUMENTATION
namespace AdobeXMPCommon {

	//!
	//! @{
	//! template functions taking care of all the functions where return types of client exposed function as well as
	//! DLL/library safe function are void.
	//! \details It calls the underlying DLL/library safe function and in case any error is reported back through
	//! pointer, it throws the error to the upper functions to handle.
	//! \param[in] ptr non const pointer to an object of className itself ( most probably this pointer ).
	//! \param[in] Func function pointer to non const member function of className accepting 0 or n number of variables
	//! anad a reference to a pointer to const #AdobeXMPCommon::IError_base object and returning void.
	//! \param[in] Vs n number of variables of varying/same type.
	//! \note These takes care of non const functions.
	//!
	template < typename className >
	void CallSafeFunctionReturningVoid( className * ptr,
		void ( APICALL className::*Func )( pcIError_base & ) )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( error );
		if ( error ) throw IError::MakeShared( error );
	}

	template< typename className, typename ... Ts >
	void CallSafeFunctionReturningVoid( className * ptr,
		void ( APICALL className::*Func )( Ts ..., pcIError_base & ), Ts ... Vs )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( Vs ..., error );
		if ( error ) throw IError::MakeShared( error );
	}
	//!
	//! @}

	//!
	//! @{
	//! template functions taking care of all the functions where return types of client exposed function as well as
	//! DLL/library safe function are void.
	//! \details It calls the underlying DLL/library safe function and in case any error is reported back through
	//! pointer, it throws the error to the upper functions to handle.
	//! \param[in] ptr const pointer to a const object of className itself ( most probably this pointer ).
	//! \param[in] Func function pointer to const member function of className accepting 0 or n number of variables
	//! and a reference to a pointer to const #AdobeXMPCommon::IError_base object and returning void.
	//! \param[in] Vs n number of variables of varying/same type.
	//! \note These takes care of const functions.
	//!
	template < typename className >
	void CallConstSafeFunctionReturningVoid( const className * const ptr,
		void ( APICALL className::*Func )( pcIError_base & ) const )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( error );
		if ( error ) throw IError::MakeShared( error );
	}

	template < typename className, typename ... Ts >
	void CallConstSafeFunctionReturningVoid( const className * const ptr,
		void ( APICALL className::*Func )( Ts ..., pcIError_base & ) const, Ts ... Vs )
	{
		pcIError_base error( NULL );
		( ptr->*Func )( Vs ..., error );
		if ( error ) throw IError::MakeShared( error );
	}
	//!
	//! @}

	//!
	//! @{
	//! template functions taking care of all the functions where return types of client exposed function as well as
	//! DLL/library safe function are nor void type neither shared pointers.
	//! \details It calls the underlying DLL/library safe function and in case any error is reported back through
	//! pointer, it throws the error to the upper function to handle.
	//! \param[in] ptr non const pointer to an object of className itself ( most probably this pointer ).
	//! \param[in] Func function pointer to non const member function of className accepting 0 or n number of variables
	//! and a reference to a pointer to const #AdobeXMPCommon::IError_base object and returning value of type internalReturnType.
	//! \param[in] Vs n number of variables of varying/same type.
	//! These take care of non const functions.
	//!
	template < typename className, typename returnType, typename internalReturnType >
	returnType CallSafeFunction( className * ptr,
		internalReturnType ( APICALL className::*Func )( pcIError_base & ) )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}

	template < typename className, typename returnType, typename internalReturnType, typename ... Ts >
	returnType CallSafeFunction( className * ptr,
		internalReturnType ( APICALL className::*Func )( Ts ..., pcIError_base & ), Ts ... Vs )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( Vs ..., error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}
	//!
	//! @}

	//!
	//! @{
	//! template functions taking care of all the functions where return types of client exposed function as well as
	//! DLL/library safe function are nor void type neither shared pointers.
	//! \details It calls the underlying DLL/library safe function and in case any error is reported back through
	//! pointer, it throws the error to the upper function to handle.
	//! \param[in] ptr const pointer to a const object of className itself ( most probably this pointer ).
	//! \param[in] Func function pointer to const member function of className accepting 0 or n number of variables
	//! and a reference to a pointer to const #AdobeXMPCommon::IError_base object and returning value of type internalReturnType.
	//! \param[in] Vs n number of variables of varying/same type.
	//! These take care of const functions.
	//!
	template < typename className, typename returnType, typename internalReturnType >
	returnType CallConstSafeFunction( const className * const ptr,
		internalReturnType ( APICALL className::*Func )( pcIError_base & ) const )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}

	template < typename className, typename returnType, typename internalReturnType, typename ... Ts >
	returnType CallConstSafeFunction( const className * const ptr,
		internalReturnType ( APICALL className::*Func )( Ts ..., pcIError_base & ) const, Ts ... Vs )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( Vs ..., error );
		if ( error ) throw IError::MakeShared( error );
	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4800 )
	#endif
		return static_cast< returnType >( returnValue );
	#if XMP_WinBuild
		#pragma warning( pop )
	#endif
	}
	//!
	//! @}

	//!
	//! @{
	//! template functions taking care of all the functions where return types of client exposed function is a
	//! shared pointer.
	//! \details It calls the underlying DLL/library safe function and in case any error is reported back through
	//! pointer, it throws the error to the upper function to handle.
	//! \param[in] ptr non const pointer to an object of className itself ( most probably this pointer ).
	//! \param[in] Func function pointer to non const member function of className accepting 0 or n number of variables
	//! and a reference to a pointer to const #AdobeXMPCommon::IError_base object and returning internalReturnType.
	//! \param[in] Vs n number of variables of varying/same type.
	//! These take care of non const functions.
	//!
	template < typename className, typename internalReturnType, typename sharedPointerType >
	shared_ptr< sharedPointerType > CallSafeFunctionReturningPointer( className * ptr,
		internalReturnType ( APICALL className::*Func )( pcIError_base & ) )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType, typename ... Ts >
	shared_ptr< sharedPointerType > CallSafeFunctionReturningPointer( className * ptr,
		internalReturnType ( APICALL className::*Func )( Ts ..., pcIError_base & ), Ts ... Vs )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( Vs ..., error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}
	//!
	//! @}

	//!
	//! @{
	//! template functions taking care of all the functions where return types of client exposed function is a
	//! shared pointer.
	//! \details It calls the underlying DLL/library safe function and in case any error is reported back through
	//! pointer, it throws the error to the upper function to handle.
	//! \param[in] ptr const pointer to a const object of className itself ( most probably this pointer ).
	//! \param[in] Func function pointer to const member function of className accepting 0 or n number of variables
	//! and a reference to a pointer to const #AdobeXMPCommon::IError_base object and returning internalReturnType.
	//! \param[in] Vs n number of variables of varying/same type.
	//! These take care of const functions.
	//!
	template < typename className, typename internalReturnType, typename sharedPointerType >
	shared_ptr< sharedPointerType > CallConstSafeFunctionReturningPointer( const className * const ptr,
		internalReturnType ( APICALL className::*Func )( pcIError_base & ) const )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = (ptr->*Func)( error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}

	template < typename className, typename internalReturnType, typename sharedPointerType, typename ... Ts >
	shared_ptr< sharedPointerType > CallConstSafeFunctionReturningPointer( const className * const ptr,
		internalReturnType ( APICALL className::*Func )( Ts ..., pcIError_base & ) const, Ts ... Vs )
	{
		pcIError_base error( NULL );
		internalReturnType returnValue = ( ptr->*Func )( Vs ..., error );
		if ( error ) throw IError::MakeShared( error );
		return sharedPointerType::MakeShared( returnValue );
	}
	//!
	//! @}
}

//! \endcond
#else
	#include "XMPCommon/Utilities/TWrapperFunctions2.h"
#endif
#endif  // TWrapperFunctions_h__
