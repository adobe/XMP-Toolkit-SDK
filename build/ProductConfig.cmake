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
# Product Config for XMP Toolkit
# ==============================================================================

if (UNIX)
	if (APPLE)
		set(XMP_ENABLE_SECURE_SETTINGS "ON")
		if (APPLE_IOS)
			set(XMP_PLATFORM_SHORT "ios")
			if(XMP_BUILD_STATIC)
				set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_INCLUDING_64_BIT)")
			else()
				set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_64_BIT)")
			endif()
			add_definitions(-DIOS_ENV=1)			

			# shared flags
			set(XMP_SHARED_COMPILE_FLAGS "${XMP_SHARED_COMPILE_FLAGS}  ${XMP_EXTRA_COMPILE_FLAGS}")
			set(XMP_SHARED_COMPILE_DEBUG_FLAGS "-O0 -DDEBUG=1 -D_DEBUG=1")
			set(XMP_SHARED_COMPILE_RELEASE_FLAGS "-Os -DNDEBUG=1 -D_NDEBUG=1")
			set(XMP_PLATFORM_BEGIN_WHOLE_ARCHIVE "")
			set(XMP_PLATFORM_END_WHOLE_ARCHIVE "")
		else ()
			set(XMP_PLATFORM_SHORT "mac")
			if(CMAKE_CL_64)
				set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)" CACHE STRING "Build architectures for OSX" FORCE)
				add_definitions(-DXMP_64=1)
			endif(CMAKE_CL_64)

			# is SDK and deployment target set?
			if(NOT DEFINED XMP_OSX_SDK)
				# no, so default to CS6 settings
				#set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "4.2")
				set(XMP_OSX_SDK		10.13)
				set(XMP_OSX_TARGET	10.11)
			endif()

			add_definitions(-DMAC_ENV=1)


			#
			# shared flags
			#
			set(XMP_SHARED_COMPILE_FLAGS "-Wall -Wextra")
			set(XMP_SHARED_COMPILE_FLAGS "${XMP_SHARED_COMPILE_FLAGS} -Wno-missing-field-initializers") # disable some warnings

			set(XMP_SHARED_COMPILE_FLAGS "${XMP_SHARED_COMPILE_FLAGS}")

			set(XMP_SHARED_CXX_COMPILE_FLAGS "${XMP_SHARED_COMPILE_FLAGS} -Wno-reorder") # disable warnings

			set(XMP_PLATFORM_BEGIN_WHOLE_ARCHIVE "")
			set(XMP_PLATFORM_END_WHOLE_ARCHIVE "")
			set(XMP_DYLIBEXTENSION	"dylib")
		endif ()

		#There were getting set from SetupTargetArchitecture. 
		if(APPLE_IOS)
			set(XMP_CPU_FOLDERNAME	"$(ARCHS)")
		else()
			if(CMAKE_CL_64)
				set(XMP_BITDEPTH		"64")
				if(CMAKE_LIBCPP)
					set(XMP_CPU_FOLDERNAME	"universal")
				endif()
			endif()
		endif()
		
		# XMP_PLATFORM_FOLDER is used in OUTPUT_DIR and Debug/Release get automatically added for VS/XCode projects
		if(APPLE_IOS)
		    set(XMP_PLATFORM_FOLDER "ios/${XMP_CPU_FOLDERNAME}")
		else()
		    set(XMP_PLATFORM_FOLDER "macintosh/${XMP_CPU_FOLDERNAME}")
		endif()

    elseif(ANDROID)

    	set(XMP_PLATFORM_FOLDER "android")
    	set(XMP_PLATFORM_SHORT "android")
    	set(XMP_PLATFORM_LINK "  ${XMPCORE_UUIDLIB_PATH} ${XMP_EXTRA_LINK_FLAGS} ${XMP_TOOLCHAIN_LINK_FLAGS} -latomic")
    	set(XMP_SHARED_COMPILE_FLAGS "-Wno-multichar  -funsigned-char  ${XMP_EXTRA_COMPILE_FLAGS} ${XMP_TOOLCHAIN_COMPILE_FLAGS}")
    	set(XMP_SHARED_COMPILE_DEBUG_FLAGS " ")
    	set(XMP_SHARED_COMPILE_RELEASE_FLAGS "-fwrapv ")

	if(NOT XMP_DISABLE_FASTMATH)
	#	set(XMP_SHARED_COMPILE_FLAGS "${XMP_SHARED_COMPILE_FLAGS} -ffast-math")
	#	set(XMP_SHARED_COMPILE_RELEASE_FLAGS "${XMP_SHARED_COMPILE_RELEASE_FLAGS} -fomit-frame-pointer -funroll-loops")
	endif()

    else()
		if(NOT CMAKE_CROSSCOMPILING)
			if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
				# running on 64bit machine
				set(XMP_EXTRA_BUILDMACHINE	"Buildmachine is 64bit")
			else()
				# running on 32bit machine
				set(XMP_EXTRA_BUILDMACHINE	"Buildmachine is 32bit")
			endif()

			if(CMAKE_CL_64)
				set(XMP_EXTRA_COMPILE_FLAGS "-m64")
				set(XMP_EXTRA_LINK_FLAGS "-m64")
				if(CENTOS)	
					set(XMP_PLATFORM_FOLDER "i80386linux_x64_centos") # add XMP_BUILDMODE_DIR to follow what other platforms do
					set(XMP_GCC_LIBPATH /opt/llvm/lib)
				else()
				set(XMP_PLATFORM_FOLDER "i80386linux_x64") # add XMP_BUILDMODE_DIR to follow what other platforms do
				set(XMP_GCC_LIBPATH /user/unicore/i80386linux_x64/compiler/gcc4.8.2/linux3.10_64/lib64)
				endif()	
			else()
				set(XMP_EXTRA_LINK_FLAGS "-m32 -mtune=i686")
				if(CENTOS)	
					set(XMP_GCC_LIBPATH /opt/llvm/lib)
					set(XMP_PLATFORM_FOLDER "i80386linux_centos") # add XMP_BUILDMODE_DIR to follow what other platforms do
				else()
				set(XMP_PLATFORM_FOLDER "i80386linux") # add XMP_BUILDMODE_DIR to follow what other platforms do
				set(XMP_GCC_LIBPATH /user/unicore/i80386linux/compiler/gcc4.8.2/linux3.10_32/lib)
			endif()
			endif()
		else()
			# running toolchain
			if(CMAKE_CL_64)
				set(XMP_EXTRA_COMPILE_FLAGS "-m64")
				set(XMP_EXTRA_LINK_FLAGS "-m64")
				
				if(CENTOS)	
					set(XMP_GCC_LIBPATH /opt/llvm/lib)
					set(XMP_PLATFORM_FOLDER "i80386linux_x64_centos") # add XMP_BUILDMODE_DIR to follow what other platforms do
				else()
				set(XMP_PLATFORM_FOLDER "i80386linux_x64") # add XMP_BUILDMODE_DIR to follow what other platforms do
				set(XMP_GCC_LIBPATH /user/unicore/i80386linux_x64/compiler/gcc4.8.2/linux3.10_64/lib64)
				endif()	
			else()
               	set(XMP_EXTRA_COMPILE_FLAGS "-m32 -mtune=i686")
				set(XMP_EXTRA_LINK_FLAGS "-m32")
				if(CENTOS)	
					set(XMP_GCC_LIBPATH /opt/llvm/lib)
					set(XMP_PLATFORM_FOLDER "i80386linux_centos") # add XMP_BUILDMODE_DIR to follow what other platforms do
				else()
				set(XMP_PLATFORM_FOLDER "i80386linux") # add XMP_BUILDMODE_DIR to follow what other platforms do
				set(XMP_GCC_LIBPATH /user/unicore/i80386linux/compiler/gcc4.8.2/linux3.10_32/lib)
			endif()
			endif()

			set(XMP_EXTRA_BUILDMACHINE	"Cross compiling")
		endif()
		set(XMP_PLATFORM_VERSION "linux3.10") # add XMP_BUILDMODE_DIR to follow what other platforms do

		add_definitions(-DUNIX_ENV=1)
		# Linux -------------------------------------------
		set(XMP_PLATFORM_SHORT "linux")
		#gcc path is not set correctly
		#set(CMAKE_C_COMPILER "/user/unicore/i80386linux/compiler/gcc4.4.4/linux2.6_32/bin/gcc")
		#set(CMAKE_CXX_COMPILER "/user/unicore/i80386linux/compiler/gcc4.4.4/linux2.6_32/bin/gcc")
		#set(XMP_GCC_LIBPATH /user/unicore/i80386linux/compiler/gcc4.4.4/linux2.6_32/lib)

   		set(XMP_PLATFORM_LINK "-z defs -Xlinker -Bsymbolic -Wl,--no-undefined  ${XMP_EXTRA_LINK_FLAGS} ${XMP_TOOLCHAIN_LINK_FLAGS} -lrt -Wl,--no-as-needed -ldl -lpthread ${XMP_GCC_LIBPATH}/libssp.a")
if(CENTOS)
		set(XMP_PLATFORM_LINK " -lc++abi ${XMP_PLATFORM_LINK}")
endif()

if(ANDROID)	
	set(XMP_SHARED_COMPILE_FLAGS "-Wno-multichar -funsigned-char  ${XMP_EXTRA_COMPILE_FLAGS} ${XMP_TOOLCHAIN_COMPILE_FLAGS}")
else()
	set(XMP_SHARED_COMPILE_FLAGS "-Wno-multichar -D_FILE_OFFSET_BITS=64 -funsigned-char  ${XMP_EXTRA_COMPILE_FLAGS} ${XMP_TOOLCHAIN_COMPILE_FLAGS}")
endif()
		set(XMP_SHARED_COMPILE_DEBUG_FLAGS " ")
		set(XMP_SHARED_COMPILE_RELEASE_FLAGS "-fwrapv ")

		if(NOT XMP_DISABLE_FASTMATH)
		#	set(XMP_SHARED_COMPILE_FLAGS "${XMP_SHARED_COMPILE_FLAGS} -ffast-math")
		#	set(XMP_SHARED_COMPILE_RELEASE_FLAGS "${XMP_SHARED_COMPILE_RELEASE_FLAGS} -fomit-frame-pointer -funroll-loops")
		endif()
	endif()
else ()
	if(CMAKE_CL_64)
		if(${CMAKE_ARCH} MATCHES "ARM64")
			set(XMP_PLATFORM_FOLDER "windows_arm64")
		else()
		set(XMP_PLATFORM_FOLDER "windows_x64") # leave XMP_BUILDMODE_DIR away, since CMAKE_CFG_INTDIR gets added by CMake automatically
		endif()
	else(CMAKE_CL_64)
		set(XMP_PLATFORM_FOLDER "windows") # leave XMP_BUILDMODE_DIR away, since CMAKE_CFG_INTDIR gets added by CMake automatically
	endif(CMAKE_CL_64)
	set(XMP_PLATFORM_SHORT "win")
	set(XMP_PLATFORM_LINK "")
	set(XMP_SHARED_COMPILE_FLAGS "-DWIN_ENV=1 -D_CRT_SECURE_NO_WARNINGS=1 -D_SCL_SECURE_NO_WARNINGS=1  /J /fp:precise")
	set(XMP_SHARED_COMPILE_DEBUG_FLAGS "")
	if(XMP_BUILD_STATIC)
		set(XMP_SHARED_COMPILE_RELEASE_FLAGS "/O1 /Ob2 /Os /Oy- /FD ")
	else()
		set(XMP_SHARED_COMPILE_RELEASE_FLAGS "/O1 /Ob2 /Os /Oy- /GL /FD ")
	endif()
	
	set(XMP_SHARED_COMPILE_DEBUG_FLAGS "${XMP_SHARED_COMPILE_DEBUG_FLAGS} /MDd")
	set(XMP_SHARED_COMPILE_RELEASE_FLAGS "${XMP_SHARED_COMPILE_RELEASE_FLAGS} /MD")

	set(XMP_PLATFORM_LINK_WIN "${XMP_WIN32_LINK_EXTRAFLAGS} /MAP")
endif ()


