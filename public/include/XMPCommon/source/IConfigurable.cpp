// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/Interfaces/BaseInterfaces/IConfigurable.h"

#if !BUILDING_XMPCOMMON_LIB && !SOURCE_COMPILING_XMP_ALL

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include <assert.h>

namespace AdobeXMPCommon {

	IConfigurableProxy::IConfigurableProxy( pIConfigurable configurable )
		: mConfigurableRawPtr( configurable ) { }

	void APICALL IConfigurableProxy::SetParameter( const uint64 & key, bool value ) {
		CombinedDataValue combinedValue;
		combinedValue.uint32Value = value ? 1 : 0;
		CallSafeFunctionReturningVoid< IConfigurable, const uint64 &, uint32, const CombinedDataValue &
		>(
			mConfigurableRawPtr, &IConfigurable::setParameter, key, static_cast< uint32 >( IConfigurable::kDTBool ), combinedValue
		);
	}

	void APICALL IConfigurableProxy::setParameter( const uint64 & key, uint32 dataType, const CombinedDataValue & dataValue, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mConfigurableRawPtr->setParameter( key, dataType, dataValue, error );
	}

	bool APICALL IConfigurableProxy::RemoveParameter( const uint64 & key ) {
		return CallSafeFunction< IConfigurable, bool, uint32, const uint64 & >(
			mConfigurableRawPtr, &IConfigurable::removeParameter, key );
	}

	uint32 APICALL IConfigurableProxy::removeParameter( const uint64 & key, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mConfigurableRawPtr->removeParameter( key, error );
	}

	bool APICALL IConfigurableProxy::GetParameter( const uint64 & key, bool & value ) const {
		CombinedDataValue combinedValue;
		bool returnValue = CallConstSafeFunction< IConfigurable, bool, uint32, const uint64 &, uint32, CombinedDataValue &
		>(
			mConfigurableRawPtr, &IConfigurable::getParameter, key, static_cast< uint32 >( IConfigurable::kDTBool ), combinedValue
		);
		value = combinedValue.uint32Value != 0 ? 1 : 0;
		return returnValue;
	}

	uint32 APICALL IConfigurableProxy::getParameter( const uint64 & key, uint32 dataType, CombinedDataValue & value, pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mConfigurableRawPtr->getParameter( key, dataType, value, error );
	}

	std::vector< uint64 > APICALL IConfigurableProxy::GetAllParameters() const {
		sizet nElements = mConfigurableRawPtr->Size();
		std::vector< uint64 > vec( nElements );
		if ( nElements > 0 )
			mConfigurableRawPtr->getAllParameters( vec.data(), nElements );
		return vec;
	}

	void APICALL IConfigurableProxy::getAllParameters( uint64 * array, sizet count ) const __NOTHROW__ {
		assert( false );
		return mConfigurableRawPtr->getAllParameters( array, count );
	}

	sizet APICALL IConfigurableProxy::Size() const __NOTHROW__ {
		return mConfigurableRawPtr->Size();
	}

	IConfigurable::eDataType APICALL IConfigurableProxy::GetDataType( const uint64 & key ) const {
		return CallConstSafeFunction< IConfigurable, eDataType, uint32, const uint64 & >(
			mConfigurableRawPtr, &IConfigurable::getDataType, key );
	}

	uint32 APICALL IConfigurableProxy::getDataType( const uint64 & key, pcIError_base & error ) const __NOTHROW__ {
		assert( false );
		return mConfigurableRawPtr->getDataType( key, error );
	}

	bool APICALL IConfigurableProxy::GetParameter( const uint64 & key, const void * & value ) const {
		CombinedDataValue combinedValue;
		bool returnValue = CallConstSafeFunction< IConfigurable, bool, uint32, const uint64 &, uint32, CombinedDataValue &
		>(
			mConfigurableRawPtr, &IConfigurable::getParameter, key, static_cast< uint32 >( IConfigurable::kDTConstVoidPtr ), combinedValue
		);
		value = combinedValue.constVoidPtrValue;
		return returnValue;
	}

	bool APICALL IConfigurableProxy::GetParameter( const uint64 & key, const char * & value ) const {
		CombinedDataValue combinedValue;
		bool returnValue = CallConstSafeFunction< IConfigurable, bool, uint32, const uint64 &, uint32, CombinedDataValue &
		>(
			mConfigurableRawPtr, &IConfigurable::getParameter, key, static_cast< uint32 >( IConfigurable::kDTConstCharBuffer ), combinedValue
		);
		value = combinedValue.constCharPtrValue;
		return returnValue;
	}

	bool APICALL IConfigurableProxy::GetParameter( const uint64 & key, char & value ) const {
		CombinedDataValue combinedValue;
		bool returnValue = CallConstSafeFunction< IConfigurable, bool, uint32, const uint64 &, uint32, CombinedDataValue &
		>(
			mConfigurableRawPtr, &IConfigurable::getParameter, key, static_cast< uint32 >( IConfigurable::kDTChar ), combinedValue
		);
		value = combinedValue.charValue;
		return returnValue;
	}

	bool APICALL IConfigurableProxy::GetParameter( const uint64 & key, double & value ) const {
		CombinedDataValue combinedValue;
		bool returnValue = CallConstSafeFunction< IConfigurable, bool, uint32, const uint64 &, uint32, CombinedDataValue &
		>(
			mConfigurableRawPtr, &IConfigurable::getParameter, key, static_cast< uint32 >( IConfigurable::kDTDouble ), combinedValue
		);
		value = combinedValue.doubleValue;
		return returnValue;
	}

	bool APICALL IConfigurableProxy::GetParameter( const uint64 & key, int64 & value ) const {
		CombinedDataValue combinedValue;
		bool returnValue = CallConstSafeFunction< IConfigurable, bool, uint32, const uint64 &, uint32, CombinedDataValue &
		>(
			mConfigurableRawPtr, &IConfigurable::getParameter, key, static_cast< uint32 >( IConfigurable::kDTInt64 ), combinedValue
		);
		value = combinedValue.int64Value;
		return returnValue;
	}

	bool APICALL IConfigurableProxy::GetParameter( const uint64 & key, uint64 & value ) const {
		CombinedDataValue combinedValue;
		bool returnValue = CallConstSafeFunction< IConfigurable, bool, uint32, const uint64 &, uint32, CombinedDataValue &
		>(
		mConfigurableRawPtr, &IConfigurable::getParameter, key, static_cast< uint32 >( IConfigurable::kDTUint64 ), combinedValue
		);
		value = combinedValue.uint64Value;
		return returnValue;
	}

	void APICALL IConfigurableProxy::SetParameter( const uint64 & key, const void * value ) {
		CombinedDataValue combinedValue;
		combinedValue.constVoidPtrValue= value;
		return CallSafeFunctionReturningVoid< IConfigurable, const uint64 &, uint32, const CombinedDataValue &
		>(
			mConfigurableRawPtr, &IConfigurable::setParameter, key, static_cast< uint32 >( IConfigurable::kDTConstVoidPtr ), combinedValue
		);
	}

	void APICALL IConfigurableProxy::SetParameter( const uint64 & key, const char * value ) {
		CombinedDataValue combinedValue;
		combinedValue.constCharPtrValue = value;
		return CallSafeFunctionReturningVoid< IConfigurable, const uint64 &, uint32, const CombinedDataValue &
		>(
			mConfigurableRawPtr, &IConfigurable::setParameter, key, static_cast< uint32 >( IConfigurable::kDTConstCharBuffer ), combinedValue
		);
	}

	void APICALL IConfigurableProxy::SetParameter( const uint64 & key, char value ) {
		CombinedDataValue combinedValue;
		combinedValue.charValue = value;
		return CallSafeFunctionReturningVoid< IConfigurable, const uint64 &, uint32, const CombinedDataValue &
		>(
			mConfigurableRawPtr, &IConfigurable::setParameter, key, static_cast< uint32 >( IConfigurable::kDTChar ), combinedValue
		);
	}

	void APICALL IConfigurableProxy::SetParameter( const uint64 & key, double value ) {
		CombinedDataValue combinedValue;
		combinedValue.doubleValue = value;
		return CallSafeFunctionReturningVoid< IConfigurable, const uint64 &, uint32, const CombinedDataValue &
		>(
			mConfigurableRawPtr, &IConfigurable::setParameter, key, static_cast< uint32 >( IConfigurable::kDTDouble ), combinedValue
		);
	}

	void APICALL IConfigurableProxy::SetParameter( const uint64 & key, int64 value ) {
		CombinedDataValue combinedValue;
		combinedValue.int64Value = value;
		return CallSafeFunctionReturningVoid< IConfigurable, const uint64 &, uint32, const CombinedDataValue &
		>(
			mConfigurableRawPtr, &IConfigurable::setParameter, key, static_cast< uint32 >( IConfigurable::kDTInt64 ), combinedValue
		);
	}

	void APICALL IConfigurableProxy::SetParameter( const uint64 & key, uint64 value ) {
		CombinedDataValue combinedValue;
		combinedValue.uint64Value = value;
		return CallSafeFunctionReturningVoid< IConfigurable, const uint64 &, uint32, const CombinedDataValue &
		>(
			mConfigurableRawPtr, &IConfigurable::setParameter, key, static_cast< uint32 >( IConfigurable::kDTUint64 ), combinedValue
		);
	}

}

#endif  // !BUILDING_XMPCOMMON_LIB && !SOURCE_COMPILING_XMP_ALL
