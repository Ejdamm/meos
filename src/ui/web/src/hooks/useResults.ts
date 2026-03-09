import { useState, useEffect, useCallback } from 'react';
import { api } from '../api/client';
import type { Result } from '../api/types';

export function useResults(params?: { classId?: number }) {
  const [results, setResults] = useState<Result[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState<Error | null>(null);

  const fetchResults = useCallback(async () => {
    setIsLoading(true);
    try {
      const data = await api.getResults(params);
      setResults(data);
      setError(null);
    } catch (err) {
      setError(err instanceof Error ? err : new Error('Failed to fetch results'));
    } finally {
      setIsLoading(false);
    }
  }, [params?.classId]);

  useEffect(() => {
    fetchResults();
  }, [fetchResults]);

  return { results, isLoading, error, refresh: fetchResults };
}
