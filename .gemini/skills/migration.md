# MeOS Web Migration Skills

## API Mapping
- **Legacy Backend**: Uses an XML-based API at `/meos`. Parameters like `get=competition`, `get=competitor` return XML wrapped in `<MOPComplete>`.
- **New Frontend**: Uses a JSON-based REST API at `/api/v1`.
- **Strategy**: For development, use MSW to mock the JSON API. In production, a proxy or backend update is required.

## TypeScript Settings (Vite 7)
- **Enum Restriction**: `erasableSyntaxOnly: true` is active. Do NOT use `enum`. Use `const object + type` pattern:
  ```typescript
  export const MyEnum = { A: 1, B: 2 } as const;
  export type MyEnum = typeof MyEnum[keyof typeof MyEnum];
  ```
- **Type Imports**: `verbatimModuleSyntax: true` is active. Always use `import type` for interfaces and types.
  ```typescript
  import type { MyType } from './types';
  ```

## Entity Mapping Insights
- **Runner Status**: Map to `StatusOK=1, StatusDNS=20, StatusCANCEL=21, StatusOutOfCompetition=15, StatusMP=3, StatusDNF=4, StatusDQ=5, StatusMAX=6, StatusNoTiming=2, StatusUnknown=0, StatusNotCompeting=99`.
- **Times**: Backend often uses tenths of a second or seconds from zero time. Frontend should handle these as strings or numbers as appropriate for the UI.
