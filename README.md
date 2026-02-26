# meos
MeOS - A Much Easier Orienteering System

Source code for the MeOS project (www.melin.nu/meos)

## Building

### Prerequisites

- CMake 3.20+
- Ninja build system
- [vcpkg](https://github.com/microsoft/vcpkg) package manager

### vcpkg Setup

**Option A – Git submodule (recommended for CI):**
```sh
git submodule add https://github.com/microsoft/vcpkg.git third_party/vcpkg
third_party/vcpkg/bootstrap-vcpkg.sh   # or bootstrap-vcpkg.bat on Windows
export VCPKG_ROOT="$PWD/third_party/vcpkg"
```

**Option B – System installation:**
```sh
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh
export VCPKG_ROOT="$HOME/vcpkg"
```

### Configure and Build

```sh
cmake --preset=default   # configures with vcpkg toolchain (Release)
cmake --build --preset=default
```

Use `--preset=debug` for a debug build.

vcpkg automatically installs the required dependencies declared in `vcpkg.json`:
- **libmysql** (MySQL Connector/C)
- **libharu** (PDF generation)
- **restbed** (Corvusoft REST framework)
- **libpng** (PNG image support)
- **zlib** (compression)
