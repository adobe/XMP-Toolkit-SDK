:: =================================================================================================
:: Copyright 2013 Adobe Systems Incorporated
:: All Rights Reserved.
::
:: NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
:: of the Adobe license agreement accompanying it.
:: =================================================================================================

echo OFF
cls
set CMAKE=..\..\tools\cmake\bin\cmake.exe
set workingDir=%~dp0%
set CMAKE=%workingDir%%CMAKE%

if NOT exist %CMAKE% ( ECHO Cmake tool not present at %CMAKE%, cannot proceed 
pause
exit /B 0)

:Start
ECHO Enter your choice:
ECHO 1. Clean
ECHO 2. VC2019, 64 Bit Project (Static)
ECHO 3. VC2019, 64 Bit Project (Dynamic)
ECHO 4. Close

ECHO
set /P choice=Enter your choice:

IF "%choice%"=="1" GOTO CleanCMake
IF "%choice%"=="2" GOTO 64VC2019
IF "%choice%"=="3" GOTO 64VC2019
IF "%choice%"=="4" GOTO Close


ECHO Invalid Choice, Exiting

:Close
pause
exit /B 0

:CleanCmake
echo "Cleaning..."
if exist cmake\AssetRelationship\build rmdir /S /Q cmake\AssetRelationship\build
if exist cmake\AssetRelationship\build_x64 rmdir /S /Q cmake\AssetRelationship\build_x64
if exist cmake\MarkerPOC\build rmdir /S /Q cmake\MarkerPOC\build
if exist cmake\MarkerPOC\build_x64 rmdir /S /Q cmake\MarkerPOC\build_x64
if exist cmake\CustomSchema\build rmdir /S /Q cmake\CustomSchema\build
if exist cmake\CustomSchema\build_x64 rmdir /S /Q cmake\CustomSchema\build_x64
if exist cmake\CustomSchemaNewDOM\build rmdir /S /Q cmake\CustomSchemaNewDOM\build
if exist cmake\CustomSchemaNewDOM\build_x64 rmdir /S /Q cmake\CustomSchemaNewDOM\build_x64
if exist cmake\DumpFile\build rmdir /S /Q cmake\DumpFile\build
if exist cmake\DumpFile\build_x64 rmdir /S /Q cmake\DumpFile\build_x64
if exist cmake\DumpMainXMP\build rmdir /S /Q cmake\DumpMainXMP\build
if exist cmake\DumpMainXMP\build_x64 rmdir /S /Q cmake\DumpMainXMP\build_x64
if exist cmake\DumpScannedXMP\build rmdir /S /Q cmake\DumpScannedXMP\build
if exist cmake\DumpScannedXMP\build_x64 rmdir /S /Q cmake\DumpScannedXMP\build_x64
if exist cmake\ModifyingXMP\build_x64 rmdir /S /Q cmake\ModifyingXMP\build_x64
if exist cmake\ModifyingXMP\build rmdir /S /Q cmake\ModifyingXMP\build
if exist cmake\ModifyingXMPNewDOM\build_x64 rmdir /S /Q cmake\ModifyingXMPNewDOM\build_x64
if exist cmake\ModifyingXMPNewDOM\build rmdir /S /Q cmake\ModifyingXMPNewDOM\build
if exist cmake\ReadingXMP\build_x64 rmdir /S /Q cmake\ReadingXMP\build_x64
if exist cmake\ReadingXMP\build rmdir /S /Q cmake\ReadingXMP\build
if exist cmake\ReadingXMPNewDOM\build_x64 rmdir /S /Q cmake\ReadingXMPNewDOM\build_x64
if exist cmake\ReadingXMPNewDOM\build rmdir /S /Q cmake\ReadingXMPNewDOM\build
if exist cmake\XMPCommand\build_x64 rmdir /S /Q cmake\XMPCommand\build_x64
if exist cmake\XMPCommand\build rmdir /S /Q cmake\XMPCommand\build
if exist cmake\XMPCoreCoverage\build_x64 rmdir /S /Q cmake\XMPCoreCoverage\build_x64
if exist cmake\XMPCoreCoverage\build rmdir /S /Q cmake\XMPCoreCoverage\build
if exist cmake\XMPFilesCoverage\build_x64 rmdir /S /Q cmake\XMPFilesCoverage\build_x64
if exist cmake\XMPFilesCoverage\build rmdir /S /Q cmake\XMPFilesCoverage\build
if exist cmake\XMPIterations\build_x64 rmdir /S /Q cmake\XMPIterations\build_x64
if exist cmake\XMPIterations\build rmdir /S /Q cmake\XMPIterations\build
if exist cmake\UnicodeCorrectness\build_x64 rmdir /S /Q cmake\UnicodeCorrectness\build_x64
if exist cmake\UnicodeCorrectness\build rmdir /S /Q cmake\UnicodeCorrectness\build
if exist cmake\UnicodeParseSerialize\build_x64 rmdir /S /Q cmake\UnicodeParseSerialize\build_x64
if exist cmake\UnicodeParseSerialize\build rmdir /S /Q cmake\UnicodeParseSerialize\build
if exist cmake\UnicodePerformance\build_x64 rmdir /S /Q cmake\UnicodePerformance\build_x64
if exist cmake\UnicodePerformance\build rmdir /S /Q cmake\UnicodePerformance\build
if exist cmake\ModifyingXMPHistory\build_x64 rmdir /S /Q cmake\ModifyingXMPHistory\build_x64
if exist cmake\ModifyingXMPHistory\build rmdir /S /Q cmake\ModifyingXMPHistory\build


if exist vc16 rmdir /S /Q vc16
if exist ..\target\windows rmdir /S /Q ..\target\windows
if exist ..\target\windows_x64 rmdir /S /Q ..\target\windows_x64

echo "Done"
echo.
echo. 
GOTO Start

:64VC2019
set GENERATOR=Visual Studio 16 2019
set DIR=vc16\windows_x64
set bit64=1
IF "%choice%"=="2" set static=1
IF "%choice%"=="3" set static=0
GOTO GenerateNow


:GenerateNow

IF NOT exist %DIR% mkdir %DIR%
cd %DIR%
if errorlevel 1 ( ECHO Cannot create folder %DIR% for generating project
goto error)
ECHO "%CMAKE%"  ..\..\cmake\ -G"%GENERATOR%" -A x64 -DCMAKE_CL_64=%bit64% -DSTATIC=%static%
"%CMAKE%"  ..\..\cmake\ -G"%GENERATOR%" -DCMAKE_CL_64=%bit64% -DSTATIC=%static%
if errorlevel 1 ( cd ..\..\
goto error)
goto ok


:error
echo CMake Build Failed.
pause
exit /B 0


:ok
cd ..\..\
echo CMake Build Success.
echo. 
echo.
GOTO Start