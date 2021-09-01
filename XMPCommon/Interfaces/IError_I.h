#ifndef IError_I_h__
#define IError_I_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCommon/Interfaces/IError.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject_I.h"
#include "XMPCommon/Interfaces/IErrorNotifier_I.h"

//! \cond NEVER_IN_DOCUMNETATION
#define PARAMETER_1( error, condition1, parameter1 )						\
	if ( condition1 ) error->AppendParameter( parameter1 );

#define PARAMETER_2( error, condition1, parameter1,							\
	condition2, parameter2 )												\
	PARAMETER_1( error, condition1, parameter1 )							\
	PARAMETER_1( error, condition2, parameter2 )

#define PARAMETER_3( error, condition1, parameter1,							\
	condition2, parameter2, condition3, parameter3 )						\
	PARAMETER_2( error, condition1, parameter1, condition2, parameter2 )	\
	PARAMETER_1( error, condition3, parameter3 )							\

#define PARAMETER_4( error, condition1, parameter1,							\
	condition2, parameter2, condition3, parameter3,							\
	condition4, parameter4 )												\
	PARAMETER_3( error, condition1, parameter1, condition2,					\
		parameter2, condition3, parameter3 )								\
	PARAMETER_1( error, condition4, parameter4 )

#define PARAMETER_5( error, condition1, parameter1,							\
	condition2, parameter2, condition3, parameter3,							\
	condition4, parameter4, condition5, parameter5 )						\
	PARAMETER_4( error, condition1, parameter1, condition2,					\
	parameter2, condition3, parameter3, condition4, parameter4 )			\
	PARAMETER_1( error, condition5, parameter5 )

#define PARAMETER_6( error, condition1, parameter1,							\
	condition2, parameter2, condition3, parameter3,							\
	condition4, parameter4, condition5, parameter5,							\
	condition6, parameter6 )												\
	PARAMETER_5( error, condition1, parameter1, condition2,					\
	parameter2, condition3, parameter3, condition4, parameter4,				\
	condition5, parameter5 )												\
	PARAMETER_1( error, condition6, parameter6 )

#define GET_MACRO( error, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, MACRO_NAME, ... ) MACRO_NAME
#define CALLAPPENDPARAMETER( ... ) _CALLAPPENDPARAMETER( GET_MACRO( __VA_ARGS__,	\
	PARAMETER_6, PARAMETER_6, PARAMETER_5, PARAMETER_5, PARAMETER_4, PARAMETER_4,	\
	PARAMETER_3, PARAMETER_3, PARAMETER_2, PARAMETER_2, PARAMETER_1, PARAMETER_1 ),	\
	__VA_ARGS__ )
#define _CALLAPPENDPARAMETER( MACRO_NAME, ... ) _CALLAPPENDPARAMETER_( MACRO_NAME( __VA_ARGS__ ) )
#define _CALLAPPENDPARAMETER_( X ) X

//! \endcond

//!
//! \brief Helper macro to create and notify warning to the client.
//! \details Creates a shared pointer of type #AdobeXMPCommon::IError, sets all the appropriate fields and
//! then notifies the client about the warning.
//! \param[in] domain an object of type #AdobeXMPCommon::IError_v1::eErrorDomain indicating the
//! domain to which this warning belongs.
//! \param[in] errorCode an object of type #AdobeXMPCommon::IError_v1::eErrorCode indicating the
//! error code within a particular domain.
//! \param[in] message pointer to a constant null terminated char buffer containing message. NULL pointer
//! will be treated as empty message string.
//! \param[in] ... variable number of pointers to constant null terminated char buffers. Each
//! entry containing a parameter to be stored along with the warning.
//!
#define NOTIFY_WARNING( domain, errorCode, message, ... ) {														\
	spIError_I warn = IError_I::CreateError( ( domain ) , ( errorCode ) , IError_base::kESWarning );			\
	warn->SetLocation( __FILE__, __LINE__ );																	\
	warn->SetMessage( ( message ) );																			\
	CALLAPPENDPARAMETER( warn, __VA_ARGS__ );																	\
	spcIError warning = warn;																					\
	if ( !IErrorNotifier_I::GetErrorNotifier()->Notify( warning ) ) {											\
		throw warning;																							\
	}																											\
}

//!
//! \brief Helper macro to create and notify warning to the client.
//! \details Creates a shared pointer of type #AdobeXMPCommon::IError, sets all the appropriate fields and
//! then notifies the client about the error.
//! \param[in] domain an object of type #AdobeXMPCommon::IError_v1::eErrorDomain indicating the
//! domain to which this error belongs.
//! \param[in] errorCode an object of type #AdobeXMPCommon::IError_v1::eErrorCode indicating the
//! error code within a particular domain.
//! \param[in] message pointer to a constant null terminated char buffer containing message. NULL
//! pointer will be treated as empty message string.
//! \param[in] severity a value of #AdobeXMPCommon::IError_v1::eErrorSeverity indicating the
//! severity of the error.
//! \param[in] ... variable number of pointers to constant null terminated char buffers. Each
//! entry containing a parameter to be stored along with the error.
//!
#define NOTIFY_ERROR( domain, errorCode, message, severity, ... ) {												\
	spIError_I err = IError_I::CreateError( ( domain ) , ( errorCode ) , severity );							\
	err->SetLocation( __FILE__, __LINE__ );																		\
	err->SetMessage( ( message ) );																				\
	CALLAPPENDPARAMETER( err, __VA_ARGS__ );																	\
	spcIError error = err;																						\
	if ( !IErrorNotifier_I::GetErrorNotifier()->Notify( error ) ) {												\
		throw error;																							\
	}																											\
	if ( severity > IError_v1::kESWarning ) {																	\
		throw error;																							\
	}																											\
}

#define CATCH_ALL() catch( ... ) {																				\
	NOTIFY_ERROR( IError_v1::kEDGeneral, IError_v1::kGECUnknownExceptionCaught,									\
		"Unknown exception caught", IError_v1::kESOperationFatal ); }
//!
//! \brief Helper macro to create and throw an error without notifying the client.
//! \details Creates a shared pointer of type #AdobeXMPCommon::IError, sets all the appropriate fields and
//! then raises an exception with this shared pointer after marking it as const.
//! \param[in] domain an object of type #AdobeXMPCommon::IError_v1::eErrorDomain indicating the
//! domain to which this error belongs.
//! \param[in] errorCode an object of type #AdobeXMPCommon::IError_v1::eErrorCode indicating the
//! error code within a particular domain.
//! \param[in] message pointer to a constant null terminated char buffer containing message. NULL
//! pointer will be treated as empty message string.
//! \param[in] severity a value of #AdobeXMPCommon::IError_v1::eErrorSeverity indicating the
//! severity of the error.
//! \param[in] ... variable number of pointers to constant null terminated char buffers. Each
//! entry containing a parameter to be stored along with the error.
//!
#define THROW_ERROR( domain, errorCode, message, severity, ... ) {											\
	spIError_I error = IError_I::CreateError( ( domain ) , ( errorCode ) , severity );						\
	error->SetLocation( __FILE__, __LINE__ );																\
	error->SetMessage( ( message ) );																		\
	CALLAPPENDPARAMETER( error, __VA_ARGS__ );																\
	spcIError cError = error;																				\
	throw cError;																							\
}


namespace XMP_COMPONENT_INT_NAMESPACE {
	
	//!
	//! \brief Internal interface that represents an error/warning encountered during processing.
	//! \details Provides all the functions to get/set required information regarding error scenario as well as
	//! creating errors.
	//! \attention Do Not support Multi-threading at object level.
	//! \attention Multi-threading not required since internally same object will not be used across multiple threads.
	//!

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif
	class IError_I
		: public virtual IError
		, public virtual ISharedObject_I
	{
	public:

		using IError_v1::AppendParameter;
		using IError_v1::GetCode;
		using IError_v1::GetDomain;
		using IError_v1::GetSeverity;
		//using IError_v1::GetMessage;
		using IError_v1::GetLocation;
		using IError_v1::GetParameter;
		using IError_v1::GetNextError;
		using IError_v1::SetNextError;

		//!
		//! Appends a parameter to the list of parameters.
		//! \param[in] parameter shared pointer to a constant an object of IUTF8String.
		//! Invalid shared pointer will be treated as empty message string.
		//!
		void APICALL AppendParameter( const spcIUTF8String & string ) __NOTHROW__;

		virtual pIError APICALL GetActualIError() __NOTHROW__ { return this; }
		virtual pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ { return this; }

		virtual pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );

		// static factory methods

		//!
		//! Creates an error object.
		//! \param[in] errDomain a value of #eErrorDomain indicating the error domain.
		//! \param[in] errCode a value of #eErrorCode indicating the error code.
		//! \param[in] errSeverity a value of #eErrorSeverity indicating the severity of the error.
		//!
		static spIError_I CreateError( eErrorDomain errDomain, eErrorCode errCode, eErrorSeverity errSeverity );


		//!
		//! Creates an error for uncaught exception.
		//! \param[in] severity a value of #IError_v1::eErrorSeverity type indicating the severity of an error.
		//! \param[in] fileName pointer to a constant null terminated char buffer containing fileName in which error
		//! originated.
		//! \param[in] lineNumber a value of #AdobeXMPCommon::uint32 type respresenting the lineNumber at which error
		//! originated.
		//! creates and returns appropriate error.
		//!
		static spIError_I CreateUnknownExceptionCaughtError( IError_v1::eErrorSeverity severity, const char * fileName, uint32 lineNumber );

		//!
		//! Creates an error for unavailable version of an interface..
		//! \param[in] severity a value of #IError_v1::eErrorSeverity type indicating the severity of an error.
		//! \param[in] interfaceID a value of uint64 type representing the interfaceID of the interface.
		//! \param[in] requestedVersion a value of uint32 type representing the version of the requested interface.
		//! \param[in] fileName pointer to a constant null terminated char buffer containing fileName in which error
		//! originated.
		//! \param[in] lineNumber a value of #AdobeXMPCommon::uint32 type respresenting the lineNumber at which error
		//! originated.
		//! creates and returns appropriate error.
		//!
		static spIError_I CreateInterfaceVersionNotAvailableError( IError_v1::eErrorSeverity severity, uint64 interfaceID, uint32 requestedVersion, const char * fileName, uint32 lineNumber );

		//!
		//! Creates an error for unavailable interface.
		//! \param[in] severity a value of #IError_v1::eErrorSeverity type indicating the severity of an error.
		//! \param[in] interfaceID a value of uint64 type representing the interfaceID of the interface.
		//! \param[in] requestedInterfaceID a value of uint64 type representing the interfaceID of the requested interface.
		//! \param[in] fileName pointer to a constant null terminated char buffer containing fileName in which error
		//! originated.
		//! \param[in] lineNumber a value of #AdobeXMPCommon::uint32 type respresenting the lineNumber at which error
		//! originated.
		//! creates and returns appropriate error.
		//!
		static spIError_I CreateInterfaceNotAvailableError( IError_v1::eErrorSeverity severity, uint64 interfaceID, uint64 requestedInterfaceID, const char * fileName, uint32 lineNumber );

		//!
		//! Creates an error for client function throwing exception.
		//! \param[in] severity a value of #IError_v1::eErrorSeverity type indicating the severity of an error.
		//! \param[in] fileName pointer to a constant null terminated char buffer containing fileName in which error
		//! originated.
		//! \param[in] lineNumber a value of #AdobeXMPCommon::uint32 type respresenting the lineNumber at which error
		//! originated.
		//! creates and returns appropriate error.
		//!
		static spIError_I CreateClientCodeExceptionError( IError_v1::eErrorSeverity severity, const char * fileName, uint32 lineNumber );

	protected:

		//!
		//! @{
		//! wrapper functions - DLL across boundary safe functions.
		//!
		virtual uint32 APICALL getCode( pcIError_base & error ) const __NOTHROW__;
		virtual uint32 APICALL getDomain( pcIError_base & error ) const __NOTHROW__;
		virtual uint32 APICALL getSeverity( pcIError_base & error ) const __NOTHROW__;
		virtual pcIUTF8String_base APICALL getMessage( pcIError_base & error ) const __NOTHROW__;
		virtual pcIUTF8String_base APICALL getLocation( pcIError_base & error ) const __NOTHROW__;
		virtual pcIUTF8String_base APICALL getParameter( sizet index, pcIError_base & error ) const __NOTHROW__;
		virtual pIError_base APICALL getNextError( pcIError_base & error ) __NOTHROW__;
		virtual pIError_base APICALL setNextError( pIError_base nextError, pcIError_base & error ) __NOTHROW__;
		virtual pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__;
		//! @}
		//!

		virtual pvoid APICALL GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel );

         virtual ~IError_I() __NOTHROW__ {}

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};

#if XMP_WinBuild
	#pragma warning( pop )
#endif

	uint32 ReportErrorAndContinueABISafe( uint32 errorDomain, uint32 errorCode, uint32 errorSeverity, const char * message, pcIError_base & error ) __NOTHROW__;
}

#endif  // IError_I_h__
