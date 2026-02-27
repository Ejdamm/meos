import apiClient from './client';
import type { Club } from '../types';

export async function getClubs(): Promise<Club[]> {
  const { data } = await apiClient.get<Club[]>('/clubs');
  return data;
}

export async function getClub(id: number): Promise<Club> {
  const { data } = await apiClient.get<Club>(`/clubs/${id}`);
  return data;
}

export async function createClub(club: Omit<Club, 'id'>): Promise<Club> {
  const { data } = await apiClient.post<Club>('/clubs', club);
  return data;
}

export async function updateClub(id: number, club: Partial<Club>): Promise<Club> {
  const { data } = await apiClient.put<Club>(`/clubs/${id}`, club);
  return data;
}

export async function deleteClub(id: number): Promise<void> {
  await apiClient.delete(`/clubs/${id}`);
}
