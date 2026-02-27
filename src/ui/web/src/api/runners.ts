import apiClient from './client';
import type { Runner } from '../types';

export async function getRunners(): Promise<Runner[]> {
  const { data } = await apiClient.get<Runner[]>('/runners');
  return data;
}

export async function getRunner(id: number): Promise<Runner> {
  const { data } = await apiClient.get<Runner>(`/runners/${id}`);
  return data;
}

export async function createRunner(runner: Omit<Runner, 'id'>): Promise<Runner> {
  const { data } = await apiClient.post<Runner>('/runners', runner);
  return data;
}

export async function updateRunner(id: number, runner: Partial<Runner>): Promise<Runner> {
  const { data } = await apiClient.put<Runner>(`/runners/${id}`, runner);
  return data;
}

export async function deleteRunner(id: number): Promise<void> {
  await apiClient.delete(`/runners/${id}`);
}
