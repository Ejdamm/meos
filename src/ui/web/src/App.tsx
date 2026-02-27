import { lazy, Suspense } from 'react';
import { createBrowserRouter, Navigate, RouterProvider, useRouteError } from 'react-router-dom';
import { AppShell } from './components/layout';

const CompetitionPage = lazy(() => import('./pages/CompetitionPage'));
const RunnersPage = lazy(() => import('./pages/RunnersPage'));
const TeamsPage = lazy(() => import('./pages/TeamsPage'));
const ClassesPage = lazy(() => import('./pages/ClassesPage'));
const CoursesPage = lazy(() => import('./pages/CoursesPage'));
const ControlsPage = lazy(() => import('./pages/ControlsPage'));
const ClubsPage = lazy(() => import('./pages/ClubsPage'));
const SiPage = lazy(() => import('./pages/SiPage'));
const ListsPage = lazy(() => import('./pages/ListsPage'));
const SpeakerPage = lazy(() => import('./pages/SpeakerPage'));
const AutomationsPage = lazy(() => import('./pages/AutomationsPage'));

function LazyPage({ children }: { children: React.ReactNode }): React.JSX.Element {
  return <Suspense fallback={<div className="app-loading">Laddar…</div>}>{children}</Suspense>;
}

function RouteErrorFallback(): React.JSX.Element {
  const error = useRouteError();
  const message = error instanceof Error ? error.message : 'Ett oväntat fel inträffade.';
  return (
    <div role="alert" style={{ display: 'flex', flexDirection: 'column', alignItems: 'center', justifyContent: 'center', padding: '2rem', minHeight: '50vh', textAlign: 'center' }}>
      <h2 style={{ fontSize: '1.5rem', marginBottom: '0.5rem' }}>Något gick fel</h2>
      <p style={{ color: '#666', marginBottom: '1.5rem', maxWidth: '480px' }}>{message}</p>
      <button onClick={() => window.location.reload()} style={{ padding: '0.5rem 1.5rem', fontSize: '1rem', cursor: 'pointer', border: '1px solid #ccc', borderRadius: '4px', background: '#fff' }}>
        Försök igen
      </button>
    </div>
  );
}

const router = createBrowserRouter([
  {
    element: <AppShell />,
    errorElement: <RouteErrorFallback />,
    children: [
      { index: true, element: <Navigate to="/competition" replace /> },
      { path: 'competition', element: <LazyPage><CompetitionPage /></LazyPage> },
      { path: 'runners', element: <LazyPage><RunnersPage /></LazyPage> },
      { path: 'teams', element: <LazyPage><TeamsPage /></LazyPage> },
      { path: 'classes', element: <LazyPage><ClassesPage /></LazyPage> },
      { path: 'courses', element: <LazyPage><CoursesPage /></LazyPage> },
      { path: 'controls', element: <LazyPage><ControlsPage /></LazyPage> },
      { path: 'clubs', element: <LazyPage><ClubsPage /></LazyPage> },
      { path: 'si', element: <LazyPage><SiPage /></LazyPage> },
      { path: 'lists', element: <LazyPage><ListsPage /></LazyPage> },
      { path: 'speaker', element: <LazyPage><SpeakerPage /></LazyPage> },
      { path: 'automations', element: <LazyPage><AutomationsPage /></LazyPage> },
      { path: '*', element: <Navigate to="/competition" replace /> },
    ],
  },
]);

function App(): React.JSX.Element {
  return <RouterProvider router={router} />;
}

export default App;
