import { test, expect, type Page, type Route } from '@playwright/test';

/**
 * E2E: Complete competition flow.
 * Uses Playwright route interception to mock the REST API,
 * then walks through: create competition → add course → add class →
 * register runner → read SI card → view results.
 */

// --------------- mock state ---------------
let competitions = [
  { id: 1, name: 'Test Competition', date: '2026-03-01', zeroTime: '10:00:00', numRunners: 0, numClasses: 0, numCourses: 0, numCards: 0 },
];
let courses = [
  { id: 1, name: 'Bana A', length: 4200, numControls: 3, controls: [31, 32, 33] },
];
let classes = [
  { id: 1, name: 'H21', type: 'individual', numStages: 1, courseId: 1 },
];
let clubs = [
  { id: 1, name: 'OK Linné', country: 'SWE' },
];
let runners: any[] = [];
let cards: any[] = [];
let controls = [
  { id: 31, name: 'Kontroll 31', status: 0, codes: [31] },
  { id: 32, name: 'Kontroll 32', status: 0, codes: [32] },
  { id: 33, name: 'Kontroll 33', status: 0, codes: [33] },
];
let results: any[] = [];

function resetState() {
  competitions = [
    { id: 1, name: 'Test Competition', date: '2026-03-01', zeroTime: '10:00:00', numRunners: 0, numClasses: 0, numCourses: 0, numCards: 0 },
  ];
  courses = [{ id: 1, name: 'Bana A', length: 4200, numControls: 3, controls: [31, 32, 33] }];
  classes = [{ id: 1, name: 'H21', type: 'individual', numStages: 1, courseId: 1 }];
  clubs = [{ id: 1, name: 'OK Linné', country: 'SWE' }];
  runners = [];
  cards = [];
  controls = [
    { id: 31, name: 'Kontroll 31', status: 0, codes: [31] },
    { id: 32, name: 'Kontroll 32', status: 0, codes: [32] },
    { id: 33, name: 'Kontroll 33', status: 0, codes: [33] },
  ];
  results = [];
}

// --------------- route mocking ---------------
async function mockApi(page: Page) {
  // Competitions
  await page.route('**/api/competitions', async (route: Route) => {
    if (route.request().method() === 'GET') {
      return route.fulfill({ json: competitions });
    }
    if (route.request().method() === 'POST') {
      const body = route.request().postDataJSON();
      const created = { id: 2, numRunners: 0, numClasses: 0, numCourses: 0, numCards: 0, ...body };
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

  // Courses
  await page.route('**/api/courses', async (route: Route) => {
    if (route.request().method() === 'GET') return route.fulfill({ json: courses });
    if (route.request().method() === 'POST') {
      const body = route.request().postDataJSON();
      const created = { id: courses.length + 1, numControls: 0, controls: [], ...body };
      courses.push(created);
      return route.fulfill({ status: 201, json: created });
    }
    return route.continue();
  });

  await page.route('**/api/courses/*', async (route: Route) => {
    if (route.request().method() === 'DELETE') return route.fulfill({ status: 204 });
    return route.continue();
  });

  // Classes
  await page.route('**/api/classes', async (route: Route) => {
    if (route.request().method() === 'GET') return route.fulfill({ json: classes });
    if (route.request().method() === 'POST') {
      const body = route.request().postDataJSON();
      const created = { id: classes.length + 1, ...body };
      classes.push(created);
      return route.fulfill({ status: 201, json: created });
    }
    return route.continue();
  });

  await page.route('**/api/classes/*', async (route: Route) => {
    if (route.request().method() === 'DELETE') return route.fulfill({ status: 204 });
    return route.continue();
  });

  // Clubs
  await page.route('**/api/clubs', async (route: Route) => {
    if (route.request().method() === 'GET') return route.fulfill({ json: clubs });
    return route.continue();
  });

  await page.route('**/api/clubs/*', async (route: Route) => {
    return route.continue();
  });

  // Controls
  await page.route('**/api/controls', async (route: Route) => {
    if (route.request().method() === 'GET') return route.fulfill({ json: controls });
    return route.continue();
  });

  // Runners
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
      // Update competition runner count
      competitions[0].numRunners = runners.length;
      return route.fulfill({ status: 201, json: created });
    }
    return route.continue();
  });

  await page.route('**/api/runners/*', async (route: Route) => {
    if (route.request().method() === 'DELETE') return route.fulfill({ status: 204 });
    return route.continue();
  });

  // Cards
  await page.route('**/api/cards', async (route: Route) => {
    if (route.request().method() === 'GET') return route.fulfill({ json: cards });
    return route.continue();
  });

  await page.route('**/api/cards/read', async (route: Route) => {
    if (route.request().method() === 'POST') {
      const body = route.request().postDataJSON();
      const card = {
        id: cards.length + 1,
        cardNo: body.cardNo,
        punches: [
          { type: 0, time: 3650, controlId: 31 },
          { type: 0, time: 3900, controlId: 32 },
          { type: 0, time: 4150, controlId: 33 },
        ],
      };
      cards.push(card);

      // Update runner status to finished
      const runner = runners.find(r => r.cardNo === body.cardNo);
      if (runner) {
        runner.status = 1;
        runner.finishTime = 5400;
        runner.runningTime = 1800;
        // Build results
        results = runners
          .filter(r => r.status === 1)
          .sort((a, b) => a.runningTime - b.runningTime)
          .map((r, i) => ({
            runnerId: r.id,
            name: r.name,
            club: r.club,
            class: r.class,
            time: formatSec(r.runningTime),
            status: 1,
            place: i + 1,
          }));
      }
      return route.fulfill({ status: 200, json: card });
    }
    return route.continue();
  });

  // Punches
  await page.route('**/api/punches', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  // Lists
  await page.route('**/api/lists', async (route: Route) => {
    return route.fulfill({
      json: [
        { type: 'startlist', name: 'Startlista' },
        { type: 'resultlist', name: 'Resultatlista' },
      ],
    });
  });

  await page.route('**/api/lists/*', async (route: Route) => {
    return route.fulfill({ json: { type: 'resultlist', html: '<p>Resultatlista genererad</p>' } });
  });

  // Results
  await page.route('**/api/results', async (route: Route) => {
    return route.fulfill({ json: results });
  });

  // Teams
  await page.route('**/api/teams', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  // Automations
  await page.route('**/api/automations', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  // Speaker
  await page.route('**/api/speaker/**', async (route: Route) => {
    if (route.request().url().includes('config')) {
      return route.fulfill({ json: { classes: [], windowSeconds: 300 } });
    }
    return route.fulfill({ json: { events: [], serverTime: '2026-03-01T10:00:00' } });
  });

  // Import / Export
  await page.route('**/api/import/**', async (route: Route) => {
    return route.fulfill({ json: { success: true, message: 'Imported', imported: 5 } });
  });
  await page.route('**/api/export/**', async (route: Route) => {
    return route.fulfill({ json: { data: 'export-data', format: 'csv' } });
  });
}

function formatSec(s: number): string {
  const m = Math.floor(s / 60);
  const sec = s % 60;
  return `${m}:${String(sec).padStart(2, '0')}`;
}

// --------------- tests ---------------

test.describe('Complete competition flow', () => {
  test.beforeEach(async ({ page }) => {
    resetState();
    // Start with no competition so we can create one
    competitions = [];
    await mockApi(page);
  });

  test('create competition → add class → register runner → read card → view results', async ({ page }) => {
    // 1. Go to competition page - no competition loaded yet
    await page.goto('/competition');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });
    await expect(page.getByText('Ingen tävling laddad')).toBeVisible();

    // 2. Create a new competition
    await page.getByRole('button', { name: 'Ny tävling' }).click();
    await page.locator('#new-comp-name').fill('Vår-OL 2026');
    await page.locator('#new-comp-date').fill('2026-06-15');
    await page.locator('#new-comp-zero-time').fill('09:00:00');
    await page.getByRole('button', { name: 'Skapa tävling' }).click();

    // Competition is created — page should now show competition data
    await expect(page.getByText('Vår-OL 2026')).toBeVisible({ timeout: 5_000 });

    // 3. Navigate to classes page - pre-populated class H21 should show
    const nav = page.locator('nav[aria-label="Huvudnavigering"]');
    await nav.getByRole('link', { name: 'Klasser' }).click();
    await page.waitForURL('**/classes');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });
    await expect(page.getByText('H21')).toBeVisible();

    // 4. Navigate to runners and add a runner
    await nav.getByRole('link', { name: 'Deltagare' }).click();
    await page.waitForURL('**/runners');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    await page.getByRole('button', { name: 'Ny deltagare' }).click();
    await page.locator('#runner-name').fill('Lisa Andersson');
    await page.locator('#runner-class').selectOption({ value: '1' });
    await page.locator('#runner-club').selectOption({ value: '1' });
    await page.locator('#runner-card').fill('54321');
    await page.locator('#runner-bib').fill('201');
    await page.getByRole('button', { name: 'Spara' }).click();

    // Runner should appear in the table
    await expect(page.getByText('Lisa Andersson')).toBeVisible({ timeout: 5_000 });

    // 5. Navigate to SI page and simulate card read
    await nav.getByRole('link', { name: 'SI' }).click();
    await page.waitForURL('**/si');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    await page.getByLabel('Bricknummer att läsa').fill('54321');
    await page.getByRole('button', { name: 'Läs bricka' }).click();

    // Card should appear in table
    await expect(page.getByText('54321')).toBeVisible({ timeout: 5_000 });

    // 6. Navigate to lists and view results
    await nav.getByRole('link', { name: 'Listor' }).click();
    await page.waitForURL('**/lists');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    // Select result list type
    await page.locator('#list-type').selectOption('resultlist');

    // The results table should show our runner
    await expect(page.getByText('Lisa Andersson')).toBeVisible({ timeout: 5_000 });
    await expect(page.getByText('OK Linné')).toBeVisible();
    await expect(page.getByText('30:00')).toBeVisible();
  });

  test('edit existing competition', async ({ page }) => {
    // Start with an existing competition
    competitions = [
      { id: 1, name: 'Original', date: '2026-01-01', zeroTime: '08:00:00', numRunners: 5, numClasses: 2, numCourses: 1, numCards: 3 },
    ];

    await page.goto('/competition');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });
    await expect(page.getByText('Original')).toBeVisible();

    // Click edit
    await page.getByRole('button', { name: 'Redigera' }).click();
    await page.locator('#comp-name').clear();
    await page.locator('#comp-name').fill('Uppdaterad tävling');
    await page.getByRole('button', { name: 'Spara' }).click();

    await expect(page.getByText('Uppdaterad tävling')).toBeVisible({ timeout: 5_000 });
  });

  test('add course and verify it appears', async ({ page }) => {
    competitions = [
      { id: 1, name: 'Test', date: '2026-01-01', zeroTime: '10:00:00', numRunners: 0, numClasses: 0, numCourses: 0, numCards: 0 },
    ];
    courses = [];

    await page.goto('/courses');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    // Table should be empty
    await expect(page.getByText('Inga banor registrerade')).toBeVisible();

    // Create a course
    await page.getByRole('button', { name: 'Ny bana' }).click();
    await page.locator('#course-name').fill('Bana Lång');
    await page.locator('#course-length').fill('6500');
    await page.getByRole('button', { name: 'Spara' }).click();

    // Course should appear
    await expect(page.getByText('Bana Lång')).toBeVisible({ timeout: 5_000 });
  });

  test('competition stats show correct counts', async ({ page }) => {
    competitions = [
      { id: 1, name: 'Stats Test', date: '2026-01-01', zeroTime: '10:00:00', numRunners: 25, numClasses: 3, numCourses: 2, numCards: 20 },
    ];

    await page.goto('/competition');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });
    const stats = page.locator('.comp-stats');
    await expect(stats.getByText('25')).toBeVisible();
    await expect(stats.getByText('Deltagare')).toBeVisible();
    await expect(stats.getByText('Klasser')).toBeVisible();
    await expect(stats.getByText('Banor')).toBeVisible();
    await expect(stats.getByText('Brickor')).toBeVisible();
  });

  test('full flow with multiple runners', async ({ page }) => {
    // Start with pre-existing competition and class
    competitions = [
      { id: 1, name: 'Multi-test', date: '2026-03-01', zeroTime: '10:00:00', numRunners: 0, numClasses: 1, numCourses: 1, numCards: 0 },
    ];

    await page.goto('/runners');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    // Add first runner
    await page.getByRole('button', { name: 'Ny deltagare' }).click();
    await page.locator('#runner-name').fill('Adam Nilsson');
    await page.locator('#runner-class').selectOption({ value: '1' });
    await page.locator('#runner-club').selectOption({ value: '1' });
    await page.locator('#runner-card').fill('11111');
    await page.locator('#runner-bib').fill('1');
    await page.getByRole('button', { name: 'Spara' }).click();
    await expect(page.getByText('Adam Nilsson')).toBeVisible({ timeout: 5_000 });

    // Add second runner
    await page.getByRole('button', { name: 'Ny deltagare' }).click();
    await page.locator('#runner-name').fill('Petra Berg');
    await page.locator('#runner-class').selectOption({ value: '1' });
    await page.locator('#runner-club').selectOption({ value: '1' });
    await page.locator('#runner-card').fill('22222');
    await page.locator('#runner-bib').fill('2');
    await page.getByRole('button', { name: 'Spara' }).click();
    await expect(page.getByText('Petra Berg')).toBeVisible({ timeout: 5_000 });

    // Read both cards via SI page
    const nav = page.locator('nav[aria-label="Huvudnavigering"]');
    await nav.getByRole('link', { name: 'SI' }).click();
    await page.waitForURL('**/si');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    await page.getByLabel('Bricknummer att läsa').fill('11111');
    await page.getByRole('button', { name: 'Läs bricka' }).click();
    await expect(page.getByText('11111')).toBeVisible({ timeout: 5_000 });

    await page.getByLabel('Bricknummer att läsa').fill('22222');
    await page.getByRole('button', { name: 'Läs bricka' }).click();
    await expect(page.getByText('22222')).toBeVisible({ timeout: 5_000 });

    // Go to lists and verify both runners in results
    await nav.getByRole('link', { name: 'Listor' }).click();
    await page.waitForURL('**/lists');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    await page.locator('#list-type').selectOption('resultlist');
    await expect(page.getByText('Adam Nilsson')).toBeVisible({ timeout: 5_000 });
    await expect(page.getByText('Petra Berg')).toBeVisible();
  });
});
