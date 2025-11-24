#!/bin/bash

# Test script to verify CMake build system setup
# This checks that all necessary components are in place

set -e

echo "====================================="
echo "CMake Build System - Setup Verification"
echo "====================================="
echo ""

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

pass() {
    echo -e "${GREEN}✓${NC} $1"
}

fail() {
    echo -e "${RED}✗${NC} $1"
    exit 1
}

# Check Docker
echo "Checking Docker..."
if command -v docker &> /dev/null; then
    pass "Docker is installed"
    if docker ps &> /dev/null; then
        pass "Docker daemon is running"
    else
        fail "Docker daemon is not running - please start Docker"
    fi
else
    fail "Docker is not installed - install from https://www.docker.com/"
fi
echo ""

# Check SGDK Docker image
echo "Checking SGDK Docker image..."
if docker image inspect ghcr.io/stephane-d/sgdk:latest &> /dev/null; then
    pass "SGDK Docker image is available"
else
    echo "Pulling SGDK Docker image (this may take a while)..."
    docker pull ghcr.io/stephane-d/sgdk:latest
    pass "SGDK Docker image downloaded"
fi
echo ""

# Check file structure
echo "Checking project files..."
[ -f "CMakeLists.txt" ] && pass "CMakeLists.txt exists" || fail "CMakeLists.txt missing"
[ -f "cmake/SGDKToolchain.cmake" ] && pass "SGDKToolchain.cmake exists" || fail "SGDKToolchain.cmake missing"
[ -f "cmake/SGDK.cmake" ] && pass "SGDK.cmake exists" || fail "SGDK.cmake missing"
[ -f "build.sh" ] && pass "build.sh exists" || fail "build.sh missing"
[ -x "build.sh" ] && pass "build.sh is executable" || fail "build.sh not executable (run: chmod +x build.sh)"
[ -f "Makefile" ] && pass "Makefile exists" || fail "Makefile missing"
echo ""

# Check source files
echo "Checking source files..."
[ -d "src" ] && pass "src/ directory exists" || fail "src/ directory missing"
[ -d "inc" ] && pass "inc/ directory exists" || fail "inc/ directory missing"
[ -d "res" ] && pass "res/ directory exists" || fail "res/ directory missing"
[ -f "src/main.c" ] && pass "src/main.c exists" || fail "src/main.c missing"
[ -f "res/resources.res" ] && pass "res/resources.res exists" || fail "res/resources.res missing"
echo ""

# Check .gitignore
echo "Checking .gitignore..."
if [ -f ".gitignore" ]; then
    if grep -q "build/" ".gitignore"; then
        pass ".gitignore includes build/"
    else
        echo "Warning: .gitignore doesn't include build/"
    fi
fi
echo ""

# Test CMake configuration (dry run)
echo "Testing CMake configuration..."
if docker run --rm -v "$PWD:/project" -w /project ghcr.io/stephane-d/sgdk:latest \
    cmake --version &> /dev/null; then
    pass "CMake is available in Docker container"
else
    fail "CMake not available in Docker container"
fi
echo ""

# Optional: Test actual build (commented out by default)
echo "Build test skipped (uncomment in test-setup.sh to enable)"
# echo "Attempting test build..."
# ./build.sh
# [ -f "build/out.bin" ] && pass "Test build successful - ROM created" || fail "Test build failed"

echo ""
echo "====================================="
echo -e "${GREEN}Setup verification complete!${NC}"
echo "====================================="
echo ""
echo "Your CMake build system is ready to use."
echo ""
echo "Next steps:"
echo "  1. Run: ./build.sh"
echo "  2. Test: build/out.bin in your emulator"
echo ""
echo "For more information, see:"
echo "  - QUICKSTART.md (quick reference)"
echo "  - CMAKE_BUILD.md (detailed documentation)"
echo "  - CMAKE_SETUP_SUMMARY.md (what was installed)"
echo ""
