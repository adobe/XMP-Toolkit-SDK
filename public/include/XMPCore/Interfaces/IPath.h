#ifndef __IPath_h__
#define __IPath_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"

namespace AdobeXMPCore {

	//!
	//! @brief Version1 of the interface that provides an easy iterative description of a specific path into the XMP tree.
	//! @details Path consists of multiple path segments in an order and each \#IPathSegment represents one segment
	//! of the path into the XMP tree.
	//! Provides all the functions to create path and get the various properties of a path.
	//! \attention Do not support multi-threading.
	//! \note Index in the path are 1-based.
	//!
	class XMP_PUBLIC IPath_v1
		: public virtual ISharedObject
		, public virtual IVersionable
	{
	public:

		//!
		//! @brief Registers a map of namespace and prefix with the object.
		//! This map will be used during serialization and parsing.
		//! \param[in] map A shared pointer of an object \#AdobeXMPCore::INameSpacePrefixMap.
		//! \return A shared pointer to the const map registered previously with the object.
		//!
		virtual spcINameSpacePrefixMap APICALL RegisterNameSpacePrefixMap( const spcINameSpacePrefixMap & map ) = 0;

		//!
		//! @brief Serializes the IPath object to a utf8 string representation. This will produce either a long form of the path using
		//! the full namespace strings or short form of the path using the prefix for the namespace.
		//! \param[in] map A shared pointer to a const \#AdobeXMPCore::INameSpacePrefixMap object which can contain the
		//! mapping for nameSpaces to prefixes. They will take precedence over the map registered with the object.
		//! \return A shard pointer to \#AdobeXMPCommon::IUTF8String object containing serialized form of path.
		//! \note In case map is not a valid shared pointer all the mappings will be picked from the map registered with the object.
		//!  If neither a map is registered nor it is provided in the arguments then it will serialize to long form of the path.
		//! \attention Error will be thrown in case
		//!		- no prefix exists for a namespace used in the path.
		//!
		virtual spIUTF8String APICALL Serialize( const spcINameSpacePrefixMap & map = spcINameSpacePrefixMap() ) const = 0;

		//!
		//! @brief Appends a path segment to the path.
		//! \param[in] segment	A shared pointer to a const \#AdobeXMPCore::IPathSegment object.
		//! \attention Error is thrown in case
		//!		- segment is not a valid shared pointer.
		//!
		virtual void APICALL AppendPathSegment( const spcIPathSegment & segment ) = 0;

		//!
		//! @brief Removes a path segment from the path.
		//! \param[in] index Indicates the index of the path segment to be removed.
		//! \returns A shared pointer to the const path segment removed.
		//! \attention Error is thrown in case
		//!		- index is out of bounds.
		//!
		virtual spcIPathSegment APICALL RemovePathSegment( sizet index ) = 0;

		//!
		//! @brief Gets the path segment at a particular index in the path
		//! \param[in] index Indicates the index for the path segment in the path.
		//! \return A shared pointer to a const path segment.
		//! \attention Error is thrown in case
		//!		-index is out of bounds.
		//!
		virtual spcIPathSegment APICALL GetPathSegment( sizet index ) const = 0;

		//!
		//! @brief Gets the number of the path segments in the path.
		//! \return The count of the path segments in the path.
		//!
		virtual sizet APICALL Size() const __NOTHROW__ = 0;

		//!
		//! @brief To check whether path is empty or not.
		//! \returns A bool object indicating true in case the path is empty (no path segment)
		//!
		XMP_PRIVATE bool IsEmpty() const {
			return Size() == 0;
		}

		//!
		//! @brief Clears the path by removing all the path segments from it
		//!
		virtual void APICALL Clear() __NOTHROW__ = 0;

		//!
		//! @brief Gets a new path having a selected range of path segments
		//! \param[in] startingIndex Indicates the starting index of the path segment to be part of the returned path object. Default value is 1.
		//! \param[in] countOfSegments Indicates the count of the path segments to be part of the returned path object starting from startingIndex.
		//!  Default value is \#AdobeXMPCommon::kMaxSize.
		//! \note In case countOfSegments is more than the number of segments available in the path object starting from the starting index
		//!  then internally it is truncated to the number of path segments available.
		//! \attention Error is thrown in case
		//!		- startingIndex is more than the count of segments in the object.
		//!
		virtual spIPath APICALL Clone( sizet startingIndex = 1, sizet countOfSegments = kMaxSize ) const = 0;

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Returns the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return Either a const or non const pointer to IPath interface.
		//!
		virtual pIPath APICALL GetActualIPath() __NOTHROW__ = 0;
		XMP_PRIVATE pcIPath GetActualIPath() const __NOTHROW__ {
			return const_cast< IPath_v1 * >( this )->GetActualIPath();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Returns the pointer to internal interfaces.
		//! \return Either a const or non const pointer to IPath_I interface.
		//!
		virtual AdobeXMPCore_Int::pIPath_I APICALL GetIPath_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPCore_Int::pcIPath_I GetIPath_I() const __NOTHROW__ {
			return const_cast< IPath_v1 * >( this )->GetIPath_I();
		}
		//!
		//! @}
		
		//!
		//! @{
		//! @brief Converts raw pointer to shared pointer. The raw pointer is of version 1 interface
		//! where as the returned shared pointer depends on the version client is interested in.
		//! \return Shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spIPath MakeShared( pIPath_base ptr );
		XMP_PRIVATE static spcIPath MakeShared( pcIPath_base ptr ) {
			return MakeShared( const_cast< pIPath_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief Returns the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIPathID; }

		//!
		//! @brief Returns the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

		// static factory functions

		//!
		//! @brief Creates an empty IPath object.
		//! \return a shared pointer to an empty IPath object
		//!
		XMP_PRIVATE static spIPath CreatePath();

		//!
		//! @brief Creates a path from a char buffer which contains the serialized path.
		//! \param[in] path Pointer to a const char buffer containing serialized form of the path.
		//! \param[in] pathLength Number of characters in the path. In case path in null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] map A shared pointer to a const \#AdobeXMPCore::IXMPNameSpacePrefixMap object which will contain the
		//!  mapping from nameSpaces to prefixes.
		//! \return A shared pointer to a \#AdobeXMPCore::IPath object.
		//! \note In case the serializedPath is NULL or the contents are empty then it will result in an empty path.
		//! \note This operation is currently not implemented for the IPath interface.
		//! \attention Error is thrown in case
		//!		- no mapping exists for a prefix to name space.
		//!		- path contains invalid data.
		//!
		XMP_PRIVATE static spIPath ParsePath( const char * path, sizet pathLength, const spcINameSpacePrefixMap & map );


	protected:
		//!
		//! Destructor
		//! 
		virtual ~IPath_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual pcINameSpacePrefixMap_base APICALL registerNameSpacePrefixMap( pcINameSpacePrefixMap_base map, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pIUTF8String_base APICALL serialize( pcINameSpacePrefixMap_base map, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual void APICALL appendPathSegment( pcIPathSegment_base segment, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pcIPathSegment_base APICALL removePathSegment( sizet index, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pcIPathSegment_base APICALL getPathSegment( sizet index, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pIPath_base APICALL clone( sizet startingIndex, sizet countOfSegemetns, pcIError_base & error ) const __NOTHROW__ = 0;


		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif  // __IPath_h__
