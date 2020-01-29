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

#include "XMPCommon/Interfaces/BaseInterfaces/IConfigurable_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"

namespace XMP_COMPONENT_INT_NAMESPACE {

	void APICALL IConfigurable_I::SetParameter( const uint64 & key, bool value ) {
		CombinedDataValue uValue;
		uValue.boolValue = value;
		SetParameter( key, kDTBool, uValue );
	}

	void APICALL IConfigurable_I::SetParameter( const uint64 & key, uint64 value ) {
		CombinedDataValue uValue;
		uValue.uint64Value = value;
		SetParameter( key, kDTUint64, uValue );
	}

	void APICALL IConfigurable_I::SetParameter( const uint64 & key, int64 value ) {
		CombinedDataValue uValue;
		uValue.int64Value = value;
		SetParameter( key, kDTInt64, uValue );
	}

	void APICALL IConfigurable_I::SetParameter( const uint64 & key, double value ) {
		CombinedDataValue uValue;
		uValue.doubleValue = value;
		SetParameter( key, kDTDouble, uValue );
	}

	void APICALL IConfigurable_I::SetParameter( const uint64 & key, char value ) {
		CombinedDataValue uValue;
		uValue.charValue = value;
		SetParameter( key, kDTChar, uValue );
	}

	void APICALL IConfigurable_I::SetParameter( const uint64 & key, const char * value ) {
		CombinedDataValue uValue;
		uValue.constCharPtrValue = value;
		SetParameter( key, kDTConstCharBuffer, uValue );
	}

	void APICALL IConfigurable_I::SetParameter( const uint64 & key, const void * value ) {
		CombinedDataValue uValue;
		uValue.constVoidPtrValue = value;
		SetParameter( key, kDTConstVoidPtr, uValue );
	}

	void APICALL IConfigurable_I::setParameter( const uint64 & key, uint32 dataType, const CombinedDataValue & dataValue, pcIError_base & error ) __NOTHROW__ {
		if ( dataType == IConfigurable::kDTBool ) {
			CombinedDataValue & temp = const_cast< CombinedDataValue & >( dataValue );
			temp.boolValue = dataValue.uint32Value != 0 ? true : false;
		}
		CallUnSafeFunctionReturningVoid< IConfigurable_I, const uint64 &, eDataType, const CombinedDataValue & >(
			error, this, &IConfigurable_I::SetParameter, __FILE__, __LINE__, key, static_cast< eDataType >( dataType ), dataValue );
	}

	bool APICALL IConfigurable_I::GetParameter( const uint64 & key, bool & value ) const {
		CombinedDataValue uValue;
		auto a = GetParameter( key, kDTBool, uValue );
		value = uValue.boolValue;
		return a;
	}

	bool APICALL IConfigurable_I::GetParameter( const uint64 & key, uint64 & value ) const {
		CombinedDataValue uValue;
		auto a = GetParameter( key, kDTUint64, uValue );
		value = uValue.uint64Value;
		return a;
	}

	bool APICALL IConfigurable_I::GetParameter( const uint64 & key, int64 & value ) const {
		CombinedDataValue uValue;
		auto a = GetParameter( key, kDTInt64, uValue );
		value = uValue.int64Value;
		return a;
	}

	bool APICALL IConfigurable_I::GetParameter( const uint64 & key, double & value ) const {
		CombinedDataValue uValue;
		auto a = GetParameter( key, kDTDouble, uValue );
		value = uValue.doubleValue;
		return a;
	}

	bool APICALL IConfigurable_I::GetParameter( const uint64 & key, char & value ) const {
		CombinedDataValue uValue;
		auto a = GetParameter( key, kDTChar, uValue );
		value = uValue.charValue;
		return a;
	}

	bool APICALL IConfigurable_I::GetParameter( const uint64 & key, const char * & value ) const {
		CombinedDataValue uValue;
		auto a = GetParameter( key, kDTConstCharBuffer, uValue );
		value = uValue.constCharPtrValue;
		return a;
	}

	bool APICALL IConfigurable_I::GetParameter( const uint64 & key, const void * & value ) const {
		CombinedDataValue uValue;
		auto a = GetParameter( key, kDTConstVoidPtr, uValue );
		value = uValue.constVoidPtrValue;
		return a;
	}

	uint32 APICALL IConfigurable_I::getParameter( const uint64 & key, uint32 dataType, CombinedDataValue & value, pcIError_base & error ) const __NOTHROW__ {
		uint32 retValue = CallConstUnSafeFunction< IConfigurable_I, uint32, bool, const uint64 &, eDataType, CombinedDataValue & >(
			error, this, false, &IConfigurable_I::GetParameter, __FILE__, __LINE__, key, static_cast< eDataType >( dataType ), value );
		if ( dataType == kDTBool )
			value.boolValue = value.uint32Value != 0;
		return retValue;
	}

	uint32 APICALL IConfigurable_I::removeParameter( const uint64 & key, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< IConfigurable, uint32, bool, const uint64 & >(
			error, this, false, &IConfigurable::RemoveParameter, __FILE__, __LINE__, key );
	}

	void APICALL IConfigurable_I::getAllParameters( uint64 * array, sizet count ) const __NOTHROW__ {
		sizet innerCount = Size();
		std::vector< uint64 > vec = GetAllParameters();
		for ( sizet i = 0; i < innerCount; i++ ) {
			array[i] = vec[i];
		}
	}

	uint32 APICALL IConfigurable_I::getDataType( const uint64 & key, pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunction< IConfigurable, uint32, eDataType, const uint64 & >(
			error, this, kDTNone, &IConfigurable::GetDataType, __FILE__, __LINE__, key );
	}

}

