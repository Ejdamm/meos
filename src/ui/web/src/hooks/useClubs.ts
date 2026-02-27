import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query';
import { getClubs, getClub, createClub, updateClub, deleteClub } from '../api';
import type { Club } from '../types';

const clubsKeys = {
  all: ['clubs'] as const,
  detail: (id: number) => ['clubs', id] as const,
};

export function useClubs() {
  return useQuery({
    queryKey: clubsKeys.all,
    queryFn: getClubs,
  });
}

export function useClub(id: number) {
  return useQuery({
    queryKey: clubsKeys.detail(id),
    queryFn: () => getClub(id),
    enabled: id > 0,
  });
}

export function useCreateClub() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (club: Omit<Club, 'id'>) => createClub(club),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: clubsKeys.all });
    },
  });
}

export function useUpdateClub() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: ({ id, club }: { id: number; club: Partial<Club> }) =>
      updateClub(id, club),
    onSuccess: (_data, { id }) => {
      queryClient.invalidateQueries({ queryKey: clubsKeys.all });
      queryClient.invalidateQueries({ queryKey: clubsKeys.detail(id) });
    },
  });
}

export function useDeleteClub() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (id: number) => deleteClub(id),
    onSuccess: (_data, id) => {
      queryClient.invalidateQueries({ queryKey: clubsKeys.all });
      queryClient.removeQueries({ queryKey: clubsKeys.detail(id) });
    },
  });
}
