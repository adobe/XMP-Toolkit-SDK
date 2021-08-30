The XMP Toolkit uses an open-source system to manage the build process. XMP Toolkit places cmake configuration files (CMakeLists.txt) in the source directories which is used to generate the standard build files.These generated build files can then be used to build the Toolkit.

To use CMake:

1. The minimum version of CMake required for this release is 3.15.5

Download the following recommended CMake distribution zipped package from the following links corresponding to the current platform (Windows, Mac, or Linux)
Windows	----	https://cmake.org/files/v3.15/cmake-3.15.5-win32-x86.zip
Mac OSX ----	https://cmake.org/files/v3.15/cmake-3.15.5-Darwin-x86_64.tar.gz
Linux   ----   https://cmake.org/files/v3.15/cmake-3.15.5-Linux-x86_64.tar.gz

2. For Windows and Linux copy the folders /bin and /share into <xmpsdk>/tools/cmake/
   For Mac:
     a) Create the folder  <xmpsdk>/tools/cmake/bin
     b) Rename and copy the app to this location <xmpsdk>/tools/cmake/bin/cmake.app
