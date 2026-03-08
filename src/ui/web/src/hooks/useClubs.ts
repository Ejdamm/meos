import { api } from '../api/client';
import { useQuery, useMutation } from './useApi';
import type { Club } from '../api/types';

export function useClubs() {
  return useQuery(() => api.getClubs(), []);
}

export function useClub(id: number) {
  return useQuery(() => api.getClub(id), [id]);
}

export function useCreateClub() {
  return useMutation((data: Partial<Club>) => api.createClub(data));
}

export function useUpdateClub(id: number) {
  return useMutation((data: Partial<Club>) => api.updateClub(id, data));
}

export function useDeleteClub() {
  return useMutation((id: number) => api.deleteClub(id));
}
