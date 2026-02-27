import { useQuery } from '@tanstack/react-query';
import { getPunches } from '../api';

const punchesKeys = {
  all: ['punches'] as const,
};

export function usePunches(refetchInterval?: number) {
  return useQuery({
    queryKey: punchesKeys.all,
    queryFn: getPunches,
    refetchInterval,
  });
}
