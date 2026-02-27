import apiClient from './client';
import type { Competition } from '../types';

export interface CreateCompetitionRequest {
  name: string;
  date?: string;
  zeroTime?: string;
}

export async function getCompetitions(): Promise<Competition[]> {
  const { data } = await apiClient.get<Competition[]>('/competitions');
  return data;
}

export async function getCompetition(id: number): Promise<Competition> {
  const { data } = await apiClient.get<Competition>(`/competitions/${id}`);
  return data;
}

export async function createCompetition(
  competition: CreateCompetitionRequest
): Promise<Competition> {
  const { data } = await apiClient.post<Competition>('/competitions', competition);
  return data;
}

export async function updateCompetition(
  id: number,
  competition: Partial<Competition>
): Promise<Competition> {
  const { data } = await apiClient.put<Competition>(`/competitions/${id}`, competition);
  return data;
}
