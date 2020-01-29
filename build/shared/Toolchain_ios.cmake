# =================================================================================================
# ADOBE SYSTEMS INCORPORATED
# Copyright 2013 Adobe Systems Incorporated
# All Rights Reserved
#
# NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
# of the Adobe license agreement accompanying it.
# =================================================================================================
# Toolchain 

# This file is based off of the Platform/Darwin.cmake and Platform/UnixPaths.cmake
# files which are included with CMake 2.8.4
# It has been altered for iOS development

# enforce build static libs on iOS
#set(XMP_BUILD_STATIC "ON")

# Standard settings
set (CMAKE_SYSTEM_NAME Darwin)
set (CMAKE_SYSTEM_VERSION 1 )
set (UNIX True)
set (APPLE True)
set (APPLE_IOS True)

# Skip the platform compiler checks for cross compiling
set (CMAKE_CXX_COMPILER_WORKS TRUE)
set (CMAKE_C_COMPILER_WORKS TRUE)

# All iOS/Darwin specific settings - some may be redundant
set (CMAKE_SHARED_LIBRARY_PREFIX "lib")
set (CMAKE_SHARED_LIBRARY_SUFFIX ".dylib")
set (CMAKE_SHARED_MODULE_PREFIX "lib")
set (CMAKE_SHARED_MODULE_SUFFIX ".so")
set (CMAKE_MODULE_EXISTS 1)
set (CMAKE_DL_LIBS "")
set(CMAKE_C_IMPLICIT_LINK_LIBRARIES "")
set(CMAKE_C_IMPLICIT_LINK_DIRECTORIES "")
set(CMAKE_C_IMPLICIT_LINK_FRAMEWORK_DIRECTORIES "")

set (CMAKE_C_OSX_COMPATIBILITY_VERSION_FLAG "-compatibility_version ")
set (CMAKE_C_OSX_CURRENT_VERSION_FLAG "-current_version ")
set (CMAKE_CXX_OSX_COMPATIBILITY_VERSION_FLAG "${CMAKE_C_OSX_COMPATIBILITY_VERSION_FLAG}")
set (CMAKE_CXX_OSX_CURRENT_VERSION_FLAG "${CMAKE_C_OSX_CURRENT_VERSION_FLAG}")

# Hidden visibilty is required for cxx on iOS 
set (CMAKE_C_FLAGS "")
set (CMAKE_CXX_FLAGS "-headerpad_max_install_names -fvisibility=hidden -fvisibility-inlines-hidden")

set (CMAKE_C_LINK_FLAGS "-Wl,-search_paths_first ${CMAKE_C_LINK_FLAGS}")
set (CMAKE_CXX_LINK_FLAGS "-Wl,-search_paths_first ${CMAKE_CXX_LINK_FLAGS}")
# Setting Standard C++ complier library as 'libstdc++' or 'libcpp' depending on the value of CMAKE_LIBCPP
if(CMAKE_LIBCPP)
	set (CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
else(CMAKE_LIBCPP)
	set (CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libstdc++")
endif(CMAKE_LIBCPP)
set (CMAKE_PLATFORM_HAS_INSTALLNAME 1)
set (CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "-dynamiclib -headerpad_max_install_names")
set (CMAKE_SHARED_MODULE_CREATE_C_FLAGS "-bundle -headerpad_max_install_names")
set (CMAKE_SHARED_MODULE_LOADER_C_FLAG "-Wl,-bundle_loader,")
set (CMAKE_SHARED_MODULE_LOADER_CXX_FLAG "-Wl,-bundle_loader,")
set (CMAKE_FIND_LIBRARY_SUFFIXES ".dylib" ".so" ".a")

# hack: if a new cmake (which uses CMAKE_INSTALL_NAME_TOOL) runs on an old build tree
# (where install_name_tool was hardcoded) and where CMAKE_INSTALL_NAME_TOOL isn't in the cache
# and still cmake didn't fail in CMakeFindBinUtils.cmake (because it isn't rerun)
# hardcode CMAKE_INSTALL_NAME_TOOL here to install_name_tool, so it behaves as it did before, Alex
if (NOT DEFINED CMAKE_INSTALL_NAME_TOOL)
	find_program(CMAKE_INSTALL_NAME_TOOL install_name_tool)
endif (NOT DEFINED CMAKE_INSTALL_NAME_TOOL)

# Setup iOS platform
if (NOT DEFINED IOS_PLATFORM)
	set (IOS_PLATFORM "OS")
endif (NOT DEFINED IOS_PLATFORM)
set (IOS_PLATFORM ${IOS_PLATFORM} CACHE STRING "Type of iOS Platform")

# Check the platform selection and setup for developer root
if (${IOS_PLATFORM} STREQUAL "OS")
	set (IOS_PLATFORM_LOCATION "iPhoneOS.platform")
elseif (${IOS_PLATFORM} STREQUAL "SIMULATOR")
	set (IOS_PLATFORM_LOCATION "iPhoneSimulator.platform")
else (${IOS_PLATFORM} STREQUAL "OS")
	message (FATAL_ERROR "Unsupported IOS_PLATFORM value selected. Please choose OS or SIMULATOR")
endif (${IOS_PLATFORM} STREQUAL "OS")

# get xcode version
execute_process(
		  COMMAND xcodebuild -version
		  OUTPUT_VARIABLE CMAKE_INSTALLED_XCODE_VERSION
		  OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REGEX MATCH "Xcode ([1-9][0-9].[0-9][.]?[0-9]?)" matches ${CMAKE_INSTALLED_XCODE_VERSION})
set(CMAKE_INSTALLED_XCODE_VERSION ${CMAKE_MATCH_1})

# Setup iOS developer location
if(${CMAKE_INSTALLED_XCODE_VERSION} LESS 4.3.0)
	set (CMAKE_IOS_DEVELOPER_ROOT "/Developer/Platforms/${IOS_PLATFORM_LOCATION}/Developer")
else()
	# get xcode installation path
	execute_process(
			  COMMAND xcode-select -print-path
			  OUTPUT_VARIABLE XMP_DEVELOPER_ROOT
			  OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	set(CMAKE_IOS_DEVELOPER_ROOT "${XMP_DEVELOPER_ROOT}/Platforms/${IOS_PLATFORM_LOCATION}/Developer")
endif()
set (CMAKE_IOS_DEVELOPER_ROOT ${CMAKE_IOS_DEVELOPER_ROOT} CACHE PATH "Location of iOS Platform")

# Find and use the most recent iOS sdk 
set(CMAKE_IOS_SDK_ROOT iphoneos)

set (CMAKE_IOS_SDK_ROOT ${CMAKE_IOS_SDK_ROOT} CACHE PATH "Location of the selected iOS SDK")
message (STATUS "CMAKE_IOS_SDK_ROOT ${CMAKE_IOS_SDK_ROOT}")

# Set the sysroot default to the most recent SDK
#set (CMAKE_OSX_SYSROOT ${CMAKE_IOS_SDK_ROOT} CACHE PATH "Sysroot used for iOS support")
#see https://cmake.org/pipermail/cmake/2017-January/064800.html for details
set (CMAKE_OSX_SYSROOT ${CMAKE_IOS_SDK_ROOT})

# set the architecture for iOS - using ARCHS_STANDARD_INCLUDING_64_BIT sets armv7 and arm64, using ARCHS_STANDARD_64_BIT sets arm64
# The other value that works is ARCHS_UNIVERSAL_IPHONE_OS but that sets armv7 only
if(XMP_BUILD_STATIC)
set (CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_INCLUDING_64_BIT)" CACHE STRING  "Build architecture for iOS")
else()
set (CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_64_BIT)" CACHE STRING  "Build architecture for iOS")
endif()

# Set the find root to the iOS developer roots and to user defined paths
set (CMAKE_FIND_ROOT_PATH ${CMAKE_IOS_DEVELOPER_ROOT} ${CMAKE_IOS_SDK_ROOT} ${CMAKE_PREFIX_PATH} CACHE STRING  "iOS find search path root")

# default to searching for frameworks first
set (CMAKE_FIND_FRAMEWORK FIRST)

# set up the default search directories for frameworks
set (CMAKE_SYSTEM_FRAMEWORK_PATH
	${CMAKE_IOS_SDK_ROOT}/System/Library/Frameworks
	${CMAKE_IOS_SDK_ROOT}/System/Library/PrivateFrameworks
	${CMAKE_IOS_SDK_ROOT}/Developer/Library/Frameworks
)

# the host system paths and the paths in CMAKE_FIND_ROOT_PATH will be searched.
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
