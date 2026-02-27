import { useState, useMemo, type FormEvent } from 'react';
import {
  useControls,
  useCreateControl,
  useUpdateControl,
  useDeleteControl,
} from '../hooks';
import type { Control } from '../types';
import './ControlsPage.css';

type SortField = 'name' | 'status' | 'codes' | 'rogainingPoints';
type SortDir = 'asc' | 'desc';

interface ControlFormData {
  name: string;
  codes: string;
  rogainingPoints: number;
}

function emptyForm(): ControlFormData {
  return { name: '', codes: '', rogainingPoints: 0 };
}

function formFromControl(c: Control): ControlFormData {
  return {
    name: c.name,
    codes: c.codes.join(', '),
    rogainingPoints: c.rogainingPoints ?? 0,
  };
}

function parseCodes(raw: string): number[] {
  return raw
    .split(/[,;\s]+/)
    .map((s) => Number(s.trim()))
    .filter((n) => !Number.isNaN(n) && n > 0);
}

const STATUS_LABELS: Record<number, string> = {
  0: 'Normal',
  1: 'Start',
  2: 'Mål',
  3: 'Korsning',
};

function statusLabel(status: number): string {
  return STATUS_LABELS[status] ?? `Typ ${status}`;
}

function ControlFormModal({
  title,
  initial,
  onClose,
  onSubmit,
  isPending,
  error,
}: {
  title: string;
  initial: ControlFormData;
  onClose: () => void;
  onSubmit: (data: ControlFormData) => void;
  isPending: boolean;
  error: string | null;
}): React.JSX.Element {
  const [form, setForm] = useState<ControlFormData>(initial);
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
    <div className="controls-modal-overlay" onClick={onClose} role="dialog" aria-modal="true" aria-label={title}>
      <div className="controls-modal" onClick={(e) => e.stopPropagation()}>
        <h3 className="controls-modal__title">{title}</h3>
        <form onSubmit={handleSubmit}>
          <div className="controls-form__field">
            <label className="controls-form__label" htmlFor="control-name">Namn</label>
            <input
              id="control-name"
              className={`controls-form__input ${nameError ? 'controls-form__input--error' : ''}`}
              type="text"
              value={form.name}
              onChange={(e) => setForm({ ...form, name: e.target.value })}
              autoFocus
            />
            {nameError && <span className="controls-form__error">{nameError}</span>}
          </div>
          <div className="controls-form__field">
            <label className="controls-form__label" htmlFor="control-codes">Kontrollkoder (kommaseparerade)</label>
            <input
              id="control-codes"
              className="controls-form__input"
              type="text"
              value={form.codes}
              onChange={(e) => setForm({ ...form, codes: e.target.value })}
              placeholder="31, 32, 33"
            />
          </div>
          <div className="controls-form__field">
            <label className="controls-form__label" htmlFor="control-rogaining">Rogaining-poäng</label>
            <input
              id="control-rogaining"
              className="controls-form__input"
              type="number"
              min={0}
              value={form.rogainingPoints}
              onChange={(e) => setForm({ ...form, rogainingPoints: Math.max(0, Number(e.target.value)) })}
            />
          </div>
          <div className="controls-form__actions">
            <button type="submit" className="controls-btn controls-btn--primary" disabled={isPending}>
              {isPending ? 'Sparar…' : 'Spara'}
            </button>
            <button type="button" className="controls-btn controls-btn--secondary" onClick={onClose}>
              Avbryt
            </button>
          </div>
          {error && <p className="controls-form__error">{error}</p>}
        </form>
      </div>
    </div>
  );
}

function ControlsPage(): React.JSX.Element {
  const { data: controls, isLoading, isError, error } = useControls();
  const createMutation = useCreateControl();
  const updateMutation = useUpdateControl();
  const deleteMutation = useDeleteControl();

  const [search, setSearch] = useState('');
  const [sortField, setSortField] = useState<SortField>('name');
  const [sortDir, setSortDir] = useState<SortDir>('asc');
  const [selectedId, setSelectedId] = useState<number | null>(null);
  const [showCreate, setShowCreate] = useState(false);
  const [editControl, setEditControl] = useState<Control | null>(null);

  const filtered = useMemo(() => {
    if (!controls) return [];
    const q = search.toLowerCase();
    return controls.filter((c) => {
      if (q && !c.name.toLowerCase().includes(q) && !c.codes.some((code) => String(code).includes(q)))
        return false;
      return true;
    });
  }, [controls, search]);

  const sorted = useMemo(() => {
    const cmp = (a: Control, b: Control): number => {
      let va: string | number;
      let vb: string | number;
      switch (sortField) {
        case 'name': va = a.name; vb = b.name; break;
        case 'status': va = a.status; vb = b.status; break;
        case 'codes': va = a.codes.length; vb = b.codes.length; break;
        case 'rogainingPoints': va = a.rogainingPoints ?? 0; vb = b.rogainingPoints ?? 0; break;
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

  function handleCreate(data: ControlFormData): void {
    createMutation.mutate(
      { name: data.name, codes: parseCodes(data.codes), status: 0, rogainingPoints: data.rogainingPoints || undefined },
      { onSuccess: () => setShowCreate(false) },
    );
  }

  function handleUpdate(data: ControlFormData): void {
    if (!editControl) return;
    updateMutation.mutate(
      { id: editControl.id, control: { name: data.name, codes: parseCodes(data.codes), rogainingPoints: data.rogainingPoints || undefined } },
      { onSuccess: () => setEditControl(null) },
    );
  }

  function handleDelete(id: number): void {
    deleteMutation.mutate(id, {
      onSuccess: () => {
        if (selectedId === id) setSelectedId(null);
        if (editControl?.id === id) setEditControl(null);
      },
    });
  }

  if (isLoading) {
    return <div className="controls-page"><p>Laddar kontroller…</p></div>;
  }

  if (isError) {
    return (
      <div className="controls-page">
        <p className="controls-form__error">Kunde inte ladda kontroller: {error.message}</p>
      </div>
    );
  }

  const selectedControl = selectedId != null ? controls?.find((c) => c.id === selectedId) : null;

  return (
    <div className="controls-page">
      <div className="controls-page__header">
        <h2>Kontroller</h2>
        <div className="controls-page__actions">
          <button className="controls-btn controls-btn--primary" onClick={() => setShowCreate(true)}>
            Ny kontroll
          </button>
          {selectedControl && (
            <>
              <button
                className="controls-btn controls-btn--secondary"
                onClick={() => setEditControl(selectedControl)}
              >
                Redigera
              </button>
              <button
                className="controls-btn controls-btn--danger controls-btn--sm"
                onClick={() => handleDelete(selectedControl.id)}
                disabled={deleteMutation.isPending}
              >
                Ta bort
              </button>
            </>
          )}
        </div>
      </div>

      <div className="controls-toolbar">
        <input
          className="controls-toolbar__search"
          type="text"
          placeholder="Sök kontroll…"
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          aria-label="Sök kontroll"
        />
      </div>

      <div className="controls-table-wrap">
        <table className="controls-table">
          <thead>
            <tr>
              <th onClick={() => handleSort('name')}>
                Namn<span className="sort-indicator">{sortIndicator('name')}</span>
              </th>
              <th onClick={() => handleSort('status')}>
                Typ<span className="sort-indicator">{sortIndicator('status')}</span>
              </th>
              <th onClick={() => handleSort('codes')}>
                Koder<span className="sort-indicator">{sortIndicator('codes')}</span>
              </th>
              <th onClick={() => handleSort('rogainingPoints')}>
                Rogaining<span className="sort-indicator">{sortIndicator('rogainingPoints')}</span>
              </th>
            </tr>
          </thead>
          <tbody>
            {sorted.length === 0 ? (
              <tr>
                <td colSpan={4} className="controls-table__empty">
                  {controls && controls.length > 0
                    ? 'Inga kontroller matchar filtret.'
                    : 'Inga kontroller registrerade.'}
                </td>
              </tr>
            ) : (
              sorted.map((c) => (
                <tr
                  key={c.id}
                  className={selectedId === c.id ? 'controls-table__row--selected' : ''}
                  onClick={() => setSelectedId(selectedId === c.id ? null : c.id)}
                >
                  <td>{c.name}</td>
                  <td>{statusLabel(c.status)}</td>
                  <td>{c.codes.join(', ') || '—'}</td>
                  <td>{c.rogainingPoints ?? '—'}</td>
                </tr>
              ))
            )}
          </tbody>
        </table>
      </div>

      <p className="controls-count">
        {sorted.length} av {controls?.length ?? 0} kontroller
      </p>

      {showCreate && (
        <ControlFormModal
          title="Ny kontroll"
          initial={emptyForm()}
          onClose={() => { setShowCreate(false); createMutation.reset(); }}
          onSubmit={handleCreate}
          isPending={createMutation.isPending}
          error={createMutation.isError ? createMutation.error.message : null}
        />
      )}

      {editControl && (
        <ControlFormModal
          title="Redigera kontroll"
          initial={formFromControl(editControl)}
          onClose={() => { setEditControl(null); updateMutation.reset(); }}
          onSubmit={handleUpdate}
          isPending={updateMutation.isPending}
          error={updateMutation.isError ? updateMutation.error.message : null}
        />
      )}
    </div>
  );
}

export default ControlsPage;
