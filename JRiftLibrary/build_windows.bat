@ECHO OFF
SET CURDIR=%CD%

:: Visual Studio 10 (2010)
SET VISUALSTUDIO_VER=10

:: Note: %~dp0 gives the current directory containing build_windows.bat
:: In this case it's also the cmake root source directory.,

:: cmake 2.8 or greater should be installed. Will use CMakeLists.txt in root 
:: of JRiftLibrary.

:: Java 1.6 or greater (32 or 64bit - doesn't matter which I believe) needs 
:: to be installed and in your path for the JNI headers to be found.

:: Out-of-source win64 cmake configuration
rmdir "%~dp0\win64" /S /Q
mkdir "%~dp0\win64"
cd /D "%~dp0\win64"
cmake -G "Visual Studio %VISUALSTUDIO_VER% Win64" ../ -Wno-dev

:: Out-of-source win32 cmake configuration
rmdir "%~dp0\win32" /S /Q
mkdir "%~dp0\win32"
cd /D "%~dp0\win32"
cmake -G "Visual Studio %VISUALSTUDIO_VER%" ../ -Wno-dev
cd /D %CURDIR%

:: MSbuild path finder shamelessly ripped from JJS, Stack-overflow. MSBuild 4 or higher required.

reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\4.0" /v MSBuildToolsPath > nul 2>&1
if ERRORLEVEL 1 goto MissingMSBuildRegistry

for /f "skip=2 tokens=2,*" %%A in ('reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\4.0" /v MSBuildToolsPath') do SET MSBUILDDIR=%%B

IF NOT EXIST %MSBUILDDIR%nul goto MissingMSBuildToolsPath
IF NOT EXIST %MSBUILDDIR%msbuild.exe goto MissingMSBuildExe

"%MSBUILDDIR%msbuild.exe" /version

:: Visual Studio 2012 or greater currently required.

del /F /Q "%~dp0\natives\windows\JRiftLibrary*.dll"

:: Build Win64 dll
"%MSBUILDDIR%msbuild.exe" "%~dp0\win64\JRiftLibrary.sln" /t:JRiftLibrary64:clean;rebuild /p:Configuration=Release;Platform=x64 /v:normal

:: Build Win32 dll
"%MSBUILDDIR%msbuild.exe" "%~dp0\win32\JRiftLibrary.sln" /t:JRiftLibrary:clean;rebuild /p:Configuration=Release;Platform=Win32 /v:normal

:: dlls will have been built to JRiftLibrary\natives\windows

cd /D %CURDIR%

goto:eof
::ERRORS
::---------------------
:MissingMSBuildRegistry
echo Cannot obtain path to MSBuild tools from registry
goto:eof
:MissingMSBuildToolsPath
echo The MSBuild tools path from the registry '%MSBUILDDIR%' does not exist
goto:eof
:MissingMSBuildExe
echo The MSBuild executable could not be found at '%MSBUILDDIR%'
goto:eof

