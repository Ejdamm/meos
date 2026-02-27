import apiClient from './client';
import type { Team } from '../types';

export async function getTeams(): Promise<Team[]> {
  const { data } = await apiClient.get<Team[]>('/teams');
  return data;
}

export async function getTeam(id: number): Promise<Team> {
  const { data } = await apiClient.get<Team>(`/teams/${id}`);
  return data;
}

export async function createTeam(team: Omit<Team, 'id'>): Promise<Team> {
  const { data } = await apiClient.post<Team>('/teams', team);
  return data;
}

export async function updateTeam(id: number, team: Partial<Team>): Promise<Team> {
  const { data } = await apiClient.put<Team>(`/teams/${id}`, team);
  return data;
}

export async function deleteTeam(id: number): Promise<void> {
  await apiClient.delete(`/teams/${id}`);
}
