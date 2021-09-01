// =================================================================================================
// Copyright Adobe
// Copyright 2011 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "HostAPIAccess.h"
#include "PluginUtils.h"
#include <cstring>
#include <string>
#define TXMP_STRING_TYPE std::string
#include "XMP.hpp"
#include "source/XMP_LibUtils.hpp"

namespace XMP_PLUGIN
{

///////////////////////////////////////////////////////////////////////////////
//
// API handling 
//
	
static HostAPIRef sHostAPI = NULL;
static XMP_Uns32 sHostAPIVersion = 0;

StandardHandler_API_V3* sStandardHandler_V3 = NULL;

// No need to call it externally as would have already called during intialising a plugin  
bool SetStandardHandlerAPIAccess()
{
	if ( sStandardHandler_V3 == NULL )
		sStandardHandler_V3 = (StandardHandler_API_V3 *) RequestAPISuite( "StandardHandler", 3 ) ;	
	return sStandardHandler_V3 != NULL;
}
	
// ============================================================================
	
static bool CheckAPICompatibility_V1 ( const HostAPIRef hostAPI )
{
	return ( hostAPI
			&& hostAPI->mFileIOAPI
			&& hostAPI->mStrAPI
			&& hostAPI->mAbortAPI
			&& hostAPI->mStandardHandlerAPI );
}

static bool CheckAPICompatibility_V4 ( const HostAPIRef hostAPI )
{
	return ( CheckAPICompatibility_V1( hostAPI )
			&& hostAPI->mRequestAPISuite != NULL );
}

// ============================================================================

bool SetHostAPI( HostAPIRef hostAPI )
{
	bool valid = false;
	if( hostAPI && hostAPI->mVersion > 0 )
	{
		if ( hostAPI->mVersion <= 3 )
		{
			// Old host API before plugin versioning changes
			valid = CheckAPICompatibility_V1( hostAPI );
		}
		else
		{
			// New host API including RequestAPISuite.
			// This version of the HostAPI struct should not be changed.
			valid = CheckAPICompatibility_V4( hostAPI );
		}
	}
	
	if( valid )
	{
		sHostAPI = hostAPI;
		sHostAPIVersion = hostAPI->mVersion;
	}
	
	return valid;
}

// ============================================================================

static HostAPIRef GetHostAPI()
{
	return sHostAPI;
}

// ============================================================================

static XMP_Uns32 GetHostAPIVersion()
{
	return sHostAPIVersion;
}

// ============================================================================

inline void CheckError( WXMP_Error & error )
{
	if( error.mErrorID != kXMPErr_NoError )
	{
		throw XMP_Error( error.mErrorID, error.mErrorMsg );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// class IOAdapter
//

XMP_Uns32 IOAdapter::Read( void* buffer, XMP_Uns32 count, bool readAll ) const
{
	WXMP_Error error;
	XMP_Uns32 result;
	GetHostAPI()->mFileIOAPI->mReadProc( this->mFileRef, buffer, count, readAll, result, &error );
	CheckError( error );
	return result;
}

// ============================================================================

void IOAdapter::Write( void* buffer, XMP_Uns32 count ) const
{
	WXMP_Error error;
	GetHostAPI()->mFileIOAPI->mWriteProc( this->mFileRef, buffer, count, &error );
	CheckError( error );
}

// ============================================================================

void IOAdapter::Seek( XMP_Int64& offset, SeekMode mode ) const
{
	WXMP_Error error;
	GetHostAPI()->mFileIOAPI->mSeekProc( this->mFileRef, offset, mode, &error );
	CheckError( error );
}

// ============================================================================

XMP_Int64 IOAdapter::Length() const
{
	WXMP_Error error;
	XMP_Int64 length = 0;
	GetHostAPI()->mFileIOAPI->mLengthProc( this->mFileRef, length, &error );
	CheckError( error );
	return length;
}

// ============================================================================

void IOAdapter::Truncate( XMP_Int64 length ) const
{
	WXMP_Error error;
	GetHostAPI()->mFileIOAPI->mTruncateProc( this->mFileRef, length, &error );
	CheckError( error );
}

// ============================================================================

XMP_IORef IOAdapter::DeriveTemp() const
{
	WXMP_Error error;
	XMP_IORef tempIO;
	GetHostAPI()->mFileIOAPI->mDeriveTempProc( this->mFileRef, tempIO, &error );
	CheckError( error );
	return tempIO;
}

// ============================================================================

void IOAdapter::AbsorbTemp() const
{
	WXMP_Error error;
	GetHostAPI()->mFileIOAPI->mAbsorbTempProc( this->mFileRef, &error );
	CheckError( error );
}

// ============================================================================

void IOAdapter::DeleteTemp() const
{
	WXMP_Error error;
	GetHostAPI()->mFileIOAPI->mDeleteTempProc( this->mFileRef, &error );
	CheckError( error );
}

///////////////////////////////////////////////////////////////////////////////
//
// Host Strings
//

StringPtr HostStringCreateBuffer( XMP_Uns32 size )
{
	WXMP_Error error;
	StringPtr buffer = NULL;
	GetHostAPI()->mStrAPI->mCreateBufferProc( &buffer, size, &error );
	CheckError( error );
	return buffer;
}

// ============================================================================

void HostStringReleaseBuffer( StringPtr buffer )
{
	WXMP_Error error;
	GetHostAPI()->mStrAPI->mReleaseBufferProc( buffer, &error );
	CheckError( error );
}

///////////////////////////////////////////////////////////////////////////////
//
// Abort functionality
//

bool CheckAbort( SessionRef session )
{
	WXMP_Error error;
	XMP_Bool abort = false;
	GetHostAPI()->mAbortAPI->mCheckAbort( session, &abort, &error );

	if( error.mErrorID == kXMPErr_Unavailable )
	{
		abort = false;
	}
	else if( error.mErrorID != kXMPErr_NoError )
	{
		throw XMP_Error( error.mErrorID, error.mErrorMsg );
	}

	return ConvertXMP_BoolToBool( abort );
}

///////////////////////////////////////////////////////////////////////////////
//
// Standard file handler access
//

bool CheckFormatStandard( SessionRef session, XMP_FileFormat format, const StringPtr path )
{
	WXMP_Error error;
	XMP_Bool ret = true;
	if ( sStandardHandler_V3 == NULL )
	{
		throw XMP_Error( kXMPErr_Unavailable, "StandardHandler suite unavailable" );
	}
	sStandardHandler_V3->mCheckFormatStandardHandler( session, format, path, ret, &error );

	if( error.mErrorID != kXMPErr_NoError )
	{
		throw XMP_Error( error.mErrorID, error.mErrorMsg );
	}

	return ConvertXMP_BoolToBool( ret );
}

// ============================================================================

bool GetXMPStandard( SessionRef session, XMP_FileFormat format, const StringPtr path, std::string& xmpStr, bool* containsXMP , XMP_OptionBits flags /*= NULL*/, std::string *packet /*= NULL*/ , XMP_PacketInfo *packetInfo /*= NULL*/, ErrorCallbackInfo * errorCallback /*= NULL*/, XMP_ProgressTracker::CallbackInfo * progCBInfoPtr /*= NULL*/ )
{
	WXMP_Error error;
	bool ret = true;
	XMP_StringPtr outXmp= NULL;
	XMP_Bool cXMP = kXMP_Bool_False;
	if ( sStandardHandler_V3 == NULL )
	{
		throw XMP_Error( kXMPErr_Unavailable, "StandardHandler suite unavailable" );
	}

	XMP_StringPtr inXMP = NULL;
	ErrorCallbackBox * errorCallbackBox = NULL;
	if( errorCallback != NULL && errorCallback->wrapperProc != 0 )
		errorCallbackBox = new ErrorCallbackBox( errorCallback->wrapperProc, errorCallback->clientProc, errorCallback->context, errorCallback->limit );
	sStandardHandler_V3->mGetXMPwithPacketStandardHandlerWOptions( session, format, path, &outXmp, &cXMP, &error, flags, &inXMP, packetInfo, errorCallbackBox, progCBInfoPtr );
	*containsXMP = ConvertXMP_BoolToBool( cXMP );

	if( error.mErrorID == kXMPErr_NoFileHandler || error.mErrorID == kXMPErr_BadFileFormat)
	{
		ret = false;
	}
	else if( error.mErrorID != kXMPErr_NoError )
	{
		throw XMP_Error( error.mErrorID, error.mErrorMsg );
	}
	xmpStr = outXmp;
	HostStringReleaseBuffer( (StringPtr)outXmp ) ;
	
	if ( inXMP != NULL )
	{
		if ( packet != NULL )
			packet->assign(inXMP);
		HostStringReleaseBuffer( (StringPtr)inXMP ) ;
	}
	if ( errorCallbackBox != NULL )
		delete errorCallbackBox ;
	return ret;
}

// ============================================================================

bool PutXMPStandard( SessionRef session, XMP_FileFormat format, const StringPtr path, const XMP_StringPtr xmpStr, XMP_OptionBits flags /*= NULL */, ErrorCallbackInfo * errorCallback /*= NULL */, XMP_ProgressTracker::CallbackInfo * progCBInfoPtr /*= NULL*/ )
{
	WXMP_Error error;
	bool ret = true;
	
	if ( sStandardHandler_V3 == NULL )
	{
		throw XMP_Error( kXMPErr_Unavailable, "StandardHandler suite unavailable" );
	}

	ErrorCallbackBox * errorCallbackBox = NULL;
	if( errorCallback != NULL && errorCallback->wrapperProc != 0 )
		errorCallbackBox = new ErrorCallbackBox( errorCallback->wrapperProc, errorCallback->clientProc, errorCallback->context, errorCallback->limit );
	sStandardHandler_V3->mPutXMPStandardHandler( session, format, path, xmpStr, &error, flags, errorCallbackBox, progCBInfoPtr );

	if( error.mErrorID == kXMPErr_NoFileHandler || error.mErrorID == kXMPErr_BadFileFormat)
	{
		ret = false;
	}
	else if( error.mErrorID != kXMPErr_NoError )
	{
		throw XMP_Error( error.mErrorID, error.mErrorMsg );
	}
	if ( errorCallbackBox != NULL )
		delete errorCallbackBox ;
	return ret;
}

// ============================================================================

bool GetFileModDateStandardHandler( SessionRef session, XMP_FileFormat format, StringPtr path, XMP_DateTime * modDate, XMP_Bool * isSuccess, XMP_OptionBits flags /*= NULL*/ )
{
	WXMP_Error error;
	bool ret = true;
	
	if ( sStandardHandler_V3 == NULL )
	{
		throw XMP_Error( kXMPErr_Unavailable, "StandardHandler suite unavailable" );
	}

	sStandardHandler_V3->mGetFileModDateStandardHandler( session, format, path, modDate, isSuccess, &error, flags );

	if( error.mErrorID == kXMPErr_NoFileHandler || error.mErrorID == kXMPErr_BadFileFormat)
	{
		ret = false;
	}
	else if( error.mErrorID != kXMPErr_NoError )
	{
		throw XMP_Error( error.mErrorID, error.mErrorMsg );
	}
	return ret;
}

// ============================================================================

static void SetClientStringVector ( void * clientPtr, XMP_StringPtr * arrayPtr, XMP_Uns32 stringCount )
{
	std::vector<std::string> * vecPtr = (std::vector<std::string> *) (clientPtr);
	vecPtr->clear();
	vecPtr->reserve( stringCount );
	for ( XMP_Uns32 i = 0; i < stringCount; i++ )
	{
		vecPtr->push_back( arrayPtr[i] );
	}
}

// ============================================================================

bool GetAssociatedResourcesStandardHandler( SessionRef session, XMP_FileFormat format, StringPtr path, std::vector<std::string> * resourceList, XMP_OptionBits flags /*= NULL*/ )
{
	WXMP_Error error;
	bool ret = true;
	
	if ( sStandardHandler_V3 == NULL )
	{
		throw XMP_Error( kXMPErr_Unavailable, "StandardHandler suite unavailable" );
	}
	sStandardHandler_V3->mGetAssociatedResourcesStandardHandler( session, format, path, resourceList, SetClientStringVector, &error, flags );

	if( error.mErrorID == kXMPErr_NoFileHandler || error.mErrorID == kXMPErr_BadFileFormat)
	{
		ret = false;
	}
	else if( error.mErrorID != kXMPErr_NoError )
	{
		throw XMP_Error( error.mErrorID, error.mErrorMsg );
	}
	return ret;
}

// ============================================================================

bool IsMetadataWritableStandardHandler( SessionRef session, XMP_FileFormat format, StringPtr path, XMP_Bool * isWritable, XMP_OptionBits flags /*= NULL*/ )
{
	WXMP_Error error;
	bool ret = true;
	
	if ( sStandardHandler_V3 == NULL )
	{
		throw XMP_Error( kXMPErr_Unavailable, "StandardHandler suite unavailable" );
	}
	sStandardHandler_V3->mIsMetadataWritableStandardHandler( session, format, path, isWritable, &error, flags );

	if( error.mErrorID == kXMPErr_NoFileHandler || error.mErrorID == kXMPErr_BadFileFormat)
	{
		ret = false;
	}
	else if( error.mErrorID != kXMPErr_NoError )
	{
		throw XMP_Error( error.mErrorID, error.mErrorMsg );
	}
	return ret;
}

// ============================================================================

void* RequestAPISuite( const char* apiName, XMP_Uns32 apiVersion )
{
	void* suite = NULL;
	
	WXMP_Error error;
	
	if (GetHostAPIVersion() >= 4)
	{
		GetHostAPI()->mRequestAPISuite( apiName, apiVersion, &suite, &error );
		CheckError(error);
	}
	else
	{
		throw XMP_Error( kXMPErr_Unavailable, "RequestAPISuite unavailable (host too old)" );
	}
	

	return suite;
}

// ============================================================================

} //namespace XMP_PLUGIN
