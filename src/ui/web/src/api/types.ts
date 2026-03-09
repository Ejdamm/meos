export const RunnerStatus = {
  Unknown: 0,
  OK: 1,
  NoTiming: 2,
  MP: 3,
  DNF: 4,
  DQ: 5,
  MAX: 6,
  OutOfCompetition: 15,
  DNS: 20,
  CANCEL: 21,
  NotCompeting: 99,
} as const;

export type RunnerStatus = typeof RunnerStatus[keyof typeof RunnerStatus];

export interface Competition {
  id: number;
  name: string;
  date: string;
  organizer: string;
  homepage?: string;
  zeroTime?: string;
}

export const StartMethod = {
  Time: 0,
  Change: 1,
  Drawn: 2,
  Pursuit: 3,
} as const;

export type StartMethod = typeof StartMethod[keyof typeof StartMethod];

export interface Class {
  id: number;
  name: string;
  sortOrder?: number;
  courseId?: number;
  startMethod?: StartMethod;
}

export interface Course {
  id: number;
  name: string;
  length?: number;
  climb?: number;
  controls: number[]; // Array of control codes/ids
}

export interface Control {
  id: number;
  name: string;
  code: string;
}

export interface Club {
  id: number;
  name: string;
  nationality?: string;
}

export interface Runner {
  id: number;
  name: string;
  bib?: string;
  nationality?: string;
  clubId?: number;
  clubName?: string;
  classId?: number;
  className?: string;
  status: RunnerStatus;
  startTime?: string;
  finishTime?: string;
  runningTime?: string;
  cardNumber?: number;
}

export interface Team {
  id: number;
  name: string;
  clubId?: number;
  classId?: number;
  status: RunnerStatus;
  members: number[]; // Array of runner IDs
}

export interface Split {
  controlId: number;
  controlName: string;
  time?: string;
  place?: number;
  timeAfter?: string;
}

export interface Result {
  runnerId: number;
  name: string;
  clubName?: string;
  className?: string;
  status: RunnerStatus;
  startTime?: string;
  runningTime?: string;
  place?: number;
  timeAfter?: string;
  splits: Split[];
}

export interface StartListEntry {
  runnerId: number;
  name: string;
  clubName?: string;
  className?: string;
  startTime: string;
  cardNumber?: number;
}
