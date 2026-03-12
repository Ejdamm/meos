# CI/CD and Quality Infrastructure Skill

Expert guidance for maintaining and extending the MeOS build, test, and quality infrastructure.

## GitHub Actions

- **Windows Runner Setup:** Always include `ilammy/msvc-dev-cmd@v1` before CMake configuration on Windows to set up the environment for the Ninja generator.
- **vcpkg Caching:** Use `lukka/run-vcpkg` for automatic bootstrap and caching of vcpkg dependencies.
- **Node.js Caching:** Use `actions/setup-node` with `cache: 'npm'` and `cache-dependency-path` for fast frontend CI runs.
- **Artifact Naming:** Use `${{ runner.os }}-${{ matrix.config }}` as a suffix for build artifacts to avoid name collisions in the matrix.

## Static Analysis (clang-tidy)

- **Integration:** Prefer integrating `clang-tidy` directly into the CMake build via `CMAKE_CXX_CLANG_TIDY`.
- **Warning Suppression:** Use `// NOLINT` for specific lines or `-readability-*` in `.clang-tidy` for global exclusions.
- **Performance:** Clang-tidy is slow. On CI, only run it on one platform (e.g., Linux) to save runner time.

## Code Formatting (clang-format)

- **Execution:** Use `find . -name "*.cpp" -o -name "*.h" | xargs clang-format -i` for bulk formatting.
- **Standards:** Always keep `.clang-format` in sync with the project's C++ version (currently C++20).

## Troubleshooting

- **vcpkg failures:** Check the `vcpkg-manifest-install.log` for details on dependency resolution errors.
- **YAML syntax:** Use a YAML linter if workflows fail to trigger. Common issues include incorrect indentation or missing quotes for complex strings.
- **MSVC Environment:** If Ninja fails to find the compiler on Windows, ensure `msvc-dev-cmd` was executed in the same step or a previous step that exports environment variables.
