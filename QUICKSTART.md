# Quick Start Guide - CMake Build System

## 🚀 Building Your ROM in 30 Seconds

### Step 1: Ensure Docker is Running
```bash
docker --version
```

### Step 2: Build the ROM
```bash
# macOS/Linux
./build.sh

# Windows
build.bat
```

### Step 3: Find Your ROM
Your ROM is ready at: `build/out.bin`

---

## 📋 Common Commands

| Command | Description |
|---------|-------------|
| `./build.sh` | Build release ROM |
| `./build.sh --debug` | Build with debug symbols |
| `./build.sh --clean` | Clean build from scratch |
| `./build.sh --verbose` | Show detailed build output |
| `make` | Alternative: build using Makefile |
| `make clean` | Alternative: clean build directory |

---

## 📂 Build Output

After building, you'll find:

```
build/
├── out.bin          # 🎮 Your ROM file (run in emulator)
├── out.elf          # Executable with debug symbols
├── out.map          # Memory map
└── out.sym          # Symbol table
```

---

## 🎮 Testing Your ROM

Use any Sega Genesis/Mega Drive emulator:

**Popular Emulators:**
- **Gens KMod**: http://gens.me/
- **BlastEm**: https://www.retrodev.com/blastem/
- **Kega Fusion**: https://www.carpeludum.com/kega-fusion/

**Command line:**
```bash
# Set your emulator path
export SGDK_EMULATOR=/path/to/emulator

# Run
make run
```

---

## 🔧 Project Structure

```
MySegaGame/
├── CMakeLists.txt          # Main build config
├── build.sh / build.bat    # Build scripts
├── Makefile                # Traditional make wrapper
├── cmake/                  # CMake modules
│   ├── SGDKToolchain.cmake
│   └── SGDK.cmake
├── src/                    # Your C source files
├── inc/                    # Your header files
└── res/                    # Resources (images, music, etc.)
    └── resources.res       # Resource definitions
```

---

## ➕ Adding New Files

### New C Source File
1. Add `src/myfile.c`
2. Add `inc/myfile.h`
3. Run `./build.sh`

CMake automatically detects new files!

### New Resource
1. Add your resource file to `res/`
2. Update `res/resources.res`:
   ```
   IMAGE my_image "my_image.png" NONE
   ```
3. Run `./build.sh`

---

## 🐛 Troubleshooting

### "Docker not found"
- Install Docker: https://www.docker.com/
- Restart terminal after install

### "Build failed"
- Try clean build: `./build.sh --clean`
- Check Docker is running
- Pull latest SGDK: `docker pull ghcr.io/stephane-d/sgdk:latest`

### "Permission denied: ./build.sh"
```bash
chmod +x build.sh
```

---

## 📚 More Information

- Full documentation: [CMAKE_BUILD.md](CMAKE_BUILD.md)
- SGDK Documentation: https://github.com/Stephane-D/SGDK/wiki
- Original SGDK: https://github.com/Stephane-D/SGDK

---

## 💡 Tips

1. **Use `--verbose` to debug build issues:**
   ```bash
   ./build.sh --verbose
   ```

2. **Check ROM size:**
   ROM size is displayed after successful build

3. **Parallel builds:**
   Build script uses all CPU cores automatically

4. **IDE Integration:**
   - VS Code: Install CMake Tools extension
   - CLion: Just open the project folder

---

## ⚙️ Build Types

| Type | Optimization | Debug Symbols | Use Case |
|------|--------------|---------------|----------|
| Release | `-O3` | No | Final ROM for distribution |
| Debug | `-O0` | Yes | Development & debugging |

Switch with:
```bash
./build.sh --release  # or
./build.sh --debug
```

---

**Happy Genesis Development! 🎮**
