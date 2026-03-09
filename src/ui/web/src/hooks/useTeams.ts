import { api } from '../api/client';
import { useQuery, useMutation } from './useApi';
import type { Team } from '../api/types';

export function useTeams(params?: { classId?: number; clubId?: number }) {
  return useQuery(
    () => api.getTeams(params),
    [params?.classId, params?.clubId]
  );
}

export function useTeam(id: number) {
  return useQuery(() => api.getTeam(id), [id]);
}

export function useCreateTeam() {
  return useMutation((data: Partial<Team>) => api.createTeam(data));
}

export function useUpdateTeam() {
  return useMutation(({ id, data }: { id: number; data: Partial<Team> }) => api.updateTeam(id, data));
}

export function useDeleteTeam() {
  return useMutation((id: number) => api.deleteTeam(id));
}
