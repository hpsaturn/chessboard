# Windows Build Support

This project now supports building on Windows through GitHub Actions.

## Build Methods

### 1. vcpkg (Recommended)
- Uses Microsoft's vcpkg package manager
- Automatically handles dependencies
- Works with Visual Studio toolchain

### 2. MSYS2/MinGW
- Uses MSYS2 environment with MinGW compiler
- Provides Unix-like build environment on Windows
- Good for cross-platform compatibility

## GitHub Actions

The workflow includes:
- **build-linux**: Ubuntu 22.04 with apt dependencies
- **build-windows**: Windows with vcpkg dependencies  
- **build-windows-msys2**: Windows with MSYS2/MinGW

## Local Windows Development

### Using vcpkg:
```bash
# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install dependencies
.\vcpkg install sdl2 sdl2-image sdl2-ttf yaml-cpp

# Configure and build
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=path\to\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build build --config Release
```

### Using MSYS2:
```bash
# Install MSYS2 and dependencies
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake \
          mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image \
          mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-yaml-cpp

# Configure and build
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```
