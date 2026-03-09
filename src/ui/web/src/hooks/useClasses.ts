import { api } from '../api/client';
import { useQuery, useMutation } from './useApi';
import type { Class } from '../api/types';

export function useClasses() {
  return useQuery(() => api.getClasses(), []);
}

export function useClass(id: number) {
  return useQuery(() => api.getClass(id), [id]);
}

export function useCreateClass() {
  return useMutation((data: Partial<Class>) => api.createClass(data));
}

export function useUpdateClass() {
  return useMutation(({ id, data }: { id: number; data: Partial<Class> }) => api.updateClass(id, data));
}

export function useDeleteClass() {
  return useMutation((id: number) => api.deleteClass(id));
}
