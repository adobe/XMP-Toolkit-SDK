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

# Enable folder grouping of projects in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_C_IMPLICIT_LINK_DIRECTORIES "")
set(CMAKE_C_IMPLICIT_LINK_FRAMEWORK_DIRECTORIES "")

# ==============================================================================
# This project
set(TARGET_NAME XMPCore)
if(CMAKE_CL_64)
	project(${TARGET_NAME}64)
else(CMAKE_CL_64)
	project(${TARGET_NAME})
endif(CMAKE_CL_64)

set(STATIC_STR "Static")
if (XMP_BUILD_STATIC)
	set(TARGET_NAME "${TARGET_NAME}${STATIC_STR}")
	set(PROJECT_LABEL_STR "${TARGET_NAME}")
else(XMP_BUILD_STATIC)
	set(PROJECT_LABEL_STR "${TARGET_NAME}")
endif(XMP_BUILD_STATIC)

# ==============================================================================
# Shared config
# ==============================================================================
# setup some values before calling shared config
set(XMP_THIS_PROJECT_RELATIVEPATH "../..")

include(${CMAKE_CURRENT_SOURCE_DIR}/${XMP_THIS_PROJECT_RELATIVEPATH}/build/XMP_Config.cmake)

add_definitions(-DBUILDING_XMPCORE_LIB=1 -DXMP_COMPONENT_INT_NAMESPACE=AdobeXMPCore_Int)
if (XMP_BUILD_STATIC)
	add_definitions(-DBUILDING_XMPCORE_AS_STATIC=1)
else()
	add_definitions(-DBUILDING_XMPCORE_AS_DYNAMIC=1)
endif()

set(TP_ZUID_PATH "${XMPROOT_DIR}/third-party/zuid/interfaces")
set(TP_EXPAT_PATH "${PROJECT_ROOT}/../../third-party/expat/lib/")

# ==============================================================================
# Project specific config starts here
# ==============================================================================
if(UNIX)
	if(APPLE)
		# OSX -------------------------------------------
		if(XMP_BUILD_STATIC)
			set(XMPCORE_LIB "XMPCoreStatic")
		else(XMP_BUILD_STATIC)
			set(XMPCORE_LIB "XMPCore")
		endif()
	else(APPLE)
		# Linux -------------------------------------------
		if(XMP_BUILD_STATIC)
			set(XMPCORE_LIB "staticXMPCore")
		else(XMP_BUILD_STATIC)
			set(XMPCORE_LIB "XMPCore")
		endif()
	endif(APPLE)
else(UNIX)
	if(WIN32)
		# Windows -------------------------------------------
		if(XMP_BUILD_STATIC)
			set(XMPCORE_LIB "XMPCoreStatic")
		else(XMP_BUILD_STATIC)
			set(XMPCORE_LIB "XMPCore")
		endif()
	endif(WIN32)
endif(UNIX)

include(${CMAKE_CURRENT_SOURCE_DIR}/${XMP_THIS_PROJECT_RELATIVEPATH}/XMPCore/build/CMakeListsCommon.txt)



