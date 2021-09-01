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

#include "XMPFiles/source/FormatSupport/WAVE/iXMLMetadata.h"
#include "source/Endian.h"

#include "source/ExpatAdapter.hpp"
#include "XMPFiles/source/XMPFiles_Impl.hpp"
#include <algorithm>

namespace IFF_RIFF {

	static const char * tagNames[ iXMLMetadata::kLastEntry - 1 ] = {
		"TAPE",										//kTape,								// std::string
		"TAKE",										//kTake,								// std::string
		"SCENE",									//kScene,								// std::string
		"NOTE",										//kNote,								// std::string
		"PROJECT",									//kProject,								// std::string
		"NO_GOOD",									//kNoGood,								// bool( true/false )
		"FILE_SAMPLE_RATE",							//kFileSampleRate,						// XMP_Uns64
		"AUDIO_BIT_DEPTH",							//kAudioBitDepth,						// XMP_Uns64
		"CIRCLED",									//kCircled,								// bool( true/false )
		"BWF_DESCRIPTION",							//kBWFDescription,						// std::string( 256 )
		"BWF_ORIGINATOR",							//kBWFOriginator,						// std::string( 32 )
		"BWF_ORIGINATOR_REFERENCE",					//kBWFOriginatorReference,				// std::string( 32 )
		"BWF_ORIGINATION_DATE",						//kBWFOriginationDate,					// std::string( 10 )
		"BWF_ORIGINATION_TIME",						//kBWFOriginationTime,					// std::string( 8 )
		"BWF_TIME_REFERENCE_LOW",					//kBWFTimeReferenceLow,					// XMP_Uns32
		"BWF_TIME_REFERENCE_HIGH",					//kBWFTimeReferenceHigh,				// XMP_Uns32
		"BWF_VERSION",								//kBWFVersion,							// XMP_Uns16
		"BWF_UMID",									//kBWFUMID,								// std::string[64]
		"BWF_CODING_HISTORY",						//kBWFHistory,							// std::string
		"TIMECODE_FLAG",							//kTimeCodeFlag,						// std::string[DF/NDF]
		"TIMECODE_RATE",							//kTimeCodeRate,						// std::string
		"TIMESTAMP_SAMPLE_RATE",					//kTimeStampSampleRate,					// XMP_Uns64
		"TIMESTAMP_SAMPLES_SINCE_MIDNIGHT_LO",		//kTimeStampSampleSinceMidnightLow,		// XMP_Uns32
		"TIMESTAMP_SAMPLES_SINCE_MIDNIGHT_HI",		//kTimeStampSampleSinceMidnightHi,		// XMP_Uns32
		"TRACK_LIST",								//kTrackList							// std::vector< TrackListInfo >
	};

	static const char * rootTagName = "BWFXML";
	static const char * speedTagName = "SPEED";
	static const char * bextTagName = "BEXT";
	static const char * trackCountTagName = "TRACK_COUNT";
	static const char * trackTagName = "TRACK";
	static const char * trackChannelIndexTagName = "CHANNEL_INDEX";
	static const char * trackInterleaveIndexTagName = "INTERLEAVE_INDEX";
	static const char * trackNameTagName = "NAME";
	static const char * trackFunctionTagName = "FUNCTION";

	iXMLMetadata::iXMLMetadata()
		: mRootNode( NULL )
		, mExpatAdapter( NULL )
		, mErrorCallback( NULL )
		, mExtraSpaceSize( 1024 ) {}

	iXMLMetadata::~iXMLMetadata() {
		if ( mExpatAdapter ) {
			mRootNode = NULL;
		}
		delete mExpatAdapter;
		delete mRootNode;
		mExpatAdapter = NULL;
	}

#define MAX_SZ ((size_t)~((size_t)0))

	void iXMLMetadata::parse( const XMP_Uns8 * chunkData, XMP_Uns64 size ) {
		if ( chunkData == NULL || size == 0 ) {
			XMP_Error error( kXMPErr_BadBlockFormat, "iXML Metadata reconciliation failure: iXML chunk is not well formed" );
			NotifyClient( kXMPErrSev_Recoverable, error );
			return;
		}

		mExpatAdapter = XMP_NewExpatAdapter ( ExpatAdapter::kUseLocalNamespaces );
		if ( mExpatAdapter == 0 ) XMP_Throw ( "iXMLMetadata: Can't create Expat adapter", kXMPErr_NoMemory );
		mExpatAdapter->SetErrorCallback( mErrorCallback );

		try {
			XMP_Uns64 parsedSize = 0;
			while ( parsedSize < size ) {
				XMP_Uns64 currentSize = std::min<XMP_Uns64>( size - parsedSize, MAX_SZ );
				mExpatAdapter->ParseBuffer( chunkData + parsedSize, (size_t) currentSize, false );
				parsedSize += currentSize;
			}
			mExpatAdapter->ParseBuffer( 0, 0, true );
		} catch( ... ) {
			XMP_Error error( kXMPErr_BadBlockFormat, "iXML Metadata reconciliation failure: iXML chunk is not well formed" );
			NotifyClient( kXMPErrSev_Recoverable, error );
			return;
		}

		// read the particular nodes we are interested in and store in the map
		// Get the root node

		// Get the root node of the XML tree.

		XML_Node & xmlTree = mExpatAdapter->tree;

		for ( size_t i = 0, limit = xmlTree.content.size(); i < limit; ++i ) {
			if ( xmlTree.content[i]->kind == kElemNode ) {
				mRootNode = xmlTree.content[i];
				break;
			}
		}

		if ( mRootNode == NULL ) {
			XMP_Error error( kXMPErr_BadBlockFormat, "iXML Metadata reconciliation failure: No Root Element present in iXML chunk" );
			NotifyClient( kXMPErrSev_Recoverable, error );
			return;
		}

		XMP_StringPtr rootLocalName = mRootNode->name.c_str() + mRootNode->nsPrefixLen;

		if ( ! XMP_LitMatch ( rootLocalName, rootTagName ) ) {
			XMP_Error error( kXMPErr_BadBlockFormat, "iXML Metadata reconciliation failure: Unexpected Root Element present in iXML chunk" );
			NotifyClient( kXMPErrSev_Recoverable, error );
			return;
		}

		XMP_StringPtr ns = mRootNode->ns.c_str();

		XML_NodePtr currentNode( NULL );

		ParseAndSetProperties();
		resetChanges();
	}

	XMP_Uns64 iXMLMetadata::serialize( XMP_Uns8** buffer ) {
		*buffer = NULL;
		if ( mRootNode == NULL ) {
			mRootNode = new XML_Node( NULL, rootTagName, kElemNode );
			if ( mRootNode == NULL ) {
				XMP_Error error( kXMPErr_NoMemory, "iXML Metadata reconciliation failure: Can't create Root Node" );
				NotifyClient( kXMPErrSev_OperationFatal, error );
				return 0;
			}
		}


		// Create SPEED and bext node if required
		XML_Node * node = mRootNode->GetNamedElement( "", speedTagName );
		if ( node == NULL ) {
			node = new XML_Node( mRootNode, speedTagName, kElemNode );
			if ( node == NULL ) {
				XMP_Error error( kXMPErr_NoMemory, "iXML Metadata reconciliation failure: Can't create Speed Node" );
				NotifyClient( kXMPErrSev_OperationFatal, error );
				return 0;
			}
			mRootNode->content.push_back( node );
		}

		node = mRootNode->GetNamedElement( "", bextTagName );
		if ( node == NULL ) {
			node = new XML_Node( mRootNode, bextTagName, kElemNode );
			if ( node == NULL ) {
				XMP_Error error( kXMPErr_NoMemory, "iXML Metadata reconciliation failure: Can't create Bext Node" );
				NotifyClient( kXMPErrSev_OperationFatal, error );
				return 0;
			}
			mRootNode->content.push_back( node );
		}

		UpdateProperties();

		// get the SPEED and bext node and remove them if empty
		if ( node->content.size() == 0 )
			RemoveXMLNode( mRootNode, bextTagName );
		node = mRootNode->GetNamedElement( "", speedTagName );
		if ( node->content.size() == 0 )
			RemoveXMLNode( mRootNode, speedTagName );
		node = NULL;

		std::string strBuffer;
		mRootNode->Serialize( &strBuffer );

		// move the contents of the string into the new bigger buffer
		size_t newSize = strBuffer.size() + mExtraSpaceSize;
		XMP_Uns8 * newBuffer = new XMP_Uns8[ newSize ];
		memset( newBuffer, 0x20, newSize );
		memcpy( newBuffer, "<\?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n", 39 );
		memcpy( newBuffer + 39, strBuffer.c_str(), strBuffer.size() );

		*buffer = newBuffer;

		return newSize;
	}

	bool iXMLMetadata::isEmptyValue( XMP_Uns32 id, ValueObject& valueObj ) {
		bool ret = true;

		switch( id )
		{
		case kTape:
		case kTake:
		case kScene:
		case kNote:
		case kProject:
		case kBWFDescription:
		case kBWFOriginator:
		case kBWFOriginatorReference:
		case kBWFOriginationDate:
		case kBWFOriginationTime:
		case kBWFHistory:
		case kBWFUMID:
		case kTimeCodeFlag:
		case kTimeCodeRate:
			{
				TValueObject<std::string>* strObj = dynamic_cast<TValueObject<std::string>*>(&valueObj);

				ret = ( strObj == NULL || ( strObj != NULL && strObj->getValue().empty() ) );
			}
			break;

		case kFileSampleRate:
		case kAudioBitDepth:
		case kBWFTimeReferenceLow:
		case kBWFTimeReferenceHigh:
		case kBWFVersion:
		case kTimeStampSampleRate:
		case kTimeStampSampleSinceMidnightLow:
		case kTimeStampSampleSinceMidnightHigh:
			ret = false;
			break;

		case kNoGood:
		case kCircled:
			ret = false;
			break;

		case kTrackList:
			ret = true;
			{
				TArrayObject< TrackListInfo > * trackListInfoArrayObj = dynamic_cast< TArrayObject< TrackListInfo > *>( &valueObj );
				if ( trackListInfoArrayObj ) {
					XMP_Uns32 nElements ( 0 );
					const TrackListInfo * trackListInfoPtr = trackListInfoArrayObj->getArray( nElements );
					if ( nElements > 0 && trackListInfoPtr != NULL )
						ret = false;
				}
			}
			break;

		case kNativeTrackCount:
			ret = false;
			break;

		default:
			ret = true;
		}

		return ret;
	}

	void iXMLMetadata::ParseAndSetStringProperty( XML_Node * parentNode, XMP_Uns32 id ) {
		std::string nodeValue = ParseStringValue( parentNode, id );
		if ( nodeValue.size() > 0 ) {
			this->setValue< std::string >( id, nodeValue );
		}
	}

	void iXMLMetadata::SetErrorCallback( GenericErrorCallback * errorCallback ) {
		mErrorCallback = errorCallback;
	}

	void iXMLMetadata::NotifyClient( XMP_ErrorSeverity severity, XMP_Error & error ) {
		XMPFileHandler::NotifyClient( mErrorCallback, severity, error );
	}

	static std::string & Trim( std::string & str ) {
		static const char * whiteSpaceChars = " \t\n\r\v";
		size_t pos = str.find_last_not_of( whiteSpaceChars );
		if ( pos != std::string::npos ) {
			str.erase( pos + 1 );
		}
		return str;
	}

	static XMP_Uns64 ConvertStringToUns64( const std::string & strValue ) {
		int count;
		char nextCh;
		XMP_Uns64 result;

		count = sscanf ( strValue.c_str(), "%llu%c", &result, &nextCh );
		if ( count != 1 ) XMP_Throw ( "Invalid integer string", kXMPErr_BadParam );

		return result;
	}

	void iXMLMetadata::ParseAndSetIntegerProperty( XML_Node * parentNode, XMP_Uns32 id ) {
		std::string strValue = ParseStringValue( parentNode, id );

		if ( strValue.size() > 0 ) {
			XMP_Uns64 uValue;
			try {
				uValue = ConvertStringToUns64( Trim( strValue ) );
			} catch( ... ) {
				// some nodes like tape can be non integer also. Treat it as warning
				XMP_Error error( kXMPErr_BadFileFormat, "iXML Metadata reconciliation failure: node is supposed to have integer value" );
				NotifyClient( kXMPErrSev_Recoverable, error );
				return;
			}
			this->setValue< XMP_Uns64 >( id, uValue );
		}
	}

	void iXMLMetadata::ParseAndSetBoolProperty( XML_Node * parentNode, XMP_Uns32 id ) {
		std::string strValue = ParseStringValue( parentNode, id );
		if ( strValue.size() > 0 ) {
			if ( strValue.compare( "TRUE" ) == 0 )
				this->setValue< bool >( id, true );
			else if ( strValue.compare( "FALSE" ) == 0 )
				this->setValue< bool >( id, false );
			else {
				XMP_Error error( kXMPErr_BadBlockFormat, "iXML Metadata reconciliation failure: invalid boolean value present" );
				NotifyClient( kXMPErrSev_Recoverable, error );
			}
		}
	}

	void iXMLMetadata::ParseAndSetProperties() {

		// top level properties
		ParseAndSetStringProperty( mRootNode, kTape );
		ParseAndSetStringProperty( mRootNode, kTake );
		ParseAndSetStringProperty( mRootNode, kScene );
		ParseAndSetStringProperty( mRootNode, kNote );
		ParseAndSetStringProperty( mRootNode, kProject );
		ParseAndSetBoolProperty( mRootNode, kNoGood );
		ParseAndSetBoolProperty( mRootNode, kCircled );

		// speed node children
		XML_Node * speedNode = mRootNode->GetNamedElement( "", speedTagName );
		if ( speedNode ) {
			ParseAndSetIntegerProperty( speedNode, kFileSampleRate );
			ParseAndSetIntegerProperty( speedNode, kAudioBitDepth );
			ParseAndSetStringProperty( speedNode, kTimeCodeFlag );
			ParseAndSetStringProperty( speedNode, kTimeCodeRate );
			ParseAndSetIntegerProperty( speedNode, kTimeStampSampleRate );
			ParseAndSetIntegerProperty( speedNode, kTimeStampSampleSinceMidnightLow );
			ParseAndSetIntegerProperty( speedNode, kTimeStampSampleSinceMidnightHigh );
		}

		// bext node children
		XML_Node * bextNode = mRootNode->GetNamedElement( "", bextTagName );
		if ( bextNode ) {
			ParseAndSetStringProperty( bextNode, kBWFDescription );
			ParseAndSetStringProperty( bextNode, kBWFOriginator );
			ParseAndSetStringProperty( bextNode, kBWFOriginatorReference );
			ParseAndSetStringProperty( bextNode, kBWFOriginationDate );
			ParseAndSetStringProperty( bextNode, kBWFOriginationTime );
			ParseAndSetIntegerProperty( bextNode, kBWFTimeReferenceLow );
			ParseAndSetIntegerProperty( bextNode, kBWFTimeReferenceHigh );
			ParseAndSetIntegerProperty( bextNode, kBWFVersion );
			ParseAndSetStringProperty( bextNode, kBWFHistory );
			ParseAndSetStringProperty( bextNode, kBWFUMID );
		}

		// TRACK_LIST
		XML_Node * trackListNode = mRootNode->GetNamedElement( "", tagNames[ kTrackList ] );
		if ( trackListNode ) {
			ParseAndSetTrackListInfo( trackListNode );
		}
	}

	void iXMLMetadata::UpdateStringProperty( XML_Node * parentNode, XMP_Uns32 id ) {
		if ( valueExists( id ) ) {
			std::string value;
			try {
				 value = this->getValue< std::string >( id );
			} catch ( ... ) {
				XMP_Error e1( kXMPErr_BadValue, "iXML Metadata reconciliation failure: expected the value to be of string type" );
				NotifyClient( kXMPErrSev_Recoverable, e1 );
				return;
			}
			UpdateXMLNode( parentNode, tagNames[ id ], value );
		} else {
			RemoveXMLNode( parentNode, tagNames[ id ] );
		}
	}

	void iXMLMetadata::UpdateXMLNode( XML_Node * parentNode, const char * localName, const std::string & value ) {
		XML_Node * node = parentNode->GetNamedElement( "", localName );
		if (node == NULL) {
			
				node = new XML_Node(parentNode, localName, kElemNode);
				if (node == NULL) {
					XMP_Error error(kXMPErr_NoMemory, "Unable to create new objects");
					NotifyClient(kXMPErrSev_OperationFatal, error);
					return;
				}
				parentNode->content.push_back(node);
			}

		if ( node->IsLeafContentNode() == false ) {
			XMP_Error error( kXMPErr_BadBlockFormat, "iXML Metadata reconciliation failure: node was supposed to be a leaf node" );
			NotifyClient( kXMPErrSev_Recoverable, error );
			node->RemoveContent();
		}
		node->SetLeafContentValue( value.c_str() );
	}

	void iXMLMetadata::RemoveXMLNode( XML_Node * parentNode, const char * localName ) {
		XML_Node * node = parentNode->GetNamedElement( "", localName );
		if ( node ) {
			// find the position
			XML_NodeVector::iterator it = std::find( parentNode->content.begin(), parentNode->content.end(), node );
			XMP_Assert( it != parentNode->content.end() );
			parentNode->content.erase( it );
			delete node;
		}
	}

#if XMP_WinBuild
	#define snprintf _snprintf
#endif
	static std::string ConvertUns64ToString( XMP_Uns64 uValue ) {
		char buffer[64];

		snprintf( buffer, sizeof( buffer ), "%llu", uValue );
		std::string str( buffer );
		return str;
	}

	void iXMLMetadata::UpdateIntegerProperty( XML_Node * parentNode, XMP_Uns32 id ) {
		if ( valueExists( id ) ) {
			XMP_Uns64 uValue;
			try {
				uValue = this->getValue< XMP_Uns64 >( id );
			} catch ( ... ) {
				XMP_Error error( kXMPErr_BadValue, "iXML Metadata reconciliation failure: expected the value to be of XMP_Uns64 type" );
				NotifyClient( kXMPErrSev_Recoverable, error );
				return;
			}
			std::string strValue = ConvertUns64ToString( uValue );
			UpdateXMLNode( parentNode, tagNames[ id ], strValue );
		} else {
			RemoveXMLNode( parentNode, tagNames[ id ] );
		}
	}

	void iXMLMetadata::UpdateBoolProperty( XML_Node * parentNode, XMP_Uns32 id ) {
		if ( valueExists( id ) ) {
			bool value;

			try {
				value = this->getValue< bool >( id );
			} catch ( ... ) {
				XMP_Error error( kXMPErr_BadValue, "iXML Metadata reconciliation failure: expected the value to be of bool type" );
				NotifyClient( kXMPErrSev_Recoverable, error );
				return;
			}

			std::string strValue;
			if ( value ) strValue = "TRUE";
			else strValue = "FALSE";

			UpdateXMLNode( parentNode, tagNames[ id ], strValue );
		} else {
			RemoveXMLNode( parentNode, tagNames[ id ] );
		}
	}

	void iXMLMetadata::UpdateTrackListInfo( XML_Node * parentNode ) {
		if ( valueExists( kTrackList ) ) {
			RemoveXMLNode(parentNode, tagNames[kTrackList]);
			XMP_Uns32 size;
			const TrackListInfo * arrayObj( NULL );
			try {
				arrayObj = this->getArray< TrackListInfo >( kTrackList, size );
			} catch( ... ) {
				XMP_Error error( kXMPErr_BadValue, "iXML Metadata reconciliation failure: expected the array of TrackListInfo type" );
				NotifyClient( kXMPErrSev_Recoverable, error );
				return;
			}

			if ( size > 0 ) {
				XML_Node * node = parentNode->GetNamedElement( "", tagNames[ kTrackList ] );

				if ( node == NULL ) {
					node = new XML_Node( parentNode, tagNames[ kTrackList ], kElemNode );
					if ( node == NULL ) {
						XMP_Error error( kXMPErr_NoMemory, "Unable to create new objects" );
						NotifyClient( kXMPErrSev_OperationFatal, error );
						return;
					}
					parentNode->content.push_back( node );
				}

				// add count
				std::string count = ConvertUns64ToString( size );
				UpdateXMLNode( node, trackCountTagName, count );

				for ( XMP_Uns32 i = 0; i < size; i++ ) {
					XML_Node * track = node->GetNamedElement( "", trackTagName, i );
					if ( track == NULL ) {
						track = new XML_Node( parentNode, trackTagName, kElemNode );
						if ( track == NULL ) {
							XMP_Error error( kXMPErr_NoMemory, "Unable to create new objects" );
							NotifyClient( kXMPErrSev_OperationFatal, error );
							return;
						}
						node->content.push_back( track );
					}
					const TrackListInfo & ref = arrayObj[i];
					if (ref.mChannelIndex.size()>0)
						UpdateXMLNode( track, trackChannelIndexTagName, ref.mChannelIndex);
					if (ref.mInterleaveIndex.size()>0)
						UpdateXMLNode( track, trackInterleaveIndexTagName,ref.mInterleaveIndex );
					if (ref.mName.size()>0)
						UpdateXMLNode( track, trackNameTagName, ref.mName );
					if (ref.mFunction.size()>0)
						UpdateXMLNode( track, trackFunctionTagName, ref.mFunction );
				}
			} else {
				RemoveXMLNode( parentNode, tagNames[ kTrackList ] );
			}
		} else {
			RemoveXMLNode( parentNode, tagNames[ kTrackList ] );
		}
	}

	void iXMLMetadata::UpdateProperties() {
		// top level properties
		UpdateStringProperty( mRootNode, kTape );
		UpdateStringProperty( mRootNode, kTake );
		UpdateStringProperty( mRootNode, kScene );
		UpdateStringProperty( mRootNode, kNote );
		UpdateStringProperty( mRootNode, kProject );
		UpdateBoolProperty( mRootNode, kNoGood );
		UpdateBoolProperty( mRootNode, kCircled );

		// speed node children
		XML_Node * speedNode = mRootNode->GetNamedElement( "", speedTagName );
		if ( speedNode ) {
			UpdateIntegerProperty( speedNode, kFileSampleRate );
			UpdateIntegerProperty( speedNode, kAudioBitDepth );
			UpdateStringProperty( speedNode, kTimeCodeFlag );
			UpdateStringProperty( speedNode, kTimeCodeRate );
			UpdateIntegerProperty( speedNode, kTimeStampSampleRate );
			UpdateIntegerProperty( speedNode, kTimeStampSampleSinceMidnightLow );
			UpdateIntegerProperty( speedNode, kTimeStampSampleSinceMidnightHigh );
		}

		// bext node children
		XML_Node * bextNode = mRootNode->GetNamedElement( "", bextTagName );
		if ( bextNode ) {
			UpdateStringProperty( bextNode, kBWFDescription );
			UpdateStringProperty( bextNode, kBWFOriginator );
			UpdateStringProperty( bextNode, kBWFOriginatorReference );
			UpdateStringProperty( bextNode, kBWFOriginationDate );
			UpdateStringProperty( bextNode, kBWFOriginationTime );
			UpdateIntegerProperty( bextNode, kBWFTimeReferenceLow );
			UpdateIntegerProperty( bextNode, kBWFTimeReferenceHigh );
			UpdateIntegerProperty( bextNode, kBWFVersion );
			UpdateStringProperty( bextNode, kBWFHistory );
			UpdateStringProperty( bextNode, kBWFUMID );
		}

		UpdateTrackListInfo( mRootNode );
	}

	bool iXMLMetadata::valueValid( XMP_Uns32 id, ValueObject * valueObj ) {
		switch( id ) {
		case kTape:
			return validateStringSize( valueObj );
			break;

		case kTake:
			return validateStringSize( valueObj );
			break;

		case kScene:
			return validateStringSize( valueObj );
			break;
			
		case kNote:
			return validateStringSize( valueObj );
			break;

		case kProject:
			return validateStringSize( valueObj );
			break;
			
		case kNoGood:
			return validateBool( valueObj );
			break;

		case kFileSampleRate:
			return validateInt( valueObj );
			break;

		case kAudioBitDepth:
			return validateInt( valueObj );
			break;

		case kCircled:
			return validateBool( valueObj );
			break;

		case kBWFDescription:
		case kBWFOriginator:
		case kBWFOriginatorReference:
			// string length can be anything but while setting it will be trimmed.
			return validateStringSize( valueObj );
			break;

		case kBWFOriginationDate:
			return validateDate( valueObj );
			break;

		case kBWFOriginationTime:
			return validateTime( valueObj );
			break;

		case kBWFTimeReferenceLow:
		case kBWFTimeReferenceHigh:
			return validateInt( valueObj, 0, Max_XMP_Uns32 );
			break;

		case kBWFVersion:
			return validateInt( valueObj, 0, Max_XMP_Uns16 );
			break;

		case kBWFUMID:
			return validateUMID( valueObj );
			break;

		case kBWFHistory:
			return validateStringSize( valueObj );
			break;

		case kTimeCodeFlag:
			return validateTimeCodeFlag( valueObj );
			break;

		case kTimeCodeRate:
			return validateRational( valueObj );
			break;

		case kTimeStampSampleRate:
			return validateInt( valueObj );
			break;

		case kTimeStampSampleSinceMidnightHigh:
		case kTimeStampSampleSinceMidnightLow:
			return validateInt( valueObj, 0, Max_XMP_Uns32 );
			break;

		case kNativeTrackCount:
			return validateInt(valueObj, 1, Max_XMP_Uns64);
			break;

		default:
			return false;
			break;
		}
	}

	void iXMLMetadata::valueModify( XMP_Uns32 id, ValueObject * value ) {
		switch( id ) {
		case kBWFDescription:
			shortenString( value, 256 );
			break;

		case kBWFOriginator:
			shortenString( value, 32 );
			break;

		case kBWFOriginatorReference:
			shortenString( value, 32 );
			break;

		case kBWFUMID:
			shortenString( value, 128 );
			break;

		default:
			// do nothing
			break;
		}
	}

	bool iXMLMetadata::validateStringSize( ValueObject * value, size_t minSize /*= 1*/, size_t maxSize /*= std::string::npos */ ) {
		TValueObject<std::string>* strObj = dynamic_cast<TValueObject<std::string>*>(value);
		if ( strObj ) {
			const std::string * strPtr = &strObj->getValue();
			size_t sizeOfString = strPtr->size();
			if ( sizeOfString < minSize ) {
				XMP_Error error( kXMPErr_BadValue, "iXML Metadata reconciliation failure: length of string is less than expected" );
				NotifyClient( kXMPErrSev_Recoverable, error );
				return false;
			} else if ( sizeOfString > maxSize ) {
				XMP_Error error( kXMPErr_BadBlockFormat, "iXML Metadata reconciliation failure: length of string is more than expected" );
				NotifyClient( kXMPErrSev_Recoverable, error);
				return false;
			} else {
				return true;
			}
		} else { // object is not of string type
			XMP_Error error( kXMPErr_BadValue, "iXML Metadata reconciliation failure: expected string value" );
			NotifyClient( kXMPErrSev_Recoverable, error );
			return false;
		}
		return false;
	}

	bool iXMLMetadata::validateInt( ValueObject * value, XMP_Uns64 minValue /*= 0*/, XMP_Uns64 maxValue /*= Max_XMP_Uns64*/ ) {
		TValueObject< XMP_Uns64 > * valuePtr = dynamic_cast< TValueObject< XMP_Uns64 > * > ( value );

		if ( valuePtr ) {
			XMP_Uns64 uValue = valuePtr->getValue();
			if ( uValue < minValue ) {
				XMP_Error error( kXMPErr_BadBlockFormat, "iXML Metadata reconciliation failure: node integer value is less than allowed" );
				NotifyClient( kXMPErrSev_Recoverable, error );
				return false;
			} else if ( uValue > maxValue ) {
				XMP_Error error( kXMPErr_BadBlockFormat, "iXML Metadata reconciliation failure: node integer value is more than allowed" );
				NotifyClient( kXMPErrSev_Recoverable, error );
				return false;
			} else {
				return true;
			}
		} else {
			XMP_Error error( kXMPErr_BadValue, "iXML Metadata reconciliation failure: expected XMP_Uns64 value" );
			NotifyClient( kXMPErrSev_Recoverable, error );
			return false;
		}
	}

	bool iXMLMetadata::validateBool( ValueObject * value ) {
		// just check typecasts is possible or not
		TValueObject< bool > * boolValuePtr = dynamic_cast< TValueObject< bool > * > ( value );
		if ( boolValuePtr ) {
			return true;
		} else {
			XMP_Error error( kXMPErr_BadValue, "iXML Metadata reconciliation failure: expected bool value" );
			NotifyClient( kXMPErrSev_Recoverable, error );
			return false;
		}
	}

	void iXMLMetadata::shortenString( ValueObject * value, size_t lengthOfString ) {
		TValueObject< std::string > * strObj = dynamic_cast< TValueObject< std::string > * >( value );
		if ( strObj ) {
			const std::string * strPtr = &strObj->getValue();
			size_t sizeOfString = strPtr->size();
			if ( sizeOfString > lengthOfString ) {
				std::string newString;
				newString.append( *strPtr, 0, lengthOfString );
				strObj->setValue( newString );
			}
		}
	}

	static bool charIsNumber( const char & ch ) {
		if ( ch >= '0' && ch <= '9' )
			return true;
		else
			return false;
	}

	bool iXMLMetadata::validateDate( ValueObject * value ) {
		bool stringOK = validateStringSize( value, 10, 10 );
		if ( stringOK ) {
			TValueObject< std::string > * strObj = dynamic_cast< TValueObject< std::string > * >( value );
			const std::string * strPtr = &strObj->getValue();
			// check 0,1,2,3,5,6,8,9 elements are integer
			for ( size_t i = 0; i < 10; i++ ) {
				if ( i == 4 || i == 7 )
					continue;
				stringOK = charIsNumber( strPtr->operator[]( i ) );
				if ( !stringOK ) {
					XMP_Error error( kXMPErr_BadValue, "iXML Metadata reconciliation failure: expected a number character" );
					NotifyClient( kXMPErrSev_Recoverable, error );
					return false;
				}
			}
			return stringOK;
		}
		return false;
	}

	bool iXMLMetadata::validateTime( ValueObject * value ) {
		bool stringOK = validateStringSize( value, 8, 8 );
		if ( stringOK ) {
			TValueObject< std::string > * strObj = dynamic_cast< TValueObject< std::string > * >( value );
			const std::string * strPtr = &strObj->getValue();
			// check 0,1,3,4,6,7 elements are integer
			for ( size_t i = 0; i < 8; i++ ) {
				if ( i == 2 || i == 5 )
					continue;
				stringOK = charIsNumber( strPtr->operator[]( i ) );
				if ( !stringOK ) {
					XMP_Error error( kXMPErr_BadValue, "iXML Metadata reconciliation failure: expected a number character" );
					NotifyClient( kXMPErrSev_Recoverable, error );
					return false;
				}
			}
			return stringOK;
		}
		return false;
	}

	static bool charIsHexDigit( const char & ch ) {
		if ( charIsNumber( ch ) ) {
			return true;
		} else {
			if ( ch >= 'A' && ch <= 'F' )
				return true;
			else if ( ch >= 'a' && ch <= 'f' )
				return true;
			else
				return false;
		}
	}

	bool iXMLMetadata::validateUMID( ValueObject * value ) {
		bool stringOK = validateStringSize( value );
		if ( stringOK ) {
			// max size to consider is 128
			TValueObject< std::string > * strObj = dynamic_cast< TValueObject< std::string > * >( value );
			const std::string * strPtr = &strObj->getValue();
			size_t effectiveLength = strPtr->size();
			if ( effectiveLength > 128 )
				effectiveLength = 128;

			// first check length needs to even
			if ( effectiveLength % 2 == 0 ) {
				for ( size_t i = 0; i < effectiveLength; i++ ) {
					stringOK = charIsHexDigit( strPtr->operator[]( i ) );
					if ( !stringOK ) {
						XMP_Error error( kXMPErr_BadValue, "iXML Metadata reconciliation failure: expected a hex character" );
						NotifyClient( kXMPErrSev_Recoverable, error );
						return false;
					}
				}
				return stringOK;
			} else {
				XMP_Error error( kXMPErr_BadValue, "iXML Metadata reconciliation failure: expected the hex string length to be even" );
				NotifyClient( kXMPErrSev_Recoverable, error );
				return false;
			}
		}
		return false;
	}

	std::string iXMLMetadata::ParseStringValue( XML_Node * parentNode, XMP_Uns32 id ) {
		return ParseStringValue( parentNode, tagNames[ id ] );
	}

	std::string iXMLMetadata::ParseStringValue( XML_Node * parentNode, const char * tagName, bool recoverable ) {
		std::string nodeValue;
		XML_Node * node = parentNode->GetNamedElement( "", tagName );
		if ( node ) {
			if ( node->IsLeafContentNode() && node->content.size() != 0 ) {
				size_t lengthOfValue = node->content[0]->value.size();
				if ( lengthOfValue > 0 ) {
					nodeValue = node->content[0]->value;
				}
			} else {
				XMP_Error error( kXMPErr_BadBlockFormat, "iXML Metadata reconciliation failure: node was supposed to be a leaf node" );
				NotifyClient( recoverable ? kXMPErrSev_Recoverable : kXMPErrSev_OperationFatal, error );
			}
		} else {
			XMP_Error error(kXMPErr_BadBlockFormat, "iXML Metadata reconciliation failure: node not present");
			NotifyClient(recoverable ? kXMPErrSev_Recoverable : kXMPErrSev_OperationFatal, error);
		}
		return nodeValue;
	}

	XMP_Uns64 iXMLMetadata::ParseUns64Value( XML_Node * parentNode, const char * tagName ) {
		std::string strValue = ParseStringValue( parentNode, tagName, false );

		if ( strValue.size() > 0 ) {
			XMP_Uns64 uValue;
			try {
				uValue = ConvertStringToUns64( Trim( strValue ) );
			} catch( ... ) {
				// some nodes like tape can be non integer also. Treat it as warning
				XMP_Error error( kXMPErr_BadFileFormat, "iXML Metadata reconciliation failure: node is supposed to have integer value" );
				NotifyClient( kXMPErrSev_OperationFatal, error );
			}
			return uValue;
		}
		return Max_XMP_Uns64;
	}

	
	/*
	* Handling of Tracklist changed in refrence to bug CTECHXMP-4169661
	*/
	void iXMLMetadata::ParseAndSetTrackListInfo( XML_Node * parentNode ) {
		XMP_Uns64 trackCount( 0 );
		XMP_Uns64 nativeTrackCount(0); //to notice if there is change in track count as read from iXML chunk
		try
		{
			nativeTrackCount = ParseUns64Value(parentNode, trackCountTagName);
			this->setValue<XMP_Uns64>(kNativeTrackCount, nativeTrackCount);
		}
		catch (...) {
			//do nothing. don't set native track count value
		}
		trackCount = parentNode->CountNamedElements("", trackTagName);
		std::vector< TrackListInfo > trackInfoListVector ;

		for ( size_t i = 0; i < trackCount; i++ ) {
			XML_Node * trackElement = parentNode->GetNamedElement( "", trackTagName, i );
			if ( trackElement ) {
				
				std::string name, function, channelIndex, interleaveIndex;

					channelIndex = ParseStringValue( trackElement, trackChannelIndexTagName );
					interleaveIndex = ParseStringValue( trackElement, trackInterleaveIndexTagName );
					name = ParseStringValue( trackElement, trackNameTagName );
					function = ParseStringValue( trackElement, trackFunctionTagName );
				
					if (channelIndex.size() > 0 || interleaveIndex.size() > 0 || name.size() > 0 || function.size() > 0)
					{
						TrackListInfo currentElement(channelIndex, name, function, interleaveIndex);
						trackInfoListVector.push_back(currentElement);
					}
				
			} else {
				trackCount = i;
			}
		}

		XMP_Uns32 count = (XMP_Uns32)trackInfoListVector.size();
		if (count > 0 ) {
			this->setArray( kTrackList, trackInfoListVector.data(), count);
		}
	}

	bool iXMLMetadata::validateTimeCodeFlag( ValueObject * value ) {
		bool returnValue = validateStringSize( value, 2, 3 );
		if ( returnValue ) {
			TValueObject<std::string>* strObj = dynamic_cast<TValueObject<std::string>*>(value);
			if ( strObj ) {
				const std::string * strPtr = &strObj->getValue();
				if ( strPtr->compare( "DF" ) == 0 )
					return true;
				else if ( strPtr->compare( "NDF" ) == 0 )
					return true;
			}
		}
		return false;
	}

	bool iXMLMetadata::validateRational( ValueObject * value ) {
		bool returnValue = validateStringSize( value, 3 );
		if ( returnValue ) {
			TValueObject<std::string>* strObj = dynamic_cast<TValueObject<std::string>*>(value);
			if ( strObj ) {
				const std::string * strPtr = &strObj->getValue();
				size_t posOfSlash = strPtr->find( "/" );
				if ( posOfSlash == std::string::npos || posOfSlash == strPtr->size() - 1 || posOfSlash == 0 ) {
					XMP_Error error( kXMPErr_BadValue, "iXML Metadata reconciliation failure: node value was supposed to be in a fractional format" );
					NotifyClient( kXMPErrSev_Recoverable, error );
					return false;
				}

				for ( size_t i = 0; i < strPtr->size(); i++ ) {
					if ( i == posOfSlash )
						continue;
					returnValue = charIsNumber( strPtr->operator[]( i ) );
					if ( ! returnValue ) {
						XMP_Error error( kXMPErr_BadValue, "iXML Metadata reconciliation failure: expected a number character" );
						NotifyClient( kXMPErrSev_Recoverable, error );
						return false;
					}
				}
				return returnValue;
			}
		}
		return false;
	}

	bool iXMLMetadata::validateTrackListInfo( ValueObject * value ) {
		return true;
	}

}
