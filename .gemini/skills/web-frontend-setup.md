# Web Frontend Setup

## Project Setup (React 19 + Vite 6 + TypeScript 5.8)

### Initializing the project
Do NOT use `npm create vite` — create all files manually for full control.

### Project Structure
- `src/ui/web/`: Main frontend directory
- `src/ui/web/src/`: Application source
- `src/ui/web/index.html`: Vite entry point (must be at project root, NOT in src/)
- `src/ui/web/vite.config.ts`: Vite build config
- `src/ui/web/vitest.config.ts`: Separate Vitest config (jsdom + v8 coverage)
- `src/ui/web/eslint.config.js`: ESLint 9+ flat config

### Dependency Versions (tested working)
- React 19, React DOM 19
- Vite 6, @vitejs/plugin-react 4
- TypeScript ~5.8
- Vitest 3, @vitest/coverage-v8 3
- jsdom 28+ (NOT 26 — older versions depend on deprecated whatwg-encoding)
- ESLint 9+, typescript-eslint 8+
- @testing-library/react 16 (works with React 19)

### Avoiding npm Deprecation Warnings
Add overrides to package.json to suppress transitive deprecation warnings:
```json
"overrides": {
  "glob": "^13.0.0"
}
```
- `jsdom@28` replaced `whatwg-encoding` with `@exodus/bytes` — use jsdom 28+
- `glob@10` and `glob@11` are deprecated by the author — override to `^13.0.0`
- Use `npm ls <pkg>` to trace which direct dep pulls in a deprecated transitive dep

### ESLint 9 Flat Config
Use `tseslint.config()` wrapper from `typescript-eslint` package:
```javascript
import js from "@eslint/js";
import globals from "globals";
import reactHooks from "eslint-plugin-react-hooks";
import reactRefresh from "eslint-plugin-react-refresh";
import tseslint from "typescript-eslint";

export default tseslint.config(
  { ignores: ["dist"] },
  {
    extends: [js.configs.recommended, ...tseslint.configs.recommended],
    files: ["**/*.{ts,tsx}"],
    languageOptions: {
      ecmaVersion: 2020,
      globals: globals.browser,
    },
    plugins: {
      "react-hooks": reactHooks,
      "react-refresh": reactRefresh,
    },
    rules: {
      ...reactHooks.configs.recommended.rules,
    },
  },
);
```

### TypeScript Config
Use project references with `tsc -b`:
- `tsconfig.json` — root with `"files": []` and references to app/node configs
- `tsconfig.app.json` — strict mode, `"jsx": "react-jsx"`, includes `src/`
- `tsconfig.node.json` — for config files (vite.config.ts, vitest.config.ts)

### Vite API Proxy
```typescript
server: {
  proxy: {
    '/api': {
      target: 'http://localhost:2009',
      changeOrigin: true,
    },
  },
}
```

## Testing with Vitest

Use a separate `vitest.config.ts` (not inline in `vite.config.ts`):
```typescript
import { defineConfig } from "vitest/config";
import react from "@vitejs/plugin-react";

export default defineConfig({
  plugins: [react()],
  test: {
    globals: true,
    environment: "jsdom",
    coverage: {
      provider: "v8",
      reporter: ["text", "json", "html"],
    },
  },
});
```

## Common Issues
- **Unused React Import:** React 17+ doesn't require `import React` for JSX. Strict `noUnusedLocals` will fail if present.
- **Package Manager:** Use `npm` consistently.
- **Working Directory:** Always run npm commands from `src/ui/web/`.
- **Build script:** Use `"tsc -b && vite build"` — `noEmit` is set in tsconfig, no need for `--noEmit` flag in build script.
- **Vite entry point:** `index.html` must be at web project root with `<script type="module" src="/src/main.tsx">`.
