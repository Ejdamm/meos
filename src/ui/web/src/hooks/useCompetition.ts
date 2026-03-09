import { api } from '../api/client';
import { useQuery } from './useApi';

export function useCompetitions() {
  return useQuery(() => api.getCompetitions());
}

export function useCompetition(id: number) {
  return useQuery(() => api.getCompetition(id), [id]);
}

export function useActiveCompetition() {
  // For now, just get the first one
  return useQuery(async () => {
    const competitions = await api.getCompetitions();
    if (competitions.length > 0) {
      return api.getCompetition(competitions[0].id);
    }
    return null;
  });
}
