import apiClient from './client';
import type { Class } from '../types';

export async function getClasses(): Promise<Class[]> {
  const { data } = await apiClient.get<Class[]>('/classes');
  return data;
}

export async function getClass(id: number): Promise<Class> {
  const { data } = await apiClient.get<Class>(`/classes/${id}`);
  return data;
}

export async function createClass(cls: Omit<Class, 'id'>): Promise<Class> {
  const { data } = await apiClient.post<Class>('/classes', cls);
  return data;
}

export async function updateClass(id: number, cls: Partial<Class>): Promise<Class> {
  const { data } = await apiClient.put<Class>(`/classes/${id}`, cls);
  return data;
}

export async function deleteClass(id: number): Promise<void> {
  await apiClient.delete(`/classes/${id}`);
}
