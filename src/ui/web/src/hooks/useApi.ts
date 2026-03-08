import { useState, useEffect, useCallback } from 'react';

interface UseQueryOptions<T> {
  enabled?: boolean;
  onSuccess?: (data: T) => void;
  onError?: (error: Error) => void;
}

export function useQuery<T>(
  queryFn: () => Promise<T>,
  deps: any[] = [],
  options: UseQueryOptions<T> = {}
) {
  const [data, setData] = useState<T | null>(null);
  const [isLoading, setIsLoading] = useState(options.enabled !== false);
  const [error, setError] = useState<Error | null>(null);

  const fetchData = useCallback(async () => {
    setIsLoading(true);
    setError(null);
    try {
      const result = await queryFn();
      setData(result);
      options.onSuccess?.(result);
    } catch (err) {
      const error = err instanceof Error ? err : new Error('An unknown error occurred');
      setError(error);
      options.onError?.(error);
    } finally {
      setIsLoading(false);
    }
  }, deps);

  useEffect(() => {
    if (options.enabled !== false) {
      fetchData();
    }
  }, [fetchData, options.enabled]);

  return { data, isLoading, error, refetch: fetchData };
}

export function useMutation<T, V>(
  mutationFn: (variables: V) => Promise<T>,
  options: {
    onSuccess?: (data: T) => void;
    onError?: (error: Error) => void;
  } = {}
) {
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState<Error | null>(null);

  const mutate = async (variables: V) => {
    setIsLoading(true);
    setError(null);
    try {
      const result = await mutationFn(variables);
      options.onSuccess?.(result);
      return result;
    } catch (err) {
      const error = err instanceof Error ? err : new Error('An unknown error occurred');
      setError(error);
      options.onError?.(error);
      throw error;
    } finally {
      setIsLoading(false);
    }
  };

  return { mutate, isLoading, error };
}
