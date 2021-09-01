// =================================================================================================
// Copyright Adobe
// Copyright 2011 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

/**************************************************************************
* @file PluginBase.h
* @brief Basic functionality of the plugin.
*
* All plugin should be derived from PluginBase. This is the basic miinimum 
* functionalty which a  plugin should provide.
* 
***************************************************************************/

#ifndef PLUGINBASE_H
#define PLUGINBASE_H


#include <cstring>
#include <vector>
#include <string>
#include "HostAPIAccess.h"
#include "PluginUtils.h"
#define TXMP_STRING_TYPE std::string
#include "XMP.hpp"
#include "source/XMP_ProgressTracker.hpp"

namespace XMP_PLUGIN
{

/** Base class for the plugin file handler. All file handler should be derived from PluginBase, 
    which is an abstract class. Plugin developer need to implement all of the virtual functions. 

	virtual void cacheFileData( const IOAdapter& file, std::string& xmpStr );
	virtual void updateFile( const IOAdapter& file, bool doSafeUpdate, const std::string& xmpStr );
	virtual void writeTempFile( const IOAdapter& srcFile, const IOAdapter& tmpFile, const std::string& xmpStr ) ;
	virtual void importToXMP( XMP_StringPtr* xmpStr, XMP_StringPtr* oldPtr = 0, XMP_PacketInfo * packetInfo = 0 );
	virtual void exportFromXMP( XMP_StringPtr xmpStr  );

    First two functions are pure virtual functions so these should be implemented for sure. 
    Last three functions(eg. writeTempFile, importToXMP, exportToXMP) may not be required by the file
    handler so plug-in developer may want to skip these functions. 

    Plugin developer also need to implement following static functions.

	static bool initialize();
	static bool terminate();
	static bool checkFileFormat( const std::string& filePath, const HostFileSys& file );
	static inline bool checkFolderFormat( const std::string& rootPath, const std::string& gpName, const std::string& parentName, const std::string& leafName );

	Though file handler may need only one of the last two function but both of them need to be implemented.
	One function which is actually required, should be implemented properly and the other function will just return false.
 */

/** @class PluginBase
 *  @brief Base class of the plugin.
 *
 *  All plugin should be derived from PluginBase. This is the basic minimum 
 *  functionality which a file handler should provide.
 */

class PluginBase
{
public:

	PluginBase( const std::string& filePath, XMP_Uns32 openFlags, XMP_Uns32 format = 0, XMP_Uns32 handlerFlags = 0 ) 
		: mPath( filePath ),mHandlerFlags(handlerFlags), mOpenFlags( openFlags ), mFormat( format ), mErrorCallback()	{}
#if WIN_ENV
	virtual ~PluginBase() NO_EXCEPT_FALSE {}
#else
	virtual ~PluginBase() {}
#endif
	/** @brief Delegator functions which will eventually call the corresponding virtual function.
	 */
	void cacheFileData( XMP_IORef fileRef, XMP_StringPtr* xmpStr );
	void updateFile( XMP_IORef fileRef, bool doSafeUpdate, XMP_StringPtr xmpStr );
	void writeTempFile( XMP_IORef srcFileRef, XMP_IORef fileRef, XMP_StringPtr xmpStr );
	void FillMetadataFiles( StringVectorRef metadataFiles, SetStringVectorProc SetStringVector );
	void FillAssociatedResources( StringVectorRef resourceList, SetStringVectorProc SetStringVector );
	/** @brief Check format with standard file handler
	*
	* Call the standard file handler to check the format of the data source.
	* This call expects that this is a replacement file handler. Otherwise
	* the call fails with an exception.
	*
	* @param path	Pointer to the path string of the file to be checked. Pass NULL to check the
	*				file passed in during initialization (see PluginBase::getPath() )
	* @return		true on success
	*/
	bool checkFormatStandard( const std::string* path = NULL );

	/** @brief Get metadata from standard file handler
	 *
	 * Call the standard file handler in order to retrieve XMP from it.
	 * This call expects that session refers to a replacement file handler. Otherwise
	 * this call fails with an exception.
	 * Calls checkFormatStandard internally before calling the actual standard handler.
	 *
	 * @param xmpStr		Serialized to XMP packet. Will be populated with XMP read from standard handler.
	 * @param path			Pointer to the path string of the file to be checked. Pass NULL to check the
	 *						file passed in during initialization (see PluginBase::getPath() )
	 * @param containsXMP	Returns true if the standard handler detected XMP
	 * @return				true on success
	 */
	bool getXMPStandard( std::string& xmpStr, const std::string* path = NULL, bool* containsXMP = NULL );

	/** @brief Get metadata from standard file handler
	 *
	 * Call the standard file handler in order to retrieve XMP from it.
	 * This call expects that session refers to a replacement file handler. Otherwise
	 * this call fails with an exception.
	 * Calls checkFormatStandard internally before calling the actual standard handler.
	 *
	 * @param xmpStr			Serialized to XMP packet. Will be populated with XMP read from standard handler.
 	 * @param flags				OpenFlags passed during opening a file
	 * @param path				Pointer to the path string of the file to be checked. Pass NULL to check the
	 *							file passed in during initialization (see PluginBase::getPath() )
	 * @param containsXMP		Returns true if the standard handler detected XMP
	 * @param packet			Returns existed XMP packet present in the file, if available
	 * @param packetInfo		Returns packet information of existed XMP packet in the file, if available
	 * @param errorCallback		Contains pointer to ErrorCallbackInfo
	 * @param progCBInfoPtr		Contains pointer to the CallbackInfo provided by user if any
	 * @return					true on success
	 */
	bool getXMPStandard( std::string& xmpStr, XMP_OptionBits flags, const std::string* path = NULL, bool* containsXMP = NULL, std::string *packet = NULL, XMP_PacketInfo* packetInfo = NULL, ErrorCallbackInfo * errorCallback = NULL, XMP_ProgressTracker::CallbackInfo * progCBInfoPtr = NULL ); 

	/** @brief Putting metadata into file using standard file handler
	 *
	 * Call the standard file handler in order to put XMP into it.
	 * This call expects that session refers to a replacement file handler. Otherwise
	 * this call fails with an exception.
	 * Calls checkFormatStandard internally before calling the actual standard handler.
	 *
	 * @param xmpStr			Serialized to XMP packet. Will be put into file using standard handler.
  	 * @param flags				OpenFlags passed during opening a file
	 * @param path				Pointer to the path string of the file to be checked. Pass NULL to check the
	 *							file passed in during initialization (see PluginBase::getPath() )
	 * @param errorCallback		Contains pointer to ErrorCallbackInfo
	 * @param progCBInfoPtr		Contains pointer to the CallbackInfo provided by user if any
	 * @return					true on success
	 */
	bool putXMPStandard( const XMP_StringPtr xmpStr, XMP_OptionBits flags = NULL, const std::string* path = NULL, ErrorCallbackInfo * errorCallback = NULL, XMP_ProgressTracker::CallbackInfo * progCBInfoPtr = NULL );

	/** @brief Getting file modification date from standard file handler
	 *
	 * Call the standard file handler in order to retrieve file modification date from it.
	 *
	 * @param modDate		will contain modification date of file obtained from the standard Handler
	 * @param isSuccess		Returns true if the standard handler detected file modification date 
	 * @param flags			OpenFlags passed during opening a file
	 * @param path			Path to the file that should be proceeded
	 * @return				true on success
	 */
	bool getFileModDateStandardHandler( XMP_DateTime * modDate, XMP_Bool * isSuccess, XMP_OptionBits flags = NULL, const std::string* path = NULL );
	
	/** @brief Getting associated resources from standard file handler
	 *
	 * Call the standard file handler in order to retrieve file associated resources from it.
	 *
	 * @param resourceList	will contain resources associated with the file obtained from the standard Handler
	 * @param flags			OpenFlags passed during opening a file
	 * @param path			Path to the file that should be proceeded
	 * @return				true on success
	 */
	bool getAssociatedResourcesStandardHandler( std::vector<std::string> * resourceList, XMP_OptionBits flags = NULL, const std::string* path = NULL );

	/** @brief Checking whether metadata is writable or not into the file from standard file handler
	 *
	 * Call the standard file handler in order to check whether the metadata is writable or not into the file.
	 *
	 * @param isWritable	Returns true if the standard handler can write on the file.
	 * @param flags			OpenFlags passed during opening a file
	 * @param path			Path to the file that should be proceeded
	 * @return				true on success
	 */
	bool isMetadataWritableStandardHandler( XMP_Bool * isWritable, XMP_OptionBits flags = NULL, const std::string* path = NULL );

	/** @brief Get file format
	 *
	 * Get the file format of this handler. 
	 * The format is defined in the plugin manifest.
	 */
	inline XMP_FileFormat getFormat() const					{ return mFormat; }

	/** @brief Get handler flags
	 *
	 * Get the handler flags for the file handler related to the current format.
	 * The flags are defined in the plugin manifest.
	 */
	inline XMP_OptionBits getHandlerFlags() const			{ return mHandlerFlags; }

	/** @brief Return the path to the input file/folder
	 *
	 *  Return the path to the input file/folder. Empty if the data source is neither
	 *  a file nor folder.
	 *  @return Path string
	 */
	inline const std::string& getPath() const { return mPath; }

	/** @brief Return the open flags.
	 *
	 *  Return the flags that describe the desired access
	 *  @return flags
	 */
	inline XMP_OptionBits getOpenFlags() const { return mOpenFlags; }

	inline ErrorCallbackInfo * getErrorCallbackInfo() { return &mErrorCallback;	}

	/** @brief Ask XMPFiles if current operation should be aborted.
	 *
	 *  @param doAbort	If true and the operation should be aborted then
	 *					abort it by throwing an exception
	 *  @return true if the current operation should be aborted
	 */
	bool checkAbort( bool doAbort = false );

	/** @brief Return the modification date/time of the metadata file
	 *
	 * The purpose of the method getFileModDate is to return the most recent file system modification timestamp 
	 * for any associated file containing metadata, XMP or non-XMP. In the case of a typical single file with 
	 * embedded metadata this is that one file. In the case of embedded non-XMP and sidecar XMP, this is the more 
	 * recent of both files.
	 * 
	 * The default implementation only cares about the case of a single file with embedded metadata.
	 * All other cases will fail and return false.
	 *
	 * @param modDate	A required pointer to return the modification date.
	 * @return			True if a modification date could be determined
	 *
	 */
	virtual bool getFileModDate ( XMP_DateTime* modDate );
	virtual void FillMetadataFiles ( std::vector<std::string> * metadataFiles );

	 /** @brief Return the list of all resources associated to the opened filePath
	 *
	 * The purpose of the method FillAssociatedResources is to return all files, XMP or non-XMP, 
	 * that are associated to the current opened filePath.In the case of a typical single file with 
	 * embedded metadata this is that one file. In the simple sidecar case one or two paths will be 
     * returned, one if there is no sidecar XMP and two if sidecar XMP exists. For folder-based 
     * handlers paths to all associated files is returned, including the files and folders necessary
     * to identify the format.In general, all the returned paths are existent.
	 * 
	 * The default implementation only cares about the case of a single file with embedded metadata.
	 * All other cases will fail and return false.
	 *
	 * @param resourceList	A required pointer to return the list of all associated resources.
	 *
	 */
	virtual void FillAssociatedResources ( std::vector<std::string> * resourceList );

	
	 /** @brief Returns true if the Metadata can be updated for the opened filePath
	 *
	 * The purpose of the method IsMetadataWritable is to to check if metadata can be updated or 
	 * written to the format. In the case of folder-based video formats only if all the metadata  
	 * files can be written to, true is returned.In other words, false is returned for a 
     * partial-write state of metadata files in folder-based media formats. 
	 * 
	 * The default implementation only cares about the case of a single file with embedded metadata.
	 * All other cases will fail and return false.
	 *
	 * @return	True if the metadata can be updated, otherwise false
	 *
	 */
	virtual bool IsMetadataWritable ( );

	/** Virtual functions which need to be implemented by the plugin Developer in the derived class. 
	 */
	virtual void cacheFileData( const IOAdapter& file, std::string& xmpStr ) = 0;
	virtual void updateFile( const IOAdapter& file, bool doSafeUpdate, const std::string& xmpStr ) = 0;
	virtual void writeTempFile( const IOAdapter& srcFile, const IOAdapter& tmpFile, const std::string& xmpStr ) {}
	virtual void importToXMP( XMP_StringPtr* xmpStr, XMP_StringPtr* oldPtr , XMP_PacketInfo * packetInfo );
	virtual void exportFromXMP( XMP_StringPtr xmpStr );
	virtual void importToXMP( XMP_StringPtr* xmpStr );
	virtual void SetErrorCallback( XMPFiles_ErrorCallbackWrapper wrapperProc, XMPFiles_ErrorCallbackProc clientProc, void * context, XMP_Uns32 limit );
	virtual void SetProgressCallback( XMP_ProgressTracker::CallbackInfo * progCBInfoPtr ) {}
	
private:
	std::string		mPath;
	XMP_OptionBits	mHandlerFlags;
	XMP_OptionBits	mOpenFlags;
	XMP_FileFormat	mFormat;
	ErrorCallbackInfo mErrorCallback;
};


/** External functions which need to be implemented by Plugin Developer.
 *  This function are unique for a plugins. Unlike PluginBase class which is used 
 *  for each file handler, these function are written only once for each plug-in.
 */
 
/** @brief Returns the unique identifier string of the plug-in.
 *
 *  Return the uid string of the plugin. This string should match with the string which is present in
 *  plugins resource file "MODULE_IDENTIFIER.txt".
 */
const char* GetModuleIdentifier();

/** @brief Do additional steps to setup the plugin during plugin initialization.
 *
 *  This function will be called during initialization of the plugin and can be
 *  used to request additional host API suites using RequestAPISuite().
 *  The plugin initialization will be aborted if false is returned.
 */
bool SetupPlugin();

/** @brief Register the file handlers available in the plug-in. 
 *
 *  This function need to be implemented by plugin-developer. This function register the file handlers available in the plugin. 
 *  Plug-in developer may want to register only few of the available plug-ins as per requirement. 
 *  Only registered plug-in will be loaded by the XMPFiles.
 */
void RegisterFileHandlers();
	
} //namespace XMP_PLUGIN
#endif // PLUGINBASE_H

