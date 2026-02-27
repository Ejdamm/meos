import { useState, useMemo, type FormEvent } from 'react';
import {
  useClubs,
  useCreateClub,
  useUpdateClub,
  useDeleteClub,
} from '../hooks';
import type { Club } from '../types';
import './ClubsPage.css';

type SortField = 'name' | 'country';
type SortDir = 'asc' | 'desc';

interface ClubFormData {
  name: string;
  country: string;
}

function emptyForm(): ClubFormData {
  return { name: '', country: '' };
}

function formFromClub(c: Club): ClubFormData {
  return { name: c.name, country: c.country ?? '' };
}

function ClubFormModal({
  title,
  initial,
  onClose,
  onSubmit,
  isPending,
  error,
}: {
  title: string;
  initial: ClubFormData;
  onClose: () => void;
  onSubmit: (data: ClubFormData) => void;
  isPending: boolean;
  error: string | null;
}): React.JSX.Element {
  const [form, setForm] = useState<ClubFormData>(initial);
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
    <div className="clubs-modal-overlay" onClick={onClose} role="dialog" aria-modal="true" aria-label={title}>
      <div className="clubs-modal" onClick={(e) => e.stopPropagation()}>
        <h3 className="clubs-modal__title">{title}</h3>
        <form onSubmit={handleSubmit}>
          <div className="clubs-form__field">
            <label className="clubs-form__label" htmlFor="club-name">Namn</label>
            <input
              id="club-name"
              className={`clubs-form__input ${nameError ? 'clubs-form__input--error' : ''}`}
              type="text"
              value={form.name}
              onChange={(e) => setForm({ ...form, name: e.target.value })}
              autoFocus
            />
            {nameError && <span className="clubs-form__error">{nameError}</span>}
          </div>
          <div className="clubs-form__field">
            <label className="clubs-form__label" htmlFor="club-country">Nationalitet</label>
            <input
              id="club-country"
              className="clubs-form__input"
              type="text"
              value={form.country}
              onChange={(e) => setForm({ ...form, country: e.target.value })}
              placeholder="t.ex. SWE"
            />
          </div>
          <div className="clubs-form__actions">
            <button type="submit" className="clubs-btn clubs-btn--primary" disabled={isPending}>
              {isPending ? 'Sparar…' : 'Spara'}
            </button>
            <button type="button" className="clubs-btn clubs-btn--secondary" onClick={onClose}>
              Avbryt
            </button>
          </div>
          {error && <p className="clubs-form__error">{error}</p>}
        </form>
      </div>
    </div>
  );
}

function ClubsPage(): React.JSX.Element {
  const { data: clubs, isLoading, isError, error } = useClubs();
  const createMutation = useCreateClub();
  const updateMutation = useUpdateClub();
  const deleteMutation = useDeleteClub();

  const [search, setSearch] = useState('');
  const [sortField, setSortField] = useState<SortField>('name');
  const [sortDir, setSortDir] = useState<SortDir>('asc');
  const [selectedId, setSelectedId] = useState<number | null>(null);
  const [showCreate, setShowCreate] = useState(false);
  const [editClub, setEditClub] = useState<Club | null>(null);

  const filtered = useMemo(() => {
    if (!clubs) return [];
    const q = search.toLowerCase();
    return clubs.filter((c) => {
      if (q && !c.name.toLowerCase().includes(q) && !(c.country ?? '').toLowerCase().includes(q))
        return false;
      return true;
    });
  }, [clubs, search]);

  const sorted = useMemo(() => {
    const cmp = (a: Club, b: Club): number => {
      let va: string;
      let vb: string;
      switch (sortField) {
        case 'name': va = a.name; vb = b.name; break;
        case 'country': va = a.country ?? ''; vb = b.country ?? ''; break;
      }
      const r = va.localeCompare(vb, 'sv');
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

  function handleCreate(data: ClubFormData): void {
    createMutation.mutate(
      { name: data.name, country: data.country || undefined },
      { onSuccess: () => setShowCreate(false) },
    );
  }

  function handleUpdate(data: ClubFormData): void {
    if (!editClub) return;
    updateMutation.mutate(
      { id: editClub.id, club: { name: data.name, country: data.country || undefined } },
      { onSuccess: () => setEditClub(null) },
    );
  }

  function handleDelete(id: number): void {
    deleteMutation.mutate(id, {
      onSuccess: () => {
        if (selectedId === id) setSelectedId(null);
        if (editClub?.id === id) setEditClub(null);
      },
    });
  }

  if (isLoading) {
    return <div className="clubs-page"><p>Laddar klubbar…</p></div>;
  }

  if (isError) {
    return (
      <div className="clubs-page">
        <p className="clubs-form__error">Kunde inte ladda klubbar: {error.message}</p>
      </div>
    );
  }

  const selectedClub = selectedId != null ? clubs?.find((c) => c.id === selectedId) : null;

  return (
    <div className="clubs-page">
      <div className="clubs-page__header">
        <h2>Klubbar</h2>
        <div className="clubs-page__actions">
          <button className="clubs-btn clubs-btn--primary" onClick={() => setShowCreate(true)}>
            Ny klubb
          </button>
          {selectedClub && (
            <>
              <button
                className="clubs-btn clubs-btn--secondary"
                onClick={() => setEditClub(selectedClub)}
              >
                Redigera
              </button>
              <button
                className="clubs-btn clubs-btn--danger clubs-btn--sm"
                onClick={() => handleDelete(selectedClub.id)}
                disabled={deleteMutation.isPending}
              >
                Ta bort
              </button>
            </>
          )}
        </div>
      </div>

      <div className="clubs-toolbar">
        <input
          className="clubs-toolbar__search"
          type="text"
          placeholder="Sök klubb…"
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          aria-label="Sök klubb"
        />
      </div>

      <div className="clubs-table-wrap">
        <table className="clubs-table">
          <thead>
            <tr>
              <th onClick={() => handleSort('name')}>
                Namn<span className="sort-indicator">{sortIndicator('name')}</span>
              </th>
              <th onClick={() => handleSort('country')}>
                Nationalitet<span className="sort-indicator">{sortIndicator('country')}</span>
              </th>
            </tr>
          </thead>
          <tbody>
            {sorted.length === 0 ? (
              <tr>
                <td colSpan={2} className="clubs-table__empty">
                  {clubs && clubs.length > 0
                    ? 'Inga klubbar matchar filtret.'
                    : 'Inga klubbar registrerade.'}
                </td>
              </tr>
            ) : (
              sorted.map((c) => (
                <tr
                  key={c.id}
                  className={selectedId === c.id ? 'clubs-table__row--selected' : ''}
                  onClick={() => setSelectedId(selectedId === c.id ? null : c.id)}
                >
                  <td>{c.name}</td>
                  <td>{c.country || '—'}</td>
                </tr>
              ))
            )}
          </tbody>
        </table>
      </div>

      <p className="clubs-count">
        {sorted.length} av {clubs?.length ?? 0} klubbar
      </p>

      {showCreate && (
        <ClubFormModal
          title="Ny klubb"
          initial={emptyForm()}
          onClose={() => { setShowCreate(false); createMutation.reset(); }}
          onSubmit={handleCreate}
          isPending={createMutation.isPending}
          error={createMutation.isError ? createMutation.error.message : null}
        />
      )}

      {editClub && (
        <ClubFormModal
          title="Redigera klubb"
          initial={formFromClub(editClub)}
          onClose={() => { setEditClub(null); updateMutation.reset(); }}
          onSubmit={handleUpdate}
          isPending={updateMutation.isPending}
          error={updateMutation.isError ? updateMutation.error.message : null}
        />
      )}
    </div>
  );
}

export default ClubsPage;
