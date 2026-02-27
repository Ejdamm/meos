import { Routes, Route, Navigate } from 'react-router-dom';
import { AppShell } from './components/layout';

function Placeholder({ title }: { title: string }): React.JSX.Element {
  return <h2>{title}</h2>;
}

function App(): React.JSX.Element {
  return (
    <Routes>
      <Route element={<AppShell />}>
        <Route index element={<Navigate to="/competition" replace />} />
        <Route path="competition" element={<Placeholder title="Tävling" />} />
        <Route path="runners" element={<Placeholder title="Deltagare" />} />
        <Route path="teams" element={<Placeholder title="Lag" />} />
        <Route path="classes" element={<Placeholder title="Klasser" />} />
        <Route path="courses" element={<Placeholder title="Banor" />} />
        <Route path="controls" element={<Placeholder title="Kontroller" />} />
        <Route path="clubs" element={<Placeholder title="Klubbar" />} />
        <Route path="si" element={<Placeholder title="SI" />} />
        <Route path="lists" element={<Placeholder title="Listor" />} />
        <Route path="speaker" element={<Placeholder title="Speaker" />} />
        <Route path="automations" element={<Placeholder title="Automatik" />} />
        <Route path="*" element={<Navigate to="/competition" replace />} />
      </Route>
    </Routes>
  );
}

export default App
