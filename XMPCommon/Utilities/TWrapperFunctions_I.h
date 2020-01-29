#ifndef TWrapperFunctions_I_h__
#define TWrapperFunctions_I_h__ 1

#include "XMPCommon/XMPCommonDefines_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions2_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {
	using namespace AdobeXMPCommon;
	using namespace AdobeXMPCommon_Int;
	//!
	//! @{
	//! template functions taking care of all the functions where return types of safe and unsafe functions
	//! are void.
	//! \details It calls the underlying unsafe function and catches all kinds of exceptions and report back them
	//! through a pointer.
	//! \param[in] error a reference to a pointer to a constant to const #AdobeXMPCommon::IError_base object.
	//! \param[in] ptr non const pointer to an object of className itself ( most probably this pointer ).
	//! \param[in] Func function pointer to non const member function of className accepting 0 or more variables
	//! and returning void.
	//! \param[in] Vs n number of variables of varying/same type.
	//! \param[in] fileName a null terminated array of characters containing file name.
	//! \param[in] lineNumber an unsigned 32 bit number containing the line number.
	//! \note These take care of non const functions.
	//!
	template < typename className >
	void CallUnSafeFunctionReturningVoid( pcIError_base & error, className * ptr,
		void ( APICALL className::*Func )(), const char * fileName, uint32 lineNumber )
	{
		error = NULL;
		try {
			(ptr->*Func)();
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		} catch ( ... ) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError( IError_v1::kESOperationFatal, fileName, lineNumber ).get();
			err->AcquireInternal();
			error = err;
		}
	}

	template < typename className, typename ... Ts >
	void CallUnSafeFunctionReturningVoid( pcIError_base & error, className * ptr,
		void ( APICALL className::*Func )( Ts ... ), const char * fileName, uint32 lineNumber, Ts ... Vs )
	{
		error = NULL;
		try {
			( ptr->*Func )( Vs ... );
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		} catch ( ... ) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError( IError_v1::kESOperationFatal, fileName, lineNumber ).get();
			err->AcquireInternal();
			error = err;
		}
	}
	
	//! 
	//! @{
	//! template functions taking care of all the functions where return types of safe and unsafe functions
	//! are void.
	//! \details It calls the underlying unsafe function and catches all kinds of exceptions and report back them
	//! through a pointer.
	//! \param[in] error a reference to a pointer to a constant to const #AdobeXMPCommon::IError_base object.
	//! \param[in] ptr a const pointer to an object of className itself ( most probably this pointer ).
	//! \param[in] Func function pointer to const member function of className accepting 0 or more variables
	//! and returning void.
	//! \param[in] Vs n number of variables of varying/same type.
	//! \param[in] fileName a null terminated array of characters containing file name.
	//! \param[in] lineNumber an unsigned 32 bit number containing the line number.
	//! \note These take care of const functions.
	//!
	template < typename className >
	void CallConstUnSafeFunctionReturningVoid( pcIError_base & error, const className * const ptr,
		void ( APICALL className::*Func )() const, const char * fileName, uint32 lineNumber )
	{
		error = NULL;
		try {
			(ptr->*Func)();
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		} catch ( ... ) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError( IError_v1::kESOperationFatal, fileName, lineNumber ).get();
			err->AcquireInternal();
			error = err;
		}
	}

	template < typename className, typename ... Ts >
	void CallConstUnSafeFunctionReturningVoid( pcIError_base & error, const className * const ptr,
		void ( APICALL className::*Func )( Ts ... ) const, const char * fileName, uint32 lineNumber, Ts ... Vs )
	{
		error = NULL;
		try {
			( ptr->*Func )( Vs ... );
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		} catch ( ... ) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError( IError_v1::kESOperationFatal, fileName, lineNumber ).get();
			err->AcquireInternal();
			error = err;
		}
	}
	//!
	//! @}

	//!
	//! @{
	//! template functions taking care of all the functions where return types of safe and unsafe functions
	//! are nor void type neither shared pointers.
	//! \details It calls the underlying unsafe function and catches all kinds of exceptions and report back them
	//! through a pointer.
	//! \param[in] error a reference to a pointer to a constant to const #AdobeXMPCommon::IError_base object.
	//! \param[in] ptr non const pointer to an object of className itself ( most probably this pointer ).
	//! \param[in] dRV default value to return from the function in case of any error.
	//! \param[in] Func function pointer to non const member function of className accepting 0 or more variables
	//! and returning internalReturnType.
	//! \param[in] Vs n number of variables of varying/same type.
	//! \param[in] fileName a null terminated array of characters containing file name.
	//! \param[in] lineNumber an unsigned 32 bit number containing the line number.
	//! \return a value of returnType.
	//! \note These take care of non const functions.
	//!
	template < typename className, typename returnType, typename internalReturnType >
	returnType CallUnSafeFunction( pcIError_base & error, className * ptr, returnType dRV,
		internalReturnType( APICALL className::*Func )( ), const char * fileName, uint32 lineNumber )
	{
		error = NULL;
		try {
			internalReturnType returnValue = (ptr->*Func)();
		#if XMP_WinBuild
			#pragma warning( push )
			#pragma warning( disable : 4800 )
		#endif
			return static_cast< returnType >( returnValue );
		#if XMP_WinBuild
			#pragma warning( pop )
		#endif
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		} catch ( ... ) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError( IError_v1::kESOperationFatal, fileName, lineNumber ).get();
			err->AcquireInternal();
			error = err;
		}
		return dRV;
	}

	template < typename className, typename returnType, typename internalReturnType, typename ... Ts >
	returnType CallUnSafeFunction( pcIError_base & error, className * ptr, returnType dRV,
		internalReturnType ( APICALL className::*Func )( Ts ... ), const char * fileName, uint32 lineNumber, Ts ... Vs )
	{
		error = NULL;
		try {
			internalReturnType returnValue = (ptr->*Func)( Vs ... );
		#if XMP_WinBuild
			#pragma warning( push )
			#pragma warning( disable : 4800 )
		#endif
			return static_cast< returnType >( returnValue );
		#if XMP_WinBuild
			#pragma warning( pop )
		#endif
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		} catch ( ... ) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError( IError_v1::kESOperationFatal, fileName, lineNumber ).get();
			err->AcquireInternal();
			error = err;
		}
		return dRV;
	}

	//!
	//! @}

	//!
	//! @{
	//! template functions taking care of all the functions where return types of safe and unsafe functions
	//! are nor void type neither shared pointers.
	//! \details It calls the underlying unsafe function and catches all kinds of exceptions and report back them
	//! through a pointer.
	//! \param[in] error a reference to a pointer to a constant to const #AdobeXMPCommon::IError_base object.
	//! \param[in] ptr a const pointer to an object of className itself ( most probably this pointer ).
	//! \param[in] dRV default value to return from the function in case of any error.
	//! \param[in] Func function pointer to a const member function of className accepting 0 or more variables
	//! and returning internalReturnType.
	//! \param[in] Vs n number of variables of varying/same type.
	//! \param[in] fileName a null terminated array of characters containing file name.
	//! \param[in] lineNumber an unsigned 32 bit number containing the line number.
	//! \return a value of returnType.
	//! \note These take care of const functions.
	//!
	template < typename className, typename returnType, typename internalReturnType >
	returnType CallConstUnSafeFunction( pcIError_base & error, const className * ptr, returnType dRV,
		internalReturnType( APICALL className::*Func )( ) const, const char * fileName, uint32 lineNumber )
	{
		error = NULL;
		try {
			internalReturnType returnValue = (ptr->*Func)();
		#if XMP_WinBuild
			#pragma warning( push )
			#pragma warning( disable : 4800 )
		#endif
			return static_cast< returnType >( returnValue );
		#if XMP_WinBuild
			#pragma warning( pop )
		#endif
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		} catch ( ... ) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError( IError_v1::kESOperationFatal, fileName, lineNumber ).get();
			err->AcquireInternal();
			error = err;
		}
		return dRV;
	}

	template < typename className, typename returnType, typename internalReturnType, typename ... Ts >
	returnType CallConstUnSafeFunction( pcIError_base & error, const className * ptr, returnType dRV,
		internalReturnType ( APICALL className::*Func )( Ts ... ) const, const char * fileName, uint32 lineNumber, Ts ... Vs )
	{
		error = NULL;
		try {
			internalReturnType returnValue = (ptr->*Func)( Vs ... );
		#if XMP_WinBuild
			#pragma warning( push )
			#pragma warning( disable : 4800 )
		#endif
			return static_cast< returnType >( returnValue );
		#if XMP_WinBuild
			#pragma warning( pop )
		#endif
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		} catch ( ... ) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError( IError_v1::kESOperationFatal, fileName, lineNumber ).get();
			err->AcquireInternal();
			error = err;
		}
		return dRV;
	}

	//!
	//! @{
	//! template functions taking care of all the functions where return types of safe and unsafe functions
	//! are raw pointers and shared pointers respectively.
	//! \details It calls the underlying unsafe function and catches all kinds of exceptions and report back them
	//! through a pointer.
	//! \param[in] error a reference to a pointer to a constant to const #AdobeXMPCommon::IError_base object.
	//! \param[in] ptr non const pointer to an object of className itself ( most probably this pointer ).
	//! \param[in] Func function pointer to non const member function of className accepting 0 or more variables
	//! and returning shared pointer of type sharedPointerType.
	//! \param[in] Vs n number of variables of varying/same type.
	//! \param[in] fileName a null terminated array of characters containing file name.
	//! \param[in] lineNumber an unsigned 32 bit number containing the line number.
	//! \return a value of returnType.
	//! \note These take care of non const functions.
	//!
	template < typename className, typename returnType, typename sharedPointerType >
	returnType CallUnSafeFunctionReturningSharedPointer( pcIError_base & error, className * ptr,
		shared_ptr< sharedPointerType >( APICALL className::*Func )( ), const char * fileName, uint32 lineNumber )
	{
		error = NULL;
		try {
			shared_ptr< sharedPointerType > returnValue = ( ptr->*Func )();
			if (returnValue) {
				returnValue->GetISharedObject_I()->AcquireInternal();
				return returnValue->template GetInterfacePointer< sharedPointerType >();
			}
			return NULL;
			//return returnValue.get();
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		} catch ( ... ) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError( IError_v1::kESOperationFatal, fileName, lineNumber ).get();
			err->AcquireInternal();
			error = err;
		}
		return NULL;
	}

	template < typename className, typename returnType, typename sharedPointerType, typename ... Ts >
	returnType CallUnSafeFunctionReturningSharedPointer(pcIError_base & error, className * ptr,
		shared_ptr< sharedPointerType >(APICALL className::*Func)(Ts ...), const char * fileName, uint32 lineNumber, Ts ... Vs)
	{
		error = NULL;
		try {
			shared_ptr< sharedPointerType > returnValue = (ptr->*Func)(Vs ...);
			if (returnValue) {
				returnValue->GetISharedObject_I()->AcquireInternal();
				return returnValue->template GetInterfacePointer< sharedPointerType >();
			}
			return NULL;
			//return returnValue.get();
		}
		catch (spcIError err) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		}
		catch (...) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError(IError_v1::kESOperationFatal, fileName, lineNumber).get();
			err->AcquireInternal();
			error = err;
		}
		return NULL;
	}

	//!
	//! @{
	//! template functions taking care of all the functions where return types of safe and unsafe functions
	//! are raw pointers and shared pointers respectively.
	//! \details It calls the underlying unsafe function and catches all kinds of exceptions and report back them
	//! through a pointer.
	//! \param[in] error a reference to a pointer to a constant to const #AdobeXMPCommon::IError_base object.
	//! \param[in] ptr a const pointer to an object of className itself ( most probably this pointer ).
	//! \param[in] Func function pointer to a const member function of className accepting 0 or more variables
	//! and returning shared pointer of type sharedPointerType.
	//! \param[in] Vs n number of variables of varying/same type.
	//! \param[in] fileName a null terminated array of characters containing file name.
	//! \param[in] lineNumber an unsigned 32 bit number containing the line number.
	//! \return a value of returnType.
	//! \note These take care of const functions.
	//!
	
	template < typename className, typename returnType, typename sharedPointerType >
	returnType CallConstUnSafeFunctionReturningSharedPointer(pcIError_base & error, const className * ptr,
		shared_ptr< sharedPointerType >(APICALL className::*Func)() const, const char * fileName, uint32 lineNumber)
	{
		error = NULL;
		try {
			shared_ptr< sharedPointerType > returnValue = ( ptr->*Func )( );
			if (returnValue) {
			returnValue->GetISharedObject_I()->AcquireInternal();
			return returnValue->template GetInterfacePointer< sharedPointerType >();
			//return returnValue.get();
			}
			return NULL;
		}
		catch (spcIError err) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		}
		catch (...) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError(IError_v1::kESOperationFatal, fileName, lineNumber).get();
			err->AcquireInternal();
			error = err;
		}
		return NULL;
	}


	template < typename className, typename returnType, typename sharedPointerType, typename ... Ts >
	returnType CallConstUnSafeFunctionReturningSharedPointer( pcIError_base & error, const className * ptr,
		shared_ptr< sharedPointerType >( APICALL className::*Func )( Ts ... ) const, const char * fileName, uint32 lineNumber, Ts ... Vs )
	{
		error = NULL;
		try {
			shared_ptr< sharedPointerType > returnValue = ( ptr->*Func )( Vs ... );
			if (returnValue) {
				returnValue->GetISharedObject_I()->AcquireInternal();
				return returnValue->template GetInterfacePointer< sharedPointerType >();
			}
			return NULL;
			//return returnValue.get();
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		} catch ( ... ) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError( IError_v1::kESOperationFatal, fileName, lineNumber ).get();
			err->AcquireInternal();
			error = err;
		}
		return NULL;
	}
	//!
	//! @}

	template< typename sharedPointerType, typename rawPointerType >
	rawPointerType ReturnRawPointerFromSharedPointer( shared_ptr< sharedPointerType >( *FuncPtr )( ), pcIError_base & error, const char * fileName, uint32 lineNumber ) __NOTHROW__ {
		error = NULL;
		try {
			auto sp = FuncPtr();
			sp->GetISharedObject_I()->AcquireInternal();
			return sp.get();
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		} catch ( ... ) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError( IError_v1::kESOperationFatal, fileName, lineNumber ).get();
			err->AcquireInternal();
			error = err;
		}
		return NULL;
	}

	template< typename sharedPointerType, typename rawPointerType, typename ... Ts >
	rawPointerType ReturnRawPointerFromSharedPointer( shared_ptr< sharedPointerType >( *FuncPtr )( Ts ... ), pcIError_base & error, const char * fileName, uint32 lineNumber, Ts ... Vs ) __NOTHROW__ {
		error = NULL;
		try {
			auto sp = FuncPtr( Vs ... );
			sp->GetISharedObject_I()->AcquireInternal();
			return sp.get();
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		} catch ( ... ) {
pIError_I err = IError_I::CreateUnknownExceptionCaughtError( IError_v1::kESOperationFatal, fileName, lineNumber ).get();
			err->AcquireInternal();
			error = err;
		}
		return NULL;
	}

}

#endif // TWrapperFunctions_I_h__
