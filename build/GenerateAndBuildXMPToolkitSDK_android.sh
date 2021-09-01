#!/bin/bash
# =================================================================================================
# ADOBE SYSTEMS INCORPORATED
# Copyright 2019 Adobe Systems Incorporated
# All Rights Reserved
#
# NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
# of the Adobe license agreement accompanying it.
# =================================================================================================

clear
# Get the absolute path of the script
abspath=$(cd ${0%/*} && echo $PWD/${0##*/})
clean()
{
	echo "Cleaning..."
	if [ -e CMakeFiles ]
	then
	rm -rf CMakeFiles
	fi
	if [ -e build.ninja ]
	then
	rm -rf build.ninja
	fi
	if [ -e rules.ninja ]
	then
	rm -rf rules.ninja
	fi
	if [ -e CMakeCache.txt ]
	then
	rm -rf CMakeCache.txt
	fi
	if [ -e ../XMPCore/build/CMakeFiles ]
	then
	rm -rf ../XMPCore/build/CMakeFiles
	fi
	if [ -e ../XMPFiles/build/CMakeFiles ]
	then
	rm -rf ../XMPFiles/build/CMakeFiles
	fi
	if [ -e ../public/libraries/android ]
	then
	rm -rf ../public/libraries/android
	fi
	
	echo "Done"
	exit 0;
}
Generate()
{
cd "`dirname \"$abspath\"`" >/dev/null
echo "`dirname \"$abspath\"`"

../tools/android/cmake -G 'Android Gradle - Ninja' -DANDROID_NDK=$cmake_ndkpath -DCMAKE_MAKE_PROGRAM=$cmake_program -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN -DCMAKE_BUILD_TYPE=$cmake_buildmode -DANDROID_ABI=$cmake_android_abi -DANDROID_NATIVE_API_LEVEL=$cmake_native_api_level -DCMAKE_CXX_FLAGS=$cmake_cxx_flags -DANDROID=true -DXMP_BUILD_STATIC=$cmake_build_static -DANDROID_STL=$cmake_android_stl CMakeLists.txt

../tools/android/ninja

rm -rf CMakeFiles CMakeCache.txt

if [  $? -ne 0 ]
then
echo "ERROR: CMAKE tool failed"
exit 1
else
echo "Android Libraries created successfully for \"$cmake_android_abi\""
fi
}

#set defaults
cmake_build_warning_as_error="Off"
TOOLCHAIN="../tools/android/ndk-bundle/build/cmake/android.toolchain.cmake"
cmake_ndkpath='../tools/android/ndk-bundle'
cmake_android_abi="armeabi-v7a"
cmake_native_api_level="17"
cmake_cxx_flags="-fexceptions"
cmake_android_stl="c++_shared"
cmake_program="ninja"
cmake_build_static="Off"
cmake_buildtype="Dynamic"

SDKx86_64()
{
cmake_android_abi="x86_64"
}
SDKx86()
{
cmake_android_abi="x86"
}
SDKarmabi-v7a()
{
cmake_android_abi="armeabi-v7a"
}
SDKarm64-v8a()
{
cmake_android_abi="arm64-v8a"
}
Debug()
{
cmake_buildmode="Debug"
Generate
}
Release()
{
cmake_buildmode="Release"
Generate
}

echo "1. Clean All"
echo "2. Generate XMPToolkitSDK x86_64"
echo "3. Generate XMPToolkitSDK x86"
echo "4. Generate XMPToolkitSDK armabi-v7a"
echo "5. Generate XMPToolkitSDK arm64-v8a"
#echo "6. Generate All"

read -p "Enter your choice: " choice
case $choice in
1) clean;;
2) SDKx86_64;;
3) SDKx86;;
4) SDKarmabi-v7a;;
5) SDKarm64-v8a;;
#6) SDKx86_64; SDKx86; SDKarmabi-v7a; SDKarm64-v8a;;
esac

if [  $choice -ne 1 ]
then
	echo "1. Debug"
	echo "2. Release"

	read -p "Enter your choice: " choice1
	case $choice1 in
	1) Debug;;
	2) Release;;
	esac
fi
exit 0
