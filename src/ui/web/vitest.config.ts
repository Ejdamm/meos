import { defineConfig } from 'vitest/config';
import react from '@vitejs/plugin-react';

export default defineConfig({
  plugins: [react()],
  test: {
    globals: true,
    environment: 'jsdom',
    setupFiles: ['./src/test/setup.ts'],
    css: true,
    include: ['src/**/*.test.{ts,tsx}'],
    reporters: process.env.CI
      ? ['default', ['junit', { outputFile: 'reports/vitest-junit.xml' }]]
      : ['default'],
    coverage: {
      provider: 'v8',
      reporter: ['text', 'lcov', 'json-summary'],
      reportsDirectory: 'reports/coverage',
      thresholds: {
        lines: 70,
        branches: 65,
        functions: 70,
        statements: 70,
      },
    },
  },
});
