// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCommon/XMPCommonErrorCodes_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	void APICALL IError_I::AppendParameter( const spcIUTF8String & string ) __NOTHROW__ {
		AppendParameter( string->c_str(), string->size() );
	}

	uint32 APICALL IError_I::getCode( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< IError_v1, uint32, eErrorCode >(
			error, this, 0, &IError_v1::GetCode, __FILE__, __LINE__ );
	}

	uint32 APICALL IError_I::getDomain( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< IError_v1, uint32, eErrorDomain >(
			error, this, 0, &IError_v1::GetDomain, __FILE__, __LINE__ );
	}

	uint32 APICALL IError_I::getSeverity( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< IError_v1, uint32, eErrorSeverity >(
			error, this, 0, &IError_v1::GetSeverity, __FILE__, __LINE__ );
	}

	pcIUTF8String_base APICALL IError_I::getMessage( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IError_v1, pcIUTF8String_base, const IUTF8String >(
			error, this, &IError_v1::GetMessage, __FILE__, __LINE__ );
	}

	pcIUTF8String_base APICALL IError_I::getLocation( pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IError_v1, pcIUTF8String_base, const IUTF8String >(
			error, this, &IError_v1::GetLocation, __FILE__, __LINE__ );

	}

	pcIUTF8String_base APICALL IError_I::getParameter( sizet index, pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IError_v1, pcIUTF8String_base, const IUTF8String, sizet >(
			error, this, &IError_v1::GetParameter, __FILE__, __LINE__, index );

	}

	pIError_base APICALL IError_I::getNextError( pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IError_v1, pIError_base, IError >(
			error, this, &IError_v1::GetNextError, __FILE__, __LINE__ );
	}

	pIError_base APICALL IError_I::setNextError( pIError_base nextError, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IError_v1, pIError_base, IError, const spIError & >(
			error, this, &IError_v1::SetNextError, __FILE__, __LINE__, IError_v1::MakeShared( nextError ) );
	}

	pvoid APICALL IError_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL IError_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__{
		return CallUnSafeFunction< IError_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, &IError_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL IError_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kIUTF8StringID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< IError_v1 * >( this );
				break;

			case kInternalInterfaceVersionNumber:
				return this;
				break;

			default:
				throw IError_I::CreateInterfaceVersionNotAvailableError(
					IError_v1::kESOperationFatal, interfaceID, interfaceVersion, __FILE__, __LINE__ );
				break;
			}
		}
		if ( isTopLevel )
			throw IError_I::CreateInterfaceNotAvailableError(
			IError_v1::kESOperationFatal, kIErrorID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

	spIError_I IError_I::CreateUnknownExceptionCaughtError( IError_v1::eErrorSeverity severity, const char * fileName, uint32 lineNumber ) {
		auto error = CreateError( IError_v1::kEDGeneral, kGECUnknownExceptionCaught, severity );
		error->SetMessage( "Unknown Exception Caught" );
		error->SetLocation( fileName, lineNumber );
		return error;
	}

	spIError_I IError_I::CreateInterfaceVersionNotAvailableError( IError_v1::eErrorSeverity severity, uint64 interfaceID, uint32 requestedVersion, const char * fileName, uint32 lineNumber ) {
		auto error = CreateError( IError_v1::kEDGeneral, kGECVersionUnavailable, severity );
		error->SetMessage( "Requested Interface Version is not available" );
		error->SetLocation( fileName, lineNumber );
		error->AppendParameter( interfaceID );
		error->AppendParameter( requestedVersion );
		return error;
	}

	spIError_I IError_I::CreateInterfaceNotAvailableError( IError_v1::eErrorSeverity severity, uint64 interfaceID, uint64 requestedInterfaceID, const char * fileName, uint32 lineNumber ) {
		auto error = CreateError( IError_v1::kEDGeneral, kGECInterfaceUnavailable, severity );
		error->SetMessage( "Requested Interface is not available" );
		error->SetLocation( fileName, lineNumber );
		error->AppendParameter( interfaceID );
		error->AppendParameter( requestedInterfaceID );
		return error;
	}


	spIError_I IError_I::CreateClientCodeExceptionError( IError_v1::eErrorSeverity severity, const char * fileName, uint32 lineNumber ) {
		auto error = CreateError( IError_v1::kEDGeneral, kGECClientThrownExceptionCaught, severity );
		error->SetMessage( "Client Code raised an exception" );
		error->SetLocation( fileName, lineNumber );
		return error;
	}


	uint32 ReportErrorAndContinueABISafe( uint32 errorDomain, uint32 errorCode, uint32 errorSeverity, const char * message, pcIError_base & _error ) __NOTHROW__ {
		try {
			NOTIFY_ERROR( static_cast< IError::eErrorDomain >( errorDomain ), errorCode, message,
				static_cast< IError::eErrorSeverity >( errorSeverity ), false, false );
			return 1;
		} catch ( spcIError err ) {
			err->GetISharedObject_I()->AcquireInternal();
			_error = err.get();
		} catch ( ... ) {
			_error = IError_I::CreateClientCodeExceptionError( static_cast< IError::eErrorSeverity >( errorSeverity ), __FILE__, __LINE__ ).get();
			_error->GetISharedObject_I()->AcquireInternal();
		}
		return 0;
	}
}
