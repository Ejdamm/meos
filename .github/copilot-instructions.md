# MeOS – Copilot Instructions

## Architecture

MeOS is a **headless C++ REST server + React web frontend** for orienteering event management.

| Component | Tech | Location |
|-----------|------|----------|
| REST server (headless, cross-platform) | C++17 / restbed | `src/` → `MeOS-server` |
| Desktop GUI (Windows only) | C++17 / WinAPI/GDI | `src/` → `MeOS` |
| Web frontend | React 19 + TypeScript + Vite | `src/ui/web/` |

The C++ source is split into modules under `src/{app,domain,ui,db,net,io,hw,speaker,print,util}/`. **All these subdirs are added as include paths**, so headers are included by bare filename: `#include "oEvent.h"`, not `#include "domain/oEvent.h"`.

## Build Commands

### C++ (CMake + vcpkg)

```sh
# Requires VCPKG_ROOT set to a vcpkg installation
cmake --preset=default              # configure (Release)
cmake --preset=debug                # configure (Debug)
cmake --build --preset=default      # build all targets
cmake --build --preset=default --target MeOS-server   # server only (Linux/macOS)
cmake --build --preset=default --target MeOS-test     # portable unit tests
```

### C++ tests

```sh
cmake --build build --target MeOS-test && ./build/MeOS-test
```

`MeOS-test` is a portable test binary covering `IEventNotifier`, notifiers, `platform_socket`, and API routing. It has no Win32 or gdioutput dependencies and runs on any platform.

### Web frontend (`src/ui/web/`)

```sh
npm ci                    # install dependencies
npm run dev               # dev server with hot-reload
npm run build             # production build → dist/
npm test                  # unit tests (Vitest, single run)
npm run test:watch        # unit tests in watch mode
npm run test:coverage     # with coverage report
npm run test:e2e          # Playwright E2E tests
npm run lint              # ESLint
npm run typecheck         # tsc --noEmit
```

Run a single Vitest test file:
```sh
npx vitest run src/__tests__/MyComponent.test.tsx
```

Run a single Playwright spec:
```sh
npx playwright test e2e/smoke.spec.ts
```

## Key C++ Conventions

### Domain objects (`src/domain/`)
- All domain classes are prefixed with `o`: `oEvent`, `oRunner`, `oClub`, `oClass`, `oCourse`, `oControl`, `oCard`.
- `oEvent` is the central competition object; it owns all other domain objects.
- Wide strings (`std::wstring`) are used throughout the domain layer for text.
- `gdioutput::toUTF8()` / `gdioutput::fromUTF8()` are the canonical string conversion helpers between `std::wstring` and UTF-8 `std::string`.

### Headless vs GUI guarding
- Use `#ifdef MEOS_SERVER` to guard Win32-specific code that must not compile in the server target.
- The server target includes stub files (`gdioutput_server_stubs.cpp`, `image_server_stub.cpp`) that provide no-op implementations of Win32-dependent functions.

### IEventNotifier pattern
- `IEventNotifier` (in `src/domain/`) is the pure-virtual interface for domain→UI communication (alerts, status, user prompts).
- `NullNotifier` (in `src/app/`) implements it as a no-op for the headless server.
- `MockNotifier` (in `test/`) implements it for unit tests.
- Domain code must take `IEventNotifier&` (or `*`), never `gdioutput&` directly, to remain testable.

### REST API (`src/net/`)
- Handlers are inline functions in `*_handlers.h` files (e.g., `runner_handlers.h`), registered on `ApiRouter`.
- Each handler file has a `registerXxxHandlers(ApiRouter &router, oEvent *oe)` function.
- `ApiRequest` / `ApiResponse` are the request/response value types defined in `api_router.h`.
- JSON serialization uses ADL `to_json(nlohmann::json &j, const oXxx &x)` inline functions in `json_serializers.h`.
- `ApiRequest::negotiateContentType()` handles JSON vs XML content negotiation.
- All endpoints live under `/api/`.

### UI tabs (`src/ui/`)
- Windows desktop UI screens are `Tab*` classes (e.g., `TabRunner`, `TabClass`), all inheriting `TabBase`.
- `gdioutput` is the Win32 rendering/widget abstraction; it is **not** available in `MeOS-server`.

## Web Frontend Conventions (`src/ui/web/`)

- Data fetching: **TanStack Query** (`@tanstack/react-query`) for all API calls; avoid plain `useEffect` + fetch.
- HTTP client: **axios** via API modules in `src/api/`.
- Routing: **React Router v7**.
- Unit tests: **Vitest** + **React Testing Library** + **MSW** (Mock Service Worker).
  - Test helpers (`renderWithProviders`, `createTestQueryClient`, MSW `server`, `handlers`, `fixtures`) are exported from `src/test/index.ts`.
- E2E tests: **Playwright** in `e2e/`, using route interception to mock the REST API.

## CMake Options

| Option | Default | Effect |
|--------|---------|--------|
| `MEOS_USE_MYSQL` | `ON` | Enable MySQL; `OFF` uses in-memory stub (`src/db/no_mysql_stub.cpp`) |
| `MEOS_BUILD_WEB` | `ON` | Build React frontend via npm during CMake build |

## Upstream Sync

This repo (`Ejdamm/meos`) is a fork of [`melinsoftware/meos`](https://github.com/melinsoftware/meos). The fork was created from upstream commit `79d019d` (MeOS 5.0 Beta 1, 2026-02-20).

| Remote | URL |
|--------|-----|
| `origin` | `git@github.com:Ejdamm/meos.git` |
| `upstream` | `https://github.com/melinsoftware/meos.git` |

If `upstream` is missing: `git remote add upstream https://github.com/melinsoftware/meos.git`

**Branch model:** `master` holds our changes; feature branches are merged to `master` via PR. Use **merge, not rebase**, to keep clear history of upstream vs local changes.

**Sync upstream (full):**
```sh
git fetch upstream
git checkout -b sync/upstream-YYYY-MM-DD master
git merge upstream/master   # resolve conflicts, then open PR against master
```

**Cherry-pick (individual fix):**
```sh
git fetch upstream
git checkout -b fix/description master
git cherry-pick <commit-sha>
git push origin fix/description   # open PR against master
```

Upstream also has a `develop` branch — monitor it for upcoming changes.

## Docker

```sh
docker build -t meos .
docker run -p 2009:2009 meos   # serves web UI + REST API on port 2009

docker compose up -d           # full stack: MeOS + MySQL 8.0
docker compose down -v         # stop and remove volumes
```

## Debugging CI Build Failures

CI log artifacts are stored as ZIP files in `plans/` (e.g., `plans/logs_<run_id>.zip`).

### Log ZIP structure

```
logs_<run_id>/
├── 0_build-windows (x64, Release).txt   # full log per job (top-level)
├── 1_build-windows (x64, Debug).txt
├── 2_web-quality.txt
├── 3_build-linux (Debug).txt
├── ...
├── build-windows (x64, Release)/        # per-step logs (subdirectory)
│   ├── 3_Configure CMake.txt
│   ├── 4_Build.txt
│   └── ...
├── docker-build/
│   ├── 4_Build Docker image.txt
│   └── ...
└── web-quality/
    ├── 5_TypeScript type check.txt
    ├── 6_ESLint.txt
    └── ...
```

### Triage workflow

1. **Extract:** `unzip -o plans/logs_<run_id>.zip -d plans/logs_<run_id>`
2. **Identify failing jobs:** grep error patterns across top-level `.txt` files:
   ```sh
   grep -li "##\[error\]\|error C\|error TS\|FAILED\|fatal error" plans/logs_<run_id>/*.txt
   ```
3. **Classify errors** by grepping for platform-specific patterns:
   - **C++ (Windows):** `error C[0-9]` (MSVC), `fatal error LNK` (linker)
   - **C++ (Linux):** `error:` from gcc/g++
   - **vcpkg:** `vcpkg install failed`, `no version database entry`, `failed to.*baseline`
   - **TypeScript:** `error TS[0-9]`
   - **ESLint:** `error  ` (two spaces before rule name)
   - **Docker:** `ERROR:`, `failed to build`, `exit code:`
4. **Get unique errors:** deduplicate to see distinct issues:
   ```sh
   grep "error TS" <logfile> | sed 's/.*error /error /' | sort -u
   ```
5. **List affected files:**
   ```sh
   grep "error TS" <logfile> | sed 's/.*\(src\/[^(]*\).*/\1/' | sort -u
   ```
6. **Drill into step logs** in subdirectories for full context around a specific failure.

### Common failure categories

| Category | Symptom | Typical cause |
|----------|---------|---------------|
| vcpkg baseline | `failed to git show versions/baseline.json` | `builtin-baseline` commit in `vcpkg.json` doesn't exist in vcpkg repo |
| vcpkg version | `no version database entry for X at Y` | `version>=` constraint specifies a version not in vcpkg's database, or baseline can't resolve |
| TS type augmentation | `Property 'X' does not exist on type 'Assertion<Y>'` | Missing vitest matcher type declarations in `vitest.d.ts` |
| ESLint unused | `'X' is defined but never used` | Unused imports/variables/type params |
| Docker npm build | `npm run build` exit code 2 | TypeScript errors in `tsc -b` (check underlying TS errors) |
