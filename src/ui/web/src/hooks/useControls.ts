import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query';
import { getControls, getControl, createControl, updateControl, deleteControl } from '../api';
import type { Control } from '../types';

const controlsKeys = {
  all: ['controls'] as const,
  detail: (id: number) => ['controls', id] as const,
};

export function useControls() {
  return useQuery({
    queryKey: controlsKeys.all,
    queryFn: getControls,
  });
}

export function useControl(id: number) {
  return useQuery({
    queryKey: controlsKeys.detail(id),
    queryFn: () => getControl(id),
    enabled: id > 0,
  });
}

export function useCreateControl() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (control: Omit<Control, 'id'>) => createControl(control),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: controlsKeys.all });
    },
  });
}

export function useUpdateControl() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: ({ id, control }: { id: number; control: Partial<Control> }) =>
      updateControl(id, control),
    onSuccess: (_data, { id }) => {
      queryClient.invalidateQueries({ queryKey: controlsKeys.all });
      queryClient.invalidateQueries({ queryKey: controlsKeys.detail(id) });
    },
  });
}

export function useDeleteControl() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (id: number) => deleteControl(id),
    onSuccess: (_data, id) => {
      queryClient.invalidateQueries({ queryKey: controlsKeys.all });
      queryClient.removeQueries({ queryKey: controlsKeys.detail(id) });
    },
  });
}
