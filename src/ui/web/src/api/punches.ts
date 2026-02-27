import apiClient from './client';
import type { Punch } from '../types';

export async function getPunches(): Promise<Punch[]> {
  const { data } = await apiClient.get<Punch[]>('/punches');
  return data;
}
