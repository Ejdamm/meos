# C++ and Frontend Linting Skill

## C++ Linting (clang-tidy & clang-format)

- **clang-format:** Use `BasedOnStyle: LLVM` or `Google`. `LLVM` is often a good baseline for modern C++.
- **clang-tidy:** Enable `readability-*` and `modernize-*` for a good balance of safety and modern standards.
- **Verification:** Run `clang-format -n --Werror` to check without applying changes.

## Frontend Linting (ESLint 9+ & Prettier)

- **ESLint 9 Flat Config:** Use `eslint.config.js`. ESLint 9+ uses a flat configuration system that is different from `.eslintrc`.
- **typescript-eslint:** Use the `tseslint.config()` helper to simplify the configuration.
- **Prettier Integration:** 
  - Install `eslint-config-prettier` to disable ESLint rules that might conflict with Prettier.
  - Install `eslint-plugin-prettier` to run Prettier as an ESLint rule (useful for seeing formatting errors as lint warnings).
- **ESLint 9 Plugin Compatibility:** Some older plugins (like `eslint-plugin-react-hooks`) may have peer dependency conflicts with ESLint 9. Stick to known compatible versions or use `--legacy-peer-deps`.
- **Scripts:** Always include `lint`, `format:check`, and `format:fix` in `package.json`.

## Reusable Snippets

### ESLint 9 Flat Config with React and TypeScript

```javascript
import js from '@eslint/js';
import globals from 'globals';
import reactHooks from 'eslint-plugin-react-hooks';
import reactRefresh from 'eslint-plugin-react-refresh';
import tseslint from 'typescript-eslint';
import react from 'eslint-plugin-react';
import prettier from 'eslint-plugin-prettier';
import eslintConfigPrettier from 'eslint-config-prettier';

export default tseslint.config(
  { ignores: ['dist', 'coverage'] },
  {
    extends: [js.configs.recommended, ...tseslint.configs.recommended],
    files: ['**/*.{ts,tsx}'],
    languageOptions: {
      ecmaVersion: 2020,
      globals: globals.browser,
    },
    plugins: {
      'react-hooks': reactHooks,
      'react-refresh': reactRefresh,
      react: react,
      prettier: prettier,
    },
    rules: {
      ...reactHooks.configs.recommended.rules,
      'react-refresh/only-export-components': ['warn', { allowConstantExport: true }],
      'prettier/prettier': 'error',
    },
  },
  eslintConfigPrettier
);
```

### Typechecking

- Use `npx tsc --noEmit` to verify TypeScript types without generating output files. This is faster and cleaner for CI and manual checks.
