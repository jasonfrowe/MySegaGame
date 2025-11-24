#!/bin/bash

# Build script for MySegaGame using SGDK Docker container
# This script runs CMake inside the SGDK Docker container

set -e  # Exit on error

# Configuration
DOCKER_IMAGE="sgdk-cmake:latest"
DOCKER_BASE_IMAGE="ghcr.io/stephane-d/sgdk:latest"
PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
BUILD_TYPE="${BUILD_TYPE:-Release}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored message
print_msg() {
    echo -e "${GREEN}==>${NC} $1"
}

print_error() {
    echo -e "${RED}Error:${NC} $1" >&2
}

print_warning() {
    echo -e "${YELLOW}Warning:${NC} $1"
}

# Check if Docker is available
if ! command -v docker &> /dev/null; then
    print_error "Docker is not installed or not in PATH"
    print_msg "Please install Docker from: https://www.docker.com/"
    exit 1
fi

# Pull the latest SGDK image if needed and build custom image
print_msg "Checking SGDK Docker image..."
if ! docker image inspect ${DOCKER_IMAGE} &> /dev/null; then
    print_msg "Building custom SGDK+CMake Docker image..."
    # Pull base image first
    docker pull --platform linux/amd64 ${DOCKER_BASE_IMAGE}
    # Build custom image
    docker build --platform linux/amd64 -t ${DOCKER_IMAGE} "${PROJECT_DIR}" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        print_error "Failed to build custom Docker image"
        exit 1
    fi
fi

# Create build directory
mkdir -p "${BUILD_DIR}"

# Parse command line arguments
CLEAN=0
VERBOSE=0
CMAKE_ARGS=""

while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--clean)
            CLEAN=1
            shift
            ;;
        -v|--verbose)
            VERBOSE=1
            CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_VERBOSE_MAKEFILE=ON"
            shift
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -c, --clean     Clean build directory before building"
            echo "  -v, --verbose   Enable verbose build output"
            echo "  -d, --debug     Build debug version"
            echo "  -r, --release   Build release version (default)"
            echo "  -h, --help      Show this help message"
            echo ""
            echo "Environment variables:"
            echo "  BUILD_TYPE      Set build type (Release or Debug)"
            echo ""
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            echo "Use -h or --help for usage information"
            exit 1
            ;;
    esac
done

# Clean if requested
if [ ${CLEAN} -eq 1 ]; then
    print_msg "Cleaning build directory..."
    rm -rf "${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}"
fi

print_msg "Building MySegaGame (${BUILD_TYPE})..."
print_msg "Project directory: ${PROJECT_DIR}"
print_msg "Build directory: ${BUILD_DIR}"

# Run CMake configuration inside Docker
print_msg "Configuring with CMake..."
docker run --rm --platform linux/amd64 \
    -v "${PROJECT_DIR}:/project" \
    -w /project \
    ${DOCKER_IMAGE} \
    -c "cmake -S /project -B /project/build -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_TOOLCHAIN_FILE=/project/cmake/SGDKToolchain.cmake ${CMAKE_ARGS}"

if [ $? -ne 0 ]; then
    print_error "CMake configuration failed"
    exit 1
fi

# Build inside Docker
print_msg "Building project..."
docker run --rm --platform linux/amd64 \
    -v "${PROJECT_DIR}:/project" \
    -w /project \
    ${DOCKER_IMAGE} \
    -c "cmake --build /project/build --config ${BUILD_TYPE} -j\$(nproc 2>/dev/null || echo 4)"

if [ $? -ne 0 ]; then
    print_error "Build failed"
    exit 1
fi

# Check if ROM was created
ROM_FILE="${BUILD_DIR}/out.bin"
if [ -f "${ROM_FILE}" ]; then
    ROM_SIZE=$(stat -f%z "${ROM_FILE}" 2>/dev/null || stat -c%s "${ROM_FILE}" 2>/dev/null)
    print_msg "Build successful!"
    print_msg "ROM created: ${ROM_FILE}"
    print_msg "ROM size: $((ROM_SIZE / 1024)) KB"
    
    # Display additional build artifacts
    if [ -f "${BUILD_DIR}/out.map" ]; then
        print_msg "Map file: ${BUILD_DIR}/out.map"
    fi
    if [ -f "${BUILD_DIR}/out.sym" ]; then
        print_msg "Symbol file: ${BUILD_DIR}/out.sym"
    fi
else
    print_error "Build failed - ROM file not created"
    exit 1
fi

print_msg "Done!"
