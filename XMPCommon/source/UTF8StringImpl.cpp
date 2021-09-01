// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include <algorithm>

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCommon/ImplHeaders/UTF8StringImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCommon/XMPCommonErrorCodes_I.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"
#include "XMPCommon/Interfaces/IObjectFactory.h"

namespace XMP_COMPONENT_INT_NAMESPACE {
	 static sizet ValidateSrcPosParameter( const spcIUTF8String & src, const sizet & srcPos ) {
		sizet srcSize = src->size();
		if ( srcPos > srcSize ) {
			NOTIFY_ERROR( IError_v1::kEDGeneral, kGECIndexOutOfBounds,
				"srcPos is greater than length of source", IError_v1::kESOperationFatal,
				true, srcPos, true, srcSize );
		}
		return srcSize;
	}

	spIUTF8String APICALL UTF8StringImpl::append( const char * buf, sizet count ) {
		if ( buf && count > 0 ) {
			if ( count == npos ) // assume null terminated string
				mString.append( buf );
			else
				mString.append( buf, count );
		}
		return returnSelfSharedPointer();
	}

	spIUTF8String APICALL UTF8StringImpl::append( const spcIUTF8String & src, sizet srcPos, sizet count ) {
		if ( src && count > 0 ) {
			sizet srcSize = ValidateSrcPosParameter( src, srcPos );
			mString.append( src->c_str() + srcPos, std::min( count, srcSize - srcPos ) );
		}
		return returnSelfSharedPointer();
	}

	spIUTF8String APICALL UTF8StringImpl::assign( const char * buf, sizet count ) {
		if ( buf && count > 0 ) {
			if ( count == npos ) // assume null terminated string
				mString.assign( buf );
			else
				mString.assign( buf, count );
		} else {
			mString.clear();
		}
		return returnSelfSharedPointer();
	}

	spIUTF8String APICALL UTF8StringImpl::assign( const spcIUTF8String & src, sizet srcPos, sizet count ) {
		if ( src && count > 0 ) {
			sizet srcSize = ValidateSrcPosParameter( src, srcPos );
			mString.assign( src->c_str() + srcPos, std::min( count, srcSize - srcPos ) );
		} else {
			mString.clear();
		}
		return returnSelfSharedPointer();
	}

	spIUTF8String APICALL UTF8StringImpl::insert( sizet pos, const char * buf, sizet count ) {
		if ( buf && count > 0 ) {
			sizet size = ValidatePosParameter( pos );
			if ( count == npos )
				mString.insert( pos, buf );
			else
				mString.insert( pos, buf, count );
		}
		return returnSelfSharedPointer();
	}

	spIUTF8String APICALL UTF8StringImpl::insert( sizet pos, const spcIUTF8String & src, sizet srcPos, sizet count ) {
		if ( src && count > 0 ) {
			sizet size = ValidatePosParameter( pos );
			sizet srcSize = ValidateSrcPosParameter( src, srcPos );
			mString.insert( pos, src->c_str() + srcPos, std::min( count, srcSize - srcPos ) );
		}
		return returnSelfSharedPointer();
	}

	spIUTF8String APICALL UTF8StringImpl::erase( sizet pos, sizet count ) {
		if ( count > 0 ) {
			sizet size = ValidatePosParameter( pos );
			mString.erase( pos, count );
		}
		return returnSelfSharedPointer();
	}

	void APICALL UTF8StringImpl::resize( sizet n ) {
		if ( n >= mString.max_size() ) {
			NOTIFY_ERROR( IError_v1::kEDGeneral, kGECParametersNotAsExpected,
				"n is equal or greater than max_size", IError_v1::kESOperationFatal,
				true, n, true, mString.max_size() );
		}
		try {
			mString.resize( n );
		} catch( ... ) {
			NOTIFY_ERROR( IError_v1::kEDMemoryManagement, kMMECAllocationFailure, 
				"failed to allocate required memory", IError_v1::kESOperationFatal, false, false );
		}
	}

	spIUTF8String APICALL UTF8StringImpl::replace( sizet pos, sizet count, const char * buf, sizet srcCount ) {
		if ( buf && count > 0 && srcCount > 0 ) {
			sizet size = ValidatePosParameter( pos );
			if ( srcCount == npos )
				mString.replace( pos, count, buf );
			else
				mString.replace( pos, count, buf, srcCount );
		}
		return returnSelfSharedPointer();
	}

	spIUTF8String APICALL UTF8StringImpl::replace( sizet pos, sizet count, const spcIUTF8String & src, sizet srcPos, sizet srcCount ) {
		if ( src && count > 0 && srcCount > 0 ) {
			sizet size = ValidatePosParameter( pos );
			sizet srcSize = ValidateSrcPosParameter( src, srcPos );
			mString.replace( pos, count, src->c_str() + srcPos, std::min( srcCount, srcSize - srcPos ) );
		}
		return returnSelfSharedPointer();
	}

	sizet APICALL UTF8StringImpl::copy( char * buf, sizet len, sizet pos ) const {
		if ( buf && len > 0 ) {
			ValidatePosParameter( pos );
			return mString.copy( buf, len, pos );
		}
		return 0;
	}

	sizet APICALL UTF8StringImpl::find( const char * buf, sizet pos, sizet count ) const {
		if ( buf && count > 0 ) {
			return mString.find( buf, pos, count );
		}
		return npos;
	}

	sizet APICALL UTF8StringImpl::find( const spcIUTF8String & src, sizet pos, sizet count ) const {
		if ( src && src->size() > 0 && count > 0 ) {
			return mString.find( src->c_str(), pos, count );
		}
		return npos;
	}

	sizet APICALL UTF8StringImpl::rfind( const char * buf, sizet pos, sizet count ) const {
		if ( buf && count > 0 ) {
			return mString.rfind( buf, pos, count );
		}
		return npos;
	}

	sizet APICALL UTF8StringImpl::rfind( const spcIUTF8String & src, sizet pos, sizet count ) const {
		if ( src && src->size() > 0 && count > 0 ) {
			return mString.rfind( src->c_str(), pos, count );
		}
		return npos;
	}

	int32 APICALL UTF8StringImpl::compare( sizet pos, sizet len, const char * buf, sizet count ) const {
		if ( buf && (len > 0||len ==0) && (count > 0 || count==0) ) {
			sizet size = ValidatePosParameter( pos );
			if ( count == npos )
				return mString.compare( pos, len, buf );
			else
				return mString.compare( pos, len, buf, count );
		}
		return -1;
	}

	int32 APICALL UTF8StringImpl::compare( sizet pos, sizet len, const spcIUTF8String & str, sizet strPos, sizet strLen ) const {
		if ( str && (len > 0||len ==0) && (strLen > 0||strLen ==0) ) {
			ValidatePosParameter( pos );
			ValidateSrcPosParameter( str, strPos );
			return mString.compare( pos, len, str->c_str() + strPos, strLen );
		}
		return -1;
	}

	spIUTF8String APICALL UTF8StringImpl::substr( sizet pos, sizet count ) const {
		ValidatePosParameter( pos );
		try {
			return CreateUTF8String( this->c_str() + pos, std::min( count, this->size() - pos ) );
		} catch ( ... ) {
			NOTIFY_ERROR( IError_v1::kEDMemoryManagement, kMMECAllocationFailure,
				"failed to allocate required memory", IError_v1::kESOperationFatal, false, false );
		}
		return spIUTF8String();
	}

	bool APICALL UTF8StringImpl::empty() const {
		return mString.empty();
	}

	const char * APICALL UTF8StringImpl::c_str() const __NOTHROW__ {
		return mString.c_str();
	}

	void APICALL UTF8StringImpl::clear() __NOTHROW__ {
		mString.clear();
	}

	sizet APICALL UTF8StringImpl::size() const __NOTHROW__ {
		return mString.size();
	}

	spIUTF8String UTF8StringImpl::returnSelfSharedPointer() {
		try {
			return shared_from_this();
		} catch ( bad_weak_ptr & ) {
			return spIUTF8String();
		}
	}

	sizet UTF8StringImpl::ValidatePosParameter( const sizet & pos ) const {
		sizet size = this->size();
		if ( pos > size ) {
			NOTIFY_ERROR( IError_v1::kEDGeneral, kGECIndexOutOfBounds,
				"pos mentioned is out of bounds", IError_v1::kESOperationFatal,
				true, pos, true, size );
		}
		return size;
	}

	spIUTF8String IUTF8String_I::CreateUTF8String( const char * buf /* = NULL */, sizet count /* = npos */ ) {
		auto sp = MakeUncheckedSharedPointer( new UTF8StringImpl(), __FILE__, __LINE__, true );
		sp->assign( buf, count );
		return sp;
	}

	TAllocator< spIUTF8String > gTAllocatorUTF8String;

}

#if BUILDING_XMPCOMMON_LIB || SOURCE_COMPILING_XMP_ALL
namespace AdobeXMPCommon {
	spIUTF8String IUTF8String_v1::MakeShared( pIUTF8String_base ptr ) {
		if ( !ptr ) return spIUTF8String();
		pIUTF8String p = IUTF8String::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IUTF8String >() : ptr;
		return XMP_COMPONENT_INT_NAMESPACE::MakeUncheckedSharedPointer( p, __FILE__, __LINE__, true );
	}

	spIUTF8String IUTF8String_v1::CreateUTF8String( pIObjectFactory objFactory ) {
		pcIError error( NULL );
		auto retValue = MakeShared( objFactory->CreateUTF8String( NULL, AdobeXMPCommon::npos, error ) );
		if ( error ) throw IError::MakeShared( error );
		return retValue;
	}

	spIUTF8String IUTF8String_v1::CreateUTF8String( pIObjectFactory objFactory, const char * buf, sizet count ) {
		pcIError error( NULL );
		auto retValue = MakeShared( objFactory->CreateUTF8String( buf, count, error ) );
		if ( error ) throw IError::MakeShared( error );
		return retValue;
	}

}
#endif  // BUILDING_XMPCOMMON_LIB || SOURCE_COMPILING_XMP_ALL
