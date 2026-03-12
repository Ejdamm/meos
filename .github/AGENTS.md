# GitHub Actions Conventions

- **Separate Workflows:** C++ and Frontend have separate workflow files (`cpp.yml` and `frontend.yml`) to allow independent triggers and parallel execution.
- **Matrix Builds:** The C++ workflow uses a matrix strategy to build on both `ubuntu-latest` and `windows-latest`.
- **vcpkg Integration:** Use `lukka/run-vcpkg@v11` for vcpkg bootstrap and caching. Ensure `vcpkgJsonGlob` is set to `vcpkg.json`.
- **MSVC Environment:** On Windows runners, always use `ilammy/msvc-dev-cmd@v1` before CMake configuration to ensure the MSVC toolchain is correctly set up for the Ninja generator.
- **Static Analysis:** `clang-tidy` is integrated into the CMake build via the `MEOS_ENABLE_CLANG_TIDY` option. It is enabled only on Linux CI runners.
- **Caching:** The Frontend workflow uses `actions/setup-node` with `cache: 'npm'` and `cache-dependency-path: src/ui/web/package-lock.json`.
- **Triggers:** Both workflows trigger on `push` and `pull_request` without branch filters to ensure coverage across all branches during the migration.
- **Artifacts:** Use `actions/upload-artifact@v4` to upload build outputs. Use platform-aware naming for artifacts (e.g., `meos-${{ runner.os }}-${{ matrix.config }}`).
