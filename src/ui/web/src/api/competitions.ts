import apiClient from './client';
import type { Competition } from '../types';

export async function getCompetitions(): Promise<Competition[]> {
  const { data } = await apiClient.get<Competition[]>('/competitions');
  return data;
}

export async function getCompetition(id: number): Promise<Competition> {
  const { data } = await apiClient.get<Competition>(`/competitions/${id}`);
  return data;
}

export async function updateCompetition(
  id: number,
  competition: Partial<Competition>
): Promise<Competition> {
  const { data } = await apiClient.put<Competition>(`/competitions/${id}`, competition);
  return data;
}
