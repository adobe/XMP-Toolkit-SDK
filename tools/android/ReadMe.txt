The XMP Toolkit uses an open-source system to manage the build process.
You must obtain and install Android tools that are required for the build process  for Android binaries of the XMP toolkit  SDK. 

1. CMake distribution can be installed  from  the  Android  Studio  SDK  Manager.  Alternatively,  obtain  a copy  of  the  CMake distribution  for  your  platform  (version  3.6.4111459)  from: 
https://dl.google.com/android/repository/cmake-3.6.4111459-darwin-x86_64.zip 

2. Ninja  executable  comes  with the  CMake distribution  -  <cmakeroot>/bin/ninja. 


3. Install  NDK  (version  19c)  from  the  Android  Studio  SDK  Manager.  Another  option  is  to  download directly  from:  
https://dl.google.com/android/repository/android-ndk-r19c-darwin-x86_64.zip


XMP Toolkit places Ndk-bundle , ninja, and cmake of android in this folder or a symbolic link can be created for the same in this folder

sudo ln -s /Users/..../Library/Android/sdk/cmake/3.6.4111459/bin/cmake ../tools/android/
sudo ln -s /Users/..../Library/Android/sdk/ndk-bundle ../tools/android/
sudo ln -s /Users/..../Library/Android/sdk/cmake/3.6.4111459/bin/ninja ../tools/android/

Note : In case you are not using symbolic links to cmake, CMake binary needs cmake share folder parallel to its parent folder i.e. at tools/⁩