import apiClient from './client';
import type { Control } from '../types';

export async function getControls(): Promise<Control[]> {
  const { data } = await apiClient.get<Control[]>('/controls');
  return data;
}

export async function getControl(id: number): Promise<Control> {
  const { data } = await apiClient.get<Control>(`/controls/${id}`);
  return data;
}

export async function createControl(control: Omit<Control, 'id'>): Promise<Control> {
  const { data } = await apiClient.post<Control>('/controls', control);
  return data;
}

export async function updateControl(id: number, control: Partial<Control>): Promise<Control> {
  const { data } = await apiClient.put<Control>(`/controls/${id}`, control);
  return data;
}

export async function deleteControl(id: number): Promise<void> {
  await apiClient.delete(`/controls/${id}`);
}
