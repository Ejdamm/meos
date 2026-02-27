import type { Competition, Runner, Team, Class, Course, Control, Club, Card, Punch } from '../types';
import type { Automation } from '../api/automations';
import type { ListType, ResultEntry } from '../api/lists';
import type { SpeakerConfig, SpeakerMonitorData } from '../api/speaker';

export const fixtures = {
  competitions: [
    {
      id: 1,
      name: 'Test Competition',
      date: '2026-03-01',
      zeroTime: '10:00:00',
      numRunners: 25,
      numClasses: 3,
      numCourses: 2,
      numCards: 20,
    },
  ] satisfies Competition[],

  runners: [
    {
      id: 1,
      name: 'Anna Svensson',
      clubId: 1,
      club: 'OK Test',
      classId: 1,
      class: 'H21',
      cardNo: 12345,
      bib: '101',
      startTime: 3600,
      finishTime: 5400,
      runningTime: 1800,
      status: 1,
      birthYear: 2000,
      nationality: 'SWE',
      sex: 0,
    },
    {
      id: 2,
      name: 'Erik Johansson',
      clubId: 1,
      club: 'OK Test',
      classId: 2,
      class: 'D21',
      cardNo: 12346,
      bib: '102',
      startTime: 3660,
      finishTime: 5600,
      runningTime: 1940,
      status: 1,
      birthYear: 1998,
      nationality: 'SWE',
      sex: 1,
    },
  ] satisfies Runner[],

  teams: [
    {
      id: 1,
      name: 'Team Alpha',
      clubId: 1,
      club: 'OK Test',
      classId: 3,
      class: 'Stafett',
      bib: '1',
      startTime: 3600,
      finishTime: 7200,
      runningTime: 3600,
      status: 1,
      runners: [1, 2],
      legResults: [
        { leg: 1, runningTime: 1800, status: 1 },
        { leg: 2, runningTime: 1800, status: 1 },
      ],
    },
  ] satisfies Team[],

  classes: [
    { id: 1, name: 'H21', type: 'individual', numStages: 1, courseId: 1 },
    { id: 2, name: 'D21', type: 'individual', numStages: 1, courseId: 2 },
    { id: 3, name: 'Stafett', type: 'relay', numStages: 3, courseId: 1 },
  ] satisfies Class[],

  courses: [
    { id: 1, name: 'Bana 1', length: 5200, numControls: 12, controls: [31, 32, 33] },
    { id: 2, name: 'Bana 2', length: 3800, numControls: 8, controls: [31, 34] },
  ] satisfies Course[],

  controls: [
    { id: 31, name: 'Kontroll 31', status: 0, codes: [31] },
    { id: 32, name: 'Kontroll 32', status: 0, codes: [32] },
    { id: 33, name: 'Kontroll 33', status: 0, codes: [33] },
    { id: 34, name: 'Kontroll 34', status: 0, codes: [34] },
  ] satisfies Control[],

  clubs: [
    { id: 1, name: 'OK Test', country: 'SWE' },
    { id: 2, name: 'IFK Göteborg', country: 'SWE' },
  ] satisfies Club[],

  cards: [
    {
      id: 1,
      cardNo: 12345,
      punches: [
        { type: 0, time: 3650, controlId: 31 },
        { type: 0, time: 3900, controlId: 32 },
      ],
    },
  ] satisfies Card[],

  automations: [
    { id: 1, name: 'Auto Results', type: 'results', enabled: true },
    { id: 2, name: 'Auto Backup', type: 'backup', enabled: false },
  ] satisfies Automation[],

  lists: [
    { type: 'startlist', name: 'Startlista' },
    { type: 'resultlist', name: 'Resultatlista' },
  ] satisfies ListType[],

  results: [
    {
      runnerId: 1,
      name: 'Anna Svensson',
      club: 'OK Test',
      class: 'H21',
      time: '30:00',
      status: 1,
      place: 1,
    },
    {
      runnerId: 2,
      name: 'Erik Johansson',
      club: 'OK Test',
      class: 'D21',
      time: '32:20',
      status: 1,
      place: 2,
    },
  ] satisfies ResultEntry[],

  speakerConfig: { classes: [], windowSeconds: 300 } satisfies SpeakerConfig,

  speakerMonitor: { events: [], serverTime: '2026-03-01T10:00:00' } satisfies SpeakerMonitorData,

  punches: [
    { type: 0, time: 3650, controlId: 31 },
    { type: 0, time: 3900, controlId: 32 },
  ] satisfies Punch[],
};
