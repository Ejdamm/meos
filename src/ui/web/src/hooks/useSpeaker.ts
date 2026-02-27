import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query';
import { getSpeakerConfig, updateSpeakerConfig, getSpeakerMonitor } from '../api';
import type { SpeakerConfig } from '../api';

const speakerKeys = {
  config: ['speaker', 'config'] as const,
  monitor: ['speaker', 'monitor'] as const,
};

export function useSpeakerConfig() {
  return useQuery({
    queryKey: speakerKeys.config,
    queryFn: getSpeakerConfig,
  });
}

export function useUpdateSpeakerConfig() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (config: SpeakerConfig) => updateSpeakerConfig(config),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: speakerKeys.config });
    },
  });
}

export function useSpeakerMonitor(refetchInterval = 5000) {
  return useQuery({
    queryKey: speakerKeys.monitor,
    queryFn: getSpeakerMonitor,
    refetchInterval,
  });
}
