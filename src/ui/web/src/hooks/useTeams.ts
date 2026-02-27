import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query';
import { getTeams, getTeam, createTeam, updateTeam, deleteTeam } from '../api';
import type { Team } from '../types';

const teamsKeys = {
  all: ['teams'] as const,
  detail: (id: number) => ['teams', id] as const,
};

export function useTeams() {
  return useQuery({
    queryKey: teamsKeys.all,
    queryFn: getTeams,
  });
}

export function useTeam(id: number) {
  return useQuery({
    queryKey: teamsKeys.detail(id),
    queryFn: () => getTeam(id),
    enabled: id > 0,
  });
}

export function useCreateTeam() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (team: Omit<Team, 'id'>) => createTeam(team),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: teamsKeys.all });
    },
  });
}

export function useUpdateTeam() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: ({ id, team }: { id: number; team: Partial<Team> }) =>
      updateTeam(id, team),
    onSuccess: (_data, { id }) => {
      queryClient.invalidateQueries({ queryKey: teamsKeys.all });
      queryClient.invalidateQueries({ queryKey: teamsKeys.detail(id) });
    },
  });
}

export function useDeleteTeam() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (id: number) => deleteTeam(id),
    onSuccess: (_data, id) => {
      queryClient.invalidateQueries({ queryKey: teamsKeys.all });
      queryClient.removeQueries({ queryKey: teamsKeys.detail(id) });
    },
  });
}
