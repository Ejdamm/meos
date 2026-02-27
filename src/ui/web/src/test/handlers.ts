import { http, HttpResponse } from 'msw';
import type { Competition } from '../types';
import type { Runner } from '../types';
import type { Team } from '../types';
import type { Class } from '../types';
import type { Course } from '../types';
import type { Control } from '../types';
import type { Club } from '../types';
import type { Card } from '../types';
import type { Automation, AutomationStatus } from '../api/automations';
import type { ListType, ListData, ResultEntry } from '../api/lists';
import type { SpeakerConfig, SpeakerMonitorData } from '../api/speaker';
import type { ImportResult, ExportResult } from '../api/importExport';
import type { Punch } from '../types';
import { fixtures } from './fixtures';

export const handlers = [
  // Competitions
  http.get('/api/competitions', () => HttpResponse.json<Competition[]>(fixtures.competitions)),
  http.get('/api/competitions/:id', ({ params }) => {
    const c = fixtures.competitions.find((x) => x.id === Number(params['id']));
    return c ? HttpResponse.json<Competition>(c) : new HttpResponse(null, { status: 404 });
  }),
  http.post('/api/competitions', async ({ request }) => {
    const body = (await request.json()) as Partial<Competition>;
    const created: Competition = {
      id: 99,
      name: body.name ?? '',
      date: body.date ?? '',
      zeroTime: body.zeroTime ?? '',
      numRunners: 0,
      numClasses: 0,
      numCourses: 0,
      numCards: 0,
    };
    return HttpResponse.json<Competition>(created, { status: 201 });
  }),
  http.put('/api/competitions/:id', async ({ params, request }) => {
    const body = (await request.json()) as Partial<Competition>;
    const existing = fixtures.competitions.find((x) => x.id === Number(params['id']));
    if (!existing) return new HttpResponse(null, { status: 404 });
    return HttpResponse.json<Competition>({ ...existing, ...body });
  }),

  // Runners
  http.get('/api/runners', () => HttpResponse.json<Runner[]>(fixtures.runners)),
  http.get('/api/runners/:id', ({ params }) => {
    const r = fixtures.runners.find((x) => x.id === Number(params['id']));
    return r ? HttpResponse.json<Runner>(r) : new HttpResponse(null, { status: 404 });
  }),
  http.post('/api/runners', async ({ request }) => {
    const body = (await request.json()) as Partial<Runner>;
    return HttpResponse.json<Runner>({ ...fixtures.runners[0]!, id: 99, ...body }, { status: 201 });
  }),
  http.put('/api/runners/:id', async ({ params, request }) => {
    const body = (await request.json()) as Partial<Runner>;
    const existing = fixtures.runners.find((x) => x.id === Number(params['id']));
    if (!existing) return new HttpResponse(null, { status: 404 });
    return HttpResponse.json<Runner>({ ...existing, ...body });
  }),
  http.delete('/api/runners/:id', () => new HttpResponse(null, { status: 204 })),

  // Teams
  http.get('/api/teams', () => HttpResponse.json<Team[]>(fixtures.teams)),
  http.get('/api/teams/:id', ({ params }) => {
    const t = fixtures.teams.find((x) => x.id === Number(params['id']));
    return t ? HttpResponse.json<Team>(t) : new HttpResponse(null, { status: 404 });
  }),
  http.post('/api/teams', async ({ request }) => {
    const body = (await request.json()) as Partial<Team>;
    return HttpResponse.json<Team>({ ...fixtures.teams[0]!, id: 99, ...body }, { status: 201 });
  }),
  http.put('/api/teams/:id', async ({ params, request }) => {
    const body = (await request.json()) as Partial<Team>;
    const existing = fixtures.teams.find((x) => x.id === Number(params['id']));
    if (!existing) return new HttpResponse(null, { status: 404 });
    return HttpResponse.json<Team>({ ...existing, ...body });
  }),
  http.delete('/api/teams/:id', () => new HttpResponse(null, { status: 204 })),

  // Classes
  http.get('/api/classes', () => HttpResponse.json<Class[]>(fixtures.classes)),
  http.get('/api/classes/:id', ({ params }) => {
    const c = fixtures.classes.find((x) => x.id === Number(params['id']));
    return c ? HttpResponse.json<Class>(c) : new HttpResponse(null, { status: 404 });
  }),
  http.post('/api/classes', async ({ request }) => {
    const body = (await request.json()) as Partial<Class>;
    return HttpResponse.json<Class>({ ...fixtures.classes[0]!, id: 99, ...body }, { status: 201 });
  }),
  http.put('/api/classes/:id', async ({ params, request }) => {
    const body = (await request.json()) as Partial<Class>;
    const existing = fixtures.classes.find((x) => x.id === Number(params['id']));
    if (!existing) return new HttpResponse(null, { status: 404 });
    return HttpResponse.json<Class>({ ...existing, ...body });
  }),
  http.delete('/api/classes/:id', () => new HttpResponse(null, { status: 204 })),

  // Courses
  http.get('/api/courses', () => HttpResponse.json<Course[]>(fixtures.courses)),
  http.get('/api/courses/:id', ({ params }) => {
    const c = fixtures.courses.find((x) => x.id === Number(params['id']));
    return c ? HttpResponse.json<Course>(c) : new HttpResponse(null, { status: 404 });
  }),
  http.post('/api/courses', async ({ request }) => {
    const body = (await request.json()) as Partial<Course>;
    return HttpResponse.json<Course>(
      { ...fixtures.courses[0]!, id: 99, ...body },
      { status: 201 },
    );
  }),
  http.put('/api/courses/:id', async ({ params, request }) => {
    const body = (await request.json()) as Partial<Course>;
    const existing = fixtures.courses.find((x) => x.id === Number(params['id']));
    if (!existing) return new HttpResponse(null, { status: 404 });
    return HttpResponse.json<Course>({ ...existing, ...body });
  }),
  http.delete('/api/courses/:id', () => new HttpResponse(null, { status: 204 })),

  // Controls
  http.get('/api/controls', () => HttpResponse.json<Control[]>(fixtures.controls)),
  http.get('/api/controls/:id', ({ params }) => {
    const c = fixtures.controls.find((x) => x.id === Number(params['id']));
    return c ? HttpResponse.json<Control>(c) : new HttpResponse(null, { status: 404 });
  }),
  http.post('/api/controls', async ({ request }) => {
    const body = (await request.json()) as Partial<Control>;
    return HttpResponse.json<Control>(
      { ...fixtures.controls[0]!, id: 99, ...body },
      { status: 201 },
    );
  }),
  http.put('/api/controls/:id', async ({ params, request }) => {
    const body = (await request.json()) as Partial<Control>;
    const existing = fixtures.controls.find((x) => x.id === Number(params['id']));
    if (!existing) return new HttpResponse(null, { status: 404 });
    return HttpResponse.json<Control>({ ...existing, ...body });
  }),
  http.delete('/api/controls/:id', () => new HttpResponse(null, { status: 204 })),

  // Clubs
  http.get('/api/clubs', () => HttpResponse.json<Club[]>(fixtures.clubs)),
  http.get('/api/clubs/:id', ({ params }) => {
    const c = fixtures.clubs.find((x) => x.id === Number(params['id']));
    return c ? HttpResponse.json<Club>(c) : new HttpResponse(null, { status: 404 });
  }),
  http.post('/api/clubs', async ({ request }) => {
    const body = (await request.json()) as Partial<Club>;
    return HttpResponse.json<Club>({ ...fixtures.clubs[0]!, id: 99, ...body }, { status: 201 });
  }),
  http.put('/api/clubs/:id', async ({ params, request }) => {
    const body = (await request.json()) as Partial<Club>;
    const existing = fixtures.clubs.find((x) => x.id === Number(params['id']));
    if (!existing) return new HttpResponse(null, { status: 404 });
    return HttpResponse.json<Club>({ ...existing, ...body });
  }),
  http.delete('/api/clubs/:id', () => new HttpResponse(null, { status: 204 })),

  // Cards
  http.get('/api/cards', () => HttpResponse.json<Card[]>(fixtures.cards)),
  http.get('/api/cards/:id', ({ params }) => {
    const c = fixtures.cards.find((x) => x.id === Number(params['id']));
    return c ? HttpResponse.json<Card>(c) : new HttpResponse(null, { status: 404 });
  }),
  http.post('/api/cards/read', async ({ request }) => {
    const body = (await request.json()) as { cardNo: number };
    return HttpResponse.json<Card>({
      id: 10,
      cardNo: body.cardNo,
      punches: [{ type: 0, time: 1000, controlId: 31 }],
    });
  }),

  // Automations
  http.get('/api/automations', () => HttpResponse.json<Automation[]>(fixtures.automations)),
  http.get('/api/automations/:id', ({ params }) => {
    const a = fixtures.automations.find((x) => x.id === Number(params['id']));
    return a ? HttpResponse.json<Automation>(a) : new HttpResponse(null, { status: 404 });
  }),
  http.get('/api/automations/:id/status', ({ params }) => {
    return HttpResponse.json<AutomationStatus>({
      id: Number(params['id']),
      running: false,
    });
  }),
  http.post('/api/automations', async ({ request }) => {
    const body = (await request.json()) as Partial<Automation>;
    return HttpResponse.json<Automation>(
      { id: 99, name: '', type: '', enabled: false, ...body },
      { status: 201 },
    );
  }),
  http.put('/api/automations/:id', async ({ params, request }) => {
    const body = (await request.json()) as Partial<Automation>;
    const existing = fixtures.automations.find((x) => x.id === Number(params['id']));
    if (!existing) return new HttpResponse(null, { status: 404 });
    return HttpResponse.json<Automation>({ ...existing, ...body });
  }),
  http.delete('/api/automations/:id', () => new HttpResponse(null, { status: 204 })),

  // Lists
  http.get('/api/lists', () => HttpResponse.json<ListType[]>(fixtures.lists)),
  http.get('/api/lists/:type', ({ params }) => {
    return HttpResponse.json<ListData>({
      type: params['type'] as string,
      html: '<p>Test list</p>',
    });
  }),

  // Results
  http.get('/api/results', () => HttpResponse.json<ResultEntry[]>(fixtures.results)),

  // Speaker
  http.get('/api/speaker/config', () => HttpResponse.json<SpeakerConfig>(fixtures.speakerConfig)),
  http.put('/api/speaker/config', async ({ request }) => {
    const body = (await request.json()) as SpeakerConfig;
    return HttpResponse.json<SpeakerConfig>(body);
  }),
  http.get('/api/speaker/monitor', () =>
    HttpResponse.json<SpeakerMonitorData>(fixtures.speakerMonitor),
  ),

  // Punches
  http.get('/api/punches', () => HttpResponse.json<Punch[]>(fixtures.punches)),

  // Import/Export
  http.post('/api/import/csv', () =>
    HttpResponse.json<ImportResult>({ success: true, message: 'Imported', imported: 5 }),
  ),
  http.post('/api/import/iof', () =>
    HttpResponse.json<ImportResult>({ success: true, message: 'Imported', imported: 10 }),
  ),
  http.get('/api/export/csv', () =>
    HttpResponse.json<ExportResult>({ data: 'csv-data', format: 'csv' }),
  ),
  http.get('/api/export/iof', () =>
    HttpResponse.json<ExportResult>({ data: '<xml/>', format: 'iof' }),
  ),
];
