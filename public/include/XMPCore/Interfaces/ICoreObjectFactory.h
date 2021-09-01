#ifndef ICoreObjectFactory_h__
#define ICoreObjectFactory_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPCommon/Interfaces/IObjectFactory.h"

namespace AdobeXMPCore {

	//!
	//! @brief Version1 of a interface that represents a factory to create various artifacts of XMP DOM like array,
	//! structure, path etc.
	//!
	//! @details Provides all the functions to create instances of various artifacts of XMP DOM and return them as shared pointers
	//! to the clients. This is the interface through which clients of the library actually get access to all other interfaces.
	//!

	class XMP_PUBLIC ICoreObjectFactory_v1
		: public virtual IObjectFactory_v1
	{
	public:

		//!
		//! @brief Creates an empty name space prefix map.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \return A pointer to an empty \#INameSpacePrefixMap_v1 object.
		//!
		virtual pINameSpacePrefixMap_base APICALL CreateNameSpacePrefixMap( pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Provides the default mapping of prefix string and nameSpace strings used by XMPCore.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \return A pointer to const \#INameSpacePrefixMap_v1 object containing all the mappings used as default by the XMPCore.
		//!
		virtual pcINameSpacePrefixMap_base APICALL GetDefaultNameSpacePrefixMap( pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Creates a normal property path segment.These are essentially all properties (simple, struct and arrays).
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the property.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the property.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \return A pointer to const \#IPathSegment_v1.
		//!
		virtual pcIPathSegment_base APICALL CreatePropertyPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Creates an array index path segment that denotes a specific element of an array.
		//! @details Such segments do not have an own name and inherits the namespace from the Array property itself.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the property.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] index An object of type \#AdobeXMP::sizet containting the index of the array element.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \return A pointer to const \#IPathSegment_v1.
		//! \attention Throws \#AdobeXMP::pcIError in case
		//!		- pointers to const char buffers are NULL,
		//!		- their content is empty.
		//!
		virtual pcIPathSegment_base APICALL CreateArrayIndexPathSegment( const char * nameSpace, sizet nameSpaceLength, sizet index, pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Creates a Qualifier path segment, which behaves like a normal property
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the property.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the property.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \return A pointer to const \#IPathSegment_v1.
		//!
		virtual pcIPathSegment_base APICALL CreateQualifierPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Creates a path segment that selects a specific qualifier by its value.
		//! For example a specific language in a alternative array of languages.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the property.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the property.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] value Pointer to a constant char buffer containing value of the language (xml:lang)
		//! \param[in] valueLength Number of characters in value. In case value is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \return A pointer to const \#IPathSegment_v1.
		//!
		virtual pcIPathSegment_base APICALL CreateQualifierSelectorPathSegment( const char * nameSpace, sizet nameSpaceLength, const char * name,
			sizet nameLength, const char * value, sizet valueLength, pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Creates an empty IPath object.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \return A pointer to an empty \#IPath_v1 object
		//!
		virtual pIPath_base APICALL CreatePath( pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Creates a path from a char buffer which contains the serialized path.
		//! \param[in] path Pointer to a const char buffer containing serialized form of the path.
		//! \param[in] pathLength Number of characters in the path. In case path in null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] map A pointer to a const \#IXMPNameSpacePrefixMap_v1 object which will contain the mapping for nameSpaces to prefixes.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \return A pointer to a \#IPath_v1 object.
		//!
		virtual pIPath_base APICALL ParsePath( const char * path, sizet pathLength, pcINameSpacePrefixMap_base map, pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Creates a simple property node which is not part of any metadata document.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the simple node.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the simple node.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] value Pointer to a constant char buffer containing value of the simple node.
		//! \param[in] valueLength Number of characters in value. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \return A pointer to a \#ISimpleNode_v1 object.
		//!
		virtual pISimpleNode_base APICALL CreateSimpleNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength,
			const char * value, sizet valueLength, pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Creates an array node which is not part of any metadata document.
		//! \param[in] arrayForm A value indicating the array type
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the array node.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the array node.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \return A pointer to a \#IArrayNode_v1 object.
		//!
		virtual pIArrayNode_base APICALL CreateArrayNode( uint32 arrayForm, const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Creates a structure node which is not part of any metadata document.
		//! \param[in] nameSpace Pointer to a constant char buffer containing name space URI of the structure node.
		//! \param[in] nameSpaceLength Number of characters in nameSpace. In case nameSpace is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[in] name Pointer to a constant char buffer containing local name of the structure node.
		//! \param[in] nameLength Number of characters in name. In case name is null terminated set it to \#AdobeXMPCommon::npos.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \return A pointer to a \#IStructureNode_v1 object.
		virtual pIStructureNode_base APICALL CreateStructureNode( const char * nameSpace, sizet nameSpaceLength, const char * name, sizet nameLength, pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Creates an empty IMetadata object.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \return a pointer to an empty \#IMetadata_v1 object.
		//!
		virtual pIMetadata_base APICALL CreateMetadata( pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Provides the reference to the database of Serializers and Parsers available with the library.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \return a pointer to \#IDOMImplementationRegistry_base object containing all the entries for serailizers and parsers.
		//!
		virtual pIDOMImplementationRegistry_base APICALL GetDOMImplementationRegistry( pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Provides access to the configuration manager of the library.
		//! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
		//! \return A pointer to \#ICoreConfigurationManager_base object.
		//!
		virtual pICoreConfigurationManager_base APICALL GetCoreConfigurationManager( pcIError_base & error ) __NOTHROW__ = 0;

		//!
        //! @brief Converts old xmp object to new xmp object.
        //! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
        //! \return A pointer to \#IMetadata_base object.
        //!
        virtual pIMetadata_base APICALL ConvertXMPMetatoIMetadata( XMPMetaRef xmpref, pcIError_base & error ) __NOTHROW__ = 0;
        
        //!
        //! @brief Converts new xmp object to old xmp object.
        //! \param[out] error A reference to a pointer to const IError object which will be filled with the error object in case of any error.
        //! \return XMPMetaRef.
        //!
        virtual XMPMetaRef APICALL ConvertIMetadatatoXMPMeta( pIMetadata iMeta, pcIError_base & error ) __NOTHROW__ = 0;
        

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Returns the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return Either a const or non const pointer to ICoreObjectFactory interface.
		//!
		virtual pICoreObjectFactory APICALL GetActualICoreObjectFactory() __NOTHROW__ = 0;
		XMP_PRIVATE pcICoreObjectFactory GetActualICoreObjectFactory() const __NOTHROW__ {
			return const_cast< ICoreObjectFactory_v1 * >( this )->GetActualICoreObjectFactory();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Returns the pointer to internal interfaces.
		//! \return Either a const or non const pointer to ICoreObjectFactory_I interface.
		//!
		virtual AdobeXMPCore_Int::pICoreObjectFactory_I APICALL GetICoreObjectFactory_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPCore_Int::pcICoreObjectFactory_I GetICoreObjectFactory_I() const __NOTHROW__ {
			return const_cast< ICoreObjectFactory_v1 * >( this )->GetICoreObjectFactory_I();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Converts raw pointer to base version to pointer to client interested version.
		//! @details The raw pointer is of version 1 interface where as the returned pointer depends on the version client is interested in.
		//! \return Pointer to const or non constant interface.
		//!
		XMP_PRIVATE static pICoreObjectFactory MakeCoreObjectFactory( pICoreObjectFactory_base ptr );
		XMP_PRIVATE static pcICoreObjectFactory MakeCoreObjectFactory( pcICoreObjectFactory_base ptr ) {
			return MakeCoreObjectFactory( const_cast< pICoreObjectFactory_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief Returns the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kICoreObjectFactoryID; }

		//!
		//! @brief returns the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

		//!
		//! @brief Gets an object of ICoreObjectFactory.
		//! \return A pointer to an ICoreObjectFactory object.
		//!
		XMP_PRIVATE static pICoreObjectFactory GetCoreObjectFactory();

		//!
		//! @{
		//! @brief Sets up the core object factory.
		//! \param[in] coreObjectFactory A pointer to an \#ICoreObjectFactory_v1 object.
		//! \note coreObjectFactory is an optional parameter and only required for clients who don't directly link with the library
		//!       but want to use its functionality.
		//!
	#if LINKING_XMPCORE_LIB
		XMP_PRIVATE static void SetupCoreObjectFactory();
	#else
		XMP_PRIVATE static void SetupCoreObjectFactory( pICoreObjectFactory_base coreObjectFactory );
	#endif
		//! @}

		//!
		//! @brief Destroy everything related to core object factory.
		//!
		XMP_PRIVATE static void DestroyCoreObjectFactory();

	protected:
		//!
		//! Destructor
		//! 
		virtual ~ICoreObjectFactory_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}
#endif // ICoreObjectFactory_h__
