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

### US-P0: Legacy Code Preparation

> **Extracted to separate PRD:** See [`plan/prd-legacy-preparation.md`](prd-legacy-preparation.md) for the full US-P0 breakdown (US-P0a through US-P0f). This work operates on the legacy `code/` directory under MSBuild and can run **in parallel** with the migration work below.

### US-001, US-015, US-016: Build & Infrastructure

> **Extracted to separate PRD:** See [`plan/prd-build-infrastructure.md`](prd-build-infrastructure.md) for the full breakdown (US-001 CMake, US-015 Test Infrastructure, US-016 CI/CD). This work operates on an empty project skeleton and can run **in parallel** with all other workstreams.

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

> **Note:** This story is split into incremental sub-stories ordered by dependency due to extreme coupling and the sheer size of oRunner (~230KB).

#### US-003a: Foundation — oBase, oDataContainer, domain_header

**Description:** Migrate the foundational base classes that all domain entities depend on.

**Acceptance Criteria:**
- [ ] `oBase.h/cpp` migrated to `src/domain/`
- [ ] `oDataContainer.h/cpp` migrated with `oDataInterface`/`oDataConstInterface`
- [ ] `domain_header.h` created with common enums and forward declarations
- [ ] `datadefiners.h` migrated
- [ ] Stubs created for `oEvent.h` and other dependents
- [ ] Domain library compiles with dependency on `util` only
- [ ] Unit tests for oBase basics

#### US-003b: Simple Entities — oControl, oPunch

**Description:** Migrate the simplest domain entities with fewest external dependencies.

**Acceptance Criteria:**
- [ ] `oControl.h/cpp` and `oPunch.h/cpp` migrated to `src/domain/`
- [ ] `SpecialPunch` enum moved to `domain_header.h` (avoid circular deps)
- [ ] Win32 string functions replaced with `meos_util` equivalents
- [ ] Stubs for `oCourse`, `oClass`, `oRunner`, `oCard`, `oFreePunch`
- [ ] Unit tests for oControl and oPunch

#### US-003c: oClub

**Description:** Migrate club entity — relatively simple with few dependencies.

**Acceptance Criteria:**
- [ ] `oClub.h/cpp` migrated to `src/domain/`
- [ ] Common enums centralized in `src/util/common_enums.h`
- [ ] Unit tests for oClub

#### US-003d: oCourse

**Description:** Migrate course entity. Courses define the control sequence and are referenced by classes.

**Acceptance Criteria:**
- [ ] `oCourse.h/cpp` migrated to `src/domain/` (full implementation, not stub)
- [ ] Course-control relationship works
- [ ] Length, climb, and course properties preserved
- [ ] Unit tests for oCourse operations

#### US-003e: oClass + Class Configuration

**Description:** Migrate class entity and related configuration. Classes are tightly coupled to courses and have complex start-method and qualification logic.

**Acceptance Criteria:**
- [ ] `oClass.h/cpp` migrated to `src/domain/` (full implementation, not stub)
- [ ] `classconfiginfo.h/cpp` migrated
- [ ] Class-course assignment works
- [ ] Start methods (individual, mass, pursuit, etc.) preserved
- [ ] `qualifications.h/cpp` migrated
- [ ] Unit tests for oClass operations

#### US-003f: oCard + oFreePunch

**Description:** Migrate card readout and free punch entities. Cards record the physical punch data from SI units.

**Acceptance Criteria:**
- [ ] `oCard.h/cpp` migrated to `src/domain/` (full implementation, not stub)
- [ ] `oFreePunch.h/cpp` migrated (full implementation, not stub)
- [ ] `SICard.h` (data structure) available in `src/util/` or `src/domain/`
- [ ] Card-punch matching logic preserved
- [ ] Unit tests for card operations

#### US-003g: oRunner

**Description:** Migrate the runner entity. This is the largest single file (~230KB) and most complex entity. Heavy stubbing will be needed for tight coupling to oEvent, oClass, oCard, and result computation.

> **Risk:** oRunner is extremely large and coupled. Consider migrating a simplified version first, then backfilling logic incrementally.

**Acceptance Criteria:**
- [ ] `oRunner.h/cpp` migrated to `src/domain/` (full implementation replacing stub)
- [ ] Runner-class, runner-club, runner-card relationships work
- [ ] Status computation (OK, DNS, DNF, DSQ, MP) works
- [ ] Split time computation works
- [ ] `oAbstractRunner` shared logic with oTeam preserved
- [ ] Unit tests for runner status, splits, and result computation

#### US-003h: oTeam

**Description:** Migrate team entity. Teams extend oAbstractRunner and contain runner members.

**Acceptance Criteria:**
- [ ] `oTeam.h/cpp` migrated to `src/domain/` (full implementation)
- [ ] Team-runner membership works
- [ ] Relay leg computation works
- [ ] Unit tests for team operations

#### US-003i: oEvent — Aggregate Root

**Description:** Migrate the full oEvent implementation. oEvent owns all domain collections and is the aggregate root. This is the final integration piece.

> **Risk:** oEvent is currently a minimal skeleton stub. The full implementation (oEvent.cpp, oEventDraw.cpp, oEventResult.cpp, etc.) touches everything. Migrate incrementally: core ownership first, then draw, then results.

**Acceptance Criteria:**
- [ ] `oEvent.cpp` full implementation replaces skeleton stub
- [ ] `oEventDraw.cpp` — class drawing and start list generation
- [ ] `oEventResult.cpp` — result computation orchestration
- [ ] Event owns and manages all entity collections
- [ ] Competition create/open/save lifecycle works
- [ ] Integration tests covering entity relationships through oEvent

#### US-003j: GeneralResult + Result Engine

**Description:** Migrate the pluggable result computation engine.

**Acceptance Criteria:**
- [ ] `generalresult.h/cpp` migrated (full implementation)
- [ ] Strategy pattern for scoring algorithms preserved
- [ ] `metalist.h/cpp` for output formatting migrated
- [ ] `oListInfo` migrated
- [ ] Result computation produces correct results for individual and relay formats
- [ ] Unit tests for result computation with known test data

#### US-003k: Domain Cleanup — Remove Stubs + Verify

**Description:** Final cleanup pass: remove all temporary stubs, verify the domain layer compiles cleanly, and ensure no Win32 dependencies remain.

**Acceptance Criteria:**
- [ ] All stubs (gdioutput stub, oSpeaker stub, Table stub, MeOSFeatures stub) removed or replaced with proper interfaces
- [ ] No `#ifdef _WIN32` guards in domain code (except `win_types.h` shims)
- [ ] Domain library compiles on Linux and Windows
- [ ] No direct Win32/GDI includes in domain code
- [ ] Full test suite passes
- [ ] `oDataContainer`/`oDataInterface` pattern verified working

### US-004: SQLite Database Layer

**Description:** As a developer, I want a SQLite-based persistence layer so that MeOS runs without a separate database server.

> **Note:** Split into incremental sub-stories.

#### US-004a: Database Abstraction + Connection + Migrations

**Description:** Set up SQLite connection management and migration system.

**Acceptance Criteria:**
- [ ] `src/db/` contains `SQLiteDatabase` with connection management
- [ ] Migration system using `_migrations` table
- [ ] Initial schema (V1) for runners and clubs
- [ ] Unit tests for database operations and migrations

#### US-004b: Schema for Simple Entities

**Description:** Add schema and CRUD for clubs, controls, and courses.

**Acceptance Criteria:**
- [ ] Schema migration for clubs, controls, courses
- [ ] CRUD operations (insert, get, update, delete) for each entity
- [ ] Repository/DAO pattern for data access
- [ ] Unit tests for each entity's CRUD operations

#### US-004c: Schema for Complex Entities

**Description:** Add schema and CRUD for runners, classes, cards, and punches.

**Acceptance Criteria:**
- [ ] Schema migration for runners, classes, cards, punches, free punches
- [ ] Foreign key relationships enforced
- [ ] CRUD operations with relationship loading (e.g., runner with club/class)
- [ ] Unit tests for complex entity operations

#### US-004d: Schema for Events + Teams

**Description:** Add event and team persistence, completing the full schema.

**Acceptance Criteria:**
- [ ] Schema migration for events and teams
- [ ] Team-runner membership persistence
- [ ] Event metadata (name, date, settings) persistence
- [ ] Concurrent access handled appropriately
- [ ] Full integration test: create event → add entities → query back
- [ ] SQLite database is a single file in the working directory

### US-005: JSON REST API — Core Entities

**Description:** As a frontend developer, I want complete CRUD endpoints for all core domain entities so that the React GUI can manage competitions.

> **Note:** Split by entity group to keep each story testable and deployable independently.

#### US-005a: API Framework + Error Handling

**Description:** Set up the HTTP server, routing infrastructure, JSON serialization, and consistent error handling.

**Acceptance Criteria:**
- [ ] HTTP server (cpp-httplib or restbed) integrated and running
- [ ] Route registration pattern established
- [ ] JSON request parsing and response serialization (nlohmann-json)
- [ ] Consistent error response format with error codes
- [ ] Input validation framework with meaningful error messages
- [ ] API versioning (`/api/v1/...`)
- [ ] Unit tests for routing and error handling

#### US-005b: Club + Control Endpoints

**Description:** CRUD endpoints for the simplest entities — clubs and controls.

**Acceptance Criteria:**
- [ ] `GET/POST/PUT/DELETE /api/v1/clubs` and `/api/v1/clubs/{id}`
- [ ] `GET/POST/PUT/DELETE /api/v1/controls` and `/api/v1/controls/{id}`
- [ ] List endpoints support basic filtering
- [ ] Integration tests for each endpoint

#### US-005c: Course + Class Endpoints

**Description:** CRUD endpoints for courses and classes, including their relationships.

**Acceptance Criteria:**
- [ ] `GET/POST/PUT/DELETE /api/v1/courses` and `/api/v1/courses/{id}`
- [ ] `GET/POST/PUT/DELETE /api/v1/classes` and `/api/v1/classes/{id}`
- [ ] Course-control sequence in responses
- [ ] Class-course assignment in requests/responses
- [ ] Integration tests

#### US-005d: Runner + Team Endpoints

**Description:** CRUD endpoints for runners and teams — the most complex entities.

**Acceptance Criteria:**
- [ ] `GET/POST/PUT/DELETE /api/v1/runners` and `/api/v1/runners/{id}`
- [ ] `GET/POST/PUT/DELETE /api/v1/teams` and `/api/v1/teams/{id}`
- [ ] Filtering by name, club, class
- [ ] Pagination support
- [ ] Competition entity endpoint (`GET/PUT /api/v1/competitions`)
- [ ] Integration tests

### US-006: JSON REST API — Competition Operations

**Description:** As a frontend developer, I want API endpoints for competition-specific operations (start lists, results, card readout) so that the GUI can support the full competition workflow.

**Acceptance Criteria:**
- [ ] `GET /api/v1/results` returns computed results (per class, overall)
- [ ] `GET /api/v1/startlist` returns start lists
- [ ] `POST /api/v1/cards` handles card readout data submission
- [ ] `POST /api/v1/runners/{id}/status` allows manual status changes (DNS, DNF, DSQ)
- [ ] Result computation uses existing `GeneralResult` logic
- [ ] Endpoints support both preliminary and final results

### US-007–010: React Web Frontend

> **Extracted to separate PRD:** See [`plan/prd-web-frontend.md`](prd-web-frontend.md) for the full breakdown (US-007 through US-010, including US-009a–c and US-010a–c). This work operates exclusively in `src/ui/web/` and can run **in parallel** with all C++ migration work since it only depends on the API contract.

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

**Description:** As a developer, I want shared utilities migrated to `src/util/` so they are available to all modules.

> **Note:** Split into logical groups.

#### US-013a: Core Utils

**Description:** Migrate the fundamental utility functions and exception handling.

**Acceptance Criteria:**
- [ ] `meos_util.h/cpp` migrated to `src/util/` (cross-platform)
- [ ] `meosexception.h` migrated
- [ ] Wide/narrow string conversion (`string2Wide`, `wide2String`, `narrow`, `widen`)
- [ ] Win32 function replacements (`_itow_s` → `to_wstring`, `sprintf_s` → `snprintf`)
- [ ] `std::filesystem` for file operations
- [ ] Unit tests for string conversion and time formatting

#### US-013b: Parsers

**Description:** Migrate XML and CSV parsing utilities.

**Acceptance Criteria:**
- [ ] `xmlparser.h/cpp` migrated to `src/util/`
- [ ] `csvparser.h/cpp` migrated to `src/util/`
- [ ] Unit tests for xmlparser and csvparser

#### US-013c: Time Handling

**Description:** Migrate time-related utilities.

**Acceptance Criteria:**
- [ ] `TimeStamp.h/cpp` migrated to `src/util/`
- [ ] `timeconstants.hpp` migrated to `src/util/`
- [ ] Unit tests for TimeStamp

#### US-013d: Localization

**Description:** Migrate i18n/localization system.

**Acceptance Criteria:**
- [ ] `localizer.h/cpp` migrated to `src/util/` (fully cross-platform)
- [ ] Win32 resource loading replaced with `std::ifstream` + `codecvt`
- [ ] `.lng` files copied to `resources/lang/`
- [ ] Unit tests for localizer

### US-014: I/O and Import/Export

**Description:** As a developer, I want import/export functionality migrated so that data exchange works in the new system.

> **Note:** Split by format — each is independent and can be migrated/tested separately.

#### US-014a: IOF 3.0 XML

**Description:** Migrate IOF XML import and export.

**Acceptance Criteria:**
- [ ] `iof30interface.h/cpp` migrated to `src/io/`
- [ ] IOF 3.0 XML import works (runners, classes, clubs, courses)
- [ ] IOF 3.0 XML export works (results, start lists)
- [ ] Platform-independent (no Win32 XML APIs)
- [ ] Unit tests with sample IOF XML files

#### US-014b: CSV Import/Export

**Description:** Migrate CSV data exchange.

**Acceptance Criteria:**
- [ ] CSV import for runners migrated to `src/io/`
- [ ] CSV export for results/start lists migrated
- [ ] Unit tests for CSV round-trip

#### US-014c: HTML Result Generation

**Description:** Migrate HTML output for results and lists.

**Acceptance Criteria:**
- [ ] HTML result generation migrated to `src/io/`
- [ ] Templates work cross-platform
- [ ] Unit tests for HTML output

#### US-014d: PDF Generation

**Description:** Migrate PDF output using libharu.

**Acceptance Criteria:**
- [ ] PDF generation (`libharu`) migrated to `src/io/`
- [ ] libharu available via vcpkg
- [ ] PDF output works on Linux and Windows
- [ ] Unit tests for PDF generation

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

The migration follows an incremental approach. Both `code/` (legacy) and `src/` (new) coexist during the transition.

0. **Phase 0 — Legacy Preparation:** Fix cross-platform blockers in `code/` *before* migration starts (case sensitivity, Win32-specific APIs in domain code, GUI/domain coupling). All changes stay in the legacy MSBuild project and must not break the Windows build.
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
┌──────────────────────────────────────────────┐
│            React + TypeScript                │
│              (src/ui/web/)                   │
│  ┌────────┬────────┬────────┬────────┐       │
│  │Runners │Classes │Results │ ...    │       │
│  └────────┴────────┴────────┴────────┘       │
└──────────────────┬───────────────────────────┘
                   │ JSON REST API
┌──────────────────┴───────────────────────────┐
│          C++ HTTP Server (src/net/)          │
│  ┌─────────────┐  ┌──────────────────┐       │
│  │ REST Routes │  │ Static File Srv  │       │
│  └──────┬──────┘  └──────────────────┘       │
│         │                                    │
│  ┌──────┴───────────────────────────┐        │
│  │    Domain Layer (src/domain/)    │        │
│  │  oEvent, oRunner, oClass, ...    │        │
│  └──────┬───────────────────────────┘        │
│         │                                    │
│  ┌──────┴───────────────────────────┐        │
│  │    Database Layer (src/db/)      │        │
│  │            SQLite                │        │
│  └──────────────────────────────────┘        │
│                                              │
│  ┌──────────────────────────────────┐        │
│  │    Utilities (src/util/)         │        │
│  │  strings, time, xml, csv, i18n   │        │
│  └──────────────────────────────────┘        │
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
