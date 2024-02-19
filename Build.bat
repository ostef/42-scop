@echo off

setlocal EnableDelayedExpansion

:: Find cl.exe and if not found call vcvarsall to setup the environment variables

where /Q cl.exe || (
	set __VSCMD_ARG_NO_LOGO=1
	for /f "tokens=*" %%i in ('"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.VisualStudio.Workload.NativeDesktop -property installationPath') do set VS=%%i
	if "!VS!" equ "" (
		echo ERROR: Visual Studio installation not found
		exit /b 1
	)
	echo INFO: Calling vcvarsall.bat for amd64
	echo INFO: Do this manually before calling the script to prevent this call every time you build and save time
	echo INFO: You can call !VS!\VC\Auxiliary\Build\vcvarsall.bat amd64
	call "!VS!\VC\Auxiliary\Build\vcvarsall.bat" amd64 || exit /b 1
)

set source_files=Source\main.cpp ^
    Source\math.cpp ^
    Source\vulkan_backend.cpp ^
    Source\obj_file.cpp

set output_filename=Scop.exe

set compiler_flags= -nologo -Oi -Od -Zi -FC
set compiler_defines=
set compiler_includes= /ISource\ /IThird_Party\ ^
    /IThird_Party\glfw-3.3.9\include\ ^
    /I%VULKAN_SDK%\Include\

set compiler_options= %compiler_flags% %compiler_defines% %compiler_includes%

set linker_flags= -incremental:no -opt:ref
set libs=Kernel32.lib DbgHelp.lib Shell32.lib User32.lib Opengl32.lib Gdi32.lib ^
    Third_Party\glfw-3.3.9\lib-vc2022\glfw3_mt.lib ^
    %VULKAN_SDK%\Lib\vulkan-1.lib

set linker_options= %libs% %linker_flags%

cl %compiler_options% %source_files% /link %linker_options% -OUT:%output_filename%
