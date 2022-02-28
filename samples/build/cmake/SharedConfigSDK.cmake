# =================================================================================================
# ADOBE SYSTEMS INCORPORATED
# Copyright 2013 Adobe Systems Incorporated
# All Rights Reserved
#
# NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
# of the Adobe license agreement accompanying it.
# =================================================================================================

# ==============================================================================
# define minimum cmake version
# For Android always build with make 3.6
if(ANDROID)
	cmake_minimum_required(VERSION 3.5.2)
else(ANDROID)
	cmake_minimum_required(VERSION 3.15.5)
endif(ANDROID)

# ==============================================================================
# Shared config for XMP Samples
# ==============================================================================


# Platform specific config
if(UNIX)
  if(APPLE)
	# OSX -------------------------------------------
	if(STATIC)
		set(BUILD_FOLDER "xcode")
		set(XMPCORE_LIB "XMPCore")
		set(XMPFILES_LIB "XMPFiles")
		set(LIB_EXT ".a")	
	else(STATIC)
		set(BUILD_FOLDER "xcode")
		set(XMPCORE_LIB "XMPCore")
		set(XMPFILES_LIB "XMPFiles")
		set(LIB_EXT ".framework")	
	endif(STATIC)
	add_definitions(-DMAC_ENV=1)	
	if(CMAKE_CL_64)
		set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)" CACHE STRING "Build architectures for OSX" FORCE)
		if(CMAKE_LIBCPP)
			set(PLATFORM_FOLDER "macintosh/universal")
		endif()
		add_definitions(-DXMP_64=1)
	endif(CMAKE_CL_64)
	# set specific SDK and compiler version
	if (NOT DEFINED OSX_DEVELOPER_ROOT)
		set(OSX_DEVELOPER_ROOT "/Applications/Xcode.app/Contents/Developer")
	endif()
	#check is the path to OSX_DEVELOPER_ROOT exists
	if(IS_DIRECTORY ${OSX_DEVELOPER_ROOT})
		set(CMAKE_OSX_SYSROOT "${OSX_DEVELOPER_ROOT}" )
	else()
		message ( FATAL_ERROR "ERROR: The sdk dir set is ${OSX_DEVELOPER_ROOT}, but it does not exist. See readme.txt for resolution")
	endif()	
	message ( STATUS "OSX SDK is set to ${CMAKE_OSX_SYSROOT}")
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9")
	set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
	
    # set Warning flags
    set(CMAKE_CXX_FLAGS "-Wno-trigraphs -Wnon-virtual-dtor -Woverloaded-virtual -Wmissing-braces -Wparentheses -Wswitch -Wunused-value -Wunknown-pragmas -Wsign-compare -Wreturn-type")
	# set Compiler build flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fshort-enums -funsigned-char  -mone-byte-bool -fno-common -ffast-math")
	# set config specific flagss
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG=1 -D_DEBUG=1")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG=1")

	set(CMAKE_C_FLAGS "")
  else(APPLE)
	# Linux -------------------------------------------
	if(STATIC)
		set(BUILD_FOLDER "gcc4")	
		set(XMPCORE_LIB "staticXMPCore")
		set(XMPFILES_LIB "staticXMPFiles")
		set(LIB_EXT ".ar")	
	else(STATIC)
		set(BUILD_FOLDER "gcc4")	
		set(XMPCORE_LIB "libXMPCore")
		set(XMPFILES_LIB "libXMPFiles")
		set(LIB_EXT ".so")	
	endif(STATIC)
	add_definitions(-DUNIX_ENV=1 -D__LITTLE_ENDIAN__=1)

	if(CMAKE_CL_64)
		add_definitions(-DXMP_64=1)
		set(PLATFORM_FOLDER "i80386linux_x64")
	else()	
		set(PLATFORM_FOLDER "i80386linux")	
	endif(CMAKE_CL_64)
    
    set(CMAKE_CXX_FLAGS "${CXX_FLAGS} -Wno-ctor-dtor-privacy -fPIC -funsigned-char -fexceptions -Wno-multichar -Wno-implicit")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG=1 -D_DEBUG=1")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG=1")
  endif(APPLE)
else(UNIX)
  if(WIN32)
  
	# Windows -------------------------------------------
	if(STATIC)
		set(BUILD_FOLDER "vc16")
		set(XMPCORE_LIB "XMPCore")
		set(XMPFILES_LIB "XMPFiles")
		set(LIB_EXT ".lib")
	else(STATIC)
		set(BUILD_FOLDER "vc16")
		set(XMPCORE_LIB "XMPCore")
		set(XMPFILES_LIB "XMPFiles")
		set(LIB_EXT ".lib")
	endif(STATIC)
	
	if(CMAKE_CL_64)
		add_definitions(-DXMP_64=1)
	    set(PLATFORM_FOLDER "windows_x64")
	else(CMAKE_CL_64)
	    set(PLATFORM_FOLDER "windows")
	endif(CMAKE_CL_64)
    
	# config independend preprocessor defines
	add_definitions(-DWIN_ENV=1 -DWIN32=1 -D_CONSOLE -DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS=1 -D_SCL_SECURE_NO_WARNINGS=1 )

	if(STATIC)
	add_definitions(-DXMP_StaticBuild=1)
	else(STATIC)
	add_definitions(-DXMP_StaticBuild=0)
	endif(STATIC)
	
  else(WIN32)
	# unknown platform
	MESSAGE(ERROR ": Unknown Platform")
  endif(WIN32)
endif(UNIX)

include(${XMP_ROOT}/build/ProductConfig.cmake)
#including the shared configs
set (COMPONENT XMP)
include(${COMMON_BUILD_SHARED_DIR}/SharedConfig.cmake)


# ==============================================================================
# Platform independend config
# ==============================================================================

# generic preprocessor defines
# add_definitions(-DXMP_StaticBuild=1)
	if(STATIC)
	add_definitions(-DXMP_StaticBuild=1)
	else(STATIC)
	add_definitions(-DXMP_StaticBuild=0)
	endif(STATIC)
	
# Where to look for the XMPCore static library
set(XMP_LIB_DIR ${XMP_ROOT}/public/libraries/${PLATFORM_FOLDER})
#message (STATUS " XMP_LIB_DIR = ${XMP_LIB_DIR}")

# We only need Debug and Release Targets
if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES Debug Release)
	set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
	"Reset the configurations to what we need"
	FORCE)
endif()

# helper macro for Mac to add frameworks to the build
macro(ADD_FRAMEWORK fwname appname)
	if(APPLE)
		find_library(FRAMEWORK_${fwname}
			NAMES ${fwname}
			PATHS ${CMAKE_OSX_SYSROOT}/System/Library
			PATH_SUFFIXES Frameworks
			NO_DEFAULT_PATH)
		if( ${FRAMEWORK_${fwname}} STREQUAL FRAMEWORK_${fwname}-NOTFOUND)
			MESSAGE(ERROR ": Framework ${fwname} not found")
		else()
			TARGET_LINK_LIBRARIES(${appname} ${FRAMEWORK_${fwname}})
			#MESSAGE(STATUS "Framework ${fwname} found at ${FRAMEWORK_${fwname}}")
		endif()
		set_target_properties(${appname} PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
	endif(APPLE)
endmacro(ADD_FRAMEWORK)


