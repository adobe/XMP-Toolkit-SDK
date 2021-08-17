Building Sample Projects:

Windows:

1. Double Click "GenerateSamples_win.bat" or run it through command prompt.
2. Enter the type of project to create
3. The project files will be created in vc14\windows or vc14\windows_x64 folder

Mac:

1. Run the shell script GenerateSamples_mac.sh.
2. Enter the type of project to create
3. The project files will be created in xcode\intel_64 folder

Linux:

1. Run the Makefile. This Makefile will call cmake to generate the makefile for all the samples. Also all the sample will be build automatically.
2. All the sample projects makefiles will be created in gcc folder.
3. All the sample will be built in ../target folder.
4. Make sure the gcc location is added to $PATH and its libraries location to $LD_LIBRARY_PATH. There is a need to add libuuid.so library path to the $LD_LIBRARY_PATH as well.
