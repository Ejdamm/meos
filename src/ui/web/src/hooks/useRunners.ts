import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query';
import { getRunners, getRunner, createRunner, updateRunner, deleteRunner } from '../api';
import type { Runner } from '../types';

const runnersKeys = {
  all: ['runners'] as const,
  detail: (id: number) => ['runners', id] as const,
};

export function useRunners() {
  return useQuery({
    queryKey: runnersKeys.all,
    queryFn: getRunners,
  });
}

export function useRunner(id: number) {
  return useQuery({
    queryKey: runnersKeys.detail(id),
    queryFn: () => getRunner(id),
    enabled: id > 0,
  });
}

export function useCreateRunner() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (runner: Omit<Runner, 'id'>) => createRunner(runner),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: runnersKeys.all });
    },
  });
}

export function useUpdateRunner() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: ({ id, runner }: { id: number; runner: Partial<Runner> }) =>
      updateRunner(id, runner),
    onSuccess: (_data, { id }) => {
      queryClient.invalidateQueries({ queryKey: runnersKeys.all });
      queryClient.invalidateQueries({ queryKey: runnersKeys.detail(id) });
    },
  });
}

export function useDeleteRunner() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (id: number) => deleteRunner(id),
    onSuccess: (_data, id) => {
      queryClient.invalidateQueries({ queryKey: runnersKeys.all });
      queryClient.removeQueries({ queryKey: runnersKeys.detail(id) });
    },
  });
}
