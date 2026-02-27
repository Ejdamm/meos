import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query';
import {
  getAutomations,
  getAutomation,
  createAutomation,
  updateAutomation,
  deleteAutomation,
  getAutomationStatus,
} from '../api';
import type { Automation } from '../api';

const automationsKeys = {
  all: ['automations'] as const,
  detail: (id: number) => ['automations', id] as const,
  status: (id: number) => ['automations', id, 'status'] as const,
};

export function useAutomations() {
  return useQuery({
    queryKey: automationsKeys.all,
    queryFn: getAutomations,
  });
}

export function useAutomation(id: number) {
  return useQuery({
    queryKey: automationsKeys.detail(id),
    queryFn: () => getAutomation(id),
    enabled: id > 0,
  });
}

export function useAutomationStatus(id: number, refetchInterval = 5000) {
  return useQuery({
    queryKey: automationsKeys.status(id),
    queryFn: () => getAutomationStatus(id),
    enabled: id > 0,
    refetchInterval,
  });
}

export function useCreateAutomation() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (automation: Omit<Automation, 'id'>) => createAutomation(automation),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: automationsKeys.all });
    },
  });
}

export function useUpdateAutomation() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: ({ id, automation }: { id: number; automation: Partial<Automation> }) =>
      updateAutomation(id, automation),
    onSuccess: (_data, { id }) => {
      queryClient.invalidateQueries({ queryKey: automationsKeys.all });
      queryClient.invalidateQueries({ queryKey: automationsKeys.detail(id) });
    },
  });
}

export function useDeleteAutomation() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (id: number) => deleteAutomation(id),
    onSuccess: (_data, id) => {
      queryClient.invalidateQueries({ queryKey: automationsKeys.all });
      queryClient.removeQueries({ queryKey: automationsKeys.detail(id) });
      queryClient.removeQueries({ queryKey: automationsKeys.status(id) });
    },
  });
}
