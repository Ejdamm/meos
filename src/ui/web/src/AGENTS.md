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

## Responsive Design
- Sidebar is fixed on desktop (`lg:static`) and off-canvas on mobile.
- Mobile sidebar is controlled by a hamburger menu in the header.
- Breakpoint for sidebar transition is `lg` (1024px in Tailwind 4 default, but can be adjusted).
