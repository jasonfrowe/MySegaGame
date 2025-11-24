#!/bin/bash

# Simple build script using SGDK's traditional makefile system
# This works with the SGDK Docker container

set -e

# Configuration
DOCKER_IMAGE="ghcr.io/stephane-d/sgdk:latest"
PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

print_msg() {
    echo -e "${GREEN}==>${NC} $1"
}

print_error() {
    echo -e "${RED}Error:${NC} $1" >&2
}

# Check Docker
if ! command -v docker &> /dev/null; then
    print_error "Docker is not installed"
    exit 1
fi

# Pull image if needed
print_msg "Checking SGDK Docker image..."
if ! docker image inspect ${DOCKER_IMAGE} &> /dev/null; then
    print_msg "Pulling SGDK Docker image..."
    docker pull ${DOCKER_IMAGE}
fi

print_msg "Building with SGDK..."
docker run --rm --platform linux/amd64 \
    -v "${PROJECT_DIR}:/project" \
    ${DOCKER_IMAGE}

if [ $? -eq 0 ]; then
    if [ -f "${PROJECT_DIR}/out/rom.bin" ]; then
        print_msg "Build successful!"
        print_msg "ROM: out/rom.bin"
        ROM_SIZE=$(stat -f%z "${PROJECT_DIR}/out/rom.bin" 2>/dev/null || stat -c%s "${PROJECT_DIR}/out/rom.bin" 2>/dev/null)
        print_msg "Size: $((ROM_SIZE / 1024)) KB"
    else
        print_error "Build completed but ROM not found"
        exit 1
    fi
else
    print_error "Build failed"
    exit 1
fi
