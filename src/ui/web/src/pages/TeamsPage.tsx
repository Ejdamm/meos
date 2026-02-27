import { useState, useMemo, type FormEvent } from 'react';
import {
  useTeams,
  useCreateTeam,
  useUpdateTeam,
  useDeleteTeam,
  useClasses,
  useClubs,
  useRunners,
} from '../hooks';
import type { Team, LegResult } from '../types';
import './TeamsPage.css';

type SortField = 'name' | 'class' | 'club' | 'bib' | 'status';
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

interface TeamFormData {
  name: string;
  classId: number;
  clubId: number;
  bib: string;
  runners: number[];
}

function emptyForm(): TeamFormData {
  return { name: '', classId: 0, clubId: 0, bib: '', runners: [] };
}

function formFromTeam(t: Team): TeamFormData {
  return {
    name: t.name,
    classId: t.classId,
    clubId: t.clubId,
    bib: t.bib,
    runners: [...t.runners],
  };
}

function TeamFormModal({
  title,
  initial,
  onClose,
  onSubmit,
  isPending,
  error,
  classes,
  clubs,
  availableRunners,
}: {
  title: string;
  initial: TeamFormData;
  onClose: () => void;
  onSubmit: (data: TeamFormData) => void;
  isPending: boolean;
  error: string | null;
  classes: { id: number; name: string }[];
  clubs: { id: number; name: string }[];
  availableRunners: { id: number; name: string }[];
}): React.JSX.Element {
  const [form, setForm] = useState<TeamFormData>(initial);
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

  function setMember(index: number, runnerId: number): void {
    const next = [...form.runners];
    next[index] = runnerId;
    setForm({ ...form, runners: next });
  }

  function addMemberSlot(): void {
    setForm({ ...form, runners: [...form.runners, 0] });
  }

  function removeMember(index: number): void {
    const next = form.runners.filter((_, i) => i !== index);
    setForm({ ...form, runners: next });
  }

  return (
    <div className="teams-modal-overlay" onClick={onClose} role="dialog" aria-modal="true" aria-label={title}>
      <div className="teams-modal" onClick={(e) => e.stopPropagation()}>
        <h3 className="teams-modal__title">{title}</h3>
        <form onSubmit={handleSubmit}>
          <div className="teams-form__field">
            <label className="teams-form__label" htmlFor="team-name">Namn</label>
            <input
              id="team-name"
              className={`teams-form__input ${nameError ? 'teams-form__input--error' : ''}`}
              type="text"
              value={form.name}
              onChange={(e) => setForm({ ...form, name: e.target.value })}
              autoFocus
            />
            {nameError && <span className="teams-form__error">{nameError}</span>}
          </div>
          <div className="teams-form__field">
            <label className="teams-form__label" htmlFor="team-class">Klass</label>
            <select
              id="team-class"
              className="teams-form__input"
              value={form.classId}
              onChange={(e) => setForm({ ...form, classId: Number(e.target.value) })}
            >
              <option value={0}>— Välj klass —</option>
              {classes.map((c) => (
                <option key={c.id} value={c.id}>{c.name}</option>
              ))}
            </select>
          </div>
          <div className="teams-form__field">
            <label className="teams-form__label" htmlFor="team-club">Klubb</label>
            <select
              id="team-club"
              className="teams-form__input"
              value={form.clubId}
              onChange={(e) => setForm({ ...form, clubId: Number(e.target.value) })}
            >
              <option value={0}>— Välj klubb —</option>
              {clubs.map((c) => (
                <option key={c.id} value={c.id}>{c.name}</option>
              ))}
            </select>
          </div>
          <div className="teams-form__field">
            <label className="teams-form__label" htmlFor="team-bib">Nummerlapp</label>
            <input
              id="team-bib"
              className="teams-form__input"
              type="text"
              value={form.bib}
              onChange={(e) => setForm({ ...form, bib: e.target.value })}
            />
          </div>

          <div className="teams-members">
            <div className="teams-members__title">Lagmedlemmar (sträckor)</div>
            <div className="teams-members__list">
              {form.runners.map((runnerId, idx) => (
                <div className="teams-members__row" key={idx}>
                  <span className="teams-members__label">Sträcka {idx + 1}</span>
                  <select
                    className="teams-members__select"
                    value={runnerId}
                    onChange={(e) => setMember(idx, Number(e.target.value))}
                    aria-label={`Sträcka ${idx + 1} löpare`}
                  >
                    <option value={0}>— Välj löpare —</option>
                    {availableRunners.map((r) => (
                      <option key={r.id} value={r.id}>{r.name}</option>
                    ))}
                  </select>
                  <button
                    type="button"
                    className="teams-btn teams-btn--danger teams-btn--sm"
                    onClick={() => removeMember(idx)}
                    aria-label={`Ta bort sträcka ${idx + 1}`}
                  >
                    ✕
                  </button>
                </div>
              ))}
            </div>
            <button
              type="button"
              className="teams-btn teams-btn--secondary teams-btn--sm teams-members__add-btn"
              onClick={addMemberSlot}
              style={{ marginTop: '0.5rem' }}
            >
              + Lägg till sträcka
            </button>
          </div>

          <div className="teams-form__actions">
            <button type="submit" className="teams-btn teams-btn--primary" disabled={isPending}>
              {isPending ? 'Sparar…' : 'Spara'}
            </button>
            <button type="button" className="teams-btn teams-btn--secondary" onClick={onClose}>
              Avbryt
            </button>
          </div>
          {error && <p className="teams-form__error">{error}</p>}
        </form>
      </div>
    </div>
  );
}

function LegResultsPanel({ team, runnerMap }: { team: Team; runnerMap: Map<number, string> }): React.JSX.Element {
  if (team.legResults.length === 0 && team.runners.length === 0) {
    return <></>;
  }

  const legs: (LegResult & { runnerName: string })[] = team.runners.map((runnerId, idx) => {
    const lr = team.legResults.find((l) => l.leg === idx + 1);
    return {
      leg: idx + 1,
      runningTime: lr?.runningTime ?? 0,
      status: lr?.status ?? 0,
      runnerName: runnerMap.get(runnerId) ?? `Löpare #${runnerId}`,
    };
  });

  return (
    <div className="teams-legs">
      <h3 className="teams-legs__title">Sträckresultat — {team.name}</h3>
      <table className="teams-legs__table">
        <thead>
          <tr>
            <th>Sträcka</th>
            <th>Löpare</th>
            <th>Tid</th>
            <th>Status</th>
          </tr>
        </thead>
        <tbody>
          {legs.map((l) => (
            <tr key={l.leg}>
              <td>{l.leg}</td>
              <td>{l.runnerName}</td>
              <td>{formatTime(l.runningTime)}</td>
              <td>{statusLabel(l.status)}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}

function TeamsPage(): React.JSX.Element {
  const { data: teams, isLoading, isError, error } = useTeams();
  const { data: classes } = useClasses();
  const { data: clubs } = useClubs();
  const { data: runners } = useRunners();
  const createMutation = useCreateTeam();
  const updateMutation = useUpdateTeam();
  const deleteMutation = useDeleteTeam();

  const [search, setSearch] = useState('');
  const [filterClass, setFilterClass] = useState('');
  const [filterClub, setFilterClub] = useState('');
  const [sortField, setSortField] = useState<SortField>('name');
  const [sortDir, setSortDir] = useState<SortDir>('asc');
  const [selectedId, setSelectedId] = useState<number | null>(null);
  const [showCreate, setShowCreate] = useState(false);
  const [editTeam, setEditTeam] = useState<Team | null>(null);

  const classList = useMemo(() => (classes ?? []).map((c) => ({ id: c.id, name: c.name })), [classes]);
  const clubList = useMemo(() => (clubs ?? []).map((c) => ({ id: c.id, name: c.name })), [clubs]);
  const runnerList = useMemo(() => (runners ?? []).map((r) => ({ id: r.id, name: r.name })), [runners]);
  const runnerMap = useMemo(() => {
    const m = new Map<number, string>();
    (runners ?? []).forEach((r) => m.set(r.id, r.name));
    return m;
  }, [runners]);

  const uniqueClasses = useMemo(() => {
    if (!teams) return [];
    const seen = new Set<string>();
    return teams
      .filter((t) => { if (seen.has(t.class)) return false; seen.add(t.class); return true; })
      .map((t) => t.class)
      .sort();
  }, [teams]);

  const uniqueClubs = useMemo(() => {
    if (!teams) return [];
    const seen = new Set<string>();
    return teams
      .filter((t) => { if (seen.has(t.club)) return false; seen.add(t.club); return true; })
      .map((t) => t.club)
      .sort();
  }, [teams]);

  const filtered = useMemo(() => {
    if (!teams) return [];
    const q = search.toLowerCase();
    return teams.filter((t) => {
      if (q && !t.name.toLowerCase().includes(q) && !t.bib.toLowerCase().includes(q)) return false;
      if (filterClass && t.class !== filterClass) return false;
      if (filterClub && t.club !== filterClub) return false;
      return true;
    });
  }, [teams, search, filterClass, filterClub]);

  const sorted = useMemo(() => {
    const cmp = (a: Team, b: Team): number => {
      let va: string | number;
      let vb: string | number;
      switch (sortField) {
        case 'name': va = a.name; vb = b.name; break;
        case 'class': va = a.class; vb = b.class; break;
        case 'club': va = a.club; vb = b.club; break;
        case 'bib': va = a.bib; vb = b.bib; break;
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

  function handleCreate(data: TeamFormData): void {
    createMutation.mutate(data as Omit<Team, 'id'>, {
      onSuccess: () => setShowCreate(false),
    });
  }

  function handleUpdate(data: TeamFormData): void {
    if (!editTeam) return;
    updateMutation.mutate(
      { id: editTeam.id, team: data },
      { onSuccess: () => setEditTeam(null) },
    );
  }

  function handleDelete(id: number): void {
    deleteMutation.mutate(id, {
      onSuccess: () => {
        if (selectedId === id) setSelectedId(null);
        if (editTeam?.id === id) setEditTeam(null);
      },
    });
  }

  if (isLoading) {
    return <div className="teams-page"><p>Laddar lag…</p></div>;
  }

  if (isError) {
    return (
      <div className="teams-page">
        <p className="teams-form__error">Kunde inte ladda lag: {error.message}</p>
      </div>
    );
  }

  const selectedTeam = selectedId != null ? teams?.find((t) => t.id === selectedId) : null;

  return (
    <div className="teams-page">
      <div className="teams-page__header">
        <h2>Lag</h2>
        <div className="teams-page__actions">
          <button className="teams-btn teams-btn--primary" onClick={() => setShowCreate(true)}>
            Nytt lag
          </button>
          {selectedTeam && (
            <>
              <button
                className="teams-btn teams-btn--secondary"
                onClick={() => setEditTeam(selectedTeam)}
              >
                Redigera
              </button>
              <button
                className="teams-btn teams-btn--danger teams-btn--sm"
                onClick={() => handleDelete(selectedTeam.id)}
                disabled={deleteMutation.isPending}
              >
                Ta bort
              </button>
            </>
          )}
        </div>
      </div>

      <div className="teams-toolbar">
        <input
          className="teams-toolbar__search"
          type="text"
          placeholder="Sök lagnamn eller nummerlapp…"
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          aria-label="Sök lag"
        />
        <select
          className="teams-toolbar__select"
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
          className="teams-toolbar__select"
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

      <div className="teams-table-wrap">
        <table className="teams-table">
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
              <th>Medlemmar</th>
              <th>Tid</th>
              <th onClick={() => handleSort('status')}>
                Status<span className="sort-indicator">{sortIndicator('status')}</span>
              </th>
            </tr>
          </thead>
          <tbody>
            {sorted.length === 0 ? (
              <tr>
                <td colSpan={7} className="teams-table__empty">
                  {teams && teams.length > 0
                    ? 'Inga lag matchar filtret.'
                    : 'Inga lag registrerade.'}
                </td>
              </tr>
            ) : (
              sorted.map((t) => (
                <tr
                  key={t.id}
                  className={selectedId === t.id ? 'teams-table__row--selected' : ''}
                  onClick={() => setSelectedId(selectedId === t.id ? null : t.id)}
                >
                  <td>{t.bib || '—'}</td>
                  <td>{t.name}</td>
                  <td>{t.class || '—'}</td>
                  <td>{t.club || '—'}</td>
                  <td>{t.runners.length}</td>
                  <td>{formatTime(t.runningTime)}</td>
                  <td>{statusLabel(t.status)}</td>
                </tr>
              ))
            )}
          </tbody>
        </table>
      </div>

      <p className="teams-count">
        {sorted.length} av {teams?.length ?? 0} lag
      </p>

      {selectedTeam && (
        <LegResultsPanel team={selectedTeam} runnerMap={runnerMap} />
      )}

      {showCreate && (
        <TeamFormModal
          title="Nytt lag"
          initial={emptyForm()}
          onClose={() => { setShowCreate(false); createMutation.reset(); }}
          onSubmit={handleCreate}
          isPending={createMutation.isPending}
          error={createMutation.isError ? createMutation.error.message : null}
          classes={classList}
          clubs={clubList}
          availableRunners={runnerList}
        />
      )}

      {editTeam && (
        <TeamFormModal
          title="Redigera lag"
          initial={formFromTeam(editTeam)}
          onClose={() => { setEditTeam(null); updateMutation.reset(); }}
          onSubmit={handleUpdate}
          isPending={updateMutation.isPending}
          error={updateMutation.isError ? updateMutation.error.message : null}
          classes={classList}
          clubs={clubList}
          availableRunners={runnerList}
        />
      )}
    </div>
  );
}

export default TeamsPage;
