# meos
MeOS - A Much Easier Orienteering System

Source code for the MeOS project (www.melin.nu/meos)

## C++ Backend

### Prerequisites

- CMake 3.28+
- Ninja build system
- C++17 compiler (GCC 13+ or Clang 16+)
- vcpkg (set `VCPKG_ROOT` environment variable)

### Build

```bash
export VCPKG_ROOT=~/vcpkg  # adjust to your vcpkg installation
cmake --preset default     # Configure (Debug)
cmake --build --preset default
```

### Test

```bash
ctest --test-dir build --output-on-failure
```

### Coverage

```bash
cmake --preset default -DMEOS_ENABLE_COVERAGE=ON
cmake --build --preset default
ctest --test-dir build
# .gcno/.gcda files in build/ for gcov/lcov
```

## Frontend (Web UI)

### Prerequisites

- Node.js 18+ and npm

### Build & Run

```bash
cd src/ui/web
npm ci
npm run build      # Production build → src/ui/web/dist/
npm run dev        # Development server with HMR
```

### Quality Checks

```bash
npm run lint       # ESLint
npm run typecheck  # TypeScript type checking
npm test           # Vitest unit tests
npm run test:coverage  # Tests with v8 coverage
```
