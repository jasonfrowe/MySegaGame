# CMake Build System Setup - Summary

## What Was Created

Your Sega Genesis SGDK project now has a modern CMake build system with Docker integration. Here's what was added:

### Core Build Files

1. **`CMakeLists.txt`** - Main CMake build configuration
   - Defines project structure
   - Manages source files
   - Handles resource compilation
   - Configures compiler and linker settings
   - Generates ROM binary

2. **`cmake/SGDKToolchain.cmake`** - Cross-compilation toolchain
   - Configures m68k-elf-gcc compiler
   - Sets SGDK paths (works inside Docker container)
   - Defines compiler flags for Genesis/Mega Drive

3. **`cmake/SGDK.cmake`** - Helper functions module
   - `sgdk_compile_resources()` - Compiles .res files with rescomp
   - `sgdk_compile_z80()` - Assembles Z80 sound code
   - `sgdk_validate_environment()` - Checks SGDK setup

### Build Scripts

4. **`build.sh`** (macOS/Linux) - Main build script
   - Runs CMake inside SGDK Docker container
   - Handles configuration and compilation
   - Supports multiple build modes
   - Usage: `./build.sh [--clean] [--debug|--release] [--verbose]`

5. **`build.bat`** (Windows) - Windows build script
   - Same functionality as build.sh for Windows
   - Usage: `build.bat [options]`

6. **`Makefile`** - Traditional make interface
   - Wrapper around build scripts
   - Provides familiar make commands
   - Usage: `make`, `make clean`, `make debug`, etc.

### Configuration Files

7. **`CMakePresets.json`** - CMake presets
   - Predefined build configurations
   - IDE integration support
   - Configurations: default, debug, release

8. **`.vscode/tasks.json`** - VS Code tasks
   - Build tasks for VS Code
   - Keyboard shortcuts integration
   - Problem matcher for error detection

9. **`.vscode/launch.json`** - VS Code launcher
   - ROM execution configuration
   - Emulator integration

### Documentation

10. **`CMAKE_BUILD.md`** - Comprehensive build documentation
    - Detailed usage instructions
    - Architecture explanation
    - Advanced customization guide
    - Troubleshooting tips

11. **`QUICKSTART.md`** - Quick reference guide
    - 30-second build instructions
    - Common commands table
    - Quick troubleshooting

12. **`.gitignore`** - Updated with CMake artifacts
    - Ignores build/ directory
    - Ignores generated files
    - Keeps cmake/ configuration files

## How It Works

### Build Flow

```
1. Run build script (build.sh or build.bat)
   ↓
2. Docker pulls SGDK image (if needed)
   ↓
3. CMake configures project inside container
   ↓
4. Resources compiled (res/resources.res → .s/.h files)
   ↓
5. C sources compiled to object files
   ↓
6. Objects linked with SGDK library
   ↓
7. ELF converted to binary ROM
   ↓
8. Output: build/out.bin (your ROM!)
```

### Docker Integration

The build system uses the official SGDK Docker image:
- **Image**: `ghcr.io/stephane-d/sgdk:latest`
- **Contains**: Complete SGDK toolchain (gcc, rescomp, tools)
- **Benefit**: No local installation needed
- **Platform**: Works on macOS, Linux, Windows

### CMake Structure

```
CMakeLists.txt
├── Project definition
├── Toolchain configuration (via SGDKToolchain.cmake)
├── Source file gathering
│   ├── Game sources (src/*.c)
│   ├── Boot files (src/boot/*)
│   └── Resources (res/resources.res)
├── Resource compilation (via SGDK.cmake)
├── Executable target
│   ├── Include directories
│   ├── Compiler flags
│   └── Linker configuration
└── Post-build steps
    ├── ELF → BIN conversion
    ├── Symbol file generation
    └── Size reporting
```

## Key Features

### ✅ Docker-Based Build
- No manual SGDK installation
- Consistent builds across platforms
- Easy toolchain updates

### ✅ Modern CMake Practices
- Out-of-source builds
- Proper dependency tracking
- Automatic resource recompilation
- Parallel compilation support

### ✅ Multiple Build Configurations
- **Release**: Optimized for size and speed (-O3)
- **Debug**: Symbols and debugging info (-O0 -ggdb)

### ✅ IDE Integration
- VS Code tasks and launch configs
- CMake presets for all IDEs
- IntelliSense support

### ✅ Developer Friendly
- Simple build scripts
- Traditional make commands
- Verbose mode for debugging
- Clean build support

## Build Outputs

After a successful build, you'll have:

```
build/
├── out.bin         # Final ROM (run in emulator)
├── out.elf         # ELF executable (with symbols)
├── out.map         # Linker map file
├── out.sym         # Symbol table
└── res/
    ├── resources.s # Compiled resources (ASM)
    └── resources.h # Resource headers
```

## Usage Examples

### Basic Build
```bash
./build.sh
```

### Clean Build
```bash
./build.sh --clean
```

### Debug Build
```bash
./build.sh --debug
```

### Using Make
```bash
make              # Build release
make debug        # Build debug
make clean        # Clean
make rebuild      # Clean + build
```

### Manual CMake
```bash
# Configure
docker run --rm -v "$PWD:/project" -w /project ghcr.io/stephane-d/sgdk:latest \
  cmake -S /project -B /project/build -DCMAKE_TOOLCHAIN_FILE=/project/cmake/SGDKToolchain.cmake

# Build
docker run --rm -v "$PWD:/project" -w /project ghcr.io/stephane-d/sgdk:latest \
  cmake --build /project/build
```

## Customization

### Change ROM Name
Edit `CMakeLists.txt`:
```cmake
set(ROM_NAME "mygame" CACHE STRING "Name of the output ROM file")
```

### Add Compiler Flags
Edit `CMakeLists.txt`:
```cmake
target_compile_options(${PROJECT_NAME} PRIVATE
    -myFlag
)
```

### Custom Resource Processing
The `sgdk_compile_resources()` function can be called multiple times:
```cmake
sgdk_compile_resources(
    RES_FILE ${CMAKE_SOURCE_DIR}/res/music.res
    OUTPUT_DIR "${CMAKE_BINARY_DIR}/res"
    OUTPUT_SOURCES MUSIC_SOURCES
    OUTPUT_HEADERS MUSIC_HEADERS
)
```

## Advantages Over Traditional Makefile

| Feature | CMake | Traditional Makefile |
|---------|-------|---------------------|
| Cross-platform | ✅ Works everywhere | ❌ Platform-specific |
| IDE integration | ✅ Native support | ⚠️ Limited |
| Dependency tracking | ✅ Automatic | ⚠️ Manual |
| Out-of-source builds | ✅ Yes | ❌ Usually in-source |
| Configuration | ✅ Cache & presets | ⚠️ Environment vars |
| Parallel builds | ✅ Automatic | ⚠️ Manual -j flag |

## Migration Notes

### From SGDK Makefile
If you previously used SGDK's makefile.gen:

**Old:**
```bash
make -f $GDK/makefile.gen
```

**New:**
```bash
./build.sh
```

### Resource Files
No changes needed! Your existing `res/resources.res` works as-is.

### Source Code
No changes needed! Your C code compiles exactly the same.

## Next Steps

1. **Test the build**: Run `./build.sh`
2. **Test your ROM**: Load `build/out.bin` in an emulator
3. **Set up your editor**: Install CMake Tools in VS Code
4. **Configure emulator**: Set `SGDK_EMULATOR` environment variable
5. **Start developing**: Edit code, run `./build.sh`, test, repeat!

## Support & Resources

- **SGDK GitHub**: https://github.com/Stephane-D/SGDK
- **SGDK Wiki**: https://github.com/Stephane-D/SGDK/wiki
- **SGDK Discord**: https://discord.gg/xmnBWQS
- **CMake Docs**: https://cmake.org/documentation/
- **Genesis Dev**: https://plutiedev.com/

## Version Information

- **CMake**: Minimum version 3.22
- **SGDK**: Latest (via Docker)
- **Docker Image**: ghcr.io/stephane-d/sgdk:latest
- **Toolchain**: m68k-elf-gcc (included in SGDK)

---

**Your project is now ready for modern CMake-based development! 🎮**
