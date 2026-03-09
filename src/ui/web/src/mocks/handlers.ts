import { http, HttpResponse } from 'msw';
import { 
  mockCompetitions, 
  mockClasses, 
  mockCourses, 
  mockControls, 
  mockClubs, 
  mockRunners, 
  mockTeams 
} from './data';
import type { 
  Competition, 
  Class, 
  Course, 
  Control, 
  Club, 
  Runner, 
  Team,
  Result,
  StartListEntry
} from '../api/types';

// In-memory state
let competitions = [...mockCompetitions];
let classes = [...mockClasses];
let courses = [...mockCourses];
let controls = [...mockControls];
let clubs = [...mockClubs];
let runners = [...mockRunners];
let teams = [...mockTeams];

const API_BASE = '/api/v1';

export const handlers = [
  // Competitions
  http.get(`${API_BASE}/competitions`, () => {
    return HttpResponse.json(competitions);
  }),
  http.get(`${API_BASE}/competitions/:id`, ({ params }) => {
    const id = Number(params.id);
    const competition = competitions.find(c => c.id === id);
    return competition ? HttpResponse.json(competition) : new HttpResponse(null, { status: 404 });
  }),
  http.put(`${API_BASE}/competitions/:id`, async ({ params, request }) => {
    const id = Number(params.id);
    const updated = await request.json() as Competition;
    competitions = competitions.map(c => c.id === id ? { ...updated, id } : c);
    return HttpResponse.json({ ...updated, id });
  }),

  // Clubs
  http.get(`${API_BASE}/clubs`, () => HttpResponse.json(clubs)),
  http.get(`${API_BASE}/clubs/:id`, ({ params }) => {
    const club = clubs.find(c => c.id === Number(params.id));
    return club ? HttpResponse.json(club) : new HttpResponse(null, { status: 404 });
  }),
  http.post(`${API_BASE}/clubs`, async ({ request }) => {
    const club = await request.json() as Club;
    const newClub = { ...club, id: clubs.length + 1 };
    clubs.push(newClub);
    return HttpResponse.json(newClub, { status: 201 });
  }),
  http.put(`${API_BASE}/clubs/:id`, async ({ params, request }) => {
    const id = Number(params.id);
    const updated = await request.json() as Club;
    clubs = clubs.map(c => c.id === id ? { ...updated, id } : c);
    return HttpResponse.json({ ...updated, id });
  }),
  http.delete(`${API_BASE}/clubs/:id`, ({ params }) => {
    const id = Number(params.id);
    clubs = clubs.filter(c => c.id !== id);
    return new HttpResponse(null, { status: 204 });
  }),

  // Controls
  http.get(`${API_BASE}/controls`, () => HttpResponse.json(controls)),
  http.get(`${API_BASE}/controls/:id`, ({ params }) => {
    const control = controls.find(c => c.id === Number(params.id));
    return control ? HttpResponse.json(control) : new HttpResponse(null, { status: 404 });
  }),
  http.post(`${API_BASE}/controls`, async ({ request }) => {
    const control = await request.json() as Control;
    const newControl = { ...control, id: controls.length + 1 };
    controls.push(newControl);
    return HttpResponse.json(newControl, { status: 201 });
  }),
  http.put(`${API_BASE}/controls/:id`, async ({ params, request }) => {
    const id = Number(params.id);
    const updated = await request.json() as Control;
    controls = controls.map(c => c.id === id ? { ...updated, id } : c);
    return HttpResponse.json({ ...updated, id });
  }),
  http.delete(`${API_BASE}/controls/:id`, ({ params }) => {
    const id = Number(params.id);
    controls = controls.filter(c => c.id !== id);
    return new HttpResponse(null, { status: 204 });
  }),

  // Courses
  http.get(`${API_BASE}/courses`, () => HttpResponse.json(courses)),
  http.get(`${API_BASE}/courses/:id`, ({ params }) => {
    const course = courses.find(c => c.id === Number(params.id));
    return course ? HttpResponse.json(course) : new HttpResponse(null, { status: 404 });
  }),
  http.post(`${API_BASE}/courses`, async ({ request }) => {
    const course = await request.json() as Course;
    const newCourse = { ...course, id: courses.length + 1 };
    courses.push(newCourse);
    return HttpResponse.json(newCourse, { status: 201 });
  }),
  http.put(`${API_BASE}/courses/:id`, async ({ params, request }) => {
    const id = Number(params.id);
    const updated = await request.json() as Course;
    courses = courses.map(c => c.id === id ? { ...updated, id } : c);
    return HttpResponse.json({ ...updated, id });
  }),
  http.delete(`${API_BASE}/courses/:id`, ({ params }) => {
    const id = Number(params.id);
    courses = courses.filter(c => c.id !== id);
    return new HttpResponse(null, { status: 204 });
  }),

  // Classes
  http.get(`${API_BASE}/classes`, () => HttpResponse.json(classes)),
  http.get(`${API_BASE}/classes/:id`, ({ params }) => {
    const cls = classes.find(c => c.id === Number(params.id));
    return cls ? HttpResponse.json(cls) : new HttpResponse(null, { status: 404 });
  }),
  http.post(`${API_BASE}/classes`, async ({ request }) => {
    const cls = await request.json() as Class;
    const newClass = { ...cls, id: classes.length + 1 };
    classes.push(newClass);
    return HttpResponse.json(newClass, { status: 201 });
  }),
  http.put(`${API_BASE}/classes/:id`, async ({ params, request }) => {
    const id = Number(params.id);
    const updated = await request.json() as Class;
    classes = classes.map(c => c.id === id ? { ...updated, id } : c);
    return HttpResponse.json({ ...updated, id });
  }),
  http.delete(`${API_BASE}/classes/:id`, ({ params }) => {
    const id = Number(params.id);
    classes = classes.filter(c => c.id !== id);
    return new HttpResponse(null, { status: 204 });
  }),

  // Runners
  http.get(`${API_BASE}/runners`, () => HttpResponse.json(runners)),
  http.get(`${API_BASE}/runners/:id`, ({ params }) => {
    const runner = runners.find(r => r.id === Number(params.id));
    return runner ? HttpResponse.json(runner) : new HttpResponse(null, { status: 404 });
  }),
  http.post(`${API_BASE}/runners`, async ({ request }) => {
    const runner = await request.json() as Runner;
    const newRunner = { ...runner, id: runners.length + 1 };
    runners.push(newRunner);
    return HttpResponse.json(newRunner, { status: 201 });
  }),
  http.put(`${API_BASE}/runners/:id`, async ({ params, request }) => {
    const id = Number(params.id);
    const updated = await request.json() as Runner;
    runners = runners.map(r => r.id === id ? { ...updated, id } : r);
    return HttpResponse.json({ ...updated, id });
  }),
  http.delete(`${API_BASE}/runners/:id`, ({ params }) => {
    const id = Number(params.id);
    runners = runners.filter(r => r.id !== id);
    return new HttpResponse(null, { status: 204 });
  }),

  // Teams
  http.get(`${API_BASE}/teams`, () => HttpResponse.json(teams)),
  http.get(`${API_BASE}/teams/:id`, ({ params }) => {
    const team = teams.find(t => t.id === Number(params.id));
    return team ? HttpResponse.json(team) : new HttpResponse(null, { status: 404 });
  }),
  http.post(`${API_BASE}/teams`, async ({ request }) => {
    const team = await request.json() as Team;
    const newTeam = { ...team, id: teams.length + 1 };
    teams.push(newTeam);
    return HttpResponse.json(newTeam, { status: 201 });
  }),
  http.put(`${API_BASE}/teams/:id`, async ({ params, request }) => {
    const id = Number(params.id);
    const updated = await request.json() as Team;
    teams = teams.map(t => t.id === id ? { ...updated, id } : t);
    return HttpResponse.json({ ...updated, id });
  }),
  http.delete(`${API_BASE}/teams/:id`, ({ params }) => {
    const id = Number(params.id);
    teams = teams.filter(t => t.id !== id);
    return new HttpResponse(null, { status: 204 });
  }),

  // Results
  http.get(`${API_BASE}/results`, () => {
    const results: Result[] = runners.map(r => ({
      runnerId: r.id,
      name: r.name,
      clubName: r.clubName,
      className: r.className,
      status: r.status,
      startTime: r.startTime,
      runningTime: '00:45:30',
      place: 1,
      timeAfter: '00:00:00',
      splits: []
    }));
    return HttpResponse.json(results);
  }),

  // Startlist
  http.get(`${API_BASE}/startlist`, () => {
    const startlist: StartListEntry[] = runners.map(r => ({
      runnerId: r.id,
      name: r.name,
      clubName: r.clubName,
      className: r.className,
      startTime: r.startTime || '00:00:00',
      cardNumber: r.cardNumber
    }));
    return HttpResponse.json(startlist);
  }),
];
