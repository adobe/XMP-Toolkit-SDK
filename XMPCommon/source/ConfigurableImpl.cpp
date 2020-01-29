// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
#include "XMPCommon/ImplHeaders/ConfigurableImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IErrorNotifier_I.h"
#include "XMPCommon/Utilities/AutoSharedLock.h"
#include "XMPCommon/Interfaces/IError_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	static uint64 ConvertToLowerCase( const uint64 & key ) {
		static char add = 'a' - 'A';
		union temp { uint64 uInt64; char chars[ 8 ]; }; 
		temp t; t.uInt64 = key;
		for ( sizet i = 0; i < 8; i++ ) {
			if ( t.chars[ i ] >= 'A' && t.chars[ i ] <= 'Z' ) {
				t.chars[ i ] += add;
			}
		}
		return t.uInt64;
	}

	template < typename T1, typename T2 >
	void NotifyError( const char * errorMsg, const uint64 & key, eConfigurableErrorCode errorCode,
		IConfigurable::eDataType type, T1 v1,
		IConfigurable::eDataType oldType, T2 v2 )
	{
		bool valuePresent = type != IConfigurable::kDTNone;
		bool oldValuePresent = oldType != IConfigurable::kDTNone;
		NOTIFY_ERROR( IError_v1::kEDConfigurable, errorCode, errorMsg, IError_v1::kESOperationFatal,
			true, key, valuePresent, static_cast< uint64 >( type ), oldValuePresent, static_cast< uint64 >( oldType ),
			valuePresent, v1, oldValuePresent, v2 );
	}

	template < typename T1 >
	void NotifyError( const char * errorMsg, const uint64 & key, eConfigurableErrorCode errorCode,
		IConfigurable::eDataType type, T1 v1,
		IConfigurable::eDataType oldType, const IConfigurable::CombinedDataValue & oldValue )
	{
		char temp[2] = { '\0', '\0' };
		switch ( oldType ) {
		case IConfigurable::kDTBool:
			NotifyError< T1, bool >( errorMsg, key, errorCode, type, v1, oldType, oldValue.boolValue );
			break;

		case IConfigurable::kDTChar:
			temp[0] = oldValue.charValue;
			NotifyError< T1, const char * >( errorMsg, key, errorCode, type, v1, oldType, &temp[0] );
			break;

		case IConfigurable::kDTUint64:
			NotifyError< T1, uint64 >( errorMsg, key, errorCode, type, v1, oldType, oldValue.uint64Value );
			break;

		case IConfigurable::kDTInt64:
			NotifyError< T1, int64 >( errorMsg, key, errorCode, type, v1, oldType, oldValue.int64Value );
			break;

		case IConfigurable::kDTDouble:
			NotifyError< T1, double >( errorMsg, key, errorCode, type, v1, oldType, oldValue.doubleValue );
			break;

		case IConfigurable::kDTConstCharBuffer:
			NotifyError< T1, void * >( errorMsg, key, errorCode, type, v1, oldType, ( void * ) oldValue.constCharPtrValue );
			break;

		default:
		case IConfigurable::kDTConstVoidPtr:
			NotifyError< T1, void * >( errorMsg, key, errorCode, type, v1, oldType, ( void * ) oldValue.constVoidPtrValue );
			break;
		}
	}

    
    
    
    static void NotifyError( const char * errorMsg, const uint64 & key, eConfigurableErrorCode errorCode,
		IConfigurable::eDataType type, const IConfigurable::CombinedDataValue & value,
		IConfigurable::eDataType oldType, const IConfigurable::CombinedDataValue & oldValue )
	{
		char temp[2] = { '\0', '\0' };
		switch ( type ) {
		case IConfigurable::kDTBool:
			NotifyError< bool >( errorMsg, key, errorCode, type, value.boolValue, oldType, oldValue );
			break;

		case IConfigurable::kDTChar:
			temp[0] = value.charValue;
			NotifyError< const char * >( errorMsg, key, errorCode, type, &temp[0], oldType, oldValue );
			break;

		case IConfigurable::kDTUint64:
			NotifyError< uint64 >( errorMsg, key, errorCode, type, value.uint64Value, oldType, oldValue );
			break;

		case IConfigurable::kDTInt64:
			NotifyError< int64 >( errorMsg, key, errorCode, type, value.int64Value, oldType, oldValue );
			break;

		case IConfigurable::kDTDouble:
			NotifyError< double >( errorMsg, key, errorCode, type, value.doubleValue, oldType, oldValue );
			break;

		case IConfigurable::kDTConstCharBuffer:
			NotifyError< void * >( errorMsg, key, errorCode, type, ( void * ) value.constCharPtrValue, oldType, oldValue );
			break;

		default:
		case IConfigurable::kDTConstVoidPtr:
			NotifyError< void * >( errorMsg, key, errorCode, type, ( void * ) value.constVoidPtrValue, oldType, oldValue );
			break;
		}
	}

	//definition of class methods
	void ConfigurableImpl::SetParameter( const uint64 & actualKey, eDataType type, const CombinedDataValue & value ) {
		uint64 key = ModifyKey( actualKey );
		if ( mTreatKeyAsCaseInsensitiveCharBuffer )
			key = ConvertToLowerCase( key );
		eConfigurableErrorCode validKey = ValidateKey( key );
		eDataType oldType = kDTNone;
		CombinedDataValue oldValue;
		if ( validKey == kCECNone ) {
			AutoSharedLock lock( GetMutex(), true );
			if ( mKeysSet ) {
				auto it = mKeysSet->find( key );
				if ( it == mKeysSet->end() )
					NotifyError( "Key is not supported", key, kCECKeyNotSupported, type, value, oldType, oldValue );
			}
            
            eConfigurableErrorCode validValue = ValidateValue( key, type, value );

			if ( validValue == kCECNone && mKeyValueTypeMap ) {
				auto it = mKeyValueTypeMap->find( key );
				if ( it != mKeyValueTypeMap->end() ) {
					if ( type != it->second ) {
						validValue = kCECValueTypeNotSupported;
					}
				}
			}

			if ( validValue == kCECNone && !mAllowDifferentValueTypesForExistingEntries ) {
				auto it = mMap.find( key );
				if ( it != mMap.end() && it->second.first != type )
					validValue = kCECPreviousTypeDifferent;
			}

			if ( validValue == kCECNone ) {
				TypeValuePair pair;
				pair.first = type;
				pair.second = value;
				mMap[ key ] = pair;
			} else {
				auto it = mMap.find( key );
				if ( it != mMap.end() ) {
					oldType = it->second.first;
					oldValue = it->second.second;
				}
				NotifyError( "Validation failed for the parameter, type and value combination", key, validValue, type,
					value, oldType, oldValue );
			}
		} else {
			NotifyError( "Key is not valid", key, validKey, type, value, oldType, oldValue );
		}
	}

	bool ConfigurableImpl::GetParameter( const uint64 & actualKey, eDataType type, CombinedDataValue & value ) const {
		uint64 key = ModifyKey( actualKey );
		if ( mTreatKeyAsCaseInsensitiveCharBuffer )
			key = ConvertToLowerCase( key );
		AutoSharedLock lock( GetMutex() );
		auto it = mMap.find( key );
		if ( it == mMap.end() )
			return false;
		if ( it->second.first != type ) {
			NOTIFY_ERROR( IError_v1::kEDConfigurable, kCECValueTypeMismatch,
				"Type mismatch for a parameter", IError_v1::kESOperationFatal,
				true, key, true, static_cast< uint64 >( it->second.first ), true, static_cast< uint64 >( type ) );
			return false;
		}
		value = it->second.second;
		return true;
	}

	bool ConfigurableImpl::RemoveParameter( const uint64 & actualKey ) {
		uint64 key = ModifyKey( actualKey );
		if ( mTreatKeyAsCaseInsensitiveCharBuffer )
			key = ConvertToLowerCase( key );
		AutoSharedLock lock( GetMutex(), true );
		bool returnValue = mMap.erase( key ) >= 1 ? true : false;
		return returnValue;
	}

	std::vector< uint64 > ConfigurableImpl::GetAllParameters() const {
		auto allocator = TAllocator< uint64 >();
		std::vector< uint64 > vec( allocator );
		AutoSharedLock lock( GetMutex() );
		vec.reserve( mMap.size() );
		auto it = mMap.begin();
		auto itEnd = mMap.end();
		for ( ; it != itEnd; it++ ) {
			vec.push_back( it->first );
		}
		return vec;
	}

	sizet ConfigurableImpl::Size() const __NOTHROW__ {
		AutoSharedLock lock( GetMutex() );
		return mMap.size();
	}

	IConfigurable::eDataType ConfigurableImpl::GetDataType( const uint64 & actualKey ) const {
		uint64 key = ModifyKey( actualKey );
		if ( mTreatKeyAsCaseInsensitiveCharBuffer )
			key = ConvertToLowerCase( key );
		AutoSharedLock lock( GetMutex() );
		auto it = mMap.find( key );
		if ( it == mMap.end() )
			return kDTNone;
		else
			return it->second.first;
	}
    //using IConfigurable_I::ValidateValue;
    
	eConfigurableErrorCode ConfigurableImpl::ValidateValue( const uint64 & key, eDataType type, const CombinedDataValue & value ) const {
		return kCECNone;
	}

	void APICALL ConfigurableImpl::TreatKeyAsCaseInsensitive( bool flag ) {
		mTreatKeyAsCaseInsensitiveCharBuffer = flag;
	}

	void APICALL ConfigurableImpl::AllowDifferentValueTypesForExistingEntries( bool flag ) {
		mAllowDifferentValueTypesForExistingEntries = flag;
	}

	void APICALL ConfigurableImpl::SetAllowedKeys( uint64 * keysTable, sizet sizeOfTable ) {
		AutoSharedLock lock ( GetMutex(), true );
		if ( mKeysSet ) { delete mKeysSet; mKeysSet = NULL; }
		if ( sizeOfTable > 0 && keysTable != NULL ) {
			mKeysSet = new KeysSet();
			for ( sizet i = 0; i < sizeOfTable; i++ ) {
				uint64 key = keysTable[ i ];
				key = ModifyKey( key );
				if ( mTreatKeyAsCaseInsensitiveCharBuffer )
					key = ConvertToLowerCase( key );
				mKeysSet->insert( key );
			}
		}
	}

	void APICALL ConfigurableImpl::SetAllowedValueTypesForKeys( KeyValueTypePair * keyValueTypePairTable, sizet sizeOfTable ) {
		AutoSharedLock lock( GetMutex(), true );
		if ( mKeyValueTypeMap ) { delete mKeyValueTypeMap; mKeyValueTypeMap = NULL; }
		if ( sizeOfTable > 0 && keyValueTypePairTable != NULL ) {
			mKeyValueTypeMap = new keyValueTypeMap();
			for ( sizet i = 0; i < sizeOfTable; i++ ) {
				uint64 key = keyValueTypePairTable[i].first;
				key = ModifyKey( key );
				if ( mTreatKeyAsCaseInsensitiveCharBuffer )
					key = ConvertToLowerCase( key );
				( *mKeyValueTypeMap )[ key ] = keyValueTypePairTable[i].second;
			}
		}
	}

	ConfigurableImpl::~ConfigurableImpl() __NOTHROW__ {
		if ( mKeysSet ) { delete mKeysSet; mKeysSet = NULL; }
		if ( mKeyValueTypeMap ) { delete mKeyValueTypeMap; mKeyValueTypeMap = NULL; }
	}


}
