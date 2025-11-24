# CMake Build System - File Index

This document provides a quick reference to all files in the CMake build system.

## 📁 Build System Files

### Core CMake Files
| File | Purpose | Edit? |
|------|---------|-------|
| `CMakeLists.txt` | Main build configuration | ✏️ Yes - to customize build |
| `cmake/SGDKToolchain.cmake` | Cross-compilation setup | ⚠️ Rarely - already configured |
| `cmake/SGDK.cmake` | Helper functions | ⚠️ Rarely - provides utilities |
| `CMakePresets.json` | Build presets | ✏️ Yes - to add configurations |

### Build Scripts
| File | Purpose | Platform | Edit? |
|------|---------|----------|-------|
| `build.sh` | Main build script | macOS/Linux | ⚠️ Rarely |
| `build.bat` | Main build script | Windows | ⚠️ Rarely |
| `Makefile` | Traditional make wrapper | All | ⚠️ Rarely |
| `test-setup.sh` | Setup verification | macOS/Linux | ❌ No |

### IDE Integration
| File | Purpose | Edit? |
|------|---------|-------|
| `.vscode/tasks.json` | VS Code build tasks | ✏️ Yes - to customize tasks |
| `.vscode/launch.json` | VS Code debugger | ✏️ Yes - to configure emulator |

### Documentation
| File | Purpose | For |
|------|---------|-----|
| `QUICKSTART.md` | Quick reference | Getting started fast |
| `CMAKE_BUILD.md` | Full documentation | Detailed usage |
| `CMAKE_SETUP_SUMMARY.md` | What was installed | Understanding setup |
| `CMAKE_FILE_INDEX.md` | This file | Finding files |
| `README.md` | Updated with build info | General project info |

### Configuration
| File | Purpose | Edit? |
|------|---------|-------|
| `.gitignore` | Updated for CMake | ⚠️ Rarely |

## 📂 Generated Files (Don't Edit)

These files are created during the build process:

```
build/                    # Build directory (created by CMake)
├── CMakeCache.txt       # CMake configuration cache
├── CMakeFiles/          # CMake internal files
├── out.bin              # Your ROM file! 🎮
├── out.elf              # Executable with symbols
├── out.map              # Memory map
├── out.sym              # Symbol table
└── res/
    ├── resources.s      # Compiled resources (ASM)
    └── resources.h      # Resource headers
```

## 🎯 Quick Reference

### What to Edit

**To customize your build:**
- `CMakeLists.txt` - Source files, compiler flags, ROM name

**To add IDE tasks:**
- `.vscode/tasks.json` - Build commands in VS Code
- `.vscode/launch.json` - Debugger configuration

**To change build configurations:**
- `CMakePresets.json` - Add new build types

**Your game code:**
- `src/*.c` - Your C source files
- `inc/*.h` - Your header files  
- `res/resources.res` - Resource definitions

### What NOT to Edit

**Don't modify these:**
- `build/` directory contents (regenerated)
- Generated `.s` and `.h` files in `build/res/`
- `cmake/SGDK.cmake` (unless extending functionality)
- `cmake/SGDKToolchain.cmake` (unless fixing bugs)

## 🔍 File Purposes Explained

### CMakeLists.txt
The heart of the build system. Defines:
- Project name and version
- Source files to compile
- Resource compilation rules
- Compiler and linker settings
- ROM generation process

### cmake/SGDKToolchain.cmake
Configures the cross-compiler:
- Sets m68k-elf-gcc as compiler
- Defines SGDK paths (works with Docker)
- Sets up compiler flags for Genesis

### cmake/SGDK.cmake
Helper functions:
- `sgdk_compile_resources()` - Compiles .res files
- `sgdk_compile_z80()` - Assembles Z80 code
- `sgdk_validate_environment()` - Checks setup

### build.sh / build.bat
Convenience scripts that:
- Check Docker availability
- Pull SGDK image if needed
- Run CMake configuration
- Execute the build
- Report results

### Makefile
Traditional interface:
- Wraps build.sh for familiar commands
- Provides `make`, `make clean`, `make debug`
- Optional - you can use build.sh directly

### CMakePresets.json
Defines build configurations:
- Default (Release build)
- Debug (with symbols)
- Release (optimized)

Used by IDEs and `cmake --preset` command

### .vscode/tasks.json
VS Code tasks for:
- Building release/debug
- Cleaning build
- Running in emulator
- Verbose builds

Accessible via: Cmd+Shift+B (Mac) or Ctrl+Shift+B (Windows)

### .vscode/launch.json
VS Code launcher:
- Runs ROM in emulator
- Triggered by F5 (if emulator configured)

## 📊 File Dependencies

```
User runs: ./build.sh
    ↓
Uses: Docker + SGDK image
    ↓
Reads: CMakeLists.txt
    ├── Includes: cmake/SGDKToolchain.cmake
    ├── Includes: cmake/SGDK.cmake
    ├── Compiles: src/*.c
    ├── Processes: res/resources.res (via rescomp)
    └── Links: SGDK library
    ↓
Outputs: build/out.bin
```

## 🎓 Learning Path

**Beginner:**
1. Read `QUICKSTART.md`
2. Run `./build.sh`
3. Test `build/out.bin` in emulator

**Intermediate:**
1. Read `CMAKE_BUILD.md`
2. Customize `CMakeLists.txt`
3. Add custom compiler flags
4. Create new build presets

**Advanced:**
1. Read `CMAKE_SETUP_SUMMARY.md`
2. Modify `cmake/SGDK.cmake`
3. Add custom build steps
4. Integrate additional tools

## 🛠️ Common Modifications

### Change ROM Name
Edit `CMakeLists.txt`:
```cmake
set(ROM_NAME "mygame" CACHE STRING "...")
```

### Add Source Files
Just create them! CMake auto-detects:
- New `.c` files in `src/`
- New `.h` files in `inc/`

### Add Resources
1. Add files to `res/`
2. Update `res/resources.res`
3. Rebuild

### Custom Compiler Flag
Edit `CMakeLists.txt`:
```cmake
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wno-unused-variable  # Example
)
```

### Add VS Code Task
Edit `.vscode/tasks.json`:
```json
{
    "label": "My Custom Build",
    "type": "shell",
    "command": "./build.sh",
    "args": ["--my-option"]
}
```

## 📞 Getting Help

**Build issues:**
- Run `./test-setup.sh` to verify setup
- Check `CMAKE_BUILD.md` troubleshooting section
- Use `./build.sh --verbose` for details

**CMake questions:**
- See `CMAKE_BUILD.md`
- CMake docs: https://cmake.org/documentation/

**SGDK questions:**
- SGDK Wiki: https://github.com/Stephane-D/SGDK/wiki
- SGDK Discord: https://discord.gg/xmnBWQS

## 📋 Checklist

Before building, ensure:
- ✅ Docker is installed and running
- ✅ `build.sh` is executable (`chmod +x build.sh`)
- ✅ All files from this index are present
- ✅ `res/resources.res` exists

After building, you should have:
- ✅ `build/out.bin` (your ROM)
- ✅ No compilation errors
- ✅ ROM size reported

---

**Quick navigation:**
- [QUICKSTART.md](QUICKSTART.md) - Get building in 30 seconds
- [CMAKE_BUILD.md](CMAKE_BUILD.md) - Comprehensive guide
- [CMAKE_SETUP_SUMMARY.md](CMAKE_SETUP_SUMMARY.md) - What's installed
- [README.md](README.md) - Project overview
