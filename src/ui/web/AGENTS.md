# Frontend Development (src/ui/web)

This is the React frontend for the modernized MeOS platform.

## Tech Stack

- **React 19** with TypeScript
- **Vite 8** for building and development
- **Vitest** for unit and component testing
- **ESLint 9** (Flat Config) with **Prettier** integration
- **jsdom** for the test environment

## Conventions

- **File Naming:** PascalCase for components (`App.tsx`), camelCase for utilities and tests (`App.test.tsx`).
- **Styling:** CSS modules or plain CSS are preferred (Vanilla CSS as per PRD).
- **Testing:**
  - Place tests next to the file they test (`File.test.tsx`).
  - Use `@testing-library/react` for component testing.
  - Smoke tests should at least verify the component renders without crashing.
- **Linting:**
  - ESLint is configured with strict TypeScript rules.
  - Prettier is integrated as an ESLint rule; `npm run lint` checks both.
  - Use `npm run lint -- --fix` to automatically format code.

## Gotchas

- **Vite 8 / Vitest Type Mismatch:** There may be a type mismatch between Vite 8's `defineConfig` and Vitest's `InlineConfig`. If TypeScript complains about the `test` property in `vite.config.ts`, cast the configuration object to a custom interface that extends `UserConfig` from `vite` and adds the `test` property from `vitest/node`.
- **Root Element:** `src/main.tsx` expects an element with `id="root"` to exist in `index.html`. It includes a runtime check and throws an error if not found, avoiding non-null assertions.
- **JSX Transform:** React 17+ JSX transform is enabled; there's no need to `import React from 'react'` in every `.tsx` file.
