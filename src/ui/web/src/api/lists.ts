import apiClient from './client';

export interface ListType {
  type: string;
  name: string;
}

export interface ListData {
  type: string;
  html: string;
}

export interface ResultEntry {
  runnerId: number;
  name: string;
  club: string;
  class: string;
  time: string;
  status: number;
  place: number;
}

export async function getLists(): Promise<ListType[]> {
  const { data } = await apiClient.get<ListType[]>('/lists');
  return data;
}

export async function getList(type: string): Promise<ListData> {
  const { data } = await apiClient.get<ListData>(`/lists/${type}`);
  return data;
}

export async function getResults(classId?: number): Promise<ResultEntry[]> {
  const params = classId !== undefined ? { classId } : undefined;
  const { data } = await apiClient.get<ResultEntry[]>('/results', { params });
  return data;
}
