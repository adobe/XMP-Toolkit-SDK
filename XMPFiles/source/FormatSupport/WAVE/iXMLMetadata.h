#ifndef __iXMLMetadata_h__
#define __iXMLMetadata_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! XMP_Environment.h must be the first included header.
#include "public/include/XMP_Const.h"
#include "public/include/XMP_IO.hpp"

#include "XMPFiles/source/NativeMetadataSupport/IMetadata.h"

class ExpatAdapter;
class XML_Node;

namespace IFF_RIFF {

	/**
	*	iXML Metadata model.
	*	Implements the IMetadata interface
	*/
	class iXMLMetadata : public IMetadata
	{
	public:

		class TrackListInfo {
		public:
			TrackListInfo() {}

			TrackListInfo(const std::string & channelIndex, const std::string & name, const std::string & function, const std::string & interleaveIndex)
				: mChannelIndex(channelIndex)
				, mName(name)
				, mFunction(function)
				, mInterleaveIndex(interleaveIndex) {}

			bool operator == (const TrackListInfo & other) const {
				return mChannelIndex.compare(other.mChannelIndex) == 0 &&
					mName.compare(other.mName) == 0 &&
					mFunction.compare(other.mFunction) == 0 &&
					mInterleaveIndex.compare(other.mInterleaveIndex) == 0;
			}

			bool operator != ( const TrackListInfo & other ) const {
				return !( this->operator==( other ) );
			}

			std::string			mChannelIndex;
			std::string			mName;
			std::string			mFunction;
			std::string			mInterleaveIndex;
		};

		enum
		{
			kTape,								// std::string
			kTake,								// std::string
			kScene,								// std::string
			kNote,								// std::string
			kProject,							// std::string
			kNoGood,							// bool( true/false )
			kFileSampleRate,					// XMP_Uns64
			kAudioBitDepth,						// XMP_Uns64
			kCircled,							// bool( true/false )
			kBWFDescription,					// std::string
			kBWFOriginator,						// std::string
			kBWFOriginatorReference,			// std::string
			kBWFOriginationDate,				// std::string
			kBWFOriginationTime,				// std::string
			kBWFTimeReferenceLow,				// XMP_Uns32
			kBWFTimeReferenceHigh,				// XMP_Uns32
			kBWFVersion,						// XMP_Uns16
			kBWFUMID,							// std::string[64]
			kBWFHistory,						// std::string
			kTimeCodeFlag,						// std::string[DF/NDF]
			kTimeCodeRate,						// std::string
			kTimeStampSampleRate,				// XMP_Uns64
			kTimeStampSampleSinceMidnightLow,	// XMP_Uns32
			kTimeStampSampleSinceMidnightHigh,	// XMP_Uns32
			kTrackList,							// std::vector< TrackListInfo >
			kNativeTrackCount,
			kLastEntry
		};

	public:
		/**
		*ctor/dtor
		*/
		iXMLMetadata();
		~iXMLMetadata();

		/**
		* Parses the given memory block and creates a data model representation
		* The implementation expects that the memory block is the data area of
		* the iXML chunk.
		* Throws exceptions if parsing is not possible
		*
		* @param input		The byte buffer to parse
		* @param size		Size of the given byte buffer
		*/
		void		parse( const XMP_Uns8* chunkData, XMP_Uns64 size );

		/**
		* See IMetadata::parse( const LFA_FileRef input )
		*/
		void		parse( XMP_IO* input ) { IMetadata::parse( input ); }

		/**
		* Serializes the data model to a memory block. 
		* The memory block will be the data area of a iXML chunk.
		* Throws exceptions if serializing is not possible
		*
		* @param buffer	Buffer that gets filled with serialized data
		* @param size		Size of passed in buffer
		*
		* @return			Size of serialized data (might be smaller than buffer size)
		*/
		XMP_Uns64		serialize( XMP_Uns8** buffer );

		void SetErrorCallback( GenericErrorCallback * errorCallback );

		void SetExtraSpaceSize( size_t size ) { mExtraSpaceSize = size; }

		size_t GetExtraSpaceSize() const { return mExtraSpaceSize; }

	protected:
		/**
		* @see IMetadata::isEmptyValue
		*/
		virtual	bool isEmptyValue( XMP_Uns32 id, ValueObject& valueObj );

		/**
		 * @see iMetadata::valueValid
		 */
		virtual bool valueValid( XMP_Uns32 id, ValueObject * valueObj );

		/**
		 * @see IMetadata::valueModify
		 */
		virtual void valueModify( XMP_Uns32 id, ValueObject * value );

		void ParseAndSetProperties();
		void UpdateProperties();

		std::string ParseStringValue( XML_Node * parentNode, XMP_Uns32 id );
		std::string ParseStringValue( XML_Node * parentNode, const char * tagName, bool recoverableError = true );
		XMP_Uns64 ParseUns64Value( XML_Node * parentNode, const char * tagName );

		void ParseAndSetTrackListInfo( XML_Node * parentNode );
		void ParseAndSetStringProperty( XML_Node * parentNode, XMP_Uns32 id );
		void ParseAndSetIntegerProperty( XML_Node * parentNode, XMP_Uns32 id );
		void ParseAndSetBoolProperty( XML_Node * parentNode, XMP_Uns32 id );

		void UpdateStringProperty( XML_Node * parentNode, XMP_Uns32 id );
		void UpdateIntegerProperty( XML_Node * parentNode, XMP_Uns32 id );
		void UpdateBoolProperty( XML_Node * parentNode, XMP_Uns32 id );
		void UpdateTrackListInfo( XML_Node * parentNode );

		void UpdateXMLNode( XML_Node * parentNode, const char * localName, const std::string &  value );
		void RemoveXMLNode( XML_Node * parentNode, const char * localName );

		void NotifyClient( XMP_ErrorSeverity severity, XMP_Error & error );

		bool validateStringSize( ValueObject * value, size_t minSize = 1, size_t maxSize = std::string::npos );
		bool validateInt( ValueObject * value, XMP_Uns64 minValue = 0, XMP_Uns64 maxValue = Max_XMP_Uns64 );
		bool validateBool( ValueObject * value );
		void shortenString( ValueObject * value, size_t lengthOfString );
		bool validateDate( ValueObject * value );
		bool validateTime( ValueObject * value );
		bool validateUMID( ValueObject * value );
		bool validateTimeCodeFlag( ValueObject * value );
		bool validateRational( ValueObject * value );
		bool validateTrackListInfo( ValueObject * value );
	private:
		// Operators hidden on purpose
		iXMLMetadata( const iXMLMetadata& ) {};
		iXMLMetadata& operator=( const iXMLMetadata& ) { return *this; };

		ExpatAdapter *			mExpatAdapter;
		XML_Node *				mRootNode;
		GenericErrorCallback *	mErrorCallback;
		size_t					mExtraSpaceSize;
	};

}

#endif  // __iXMLMetadata_h__
