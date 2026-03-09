import { api } from '../api/client';
import { useQuery, useMutation } from './useApi';
import type { Runner, RunnerStatus } from '../api/types';

export function useRunners(params?: { classId?: number; clubId?: number; search?: string }) {
  return useQuery(
    () => api.getRunners(params),
    [params?.classId, params?.clubId, params?.search]
  );
}

export function useRunner(id: number) {
  return useQuery(() => api.getRunner(id), [id]);
}

export function useCreateRunner() {
  return useMutation((data: Partial<Runner>) => api.createRunner(data));
}

export function useUpdateRunner() {
  return useMutation(({ id, data }: { id: number; data: Partial<Runner> }) => api.updateRunner(id, data));
}

export function useDeleteRunner() {
  return useMutation((id: number) => api.deleteRunner(id));
}

export function useBulkCreateRunners() {
  return useMutation((data: Partial<Runner>[]) => api.createRunnersBulk(data));
}

export function useUpdateRunnersBulk() {
  return useMutation(({ ids, data }: { ids: number[]; data: Partial<Runner> }) => api.updateRunnersBulk(ids, data));
}

export function useAssignStartTimesBulk() {
  return useMutation((ids: number[]) => api.assignStartTimesBulk(ids));
}

export function useImportXMLRunners() {
  return useMutation((file: File) => api.importRunnersFromXML(file));
}

export function useSetRunnerStatus() {
  return useMutation(({ id, status }: { id: number; status: RunnerStatus }) =>
    api.setRunnerStatus(id, status)
  );
}
