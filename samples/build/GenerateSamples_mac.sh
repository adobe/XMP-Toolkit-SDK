#!/bin/bash
# =================================================================================================
# ADOBE SYSTEMS INCORPORATED
# Copyright 2013 Adobe Systems Incorporated
# All Rights Reserved
#
# NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
# of the Adobe license agreement accompanying it.
# =================================================================================================

# Get the absolute path of the script
abspath=$(cd ${0%/*} && echo $PWD/${0##*/})
scriptdir="`dirname \"$abspath\"`"
clear
CMAKE="$scriptdir/../../tools/cmake/bin/cmake.app/Contents/bin/cmake"
if [ ! -f "$CMAKE" ]
then
echo "ERROR: Cmake tool not present at $CMAKE, cannot proceed "
exit 1
fi
clean()
{
echo "Cleaning..."

if [ -e cmake/CustomSchema/build_x64 ]
then
rm -rf cmake/CustomSchema/build_x64
fi

if [ -e cmake/CustomSchemaNewDOM/build_x64 ]
then
rm -rf cmake/CustomSchemaNewDOM/build_x64
fi

if [ -e cmake/DumpFile/build_x64 ]
then
rm -rf cmake/DumpFile/build_x64
fi

if [ -e cmake/DumpMainXMP/build_x64 ]
then
rm -rf cmake/DumpMainXMP/build_x64
fi

if [ -e cmake/DumpScannedXMP/build_x64 ]
then
rm -rf cmake/DumpScannedXMP/build_x64
fi

if [ -e cmake/ModifyingXMP/build_x64 ]
then
rm -rf cmake/ModifyingXMP/build_x64
fi

if [ -e cmake/ModifyingXMPNewDOM/build_x64 ]
then
rm -rf cmake/ModifyingXMPNewDOM/build_x64
fi

if [ -e cmake/ReadingXMP/build_x64 ]
then
rm -rf cmake/ReadingXMP/build_x64
fi

if [ -e cmake/ReadingXMPNewDOM/build_x64 ]
then
rm -rf cmake/ReadingXMPNewDOM/build_x64
fi

if [ -e cmake/XMPCommand/build_x64 ]
then
rm -rf cmake/XMPCommand/build_x64
fi

if [ cmake/XMPCoreCoverage/build_x64 ]
then
rm -rf cmake/XMPCoreCoverage/build_x64
fi

if [ -e cmake/XMPFilesCoverage/build_x64 ]
then
rm -rf cmake/XMPFilesCoverage/build_x64
fi

if [ -e cmake/XMPIterations/build_x64 ]
then
rm -rf cmake/XMPIterations/build_x64
fi

if [ -e cmake/UnicodeCorrectness/build_x64 ]
then
rm -rf cmake/UnicodeCorrectness/build_x64
fi

if [ -e cmake/UnicodeParseSerialize/build_x64 ]
then
rm -rf cmake/UnicodeParseSerialize/build_x64
fi

if [ -e cmake/UnicodePerformance/build_x64 ]
then
rm -rf cmake/UnicodePerformance/build_x64
fi

if [ -e xcode ]
then
rm -rf xcode
fi
if [ -e ../target/macintosh ]
then
rm -rf ../target/macintosh
fi
echo "Done"
exit 0;
}

Xcode64()
{
#create 64bit Xcode Project
DIR="xcode/intel_64"
TOOLCHAIN="$scriptdir/../../build/shared/ToolchainLLVM.cmake"
bit64="1"
}

createProject()
{
#creating the directory
if [ ! -e $DIR ]
then
if ! mkdir -p $DIR
then
echo "ERROR: Cannot create $DIR"
exit 1
fi
fi
if ! cd $DIR
then
echo "ERROR: Cannot change to $DIR"
exit 1
fi

#generate the make file
echo "PWD is $PWD, CMAKE LOCATION IS $CMAKE, dir is $DIR"
if ! ("$CMAKE" -DCMAKE_CL_64=$bit64 -DSTATIC=$static -G "Xcode" -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" -DCMAKE_LIBCPP="On" ../../cmake)
then
echo "ERROR: CMAKE tool failed"
exit 1
else
echo "Xcode project created successfully"
fi
}

echo "Enter your choice:"
echo "1. Clean"
echo "2. 64 Bit Xcode Project (Static)"
echo "3. 64 Bit Xcode Project (Dynamic)"

read choice

case $choice in
  1) clean; break;;
  2) static="1"; Xcode64; createProject;;
  3) static="0"; Xcode64; createProject;;
  *) echo "ERROR: Invalid Choice, Exiting"; exit 1;;
esac
