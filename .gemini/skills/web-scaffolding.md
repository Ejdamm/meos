# Web Scaffolding Skill

## Project Setup (React 18 + Vite 7 + Tailwind 4)

### Initializing the project
Use `npx --yes create-vite@7 src/ui/web --template react-ts` to avoid interactive prompts and ensure compatibility.

### Downgrading to React 18
Vite 7 might default to React 19. If React 18 is required, downgrade manually:
```bash
npm install react@18 react-dom@18 react-router-dom
npm install -D @types/react@18 @types/react-dom@18
```

### Tailwind 4 Configuration
Tailwind 4 is a CSS-first framework. You only need to `@import "tailwindcss";` in your main CSS file (e.g., `src/index.css`).
No `tailwind.config.ts` or `postcss.config.js` is strictly required unless using advanced features.

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
For a shadcn-like setup with Tailwind 4:
1. Install base utilities: `lucide-react class-variance-authority clsx tailwind-merge @radix-ui/react-slot`.
2. Define CSS variables for themes in `@layer base` within the main CSS file.
3. Use `@theme` to extend Tailwind's default theme (e.g., custom fonts).
