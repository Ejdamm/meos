# Frontend Migration Skill

This skill provides patterns and knowledge for migrating or setting up the React frontend with Vite and Vitest.

## Scaffolding the Frontend
- **Avoid Interactive Tools:** `npm create vite` and similar tools are often interactive. For automated tasks, manually create `package.json`, `tsconfig.json`, and `vite.config.ts`.
- **Project Structure:**
  - `src/ui/web/`: Main frontend directory.
  - `src/ui/web/src/`: Application source.
  - `src/ui/web/src/__tests__/`: Component and logic tests.

## Testing with Vitest
- **Config File:** `vitest.config.ts` should define the `jsdom` environment and `setupFiles`.
- **Setup File:** `src/setupTests.ts` should import `@testing-library/jest-dom` for enhanced matchers.
- **Example Config:**
```typescript
/// <reference types="vitest" />
import { defineConfig } from 'vitest/config'
import react from '@vitejs/plugin-react'

export default defineConfig({
  plugins: [react()],
  test: {
    globals: true,
    environment: 'jsdom',
    setupFiles: ['./src/setupTests.ts'],
    coverage: {
      provider: 'v8',
      reporter: ['text', 'json', 'html'],
    },
  },
})
```

## Common Issues
- **Unused React Import:** React 17+ doesn't require explicit `import React from 'react'` for JSX. Strict TypeScript checks (`noUnusedLocals`) will fail if it's present but unused.
- **Package Manager:** Use `npm` for dependency management consistently.
- **Node.js Environment:** Always run commands like `npm test` from the `src/ui/web/` directory to ensure relative paths in configuration work correctly.
