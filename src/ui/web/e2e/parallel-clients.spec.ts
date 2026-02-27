import { test, expect, type Page, type Route, type Browser, type BrowserContext } from '@playwright/test';

/**
 * E2E: Parallel clients.
 * Opens two browser contexts sharing the same mock API state,
 * makes changes in one context, and verifies the other sees updates
 * when it refetches/navigates.
 */

// --------------- shared mock state ---------------
let runners: any[] = [];
let competitions: any[] = [];
let classes: any[] = [];
let clubs: any[] = [];
let courses: any[] = [];

function resetState() {
  competitions = [
    { id: 1, name: 'Parallell-test', date: '2026-03-01', zeroTime: '10:00:00', numRunners: 0, numClasses: 1, numCourses: 1, numCards: 0 },
  ];
  classes = [{ id: 1, name: 'H21', type: 'individual', numStages: 1, courseId: 1 }];
  clubs = [{ id: 1, name: 'OK Linné', country: 'SWE' }];
  courses = [{ id: 1, name: 'Bana A', length: 4200, numControls: 3, controls: [31, 32, 33] }];
  runners = [];
}

/** Install route mocks on a page — all pages share the same mutable state arrays. */
async function mockApi(page: Page) {
  await page.route('**/api/competitions', async (route: Route) => {
    if (route.request().method() === 'GET') return route.fulfill({ json: competitions });
    if (route.request().method() === 'POST') {
      const body = route.request().postDataJSON();
      const created = { id: competitions.length + 1, numRunners: 0, numClasses: 0, numCourses: 0, numCards: 0, ...body };
      competitions.push(created);
      return route.fulfill({ status: 201, json: created });
    }
    return route.continue();
  });

  await page.route('**/api/competitions/*', async (route: Route) => {
    if (route.request().method() === 'PUT') {
      const body = route.request().postDataJSON();
      const url = route.request().url();
      const id = Number(url.split('/').pop());
      const idx = competitions.findIndex(c => c.id === id);
      if (idx >= 0) {
        competitions[idx] = { ...competitions[idx], ...body };
        return route.fulfill({ json: competitions[idx] });
      }
    }
    return route.fulfill({ status: 404 });
  });

  await page.route('**/api/runners', async (route: Route) => {
    if (route.request().method() === 'GET') return route.fulfill({ json: runners });
    if (route.request().method() === 'POST') {
      const body = route.request().postDataJSON();
      const cls = classes.find(c => c.id === body.classId);
      const club = clubs.find(c => c.id === body.clubId);
      const created = {
        id: runners.length + 1,
        startTime: 3600,
        finishTime: 0,
        runningTime: 0,
        status: 0,
        sex: 0,
        class: cls?.name ?? '',
        club: club?.name ?? '',
        ...body,
      };
      runners.push(created);
      competitions[0].numRunners = runners.length;
      return route.fulfill({ status: 201, json: created });
    }
    return route.continue();
  });

  await page.route('**/api/runners/*', async (route: Route) => {
    if (route.request().method() === 'DELETE') {
      const url = route.request().url();
      const id = Number(url.split('/').pop());
      runners = runners.filter(r => r.id !== id);
      competitions[0].numRunners = runners.length;
      return route.fulfill({ status: 204 });
    }
    return route.continue();
  });

  await page.route('**/api/classes', async (route: Route) => {
    return route.fulfill({ json: classes });
  });

  await page.route('**/api/clubs', async (route: Route) => {
    return route.fulfill({ json: clubs });
  });

  await page.route('**/api/courses', async (route: Route) => {
    if (route.request().method() === 'GET') return route.fulfill({ json: courses });
    if (route.request().method() === 'POST') {
      const body = route.request().postDataJSON();
      const created = { id: courses.length + 1, numControls: 0, controls: [], ...body };
      courses.push(created);
      competitions[0].numCourses = courses.length;
      return route.fulfill({ status: 201, json: created });
    }
    return route.continue();
  });

  await page.route('**/api/courses/*', async (route: Route) => {
    return route.continue();
  });

  await page.route('**/api/controls', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/cards', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/cards/read', async (route: Route) => {
    return route.fulfill({ json: {} });
  });

  await page.route('**/api/punches', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/teams', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/results', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/lists', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/automations', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/speaker/**', async (route: Route) => {
    return route.fulfill({ json: {} });
  });

  await page.route('**/api/import/**', async (route: Route) => {
    return route.fulfill({ json: { success: true } });
  });

  await page.route('**/api/export/**', async (route: Route) => {
    return route.fulfill({ json: {} });
  });
}

// --------------- tests ---------------

test.describe('Parallel clients', () => {
  let contextA: BrowserContext;
  let contextB: BrowserContext;
  let pageA: Page;
  let pageB: Page;

  test.beforeEach(async ({ browser }) => {
    resetState();
    contextA = await browser.newContext();
    contextB = await browser.newContext();
    pageA = await contextA.newPage();
    pageB = await contextB.newPage();
    await mockApi(pageA);
    await mockApi(pageB);
  });

  test.afterEach(async () => {
    await contextA.close();
    await contextB.close();
  });

  test('runner added in client A appears in client B after refetch', async () => {
    // Client A: go to runners page and add a runner
    await pageA.goto('/runners');
    await expect(pageA.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    await pageA.getByRole('button', { name: 'Ny deltagare' }).click();
    await pageA.locator('#runner-name').fill('Klient-A Löpare');
    await pageA.locator('#runner-class').selectOption({ value: '1' });
    await pageA.locator('#runner-club').selectOption({ value: '1' });
    await pageA.locator('#runner-card').fill('99001');
    await pageA.locator('#runner-bib').fill('501');
    await pageA.getByRole('button', { name: 'Spara' }).click();
    await expect(pageA.getByText('Klient-A Löpare')).toBeVisible({ timeout: 5_000 });

    // Client B: navigate to runners — shared state already has the new runner
    await pageB.goto('/runners');
    await expect(pageB.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });
    await expect(pageB.getByText('Klient-A Löpare')).toBeVisible({ timeout: 5_000 });
  });

  test('competition update in client A is visible in client B', async () => {
    // Both clients open competition page
    await pageA.goto('/competition');
    await expect(pageA.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });
    await expect(pageA.getByText('Parallell-test')).toBeVisible();

    await pageB.goto('/competition');
    await expect(pageB.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });
    await expect(pageB.getByText('Parallell-test')).toBeVisible();

    // Client A: rename competition
    await pageA.getByRole('button', { name: 'Redigera' }).click();
    await pageA.locator('#comp-name').clear();
    await pageA.locator('#comp-name').fill('Uppdaterad av klient A');
    await pageA.getByRole('button', { name: 'Spara' }).click();
    await expect(pageA.getByText('Uppdaterad av klient A')).toBeVisible({ timeout: 5_000 });

    // Client B: reload to refetch from server (bypasses TanStack Query cache)
    await pageB.reload();
    await expect(pageB.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });
    await expect(pageB.getByText('Uppdaterad av klient A')).toBeVisible({ timeout: 5_000 });
  });

  test('multiple runners from different clients merge correctly', async () => {
    // Client A adds a runner
    await pageA.goto('/runners');
    await expect(pageA.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    await pageA.getByRole('button', { name: 'Ny deltagare' }).click();
    await pageA.locator('#runner-name').fill('Från Klient A');
    await pageA.locator('#runner-class').selectOption({ value: '1' });
    await pageA.locator('#runner-club').selectOption({ value: '1' });
    await pageA.locator('#runner-card').fill('10001');
    await pageA.locator('#runner-bib').fill('100');
    await pageA.getByRole('button', { name: 'Spara' }).click();
    await expect(pageA.getByText('Från Klient A')).toBeVisible({ timeout: 5_000 });

    // Client B adds another runner
    await pageB.goto('/runners');
    await expect(pageB.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    await pageB.getByRole('button', { name: 'Ny deltagare' }).click();
    await pageB.locator('#runner-name').fill('Från Klient B');
    await pageB.locator('#runner-class').selectOption({ value: '1' });
    await pageB.locator('#runner-club').selectOption({ value: '1' });
    await pageB.locator('#runner-card').fill('20001');
    await pageB.locator('#runner-bib').fill('200');
    await pageB.getByRole('button', { name: 'Spara' }).click();
    await expect(pageB.getByText('Från Klient B')).toBeVisible({ timeout: 5_000 });

    // Client A: reload to see both runners
    await pageA.reload();
    await expect(pageA.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });
    await expect(pageA.getByText('Från Klient A')).toBeVisible({ timeout: 5_000 });
    await expect(pageA.getByText('Från Klient B')).toBeVisible({ timeout: 5_000 });

    // Client B: reload to verify both visible too
    await pageB.reload();
    await expect(pageB.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });
    await expect(pageB.getByText('Från Klient A')).toBeVisible({ timeout: 5_000 });
    await expect(pageB.getByText('Från Klient B')).toBeVisible({ timeout: 5_000 });
  });

  test('course created in client A shows in client B on navigation', async () => {
    // Client A: create a course
    await pageA.goto('/courses');
    await expect(pageA.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    await pageA.getByRole('button', { name: 'Ny bana' }).click();
    await pageA.locator('#course-name').fill('Parallell-bana');
    await pageA.locator('#course-length').fill('3500');
    await pageA.getByRole('button', { name: 'Spara' }).click();
    await expect(pageA.getByText('Parallell-bana')).toBeVisible({ timeout: 5_000 });

    // Client B: navigate to courses and see the new course
    await pageB.goto('/courses');
    await expect(pageB.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });
    await expect(pageB.getByText('Parallell-bana')).toBeVisible({ timeout: 5_000 });
  });
});
