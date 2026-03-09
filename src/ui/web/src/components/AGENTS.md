# Form Components Guide

This directory contains reusable form components for the MeOS Web Frontend.

## Core Components

- `DataTable`: A reusable data table component with sorting, filtering, and pagination.
  - Supports row selection by setting `enableSelection={true}`.
  - Use `selectedItems` and `onSelectionChange` to manage the selection state.
  - Optional `getItemId` prop to uniquely identify items (defaults to `(item) => item.id`).
- `Checkbox`: A simple Tailwind-styled checkbox component for use in forms and data tables.
- `FormField`: A wrapper that provides a label, required indicator, and error message display.
- `FormInput`: A standard text/number input wrapped in `FormField`.
- `FormSelect`: A standard dropdown select wrapped in `FormField`.
- `SearchableSelect`: A searchable combobox for large option lists, wrapped in `FormField`.
- `ControlSequenceBuilder`: A specialized component for managing a course's control sequence with drag-and-drop-like reordering (up/down buttons) and removal.
- `TeamMemberManager`: A specialized component for managing a team's runner list, supporting reordering and removal, ideal for relay team management.
- `ImportRunnersDialog`: A multi-step dialog for importing runners from CSV and IOF XML files, including a preview step with a data table.

## Usage with react-hook-form

All form components are compatible with `react-hook-form` via `forwardRef`.

Example:
```tsx
<FormInput
  label="Name"
  {...register('name')}
  error={errors.name?.message}
  required
/>
```

## Styling

Components use Tailwind CSS and Radix UI primitives for accessibility and styling. Base UI primitives are located in the `ui/` subdirectory.

## Validation

Use `zod` for schema definition and `zodResolver` from `@hookform/resolvers/zod` to integrate with `react-hook-form`.
