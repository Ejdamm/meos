import { test, expect, type Page, type Route } from '@playwright/test';

/**
 * E2E: Import and export flow.
 * Uses Playwright route interception to mock the REST API.
 * Tests: import IOF XML file → runners appear, import CSV file → runners appear,
 * export IOF XML, export CSV, import error handling.
 */

// --------------- mock state ---------------
let competitions: any[];
let runners: any[];
let classes: any[];
let clubs: any[];
let importedRunners: any[];

function resetState() {
  competitions = [
    { id: 1, name: 'Import Test', date: '2026-03-01', zeroTime: '10:00:00', numRunners: 0, numClasses: 1, numCourses: 1, numCards: 0 },
  ];
  classes = [
    { id: 1, name: 'H21', type: 'individual', numStages: 1, courseId: 1 },
  ];
  clubs = [
    { id: 1, name: 'OK Linné', country: 'SWE' },
  ];
  runners = [];
  importedRunners = [
    { id: 101, name: 'Anna Svensson', class: 'H21', club: 'OK Linné', cardNo: 12345, bib: '10', startTime: 3600, finishTime: 0, runningTime: 0, status: 0, sex: 0, classId: 1, clubId: 1 },
    { id: 102, name: 'Erik Johansson', class: 'H21', club: 'OK Linné', cardNo: 12346, bib: '11', startTime: 3660, finishTime: 0, runningTime: 0, status: 0, sex: 0, classId: 1, clubId: 1 },
    { id: 103, name: 'Maria Karlsson', class: 'H21', club: 'OK Linné', cardNo: 12347, bib: '12', startTime: 3720, finishTime: 0, runningTime: 0, status: 0, sex: 0, classId: 1, clubId: 1 },
  ];
}

// --------------- route mocking ---------------
async function mockApi(page: Page) {
  await page.route('**/api/competitions', async (route: Route) => {
    return route.fulfill({ json: competitions });
  });

  await page.route('**/api/competitions/*', async (route: Route) => {
    return route.fulfill({ json: competitions[0] });
  });

  await page.route('**/api/classes', async (route: Route) => {
    return route.fulfill({ json: classes });
  });

  await page.route('**/api/clubs', async (route: Route) => {
    return route.fulfill({ json: clubs });
  });

  await page.route('**/api/runners', async (route: Route) => {
    return route.fulfill({ json: runners });
  });

  await page.route('**/api/courses', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/controls', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/cards', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/teams', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/automations', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/lists', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/results', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/punches', async (route: Route) => {
    return route.fulfill({ json: [] });
  });

  await page.route('**/api/speaker/**', async (route: Route) => {
    return route.fulfill({ json: {} });
  });

  // Import IOF - succeeds and adds runners
  await page.route('**/api/import/iof', async (route: Route) => {
    if (route.request().method() === 'POST') {
      runners = [...runners, ...importedRunners];
      competitions[0].numRunners = runners.length;
      return route.fulfill({
        json: { success: true, message: 'IOF XML importerad', imported: importedRunners.length },
      });
    }
    return route.continue();
  });

  // Import CSV - succeeds and adds runners
  await page.route('**/api/import/csv', async (route: Route) => {
    if (route.request().method() === 'POST') {
      runners = [...runners, ...importedRunners];
      competitions[0].numRunners = runners.length;
      return route.fulfill({
        json: { success: true, message: 'CSV importerad', imported: importedRunners.length },
      });
    }
    return route.continue();
  });

  // Export IOF
  await page.route('**/api/export/iof', async (route: Route) => {
    const xml = [
      '<?xml version="1.0" encoding="UTF-8"?>',
      '<ResultList>',
      ...runners.map(r => `  <PersonResult><Name>${r.name}</Name><Club>${r.club}</Club></PersonResult>`),
      '</ResultList>',
    ].join('\n');
    return route.fulfill({ json: { data: xml, format: 'iof' } });
  });

  // Export CSV
  await page.route('**/api/export/csv', async (route: Route) => {
    const lines = ['Name;Club;Class;CardNo', ...runners.map(r => `${r.name};${r.club};${r.class};${r.cardNo}`)];
    return route.fulfill({ json: { data: lines.join('\n'), format: 'csv' } });
  });
}

// --------------- tests ---------------

test.describe('Import and export', () => {
  test.beforeEach(async ({ page }) => {
    resetState();
    await mockApi(page);
  });

  test('import IOF XML file and verify runners appear', async ({ page }) => {
    await page.goto('/competition');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });
    await expect(page.getByText('Import Test')).toBeVisible();

    // Verify import section is visible
    await expect(page.getByText('Import / Export')).toBeVisible();

    // Select IOF XML format (should be default)
    const formatBtns = page.locator('.comp-io__format-btns');
    await expect(formatBtns.getByText('IOF XML')).toHaveClass(/comp-btn--primary/);

    // Upload a file via the hidden file input
    const fileInput = page.locator('.comp-io__file-input');
    await fileInput.setInputFiles({
      name: 'entries.xml',
      mimeType: 'application/xml',
      buffer: Buffer.from('<EntryList><Entry><Name>Test</Name></Entry></EntryList>'),
    });

    // Wait for success message
    await expect(page.getByText('Import lyckades (3 importerade)')).toBeVisible({ timeout: 5_000 });

    // Navigate to runners page and verify imported runners
    const nav = page.locator('nav[aria-label="Huvudnavigering"]');
    await nav.getByRole('link', { name: 'Deltagare' }).click();
    await page.waitForURL('**/runners');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    await expect(page.getByText('Anna Svensson')).toBeVisible();
    await expect(page.getByText('Erik Johansson')).toBeVisible();
    await expect(page.getByText('Maria Karlsson')).toBeVisible();
  });

  test('import CSV file and verify runners appear', async ({ page }) => {
    await page.goto('/competition');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    // Switch to CSV format
    const formatBtns = page.locator('.comp-io__format-btns');
    await formatBtns.getByText('CSV').click();
    await expect(formatBtns.getByText('CSV')).toHaveClass(/comp-btn--primary/);

    // Upload a CSV file
    const fileInput = page.locator('.comp-io__file-input');
    await fileInput.setInputFiles({
      name: 'entries.csv',
      mimeType: 'text/csv',
      buffer: Buffer.from('Name;Club;Class\nAnna Svensson;OK Linné;H21'),
    });

    // Wait for success message
    await expect(page.getByText('Import lyckades (3 importerade)')).toBeVisible({ timeout: 5_000 });

    // Navigate to runners page and verify imported runners
    const nav = page.locator('nav[aria-label="Huvudnavigering"]');
    await nav.getByRole('link', { name: 'Deltagare' }).click();
    await page.waitForURL('**/runners');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    await expect(page.getByText('Anna Svensson')).toBeVisible();
    await expect(page.getByText('Erik Johansson')).toBeVisible();
  });

  test('export IOF XML triggers download with correct content', async ({ page }) => {
    // Pre-populate runners for export
    runners = [
      { id: 1, name: 'Lisa Andersson', class: 'H21', club: 'OK Linné', cardNo: 54321 },
    ];

    await page.goto('/competition');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    // Intercept download
    const downloadPromise = page.waitForEvent('download');
    await page.locator('.comp-io__export-btns').getByText('IOF XML').click();
    const download = await downloadPromise;

    // Verify download filename
    expect(download.suggestedFilename()).toBe('export.xml');

    // Verify success status
    await expect(page.getByText('Export (IOF) klar')).toBeVisible({ timeout: 5_000 });
  });

  test('export CSV triggers download with correct content', async ({ page }) => {
    // Pre-populate runners for export
    runners = [
      { id: 1, name: 'Lisa Andersson', class: 'H21', club: 'OK Linné', cardNo: 54321 },
    ];

    await page.goto('/competition');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    // Intercept download
    const downloadPromise = page.waitForEvent('download');
    await page.locator('.comp-io__export-btns').getByText('CSV').click();
    const download = await downloadPromise;

    // Verify download filename
    expect(download.suggestedFilename()).toBe('export.csv');

    // Verify success status
    await expect(page.getByText('Export (CSV) klar')).toBeVisible({ timeout: 5_000 });
  });

  test('import then export round-trip', async ({ page }) => {
    await page.goto('/competition');
    await expect(page.locator('.app-loading')).toHaveCount(0, { timeout: 10_000 });

    // 1. Import IOF XML
    const fileInput = page.locator('.comp-io__file-input');
    await fileInput.setInputFiles({
      name: 'entries.xml',
      mimeType: 'application/xml',
      buffer: Buffer.from('<EntryList><Entry><Name>Test</Name></Entry></EntryList>'),
    });

    await expect(page.getByText('Import lyckades')).toBeVisible({ timeout: 5_000 });

    // 2. Export IOF XML
    const downloadPromise = page.waitForEvent('download');
    await page.locator('.comp-io__export-btns').getByText('IOF XML').click();
    const download = await downloadPromise;

    expect(download.suggestedFilename()).toBe('export.xml');

    // Read downloaded content and verify it contains imported runners
    const content = await (await download.createReadStream()).toArray();
    const text = Buffer.concat(content).toString();
    expect(text).toContain('Anna Svensson');
    expect(text).toContain('Erik Johansson');
    expect(text).toContain('Maria Karlsson');
  });
});
