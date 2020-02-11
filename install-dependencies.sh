#!/bin/bash

DIRECTORY=`dirname $0`
OVERWRITE=0

## Expat (2.1.0)
if [[ ! -d $DIRECTORY/third-party/expat/lib || $OVERWRITE ]]; then
	wget -O /tmp/expat.tar.gz https://github.com/libexpat/libexpat/releases/download/R_2_1_0/expat-2.1.0.tar.gz

	if [ $? -ne 0 ]; then
		echo "Error during expat download"
		exit $?
	fi

	tar xfv /tmp/expat.tar.gz -C /tmp expat-2.1.0/lib

	mkdir -p $DIRECTORY/third-party/expat/lib
	mv -f /tmp/expat-2.1.0/lib/* $DIRECTORY/third-party/expat/lib # Force

	rm -rf /tmp/expat-2.1.0 /tmp/expat.tar.gz # Clean
fi

## Zlib (1.2.8)
if [[ ! -f $DIRECTORY/third-party/zlib/zlib.h || $OVERWRITE ]]; then
	wget -O /tmp/zlib.zip https://github.com/madler/zlib/archive/v1.2.8.zip

	if [ $? -ne 0 ]; then
		echo "Error during zlib download"
		exit $?
	fi

	unzip -oC /tmp/zlib.zip '*.c' '*.h' -d /tmp/zlib

	mv -f /tmp/zlib/zlib-1.2.8/*.{c,h} $DIRECTORY/third-party/zlib # Force

	rm -rf /tmp/zlib # Clean
fi

## Cmake (3.15.5+)
if [[ ! -f $DIRECTORY/tools/cmake/bin/cmake|| $OVERWRITE ]]; then
	CMAKE_PATH=$(which cmake)
	mkdir -p $DIRECTORY/tools/cmake/bin
	rm -f $DIRECTORY/tools/cmake/bin/cmake # Clean

	if [[ -z $CMAKE_PATH || "$CMAKE_PATH" == *"not found" ]]; then
		echo "No cmake binary found on your computer."
		echo "You can download Cmake (3.15.5+) here: https://cmake.org/download/"

		if  [[ "$OSTYPE" == "darwin"* ]]; then
			echo "You also can download Cmake with brew or macport"
		fi

		exit 2
	fi

	ln -s /usr/local/bin/cmake $DIRECTORY/tools/cmake/bin/cmake
fi