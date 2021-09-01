// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/PathSegmentImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"

#include <cstring>
namespace AdobeXMPCore_Int {

	// All virtual functions

	PathSegmentImpl::PathSegmentImpl( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength,
		const char * value, sizet valueLength, ePathSegmentType type, sizet index )
		: mNameSpace( IUTF8String_I::CreateUTF8String( nameSpace, nameSpaceLength ) )
		, mName( IUTF8String_I::CreateUTF8String( name, nameLength ) )
		, mValue( IUTF8String_I::CreateUTF8String( value, valueLength ) )
		, mType( type )
		, mIndex( index ) { }

	spcIUTF8String APICALL PathSegmentImpl::GetNameSpace() const {
		return mNameSpace;
	}

	spcIUTF8String APICALL PathSegmentImpl::GetName() const {
		return mName;
	}

	IPathSegment_v1::ePathSegmentType APICALL PathSegmentImpl::GetType() const {
		return mType;
	}

	sizet APICALL PathSegmentImpl::GetIndex() const __NOTHROW__ {
		return mIndex;
	}

	spcIUTF8String APICALL PathSegmentImpl::GetValue() const {
		return mValue;
	}

	AdobeXMPCore::spcIPathSegment PathSegmentImpl::Clone() const {
		return MakeUncheckedSharedPointer(
			new PathSegmentImpl( this->mNameSpace->c_str(), this->mNameSpace->size(),
			this->mName->c_str(), this->mName->size(), this->mValue->c_str(), this->mValue->size(), mType, mIndex ), __FILE__, __LINE__, true );
	}

	spcIPathSegment IPathSegment_I::CreatePropertyPathSegment( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) {
		return MakeUncheckedSharedPointer(
			new PathSegmentImpl( nameSpace->c_str(), nameSpace->size(), name->c_str(), name->size(), NULL, AdobeXMPCommon::npos, kPSTProperty, kMaxSize ), __FILE__, __LINE__, true );
	}

	spcIPathSegment IPathSegment_I::CreateArrayIndexPathSegment( const spcIUTF8String & nameSpace, sizet index ) {
		return MakeUncheckedSharedPointer(
			new PathSegmentImpl( nameSpace->c_str(), nameSpace->size() , NULL, AdobeXMPCommon::npos, NULL, AdobeXMPCommon::npos, kPSTArrayIndex, index ), __FILE__, __LINE__, true );
	}

	spcIPathSegment IPathSegment_I::CreateQualifierPathSegment( const spcIUTF8String & nameSpace, const spcIUTF8String & name ) {
		return MakeUncheckedSharedPointer(
			new PathSegmentImpl( nameSpace->c_str(), nameSpace->size(), name->c_str(), name->size(), NULL, AdobeXMPCommon::npos, kPSTQualifier, kMaxSize ), __FILE__, __LINE__, true );
	}

	spcIPathSegment IPathSegment_I::CreateQualifierSelectorPathSegment( const spcIUTF8String & nameSpace, const spcIUTF8String & name, const spcIUTF8String & value ) {
		return MakeUncheckedSharedPointer(
			new PathSegmentImpl( nameSpace->c_str(), nameSpace->size(), name->c_str(), name->size(), value->c_str(), value->size(), kPSTQualifierSelector, kMaxSize ), __FILE__, __LINE__, true );
	}

}

#if BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
namespace AdobeXMPCore {
	using namespace AdobeXMPCore_Int;

	spcIPathSegment IPathSegment_v1::CreatePropertyPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		if ( nameSpace && nameSpaceLength == AdobeXMPCommon::npos )
			nameSpaceLength = strlen( nameSpace );
		if ( name && nameLength == AdobeXMPCommon::npos )
			nameLength = strlen( name );

		if ( nameSpace == NULL || name == NULL || nameSpaceLength == 0 || nameLength == 0 )
			NOTIFY_ERROR( IError::kEDGeneral, kGECParametersNotAsExpected, "Parameters to CreatePropertyPathSegment() are not as expected",
				IError::kESOperationFatal, true, ( void * ) nameSpace, true, ( void * ) name, true, nameSpaceLength, true, nameLength, nameSpace, nameSpace, name, name );
		return MakeUncheckedSharedPointer(
			new PathSegmentImpl( nameSpace, nameSpaceLength, name, nameLength, NULL, AdobeXMPCommon::npos, kPSTProperty, kMaxSize ), __FILE__, __LINE__, true );
	}

	spcIPathSegment IPathSegment_v1::CreateArrayIndexPathSegment( const char * nameSpace, sizet nameSpaceLength, sizet index ) {
		if ( nameSpace && nameSpaceLength == AdobeXMPCommon::npos )
			nameSpaceLength = strlen( nameSpace );
		if ( nameSpace == NULL || nameSpaceLength == 0 )
			NOTIFY_ERROR( IError::kEDGeneral, kGECParametersNotAsExpected, "Parameters to CreateArrayIndexPathSegment() are not as expected",
				IError::kESOperationFatal, true, ( void * ) nameSpace, true, nameSpaceLength, nameSpace, nameSpace );
		return MakeUncheckedSharedPointer(
			new PathSegmentImpl( nameSpace, nameSpaceLength, NULL, AdobeXMPCommon::npos, NULL, AdobeXMPCommon::npos, kPSTArrayIndex, index ), __FILE__, __LINE__, true );
	}

	spcIPathSegment IPathSegment_v1::CreateQualifierPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength ) {
		if ( nameSpace && nameSpaceLength == AdobeXMPCommon::npos )
			nameSpaceLength = strlen( nameSpace );
		if ( name && nameLength == AdobeXMPCommon::npos )
			nameLength = strlen( name );

		if ( nameSpace == NULL || name == NULL || nameSpaceLength == 0 || nameLength == 0 )
			NOTIFY_ERROR( IError::kEDGeneral, kGECParametersNotAsExpected, "Parameters to CreateQualifierPathSegment() are not as expected",
				IError::kESOperationFatal, true, ( void * ) nameSpace, true, ( void * ) name, true, nameSpaceLength, true, nameLength, nameSpace, nameSpace, name, name );
		return MakeUncheckedSharedPointer(
			new PathSegmentImpl( nameSpace, nameSpaceLength, name, nameLength, NULL, AdobeXMPCommon::npos, kPSTQualifier, kMaxSize ), __FILE__, __LINE__, true );
	}

	spcIPathSegment IPathSegment_v1::CreateQualifierSelectorPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength,
		const char * value, sizet valueLength )
	{
		if ( nameSpace && nameSpaceLength == AdobeXMPCommon::npos )
			nameSpaceLength = strlen( nameSpace );
		if ( name && nameLength == AdobeXMPCommon::npos )
			nameLength = strlen( name );
		if ( value && valueLength == AdobeXMPCommon::npos )
			valueLength = strlen( value );

		if ( nameSpace == NULL || name == NULL || value == NULL || nameSpaceLength == 0 || nameLength == 0 || valueLength == 0 )
			NOTIFY_ERROR( IError::kEDGeneral, kGECParametersNotAsExpected, "Parameters to CreateQualifierSelectorPathSegment() are not as expected",
				IError::kESOperationFatal, true, ( void * ) nameSpace, true, ( void * ) name, true, ( void * ) value,
				true, nameSpaceLength, true, nameLength, true, valueLength );
		return MakeUncheckedSharedPointer(
			new PathSegmentImpl( nameSpace, nameSpaceLength, name, nameLength, value, valueLength, kPSTQualifierSelector, kMaxSize ), __FILE__, __LINE__, true );
	}

	spIPathSegment IPathSegment_v1::MakeShared( pIPathSegment_base ptr ) {
		if ( !ptr ) return spIPathSegment();
		pIPathSegment p = IPathSegment::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IPathSegment >() : ptr;
		return MakeUncheckedSharedPointer( p, __FILE__, __LINE__, false );
	}
}
#endif  // BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB

