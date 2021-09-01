//!
//! @file IConfigurable.h
//!

#ifndef IConfigurable_h__
#define IConfigurable_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/XMPCommonFwdDeclarations.h"

namespace AdobeXMPCommon {

	//!
	//! \brief Interface that allows to attach various key-value parameters to the underlying object.
	//!
	//! \details Key is an unsigned 64-bit integer value which can be a char buffer of eight characters also.
	//! \note For all value types except user data ( const void * ) or char buffer ( const char * ) a copy is made
	//!  and is stored, so the scope is maintained internally. But for user data ( const void * ) or char buffer
	//!  ( const char * ) its clients responsibility to make sure these pointers remain valid through out the life
	//!  span of the object or objects derived from it.
	//!
	class XMP_PUBLIC IConfigurable
	{
	public:

		//!
		//! @brief Indicates various types of parameter values.
		//!
		typedef enum {
			//!< Data type is none.
			kDTNone					= 0,
			//!< Data type is boolean.
			kDTBool					= 1 << 0,
			//!< Data type is unsigned 64 bit integer.
			kDTUint64				= 1 << 1,
			//!< Data type is signed 64 bit integer.
			kDTInt64				= 1 << 2,
			//!< Data type is character.
			kDTChar					= 1 << 3,
			//!< Data type is double.
			kDTDouble				= 1 << 4,
			//!< Data type is char buffer.
			kDTConstCharBuffer		= 1 << 5,
			//!< Data type is user data ( pointer to const void ).
			kDTConstVoidPtr			= 1 << 6,

			//!< Maximum value this enum can hold.
			kDTAll					= 0xFFFFFFFF
		} eDataType;

		//!
		//! @{
		//! @brief Add/Change a value of a parameter.
		//! \param[in] key An unsigned 64 bit integer value indicating the key.
		//! \param[in] value New value of the parameter.
		//! \attention Error is thrown in case
		//!		- the previous type of value associated with key is of different type.
		//!		- the type of value associated with key is not as expected.
		//!
		virtual void APICALL SetParameter( const uint64 & key, bool value ) = 0;
		virtual void APICALL SetParameter( const uint64 & key, uint64 value ) = 0;
		virtual void APICALL SetParameter( const uint64 & key, int64 value ) = 0;
		virtual void APICALL SetParameter( const uint64 & key, double value ) = 0;
		virtual void APICALL SetParameter( const uint64 & key, char value ) = 0;
		virtual void APICALL SetParameter( const uint64 & key, const char * value ) = 0;
		virtual void APICALL SetParameter( const uint64 & key, const void * value ) = 0;
		//! @}

		//!
		//! @brief Removes a particular parameter if present.
		//! \param[in] key An unsigned 64 bit integer value indicating the key.
		//! \return True in case key was present and is deleted.
		//! \attention Error is thrown in case
		//!		- key is a must have for the underlying object.
		//!
		virtual bool APICALL RemoveParameter( const uint64 & key ) = 0;

		//!
		//! @{
		//! @brief Get the value of a parameter if present.
		//! \param[in] key An unsigned 64 bit integer value indicating the key.
		//! \param[out] value The value of the parameter.
		//! \return false if no such parameter is present, otherwise true.
		//! \attention Error is thrown in case the type of the parameter is not
		//!  the one client is asking for.
		//!
		virtual bool APICALL GetParameter( const uint64 & key, bool & value ) const = 0;
		virtual bool APICALL GetParameter( const uint64 & key, uint64 & value ) const = 0;
		virtual bool APICALL GetParameter( const uint64 & key, int64 & value ) const = 0;
		virtual bool APICALL GetParameter( const uint64 & key, double & value ) const = 0;
		virtual bool APICALL GetParameter( const uint64 & key, char & value ) const = 0;
		virtual bool APICALL GetParameter( const uint64 & key, const char * & value ) const = 0;
		virtual bool APICALL GetParameter( const uint64 & key, const void * & value ) const = 0;
		//! @}

		//!
		//! @brief Get all the keys of the parameters associated with the object.
		//! \details Provide a std::vector containing the keys of all the parameters associated with the object.
		//! \return A std::vector of unsigned 64 bit integers.
		//!
		virtual std::vector< uint64 > APICALL GetAllParameters() const = 0;

		//!
		//! @brief Get the number of parameters associated with the object.
		//!
		virtual sizet APICALL Size() const __NOTHROW__ = 0;

		//!
		//! @brief Get the value type of a particular parameter.
		//! \param[in] key An unsigned 64 bit integer value indicating the key.
		//! \return A value of type eDataType indicating the type of value the parameter is supposed to hold.
		//! \note return kDTNone in case no such key is associated with the object.
		//!
		virtual eDataType APICALL GetDataType( const uint64 & key ) const = 0;

		//!
		//! @brief Utility function to convert character buffer ( maximum of 8 characters ) to uint64 representation.
		//! \param[in] key A pointer to const char buffer, maximum characters used are 8 provided there is no
		//!  null character present in the buffer between 1st to 8 characters, otherwise characters upto NULL
		//!  character (excluding NULL) are read.
		//! \return A 64-bit unsigned integer representing the first 8 characters of the character buffer.
		//! \note Return 0 in case key is NULL.
		//!
		static uint64 ConvertCharBufferToUint64( const char * key ) {
			uint64 keyAsuint64 = 0;
			if ( key ) {
				for ( int i = 0; i < 8 && key[ i ] != '\0'; i++ ) {
					keyAsuint64 = keyAsuint64 << 8;
					keyAsuint64 += ( unsigned char ) key[ i ];
				}
			}
			return keyAsuint64;
		}

		//!
		//! @brief A union data type to store all kind of values.
		//!
		union CombinedDataValue {
			bool			boolValue;
			uint32			uint32Value;
			uint64			uint64Value;
			int64			int64Value;
			double			doubleValue;
			char			charValue;
			const char *	constCharPtrValue;
			const void *	constVoidPtrValue;
		};

	protected:
		//! \cond XMP_INTERNAL_DOCUMENTATION
		// all safe functions
		virtual void APICALL setParameter( const uint64 & key, uint32 dataType, const CombinedDataValue & dataValue, pcIError_base & error ) __NOTHROW__ = 0;
		virtual uint32 APICALL removeParameter( const uint64 & key, pcIError_base & error ) __NOTHROW__ = 0;
		virtual uint32 APICALL getParameter( const uint64 & key, uint32 dataType, CombinedDataValue & value, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual void APICALL getAllParameters( uint64 * array, sizet count ) const __NOTHROW__ = 0;
		virtual uint32 APICALL getDataType( const uint64 & key, pcIError_base & error ) const __NOTHROW__ = 0;
		//! \endcond

	protected:
		//!
		//! protected Virtual Destructor
		//!
		virtual ~IConfigurable() __NOTHROW__ {};

		friend class IConfigurableProxy;
	#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
	#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};

//! \cond XMP_INTERNAL_DOCUMENTATION
#if !BUILDING_XMPCOMMON_LIB && !SOURCE_COMPILING_XMP_ALL

	class IConfigurableProxy
		: public virtual IConfigurable {
	public:
		IConfigurableProxy( pIConfigurable configurable );
		virtual void APICALL SetParameter( const uint64 & key, bool value );
		virtual void APICALL SetParameter( const uint64 & key, uint64 value );
		virtual void APICALL SetParameter( const uint64 & key, int64 value );
		virtual void APICALL SetParameter( const uint64 & key, double value );
		virtual void APICALL SetParameter( const uint64 & key, char value );
		virtual void APICALL SetParameter( const uint64 & key, const char * value );
		virtual void APICALL SetParameter( const uint64 & key, const void * value );
		virtual void APICALL setParameter( const uint64 & key, uint32 dataType, const CombinedDataValue & dataValue, pcIError_base & error ) __NOTHROW__;

		virtual bool APICALL RemoveParameter( const uint64 & key );
		virtual uint32 APICALL removeParameter( const uint64 & key, pcIError_base & error ) __NOTHROW__;

		virtual bool APICALL GetParameter( const uint64 & key, bool & value ) const;
		virtual bool APICALL GetParameter( const uint64 & key, uint64 & value ) const;
		virtual bool APICALL GetParameter( const uint64 & key, int64 & value ) const;
		virtual bool APICALL GetParameter( const uint64 & key, double & value ) const;
		virtual bool APICALL GetParameter( const uint64 & key, char & value ) const;
		virtual bool APICALL GetParameter( const uint64 & key, const char * & value ) const;
		virtual bool APICALL GetParameter( const uint64 & key, const void * & value ) const;
		virtual uint32 APICALL getParameter( const uint64 & key, uint32 dataType, CombinedDataValue & value, pcIError_base & error ) const __NOTHROW__;

		virtual std::vector< uint64 > APICALL GetAllParameters() const;
		virtual void APICALL getAllParameters( uint64 * array, sizet count ) const __NOTHROW__;

		virtual sizet APICALL Size() const __NOTHROW__;

		virtual eDataType APICALL GetDataType( const uint64 & key ) const;
		virtual uint32 APICALL getDataType( const uint64 & key, pcIError_base & error ) const __NOTHROW__;

	protected:
		pIConfigurable		mConfigurableRawPtr;
	};

#endif  // !BUILDING_XMPCOMMON_LIB && !SOURCE_COMPILING_XMP_ALL
//! \endcond

}

#endif  // IConfigurable_h__
