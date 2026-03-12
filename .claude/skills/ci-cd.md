# CI/CD Infrastructure Skill

## GitHub Actions Patterns

- **Separation of Concerns:** Use separate workflow files for different stacks (e.g., `cpp.yml`, `frontend.yml`). This makes it easier to debug failures and allows for parallel execution.
- **Matrix Strategy:** Use a matrix strategy to test on multiple operating systems (e.g., `ubuntu-latest`, `windows-latest`).
- **Caching:** 
  - For Node.js, use `actions/setup-node` with `cache: 'npm'` and specify `cache-dependency-path` to include subdirectories.
  - For vcpkg, use `lukka/run-vcpkg` which handles dependency installation and binary caching.
- **Artifacts:** Upload build artifacts with platform-specific names to distinguish between builds from different matrix jobs.

## C++ CI Configuration

- **vcpkg Integration:** Use `lukka/run-vcpkg` to manage dependencies. It integrates with CMake and handles caching efficiently.
- **Ninja Generator:** Prefer `Ninja` for faster builds across all platforms. Ninja is preinstalled on GitHub runners.
- **Static Analysis in CI:** 
  - Guard `clang-tidy` or other heavy linting tools to run only on a single platform (e.g., Linux) to save runner minutes.
  - Use `CMAKE_CXX_CLANG_TIDY` in CMake to integrate `clang-tidy` directly into the build process.
- **Testing:** Use `ctest --output-on-failure` to ensure that test logs are available in the GitHub Actions output when a test fails.

## Frontend CI Configuration

- **Working Directory:** Use `defaults: run: working-directory: path/to/frontend` to avoid repeating paths in every step.
- **npm ci:** Always use `npm ci` instead of `npm install` in CI environments to ensure a consistent and clean installation of dependencies based on `package-lock.json`.
- **Pre-build Checks:** Run linting and tests before building the application to catch errors early and avoid unnecessary build time.

## Gotchas

- **Trigger without branch filter:** Use `on: [push, pull_request]` — adding a branch filter like `branches: [main]` breaks repos that use `master` as default branch.
- **MSVC on Windows:** Always use `ilammy/msvc-dev-cmd@v1` before cmake on Windows — without it, cmake may pick MinGW g++ instead of MSVC cl.exe.
- **Conditional cmake flags:** Use `${{ runner.os == 'Linux' && '-DFLAG=ON' || '' }}` inline in the cmake command — avoids duplicating configure steps per platform.
- **clang-tidy:** Disable `modernize-use-trailing-return-type` in `.clang-tidy` — it rewrites every function to trailing return style, which is too noisy for legacy code.
- **Artifact paths:** Use `if-no-files-found: warn` on upload-artifact to prevent workflow failure when paths differ across platforms (e.g., `meos` vs `meos.exe`).

## Reusable Snippets

### C++ CI Workflow (cpp.yml)

```yaml
name: C++ CI
on: [push, pull_request]
jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest]
    steps:
      - uses: actions/checkout@v4
      - name: Setup MSVC
        if: runner.os == 'Windows'
        uses: ilammy/msvc-dev-cmd@v1
      - uses: lukka/run-vcpkg@v11
      - name: Configure
        run: cmake --preset default ${{ runner.os == 'Linux' && '-DMEOS_ENABLE_CLANG_TIDY=ON' || '' }}
      - name: Build
        run: cmake --build --preset default
      - name: Test
        run: ctest --test-dir build --output-on-failure
      - name: Upload build artifacts
        uses: actions/upload-artifact@v4
        with:
          name: meos-${{ runner.os }}
          path: |
            build/meos
            build/meos.exe
          if-no-files-found: warn
```

### Frontend CI Workflow (frontend.yml)

```yaml
name: Frontend CI
on: [push, pull_request]
jobs:
  build:
    runs-on: ubuntu-latest
    defaults:
      run:
        working-directory: src/ui/web
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-node@v4
        with:
          node-version: 20
          cache: 'npm'
          cache-dependency-path: 'src/ui/web/package-lock.json'
      - run: npm ci
      - run: npm run lint
      - run: npm test
      - run: npm run build
```
