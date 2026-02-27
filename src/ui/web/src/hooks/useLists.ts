import { useQuery } from '@tanstack/react-query';
import { getLists, getList, getResults } from '../api';

const listsKeys = {
  all: ['lists'] as const,
  detail: (type: string) => ['lists', type] as const,
  results: (classId?: number) => ['results', classId] as const,
};

export function useLists() {
  return useQuery({
    queryKey: listsKeys.all,
    queryFn: getLists,
  });
}

export function useList(type: string) {
  return useQuery({
    queryKey: listsKeys.detail(type),
    queryFn: () => getList(type),
    enabled: type.length > 0,
  });
}

export function useResults(classId?: number) {
  return useQuery({
    queryKey: listsKeys.results(classId),
    queryFn: () => getResults(classId),
  });
}
