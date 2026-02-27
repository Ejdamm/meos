import { useState, useMemo, type FormEvent } from 'react';
import {
  useAutomations,
  useCreateAutomation,
  useDeleteAutomation,
} from '../hooks';
import { Modal } from '../components/Modal';
import type { Automation } from '../api';
import './AutomationsPage.css';

const KNOWN_TYPES = [
  'onlineinput',
  'onlineresults',
  'printresult',
  'backup',
  'splits',
  'infoserver',
  'punchtest',
  'prewarning',
  'reconnect',
] as const;

const POLL_INTERVAL = 5000;

type SortField = 'name' | 'type';
type SortDir = 'asc' | 'desc';

function typeLabel(type: string): { label: string; className: string } {
  switch (type) {
    case 'onlineinput':
      return { label: 'Online-inmatning', className: 'auto-type auto-type--online' };
    case 'onlineresults':
      return { label: 'Online-resultat', className: 'auto-type auto-type--online' };
    case 'printresult':
      return { label: 'Skriv ut resultat', className: 'auto-type auto-type--print' };
    case 'backup':
      return { label: 'Säkerhetskopiering', className: 'auto-type auto-type--backup' };
    case 'splits':
      return { label: 'Sträcktider', className: 'auto-type auto-type--splits' };
    case 'infoserver':
      return { label: 'Infoserver', className: 'auto-type auto-type--info' };
    case 'punchtest':
      return { label: 'Stämplingstest', className: 'auto-type auto-type--test' };
    case 'prewarning':
      return { label: 'Förvarning', className: 'auto-type auto-type--warning' };
    case 'reconnect':
      return { label: 'Återanslut', className: 'auto-type auto-type--reconnect' };
    default:
      return { label: type, className: 'auto-type auto-type--unknown' };
  }
}

interface CreateFormData {
  type: string;
  name: string;
}

function emptyForm(): CreateFormData {
  return { type: KNOWN_TYPES[0], name: 'default' };
}

function CreateModal({
  onClose,
  onSubmit,
  isPending,
}: {
  onClose: () => void;
  onSubmit: (data: CreateFormData) => void;
  isPending: boolean;
}) {
  const [form, setForm] = useState<CreateFormData>(emptyForm());

  function handleSubmit(e: FormEvent): void {
    e.preventDefault();
    if (!form.type || !form.name.trim()) return;
    onSubmit(form);
  }

  return (
    <Modal title="Skapa automation" onClose={onClose}>
        <form onSubmit={handleSubmit}>
          <div className="auto-modal__field">
            <label className="auto-modal__label" htmlFor="auto-type">Typ</label>
            <select
              id="auto-type"
              className="auto-modal__select"
              value={form.type}
              onChange={(e) => setForm({ ...form, type: e.target.value })}
            >
              {KNOWN_TYPES.map((t) => (
                <option key={t} value={t}>
                  {typeLabel(t).label}
                </option>
              ))}
            </select>
          </div>
          <div className="auto-modal__field">
            <label className="auto-modal__label" htmlFor="auto-name">Namn</label>
            <input
              id="auto-name"
              className="auto-modal__input"
              type="text"
              value={form.name}
              onChange={(e) => setForm({ ...form, name: e.target.value })}
              placeholder="default"
            />
          </div>
          <div className="auto-modal__actions">
            <button
              type="button"
              className="auto-btn auto-btn--secondary"
              onClick={onClose}
            >
              Avbryt
            </button>
            <button
              type="submit"
              className="auto-btn auto-btn--primary"
              disabled={isPending || !form.name.trim()}
            >
              {isPending ? 'Sparar…' : 'Skapa'}
            </button>
          </div>
        </form>
    </Modal>
  );
}

function AutomationsPage(): React.JSX.Element {
  const { data: automations, isLoading, isError, error, refetch } = useAutomations();
  const createMutation = useCreateAutomation();
  const deleteMutation = useDeleteAutomation();

  const [search, setSearch] = useState('');
  const [sortField, setSortField] = useState<SortField>('type');
  const [sortDir, setSortDir] = useState<SortDir>('asc');
  const [selected, setSelected] = useState<string | null>(null);
  const [showCreate, setShowCreate] = useState(false);

  // Poll for live updates
  const _poll = useAutomations();
  void _poll;

  const items = useMemo(() => automations ?? [], [automations]);

  const filtered = useMemo(() => {
    const q = search.toLowerCase();
    if (!q) return items;
    return items.filter((a) => {
      const tl = typeLabel(a.type);
      if (a.name.toLowerCase().includes(q)) return true;
      if (a.type.toLowerCase().includes(q)) return true;
      if (tl.label.toLowerCase().includes(q)) return true;
      return false;
    });
  }, [items, search]);

  const sorted = useMemo(() => {
    const cmp = (a: Automation, b: Automation): number => {
      let r = 0;
      switch (sortField) {
        case 'name':
          r = a.name.localeCompare(b.name, 'sv');
          break;
        case 'type':
          r = a.type.localeCompare(b.type, 'sv');
          break;
      }
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

  function automationId(a: Automation): string {
    return typeof a.id === 'string' ? (a.id as unknown as string) : `${a.type}/${a.name}`;
  }

  function handleRowClick(a: Automation): void {
    const aid = automationId(a);
    setSelected(selected === aid ? null : aid);
  }

  function handleCreate(data: CreateFormData): void {
    createMutation.mutate(
      { type: data.type, name: data.name, enabled: true },
      {
        onSuccess: () => setShowCreate(false),
      }
    );
  }

  function handleDelete(): void {
    if (selected == null) return;
    const auto = items.find((a) => automationId(a) === selected);
    if (!auto) return;
    // The API expects the id; cast to number for the hook but the actual HTTP
    // call will stringify it into the URL which works for composite string ids.
    deleteMutation.mutate(auto.id, {
      onSuccess: () => setSelected(null),
    });
  }

  if (isLoading) {
    return <div className="auto-page"><p aria-live="polite">Laddar automationer…</p></div>;
  }

  if (isError) {
    return (
      <div className="auto-page">
        <p className="auto-error">
          Kunde inte ladda automationer: {(error as Error)?.message ?? 'Okänt fel'}
        </p>
        <button className="auto-btn auto-btn--primary" onClick={() => void refetch()}>
          Försök igen
        </button>
      </div>
    );
  }

  const selectedAuto = selected
    ? items.find((a) => automationId(a) === selected)
    : null;

  return (
    <div className="auto-page">
      <div className="auto-page__header">
        <h2>Automatik</h2>
        <div className="auto-page__actions">
          <span className="auto-page__poll-label">
            <span className="auto-page__poll-dot" />
            Polling {POLL_INTERVAL / 1000}s
          </span>
          <button
            className="auto-btn auto-btn--primary"
            onClick={() => setShowCreate(true)}
          >
            + Ny automation
          </button>
          {selectedAuto && (
            <button
              className="auto-btn auto-btn--danger"
              onClick={handleDelete}
              disabled={deleteMutation.isPending}
            >
              {deleteMutation.isPending ? 'Tar bort…' : 'Ta bort'}
            </button>
          )}
        </div>
      </div>

      <div className="auto-toolbar">
        <input
          className="auto-toolbar__search"
          type="text"
          placeholder="Sök automation…"
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          aria-label="Sök automationer"
        />
      </div>

      <div className="auto-table-wrap">
        <table className="auto-table">
          <thead>
            <tr>
              <th onClick={() => handleSort('type')}>
                Typ<span className="sort-indicator">{sortIndicator('type')}</span>
              </th>
              <th onClick={() => handleSort('name')}>
                Namn<span className="sort-indicator">{sortIndicator('name')}</span>
              </th>
              <th>Status</th>
            </tr>
          </thead>
          <tbody>
            {sorted.length === 0 ? (
              <tr>
                <td colSpan={3} className="auto-table__empty">
                  {items.length > 0
                    ? 'Inga automationer matchar sökningen.'
                    : 'Inga automationer konfigurerade.'}
                </td>
              </tr>
            ) : (
              sorted.map((a) => {
                const aid = automationId(a);
                const tl = typeLabel(a.type);
                const isSelected = selected === aid;
                return (
                  <tr
                    key={aid}
                    className={isSelected ? 'auto-table__row--selected' : ''}
                    onClick={() => handleRowClick(a)}
                  >
                    <td>
                      <span className={tl.className}>{tl.label}</span>
                    </td>
                    <td>{a.name}</td>
                    <td>
                      <span className="auto-status auto-status--configured">
                        Konfigurerad
                      </span>
                    </td>
                  </tr>
                );
              })
            )}
          </tbody>
        </table>
      </div>

      <p className="auto-count" role="status">
        {sorted.length} av {items.length} automationer
      </p>

      {(createMutation.isError || deleteMutation.isError) && (
        <p className="auto-error">
          {createMutation.isError
            ? `Fel vid skapande: ${createMutation.error.message}`
            : `Fel vid borttagning: ${deleteMutation.error.message}`}
        </p>
      )}

      {showCreate && (
        <CreateModal
          onClose={() => setShowCreate(false)}
          onSubmit={handleCreate}
          isPending={createMutation.isPending}
        />
      )}
    </div>
  );
}

export default AutomationsPage;
