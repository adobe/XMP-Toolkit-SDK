#// =================================================================================================
#// Copyright 2020 Adobe
#// All Rights Reserved.
#// NOTICE: Adobe permits you to use, modify, and distribute this file in
#// accordance with the terms of the Adobe license agreement accompanying
#// it. 
#// =================================================================================================

# ==============================================================================
# define minimum cmake version
# For Android always build with make 3.6
if(ANDROID)
	cmake_minimum_required(VERSION 3.5.2)
else(ANDROID)
	cmake_minimum_required(VERSION 3.15.5)
endif(ANDROID)

# ==============================================================================
# Shared config for android
# ==============================================================================

add_definitions(-DANDROID_ENV=1)

set(COMMON_PLATFORM_LINK "${${COMPONENT}_PLATFORM_LINK} ")
set(COMMON_SHARED_COMPILE_FLAGS "${COMMON_SHARED_COMPILE_FLAGS} -fPIC  ${${COMPONENT}_SHARED_COMPILE_FLAGS} -fexceptions -Wformat -Wformat-security ")
set(COMMON_SHARED_COMPILE_DEBUG_FLAGS "${COMMON_SHARED_COMPILE_DEBUG_FLAGS} ${${COMPONENT}_SHARED_COMPILE_DEBUG_FLAGS} -g -DDEBUG=1 -D_DEBUG=1")
set(COMMON_SHARED_COMPILE_RELEASE_FLAGS "${COMMON_SHARED_COMPILE_RELEASE_FLAGS} ${${COMPONENT}_SHARED_COMPILE_RELEASE_FLAGS} -DNDEBUG=1 -D_NDEBUG=1")


if(NOT ${COMPONENT}_DISABLE_ALL_WARNINGS)
	set(COMMON_SHARED_COMPILE_FLAGS "${COMMON_SHARED_COMPILE_FLAGS} -Wall")
endif()

if(${COMPONENT}_ENABLE_SECURE_SETTINGS)
	set(COMMON_SHARED_COMPILE_FLAGS "${COMMON_SHARED_COMPILE_FLAGS} -fstack-protector -O -D_FORTIFY_SOURCE=2")
	set(COMMON_PLATFORM_LINK "${COMMON_PLATFORM_LINK} -Wl,-z,relro -Wl,-z,now")
else()
	set(COMMON_SHARED_COMPILE_DEBUG_FLAGS "${COMMON_SHARED_COMPILE_DEBUG_FLAGS} ${COMMON_SHARED_COMPILE_FLAGS} -O0")
	set(COMMON_SHARED_COMPILE_RELEASE_FLAGS "${COMMON_SHARED_COMPILE_RELEASE_FLAGS} ${COMMON_SHARED_COMPILE_FLAGS} -Os")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_SHARED_COMPILE_FLAGS} ${${COMPONENT}_EXTRA_C_COMPILE_FLAGS} -Wno-implicit ")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${COMMON_SHARED_COMPILE_DEBUG_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${COMMON_SHARED_COMPILE_RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_SHARED_COMPILE_FLAGS} ${COMMON_EXTRA_CXX_COMPILE_FLAGS} -mfpu=vfpv3-d16 -Wno-reorder -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG " ${COMMON_SHARED_COMPILE_DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE " ${COMMON_SHARED_COMPILE_RELEASE_FLAGS} -s")
set(COMMON_PLATFORM_BEGIN_WHOLE_ARCHIVE "-Wl,--whole-archive")
set(COMMON_PLATFORM_END_WHOLE_ARCHIVE "-Wl,--no-whole-archive")
set(COMMON_DYLIBEXTENSION	"so")

if(NOT CMAKE_BUILD_WITH_INSTALL_RPATH)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

if(ANDROID_STL)
	set(STL_PLATFORM	${ANDROID_STL})
else()
	set(STL_PLATFORM	"c++_shared")
endif()

if(ANDROID_ABI)
	if(ANDROID_ABI STREQUAL "armeabi-v7a")
		add_definitions(-DXMP_ANDROID_ARM=1)
	endif()
endif()


SetupCompilerFlags()
SetupGeneralDirectories()

# ==============================================================================
# Functions
# ==============================================================================

# ==============================================================================
# Function: Add Mac framework to list of libraries to link to
# ==============================================================================
#
function(AddMacFramework appname fwname)
	# ignore on Android
endfunction(AddMacFramework)

# ==============================================================================
# Function: Set platform specific link flags to target
# ==============================================================================
#
# TODO: make function work on all platforms. Unify with AddMacFramework.
function(SetPlatformLinkFlags target linkflags libname)
	set_target_properties(${target} PROPERTIES LINK_FLAGS "${COMMON_PLATFORM_LINK} ${linkflags}")
	
endfunction(SetPlatformLinkFlags)

# ==============================================================================
# Function: Set the output path depending on isExecutable
#	The output path is corrected. 
# ==============================================================================
#
function(SetOutputPath2 path targetName targetType)
	# remove last path item if not makefile
	if(NOT XMP_IS_MAKEFILE_BUILD)
		get_filename_component(correctedPath ${path} PATH)
		#message("SetOutputPath: ${path} to ${correctedPath}")
	else()
		set(correctedPath ${path})
	endif()

	if(${targetType} MATCHES "EXECUTABLE")
		set_property(TARGET ${targetName} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${correctedPath})
	elseif(${targetType} MATCHES "DYNAMIC")
		set_property(TARGET ${targetName} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${correctedPath})
	else()
		set_property(TARGET ${targetName} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${correctedPath})
	endif()
endfunction(SetOutputPath2)

# ==============================================================================
# Function: Set precompiled headers
# ==============================================================================
#
function( SetPrecompiledHeaders target pchHeader pchSource )
	# not for Android
endfunction( SetPrecompiledHeaders )

# ==============================================================================
# Function: Add library in native OS name to variable resultName
# ==============================================================================
#
function(AddLibraryNameInNativeFormat libname libpath resultName)
	foreach(libname_item ${libname})
		set(result "${libpath}/lib${libname_item}.a")
		set(resultList ${resultList} ${result})
	endforeach()	
	set(${resultName} ${${resultName}} ${resultList} PARENT_SCOPE)
	#message("AddLibraryNameInNativeFormat: ${resultList} ")
endfunction(AddLibraryNameInNativeFormat)

# ==============================================================================
# Function: Add shared library in native OS name to variable resultName
# ==============================================================================
#
function(AddSharedLibraryNameInNativeFormat libname libpath resultName)
	foreach(libname_item ${libname})
		set(result "${libpath}/lib${libname_item}.so")
		set(resultList ${resultList} ${result})
	endforeach()	
	set(${resultName} ${${resultName}} ${resultList} PARENT_SCOPE)
	#message("AddSharedLibraryNameInNativeFormat: ${resultList} ")
endfunction(AddSharedLibraryNameInNativeFormat)
