# Web Frontend Setup

## Project Setup (React 18 + Vite 7 + Tailwind 4)

### Initializing the project
Use `npx --yes create-vite@7 src/ui/web --template react-ts` to avoid interactive prompts and ensure compatibility.

For fully automated tasks, manually create `package.json`, `tsconfig.json`, and `vite.config.ts` instead.

### Project Structure
- `src/ui/web/`: Main frontend directory.
- `src/ui/web/src/`: Application source.
- `src/ui/web/src/__tests__/`: Component and logic tests.

### Downgrading to React 18
Vite 7 might default to React 19. If React 18 is required, downgrade manually:
```bash
npm install react@18 react-dom@18 react-router-dom
npm install -D @types/react@18 @types/react-dom@18
```

### Tailwind 4 Configuration
Tailwind 4 is CSS-first. You only need `@import "tailwindcss";` in your main CSS file (e.g., `src/index.css`).
No `tailwind.config.ts` or `postcss.config.js` is required unless using advanced features.

### Vite API Proxy
Configure the proxy in `vite.config.ts` to handle `/api` requests:
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

### Shadcn/UI with Tailwind 4
1. Install base utilities: `lucide-react class-variance-authority clsx tailwind-merge @radix-ui/react-slot`.
2. Define CSS variables for themes in `@layer base` within the main CSS file.
3. Use `@theme` to extend Tailwind's default theme (e.g., custom fonts).

## Testing with Vitest

**Config File:** `vitest.config.ts` should define the `jsdom` environment and `setupFiles`.
**Setup File:** `src/setupTests.ts` should import `@testing-library/jest-dom` for enhanced matchers.

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
- **Package Manager:** Use `npm` consistently.
- **Working Directory:** Always run commands like `npm test` from the `src/ui/web/` directory to ensure relative paths work correctly.
