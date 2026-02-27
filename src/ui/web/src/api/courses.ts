import apiClient from './client';
import type { Course } from '../types';

export async function getCourses(): Promise<Course[]> {
  const { data } = await apiClient.get<Course[]>('/courses');
  return data;
}

export async function getCourse(id: number): Promise<Course> {
  const { data } = await apiClient.get<Course>(`/courses/${id}`);
  return data;
}

export async function createCourse(course: Omit<Course, 'id'>): Promise<Course> {
  const { data } = await apiClient.post<Course>('/courses', course);
  return data;
}

export async function updateCourse(id: number, course: Partial<Course>): Promise<Course> {
  const { data } = await apiClient.put<Course>(`/courses/${id}`, course);
  return data;
}

export async function deleteCourse(id: number): Promise<void> {
  await apiClient.delete(`/courses/${id}`);
}
