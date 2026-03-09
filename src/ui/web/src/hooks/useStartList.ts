import { useState, useEffect, useCallback } from 'react';
import { api } from '../api/client';
import type { StartListEntry } from '../api/types';

export function useStartList(params?: { classId?: number }) {
  const [startList, setStartList] = useState<StartListEntry[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState<Error | null>(null);

  const fetchStartList = useCallback(async () => {
    setIsLoading(true);
    try {
      const data = await api.getStartList(params);
      setStartList(data);
      setError(null);
    } catch (err) {
      setError(err instanceof Error ? err : new Error('Failed to fetch start list'));
    } finally {
      setIsLoading(false);
    }
  }, [params?.classId]);

  useEffect(() => {
    fetchStartList();
  }, [fetchStartList]);

  return { startList, isLoading, error, refresh: fetchStartList };
}
