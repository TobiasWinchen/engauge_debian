@echo off

rem this script builds Engauge Digitizer for MS Windows platforms
rem usage: release_windows.bat version_number_prefix version_number_suffix [build_directory]
rem author: mark mitchell

rem release directory %releaseDir% is one level below the %releaseRootDir% directory, to be
rem consistent with the linux and osx environments where the release directory ends up in
rem the user's current directory when he/she untars!
set releaseRootDir=releaseRoot

rem parse optional command line argument to override directory defaults
if "%1" == "" goto usage
if "%2" == "" goto usage
set version=%1_%2
if "%3" == "" goto noDirectory
   set releaseRootDir=%3%
:noDirectory
set releaseDir=%releaseRootDir%\engauge

rem make sure executable exists in bin directory
if not exist bin\engauge.exe goto missingExe

set zipFile=..\digit-exe-windows-%version%.zip

echo
echo Version number: %version%
echo Building release in directory: %releaseDir%
echo Writing to zip file: %zipFile%

rem clean up release directory. use prompt since empty entry would delete drive
del /s /q %releaseDir%

rem make directory tree
echo Recreating release directory...
mkdir %releaseDir%
mkdir %releaseDir%\samples
mkdir %releaseDir%\usermanual

rem copy files. note that all wildcard copies are performed with file suffixes
rem so the CVS subdirectory is not copied
echo Copying files...
copy bin\engauge.exe %releaseDir%
copy LICENSE %releaseDir%
copy samples\*.bmp %releaseDir%\samples
copy samples\*.gif %releaseDir%\samples
copy samples\*.jpg %releaseDir%\samples
copy samples\*.png %releaseDir%\samples
copy usermanual\*.gif %releaseDir%\usermanual
copy usermanual\*.html %releaseDir%\usermanual
copy usermanual\*.jpg %releaseDir%\usermanual
copy usermanual\*.png %releaseDir%\usermanual

rem we made it this far, so finish up by zipping up the release
echo Writing zip file...
"\program files\winzip\winzip64.exe" -a -r %zipFile% %releaseRootDir%

goto end

:usage
echo Usage: release_windows.bat version_number_prefix version_number_suffix [build_directory]
goto end

:missingExe
echo executable has not been created. see RELEASE file
goto end

:end
