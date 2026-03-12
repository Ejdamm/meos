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
- **Use latest stable versions** for all third-party libraries, tools, and dependencies (npm packages, vcpkg ports, GitHub Actions, etc.). Do not pin to old versions without a documented reason. When creating `package.json`, `vcpkg.json`, or workflow files, look up current stable versions rather than guessing. After running `npm ci`, verify there are no `npm warn deprecated` messages — if there are, update or replace the offending dependencies

## Goals

- Establish a cross-platform CMake build that compiles on Linux, macOS, and Windows
- Integrate vcpkg as the single dependency manager
- Set up Google Test and Vitest with per-module test targets
- Automate builds, tests, and linting via GitHub Actions
- Provide a working foundation that migration stories (US-003, US-013, etc.) can immediately use

## Codebase Patterns (from Previous Runs)

These patterns were discovered during previous Ralph runs and should be followed:

- Use `target_precompile_headers` for common STL headers.
- Always use namespaces (e.g., `meos::util`) in the modernized codebase.
- Prefer vcpkg manifest mode for all external dependencies.
- Follow `lower_case.cpp/h` naming convention for files.
- Use ESLint 9+ flat config with `typescript-eslint` 8+ for frontend modules.
- Use `meos_add_test` macro for standardized C++ test registration.
- Enable code coverage via `-DMEOS_ENABLE_COVERAGE=ON`.
- Separate GitHub Actions workflows for different stacks (C++, frontend).
- Use `lukka/run-vcpkg` for vcpkg integration in CI.
- Use `defaults: run: working-directory` for nested modules in CI.
- Use `MEOS_ENABLE_CLANG_TIDY` toggle in CMake for CI static analysis.
- Use `CMakePresets.json` version 6 for modern C++ projects.
- Integrate vcpkg via `CMAKE_TOOLCHAIN_FILE` in CMake presets.
- Always use `$env{VCPKG_ROOT}` to avoid hardcoding paths.
- Enable `CMAKE_EXPORT_COMPILE_COMMANDS` for IDE support.
- Keep the top-level `CMakeLists.txt` minimal for initial scaffolding.
- Use Vite for React frontend development and building.
- Use Vitest with `jsdom` for React component testing.
- Always add `.prettierignore` to exclude `dist` and `node_modules`.
- Apply coverage flags (`--coverage`) to both compile and link steps for GCC/Clang.
- Do not reference ESLint plugins in rules unless the plugin is actually installed as a dependency.
- CI workflow branch triggers must match the actual default branch name — use `on: [push, pull_request]` to avoid `main` vs `master` mismatches.

## User Stories

### US-001: CMake Build System

**Description:** As a developer, I want a CMake build system skeleton so that the project can compile on any platform. This story sets up the **build infrastructure only** — it does NOT migrate or compile any MeOS domain code.

**Scope:** A minimal "hello world" level project that proves CMake, vcpkg, and cross-platform compilation work. The actual MeOS source files are added later by migration stories (US-003, US-013, etc.).

**Acceptance Criteria:**
- [ ] Top-level `CMakeLists.txt` exists and builds a minimal stub executable named `meos` from `src/main.cpp` (e.g., `int main() { return 0; }`)
- [ ] No module library targets — only the stub executable. Module directories (`src/domain/`, `src/util/`, etc.) are created by migration stories when needed
- [ ] vcpkg is integrated as the dependency manager (via toolchain file)
- [ ] `vcpkg.json` manifest declares all external dependencies
- [ ] Builds successfully on Linux (GCC/Clang) and Windows (MSVC)
- [ ] Debug and Release configurations work
- [ ] `CMAKE_EXPORT_COMPILE_COMMANDS` is enabled for clangd/LSP tooling support
- [ ] `CMakeUserPresets.json` is listed in `.gitignore` for local developer overrides
- [ ] `CMakePresets.json` does NOT hardcode `VCPKG_ROOT` — relies solely on `$env{VCPKG_ROOT}`
- [ ] `vcpkg.json` uses `"version"` (not `"version-string"`) for semver-compatible version strings

**Out of scope:**
- Compiling any legacy `code/` files
- Migrating domain code (`oRunner`, `oEvent`, etc.) — that is US-003
- Making the full MeOS application build end-to-end — that happens incrementally as migration stories land

**Implementation Notes:**
- The entire C++ skeleton is a single `src/main.cpp` that compiles and links — no module subdirectories. Module directories (`src/domain/`, `src/util/`, etc.) are created by migration stories when they need them
- **`vcpkg.json` must include `builtin-baseline`** — a vcpkg Git commit hash that pins the dependency versions. Without it, `lukka/run-vcpkg` in CI will fail. Get the latest with `git ls-remote https://github.com/microsoft/vcpkg.git HEAD`
- vcpkg toolchain: `-DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake`
- **VCPKG_ROOT:** On the dev machine it's `~/vcpkg`. CMakePresets.json should use `$env{VCPKG_ROOT}` — set the env var before invoking cmake
- **Use CMake preset version 6** (cmake 3.28 is available). g++ 13.3 and ninja are available as build tools
- Known risk: `libmysql` via vcpkg is problematic on Linux — stub or exclude early
- The definition of done is: `cmake --build` compiles the skeleton successfully, not that MeOS runs

**Learnings from Previous Runs:**
- The local vcpkg repository might be slightly behind the latest commit hash on GitHub. Always check the local HEAD hash for `builtin-baseline` if configuration fails.
- `VCPKG_ROOT` must be explicitly exported in the shell for the CMake preset to find it — `$env{VCPKG_ROOT}` won't resolve otherwise.
- `CMakePresets.json` must NOT hardcode `VCPKG_ROOT` in the `environment` block — this makes the preset unusable for other developers. Use only `$env{VCPKG_ROOT}` (reads from the shell environment). Developers with non-standard paths should create a `CMakeUserPresets.json` (gitignored) that inherits from `default` and overrides the environment.
- Enable `CMAKE_EXPORT_COMPILE_COMMANDS` in the top-level `CMakeLists.txt` — without it, clangd and other LSP tools cannot provide code navigation or diagnostics. This is especially important for onboarding developers who use VS Code or other LSP-based editors.
- `vcpkg.json` should use `"version"` instead of `"version-string"` when the version follows semver. `version-string` is for non-semver formats and bypasses vcpkg's version comparison logic.
- Do not create empty shell module directories (domain, util, db, etc.) up front — they add boilerplate without value. Let migration stories create module directories when they have real code to put in them.
- Modular `CMakeLists.txt` using `add_subdirectory` is cleaner than monolithic build files.
- Using `target_precompile_headers` on a base library like `meos_util` is a good way to share common STL headers across the project.
- CMake presets version 6 is a good choice for modern C++ projects with cmake 3.28+.
- The top-level `CMakeLists.txt` should be kept minimal, focusing only on project configuration and global settings initially.

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

**Learnings from Previous Runs:**
- `enable_testing()` MUST be in the top-level `CMakeLists.txt` for `ctest` to find tests from the build root.
- Using a custom macro like `meos_add_test` reduces boilerplate in `tests/CMakeLists.txt`.
- Relative paths in `add_executable` within a subdirectory's `CMakeLists.txt` are relative to that subdirectory, not the project root.
- Coverage flags (`--coverage`) work for both GCC and Clang and require both compile and link options.
- Manual creation of smoke tests for each module is repetitive — consider automating this in future runs.
- `find_package(GTest)` is better placed in the `tests/` directory to keep the top-level CMakeLists.txt clean.

### US-017: React Frontend Shell

**Description:** As a developer, I want a minimal React project skeleton in `src/ui/web/` so that the frontend build, test, and CI infrastructure has something to compile and run against. This is the frontend equivalent of the C++ stub executable in US-001.

**Scope:** A minimal buildable React app — just enough for Vitest, ESLint, Prettier, and `npm run build` to succeed. No actual MeOS UI components.

**Acceptance Criteria:**
- [ ] `src/ui/web/package.json` exists with React, TypeScript, Vite, Vitest, ESLint, and Prettier as dependencies
- [ ] `src/ui/web/vite.config.ts` configures the Vite build
- [ ] `src/ui/web/tsconfig.json` configures TypeScript with strict mode
- [ ] A minimal `src/ui/web/src/App.tsx` renders a placeholder element
- [ ] `src/ui/web/src/App.test.tsx` contains a trivial smoke test (renders without crashing)
- [ ] `npm run build` produces output in `src/ui/web/dist/`
- [ ] `npm test` runs Vitest and passes
- [ ] `npm run lint` runs ESLint + Prettier and passes
- [ ] Vitest coverage reporting is configured (v8 provider)

**Out of scope:**
- Actual MeOS UI components, pages, or routing — those come with frontend stories (US-007, etc.)
- State management, API integration, or design system setup

**Implementation Notes:**
- **Do NOT use `npm create vite@latest`** — it's interactive and hangs in automation. Create all files manually
- **React 17+ does not need `import React`** in every JSX/TSX file — omit it to avoid unused-import lint errors
- **Use `eslint.config.js` (flat config)** for ESLint 9+ — old `.eslintrc.*` is deprecated
- Integrate Prettier with ESLint via **`eslint-plugin-prettier`** to avoid conflicts
- **Use v8 as Vitest coverage provider**, jsdom as test environment for DOM testing
- Keep the shell as minimal as possible — migration stories will add real components
- The goal is that US-015 (test infra) and US-016 (CI/CD) can run frontend checks without waiting for domain UI work

**Learnings from Previous Runs:**
- `typescript-eslint` 8.x is required for full ESLint 9 compatibility.
- Prettier might flag formatting in `dist/` if not explicitly ignored, leading to lint failures in CI if the build is run before lint.
- Peer dependency conflicts are common with ESLint 9 as plugins migrate — expect resolution issues.
- Vite requires an `index.html` at the root of the project (or configured) and a `main.tsx` entry point.
- Vitest setup with `jsdom` and `@testing-library/jest-dom` is straightforward and provides a good testing environment for React.
- Using `npm run build` which includes `tsc` ensures type safety before bundling.
- Do not add ESLint rules referencing plugins (e.g., `react-refresh`) that are not installed — this causes lint failures.
- After `npm ci`, check for deprecation warnings (`npm warn deprecated`). Transitive deps like `whatwg-encoding` and `glob` may be pulled in by outdated direct dependencies — update or replace the offending direct dependency to resolve.

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
- **Ninja is preinstalled** on GitHub Actions runners (`ubuntu-latest` and `windows-latest`) — do NOT use third-party actions to install it (e.g., `getsentry/action-setup-ninja` does not exist)
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

**Learnings from Previous Runs:**
- Use `defaults: run: working-directory` in the workflow for nested frontend modules rather than `cd` in each step.
- Prettier formatting of `AGENTS.md` files may cause linting failures in frontend CI if not properly handled.
- `clang-tidy` may need explicit installation via `apt-get` on standard GitHub runners before use.
- Adding a CMake option like `MEOS_ENABLE_CLANG_TIDY` makes it easy to selectively enable static analysis in CI without affecting local development by default.
- `lukka/run-vcpkg` handles vcpkg bootstrapping and caching efficiently — no manual `actions/cache` setup needed.
- Clang-Tidy integration via CMake's `CMAKE_CXX_CLANG_TIDY` can be slow; running it as a separate CI step or only on Debug builds saves time.
- Always ensure `package-lock.json` is tracked in git when using `npm ci` in CI.
- Workflow branch triggers (`on: push/pull_request: branches`) must match the actual default branch — use `on: [push, pull_request]` to trigger on all branches and avoid `main` vs `master` mismatches.

## Functional Requirements

- FR-1: CMake must produce working binaries on Linux (GCC 12+, Clang 15+) and Windows (MSVC v143)
- FR-2: All external dependencies must be declared in `vcpkg.json` and resolved automatically
- FR-3: `ctest` must discover and run all C++ test executables
- FR-4: CI must fail on test failures, build errors, or lint violations
- FR-5: Build artifacts must be downloadable from CI runs

## Non-Goals

- Migrating actual domain code (covered by main PRD US-003)
- Building out actual React UI components/pages (covered by frontend PRD US-007) — US-017 only provides a minimal shell
- Database or HTTP server setup (covered by main PRD US-004/US-005)
- macOS CI (nice-to-have, not required initially — local macOS builds should work via CMake)

## Dependency Order

```
US-001 (CMake)           — start here, everything else depends on it
US-017 (React shell)     — independent of US-001 (no C++ dependency), can start in parallel
US-015 (test infra)      — after US-001 + US-017 (needs CMake targets and React shell to attach tests to)
US-016 (CI/CD)           — after US-001 + US-015 + US-017 (needs something to build and test)
```

US-001 and US-017 can run in parallel. US-015 and US-016 can overlap once both foundations are in place.

## Success Metrics

- `cmake --build` succeeds on Linux and Windows from a clean checkout
- `ctest` runs and reports test results
- GitHub Actions workflow runs green on PRs
- New migration stories (US-003, US-013) can immediately add source files and tests without build system changes
- Developer onboarding: clone → `cmake --preset default` → build → test in under 5 minutes
