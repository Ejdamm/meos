import { test, expect, type Page } from '@playwright/test';

const routes = [
  { path: '/competition', label: 'Tävling', heading: /tävling/i },
  { path: '/runners', label: 'Deltagare', heading: /deltagare/i },
  { path: '/teams', label: 'Lag', heading: /lag/i },
  { path: '/classes', label: 'Klasser', heading: /klass/i },
  { path: '/courses', label: 'Banor', heading: /ban/i },
  { path: '/controls', label: 'Kontroller', heading: /kontroll/i },
  { path: '/clubs', label: 'Klubbar', heading: /klubb/i },
  { path: '/si', label: 'SI', heading: /si/i },
  { path: '/lists', label: 'Listor', heading: /list/i },
  { path: '/speaker', label: 'Speaker', heading: /speaker/i },
  { path: '/automations', label: 'Automatik', heading: /automa/i },
] as const;

test.describe('Smoke tests', () => {
  test('app loads and shows header', async ({ page }) => {
    await page.goto('/');
    await expect(page.locator('h1')).toContainText('MeOS');
  });

  test('root redirects to /competition', async ({ page }) => {
    await page.goto('/');
    await page.waitForURL('**/competition');
    expect(page.url()).toContain('/competition');
  });

  test('navigation bar is visible with all links', async ({ page }) => {
    await page.goto('/competition');
    const nav = page.locator('nav[aria-label="Huvudnavigering"]');
    await expect(nav).toBeVisible();

    for (const route of routes) {
      const link = nav.getByRole('link', { name: route.label });
      await expect(link).toBeVisible();
    }
  });

  test('unknown route redirects to /competition', async ({ page }) => {
    await page.goto('/nonexistent-page');
    await page.waitForURL('**/competition');
    expect(page.url()).toContain('/competition');
  });

  for (const route of routes) {
    test(`page ${route.path} renders`, async ({ page }) => {
      await page.goto(route.path);
      // Wait for lazy-loaded page content (Suspense fallback disappears)
      await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });
      // Verify main content area exists
      await expect(page.locator('#main-content')).toBeVisible();
    });
  }

  test('navigation between pages works', async ({ page }) => {
    await page.goto('/competition');
    const nav = page.locator('nav[aria-label="Huvudnavigering"]');

    // Navigate to Runners via click
    await nav.getByRole('link', { name: 'Deltagare' }).click();
    await page.waitForURL('**/runners');
    expect(page.url()).toContain('/runners');

    // Navigate to Classes via click
    await nav.getByRole('link', { name: 'Klasser' }).click();
    await page.waitForURL('**/classes');
    expect(page.url()).toContain('/classes');

    // Navigate back to Competition
    await nav.getByRole('link', { name: 'Tävling' }).click();
    await page.waitForURL('**/competition');
    expect(page.url()).toContain('/competition');
  });

  test('active nav link is highlighted', async ({ page }) => {
    await page.goto('/competition');
    const nav = page.locator('nav[aria-label="Huvudnavigering"]');
    const activeLink = nav.getByRole('link', { name: 'Tävling' });
    await expect(activeLink).toHaveClass(/active/);
  });

  test('skip-nav link exists for accessibility', async ({ page }) => {
    await page.goto('/competition');
    const skipLink = page.locator('a.skip-nav');
    await expect(skipLink).toBeAttached();
    await expect(skipLink).toHaveAttribute('href', '#main-content');
  });
});
