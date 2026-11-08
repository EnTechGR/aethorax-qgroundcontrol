@echo off
rem AETHORAX QGroundControl: the Windows build, as QGroundControl's CI does it
rem (custom/README.md, "Build"). Run from a normal Command Prompt:
rem
rem     custom\tools\windows-build.cmd            a test build ("... Daily")
rem     custom\tools\windows-build.cmd --stable   a release build (the version from the git tag)
rem
rem Needs, once (custom/README.md, "Windows build tools"): Visual Studio 2022 Build Tools (C++),
rem NSIS and GStreamer from tools\setup\install_dependencies, Qt in C:\Qt from tools\setup\install_qt.py,
rem and Python 3.12. The result: build\AETHORAX-QGroundControl-installer*.exe.
setlocal
rem cmd.exe itself: "for /f" (used by Visual Studio's scripts) starts it through COMSPEC,
rem and an installer that broke COMSPEC would stop the build without a word
set "COMSPEC=%SystemRoot%\system32\cmd.exe"
set "SOURCE=%~dp0..\.."
pushd "%SOURCE%" || exit /b 1

if not defined QT_ROOT_DIR set "QT_ROOT_DIR=C:\Qt\6.11.1\msvc2022_64"
if not defined GSTREAMER_1_0_ROOT_MSVC_X86_64 set "GSTREAMER_1_0_ROOT_MSVC_X86_64=C:\gstreamer\1.0\msvc_x86_64\"
if not defined PYTHON set "PYTHON=%LOCALAPPDATA%\Programs\Python\Python312\python.exe"
set "STABLE="
if /i "%~1"=="--stable" set "STABLE=--stable"

if not exist "%QT_ROOT_DIR%\bin\qmake.exe" (echo Qt not found at %QT_ROOT_DIR% & goto :fail)
if not exist "%GSTREAMER_1_0_ROOT_MSVC_X86_64%bin\gst-launch-1.0.exe" (echo GStreamer not found at %GSTREAMER_1_0_ROOT_MSVC_X86_64% & goto :fail)

rem The Visual Studio C++ compiler, CMake and Ninja (vswhere's answer through a file: in a
rem "for /f", the ")" of "(x86)" and the quotes break the line)
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath > "%TEMP%\aethorax-vs.txt"
set /p VSINSTALL=<"%TEMP%\aethorax-vs.txt"
if not defined VSINSTALL (echo Visual Studio 2022 Build Tools with C++ not found & goto :fail)
call "%VSINSTALL%\VC\Auxiliary\Build\vcvars64.bat" >nul || goto :fail

rem The Python the build's code generators use (jinja2)
if not exist ".venv\Scripts\python.exe" ("%PYTHON%" -m venv .venv || goto :fail)
".venv\Scripts\python.exe" -m pip install --quiet --disable-pip-version-check jinja2 defusedxml httpx || goto :fail

set "PATH=%GSTREAMER_1_0_ROOT_MSVC_X86_64%bin;%PATH%"

".venv\Scripts\python.exe" tools\configure.py -S . -B build -t Release %STABLE% --qt-root "%QT_ROOT_DIR%" -- -DQGC_ENABLE_GST_VIDEOSTREAMING=ON "-DPython3_EXECUTABLE=%CD%\.venv\Scripts\python.exe" || goto :fail
cmake --build build --config Release || goto :fail
cmake --install build --config Release || goto :fail

echo.
dir /b build\*installer*.exe
popd
exit /b 0

:fail
echo AETHORAX: the Windows build failed
popd
exit /b 1
