import { renderHook, waitFor } from '@testing-library/react';
import { http, HttpResponse } from 'msw';
import { describe, it, expect, vi } from 'vitest';
import { QueryClient, QueryClientProvider } from '@tanstack/react-query';
import type { ReactNode } from 'react';
import { server, fixtures } from '../test';

import {
  useRunners, useRunner, useCreateRunner, useUpdateRunner, useDeleteRunner,
  useCompetitions, useCompetition, useCreateCompetition, useUpdateCompetition,
  useClasses, useCreateClass, useDeleteClass,
  useTeams, useCreateTeam,
  useCourses, useDeleteCourse,
  useControls, useUpdateControl,
  useClubs, useCreateClub,
  useCards, useCard, useReadCard,
} from '../hooks';

function createWrapper() {
  const queryClient = new QueryClient({
    defaultOptions: {
      queries: { retry: false, gcTime: 0 },
      mutations: { retry: false },
    },
  });
  return {
    queryClient,
    wrapper: ({ children }: { children: ReactNode }) => (
      <QueryClientProvider client={queryClient}>{children}</QueryClientProvider>
    ),
  };
}

// --- Runners ---
describe('useRunners', () => {
  it('returns loading then data', async () => {
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useRunners(), { wrapper });
    expect(result.current.isLoading).toBe(true);
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(result.current.data).toEqual(fixtures.runners);
  });

  it('handles error state', async () => {
    server.use(http.get('/api/runners', () => new HttpResponse(null, { status: 500 })));
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useRunners(), { wrapper });
    await waitFor(() => expect(result.current.isError).toBe(true));
  });
});

describe('useRunner', () => {
  it('fetches single runner by id', async () => {
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useRunner(1), { wrapper });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(result.current.data).toEqual(fixtures.runners[0]);
  });

  it('is disabled when id <= 0', () => {
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useRunner(0), { wrapper });
    expect(result.current.fetchStatus).toBe('idle');
  });
});

describe('useCreateRunner', () => {
  it('creates runner and invalidates list cache', async () => {
    const { wrapper, queryClient } = createWrapper();
    const invalidateSpy = vi.spyOn(queryClient, 'invalidateQueries');
    queryClient.setQueryData(['runners'], fixtures.runners);

    const { result } = renderHook(() => useCreateRunner(), { wrapper });
    result.current.mutate({ name: 'New Runner', clubId: 1, club: 'OK Test', classId: 1, class: 'H21', cardNo: 99999, bib: '200', startTime: 0, finishTime: 0, runningTime: 0, status: 0, birthYear: 2000, nationality: 'SWE', sex: 0 });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(invalidateSpy).toHaveBeenCalledWith({ queryKey: ['runners'] });
  });
});

describe('useUpdateRunner', () => {
  it('updates runner and invalidates caches', async () => {
    const { wrapper, queryClient } = createWrapper();
    const invalidateSpy = vi.spyOn(queryClient, 'invalidateQueries');

    const { result } = renderHook(() => useUpdateRunner(), { wrapper });
    result.current.mutate({ id: 1, runner: { name: 'Updated' } });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(invalidateSpy).toHaveBeenCalledWith({ queryKey: ['runners'] });
    expect(invalidateSpy).toHaveBeenCalledWith({ queryKey: ['runners', 1] });
  });
});

describe('useDeleteRunner', () => {
  it('deletes runner and clears detail cache', async () => {
    const { wrapper, queryClient } = createWrapper();
    const invalidateSpy = vi.spyOn(queryClient, 'invalidateQueries');
    const removeSpy = vi.spyOn(queryClient, 'removeQueries');
    queryClient.setQueryData(['runners', 1], fixtures.runners[0]);

    const { result } = renderHook(() => useDeleteRunner(), { wrapper });
    result.current.mutate(1);
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(invalidateSpy).toHaveBeenCalledWith({ queryKey: ['runners'] });
    expect(removeSpy).toHaveBeenCalledWith({ queryKey: ['runners', 1] });
    expect(queryClient.getQueryData(['runners', 1])).toBeUndefined();
  });
});

// --- Competitions ---
describe('useCompetitions', () => {
  it('returns competitions data', async () => {
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useCompetitions(), { wrapper });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(result.current.data).toEqual(fixtures.competitions);
  });

  it('handles error state', async () => {
    server.use(http.get('/api/competitions', () => new HttpResponse(null, { status: 500 })));
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useCompetitions(), { wrapper });
    await waitFor(() => expect(result.current.isError).toBe(true));
  });
});

describe('useCompetition', () => {
  it('fetches single competition', async () => {
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useCompetition(1), { wrapper });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(result.current.data).toEqual(fixtures.competitions[0]);
  });
});

describe('useCreateCompetition', () => {
  it('creates competition and invalidates cache', async () => {
    const { wrapper, queryClient } = createWrapper();
    const invalidateSpy = vi.spyOn(queryClient, 'invalidateQueries');

    const { result } = renderHook(() => useCreateCompetition(), { wrapper });
    result.current.mutate({ name: 'New Comp', date: '2026-04-01', zeroTime: '09:00:00' });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(invalidateSpy).toHaveBeenCalledWith({ queryKey: ['competitions'] });
  });
});

describe('useUpdateCompetition', () => {
  it('updates competition and invalidates caches', async () => {
    const { wrapper, queryClient } = createWrapper();
    const invalidateSpy = vi.spyOn(queryClient, 'invalidateQueries');

    const { result } = renderHook(() => useUpdateCompetition(), { wrapper });
    result.current.mutate({ id: 1, competition: { name: 'Updated' } });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(invalidateSpy).toHaveBeenCalledWith({ queryKey: ['competitions'] });
    expect(invalidateSpy).toHaveBeenCalledWith({ queryKey: ['competitions', 1] });
  });
});

// --- Classes ---
describe('useClasses', () => {
  it('returns classes data', async () => {
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useClasses(), { wrapper });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(result.current.data).toEqual(fixtures.classes);
  });
});

describe('useCreateClass', () => {
  it('creates class and invalidates cache', async () => {
    const { wrapper, queryClient } = createWrapper();
    const invalidateSpy = vi.spyOn(queryClient, 'invalidateQueries');

    const { result } = renderHook(() => useCreateClass(), { wrapper });
    result.current.mutate({ name: 'H35', type: 'individual', numStages: 1, courseId: 1 });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(invalidateSpy).toHaveBeenCalledWith({ queryKey: ['classes'] });
  });
});

describe('useDeleteClass', () => {
  it('deletes class and removes detail cache', async () => {
    const { wrapper, queryClient } = createWrapper();
    const removeSpy = vi.spyOn(queryClient, 'removeQueries');
    queryClient.setQueryData(['classes', 1], fixtures.classes[0]);

    const { result } = renderHook(() => useDeleteClass(), { wrapper });
    result.current.mutate(1);
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(removeSpy).toHaveBeenCalledWith({ queryKey: ['classes', 1] });
    expect(queryClient.getQueryData(['classes', 1])).toBeUndefined();
  });
});

// --- Teams ---
describe('useTeams', () => {
  it('returns teams data', async () => {
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useTeams(), { wrapper });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(result.current.data).toEqual(fixtures.teams);
  });
});

describe('useCreateTeam', () => {
  it('creates team and invalidates cache', async () => {
    const { wrapper, queryClient } = createWrapper();
    const invalidateSpy = vi.spyOn(queryClient, 'invalidateQueries');

    const { result } = renderHook(() => useCreateTeam(), { wrapper });
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    const { id: _id, ...teamData } = fixtures.teams[0]!;
    result.current.mutate({ ...teamData, name: 'New Team' });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(invalidateSpy).toHaveBeenCalledWith({ queryKey: ['teams'] });
  });
});

// --- Courses ---
describe('useCourses', () => {
  it('returns courses data', async () => {
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useCourses(), { wrapper });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(result.current.data).toEqual(fixtures.courses);
  });
});

describe('useDeleteCourse', () => {
  it('deletes course and removes detail cache', async () => {
    const { wrapper, queryClient } = createWrapper();
    const removeSpy = vi.spyOn(queryClient, 'removeQueries');
    queryClient.setQueryData(['courses', 1], fixtures.courses[0]);

    const { result } = renderHook(() => useDeleteCourse(), { wrapper });
    result.current.mutate(1);
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(removeSpy).toHaveBeenCalledWith({ queryKey: ['courses', 1] });
    expect(queryClient.getQueryData(['courses', 1])).toBeUndefined();
  });
});

// --- Controls ---
describe('useControls', () => {
  it('returns controls data', async () => {
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useControls(), { wrapper });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(result.current.data).toEqual(fixtures.controls);
  });
});

describe('useUpdateControl', () => {
  it('updates control and invalidates caches', async () => {
    const { wrapper, queryClient } = createWrapper();
    const invalidateSpy = vi.spyOn(queryClient, 'invalidateQueries');

    const { result } = renderHook(() => useUpdateControl(), { wrapper });
    result.current.mutate({ id: 31, control: { name: 'Updated' } });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(invalidateSpy).toHaveBeenCalledWith({ queryKey: ['controls'] });
    expect(invalidateSpy).toHaveBeenCalledWith({ queryKey: ['controls', 31] });
  });
});

// --- Clubs ---
describe('useClubs', () => {
  it('returns clubs data', async () => {
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useClubs(), { wrapper });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(result.current.data).toEqual(fixtures.clubs);
  });
});

describe('useCreateClub', () => {
  it('creates club and invalidates cache', async () => {
    const { wrapper, queryClient } = createWrapper();
    const invalidateSpy = vi.spyOn(queryClient, 'invalidateQueries');

    const { result } = renderHook(() => useCreateClub(), { wrapper });
    result.current.mutate({ name: 'New Club', country: 'NOR' });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(invalidateSpy).toHaveBeenCalledWith({ queryKey: ['clubs'] });
  });
});

// --- Cards ---
describe('useCards', () => {
  it('returns cards data', async () => {
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useCards(), { wrapper });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(result.current.data).toEqual(fixtures.cards);
  });
});

describe('useCard', () => {
  it('fetches single card', async () => {
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useCard(1), { wrapper });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(result.current.data).toEqual(fixtures.cards[0]);
  });

  it('is disabled when id <= 0', () => {
    const { wrapper } = createWrapper();
    const { result } = renderHook(() => useCard(0), { wrapper });
    expect(result.current.fetchStatus).toBe('idle');
  });
});

describe('useReadCard', () => {
  it('reads card and invalidates cards cache', async () => {
    const { wrapper, queryClient } = createWrapper();
    const invalidateSpy = vi.spyOn(queryClient, 'invalidateQueries');

    const { result } = renderHook(() => useReadCard(), { wrapper });
    result.current.mutate({ cardNo: 55555 });
    await waitFor(() => expect(result.current.isSuccess).toBe(true));
    expect(result.current.data?.cardNo).toBe(55555);
    expect(invalidateSpy).toHaveBeenCalledWith({ queryKey: ['cards'] });
  });
});
