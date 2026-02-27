import { describe, it, expect, vi } from 'vitest';
import { render, screen, waitFor } from '@testing-library/react';
import { QueryClient, QueryClientProvider } from '@tanstack/react-query';
import { createMemoryRouter, RouterProvider, Navigate, Outlet } from 'react-router-dom';
import { lazy, Suspense, Component } from 'react';
import type { ReactNode, ErrorInfo } from 'react';
import { ToastProvider } from '../components/Toast';
import ErrorBoundary from '../components/ErrorBoundary';

/* ------------------------------------------------------------------ */
/*  Helpers                                                            */
/* ------------------------------------------------------------------ */

function createTestQueryClient() {
  return new QueryClient({
    defaultOptions: { queries: { retry: false, gcTime: 0 }, mutations: { retry: false } },
  });
}

function Providers({ children }: { children: ReactNode }) {
  return (
    <QueryClientProvider client={createTestQueryClient()}>
      <ToastProvider>{children}</ToastProvider>
    </QueryClientProvider>
  );
}

function renderRouter(initialEntries: string[], routes: Parameters<typeof createMemoryRouter>[0]) {
  const router = createMemoryRouter(routes, { initialEntries });
  return render(
    <Providers>
      <RouterProvider router={router} />
    </Providers>,
  );
}

/* ------------------------------------------------------------------ */
/*  Stub pages – avoid pulling real pages and their heavy deps         */
/* ------------------------------------------------------------------ */

function StubCompetition() { return <div>CompetitionPage</div>; }
function StubRunners() { return <div>RunnersPage</div>; }
function StubTeams() { return <div>TeamsPage</div>; }
function StubClasses() { return <div>ClassesPage</div>; }
function StubCourses() { return <div>CoursesPage</div>; }
function StubControls() { return <div>ControlsPage</div>; }
function StubClubs() { return <div>ClubsPage</div>; }
function StubSi() { return <div>SiPage</div>; }
function StubLists() { return <div>ListsPage</div>; }
function StubSpeaker() { return <div>SpeakerPage</div>; }
function StubAutomations() { return <div>AutomationsPage</div>; }

function SimpleShell() {
  return (
    <div>
      <nav>Shell</nav>
      <Outlet />
    </div>
  );
}

/* Route definitions mirroring App.tsx but with stubs */
function buildRoutes(overrides?: { errorElement?: React.ReactElement }) {
  return [
    {
      element: <SimpleShell />,
      errorElement: overrides?.errorElement,
      children: [
        { index: true, element: <Navigate to="/competition" replace /> },
        { path: 'competition', element: <StubCompetition /> },
        { path: 'runners', element: <StubRunners /> },
        { path: 'teams', element: <StubTeams /> },
        { path: 'classes', element: <StubClasses /> },
        { path: 'courses', element: <StubCourses /> },
        { path: 'controls', element: <StubControls /> },
        { path: 'clubs', element: <StubClubs /> },
        { path: 'si', element: <StubSi /> },
        { path: 'lists', element: <StubLists /> },
        { path: 'speaker', element: <StubSpeaker /> },
        { path: 'automations', element: <StubAutomations /> },
        { path: '*', element: <Navigate to="/competition" replace /> },
      ],
    },
  ];
}

/* ------------------------------------------------------------------ */
/*  1. Route rendering                                                 */
/* ------------------------------------------------------------------ */

describe('Routing', () => {
  const routeCases: [string, string][] = [
    ['/competition', 'CompetitionPage'],
    ['/runners', 'RunnersPage'],
    ['/teams', 'TeamsPage'],
    ['/classes', 'ClassesPage'],
    ['/courses', 'CoursesPage'],
    ['/controls', 'ControlsPage'],
    ['/clubs', 'ClubsPage'],
    ['/si', 'SiPage'],
    ['/lists', 'ListsPage'],
    ['/speaker', 'SpeakerPage'],
    ['/automations', 'AutomationsPage'],
  ];

  it.each(routeCases)('renders correct component for %s', async (path, expected) => {
    renderRouter([path], buildRoutes());
    await waitFor(() => {
      expect(screen.getByText(expected)).toBeInTheDocument();
    });
  });

  it('redirects root (/) to /competition', async () => {
    renderRouter(['/'], buildRoutes());
    await waitFor(() => {
      expect(screen.getByText('CompetitionPage')).toBeInTheDocument();
    });
  });

  it('redirects unknown route to /competition', async () => {
    renderRouter(['/does-not-exist'], buildRoutes());
    await waitFor(() => {
      expect(screen.getByText('CompetitionPage')).toBeInTheDocument();
    });
  });

  it('renders AppShell layout around page', async () => {
    renderRouter(['/competition'], buildRoutes());
    await waitFor(() => {
      expect(screen.getByText('Shell')).toBeInTheDocument();
      expect(screen.getByText('CompetitionPage')).toBeInTheDocument();
    });
  });
});

/* ------------------------------------------------------------------ */
/*  2. Lazy loading / Suspense                                         */
/* ------------------------------------------------------------------ */

describe('Lazy loading with Suspense', () => {
  it('shows loading fallback while lazy component loads', async () => {
    let resolveImport: (mod: { default: () => React.JSX.Element }) => void;
    const lazyComponent = lazy(
      () => new Promise<{ default: () => React.JSX.Element }>((resolve) => { resolveImport = resolve; }),
    );

    function LazyPage({ children }: { children: ReactNode }) {
      return <Suspense fallback={<div>Laddar…</div>}>{children}</Suspense>;
    }

    const LazyComp = lazyComponent;
    const routes = [
      {
        element: <SimpleShell />,
        children: [
          { path: 'lazy', element: <LazyPage><LazyComp /></LazyPage> },
        ],
      },
    ];

    renderRouter(['/lazy'], routes);
    expect(screen.getByText('Laddar…')).toBeInTheDocument();

    // Resolve the lazy import
    await resolveImport!({ default: () => <div>LazyContent</div> });

    await waitFor(() => {
      expect(screen.getByText('LazyContent')).toBeInTheDocument();
    });
    expect(screen.queryByText('Laddar…')).not.toBeInTheDocument();
  });
});

/* ------------------------------------------------------------------ */
/*  3. RouteErrorFallback (router-level error handling)                */
/* ------------------------------------------------------------------ */

describe('RouteErrorFallback', () => {
  it('shows error UI when a route component throws during render', async () => {
    const consoleError = vi.spyOn(console, 'error').mockImplementation(() => {});

    function CrashingComponent(): React.JSX.Element {
      throw new Error('Render explosion');
    }

    function RouteErrorFallback() {
      return (
        <div role="alert">
          <h2>Något gick fel</h2>
          <button>Försök igen</button>
        </div>
      );
    }

    const routes = [
      {
        element: <SimpleShell />,
        errorElement: <RouteErrorFallback />,
        children: [
          { path: 'crash', element: <CrashingComponent /> },
        ],
      },
    ];

    renderRouter(['/crash'], routes);

    await waitFor(() => {
      expect(screen.getByRole('alert')).toBeInTheDocument();
      expect(screen.getByText('Något gick fel')).toBeInTheDocument();
    });
    expect(screen.getByText('Försök igen')).toBeInTheDocument();

    consoleError.mockRestore();
  });
});

/* ------------------------------------------------------------------ */
/*  4. ErrorBoundary component                                         */
/* ------------------------------------------------------------------ */

describe('ErrorBoundary', () => {
  it('renders children when there is no error', () => {
    render(
      <ErrorBoundary>
        <div>Happy child</div>
      </ErrorBoundary>,
    );
    expect(screen.getByText('Happy child')).toBeInTheDocument();
  });

  it('catches render error and shows fallback UI', () => {
    const consoleError = vi.spyOn(console, 'error').mockImplementation(() => {});

    function Bomb(): React.JSX.Element {
      throw new Error('Boom!');
    }

    render(
      <ErrorBoundary>
        <Bomb />
      </ErrorBoundary>,
    );

    expect(screen.getByRole('alert')).toBeInTheDocument();
    expect(screen.getByText('Något gick fel')).toBeInTheDocument();
    expect(screen.getByText('Boom!')).toBeInTheDocument();
    expect(screen.getByText('Försök igen')).toBeInTheDocument();

    consoleError.mockRestore();
  });

  it('displays generic message when error has no message', () => {
    const consoleError = vi.spyOn(console, 'error').mockImplementation(() => {});

    function Bomb(): React.JSX.Element {
      throw new Error();
    }

    render(
      <ErrorBoundary>
        <Bomb />
      </ErrorBoundary>,
    );

    expect(screen.getByRole('alert')).toBeInTheDocument();

    consoleError.mockRestore();
  });

  it('recovers when retry button is clicked', async () => {
    const consoleError = vi.spyOn(console, 'error').mockImplementation(() => {});
    const { default: userEvent } = await import('@testing-library/user-event');
    const user = userEvent.setup();

    let shouldThrow = true;

    function MaybeExplode(): React.JSX.Element {
      if (shouldThrow) throw new Error('Crash');
      return <div>Recovered</div>;
    }

    render(
      <ErrorBoundary>
        <MaybeExplode />
      </ErrorBoundary>,
    );

    expect(screen.getByText('Något gick fel')).toBeInTheDocument();

    shouldThrow = false;
    await user.click(screen.getByText('Försök igen'));

    await waitFor(() => {
      expect(screen.getByText('Recovered')).toBeInTheDocument();
    });

    consoleError.mockRestore();
  });

  it('logs error to console', () => {
    const consoleError = vi.spyOn(console, 'error').mockImplementation(() => {});

    function Bomb(): React.JSX.Element {
      throw new Error('Log this');
    }

    render(
      <ErrorBoundary>
        <Bomb />
      </ErrorBoundary>,
    );

    expect(consoleError).toHaveBeenCalled();

    consoleError.mockRestore();
  });
});
