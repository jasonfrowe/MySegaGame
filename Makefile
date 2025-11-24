# Makefile wrapper for CMake build system
# This provides traditional make commands that invoke the CMake build

.PHONY: all clean rebuild debug release run help

# Default target
all: release

# Release build
release:
	@./build.sh --release

# Debug build  
debug:
	@./build.sh --debug

# Clean build directory
clean:
	@echo "Cleaning build directory..."
	@rm -rf build/

# Rebuild from scratch
rebuild: clean
	@./build.sh --clean --release

# Rebuild debug from scratch
rebuild-debug: clean
	@./build.sh --clean --debug

# Run in emulator (requires SGDK_EMULATOR environment variable)
run: release
	@if [ -z "$$SGDK_EMULATOR" ]; then \
		echo "Error: SGDK_EMULATOR environment variable not set"; \
		echo "Set it to your emulator path, e.g.: export SGDK_EMULATOR=/path/to/emulator"; \
		exit 1; \
	fi
	@$$SGDK_EMULATOR build/out.bin

# Show help
help:
	@echo "MySegaGame - Makefile Commands"
	@echo "=============================="
	@echo ""
	@echo "Available targets:"
	@echo "  make             - Build release version (default)"
	@echo "  make release     - Build release version"
	@echo "  make debug       - Build debug version"
	@echo "  make clean       - Remove build directory"
	@echo "  make rebuild     - Clean and build release"
	@echo "  make rebuild-debug - Clean and build debug"
	@echo "  make run         - Build and run in emulator"
	@echo "  make help        - Show this help message"
	@echo ""
	@echo "Environment variables:"
	@echo "  SGDK_EMULATOR    - Path to Genesis emulator (for 'make run')"
	@echo ""
