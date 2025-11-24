@echo off
REM Build script for MySegaGame using SGDK Docker container
REM This script runs CMake inside the SGDK Docker container

setlocal enabledelayedexpansion

REM Configuration
set DOCKER_IMAGE=ghcr.io/stephane-d/sgdk:latest
set PROJECT_DIR=%~dp0
set BUILD_DIR=%PROJECT_DIR%build
set BUILD_TYPE=Release

REM Parse command line arguments
set CLEAN=0
set VERBOSE=0
set CMAKE_ARGS=

:parse_args
if "%~1"=="" goto end_parse_args
if /i "%~1"=="-c" (
    set CLEAN=1
    shift
    goto parse_args
)
if /i "%~1"=="--clean" (
    set CLEAN=1
    shift
    goto parse_args
)
if /i "%~1"=="-v" (
    set VERBOSE=1
    set CMAKE_ARGS=!CMAKE_ARGS! -DCMAKE_VERBOSE_MAKEFILE=ON
    shift
    goto parse_args
)
if /i "%~1"=="--verbose" (
    set VERBOSE=1
    set CMAKE_ARGS=!CMAKE_ARGS! -DCMAKE_VERBOSE_MAKEFILE=ON
    shift
    goto parse_args
)
if /i "%~1"=="-d" (
    set BUILD_TYPE=Debug
    shift
    goto parse_args
)
if /i "%~1"=="--debug" (
    set BUILD_TYPE=Debug
    shift
    goto parse_args
)
if /i "%~1"=="-r" (
    set BUILD_TYPE=Release
    shift
    goto parse_args
)
if /i "%~1"=="--release" (
    set BUILD_TYPE=Release
    shift
    goto parse_args
)
if /i "%~1"=="-h" goto show_help
if /i "%~1"=="--help" goto show_help

echo Unknown option: %~1
echo Use -h or --help for usage information
exit /b 1

:show_help
echo Usage: %~nx0 [OPTIONS]
echo.
echo Options:
echo   -c, --clean     Clean build directory before building
echo   -v, --verbose   Enable verbose build output
echo   -d, --debug     Build debug version
echo   -r, --release   Build release version (default)
echo   -h, --help      Show this help message
echo.
exit /b 0

:end_parse_args

REM Check if Docker is available
docker --version >nul 2>&1
if errorlevel 1 (
    echo Error: Docker is not installed or not in PATH
    echo Please install Docker from: https://www.docker.com/
    exit /b 1
)

REM Pull the latest SGDK image if needed
echo Checking SGDK Docker image...
docker image inspect %DOCKER_IMAGE% >nul 2>&1
if errorlevel 1 (
    echo Pulling SGDK Docker image (this may take a while)...
    docker pull %DOCKER_IMAGE%
)

REM Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Clean if requested
if %CLEAN%==1 (
    echo Cleaning build directory...
    rmdir /s /q "%BUILD_DIR%" 2>nul
    mkdir "%BUILD_DIR%"
)

echo Building MySegaGame (%BUILD_TYPE%)...
echo Project directory: %PROJECT_DIR%
echo Build directory: %BUILD_DIR%

REM Convert Windows paths to Unix-style for Docker
set UNIX_PROJECT_DIR=/project

REM Run CMake configuration inside Docker
echo Configuring with CMake...
docker run --rm --platform linux/amd64 ^
    -v "%PROJECT_DIR%:/project" ^
    -w /project ^
    %DOCKER_IMAGE% ^
    /bin/bash -c "cmake -S /project -B /project/build -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_TOOLCHAIN_FILE=/project/cmake/SGDKToolchain.cmake %CMAKE_ARGS%"

if errorlevel 1 (
    echo Error: CMake configuration failed
    exit /b 1
)

REM Build inside Docker
echo Building project...
docker run --rm --platform linux/amd64 ^
    -v "%PROJECT_DIR%:/project" ^
    -w /project ^
    %DOCKER_IMAGE% ^
    /bin/bash -c "cmake --build /project/build --config %BUILD_TYPE% -j4"

if errorlevel 1 (
    echo Error: Build failed
    exit /b 1
)

REM Check if ROM was created
set ROM_FILE=%BUILD_DIR%\out.bin
if exist "%ROM_FILE%" (
    echo Build successful!
    echo ROM created: %ROM_FILE%
    
    REM Display file size
    for %%A in ("%ROM_FILE%") do set ROM_SIZE=%%~zA
    set /a ROM_SIZE_KB=!ROM_SIZE! / 1024
    echo ROM size: !ROM_SIZE_KB! KB
    
    if exist "%BUILD_DIR%\out.map" echo Map file: %BUILD_DIR%\out.map
    if exist "%BUILD_DIR%\out.sym" echo Symbol file: %BUILD_DIR%\out.sym
) else (
    echo Error: Build failed - ROM file not created
    exit /b 1
)

echo Done!
exit /b 0
