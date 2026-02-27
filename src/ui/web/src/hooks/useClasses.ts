import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query';
import { getClasses, getClass, createClass, updateClass, deleteClass } from '../api';
import type { Class } from '../types';

const classesKeys = {
  all: ['classes'] as const,
  detail: (id: number) => ['classes', id] as const,
};

export function useClasses() {
  return useQuery({
    queryKey: classesKeys.all,
    queryFn: getClasses,
  });
}

export function useClass(id: number) {
  return useQuery({
    queryKey: classesKeys.detail(id),
    queryFn: () => getClass(id),
    enabled: id > 0,
  });
}

export function useCreateClass() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (cls: Omit<Class, 'id'>) => createClass(cls),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: classesKeys.all });
    },
  });
}

export function useUpdateClass() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: ({ id, cls }: { id: number; cls: Partial<Class> }) =>
      updateClass(id, cls),
    onSuccess: (_data, { id }) => {
      queryClient.invalidateQueries({ queryKey: classesKeys.all });
      queryClient.invalidateQueries({ queryKey: classesKeys.detail(id) });
    },
  });
}

export function useDeleteClass() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (id: number) => deleteClass(id),
    onSuccess: (_data, id) => {
      queryClient.invalidateQueries({ queryKey: classesKeys.all });
      queryClient.removeQueries({ queryKey: classesKeys.detail(id) });
    },
  });
}
