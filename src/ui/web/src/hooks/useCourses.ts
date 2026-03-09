import { api } from '../api/client';
import { useQuery, useMutation } from './useApi';
import type { Course } from '../api/types';

export function useCourses() {
  return useQuery(() => api.getCourses(), []);
}

export function useCourse(id: number) {
  return useQuery(() => api.getCourse(id), [id]);
}

export function useCreateCourse() {
  return useMutation((data: Partial<Course>) => api.createCourse(data));
}

export function useUpdateCourse() {
  return useMutation(({ id, data }: { id: number; data: Partial<Course> }) => api.updateCourse(id, data));
}

export function useDeleteCourse() {
  return useMutation((id: number) => api.deleteCourse(id));
}
