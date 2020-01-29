#ifndef ConfigurableImpl_h__
#define ConfigurableImpl_h__ 1

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

#if !(IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED)
	#error "Not adhering to design constraints"
	// this file should only be included from its own cpp file
#endif

#include "XMPCommon/Interfaces/BaseInterfaces/IConfigurable_I.h"
#include "XMPCommon/Utilities/TAllocator.h"

#include <set>

namespace XMP_COMPONENT_INT_NAMESPACE {
	
	class ConfigurableImpl
		: public virtual IConfigurable_I
	{
	public:
		typedef std::map< uint64, TypeValuePair, std::less< uint64 >, TAllocator< KeyValuePair > > KeyValueMap;
		typedef std::set< uint64, std::less< uint64 >, TAllocator< uint64 > > KeysSet;
		typedef std::map< uint64, eDataType, std::less< uint64 >, TAllocator< KeyValueTypePair > > keyValueTypeMap;

		ConfigurableImpl()
			: mKeysSet( NULL )
			, mKeyValueTypeMap( NULL )
			, mTreatKeyAsCaseInsensitiveCharBuffer( false )
			, mAllowDifferentValueTypesForExistingEntries( false ) {}

		using IConfigurable_I::SetParameter;
		using IConfigurable_I::GetParameter;
		
		virtual void APICALL SetParameter( const uint64 & key, eDataType type, const CombinedDataValue & value );
		virtual bool APICALL GetParameter( const uint64 & key, eDataType type, CombinedDataValue & value ) const;
		virtual bool APICALL RemoveParameter( const uint64 & key );
		virtual std::vector< uint64 > APICALL GetAllParameters() const;
		virtual sizet APICALL Size() const __NOTHROW__;
		virtual eDataType APICALL GetDataType( const uint64 & key ) const;
        
        virtual eConfigurableErrorCode APICALL ValidateValue( const uint64 & key, eDataType type, const CombinedDataValue & value ) const;
        
        
		virtual void APICALL TreatKeyAsCaseInsensitive( bool flag = true );
		virtual void APICALL AllowDifferentValueTypesForExistingEntries( bool flag = true );

		void APICALL SetAllowedKeys( uint64 * keysTable, sizet sizeOfTable );
		void APICALL SetAllowedValueTypesForKeys( KeyValueTypePair * keyValueTypePairTable, sizet sizeOfTable );
       

		virtual ~ConfigurableImpl() __NOTHROW__;

	protected:
		KeyValueMap					mMap;
		KeysSet *					mKeysSet;
		keyValueTypeMap *			mKeyValueTypeMap;
		bool						mTreatKeyAsCaseInsensitiveCharBuffer;
		bool						mAllowDifferentValueTypesForExistingEntries;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();

	};
}

#endif  // ConfigurableImpl_h__
