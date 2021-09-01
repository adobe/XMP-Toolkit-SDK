// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCommon/ImplHeaders/ErrorImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Utilities/UTF8String.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCommon/XMPCommonErrorCodes_I.h"
#include "XMPCommon/Interfaces/IObjectFactory.h"
#include <cstdarg>

#include "XMPCommon/Utilities/TSmartPointers_I.h"
#include <sstream>
#include <iomanip>
#define PRECISION_LIMIT 6

namespace XMP_COMPONENT_INT_NAMESPACE {

	ErrorImpl::ErrorImpl( eErrorDomain domain, eErrorCode code, eErrorSeverity severity )
		: mErrorDomain( domain )
		, mErrorCode( code )
		, mErrorSeverity( severity ) { }

	ErrorImpl::~ErrorImpl() __NOTHROW__ {
		mNextError.reset();
		mLocation.reset();
		mMessage.reset();
	}

	IError_v1::eErrorCode APICALL ErrorImpl::GetCode() const {
		return mErrorCode;
	}

	IError_v1::eErrorDomain APICALL ErrorImpl::GetDomain() const {
		return mErrorDomain;
	}

	IError_v1::eErrorSeverity APICALL ErrorImpl::GetSeverity() const {
		return mErrorSeverity;
	}

	spcIUTF8String APICALL ErrorImpl::GetMessage() const {
		return mMessage;
	}

	spcIUTF8String APICALL ErrorImpl::GetLocation() const {
		return mLocation;
	}

	spcIUTF8String APICALL ErrorImpl::GetParameter( sizet index ) const {
		if ( index < mParameters.size() ) {
			return mParameters[ index ];
		} else {
			NOTIFY_ERROR( kEDGeneral, kGECParametersNotAsExpected,
				"Parameter to IError::GetParamater() is out of range", kESOperationFatal,
				true, static_cast< sizet >( mParameters.size() ),
				true, index );
			return spcIUTF8String_I();
		}
	}

	sizet APICALL ErrorImpl::GetParametersCount() const __NOTHROW__ {
		return mParameters.size();
	}

	spIError APICALL ErrorImpl::GetNextError() {
		return mNextError;
	}

	spIError APICALL ErrorImpl::SetNextError( const spIError & error ) {
		auto returnValue = mNextError;
		mNextError = error;
		return returnValue;
	}

	void APICALL ErrorImpl::SetMessage( const char * message, sizet len /*= npos */ ) __NOTHROW__ {
		if ( mMessage )
			mMessage->assign( message, len );
		else
			mMessage = IUTF8String_I::CreateUTF8String( message, len );
	}

	void APICALL ErrorImpl::SetLocation( const char * fileName, sizet lineNumber ) __NOTHROW__ {
		if ( mLocation )
			mLocation->assign( fileName, npos );
		else
			mLocation = IUTF8String_I::CreateUTF8String( fileName, npos );
		mLocation->append(":", 1 );
		UTF8StringStream ss;
		ss<<lineNumber;
		mLocation->append( ss.str().c_str(), ss.str().size() );
	}

	void APICALL ErrorImpl::AppendParameter( const char * parameter, sizet len /*= npos */ ) __NOTHROW__ {
		mParameters.push_back( IUTF8String_I::CreateUTF8String( parameter, len ) );
	}

	void APICALL ErrorImpl::AppendParameter( void * addressParameter ) __NOTHROW__ {
		UTF8StringStream stm;
		stm.setf( std::ios::hex );
        bool checkDone( false );
		bool appends0x( false );

		if ( !checkDone ) {
			UTF8StringStream stm1;
			stm1.setf( std::ios::hex );
			stm1 << "0x" << addressParameter;
			if ( stm1.str().size() > 4 ) {
				const char * charPtr = stm1.str().c_str();
				if ( charPtr[ 2 ] == '0' && ( charPtr[ 3 ] == 'x' || charPtr[ 3 ] == 'X' ) ) {
					appends0x = true;
				}
				checkDone = true;
			}
		}

		if ( appends0x ) stm << addressParameter;
		else stm << "0x" << addressParameter;
		AppendParameter( stm.str().c_str() );
	}

	template< typename numericType >
	void TAppendParameter( ErrorImpl * ptr, const numericType & numericValue ) {
        std::ostringstream oss;
        oss << numericValue;
        
		std::string str = oss.str();
		ptr->AppendParameter( str.c_str(), str.size() );
	}

	template<>
	void TAppendParameter(ErrorImpl * ptr, const double & numericValue) {
		std::ostringstream oss;
		oss << std::fixed<<std::setprecision(PRECISION_LIMIT)<<numericValue;

		std::string str = oss.str();
		ptr->AppendParameter(str.c_str(), str.size());
	}

	template<>
	void TAppendParameter(ErrorImpl * ptr, const float & numericValue) {
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(PRECISION_LIMIT) << numericValue;

		std::string str = oss.str();
		ptr->AppendParameter(str.c_str(), str.size());
	}

	void APICALL ErrorImpl::AppendParameter( const uint32 & integerValue ) __NOTHROW__ {
		TAppendParameter( this, integerValue );
	}

	void APICALL ErrorImpl::AppendParameter( const uint64 & integerValue ) __NOTHROW__ {
		TAppendParameter( this, integerValue );
	}

	void APICALL ErrorImpl::AppendParameter( const int32 & integerValue ) __NOTHROW__ {
		TAppendParameter( this, integerValue );
	}

	void APICALL ErrorImpl::AppendParameter( const int64 & integerValue ) __NOTHROW__ {
		TAppendParameter( this, integerValue );
	}

	void APICALL ErrorImpl::AppendParameter( const float & floatValue ) __NOTHROW__ {
		TAppendParameter( this, floatValue );
	}

	void APICALL ErrorImpl::AppendParameter( const double & doubleValue ) __NOTHROW__ {
		TAppendParameter( this, doubleValue );
	}

	void APICALL ErrorImpl::AppendParameter( bool booleanValue ) __NOTHROW__ {
		if ( booleanValue )
			AppendParameter( "true", 4 );
		else
			AppendParameter( "false", 5 );
	}

	spIError_I IError_I::CreateError( eErrorDomain errDomain, eErrorCode errCode, eErrorSeverity errSeverity ) {
		return MakeUncheckedSharedPointer( new ErrorImpl( errDomain, errCode, errSeverity ), __FILE__, __LINE__, true );
	}
}

#if BUILDING_XMPCOMMON_LIB || SOURCE_COMPILING_XMP_ALL
namespace AdobeXMPCommon {
	using namespace XMP_COMPONENT_INT_NAMESPACE;

	spIError IError_v1::CreateError( pIObjectFactory objFactory, eErrorDomain errDomain, eErrorCode errCode, eErrorSeverity errSeverity ) {
		pcIError error( NULL );
		auto retValue = MakeShared( objFactory->CreateError( static_cast< uint32 >( errDomain ), 
			static_cast< uint32 >( errCode ), static_cast< uint32 >( errSeverity ), error ) );
		//if ( error ) throw error;
		return retValue;
	}

	spIError IError_v1::MakeShared( pIError_base ptr ) {
		if ( !ptr ) return spIError();
		pIError p = IError::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IError >() : ptr;
		return MakeUncheckedSharedPointer( p, __FILE__, __LINE__, false );
	}

}
#endif  // BUILDING_XMPCOMMON_LIB || SOURCE_COMPILING_XMP_ALL
