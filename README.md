# MeOS

**MeOS – A Much Easier Orienteering System**

Source code for the MeOS project ([www.melin.nu/meos](http://www.melin.nu/meos))

## Architecture

MeOS uses a **headless C++ server + React frontend** architecture:

| Component | Technology | Location |
|-----------|-----------|----------|
| Server (headless) | C++ / restbed REST API | `src/` (target: `MeOS-server`) |
| Desktop GUI | C++ / WinAPI (Windows only) | `src/` (target: `MeOS`) |
| Web frontend | React + TypeScript + Vite | `src/ui/web/` |
| Database | MySQL 8.0 (optional) | External |

### Build targets

- **`MeOS-server`** – Cross-platform headless REST API server that serves the React web UI. No desktop dependencies required.
- **`MeOS`** – Windows desktop GUI application (WinAPI/GDI).
- **`MeOS-test`** – Unit test runner.

### Headless vs GUI mode

| | MeOS (GUI) | MeOS-server (Headless) |
|---|-----------|----------------------|
| Platform | Windows | Linux / macOS / Windows |
| User interface | Native WinAPI desktop | React web UI (browser) |
| REST API | ✓ | ✓ |
| Web UI served | ✗ | ✓ (via `--web-root`) |

### REST API overview

All endpoints are under `/api/` and return JSON (content-negotiation supported):

| Endpoint | Description |
|----------|-------------|
| `/api/competitions[/:id]` | Competition CRUD |
| `/api/runners[/:id]` | Runner management |
| `/api/teams[/:id]` | Team management |
| `/api/classes[/:id]` | Class configuration |
| `/api/courses[/:id]` | Course design |
| `/api/controls[/:id]` | Control points |
| `/api/clubs[/:id]` | Club management |
| `/api/cards` | SI card reads / punches |
| `/api/results` | Result queries |
| `/api/lists/:type` | Report / list generation |
| `/api/speaker/*` | Speaker monitoring |
| `/api/automations[/:id]` | Automation workflows |
| `/api/import/*`, `/api/export/*` | IOF XML & CSV import/export |

## Building

### Prerequisites

- CMake 3.20+
- Ninja build system
- [vcpkg](https://github.com/microsoft/vcpkg) package manager
- Node.js 20+ (for the web frontend)

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

### CMake options

| Option | Default | Description |
|--------|---------|-------------|
| `MEOS_USE_MYSQL` | `ON` | Enable MySQL database support. When `OFF`, uses an in-memory stub. |
| `MEOS_BUILD_WEB` | `ON` | Build the React web UI (requires Node.js). |

### Building on Linux (server only)

```sh
# Install system dependencies
sudo apt-get install build-essential cmake ninja-build pkg-config libssl-dev

# Setup vcpkg (see above), then:
cmake --preset=default
cmake --build --preset=default --target MeOS-server
```

### Building on macOS (server only)

```sh
brew install cmake ninja pkg-config openssl
# Setup vcpkg (see above), then:
cmake --preset=default
cmake --build --preset=default --target MeOS-server
```

### Building on Windows (GUI or server)

```powershell
# Install prerequisites via winget or Visual Studio
# Setup vcpkg (see above), then:
cmake --preset=default
cmake --build --preset=default          # builds MeOS desktop GUI
cmake --build --preset=default --target MeOS-server  # headless server
```

### Web frontend (standalone)

```sh
cd src/ui/web
npm ci
npm run dev     # development server with hot-reload
npm run build   # production build → dist/
npm test        # unit tests (Vitest)
npm run test:e2e  # E2E tests (Playwright)
```

## Docker

### Single container

```sh
docker build -t meos .
docker run -p 2009:2009 meos
```

The Docker image is a multi-stage build: React frontend → C++ server → minimal runtime. The server listens on port **2009** and serves the web UI.

### Full stack with Docker Compose

```sh
docker compose up -d
```

This starts:
- **meos** – the application server (port 2009)
- **mysql** – MySQL 8.0 database with health checks

Data is persisted in named volumes (`meos-data`, `mysql-data`).

```sh
docker compose down       # stop services
docker compose down -v    # stop and remove volumes
```

## License

See [LICENSE](LICENSE).
