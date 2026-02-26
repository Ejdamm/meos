export interface LegResult {
  leg: number;
  runningTime: number;
  status: number;
}

export interface Team {
  id: number;
  name: string;
  clubId: number;
  club: string;
  classId: number;
  class: string;
  bib: string;
  startTime: number;
  finishTime: number;
  runningTime: number;
  status: number;
  runners: number[];
  legResults: LegResult[];
}
