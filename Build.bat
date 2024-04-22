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
    Source\obj_file.cpp ^
    Source\mesh.cpp

set compiler_flags= -nologo -Oi -Od -Zi -FC -FoObj\
set compiler_defines=
set compiler_includes= /ISource\ /IThird_Party\glfw-3.4\include\

set linker_flags= -incremental:no -opt:ref
set libs=Kernel32.lib DbgHelp.lib Shell32.lib User32.lib Opengl32.lib Gdi32.lib ^
    Third_Party\glfw-3.4\lib-windows-vc2022\glfw3_mt.lib

if ["%1"]==["-vulkan"] (
    echo Compiling for Vulkan backend -> ScopVk.exe

    set output_filename=ScopVk.exe
    set source_files= %source_files% Source\vulkan_backend.cpp
    set compiler_defines= %compiler_defines% /DSCOP_BACKEND_VULKAN
    set compiler_includes= %compiler_includes% /I%VULKAN_SDK%\Include\
    set libs= %libs% %VULKAN_SDK%\Lib\vulkan-1.lib
) else (
    echo Compiling for OpenGL backend -> ScopGL.exe

    set output_filename=ScopGL.exe
    set source_files= %source_files% Source\opengl_backend.cpp Third_Party\glad\src\glad.c
    set compiler_defines= %compiler_defines% /DSCOP_BACKEND_OPENGL
    set compiler_includes= %compiler_includes% /IThird_Party\glad\include\
)

set compiler_options= %compiler_flags% %compiler_defines% %compiler_includes%
set linker_options= %libs% %linker_flags%

if not exist Obj\ (
    mkdir Obj
)

cl %compiler_options% %source_files% /link %linker_options% -OUT:%output_filename%
