# CMake Build System for MySegaGame

This project now uses CMake as its build system, integrated with the SGDK (Sega Genesis Development Kit) via Docker.

## Prerequisites

- **Docker**: Required to run the SGDK toolchain
  - Install from: https://www.docker.com/
  - The build scripts will automatically pull the SGDK Docker image: `ghcr.io/stephane-d/sgdk:latest`

- **CMake** (Optional): Only needed if building manually outside of Docker
  - Version 3.22 or higher
  - Install from: https://cmake.org/download/

## Quick Start

### Building the ROM

**On macOS/Linux:**
```bash
./build.sh
```

**On Windows:**
```cmd
build.bat
```

This will:
1. Pull the SGDK Docker image (if not already available)
2. Configure the project with CMake
3. Compile all source files
4. Process resources using rescomp
5. Generate the ROM file: `build/out.bin`

### Build Options

**Clean build:**
```bash
./build.sh --clean
```

**Debug build:**
```bash
./build.sh --debug
```

**Release build (default):**
```bash
./build.sh --release
```

**Verbose output:**
```bash
./build.sh --verbose
```

**Show help:**
```bash
./build.sh --help
```

## Project Structure

```
MySegaGame/
├── CMakeLists.txt              # Main CMake configuration
├── build.sh                    # Build script for macOS/Linux
├── build.bat                   # Build script for Windows
├── cmake/
│   ├── SGDKToolchain.cmake     # Toolchain file for cross-compilation
│   └── SGDK.cmake              # Helper functions for SGDK resources
├── src/
│   ├── main.c
│   ├── *.c                     # Game source files
│   └── boot/
│       ├── rom_head.c
│       └── sega.s
├── inc/
│   └── *.h                     # Header files
├── res/
│   ├── resources.res           # Resource definition file
│   └── ...                     # Resource files (images, music, etc.)
└── build/                      # Build output (generated)
    ├── out.bin                 # Final ROM file
    ├── out.elf                 # ELF executable
    ├── out.map                 # Linker map file
    └── out.sym                 # Symbol file
```

## CMake Build System Details

### Main Components

1. **CMakeLists.txt**: The main build configuration that:
   - Defines the project and version
   - Gathers source files
   - Compiles SGDK resources
   - Links against SGDK library
   - Generates the ROM binary

2. **cmake/SGDKToolchain.cmake**: Cross-compilation toolchain file that:
   - Configures the m68k-elf-gcc compiler
   - Sets up SGDK paths and tools
   - Defines compiler and linker flags

3. **cmake/SGDK.cmake**: Helper module providing:
   - `sgdk_compile_resources()`: Compiles .res files using rescomp
   - `sgdk_compile_z80()`: Assembles Z80 sound driver code
   - `sgdk_validate_environment()`: Checks SGDK installation

### Build Targets

- **Default target**: Builds the ROM file
- **clean-all**: Removes all build artifacts
- **run**: Runs the ROM in an emulator (if SGDK_EMULATOR is set)
- **install**: Copies the ROM to the install directory

### Configuration Options

You can customize the build by setting CMake variables:

```bash
# Custom ROM name
docker run --rm -v "$PWD:/project" -w /project ghcr.io/stephane-d/sgdk:latest \
  cmake -S /project -B /project/build -DROM_NAME=mygame
```

## Manual Build (Without Scripts)

If you prefer to run CMake commands directly:

### Configure
```bash
docker run --rm -v "$PWD:/project" -w /project ghcr.io/stephane-d/sgdk:latest \
  cmake -S /project -B /project/build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=/project/cmake/SGDKToolchain.cmake
```

### Build
```bash
docker run --rm -v "$PWD:/project" -w /project ghcr.io/stephane-d/sgdk:latest \
  cmake --build /project/build -j$(nproc)
```

## Resource Compilation

Resources are defined in `res/resources.res` and are automatically compiled during the build process. The CMake system:

1. Detects changes to `resources.res`
2. Runs `rescomp` to generate `.s` and `.h` files
3. Includes the generated assembly in the build
4. Makes headers available to C source files

## Debugging

### Debug Build

Build with debug symbols:
```bash
./build.sh --debug
```

This creates a debug build with:
- Optimization disabled (-O0)
- Debug symbols enabled (-ggdb)
- DEBUG macro defined

### Symbol File

The build generates `build/out.sym` containing all symbols, useful for:
- Debugging with emulators
- Understanding memory layout
- Analyzing ROM structure

### Map File

The linker map file `build/out.map` shows:
- Memory sections
- Symbol addresses
- File contributions to ROM size

## Environment Variables

- **BUILD_TYPE**: Set to `Debug` or `Release` (default: Release)
- **SGDK_EMULATOR**: Path to emulator for the `run` target

Example:
```bash
BUILD_TYPE=Debug ./build.sh
```

## Troubleshooting

### Docker Image Issues

If you encounter issues with the Docker image:
```bash
# Pull the latest image
docker pull ghcr.io/stephane-d/sgdk:latest

# Clean build
./build.sh --clean
```

### Build Failures

1. Check that Docker is running
2. Ensure you have the latest SGDK Docker image
3. Try a clean build: `./build.sh --clean`
4. Check for syntax errors in your C code or resources.res

### Path Issues on Windows

If you encounter path-related issues on Windows, ensure:
- Your project is not in a path with spaces
- You're running from the project root directory

## Advanced Usage

### Adding New Source Files

Source files are automatically detected by CMake. After adding new `.c` files to `src/`, just run the build script again.

### Adding New Resources

1. Add resource files to `res/` directory
2. Update `res/resources.res` with new resource definitions
3. Rebuild - CMake will automatically recompile resources

### Custom Compiler Flags

To add custom compiler flags, edit `CMakeLists.txt`:

```cmake
target_compile_options(${PROJECT_NAME} PRIVATE
    -myCustomFlag
)
```

## Integration with IDEs

### Visual Studio Code

1. Install the CMake Tools extension
2. Configure Docker integration
3. Open the project folder
4. Select the SGDK toolchain when prompted

### CLion

1. Open project
2. Configure toolchain in Settings → Build → Toolchains
3. Set CMake options to use the Docker container

## References

- SGDK Documentation: https://github.com/Stephane-D/SGDK
- SGDK Wiki: https://github.com/Stephane-D/SGDK/wiki
- CMake Documentation: https://cmake.org/documentation/
- Sega Genesis Dev Resources: https://plutiedev.com/

## License

This build system configuration is provided as-is. See the main project README for game licensing information.
