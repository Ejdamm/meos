import { useState, useMemo, type FormEvent } from 'react';
import {
  useRunners,
  useCreateRunner,
  useUpdateRunner,
  useDeleteRunner,
  useClasses,
  useClubs,
} from '../hooks';
import { Modal } from '../components/Modal';
import type { Runner } from '../types';
import './RunnersPage.css';

type SortField = 'name' | 'class' | 'club' | 'bib' | 'cardNo' | 'status';
type SortDir = 'asc' | 'desc';

const STATUS_LABELS: Record<number, string> = {
  0: 'Ej start',
  1: 'OK',
  2: 'DNS',
  3: 'DNF',
  4: 'DSQ',
  5: 'Max-tid',
  20: 'Oplacerad',
};

function statusLabel(code: number): string {
  return STATUS_LABELS[code] ?? `Status ${code}`;
}

function formatTime(seconds: number): string {
  if (seconds <= 0) return '—';
  const h = Math.floor(seconds / 3600);
  const m = Math.floor((seconds % 3600) / 60);
  const s = seconds % 60;
  if (h > 0) return `${h}:${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;
  return `${m}:${String(s).padStart(2, '0')}`;
}

interface RunnerFormData {
  name: string;
  classId: number;
  clubId: number;
  cardNo: number;
  bib: string;
  birthYear: number;
  nationality: string;
}

function emptyForm(): RunnerFormData {
  return { name: '', classId: 0, clubId: 0, cardNo: 0, bib: '', birthYear: 0, nationality: '' };
}

function formFromRunner(r: Runner): RunnerFormData {
  return {
    name: r.name,
    classId: r.classId,
    clubId: r.clubId,
    cardNo: r.cardNo,
    bib: r.bib,
    birthYear: r.birthYear,
    nationality: r.nationality,
  };
}

function RunnerFormModal({
  title,
  initial,
  onClose,
  onSubmit,
  isPending,
  error,
  classes,
  clubs,
}: {
  title: string;
  initial: RunnerFormData;
  onClose: () => void;
  onSubmit: (data: RunnerFormData) => void;
  isPending: boolean;
  error: string | null;
  classes: { id: number; name: string }[];
  clubs: { id: number; name: string }[];
}): React.JSX.Element {
  const [form, setForm] = useState<RunnerFormData>(initial);
  const [nameError, setNameError] = useState('');

  function handleSubmit(e: FormEvent): void {
    e.preventDefault();
    if (!form.name.trim()) {
      setNameError('Namn krävs');
      return;
    }
    setNameError('');
    onSubmit(form);
  }

  return (
    <Modal title={title} onClose={onClose}>
        <form onSubmit={handleSubmit}>
          <div className="runners-form__field">
            <label className="runners-form__label" htmlFor="runner-name">Namn</label>
            <input
              id="runner-name"
              className={`runners-form__input ${nameError ? 'runners-form__input--error' : ''}`}
              type="text"
              value={form.name}
              onChange={(e) => setForm({ ...form, name: e.target.value })}
              aria-invalid={nameError ? true : undefined}
              aria-describedby={nameError ? 'runner-name-error' : undefined}
            />
            {nameError && <span id="runner-name-error" className="runners-form__error" role="alert">{nameError}</span>}
          </div>
          <div className="runners-form__field">
            <label className="runners-form__label" htmlFor="runner-class">Klass</label>
            <select
              id="runner-class"
              className="runners-form__input"
              value={form.classId}
              onChange={(e) => setForm({ ...form, classId: Number(e.target.value) })}
            >
              <option value={0}>— Välj klass —</option>
              {classes.map((c) => (
                <option key={c.id} value={c.id}>{c.name}</option>
              ))}
            </select>
          </div>
          <div className="runners-form__field">
            <label className="runners-form__label" htmlFor="runner-club">Klubb</label>
            <select
              id="runner-club"
              className="runners-form__input"
              value={form.clubId}
              onChange={(e) => setForm({ ...form, clubId: Number(e.target.value) })}
            >
              <option value={0}>— Välj klubb —</option>
              {clubs.map((c) => (
                <option key={c.id} value={c.id}>{c.name}</option>
              ))}
            </select>
          </div>
          <div className="runners-form__field">
            <label className="runners-form__label" htmlFor="runner-card">Bricknummer</label>
            <input
              id="runner-card"
              className="runners-form__input"
              type="number"
              min={0}
              value={form.cardNo || ''}
              onChange={(e) => setForm({ ...form, cardNo: Number(e.target.value) })}
            />
          </div>
          <div className="runners-form__field">
            <label className="runners-form__label" htmlFor="runner-bib">Nummerlapp</label>
            <input
              id="runner-bib"
              className="runners-form__input"
              type="text"
              value={form.bib}
              onChange={(e) => setForm({ ...form, bib: e.target.value })}
            />
          </div>
          <div className="runners-form__field">
            <label className="runners-form__label" htmlFor="runner-birth">Födelseår</label>
            <input
              id="runner-birth"
              className="runners-form__input"
              type="number"
              min={0}
              value={form.birthYear || ''}
              onChange={(e) => setForm({ ...form, birthYear: Number(e.target.value) })}
            />
          </div>
          <div className="runners-form__field">
            <label className="runners-form__label" htmlFor="runner-nat">Nationalitet</label>
            <input
              id="runner-nat"
              className="runners-form__input"
              type="text"
              value={form.nationality}
              onChange={(e) => setForm({ ...form, nationality: e.target.value })}
            />
          </div>
          <div className="runners-form__actions">
            <button type="submit" className="runners-btn runners-btn--primary" disabled={isPending}>
              {isPending ? 'Sparar…' : 'Spara'}
            </button>
            <button type="button" className="runners-btn runners-btn--secondary" onClick={onClose}>
              Avbryt
            </button>
          </div>
          {error && <p className="runners-form__error" role="alert">{error}</p>}
        </form>
    </Modal>
  );
}

function RunnersPage(): React.JSX.Element {
  const { data: runners, isLoading, isError, error } = useRunners();
  const { data: classes } = useClasses();
  const { data: clubs } = useClubs();
  const createMutation = useCreateRunner();
  const updateMutation = useUpdateRunner();
  const deleteMutation = useDeleteRunner();

  const [search, setSearch] = useState('');
  const [filterClass, setFilterClass] = useState('');
  const [filterClub, setFilterClub] = useState('');
  const [sortField, setSortField] = useState<SortField>('name');
  const [sortDir, setSortDir] = useState<SortDir>('asc');
  const [selectedId, setSelectedId] = useState<number | null>(null);
  const [showCreate, setShowCreate] = useState(false);
  const [editRunner, setEditRunner] = useState<Runner | null>(null);

  const classList = useMemo(() => (classes ?? []).map((c) => ({ id: c.id, name: c.name })), [classes]);
  const clubList = useMemo(() => (clubs ?? []).map((c) => ({ id: c.id, name: c.name })), [clubs]);

  // Unique class/club values from runners for filter dropdowns
  const uniqueClasses = useMemo(() => {
    if (!runners) return [];
    const seen = new Map<string, boolean>();
    return runners
      .filter((r) => {
        if (seen.has(r.class)) return false;
        seen.set(r.class, true);
        return true;
      })
      .map((r) => r.class)
      .sort();
  }, [runners]);

  const uniqueClubs = useMemo(() => {
    if (!runners) return [];
    const seen = new Map<string, boolean>();
    return runners
      .filter((r) => {
        if (seen.has(r.club)) return false;
        seen.set(r.club, true);
        return true;
      })
      .map((r) => r.club)
      .sort();
  }, [runners]);

  const filtered = useMemo(() => {
    if (!runners) return [];
    const q = search.toLowerCase();
    return runners.filter((r) => {
      if (q && !r.name.toLowerCase().includes(q) && !r.bib.toLowerCase().includes(q)) return false;
      if (filterClass && r.class !== filterClass) return false;
      if (filterClub && r.club !== filterClub) return false;
      return true;
    });
  }, [runners, search, filterClass, filterClub]);

  const sorted = useMemo(() => {
    const cmp = (a: Runner, b: Runner): number => {
      let va: string | number;
      let vb: string | number;
      switch (sortField) {
        case 'name': va = a.name; vb = b.name; break;
        case 'class': va = a.class; vb = b.class; break;
        case 'club': va = a.club; vb = b.club; break;
        case 'bib': va = a.bib; vb = b.bib; break;
        case 'cardNo': va = a.cardNo; vb = b.cardNo; break;
        case 'status': va = a.status; vb = b.status; break;
      }
      if (typeof va === 'string') {
        const r = va.localeCompare(vb as string, 'sv');
        return sortDir === 'asc' ? r : -r;
      }
      const r = (va as number) - (vb as number);
      return sortDir === 'asc' ? r : -r;
    };
    return [...filtered].sort(cmp);
  }, [filtered, sortField, sortDir]);

  function handleSort(field: SortField): void {
    if (sortField === field) {
      setSortDir(sortDir === 'asc' ? 'desc' : 'asc');
    } else {
      setSortField(field);
      setSortDir('asc');
    }
  }

  function sortIndicator(field: SortField): string {
    if (sortField !== field) return '';
    return sortDir === 'asc' ? ' ▲' : ' ▼';
  }

  function handleCreate(data: RunnerFormData): void {
    createMutation.mutate(data as Omit<Runner, 'id'>, {
      onSuccess: () => setShowCreate(false),
    });
  }

  function handleUpdate(data: RunnerFormData): void {
    if (!editRunner) return;
    updateMutation.mutate(
      { id: editRunner.id, runner: data },
      { onSuccess: () => setEditRunner(null) }
    );
  }

  function handleDelete(id: number): void {
    deleteMutation.mutate(id, {
      onSuccess: () => {
        if (selectedId === id) setSelectedId(null);
        if (editRunner?.id === id) setEditRunner(null);
      },
    });
  }

  if (isLoading) {
    return <div className="runners-page"><p aria-live="polite">Laddar deltagare…</p></div>;
  }

  if (isError) {
    return (
      <div className="runners-page">
        <p className="runners-form__error">Kunde inte ladda deltagare: {error.message}</p>
      </div>
    );
  }

  const selectedRunner = selectedId != null ? runners?.find((r) => r.id === selectedId) : null;

  return (
    <div className="runners-page">
      <div className="runners-page__header">
        <h2>Deltagare</h2>
        <div className="runners-page__actions">
          <button className="runners-btn runners-btn--primary" onClick={() => setShowCreate(true)}>
            Ny deltagare
          </button>
          {selectedRunner && (
            <>
              <button
                className="runners-btn runners-btn--secondary"
                onClick={() => setEditRunner(selectedRunner)}
              >
                Redigera
              </button>
              <button
                className="runners-btn runners-btn--danger runners-btn--sm"
                onClick={() => handleDelete(selectedRunner.id)}
                disabled={deleteMutation.isPending}
              >
                Ta bort
              </button>
            </>
          )}
        </div>
      </div>

      <div className="runners-toolbar">
        <input
          className="runners-toolbar__search"
          type="text"
          placeholder="Sök namn eller nummerlapp…"
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          aria-label="Sök deltagare"
        />
        <select
          className="runners-toolbar__select"
          value={filterClass}
          onChange={(e) => setFilterClass(e.target.value)}
          aria-label="Filtrera klass"
        >
          <option value="">Alla klasser</option>
          {uniqueClasses.map((c) => (
            <option key={c} value={c}>{c}</option>
          ))}
        </select>
        <select
          className="runners-toolbar__select"
          value={filterClub}
          onChange={(e) => setFilterClub(e.target.value)}
          aria-label="Filtrera klubb"
        >
          <option value="">Alla klubbar</option>
          {uniqueClubs.map((c) => (
            <option key={c} value={c}>{c}</option>
          ))}
        </select>
      </div>

      <div className="runners-table-wrap">
        <table className="runners-table">
          <thead>
            <tr>
              <th onClick={() => handleSort('bib')}>
                Nr<span className="sort-indicator">{sortIndicator('bib')}</span>
              </th>
              <th onClick={() => handleSort('name')}>
                Namn<span className="sort-indicator">{sortIndicator('name')}</span>
              </th>
              <th onClick={() => handleSort('class')}>
                Klass<span className="sort-indicator">{sortIndicator('class')}</span>
              </th>
              <th onClick={() => handleSort('club')}>
                Klubb<span className="sort-indicator">{sortIndicator('club')}</span>
              </th>
              <th onClick={() => handleSort('cardNo')}>
                Bricka<span className="sort-indicator">{sortIndicator('cardNo')}</span>
              </th>
              <th>Tid</th>
              <th onClick={() => handleSort('status')}>
                Status<span className="sort-indicator">{sortIndicator('status')}</span>
              </th>
            </tr>
          </thead>
          <tbody>
            {sorted.length === 0 ? (
              <tr>
                <td colSpan={7} className="runners-table__empty">
                  {runners && runners.length > 0
                    ? 'Inga deltagare matchar filtret.'
                    : 'Inga deltagare registrerade.'}
                </td>
              </tr>
            ) : (
              sorted.map((r) => (
                <tr
                  key={r.id}
                  className={selectedId === r.id ? 'runners-table__row--selected' : ''}
                  onClick={() => setSelectedId(selectedId === r.id ? null : r.id)}
                >
                  <td>{r.bib || '—'}</td>
                  <td>{r.name}</td>
                  <td>{r.class || '—'}</td>
                  <td>{r.club || '—'}</td>
                  <td>{r.cardNo || '—'}</td>
                  <td>{formatTime(r.runningTime)}</td>
                  <td>{statusLabel(r.status)}</td>
                </tr>
              ))
            )}
          </tbody>
        </table>
      </div>

      <p className="runners-count" role="status">
        {sorted.length} av {runners?.length ?? 0} deltagare
      </p>

      {showCreate && (
        <RunnerFormModal
          title="Ny deltagare"
          initial={emptyForm()}
          onClose={() => { setShowCreate(false); createMutation.reset(); }}
          onSubmit={handleCreate}
          isPending={createMutation.isPending}
          error={createMutation.isError ? createMutation.error.message : null}
          classes={classList}
          clubs={clubList}
        />
      )}

      {editRunner && (
        <RunnerFormModal
          title="Redigera deltagare"
          initial={formFromRunner(editRunner)}
          onClose={() => { setEditRunner(null); updateMutation.reset(); }}
          onSubmit={handleUpdate}
          isPending={updateMutation.isPending}
          error={updateMutation.isError ? updateMutation.error.message : null}
          classes={classList}
          clubs={clubList}
        />
      )}
    </div>
  );
}

export default RunnersPage;
