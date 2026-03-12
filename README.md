# MeOS - A Much Easier Orienteering System

MeOS is an orienteering system designed to be easy to use.

## Build Prerequisites

- **CMake** (3.24 or later)
- **Ninja**
- **C++ Compiler** (supporting C++20)
- **vcpkg** (with `VCPKG_ROOT` environment variable set)
- **Node.js** (v20 or later) and **npm**

## Build Instructions

### C++ Backend

This project uses CMake presets for configuration and building.

1.  **Configure the project:**
    ```bash
    cmake --preset debug
    # or
    cmake --preset release
    ```

2.  **Build the project:**
    ```bash
    cmake --build --preset debug
    # or
    cmake --build --preset release
    ```

### React Frontend

The frontend is located in `src/ui/web/`.

1.  **Install dependencies:**
    ```bash
    cd src/ui/web
    npm install
    ```

2.  **Build the frontend:**
    ```bash
    npm run build
    ```

3.  **Run tests and linting:**
    ```bash
    npm test
    npm run lint
    ```

## Run Instructions

### C++ Backend

After building, you can run the executable:

```bash
./build/debug/meos
# or
./build/release/meos
```

### React Frontend

To run the frontend in development mode:

```bash
cd src/ui/web
npm run dev
```

To preview the production build:

```bash
cd src/ui/web
npm run preview
```

## Platform Notes

- **Linux**: Build tested on Ubuntu with GCC 13.
- **Windows**: Build using MSVC or GCC (MinGW).
- **macOS**: Build using Clang.
