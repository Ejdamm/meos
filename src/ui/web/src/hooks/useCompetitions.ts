import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query';
import { getCompetitions, getCompetition, updateCompetition } from '../api';
import type { Competition } from '../types';

const competitionsKeys = {
  all: ['competitions'] as const,
  detail: (id: number) => ['competitions', id] as const,
};

export function useCompetitions() {
  return useQuery({
    queryKey: competitionsKeys.all,
    queryFn: getCompetitions,
  });
}

export function useCompetition(id: number) {
  return useQuery({
    queryKey: competitionsKeys.detail(id),
    queryFn: () => getCompetition(id),
    enabled: id > 0,
  });
}

export function useUpdateCompetition() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: ({ id, competition }: { id: number; competition: Partial<Competition> }) =>
      updateCompetition(id, competition),
    onSuccess: (_data, { id }) => {
      queryClient.invalidateQueries({ queryKey: competitionsKeys.all });
      queryClient.invalidateQueries({ queryKey: competitionsKeys.detail(id) });
    },
  });
}
