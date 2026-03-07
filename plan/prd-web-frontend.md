# PRD: MeOS Web Frontend

## Introduction

This PRD covers the React + TypeScript web frontend that replaces MeOS's Win32/GDI desktop GUI. The frontend is a standalone Single Page Application (SPA) that communicates exclusively through the JSON REST API — it has **zero dependency on the legacy `code/` directory** and can be developed entirely in parallel with the C++ migration work.

### Context

MeOS currently uses a Win32/GDI GUI built around a custom `gdioutput` wrapper with tab-based navigation (`TabRunner`, `TabClass`, etc.). The new web frontend mirrors this tab structure but uses modern web technologies, making it accessible from any browser on any platform.

The frontend connects to the C++ backend via `http://localhost:<port>/api/v1/...` endpoints. During development, a Vite dev server with API proxying enables frontend work without a running C++ backend (using mock data or a stub server).

### Constraints

- All development happens in `src/ui/web/` — no changes to `code/` or other `src/` directories
- Communication with the backend is exclusively through the JSON REST API (no direct C++ calls)
- The API contract (endpoint paths, request/response JSON shapes) is the only coupling point
- Production builds must output static files that the C++ server can serve (US-011 in main PRD)
- The frontend must work without any build-time dependency on the C++ codebase

## Goals

- Provide a modern, responsive web interface for competition management
- Mirror the existing tab-based workflow for familiarity (Competition, Classes, Courses, Controls, Clubs, Runners, Teams, Results)
- Support tablet use for field operations
- Enable frontend development fully decoupled from C++ backend work

## User Stories

### US-007: React + TypeScript Web GUI Shell

**Description:** As a user, I want a modern web-based interface so that I can manage competitions from any browser on any platform.

**Acceptance Criteria:**
- [ ] React 18+ app with TypeScript in `src/ui/web/`
- [ ] Vite as build tool
- [ ] Routing with React Router (tabs map to routes: `/runners`, `/classes`, `/clubs`, etc.)
- [ ] API client layer with typed interfaces matching REST API
- [ ] Basic layout: navigation sidebar/tabs + content area
- [ ] Responsive design (works on tablets for field use)
- [ ] Production build outputs static files that the C++ server serves

**Implementation Notes:**
- Use a component library (e.g., Radix UI, shadcn/ui) for consistent, accessible UI components
- API client should be generated from or match the API contract types
- Vite proxy config for development: `proxy: { '/api': 'http://localhost:<port>' }`
- Consider a mock API layer (MSW or similar) for frontend-only development

### US-008: Web GUI — Competition Management

**Description:** As a competition organizer, I want to create and configure competitions, manage classes, courses, and clubs through the web interface.

**Acceptance Criteria:**
- [ ] Create/open/save competition
- [ ] CRUD interface for classes (name, course assignment, start method)
- [ ] CRUD interface for courses (name, length, controls)
- [ ] CRUD interface for clubs
- [ ] CRUD interface for controls
- [ ] Form validation with user-friendly error messages

**Implementation Notes:**
- Each entity type maps to a tab/route
- Table views should support sorting, filtering, and inline editing for efficiency
- Form validation should provide immediate feedback (client-side) with server-side validation as backup
- Course editor should include a control sequence builder (drag-and-drop or ordered list)

### US-009: Web GUI — Runner & Team Management

**Description:** As a competition organizer, I want to manage runners and teams through the web interface.

> **Note:** Split into separate stories for basic CRUD, import, and bulk operations — each has different complexity.

#### US-009a: Runner & Team CRUD

**Description:** Basic create/read/update/delete for runners and teams.

**Acceptance Criteria:**
- [ ] CRUD interface for runners (name, club, class, start time, card number)
- [ ] CRUD interface for teams (name, club, class, members)
- [ ] Search/filter runners by name, club, class
- [ ] Form validation with user-friendly error messages

**Implementation Notes:**
- Runner list is the most data-heavy view — needs virtualized scrolling for large competitions (1000+ runners)
- Club and class fields should use searchable dropdowns with typeahead

#### US-009b: Import Runners

**Description:** Import runners from external data sources.

**Acceptance Criteria:**
- [ ] Import runners from CSV
- [ ] Import runners from IOF XML
- [ ] Preview before import with conflict detection
- [ ] Error reporting for invalid data

**Implementation Notes:**
- CSV parsing can happen client-side (Papa Parse or similar)
- IOF XML parsing may be better handled server-side with a dedicated upload endpoint
- Preview step should show a diff-like view: new runners, updated runners, conflicts

#### US-009c: Bulk Operations

**Description:** Efficient operations on multiple runners at once.

**Acceptance Criteria:**
- [ ] Select multiple runners
- [ ] Bulk assign class
- [ ] Bulk assign start times (draw)
- [ ] Bulk status changes

**Implementation Notes:**
- Checkbox selection in table views
- Bulk operations should show a confirmation dialog with the number of affected runners

### US-010: Web GUI — Results & Live View

**Description:** As a competition organizer, I want to view results and start lists in the web interface.

> **Note:** Split into basic view vs. real-time updates vs. export — each is a distinct capability.

#### US-010a: Results & Start List Views

**Description:** Static display of results and start lists.

**Acceptance Criteria:**
- [ ] Results view per class with split times
- [ ] Start list view per class
- [ ] Print-friendly result formatting

**Implementation Notes:**
- Split time display should use standard orienteering formatting (time behind leader, +/- optimal)
- Print styles via CSS `@media print`
- Consider a dedicated print layout component

#### US-010b: Live Results

**Description:** Auto-updating results for live competition monitoring.

**Acceptance Criteria:**
- [ ] Auto-refresh for live results (polling initially)
- [ ] Visual indication of recent changes
- [ ] Configurable refresh interval

**Implementation Notes:**
- Start with polling (`setInterval` + `fetch`), upgrade to WebSocket/SSE later if needed
- Highlight recently changed rows with a fade animation
- Default refresh interval: 10 seconds, configurable in settings

#### US-010c: Results Export

**Description:** Export results in standard formats.

**Acceptance Criteria:**
- [ ] Export results as CSV
- [ ] Export results as IOF XML
- [ ] Export start lists

**Implementation Notes:**
- CSV export can be client-side (generate and trigger download)
- IOF XML export should call a backend endpoint that generates the XML
- Provide download buttons in the results/start list views

## Functional Requirements

- FR-1: The frontend must be a standalone React + TypeScript SPA with no C++ build dependencies
- FR-2: All data access must go through the JSON REST API (`/api/v1/...`)
- FR-3: The frontend must work with Vite dev server (proxy to backend) and as static files served by the C++ server
- FR-4: The UI must be responsive and usable on tablets (min 768px width)
- FR-5: Form validation must provide immediate user feedback
- FR-6: Table views must support sorting, filtering, and pagination
- FR-7: The production build must output to a directory that CMake can bundle with the executable

## Non-Goals

- Native mobile app (responsive web only)
- Offline mode / PWA (may be added later)
- Dark mode (nice-to-have, not required initially)
- Map rendering or course visualization
- SportIdent hardware UI (deferred)
- Speaker/announcer view (deferred)
- Multi-language frontend i18n (backend handles localization initially)

## Design Considerations

- Mirror the existing MeOS tab structure: Competition, Classes, Courses, Controls, Clubs, Runners, Teams, Results, Lists
- Use a component library (Radix UI, shadcn/ui) for consistent, accessible components
- Table views are the primary interaction pattern — optimize for data-heavy displays
- The UI should work well on tablets (orienteering events often use tablets in the field)
- Use TypeScript strict mode for type safety
- API client types should be the single source of truth for data shapes

## Technical Considerations

### Project Structure

```
src/ui/web/
├── src/
│   ├── api/           # API client, types, hooks
│   ├── components/    # Shared UI components
│   ├── pages/         # Route-level components (one per tab)
│   │   ├── Competition.tsx
│   │   ├── Classes.tsx
│   │   ├── Courses.tsx
│   │   ├── Controls.tsx
│   │   ├── Clubs.tsx
│   │   ├── Runners.tsx
│   │   ├── Teams.tsx
│   │   ├── Results.tsx
│   │   └── StartList.tsx
│   ├── hooks/         # Custom React hooks
│   ├── types/         # TypeScript type definitions
│   └── App.tsx
├── package.json
├── tsconfig.json
├── vite.config.ts
└── vitest.config.ts
```

### API Contract (Key Endpoints)

The frontend depends on these REST API endpoints (defined in the main PRD as US-005/US-006):

```
GET/POST       /api/v1/clubs
GET/PUT/DELETE /api/v1/clubs/{id}
GET/POST       /api/v1/controls
GET/PUT/DELETE /api/v1/controls/{id}
GET/POST       /api/v1/courses
GET/PUT/DELETE /api/v1/courses/{id}
GET/POST       /api/v1/classes
GET/PUT/DELETE /api/v1/classes/{id}
GET/POST       /api/v1/runners
GET/PUT/DELETE /api/v1/runners/{id}
GET/POST       /api/v1/teams
GET/PUT/DELETE /api/v1/teams/{id}
GET/PUT         /api/v1/competitions
GET             /api/v1/results
GET             /api/v1/startlist
POST            /api/v1/cards
POST            /api/v1/runners/{id}/status
```

### Development Workflow

The frontend can be developed independently using:
1. **Vite dev server** with hot module replacement
2. **Mock API** (MSW or json-server) for frontend-only development
3. **Proxy to real backend** when C++ server is available

This means frontend work can start immediately and run fully in parallel with backend/migration work.

### Testing

- **Vitest** for unit tests
- **React Testing Library** for component tests
- **MSW** (Mock Service Worker) for API mocking in tests
- ESLint + Prettier for code quality

## Dependency Order

```
US-007 (GUI shell)     — start here, no backend needed
US-008 (competition)   — after US-007, needs mock API
US-009a (runner CRUD)  — after US-007, parallel with US-008
US-009b (import)       — after US-009a
US-009c (bulk ops)     — after US-009a
US-010a (results view) — after US-007, parallel with US-008/009
US-010b (live results) — after US-010a
US-010c (export)       — after US-010a
```

US-007 is the foundation. After that, US-008, US-009a, and US-010a can be developed in parallel.

## Success Metrics

- All CRUD operations work through the web interface for every entity type
- Competition workflow (create → configure → add runners → view results) works end-to-end via the web GUI
- UI is responsive and usable on tablets (768px+)
- Vitest test suite passes with good coverage on components and API client
- Production build produces static files under 2MB (gzipped)
- Page load time under 2 seconds on a local connection
