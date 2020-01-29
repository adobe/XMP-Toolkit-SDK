#ifndef IConfigurable_I_h__
#define IConfigurable_I_h__ 1

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

#include "XMPCommon/XMPCommonFwdDeclarations_I.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IConfigurable.h"
#include "XMPCommon/XMPCommonErrorCodes.h"
#include <map>

namespace XMP_COMPONENT_INT_NAMESPACE {

	class IConfigurable_I
		: public virtual IConfigurable
	{
	public:

		typedef struct std::pair< eDataType, CombinedDataValue > TypeValuePair;
		typedef std::pair< const uint64, TypeValuePair >	KeyValuePair;
		typedef std::pair< const uint64, eDataType > KeyValueTypePair;

		//!
		//! Function to validate a key.
		//! \param[in] key an unsigned 64 bit number indicating the key of the parameter.
		//! If an object only supports fixed number of keys, it can override this function and
		//! return error for an unsupported key, otherwise return #kCECNone.
		//!
		virtual eConfigurableErrorCode APICALL ValidateKey( const uint64 & key ) const { return kCECNone; }
		

		//!
		//! Function to modify the key.
		//! \param[in] key an unsigned 64 bit number indicating the key of the parameter.
		//! If an object supports eight character case in-sensitive keys, it can override this function
		//! and convert them to lower case.
		//! \return the new key.
		//!
		virtual uint64 APICALL ModifyKey( const uint64 & key ) const { return key; }

		//!
		//! Function to validate the value and its type for a particular key.
		//! \param[in] key an unsigned 64 bit number indicating the key of the parameter.
		//! \param[in] type an eDataType enum value indicating the type of data associated with the key.
		//! \param[in] value indicates the value of the data.
		//! If an object supports a key with only a fixed type or it wants to constrain the values a key
		//! can hold, it can override this function for such operations. Returns an error code in case
		//! of an error condition, otherwise return kCECNone.
		//!
        
        virtual eConfigurableErrorCode APICALL ValidateValue( const uint64 & key, eDataType type, const CombinedDataValue & value ) const=0;
        
       
        //!
		//! Allows keys to be treated as case insensitive.
		//! \param[in] flag a boolean value if set to true makes key as case insensitive
		//!
		virtual void APICALL TreatKeyAsCaseInsensitive( bool flag = true ) = 0;

		//!
		//! Allows type of value to be changed later on.
		//! \param[in] flag a boolean value if set to true allows even the type of data associated with the key to be changed later on.
		//!
		virtual void APICALL AllowDifferentValueTypesForExistingEntries( bool flag = true ) = 0;

		//!
		//! Restrict to have only specific keys.
		//! \param[in] keysTable an array of uint64 values containing all the keys.
		//! \param[in] sizeofTable number of entries in the array.
		//!
		virtual void APICALL SetAllowedKeys( uint64 * keysTable, sizet sizeOfTable ) = 0;

		//!
		//! Restrict data type of particular keys.
		//! \param[in] keyValueTypePairTable an array of KeyValueTypePair containing all the allowed dataTypes for the keys.
		//! \param[in] sizeOfTable number of entries in the array.
		//!
		virtual void APICALL SetAllowedValueTypesForKeys( KeyValueTypePair * keyValueTypePairTable, sizet sizeOfTable ) = 0;

		//!
		//! Function to get access to the mutex object.
		//!
		virtual spISharedMutex APICALL GetMutex() const = 0;

		virtual void APICALL SetParameter( const uint64 & key, bool value );
		virtual void APICALL SetParameter( const uint64 & key, uint64 value );
		virtual void APICALL SetParameter( const uint64 & key, int64 value );
		virtual void APICALL SetParameter( const uint64 & key, double value );
		virtual void APICALL SetParameter( const uint64 & key, char value );
		virtual void APICALL SetParameter( const uint64 & key, const char * value );
		virtual void APICALL SetParameter( const uint64 & key, const void * value );

		virtual bool APICALL GetParameter( const uint64 & key, bool & value ) const;
		virtual bool APICALL GetParameter( const uint64 & key, uint64 & value ) const;
		virtual bool APICALL GetParameter( const uint64 & key, int64 & value ) const;
		virtual bool APICALL GetParameter( const uint64 & key, double & value ) const;
		virtual bool APICALL GetParameter( const uint64 & key, char & value ) const;
		virtual bool APICALL GetParameter( const uint64 & key, const char * & value ) const;
		virtual bool APICALL GetParameter( const uint64 & key, const void * & value ) const;

	protected:

		//!
		//! protected Virtual Destructor
		//!
		virtual ~IConfigurable_I() __NOTHROW__ {};

		virtual void APICALL setParameter( const uint64 & key, uint32 dataType, const CombinedDataValue & dataValue, pcIError_base & error ) __NOTHROW__;
		virtual uint32 APICALL removeParameter( const uint64 & key, pcIError_base & error ) __NOTHROW__;
		virtual uint32 APICALL getParameter( const uint64 & key, uint32 dataType, CombinedDataValue & value, pcIError_base & error ) const __NOTHROW__;
		virtual void APICALL getAllParameters( uint64 * array, sizet count ) const __NOTHROW__;
		virtual uint32 APICALL getDataType( const uint64 & key, pcIError_base & error ) const __NOTHROW__;

		virtual void APICALL SetParameter( const uint64 & key, eDataType type, const CombinedDataValue & value ) = 0;
		virtual bool APICALL GetParameter( const uint64 & key, eDataType type, CombinedDataValue & value ) const = 0;

	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};

}

#endif  // IConfigurable_I_h__
