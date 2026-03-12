# MeOS Modernization

Modernized version of [MeOS](https://github.com/melinsoftware/meos).

## Build Prerequisites

- **CMake** (3.25 or newer)
- **Ninja**
- **vcpkg** (dependency manager)
- **C++20 compiler** (GCC 13+, Clang 16+, MSVC 19.34+)
- **Node.js** (20.x or newer) and **npm** (10.x or newer)

## Build Instructions

### C++ Core
1. Ensure the `VCPKG_ROOT` environment variable is set to your vcpkg installation directory.
2. Configure the project:
   ```bash
   cmake --preset default
   ```
3. Build the project:
   ```bash
   cmake --build build
   ```

### React Frontend
1. Navigate to the frontend directory:
   ```bash
   cd src/ui/web
   ```
2. Install dependencies:
   ```bash
   npm ci
   ```
3. Build the frontend:
   ```bash
   npm run build
   ```

## Run Instructions

### C++ Core
After building, the executable can be found in the `build/` directory:
```bash
./build/meos
```

### React Frontend
To run the frontend in development mode with Hot Module Replacement (HMR):
```bash
cd src/ui/web
npm run dev
```

## Testing

### C++ Core
Run the tests using `ctest`:
```bash
ctest --test-dir build
```

To run with code coverage (GCC/Clang):
```bash
cmake build -DMEOS_ENABLE_COVERAGE=ON
cmake --build build
ctest --test-dir build
# Use gcovr/lcov to process .gcda files in build/
```

### React Frontend
Run the Vitest tests:
```bash
cd src/ui/web
npm test
```

## Quality Checks

### React Frontend
Run ESLint and Prettier:
```bash
cd src/ui/web
npm run lint
```

## Platform Notes

- **Linux:** Verified on Ubuntu with GCC 13 and Ninja.
- **Windows:** Supports MSVC with Ninja or Visual Studio generators via CMake presets.
