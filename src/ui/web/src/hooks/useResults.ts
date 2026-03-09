import { useState, useEffect, useCallback, useRef } from 'react';
import { api } from '../api/client';
import type { Result } from '../api/types';

export function useResults(params?: { classId?: number }) {
  const [results, setResults] = useState<Result[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  const [isRefreshing, setIsRefreshing] = useState(false);
  const [error, setError] = useState<Error | null>(null);
  const lastUpdatedRef = useRef<Date>(new Date());

  const fetchResults = useCallback(async (silent = false) => {
    if (!silent) setIsLoading(true);
    else setIsRefreshing(true);
    
    try {
      const data = await api.getResults(params);
      setResults(data);
      lastUpdatedRef.current = new Date();
      setError(null);
    } catch (err) {
      setError(err instanceof Error ? err : new Error('Failed to fetch results'));
    } finally {
      setIsLoading(false);
      setIsRefreshing(false);
    }
  }, [params?.classId]);

  useEffect(() => {
    fetchResults();
  }, [fetchResults]);

  return { 
    results, 
    isLoading, 
    isRefreshing,
    error, 
    refresh: () => fetchResults(false),
    silentRefresh: () => fetchResults(true),
    lastUpdated: lastUpdatedRef.current
  };
}
