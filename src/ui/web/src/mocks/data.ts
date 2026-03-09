import type { 
  Competition, 
  Class, 
  Course, 
  Control, 
  Club, 
  Runner, 
  Team 
} from '../api/types';

export const mockCompetitions: Competition[] = [
  {
    id: 1,
    name: 'Spring Orienteering Festival',
    date: '2026-04-12',
    organizer: 'Forest Runners SC',
    homepage: 'https://springfestival.example.com',
    zeroTime: '10:00:00',
  },
];

export const mockControls: Control[] = [
  { id: 1, name: 'S1', code: '31' },
  { id: 2, name: 'S2', code: '32' },
  { id: 3, name: 'S3', code: '33' },
  { id: 4, name: 'S4', code: '34' },
  { id: 5, name: 'S5', code: '35' },
  { id: 6, name: 'F1', code: '99' },
];

export const mockCourses: Course[] = [
  { id: 1, name: 'Long A', length: 7200, climb: 240, controls: [31, 32, 33, 34, 35, 99] },
  { id: 2, name: 'Short B', length: 3500, climb: 110, controls: [31, 33, 35, 99] },
  { id: 3, name: 'Easy C', length: 2100, climb: 45, controls: [32, 34, 99] },
];

export const mockClasses: Class[] = [
  { id: 1, name: 'H21E', sortOrder: 1, courseId: 1, startMethod: 0 },
  { id: 2, name: 'D21E', sortOrder: 2, courseId: 1, startMethod: 0 },
  { id: 3, name: 'H45', sortOrder: 3, courseId: 2, startMethod: 0 },
  { id: 4, name: 'D45', sortOrder: 4, courseId: 2, startMethod: 0 },
  { id: 5, name: 'H12', sortOrder: 5, courseId: 3, startMethod: 0 },
  { id: 6, name: 'D12', sortOrder: 6, courseId: 3, startMethod: 0 },
];

export const mockClubs: Club[] = [
  { id: 1, name: 'OK Skogsfalken', nationality: 'SWE' },
  { id: 2, name: 'Paimion Rasti', nationality: 'FIN' },
  { id: 3, name: 'Nydalens SK', nationality: 'NOR' },
  { id: 4, name: 'IFK Göteborg', nationality: 'SWE' },
  { id: 5, name: 'Koovee', nationality: 'FIN' },
];

export const mockRunners: Runner[] = [
  { 
    id: 1, 
    name: 'Anders Andersson', 
    bib: '101', 
    clubId: 1, 
    clubName: 'OK Skogsfalken', 
    classId: 1, 
    className: 'H21E', 
    status: 1, 
    startTime: '10:00:00', 
    cardNumber: 1234567 
  },
  { 
    id: 2, 
    name: 'Maria Mäkinen', 
    bib: '102', 
    clubId: 2, 
    clubName: 'Paimion Rasti', 
    classId: 2, 
    className: 'D21E', 
    status: 1, 
    startTime: '10:02:00', 
    cardNumber: 2345678 
  },
  { 
    id: 3, 
    name: 'Bjørn Bjørnsen', 
    bib: '103', 
    clubId: 3, 
    clubName: 'Nydalens SK', 
    classId: 1, 
    className: 'H21E', 
    status: 1, 
    startTime: '10:04:00', 
    cardNumber: 3456789 
  },
  { 
    id: 4, 
    name: 'Sven Svensson', 
    bib: '104', 
    clubId: 4, 
    clubName: 'IFK Göteborg', 
    classId: 3, 
    className: 'H45', 
    status: 1, 
    startTime: '10:10:00', 
    cardNumber: 4567890 
  },
  { 
    id: 5, 
    name: 'Liisa Laitinen', 
    bib: '105', 
    clubId: 5, 
    clubName: 'Koovee', 
    classId: 4, 
    className: 'D45', 
    status: 1, 
    startTime: '10:12:00', 
    cardNumber: 5678901 
  },
];

export const mockTeams: Team[] = [
  { 
    id: 1, 
    name: 'Team Skogsfalken 1', 
    clubId: 1, 
    classId: 1, 
    status: 1, 
    members: [1] 
  },
  { 
    id: 2, 
    name: 'Team Paimion 1', 
    clubId: 2, 
    classId: 2, 
    status: 1, 
    members: [2] 
  },
];
