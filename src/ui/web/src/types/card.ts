export interface Punch {
  type: number;
  time: number;
  controlId: number;
}

export interface Card {
  id: number;
  cardNo: number;
  punches: Punch[];
}
