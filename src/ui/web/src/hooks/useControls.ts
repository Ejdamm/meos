import { api } from '../api/client';
import { useQuery, useMutation } from './useApi';
import type { Control } from '../api/types';

export function useControls() {
  return useQuery(() => api.getControls(), []);
}

export function useControl(id: number) {
  return useQuery(() => api.getControl(id), [id]);
}

export function useCreateControl() {
  return useMutation((data: Partial<Control>) => api.createControl(data));
}

export function useUpdateControl() {
  return useMutation(({ id, data }: { id: number; data: Partial<Control> }) => api.updateControl(id, data));
}

export function useDeleteControl() {
  return useMutation((id: number) => api.deleteControl(id));
}
