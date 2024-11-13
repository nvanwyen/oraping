@echo off

cls
setlocal

:: save directories (launch/current)
set ldir=%~dp0
set cdir=%CD%

:: find the compiler
for /f "delims=" %%a in ( 'where devenv.exe' ) do @set cc=%%a

:: check for the compiler
if [%cc] == [] GOTO nocc

:: find project file
if not exist %ldir%\VERSION goto nodir

:: ensure out-of-source build directory
if not exist %ldir%\build (
md %ldir%\build
)

:: goto and cleean the build directory
cd %ldir%\build

:: clean up any old data
for /F "delims=" %%i in ('dir /b') do (rmdir "%%i" /s /q || del "%%i" /s /q)

:: run cmake
cmake -Wno-dev ..

:: check return code
if not errorlevel 0 goto failure

:: check that the solution file was created
if not exist %ldir%\build\oraping.sln goto nodir

:: compile Release
devenv oraping.sln /Build Release

:: check return code
if not errorlevel 0 goto falied

:: compile Debug
:: devenv oraping.sln /Build Debug

:: check return code
if not errorlevel 0 goto falied

:: all done
goto end

:nodir
echo Could not find project file %ldir%\VERSION
goto end

:nocc
echo Could not find the Visual Studio compiler (devenv.exe) in the path
goto end

:failure
echo Cmake out-of-source build failed
goto end

:falied
echo Solution falied to build
goto end

:end
cd %cdir%
:: end of batch
