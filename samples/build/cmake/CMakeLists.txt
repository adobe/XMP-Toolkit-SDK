# =================================================================================================
# ADOBE SYSTEMS INCORPORATED
# Copyright 2013 Adobe Systems Incorporated
# All Rights Reserved
#
# NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
# of the Adobe license agreement accompanying it.
# =================================================================================================

# define minimum cmake version
# For Android always build with make 3.6
if(ANDROID)
	cmake_minimum_required(VERSION 3.5.2)
else(ANDROID)
	cmake_minimum_required(VERSION 3.15.5)
endif(ANDROID)

project(${PROJECT_NAME}64)

#This projects relative path to XMP Root
set ( XMP_THIS_PROJECT_RELATIVEPATH "../../../")
#setting the root for XMP SDK
if(NOT DEFINED XMP_ROOT)
	set(XMP_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/${XMP_THIS_PROJECT_RELATIVEPATH})
endif()

if(NOT DEFINED COMMON_BUILD_SHARED_DIR)
	set(COMMON_BUILD_SHARED_DIR ${XMP_ROOT}/build/shared) 
endif()

set(USE_BUILDMODE_LIBNAME 0)

include(CMakeListsCommonSDK.txt)