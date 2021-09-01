#ifndef XMPCoreLatestInterfaceVersions_h__
#define XMPCoreLatestInterfaceVersions_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

//!
//! @brief Macro to include a client file through with client can control the interface versions he wants to stick with
//! if not the latest ones.
//!
#if !SOURCE_COMPILING_XMPCORE_LIB
	#ifdef XMPCORE_CLIENT_VERSION_NUMBER_FILE
		#include QUOTEME(XMPCORE_CLIENT_VERSION_NUMBER_FILE)
	#endif
#endif

#ifndef INAMESPACEPREFIXMAP_VERSION
	#define INAMESPACEPREFIXMAP_VERSION 1
#endif

#ifndef IPATHSEGMENT_VERSION
	#define IPATHSEGMENT_VERSION 1
#endif

#ifndef IPATH_VERSION
	#define IPATH_VERSION 1
#endif

#ifndef INODE_VERSION
	#define INODE_VERSION 1
#endif

#ifndef INODEITERATOR_VERSION
	#define INODEITERATOR_VERSION 1
#endif

#ifndef ISIMPLENODE_VERSION
	#define ISIMPLENODE_VERSION 1
#endif

#ifndef ICOMPOSITENODE_VERSION
	#define ICOMPOSITENODE_VERSION 1
#endif

#ifndef ISTRUCTURENODE_VERSION
	#define ISTRUCTURENODE_VERSION 1
#endif

#ifndef IARRAYNODE_VERSION
	#define IARRAYNODE_VERSION 1
#endif

#ifndef IMETADATA_VERSION
	#define IMETADATA_VERSION 1
#endif

#ifndef ICLIENTDOMPARSER_VERSION
	#define ICLIENTDOMPARSER_VERSION 1
#endif

#ifndef ICLIENTDOMSERIALIZER_VERSION
	#define ICLIENTDOMSERIALIZER_VERSION 1
#endif

#ifndef IDOMPARSER_VERSION
	#define IDOMPARSER_VERSION 1
#endif

#ifndef IDOMSERIALIZER_VERSION
	#define IDOMSERIALIZER_VERSION 1
#endif

#ifndef IDOMIMPLEMENTATIONREGISTRY_VERSION
	#define IDOMIMPLEMENTATIONREGISTRY_VERSION 1
#endif

#ifndef ICOREOBJECTFACTORY_VERSION
	#define ICOREOBJECTFACTORY_VERSION 1
#endif

#ifndef ICORECONFIGURATIONMANAGER_VERSION
	#define ICORECONFIGURATIONMANAGER_VERSION 1
#endif

#ifndef IMETADATACONVERTERUTILS_VERSION
    #define IMETADATACONVERTERUTILS_VERSION 1
#endif


#endif  // XMPCoreLatestInterfaceVersions_h__

