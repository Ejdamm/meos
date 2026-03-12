# Skill: Cross-platform CI with vcpkg and CMake

## Patterns

- **lukka/run-vcpkg**: Use this action to handle vcpkg bootstrapping, dependency installation, and caching. It's more robust than manual setup.
- **VCPKG_ROOT**: Always ensure `VCPKG_ROOT` is set and points to the same location as your CMake presets/toolchain file.
- **Separate Workflows**: Use separate YAML files for backend (C++) and frontend (Node/React) to isolate build environments and speed up feedback.
- **Matrix Strategy**: Use GitHub Actions' matrix strategy to test across platforms (Linux, Windows) and build types (Debug, Release).
- **Clang-Tidy via CMake**: Use `CMAKE_CXX_CLANG_TIDY` for integrated static analysis. Enable it selectively (e.g., only on Linux Debug builds) to save CI time.
- **Artifact Upload**: Use `actions/upload-artifact` with platform-aware paths (e.g., `meos` vs `meos.exe`).

## Gotchas

- **package-lock.json**: Must be committed for `npm ci` to work reliably in CI. If it's in `.gitignore`, it will cause CI failures.
- **Ninja**: Preinstalled on GitHub runners, so don't use extra actions to install it unless a specific version is needed.
- **vcpkg.json location**: If `vcpkg.json` is not in the root, specify `vcpkgJsonGlob` in `lukka/run-vcpkg`.
- **Prettier/ESLint in CI**: Ensure they are configured to ignore build artifacts and coverage reports to avoid false positives.

## Reusable Snippets

### C++ Workflow Step
```yaml
    - name: Restore vcpkg and build dependencies
      uses: lukka/run-vcpkg@v11
      with:
        vcpkgJsonGlob: 'vcpkg.json'
```

### Frontend Workflow Step
```yaml
    - name: Use Node.js
      uses: actions/setup-node@v4
      with:
        node-version: 'latest'
        cache: 'npm'
        cache-dependency-path: src/ui/web/package-lock.json
```
