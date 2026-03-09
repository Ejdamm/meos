# MeOS Web Frontend - Source Patterns

## Directory Structure
- `src/components/`: Reusable UI components.
- `src/pages/`: Page-level components associated with routes.
- `src/lib/`: Utility functions and shared library configurations.
- `src/hooks/`: Custom React hooks for data fetching and state management.
- `src/api/`: API client and type definitions.

## Styling
- Use **Tailwind CSS 4** for styling.
- Use the `cn` utility from `@/lib/utils` for conditional class merging.
- Prefer Radix UI primitives for complex accessible components.
- Icons are from **lucide-react**.

## Routing
- Use `react-router-dom` for navigation.
- Main layout is defined in `src/components/Layout.tsx` and includes the sidebar.
- Routes are configured in `App.tsx`.
- Use `NavLink` for sidebar links to get automatic "active" state styling.

## API and Data Fetching
- **Client**: Use the singleton `api` object from `@/api/client` for all backend communication.
- **Hooks**: Prefer custom hooks in `@/hooks/` (e.g., `useRunners`, `useClubs`) instead of calling the API directly in components.
- **Mutations**: The `useMutation` hook in `@/hooks/useApi` provides a `mutate` function that takes a single `variables` argument. Since it doesn't support options like `onSuccess` in the `mutate` call, use `async/await` to handle success/error logic or pass callbacks directly to the `useMutation` hook.
- **Endpoints**: All endpoints are under `/api/v1/`.
- **Mocking**: Use MSW (Mock Service Worker) for local development if the C++ backend is not available.
  - Mock data is in `src/mocks/data.ts`.
  - API handlers are in `src/mocks/handlers.ts`.
  - Mocking is enabled in `src/main.tsx` if `import.meta.env.MODE === 'development'`.
  - Always run `npx msw init public/` if the service worker script is missing.

## Forms and Inputs
- **React Hook Form**: Use `react-hook-form` with `zodResolver` for form management and validation.
- **Selects**: The `FormSelect` component expects `value` and `onValueChange` to be strings. When working with numeric IDs, convert them using `String()` for the select and `Number()` when submitting the form.
- **Numeric Inputs**: For numeric fields in `FormInput`, use `type="number"` and pass `{ valueAsNumber: true }` to the `register` function to ensure correct type handling in the form state.

## Dialogs
- **FormDialog**: Supports an optional `size` property (`sm`, `md`, `lg`, `xl`) to control the maximum width. Default is `sm` (425px). Use `lg` or `xl` for complex forms (e.g., course editor with control sequence).
- **ConfirmDialog**: Use for delete operations and other critical actions.

## Specialized Components
- **ControlSequenceBuilder**: Used in `CoursesPage` to manage the list of control IDs for a course. It handles adding from a searchable list, removing, and reordering via up/down buttons.

## Data Table
- Use the `DataTable` component from `src/components/DataTable` for all entity lists.
- It supports sorting, global filtering, and pagination out of the box.
- Columns are defined as an array of objects:
  ```typescript
  const columns: Column<MyEntity>[] = [
    { header: 'Name', accessorKey: 'name', sortable: true },
    { header: 'Status', accessorKey: 'status', cell: (item) => <StatusBadge status={item.status} /> }
  ];
  ```

## TypeScript and Types
- **Interfaces**: Defined in `@/api/types.ts`.
- **Enums**: Do NOT use TypeScript `enum`. Use `const object + type` pattern to comply with `erasableSyntaxOnly` settings.
  ```typescript
  export const MyStatus = { OK: 1, Error: 0 } as const;
  export type MyStatus = typeof MyStatus[keyof typeof MyStatus];
  ```
- **Imports**: Always use `import type` when importing interfaces or types to comply with `verbatimModuleSyntax`.
  ```typescript
  import type { MyType } from './types';
  ```
