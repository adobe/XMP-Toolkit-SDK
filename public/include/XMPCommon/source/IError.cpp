// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#define FRIEND_CLASS_DECLARATION() friend class IErrorProxy;

#include "XMPCommon/Interfaces/IError.h"

#if !BUILDING_XMPCOMMON_LIB && !SOURCE_COMPILING_XMP_ALL

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCommon/Interfaces/IObjectFactory.h"

#include <assert.h>

namespace AdobeXMPCommon {
	class IErrorProxy
		: public virtual IError
	{
	private:
		pIError mRawPtr;

	public:
		IErrorProxy( pIError ptr )
			: mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~IErrorProxy() __NOTHROW__ { mRawPtr->Release(); }

		pIError APICALL GetActualIError() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction<
				IVersionable, pvoid, pvoid, uint64, uint32
			>( mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		eErrorCode APICALL GetCode() const {
			return CallConstSafeFunction< IError, eErrorCode, uint32 >(
				mRawPtr, &IError::getCode );
		}

		uint32 APICALL getCode( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getCode( error );
		}

		eErrorDomain APICALL GetDomain() const {
			return CallConstSafeFunction< IError, eErrorDomain, uint32 >(
				mRawPtr, &IError::getDomain );
		}

		uint32 APICALL getDomain( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getDomain( error );
		}

		eErrorSeverity APICALL GetSeverity() const {
			return CallConstSafeFunction< IError, eErrorSeverity, uint32 >(
				mRawPtr, &IError::getSeverity );
		}

		uint32 APICALL getSeverity( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getSeverity( error );
		}

		spcIUTF8String APICALL GetMessage() const {
			return CallConstSafeFunctionReturningPointer< IError, pcIUTF8String_base, const IUTF8String >(
				mRawPtr, &IError::getMessage );
		}

		pcIUTF8String_base APICALL getMessage( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getMessage( error );
		}

		spcIUTF8String APICALL GetLocation() const {
			return CallConstSafeFunctionReturningPointer< IError, pcIUTF8String_base, const IUTF8String >(
				mRawPtr, &IError::getLocation );
		}

		pcIUTF8String_base APICALL getLocation( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getLocation( error );
		}

		spcIUTF8String APICALL GetParameter( sizet index ) const {
			return CallConstSafeFunctionReturningPointer< IError, pcIUTF8String_base, const IUTF8String, sizet >(
				mRawPtr, &IError::getParameter, index );
		}

		pcIUTF8String_base APICALL getParameter( sizet index, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getParameter( index, error );
		}

		sizet APICALL GetParametersCount() const __NOTHROW__ {
			return mRawPtr->GetParametersCount();
		}

		spIError APICALL GetNextError() {
			pcIError_base error( NULL );
			pIError_base ptr = mRawPtr->getNextError( error );
			if ( error ) throw MakeShared( error );
			return MakeShared( ptr );
		}

		pIError_base APICALL getNextError( pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getNextError( error );
		}

		spIError APICALL SetNextError( const spIError & nextError ) {
			pcIError_base error( NULL );
			pIError_base ptr = mRawPtr->setNextError( nextError ? nextError->GetActualIError() : NULL, error );
			if ( error ) throw MakeShared( error );
			return MakeShared( ptr );
		}

		pIError_base APICALL setNextError( pIError_base nextError, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->setNextError( nextError, error );
		}

		void APICALL SetMessage( const char * message, sizet len ) __NOTHROW__ {
			mRawPtr->SetMessage( message, len );
		}

		void APICALL SetLocation( const char * fileName, sizet lineNumber ) __NOTHROW__ {
			mRawPtr->SetLocation( fileName, lineNumber );
		}

		void APICALL AppendParameter( const char * parameter, sizet len ) __NOTHROW__ {
			mRawPtr->AppendParameter( parameter, len );
		}

		void APICALL AppendParameter( void * addressParameter ) __NOTHROW__ {
			return mRawPtr->AppendParameter( addressParameter );
		}

		void APICALL AppendParameter( const uint32 & integerValue ) __NOTHROW__ {
			return mRawPtr->AppendParameter( integerValue );
		}

		void APICALL AppendParameter( const uint64 & integerValue ) __NOTHROW__ {
			return mRawPtr->AppendParameter( integerValue );
		}

		void APICALL AppendParameter( const int32 & integerValue ) __NOTHROW__ {
			return mRawPtr->AppendParameter( integerValue );
		}

		void APICALL AppendParameter( const int64 & integerValue ) __NOTHROW__ {
			return mRawPtr->AppendParameter( integerValue );
		}

		void APICALL AppendParameter( const float & floatValue ) __NOTHROW__ {
			return mRawPtr->AppendParameter( floatValue );
		}

		void APICALL AppendParameter( const double & doubleValue ) __NOTHROW__ {
			return mRawPtr->AppendParameter( doubleValue );
		}

		void APICALL AppendParameter( bool booleanValue ) __NOTHROW__ {
			return mRawPtr->AppendParameter( booleanValue );
		}

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

	};

	spIError IError_v1::CreateError( pIObjectFactory objFactory, eErrorDomain errDomain, eErrorCode errCode, eErrorSeverity errSeverity ) {
		pIError_base temp = CallSafeFunction< IObjectFactory_base, pIError_base, pIError_base, uint32, uint32, uint32 >(
			objFactory, &IObjectFactory_base::CreateError, static_cast< uint32 >( errDomain ),
			static_cast< uint32 >( errCode ), static_cast< uint32 >( errSeverity ) );
		return MakeShared( temp );
	}

	spIError IError_v1::MakeShared( pIError_base ptr ) {
		if ( !ptr ) return spIError();
		pIError p = IError::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer<IError>() : ptr;
		return shared_ptr< IError >( new IErrorProxy( p ) );
	}
}

#endif  // !BUILDING_XMPCOMMON_LIB && !SOURCE_COMPILING_XMP_ALL
