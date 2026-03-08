# Frontend (src/ui/web)

This directory contains the React-based frontend for MeOS, built with Vite and TypeScript.

## Development Patterns
- **Build Tool:** Vite 6
- **UI Library:** React 18
- **Language:** TypeScript 5 (Strict Mode)
- **Styling:** Vanilla CSS (no Tailwind/libraries unless added)
- **Testing:** Vitest + React Testing Library + jsdom
- **Dependencies:** Managed via npm

## Testing Approach
- Use `vitest` as the runner.
- Components are tested using `@testing-library/react`.
- DOM assertions are provided by `@testing-library/jest-dom` (configured in `src/setupTests.ts`).
- Mocking should be done using Vitest's `vi`.

## Key Commands
- `npm run dev`: Start dev server
- `npm run test`: Run tests once
- `npm run test:watch`: Run tests in watch mode
- `npm run test:coverage`: Run tests with coverage report
- `npm run build`: Build production artifacts (with typecheck)
- `npx tsc --noEmit`: Run typecheck only

## Gotchas
- Unused `import React from 'react'` in JSX files will trigger TypeScript errors.
- Vitest globals are enabled (`describe`, `it`, `expect` are available without import).
- `jsdom` is the default environment for tests.
