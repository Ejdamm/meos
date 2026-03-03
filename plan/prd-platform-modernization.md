# PRD: MeOS Platform Modernization

## Introduction

MeOS (Much Easier Orienteering System) is a mature Windows desktop application for managing orienteering competitions, built with C++17, Win32/GDI, and MSBuild. This PRD describes a comprehensive modernization effort to make MeOS platform-independent, replace the Win32 GUI with a React+TypeScript web interface, adopt CMake, introduce proper testing and CI/CD, and restructure the codebase into a modular layout.

The end result is a single executable that starts an embedded HTTP server and serves a React web GUI — users open `http://localhost:<port>` in any browser. The application runs on Linux, macOS, and Windows without modification.

### Current State

- **Build:** MSBuild / Visual Studio 2022 (Windows-only)
- **GUI:** Win32/GDI via custom `gdioutput` wrapper, tab-based (`TabRunner`, `TabClass`, etc.)
- **Source layout:** Flat `code/` directory (~200+ files, no subdirectories for source)
- **Database:** MySQL via custom ORM (`MeosSQL` → `mysqlwrapper`)
- **REST API:** Minimal query-parameter based API returning XML/HTML via restbed
- **Hardware:** SportIdent reader integration via Windows serial port APIs
- **Dependencies:** Vendored in `code/` (restbed, libharu, minizip, mysql, png, sound, DLLs)
- **Tests:** Stub test framework (`testmeos.cpp`), no CI/CD

### Target State

- **Build:** CMake (cross-platform)
- **GUI:** React + TypeScript SPA served by embedded HTTP server
- **Source layout:** Modular `src/` with domain, net, db, util, etc.
- **Database:** SQLite (embedded, no separate server)
- **REST API:** Full CRUD JSON API covering all domain entities
- **Hardware:** Deferred to later phase
- **Dependencies:** vcpkg package manager
- **Tests:** Google Test (C++), Vitest (React), CI/CD via GitHub Actions

## Goals

- Make MeOS fully platform-independent (Linux, macOS, Windows)
- Replace Win32 GUI with a modern React + TypeScript web interface
- Migrate from MSBuild to CMake for cross-platform builds
- Restructure source code from flat `code/` into modular `src/` layout
- Replace MySQL with SQLite for zero-configuration deployment
- Build complete CRUD REST API with JSON for all domain entities
- Replace vendored libraries with vcpkg-managed dependencies
- Add comprehensive test coverage and CI/CD from the start
- Migrate existing domain logic incrementally (module by module)
- Maintain single-binary deployment model (download and run)

## User Stories

### US-001: CMake Build System

**Description:** As a developer, I want to build MeOS with CMake so that I can compile on any platform.

**Acceptance Criteria:**
- [ ] Top-level `CMakeLists.txt` exists and builds a minimal executable
- [ ] vcpkg is integrated as the dependency manager (via toolchain file)
- [ ] `vcpkg.json` manifest declares all external dependencies
- [ ] Builds successfully on Linux (GCC/Clang) and Windows (MSVC)
- [ ] Precompiled headers are configured for build performance
- [ ] Debug and Release configurations work

### US-002: Modular Source Layout

**Description:** As a developer, I want source code organized in logical modules so that the codebase is navigable and each layer has clear boundaries.

**Acceptance Criteria:**
- [ ] `src/` directory created with subdirectories: `app/`, `domain/`, `net/`, `db/`, `util/`, `io/`
- [ ] CMake targets defined per module (static libraries + main executable)
- [ ] Module dependencies are explicit in CMake (no circular dependencies)
- [ ] Header include paths configured so bare `#include "oBase.h"` still works within modules
- [ ] Builds and links successfully with the new layout

### US-003: Domain Layer Extraction

**Description:** As a developer, I want the domain model classes (oEvent, oRunner, oClass, etc.) extracted into a standalone library with no GUI or database dependencies so that they can be tested and used independently.

**Acceptance Criteria:**
- [ ] `src/domain/` contains migrated domain classes: `oBase`, `oEvent`, `oRunner`, `oTeam`, `oClass`, `oClub`, `oCourse`, `oControl`, `oCard`, `oFreePunch`, `oPunch`
- [ ] Domain layer compiles as a static library with zero dependencies on GUI code
- [ ] Domain layer has no direct dependency on database implementation (uses interfaces/abstractions)
- [ ] All `#include` references to Win32/GDI headers are removed from domain code
- [ ] Existing domain logic (results, drawing, qualification) is preserved
- [ ] `oDataContainer`/`oDataInterface` pattern is retained
- [ ] Unit tests exist for core domain operations

### US-004: SQLite Database Layer

**Description:** As a developer, I want a SQLite-based persistence layer so that MeOS runs without a separate database server.

**Acceptance Criteria:**
- [ ] `src/db/` contains a database abstraction interface
- [ ] SQLite implementation of the interface exists
- [ ] Schema covers all domain entities (runners, teams, classes, clubs, courses, controls, cards, punches)
- [ ] CRUD operations work for all entities
- [ ] Data migration path from old MySQL format is documented (as a future task)
- [ ] SQLite database is a single file in the working directory
- [ ] Concurrent access is handled appropriately

### US-005: JSON REST API — Core Entities

**Description:** As a frontend developer, I want complete CRUD endpoints for all core domain entities so that the React GUI can manage competitions.

**Acceptance Criteria:**
- [ ] All endpoints use JSON request/response format
- [ ] REST conventions followed: `GET /api/v1/{entity}`, `GET /api/v1/{entity}/{id}`, `POST`, `PUT`, `DELETE`
- [ ] Endpoints exist for: competitions, runners, teams, classes, clubs, courses, controls
- [ ] List endpoints support filtering and pagination
- [ ] Error responses use consistent JSON format with error codes
- [ ] Input validation with meaningful error messages
- [ ] API is documented (OpenAPI/Swagger spec or equivalent)

### US-006: JSON REST API — Competition Operations

**Description:** As a frontend developer, I want API endpoints for competition-specific operations (start lists, results, card readout) so that the GUI can support the full competition workflow.

**Acceptance Criteria:**
- [ ] `GET /api/v1/results` returns computed results (per class, overall)
- [ ] `GET /api/v1/startlist` returns start lists
- [ ] `POST /api/v1/cards` handles card readout data submission
- [ ] `POST /api/v1/runners/{id}/status` allows manual status changes (DNS, DNF, DSQ)
- [ ] Result computation uses existing `GeneralResult` logic
- [ ] Endpoints support both preliminary and final results

### US-007: React + TypeScript Web GUI Shell

**Description:** As a user, I want a modern web-based interface so that I can manage competitions from any browser on any platform.

**Acceptance Criteria:**
- [ ] React 18+ app with TypeScript in `src/ui/web/`
- [ ] Vite as build tool
- [ ] Routing with React Router (tabs map to routes: `/runners`, `/classes`, `/clubs`, etc.)
- [ ] API client layer with typed interfaces matching REST API
- [ ] Basic layout: navigation sidebar/tabs + content area
- [ ] Responsive design (works on tablets for field use)
- [ ] Production build outputs static files that the C++ server serves

### US-008: Web GUI — Competition Management

**Description:** As a competition organizer, I want to create and configure competitions, manage classes, courses, and clubs through the web interface.

**Acceptance Criteria:**
- [ ] Create/open/save competition
- [ ] CRUD interface for classes (name, course assignment, start method)
- [ ] CRUD interface for courses (name, length, controls)
- [ ] CRUD interface for clubs
- [ ] CRUD interface for controls
- [ ] Form validation with user-friendly error messages

### US-009: Web GUI — Runner & Team Management

**Description:** As a competition organizer, I want to manage runners and teams through the web interface.

**Acceptance Criteria:**
- [ ] CRUD interface for runners (name, club, class, start time, card number)
- [ ] CRUD interface for teams (name, club, class, members)
- [ ] Import runners from CSV/IOF XML
- [ ] Search/filter runners by name, club, class
- [ ] Bulk operations (assign class, assign start times)

### US-010: Web GUI — Results & Live View

**Description:** As a competition organizer, I want to view results and start lists in the web interface.

**Acceptance Criteria:**
- [ ] Results view per class with split times
- [ ] Start list view per class
- [ ] Auto-refresh for live results (polling or WebSocket)
- [ ] Print-friendly result formatting
- [ ] Export results (CSV, IOF XML)

### US-011: Static File Serving from C++ Server

**Description:** As a user, I want the C++ executable to serve the React frontend so that no separate web server is needed.

**Acceptance Criteria:**
- [ ] C++ HTTP server serves static files from an embedded or bundled directory
- [ ] `index.html` served at `/` with SPA fallback (all non-API routes serve `index.html`)
- [ ] Correct MIME types for `.js`, `.css`, `.html`, `.svg`, `.png`, `.woff2`
- [ ] Gzip/compression for static assets
- [ ] React production build is integrated into CMake build process

### US-012: Remove Win32 GUI Code

**Description:** As a developer, I want Win32-specific GUI code removed so that the codebase is platform-independent.

**Acceptance Criteria:**
- [ ] All `Tab*.cpp/h` files removed or migrated to web equivalents
- [ ] `gdioutput.cpp/h` and related GDI files removed
- [ ] Win32-specific APIs (`CreateWindow`, `SendMessage`, etc.) eliminated from core code
- [ ] No Windows-only headers (`windows.h`, `commctrl.h`) in domain or server code
- [ ] Application compiles and runs on Linux without Win32 dependencies

### US-013: Utility Migration

**Description:** As a developer, I want shared utilities (string conversion, time handling, XML/CSV parsing, etc.) migrated to `src/util/` so they are available to all modules.

**Acceptance Criteria:**
- [ ] `meos_util.h/cpp` migrated to `src/util/`
- [ ] `xmlparser.h/cpp`, `csvparser.h/cpp` migrated
- [ ] `TimeStamp` and time constants migrated
- [ ] `meosexception.h` migrated
- [ ] `localizer.h/cpp` migrated (i18n support)
- [ ] Wide string / narrow string conversion utilities preserved
- [ ] All migrated utilities compile on Linux and Windows

### US-014: I/O and Import/Export

**Description:** As a developer, I want import/export functionality (IOF XML, CSV, HTML) migrated so that data exchange works in the new system.

**Acceptance Criteria:**
- [ ] IOF 3.0 XML import/export (`iof30interface`) migrated to `src/io/`
- [ ] CSV import/export migrated
- [ ] HTML result generation migrated
- [ ] PDF generation (libharu) migrated
- [ ] All I/O code is platform-independent

### US-015: Test Infrastructure

**Description:** As a developer, I want a proper test framework so that I can write and run tests for all modules.

**Acceptance Criteria:**
- [ ] Google Test integrated via vcpkg
- [ ] Test targets in CMake for each module (`ctest` runs all tests)
- [ ] At least one test per domain entity (smoke tests for CRUD operations)
- [ ] Vitest configured for React frontend with React Testing Library
- [ ] Test coverage reporting configured

### US-016: CI/CD Pipeline

**Description:** As a developer, I want automated builds, tests, and quality checks on every push and PR so that regressions are caught early.

**Acceptance Criteria:**
- [ ] GitHub Actions workflow builds on Linux and Windows
- [ ] All C++ tests run in CI
- [ ] All frontend tests run in CI
- [ ] Linting: clang-tidy for C++, ESLint + Prettier for TypeScript
- [ ] Build artifacts uploaded (executables for Linux and Windows)
- [ ] CI runs on push to main and on pull requests

## Functional Requirements

- FR-1: The application must build with CMake on Linux (GCC 12+, Clang 15+) and Windows (MSVC v143)
- FR-2: All external dependencies must be managed through vcpkg
- FR-3: The domain layer must compile as an independent static library with no platform-specific code
- FR-4: Persistence must use SQLite with the database stored as a single file
- FR-5: The REST API must expose full CRUD operations for all domain entities via JSON
- FR-6: The REST API must follow RESTful conventions with versioned paths (`/api/v1/...`)
- FR-7: The C++ executable must serve the React SPA as static files
- FR-8: The React frontend must communicate exclusively through the JSON REST API
- FR-9: The application must start with a single command/double-click (no external dependencies at runtime)
- FR-10: The build system must produce a single distributable binary (with bundled frontend assets)
- FR-11: Source code must be organized in `src/` with clear module boundaries
- FR-12: All new code must have corresponding unit tests
- FR-13: CI must run on every push and pull request
- FR-14: The domain model must preserve existing business logic (result computation, class drawing, qualification systems)
- FR-15: Localization support must be retained (Swedish primary, multi-language via `.lng` files)

## Non-Goals

- **SportIdent hardware integration** — deferred to a future phase; no serial port or USB code in this scope
- **MySQL support** — SQLite only; migration tool from MySQL is out of scope (documented as future work)
- **Mobile-native app** — responsive web only, no iOS/Android native
- **Multi-user authentication** — single-user/local access model initially
- **Cloud deployment** — designed for local execution; cloud hosting is a user choice, not a project goal
- **Speaker/announcer functionality** — deferred; complex real-time features come after core is stable
- **Online results push** — existing `onlineresults`/`liveresult` integration deferred
- **Map rendering** — `maprenderer` functionality deferred
- **Backward compatibility with existing MeOS data files** — documented as future migration path

## Design Considerations

- The React GUI should mirror the existing tab structure for familiarity: Competition, Classes, Courses, Controls, Clubs, Runners, Teams, Results, Lists
- Use a component library (e.g., Radix UI, shadcn/ui) for consistent, accessible UI components
- Table views should support sorting, filtering, and inline editing for efficiency
- The UI should work well on tablets (orienteering events often use tablets in the field)
- Dark mode support is a nice-to-have but not required initially

## Technical Considerations

### Migration Strategy

The migration follows an incremental approach. Both `code/` (legacy) and `src/` (new) coexist during the transition:

1. **Phase 1 — Foundation:** CMake + vcpkg + modular `src/` structure + CI/CD + test framework (empty shell that compiles)
2. **Phase 2 — Utilities & Domain:** Migrate `src/util/` and `src/domain/` (no GUI, no DB), add tests
3. **Phase 3 — Database:** Implement SQLite layer in `src/db/`, wire to domain
4. **Phase 4 — REST API:** Build JSON CRUD API in `src/net/`, wire to domain + DB
5. **Phase 5 — React Frontend:** Build web GUI in `src/ui/web/`, connect to API
6. **Phase 6 — Static Serving & Integration:** C++ server serves React build, single-binary packaging
7. **Phase 7 — Cleanup:** Remove Win32 code, remove `code/` directory

Each phase produces a working, testable increment. No phase depends on Win32 GUI code — avoiding wasted effort.

### Key Dependencies (vcpkg)

- **restbed** or **cpp-httplib** — HTTP server (evaluate if restbed is available in vcpkg, otherwise switch)
- **nlohmann-json** — JSON serialization
- **sqlite3** — Database
- **libharu** — PDF generation
- **gtest** — Testing
- **pugixml** or **tinyxml2** — XML parsing (replace custom parser if beneficial)
- **zlib** — Compression (for minizip replacement)

### Architecture Diagram

```
┌─────────────────────────────────────────────┐
│               React + TypeScript            │
│            (src/ui/web/)                    │
│  ┌────────┬────────┬────────┬────────┐     │
│  │Runners │Classes │Results │ ...    │     │
│  └────────┴────────┴────────┴────────┘     │
└──────────────────┬──────────────────────────┘
                   │ JSON REST API
┌──────────────────┴──────────────────────────┐
│            C++ HTTP Server (src/net/)        │
│  ┌─────────────┐  ┌──────────────────┐      │
│  │ REST Routes  │  │ Static File Srv  │      │
│  └──────┬──────┘  └──────────────────┘      │
│         │                                    │
│  ┌──────┴──────────────────────────┐        │
│  │     Domain Layer (src/domain/)   │        │
│  │  oEvent, oRunner, oClass, ...   │        │
│  └──────┬──────────────────────────┘        │
│         │                                    │
│  ┌──────┴──────────────────────────┐        │
│  │    Database Layer (src/db/)      │        │
│  │         SQLite                   │        │
│  └─────────────────────────────────┘        │
│                                              │
│  ┌─────────────────────────────────┐        │
│  │    Utilities (src/util/)         │        │
│  │  strings, time, xml, csv, i18n  │        │
│  └─────────────────────────────────┘        │
└──────────────────────────────────────────────┘
```

### Source Layout

```
src/
├── app/           # Main entry point, application lifecycle
├── domain/        # Domain model (oEvent, oRunner, oClass, etc.)
├── db/            # Database abstraction + SQLite implementation
├── net/           # HTTP server, REST API routes
├── io/            # Import/export (IOF XML, CSV, HTML, PDF)
├── util/          # Shared utilities (strings, time, parsing, i18n)
└── ui/
    └── web/       # React + TypeScript frontend
        ├── src/
        ├── package.json
        ├── tsconfig.json
        └── vite.config.ts
```

## Success Metrics

- MeOS builds and runs on Linux and Windows from a single CMake command
- All domain entity CRUD operations work end-to-end (GUI → API → DB → response)
- Test suite passes with >80% coverage on new code
- CI pipeline runs green on every PR
- Single binary + bundled frontend deploys without any external dependencies
- Competition workflow (create competition → add classes/courses → add runners → record results → view results) works entirely through the web GUI

## Open Questions

1. Should the REST API support WebSocket for real-time updates (live results), or is polling sufficient for the initial version?
2. Which HTTP server library should replace/augment restbed? cpp-httplib is header-only and simpler; restbed is more feature-rich but heavier.
3. Should the React frontend support offline mode (service worker / PWA)?
4. How should the bundled frontend assets be embedded — as files alongside the binary, or compiled into the binary as resources?
5. Should localization in the frontend reuse the existing `.lng` files or adopt a standard i18n library (e.g., react-intl)?
6. What is the minimum supported browser version for the web GUI?
