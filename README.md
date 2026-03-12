# meos
MeOS - A Much Easier Orienteering System

Source code for the MeOS project (www.melin.nu/meos)

## Frontend (Web UI)

### Prerequisites

- Node.js 18+ and npm

### Build & Run

```bash
cd src/ui/web
npm ci
npm run build      # Production build → src/ui/web/dist/
npm run dev        # Development server with HMR
```

### Quality Checks

```bash
npm run lint       # ESLint
npm run typecheck  # TypeScript type checking
npm test           # Vitest unit tests
npm run test:coverage  # Tests with v8 coverage
```
