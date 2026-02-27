import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query';
import { getCourses, getCourse, createCourse, updateCourse, deleteCourse } from '../api';
import type { Course } from '../types';

const coursesKeys = {
  all: ['courses'] as const,
  detail: (id: number) => ['courses', id] as const,
};

export function useCourses() {
  return useQuery({
    queryKey: coursesKeys.all,
    queryFn: getCourses,
  });
}

export function useCourse(id: number) {
  return useQuery({
    queryKey: coursesKeys.detail(id),
    queryFn: () => getCourse(id),
    enabled: id > 0,
  });
}

export function useCreateCourse() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (course: Omit<Course, 'id'>) => createCourse(course),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: coursesKeys.all });
    },
  });
}

export function useUpdateCourse() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: ({ id, course }: { id: number; course: Partial<Course> }) =>
      updateCourse(id, course),
    onSuccess: (_data, { id }) => {
      queryClient.invalidateQueries({ queryKey: coursesKeys.all });
      queryClient.invalidateQueries({ queryKey: coursesKeys.detail(id) });
    },
  });
}

export function useDeleteCourse() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (id: number) => deleteCourse(id),
    onSuccess: (_data, id) => {
      queryClient.invalidateQueries({ queryKey: coursesKeys.all });
      queryClient.removeQueries({ queryKey: coursesKeys.detail(id) });
    },
  });
}
