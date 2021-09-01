// =================================================================================================
// Copyright Adobe
// Copyright 2011 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "FileHandlerInstance.h"
#if EnablePluginManager
namespace XMP_PLUGIN
{

FileHandlerInstance::FileHandlerInstance ( SessionRef object, FileHandlerSharedPtr handler, XMPFiles * _parent ):
XMPFileHandler( _parent ), mObject( object ), mHandler( handler )
{
	this->handlerFlags = mHandler->getHandlerFlags();
	this->stdCharForm  = kXMP_Char8Bit;
	PluginManager::addHandlerInstance( this->mObject, this );
}

FileHandlerInstance::~FileHandlerInstance()
{
	WXMP_Error error;

	PluginManager::removeHandlerInstance(this->mObject);
	mHandler->getModule()->getPluginAPIs()->mTerminateSessionProc(this->mObject, &error);
	CheckError( error );
}

bool FileHandlerInstance::GetFileModDate ( XMP_DateTime * modDate )
{
	XMP_Bool ok;
	WXMP_Error error;
	GetSessionFileModDateProc wGetFileModDate = mHandler->getModule()->getPluginAPIs()->mGetFileModDateProc;
	wGetFileModDate ( this->mObject, &ok, modDate, &error );
	CheckError ( error );
	return ConvertXMP_BoolToBool( ok );
}

void FileHandlerInstance::CacheFileData()
{
	if( this->containsXMP ) return;
	
	WXMP_Error error;
	XMP_StringPtr xmpStr = NULL;
	mHandler->getModule()->getPluginAPIs()->mCacheFileDataProc( this->mObject, this->parent->ioRef, &xmpStr, &error );
	
	if( error.mErrorID != kXMPErr_NoError )
	{
		if ( xmpStr != 0 ) free( (void*) xmpStr );
		if ( error.mErrorID == kXMPErr_FilePermission )
			throw XMP_Error( kXMPErr_FilePermission, error.mErrorMsg );
		else
			throw XMP_Error( kXMPErr_InternalFailure, error.mErrorMsg );
	}

	if( xmpStr != NULL )
	{
		this->xmpPacket.assign( xmpStr );
		free( (void*) xmpStr ); // It should be freed as documentation of mCacheFileDataProc says so.
		this->containsXMP = true;
	}
	else
		this->containsXMP = false;
}

void FileHandlerInstance::ProcessXMP()
{
	if( this->processedXMP ) return;
	this->processedXMP = true;

	SXMPUtils::RemoveProperties ( &this->xmpObj, 0, 0, kXMPUtil_DoAllProperties );
	if ( this->xmpPacket.size() != 0 )
		this->xmpObj.ParseFromBuffer ( this->xmpPacket.c_str(), (XMP_StringLen)this->xmpPacket.size() );

	WXMP_Error error;
	if ( mHandler->getModule()->getPluginAPIs()->mVersion >= 4 && mHandler->getModule()->getPluginAPIs()->mImportToXMPStringWithPacketProc )
	{
		XMP_StringPtr xmpStr = this->xmpPacket.c_str();
		XMP_StringPtr oldPacketPtr = NULL;
		XMP_PacketInfo packetInfo;
		mHandler->getModule()->getPluginAPIs()->mImportToXMPStringWithPacketProc( this->mObject, &xmpStr, &error, &oldPacketPtr, &packetInfo );
		
		if( xmpStr != NULL && xmpStr != this->xmpPacket.c_str() )
		{
			XMP_StringLen newLen = static_cast<XMP_StringLen>(strlen( xmpStr ));
			this->xmpObj.Erase();
			this->xmpObj.ParseFromBuffer( xmpStr, newLen, 0 );
			
			// Note: Freeing memory would not create any problem as plugin would have allocated memory using Host library function
			free( ( void * ) xmpStr );
			this->containsXMP = true;
		}

		if( oldPacketPtr != NULL )
		{
			this->xmpPacket.resize( strlen( oldPacketPtr ) );
			this->xmpPacket.assign( oldPacketPtr );
			this->packetInfo = packetInfo;
			
			// Note: Freeing memory would not create any problem as plugin would have allocated memory using Host library function
			free( ( void * ) oldPacketPtr );
			this->containsXMP = true;
		}
	}
	else if( mHandler->getModule()->getPluginAPIs()->mVersion >= 2 && mHandler->getModule()->getPluginAPIs()->mImportToXMPStringProc )
	{
		XMP_StringPtr xmpStr = this->xmpPacket.c_str();
		mHandler->getModule()->getPluginAPIs()->mImportToXMPStringProc( this->mObject, &xmpStr, &error );
		
		if( xmpStr != NULL && xmpStr != this->xmpPacket.c_str() )
		{
			XMP_StringLen newLen = static_cast<XMP_StringLen>(strlen( xmpStr ));
			this->xmpObj.Erase();
			this->xmpObj.ParseFromBuffer( xmpStr, newLen, 0 );
			
			// Note: Freeing memory would not create any problem as plugin would have allocated memory using Host library function
			free( ( void * ) xmpStr );
			this->containsXMP = true;
		}
	}
	else
	{
		if( mHandler->getModule()->getPluginAPIs()->mImportToXMPProc )
			mHandler->getModule()->getPluginAPIs()->mImportToXMPProc( this->mObject, this->xmpObj.GetInternalRef(), &error );
		this->containsXMP = true;
	}
	CheckError( error );
}

void FileHandlerInstance::UpdateFile ( bool doSafeUpdate )
{
	bool optimizeFileLayout = XMP_OptionIsSet ( this->parent->openFlags, kXMPFiles_OptimizeFileLayout );
	this->needsUpdate |= optimizeFileLayout;
	if( !this->needsUpdate ) return;
	WXMP_Error error;

	if ( xmpPacket.size() != 0 )
	{
		if( mHandler->getModule()->getPluginAPIs()->mExportFromXMPStringProc )
		{
			std::string xmp;
			this->xmpObj.SerializeToBuffer( &xmp, kXMP_NoOptions, 0 );
			XMP_StringPtr xmpStr = xmp.c_str();
			mHandler->getModule()->getPluginAPIs()->mExportFromXMPStringProc( this->mObject, xmpStr, &error );
			if ( xmpStr != xmp.c_str() )
				this->xmpObj.SerializeToBuffer ( &this->xmpPacket, mHandler->getSerializeOption() );
		}
		else
		{
			if( mHandler->getModule()->getPluginAPIs()->mExportFromXMPProc )
			{
				mHandler->getModule()->getPluginAPIs()->mExportFromXMPProc( this->mObject, this->xmpObj.GetInternalRef(), &error );
				this->xmpObj.SerializeToBuffer ( &this->xmpPacket, mHandler->getSerializeOption() );
			}
		}
		CheckError( error );

		
	}
	mHandler->getModule()->getPluginAPIs()->mUpdateFileProc( this->mObject, this->parent->ioRef, doSafeUpdate, this->xmpPacket.c_str(), &error );
	CheckError( error );
	this->needsUpdate = false;
}

void FileHandlerInstance::WriteTempFile( XMP_IO* tempRef )
{
	WXMP_Error error;
	if( mHandler->getModule()->getPluginAPIs()->mExportFromXMPProc )
		mHandler->getModule()->getPluginAPIs()->mExportFromXMPProc( this->mObject, this->xmpObj.GetInternalRef(), &error );
	CheckError( error );

	this->xmpObj.SerializeToBuffer ( &this->xmpPacket, mHandler->getSerializeOption() );

	mHandler->getModule()->getPluginAPIs()->mWriteTempFileProc( this->mObject, this->parent->ioRef, tempRef, this->xmpPacket.c_str(), &error );
	CheckError( error );
}

static void SetStringVector ( StringVectorRef clientPtr, XMP_StringPtr * arrayPtr, XMP_Uns32 stringCount )
{
	std::vector<std::string>* clientVec = (std::vector<std::string>*) clientPtr;
	clientVec->clear();
	for ( XMP_Uns32 i = 0; i < stringCount; ++i ) {
		std::string nextValue ( arrayPtr[i] );
		clientVec->push_back ( nextValue );
	}
}

void FileHandlerInstance::FillMetadataFiles( std::vector<std::string> * metadataFiles )
{
	WXMP_Error error;
	FillMetadataFilesProc wFillMetadataFilesProc = mHandler->getModule()->getPluginAPIs()->mFillMetadataFilesProc;
	if ( wFillMetadataFilesProc ) {
		wFillMetadataFilesProc( this->mObject, metadataFiles, SetStringVector, &error);
		CheckError( error );
	} else {
		XMP_Throw ( "This version of plugin does not support FillMetadataFiles API", kXMPErr_Unimplemented );
	}
}

void FileHandlerInstance::FillAssociatedResources( std::vector<std::string> * resourceList )
{
	WXMP_Error error;
	FillAssociatedResourcesProc wFillAssociatedResourcesProc = mHandler->getModule()->getPluginAPIs()->mFillAssociatedResourcesProc;
	if ( wFillAssociatedResourcesProc ) {
		wFillAssociatedResourcesProc( this->mObject, resourceList, SetStringVector, &error);
		CheckError( error );
	} else {
		XMP_Throw ( "This version of plugin does not support FillAssociatedResources API", kXMPErr_Unimplemented );
	}
}

bool FileHandlerInstance::IsMetadataWritable( )
{
	WXMP_Error error;
	XMP_Bool result = kXMP_Bool_False;
	IsMetadataWritableProc wIsMetadataWritableProc = mHandler->getModule()->getPluginAPIs()->mIsMetadataWritableProc;
	if ( wIsMetadataWritableProc ) {
		wIsMetadataWritableProc( this->mObject, &result, &error);
		CheckError( error );
	} else {
		XMP_Throw ( "This version of plugin does not support IsMetadataWritable API", kXMPErr_Unimplemented );
	}
	return ConvertXMP_BoolToBool( result );
}

void FileHandlerInstance::SetErrorCallback ( ErrorCallbackBox errorCallbackBox )
{
	WXMP_Error error;
	SetErrorCallbackproc wSetErrorCallbackproc = mHandler->getModule()->getPluginAPIs()->mSetErrorCallbackproc ;
	if( wSetErrorCallbackproc )	{
		wSetErrorCallbackproc( this->mObject, errorCallbackBox, &error );
		CheckError( error );
	} else {
		XMP_Throw ( "This version of plugin does not support IsMetadataWritable API", kXMPErr_Unimplemented );
	}
}

void FileHandlerInstance::SetProgressCallback ( XMP_ProgressTracker::CallbackInfo * progCBInfoPtr )
{
	WXMP_Error error;
	SetProgressCallbackproc wSetProgressCallbackproc = mHandler->getModule()->getPluginAPIs()->mSetProgressCallbackproc ;
	if( wSetProgressCallbackproc )	{
		wSetProgressCallbackproc( this->mObject, progCBInfoPtr, &error );
		CheckError( error );
	} else {
		XMP_Throw ( "This version of plugin does not support IsMetadataWritable API", kXMPErr_Unimplemented );
	}
}

} //namespace XMP_PLUGIN
#endif
