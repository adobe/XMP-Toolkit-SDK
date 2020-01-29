#ifndef TWrapperFunctions2_I_h__
#define TWrapperFunctions2_I_h__ 1

#include "XMPCommon/XMPCommonDefines_I.h"

#if XMP_UNIXBuild
#include "XMPCommon/XMPCommonDefines_I.h"
#include "XMPCommon/Interfaces/IError_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {
	using namespace AdobeXMPCommon;
	using namespace AdobeXMPCommon_Int;
	
	

	//!
	//! template functions taking care of all the functions where return types of safe and unsafe functions
	//! are void and number of arguments for public unsafe function is 3.
	//! \details It calls the underlying unsafe function and catches all kinds of exceptions and report back them
	//! through a pointer.
	//! \param[in] error a reference to a pointer to a constant to const #NS_XMPCOMMON::IError_base object.
	//! \param[in] ptr non const pointer to an object of className itself ( most probably this pointer ).
	//! \param[in] Func function pointer to non const member function of className accepting values of
	//! type arg1Type, arg2Type and arg3Type and returning void.
	//! \param[in] arg1Value a value of type arg1Type.
	//! \param[in] arg2Value a value of type arg2Type.
	//! \param[in] arg3Value a value of type arg3Type.
	//! \note These take care of non const functions.
	//!
	template < typename className, typename arg1Type, typename arg2Type, typename arg3Type >
	void CallUnSafeFunctionReturningVoid( pcIError_base & error, className * ptr,
		void (className::*Func)( arg1Type, arg2Type, arg3Type ), const char * fileName, uint32 lineNumber, arg1Type arg1Value, arg2Type arg2Value, arg3Type arg3Value ) {

		error = NULL;
		try {
			(ptr->*Func)( arg1Value, arg2Value, arg3Value );
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->GetISharedObject_I()->AcquireInternal();
		} catch ( ... ) {
			pIError_I err = IError_I::CreateUnknownExceptionCaughtError( IError_v1::kESOperationFatal, fileName, lineNumber ).get();
			err->AcquireInternal();
			error = err;
		}
	}

	template < typename className, typename returnType, typename internalReturnType, typename arg1Type, typename arg2Type >
	returnType CallUnSafeFunction( pcIError_base & error, className * ptr, returnType dRV,
		internalReturnType (className::*Func)( arg1Type, arg2Type ),  const char * fileName, uint32 lineNumber , arg1Type arg1Value, arg2Type arg2Value )
	{
		error = NULL;
		try {
		internalReturnType returnValue = (ptr->*Func)( arg1Value, arg2Value );
		
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

	template < typename className, typename returnType, typename internalReturnType, typename arg1Type, typename arg2Type, typename arg3Type >
	returnType CallConstUnSafeFunction( pcIError_base & error, const className * const ptr, returnType dRV,
		internalReturnType (className::*Func)( arg1Type, arg2Type, arg3Type ) const, const char * fileName, uint32 lineNumber, arg1Type arg1Value, arg2Type arg2Value, arg3Type arg3Value )
	 {
		error = NULL;
		try {
			internalReturnType returnValue = (ptr->*Func)( arg1Value, arg2Value, arg3Value );
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
	//! template functions taking care of all the functions where return types of safe and unsafe functions
	//! are raw pointers and shared pointers respectively and number of arguments for public unsafe function is 3.
	//! \details It calls the underlying unsafe function and catches all kinds of exceptions and report back them
	//! through a pointer.
	//! \param[in] error a reference to a pointer to a constant to const #NS_XMPCOMMON::IError_base object.
	//! \param[in] ptr a pointer to an object of className itself ( most probably this pointer ).
	//! \param[in] Func function pointer to non const member function of className accepting values of
	//! type arg1Type ,arg2Type and arg3Type and returning shared pointer of type sharedPointerType.
	//! \param[in] arg1Value a value of type arg1Type.
	//! \param[in] arg2Value a value of type arg2Type.
	//! \param[in] arg3Value a value of type arg3Type.
	//! \note These take care of non const functions.
	//!
	template < typename className, typename returnType, typename sharedPointerType, typename arg1Type, typename arg2Type, typename arg3Type >
	returnType CallUnSafeFunctionReturningSharedPointer( pcIError_base & error, className * ptr,
		shared_ptr< sharedPointerType > (className::*Func)( arg1Type, arg2Type, arg3Type ), const char * fileName, uint32 lineNumber,arg1Type arg1Value, arg2Type arg2Value, arg3Type arg3Value )
	{
		error = NULL;
		try {
			shared_ptr< sharedPointerType > returnValue = (ptr->*Func)( arg1Value, arg2Value, arg3Value );
			if ( returnValue ) {
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
	//! template functions taking care of all the functions where return types of safe and unsafe functions
	//! are raw pointers and shared pointers respectively and number of arguments for public unsafe function is 4.
	//! \details It calls the underlying unsafe function and catches all kinds of exceptions and report back them
	//! through a pointer.
	//! \param[in] error a reference to a pointer to a constant to const #NS_XMPCOMMON::IError_base object.
	//! \param[in] ptr a pointer to an object of className itself ( most probably this pointer ).
	//! \param[in] Func function pointer to non const member function of className accepting values of
	//! type arg1Type, arg2Type, arg3Type and arg4Type and returning shared pointer of type sharedPointerType.
	//! \param[in] arg1Value a value of type arg1Type.
	//! \param[in] arg2Value a value of type arg2Type.
	//! \param[in] arg3Value a value of type arg3Type.
	//! \param[in] arg4Value a value of type arg4Type.
	//! \note These take care of non const functions.
	//!
	template < typename className, typename returnType, typename sharedPointerType, typename arg1Type, typename arg2Type,
		typename arg3Type, typename arg4Type >
	returnType CallUnSafeFunctionReturningSharedPointer( pcIError_base & error, className * ptr,
		shared_ptr< sharedPointerType > (className::*Func)( arg1Type, arg2Type, arg3Type, arg4Type ), const char * fileName, uint32 lineNumber, arg1Type arg1Value, arg2Type arg2Value, arg3Type arg3Value, arg4Type arg4Value )
	{
		error = NULL;
		try {
			shared_ptr< sharedPointerType > returnValue = (ptr->*Func)( arg1Value, arg2Value, arg3Value, arg4Value );
			if ( returnValue ) {
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
	//! template functions taking care of all the functions where return types of safe and unsafe functions
	//! are raw pointers and shared pointers respectively and number of arguments for public unsafe function is 4.
	//! \details It calls the underlying unsafe function and catches all kinds of exceptions and report back them
	//! through a pointer.
	//! \param[in] error a reference to a pointer to a constant to const #NS_XMPCOMMON::IError_base object.
	//! \param[in] ptr a pointer to an object of className itself ( most probably this pointer ).
	//! \param[in] Func function pointer to non const member function of className accepting values of
	//! type arg1Type, arg2Type, arg3Type and arg4Type and returning shared pointer of type sharedPointerType.
	//! \param[in] arg1Value a value of type arg1Type.
	//! \param[in] arg2Value a value of type arg2Type.
	//! \param[in] arg3Value a value of type arg3Type.
	//! \param[in] arg4Value a value of type arg4Type.
	//! \param[in] arg4Value a value of type arg5Type.
	//! \note These take care of non const functions.
	//!
	template < typename className, typename returnType, typename sharedPointerType, typename arg1Type, typename arg2Type,
		typename arg3Type, typename arg4Type, typename arg5Type >
		returnType CallUnSafeFunctionReturningSharedPointer(pcIError_base & error, className * ptr,
		shared_ptr< sharedPointerType >(className::*Func)(arg1Type, arg2Type, arg3Type, arg4Type, arg5Type), const char * fileName, uint32 lineNumber, arg1Type arg1Value, arg2Type arg2Value, arg3Type arg3Value, arg4Type arg4Value, arg5Type arg5Value)
	{
		error = NULL;
		try {
			shared_ptr< sharedPointerType > returnValue = (ptr->*Func)(arg1Value, arg2Value, arg3Value, arg4Value, arg5Value);
			if (returnValue) {
				returnValue->GetISharedObject_I()->AcquireInternal();
				return returnValue->template GetInterfacePointer< sharedPointerType >();
			}
			return NULL;
			//return returnValue.get();
		}catch ( spcIError err ) {
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

#endif 
#endif // TWrapperFunctions2_I_h__
