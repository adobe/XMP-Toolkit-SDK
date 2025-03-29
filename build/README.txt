This README describes steps to generate project files on different platforms. These platform specific projects can then be used to build the XMPToolkit SDK. Before generating the projects the SDK developer needs to copy the dependencies to the proper locations. Refer to "Dependencies" section in the XMP Toolkit Programmer's Guide.


WINDOWS 7 and above

Options to generate Project files in Windows are of the form "Generate XMPSDKToolkit [Dynamic|static] [Win32|x64]". An option of "Dynamic" indicates that the generated project files will build a dll for the XMP SDK components. On the other hand the option of "Static" indicates that the generated project files will build a static lib for the XMP SDK components. Options for "Win32", "x64", or "ARM64" indicate that the generated project files will build the library for x86 (32 bit), x64 (64 bit), or ARM64 (Arm 64 bit) applications respectively. 

1. Open the windows command prompt and change the current directory to <xmpsdk>/build.
2. Run the batch file GenerateXMPToolkitSDK_win.bat. There will be two options(2 & 3) to build projects for different configurations, one option(4) to generate projects for all configurations and one option(1) to clean all the projects.
3. After running the batch file (options 2 & 3),the vc14 projects(using cmake) will be generated at the location <xmpsdk>/build/vc14/<static|dynamic>/windows_x64>/XMPToolkitSDK64.sln.
4. Open the Visual Studio solution file and build XMP Toolkit SDK for debug/release configuration.

MAC OS X 10.9 and above

Options to generate Project files in Mac are of the form "Generate XMPSDKToolkit [Dynamic|static] [64|iOS]". An option of "Dynamic" indicates that the generated project files will build a framework for the XMP SDK components. On the other hand the option of "Static" indicates that the generated project files will build a static lib for the XMP SDK components. Option for "64" indicate that the generated project files will build for 64 bit applications. The option of "iOS" will generate the project files that will build XMP Core and XMP Files library for iOS simulators and iOS devices.

1. Open the Terminal on Mac and change the current directory to <xmpsdk>/build.
2. Run the shell script GenerateXMPToolkitSDK_mac.sh. There will be three options(2-4) to build projects for different configurations, one option(5) to generate projects for all configurations and one option(1) to clean all the projects.
3. After running the batch file (options 2-5),the xcode projects will be generated(using cmake) at the location <xmpsdk>/build/xcode/<static|dynamic>/<intel_64|ios>/<XMPToolkitSDK.xcodeproj|XMPToolkitSDK64.xcodeproj>.
4. Open the xcodeproj file and build XMP Toolkit SDK for debug/release configuration.

Note: When multiple Xcode versions are installed on a system, the developer has to use xcode-select command to specify the  Xcode version that he/she wishes to use. This is done in two steps by fetching the current Xcode folder and changing it if required.

1) Fetch the current Xcode folder
	$ xcode-select -print-path

2) Change the Xcode Folder if required. For example if the new XCode path is /Applications/Xcode.app
	$ sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer

Note: Presently a single scheme is created for each project. For building different configurations the Release or Debug configurations has to be selected using "Edit Scheme -> Run -> Release/Debug

LINUX

Options to generate makefiles in Linux are given in the form of a Makefile with different targets. The targets available are

StaticAll: Generates the Makefiles for static libraries and builds them. Includes both Debug and Release libs.
DynamicAll: Generates the Makefiles for shared libraries and builds them. Includes both Debug and Release libs.
DebugAll: Generates the Makefiles for Debug libraries and builds them. Includes both static and dynamic libs.
ReleaseAll: Generates the Makefiles for Release libraries and builds them. Includes both static and dynamic libs.

1. Change the current directory to <xmpsdk>/build.
2. Make sure the gcc location is added to $PATH and its libraries location to $LD_LIBRARY_PATH. There is a need to add libuuid.so library path to the $LD_LIBRARY_PATH as well.
3. This <xmpsdk>/build folder contains a Makefile which has different targets to build and the default is to build all of them. The Makefile generates the makefiles(using cmake) to build the project and then calls these makefiles to build the target. 



Note: Generating gcc Makefiles to build the project requires the path to the gcc set in the CMake toolchain file for Linux i.e. <xmpsdk>/build/shared/ToolchainGCC.cmake. In this file modifications must be done by the developer to set the CMAKE_FIND_ROOT_PATH so that CMake picks the desired gcc compiler. He/She may also want to change the parameter XMP_ENABLE_SECURE_SETTINGS as per the configured gcc.

 a) If the gcc is configured with --enable-libssp (can be checked by executing gcc -v), he/she has to set the variable XMP_GCC_LIBPATH inside of file <xmpsdk>/build/ProductConfig.cmake to the path containing the static lib( libssp.a). In this case he/she can set the variable the XMP_ENABLE_SECURE_SETTINGS on.

b) If the gcc is configured with --disable-libssp, he/she has to set the variable XMP_ENABLE_SECURE_SETTINGS off.

=================================================================================================================================================

Note: The projects for different platforms are generated using CMake. CMake generates projects with absolute paths so they cannot be copied from one place to other. If the project is to be moved, then the whole tree has to be copied to the new location and CMake should be invoked again.
