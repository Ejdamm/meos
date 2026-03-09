# PRD: MeOS Build & Infrastructure

## Introduction

This PRD covers the build system, test infrastructure, and CI/CD pipeline for the MeOS modernization. These are scaffolding stories that can be set up on an **empty project skeleton** — they require no actual MeOS domain code and no dependency on the legacy `code/` directory.

This work can run **in parallel** with both the legacy preparation (Phase 0) and the domain migration, since it only needs a minimal compilable shell to prove out CMake, testing, and CI.

### Context

MeOS currently builds with MSBuild / Visual Studio 2022 (Windows-only), has no real test framework, and no CI/CD. The target state is:

- **CMake** for cross-platform builds (Linux, macOS, Windows)
- **vcpkg** for dependency management
- **Google Test** for C++ unit tests, **Vitest** for React frontend tests
- **GitHub Actions** for automated builds, tests, and quality checks

All three stories establish the infrastructure that every other workstream depends on — but the infrastructure itself only needs a minimal "hello world" level project to be built and verified.

### Constraints

- CMake must support the modular `src/` layout defined in US-002 (main PRD)
- vcpkg integration must work on both Linux and Windows
- Test infrastructure must support per-module test targets
- CI must build and test on at least Linux and Windows
- No dependency on legacy `code/` — works on empty `src/` skeleton

## Goals

- Establish a cross-platform CMake build that compiles on Linux, macOS, and Windows
- Integrate vcpkg as the single dependency manager
- Set up Google Test and Vitest with per-module test targets
- Automate builds, tests, and linting via GitHub Actions
- Provide a working foundation that migration stories (US-003, US-013, etc.) can immediately use

## User Stories

### US-001: CMake Build System

**Description:** As a developer, I want a CMake build system skeleton so that the project can compile on any platform. This story sets up the **build infrastructure only** — it does NOT migrate or compile any MeOS domain code.

**Scope:** A minimal "hello world" level project that proves CMake, vcpkg, and cross-platform compilation work. The actual MeOS source files are added later by migration stories (US-003, US-013, etc.).

**Acceptance Criteria:**
- [ ] Top-level `CMakeLists.txt` exists and builds a minimal stub executable (e.g., `main.cpp` with `int main() { return 0; }`)
- [ ] Module library targets exist as empty shells (e.g., `src/domain/`, `src/util/`) ready for migration stories to populate
- [ ] vcpkg is integrated as the dependency manager (via toolchain file)
- [ ] `vcpkg.json` manifest declares all external dependencies
- [ ] Builds successfully on Linux (GCC/Clang) and Windows (MSVC)
- [ ] Precompiled headers are configured for build performance
- [ ] Debug and Release configurations work

**Out of scope:**
- Compiling any legacy `code/` files
- Migrating domain code (`oRunner`, `oEvent`, etc.) — that is US-003
- Making the full MeOS application build end-to-end — that happens incrementally as migration stories land

**Implementation Notes:**
- Start with a minimal `main.cpp` that compiles and links — domain code comes later via other stories
- Create empty/stub CMake library targets for the modular layout (US-002): `src/domain/`, `src/util/`, etc. — these are placeholders that migration stories will populate with real source files
- **Placeholder .cpp files are required** in every library target (CMake won't create a target with no sources) — a file with just a comment is sufficient
- vcpkg toolchain: `-DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake`
- **VCPKG_ROOT:** On the dev machine it's `~/vcpkg`. CMakePresets.json should use `$env{VCPKG_ROOT}` — set the env var before invoking cmake
- **Use CMake preset version 6** (cmake 3.28 is available). g++ 13.3 and ninja are available as build tools
- Precompiled headers via `target_precompile_headers()` — add common STL headers
- Known risk: `libmysql` via vcpkg is problematic on Linux — stub or exclude early
- The definition of done is: `cmake --build` compiles the skeleton successfully, not that MeOS runs

### US-015: Test Infrastructure

**Description:** As a developer, I want a test framework skeleton so that migration stories can immediately add tests. This story sets up the **test plumbing only** — real entity tests come with the migration stories.

**Acceptance Criteria:**
- [ ] Google Test integrated via vcpkg
- [ ] Test targets in CMake for each module (`ctest` runs all tests)
- [ ] At least one trivial smoke test per module target (e.g., `TEST(Smoke, Compiles) { EXPECT_TRUE(true); }`) to verify the test infrastructure works
- [ ] Vitest configured for React frontend with React Testing Library
- [ ] Test coverage reporting configured

**Out of scope:**
- Writing real domain entity tests — those come with migration stories (US-003, etc.)
- Testing actual MeOS functionality

**Implementation Notes:**
- C++ tests: one test executable per module (e.g., `test_util`, `test_domain`)
- CMake pattern: `add_executable(test_util ...)` + `target_link_libraries(test_util GTest::gtest_main util)`
- **Use `GTest::gtest_main`** — it provides `main()` automatically, no boilerplate needed
- **`find_package(GTest REQUIRED)`** should be in `tests/CMakeLists.txt` (not top-level)
- `ctest` discovers and runs all test executables. **Tip:** `ctest --test-dir build` when using a separate build directory
- Vitest setup belongs in `src/ui/web/` but the CI integration is configured here
- **Do NOT use `npm create vite@latest`** — it's interactive and hangs in automation. Create `package.json`, `vite.config.ts`, etc. manually
- **React 17+ does not need `import React`** in every JSX/TSX file — omit it to avoid unused-import lint errors
- **Use v8 as Vitest coverage provider**, jsdom as test environment for DOM testing
- Coverage: `gcov`/`lcov` for C++, `vitest --coverage` for frontend
- The goal is that when a migration story (e.g., US-003a) lands, it can add test files and they "just work" — no build system changes needed

### US-016: CI/CD Pipeline

**Description:** As a developer, I want automated builds, tests, and quality checks on every push and PR so that regressions are caught early.

**Acceptance Criteria:**
- [ ] GitHub Actions workflow builds on Linux and Windows
- [ ] All C++ tests run in CI
- [ ] All frontend tests run in CI
- [ ] Linting: clang-tidy for C++, ESLint + Prettier for TypeScript
- [ ] Build artifacts uploaded (executables for Linux and Windows)
- [ ] CI runs on push to main and on pull requests

**Implementation Notes:**
- **Use separate workflows** for C++ and frontend — allows independent failures and clearer CI feedback
- GitHub Actions matrix strategy: `os: [ubuntu-latest, windows-latest]`
- **Use `lukka/run-vcpkg`** — it handles vcpkg bootstrap and caching automatically (no manual `actions/cache` needed)
- Workflow steps: checkout → vcpkg bootstrap → cmake configure → build → test → lint → upload artifacts
- **clang-tidy only runs on Linux** — it's not available by default on Windows runners. Guard with `if: runner.os == 'Linux'`
- **Artifact upload needs platform-aware paths** for binaries (Linux vs Windows extensions)
- **Linting config:**
  - `.clang-tidy` with readability + modernize checks; `.clang-format` based on LLVM style
  - **Use `eslint.config.js` (flat config)** for ESLint 9+ — old `.eslintrc.*` is deprecated
  - Integrate Prettier with ESLint via **`eslint-plugin-prettier`** to avoid conflicts
  - clang-format/clang-tidy may need **version suffixes** on some systems (e.g., `clang-format-18`)
- **`actions/setup-node`** has built-in npm caching — use `cache: 'npm'` with `cache-dependency-path`
- **`working-directory`** is essential when running npm commands in a subdirectory
- Frontend CI: `cd src/ui/web && npm ci && npm run lint && npm test && npm run build`

## Functional Requirements

- FR-1: CMake must produce working binaries on Linux (GCC 12+, Clang 15+) and Windows (MSVC v143)
- FR-2: All external dependencies must be declared in `vcpkg.json` and resolved automatically
- FR-3: `ctest` must discover and run all C++ test executables
- FR-4: CI must fail on test failures, build errors, or lint violations
- FR-5: Build artifacts must be downloadable from CI runs

## Non-Goals

- Migrating actual domain code (covered by main PRD US-003)
- Setting up the React project itself (covered by frontend PRD US-007)
- Database or HTTP server setup (covered by main PRD US-004/US-005)
- macOS CI (nice-to-have, not required initially — local macOS builds should work via CMake)

## Dependency Order

```
US-001 (CMake)           — start here, everything else depends on it
US-015 (test infra)      — after US-001 (needs CMake targets to attach tests to)
US-016 (CI/CD)           — after US-001 + US-015 (needs something to build and test)
```

US-001 is the foundation. US-015 and US-016 can overlap once the basic CMake build works.

## Success Metrics

- `cmake --build` succeeds on Linux and Windows from a clean checkout
- `ctest` runs and reports test results
- GitHub Actions workflow runs green on PRs
- New migration stories (US-003, US-013) can immediately add source files and tests without build system changes
- Developer onboarding: clone → `cmake --preset default` → build → test in under 5 minutes
