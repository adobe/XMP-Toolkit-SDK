//! @file IError.h
#ifndef __IError_h__
#define __IError_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// ================================================================================================

#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"

#include "XMPCommon/XMPCommonFwdDeclarations.h"

namespace AdobeXMPCommon {

	//!
	//!  @brief Version1 of the interface that represents an error/warning encountered during processing.
	//! \details Provides all the functions to get required information regarding error scenario.
	//! \attention Do Not support Multi-threading at object level.
	//! \attention Multi-threading not required since clients will only be provided const objects.
	//!
	class XMP_PUBLIC IError_v1
		: public virtual ISharedObject
		, public virtual IVersionable
	{
	public:
		//!
		//! @brief Indicates various types of errors.
		//!
		typedef enum {
			//! No severity, not to be used.
			kESNone				= 0,

			//! Recovery is possible, client can choose to ignore and let library continue with the best possible way.
			kESWarning			= 1 << 0,

			//! Recovery is not possible, an exception of type pcIError_base will be thrown aborting the API call.
			kESOperationFatal	= 1 << 1,

			//! Recovery is not possible, an exception of type pcIError_base will be thrown, client should abort the process.
			kESProcessFatal		= 1 << 2,

			// Add new severities here

			//! Maximum value this enum can hold, should be treated as invalid value.
			kESMaxValue			= 1 << 31,

			//! Union of all severities
			kESAll				= kAllBits,
		} eErrorSeverity;


		//!
		//! @brief Indicates various types of error domains.
		//!
		typedef enum {
			//! No Domain
			kEDNone							= 0,

			//! Indicates error related to general conditions.
			kEDGeneral						= 1,

			//! Indicates error related to memory allocation-deallocation conditions.
			kEDMemoryManagement				= 2,

			//! Indicates error related to configurable APIs.
			kEDConfigurable					= 3,

			//! Indicates error releated to multithreading.
			kEDMultiThreading				= 4,

			//! Indicates error related to XMP Data Model Management.
			kEDDataModel					= 100,

			//! Indicates error related to XMP Parsing.
			kEDParser						= 101,

			//! Indicates error related to XMP Serializing.
			kEDSerializer					= 102,


			//! Indicates error related to dealing with XMP in various file formats.
			kEDXMPFiles						= 200,


			//! Indicates error related to Conflict Identification.
			kEDConflictIdentification		= 400,

			//! Indicates error related to Conflict Resolution.
			kEDConflictResolution			= 500,

			//! Indicates error related to 3 Way Merge.
			kEDThreeWayMerge				= 600,

			//! Indicates error related to Generic Strategy Database
			kEDGenericStrategyDatabase		= 601,

			//! Indicates error related to Asset Management.
			kEDAssetManagement				= 10000,

			//! Maximum value this enum can hold, should be treated as invalid value.
			kEDMaxValue						= kMaxEnumValue
		} eErrorDomain;

		typedef uint32 eErrorCode;

		//!
		//! @brief Get the error code.
		//! \return An object of type #eErrorCode indicating the error code.
		//!
		virtual eErrorCode APICALL GetCode() const = 0;

		//!
		//! @brief Get the error domain.
		//! \return An object of type #eErrorDomain indicating the error domain.
		//!
		virtual eErrorDomain APICALL GetDomain() const = 0;

		//!
		//! @brief Get the error severity.
		//! \return An object of type #eErrorSeverity indicating the severity of error.
		//!
		virtual eErrorSeverity APICALL GetSeverity() const = 0;

		//!
		//! @brief Get the error message.
		//! \details Error message contains a descriptive string, for debugging use only. It must not be shown to users
		//! in a final product. It is written for developers, not users, and never localized.
		//! \return A shared pointer to const AdobeXMPCommon::IUTF8String object containing message string.
		//!
		virtual spcIUTF8String APICALL GetMessage() const = 0;

		//!
		//! @brief Get the location of the error origin.
		//! \return A shared pointer to const AdobeXMPCommon::IUTF8String object containing location as like file name
		//!  and line number.
		//! \note For debugging use only.
		//!
		virtual spcIUTF8String APICALL GetLocation() const = 0;

		//!
		//! @brief Get the value of a parameter at a particular index.
		//! \details Based on each error condition various parameters are stored along with the error object. Clients can 
		//! one by one get access to each parameter that can be later used for debugging.
		//! \param[in] index A value of \#AdobeXMPCommon::sizet indicating the index of the parameter client is
		//!  interested in retrieving.
		//! \return A shared pointer to const \#AdobeXMPCommon::IUTF8String object containing some string.
		//! \attention Throws \#AdobeXMPCommon::pcIError_base in case index is out of bounds.
		//! \note For debugging use only.
		//!
		virtual spcIUTF8String APICALL GetParameter( sizet index ) const = 0;

		//!
		//! @brief Get the count of parameters.
		//! \return An object of type \#AdobeXMPCommon::sizet containing the count of paramaters associated with the error object.
		//!
		virtual sizet APICALL GetParametersCount() const __NOTHROW__ = 0;

		//!
		//! @{
		//! @brief Get the next error in the chain.
		//! \return A pointer to const/non-const \#AdobeXMPCommon::IError object which is the next error in the chain.
		//! \note Return an invalid shared pointer in case it is the last error object in the chain.
		//!
		virtual spIError APICALL GetNextError() = 0;

		XMP_PRIVATE spcIError GetNextError() const {
			return const_cast< IError_v1 * >( this )->GetNextError();
		}
		//! @}

		//!
		//! @brief Set the next error in the chain.
		//! \param[in] error A pointer to \#AdobeXMP::IError object which will be the next error in the chain.
		//! \return A pointer to \#AdobeXMPCommon::IError object which is the current next error in the chain.
		//! \note Return an invalid pointer in case there is no current next error in the chain.
		//!
		virtual spIError APICALL SetNextError( const spIError & error ) = 0;

		//!
		//! @brief Set the error message.
		//! \param[in] message Pointer to a constant char buffer containing message. It can be null terminated or not.
		//!                    NULL pointer will be treated as empty message string.
		//! \param[in] len A value of \#AdobeXMPCommon::sizet indicating the length in case message is not null
		//!                terminated. In case message is null terminated it can be set to its default value ( npos ).
		//!
		virtual void APICALL SetMessage( const char * message, sizet len = npos ) __NOTHROW__  = 0;

		//!
		//! @brief Set the location of origin of error.
		//! \param[in] fileName Pointer to a null terminated char buffer containing the file name from which the error
		//! originated. NULL pointer will be treated as empty fileName.
		//! \param[in] lineNumber A value of \#AdobeXMPCommon::sizet indicating the line in source file from which the error
		//! originated.
		//!
		virtual void APICALL SetLocation( const char * fileName, sizet lineNumber ) __NOTHROW__ = 0;

		//!
		//! @brief Appends a parameter to the list of parameters.
		//! \param[in] parameter Pointer to a constant char buffer containing parameter. It can be null terminated or not.
		//!  NULL pointer will be treated as empty message string.
		//! \param[in] len A value of AdobeXMPCommon::sizet indicating the length in case parameter is not null
		//!  terminated. In case parameter is null terminated it can be set to its default value ( npos ).
		//!
		virtual void APICALL AppendParameter( const char * parameter, sizet len = npos ) __NOTHROW__ = 0;

		//!
		//! @brief Appends an address as a string to the list of parameters.
		//! \param[in] addressParameter A value of void * type containing the address of the location to be used as parameter.
		//!
		virtual void APICALL AppendParameter( void * addressParameter ) __NOTHROW__ = 0;

		//!
		//! @brief Appends a 32 bit unsigned integer value as a string to the list of parameters.
		//! \param[in] integerValue A value of AdobeXMPCommon::uint32 type containing the integral value to be used as parameter.
		//!
		virtual void APICALL AppendParameter( const uint32 & integerValue ) __NOTHROW__ = 0;

		//!
		//! @brief Appends a 64 bit unsigned integer value as a string to the list of parameters.
		//! \param[in] integerValue A value of AdobeXMPCommon::uint64 type containing the integral value to be used as parameter.
		//!
		virtual void APICALL AppendParameter( const uint64 & integerValue ) __NOTHROW__ = 0;

		//!
		//! @brief Appends a 32 bit integer value as a string to the list of parameters.
		//! \param[in] integerValue A value of AdobeXMPCommon::int32 type containing the integral value to be used as parameter.
		//!
		virtual void APICALL AppendParameter( const int32 & integerValue ) __NOTHROW__ = 0;

		//!
		//! @brief Appends a 64 bit integer value as a string to the list of parameters.
		//! \param[in] integerValue A value of AdobeXMPCommon::uint64 type containing the integral value to be used as parameter.
		//!
		virtual void APICALL AppendParameter( const int64 & integerValue ) __NOTHROW__ = 0;

		//!
		//! @brief Appends a floating value as a string to the list of parameters.
		//! \param[in] floatValue A value of float type containing the floating value to be used as parameter.
		//!
		virtual void APICALL AppendParameter( const float & floatValue ) __NOTHROW__ = 0;

		//!
		//! @brief Appends a double floating value as a string to the list of parameters.
		//! \param[in] doubleValue A value of double type containing the floating value to be used as parameter.
		//!
		virtual void APICALL AppendParameter( const double & doubleValue ) __NOTHROW__ = 0;

		//!
		//! @brief Appends a boolean value as a string to the list of parameters.
		//! \param[in] booleanValue A value of bool type containing the boolean value to be used as parameter.
		//!
		virtual void APICALL AppendParameter( bool booleanValue ) __NOTHROW__ = 0;

		//!
		//! @brief Creates an error object.
		//! \param[in] objFactory A pointer to IObjectFactory object.
		//! \param[in] errDomain A value of #eErrorDomain indicating the error domain.
		//! \param[in] errCode A value of #eErrorCode indicating the error code.
		//! \param[in] errSeverity A value of #eErrorSeverity indicating the severity of the error.
		//! \return A shared pointer to an object of IError_v1.
		//!
		static spIError CreateError( pIObjectFactory objFactory, eErrorDomain errDomain,
			eErrorCode errCode, eErrorSeverity errSeverity );

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		//! @{
		//! @brief Return the actual raw pointer from the pointer available to client, which can be of a proxy class.
		//! \return Either a const or non const pointer to IError interface.
		//!
		virtual pIError APICALL GetActualIError() __NOTHROW__ = 0;
		XMP_PRIVATE pcIError GetActualIError() const __NOTHROW__ {
			return const_cast< IError_v1 * >( this )->GetActualIError();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Convert raw pointer to shared pointer.
		//! @details The raw pointer is of version 1 interface where as the returned shared pointer depends on the version client who is interested in.
		//! 
		//! \return Shared pointer to const or non constant IError interface.
		//!
		XMP_PRIVATE static spIError MakeShared( pIError_base ptr );
		XMP_PRIVATE static spcIError MakeShared( pcIError_base ptr ) {
			return MakeShared( const_cast< pIError_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief Returns the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIErrorID; }

		//!
		//! @brief Returns the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

	protected:
		//!
		//! Destructor
		//! 
		virtual ~IError_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual uint32 APICALL getCode( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual uint32 APICALL getDomain( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual uint32 APICALL getSeverity( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pcIUTF8String_base APICALL getMessage( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pcIUTF8String_base APICALL getLocation( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pcIUTF8String_base APICALL getParameter( sizet index, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pIError_base APICALL getNextError( pcIError_base & error ) __NOTHROW__ = 0;
		virtual pIError_base APICALL setNextError( pIError_base nextError, pcIError_base & error ) __NOTHROW__ = 0;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond
	};

	//!
	//! @brief A function pointer to report back errors and warnings to the library encountered during the serialization operation.
	//! @details Based on the error condition library can return 0 or non zero to indicate that a particular warning can be ignored
	//!          and operation can continue.
	//! \param[in] errorDomain An unsigned 32 bit integer indicating the domain of the error.
	//! \param[in] errorCode An unsigned 32 bit integer indicating the code of the error.
	//! \param[in] errorSeverity An unsigned 32 bit integer indicating the severity of the error.
	//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
	//! \return non zero value indicating that process can continue ignoring the warning, otherwise return 0 to indicate it should stop immediately.
	//!
	typedef uint32( *ReportErrorAndContinueABISafeProc )( uint32 errorDomain, uint32 errorCode, uint32 errorSeverity, const char * message, pcIError_base & error );

	//!
	//! @brief A Function object used by the client to report back and warnings to the library encountered during the serialization operation.
	//! @details Based on the error condition library can return 0 or non zero to indicate that a particular warning can be ignored
	//!          and operation can continue.
	//!
	class ReportErrorAndContinueFunctor {
	public:
		ReportErrorAndContinueFunctor( ReportErrorAndContinueABISafeProc safeProc )
			: mSafeProc( safeProc ) {}

		//!
		//! \param[in] errorDomain A value of \#IError_v1::eErrorDomain indicating the domain of the error.
		//! \param[in] errorCode A value of \#IError_v1::eErrorCode indicating the code of the error.
		//! \param[in] errorSeverity A value of \#IError_v1::eErrorSeverity indicating the severity of the error.
		//! \param[in] message Pointer to a constant char buffer containing message. 
		//! \return true value indicating that process can continue ignoring the warning, otherwise return false to indicate it should stop immediately.
		//!
		bool operator()( IError_v1::eErrorDomain errorDomain, IError_v1::eErrorCode errorCode, IError_v1::eErrorSeverity errorSeverity, const char * message ) {
			pcIError_base error( NULL );
			auto retValue = mSafeProc( static_cast< uint32 >( errorDomain ), static_cast< uint32 >( errorCode ), static_cast< uint32 >( errorSeverity ), message, error );
			if ( error )
				throw IError_v1::MakeShared( error );
			return retValue != 0 ? true : false;
		}

	protected:
		ReportErrorAndContinueABISafeProc		mSafeProc;
	};


}

#endif  // __IError_h__
