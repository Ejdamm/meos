export interface Course {
  id: number;
  name: string;
  length: number;
  numControls: number;
  controls: number[];
  startId?: number;
  finishId?: number;
}
