import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query';
import { getCards, getCard, readCard } from '../api';
import type { ReadCardRequest } from '../api';

const cardsKeys = {
  all: ['cards'] as const,
  detail: (id: number) => ['cards', id] as const,
};

export function useCards() {
  return useQuery({
    queryKey: cardsKeys.all,
    queryFn: getCards,
  });
}

export function useCard(id: number) {
  return useQuery({
    queryKey: cardsKeys.detail(id),
    queryFn: () => getCard(id),
    enabled: id > 0,
  });
}

export function useReadCard() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (request: ReadCardRequest) => readCard(request),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: cardsKeys.all });
    },
  });
}
