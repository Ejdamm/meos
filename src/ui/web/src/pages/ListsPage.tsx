import { useState, useMemo } from 'react';
import { useLists, useList, useResults } from '../hooks';
import { useClasses } from '../hooks';
import type { ResultEntry } from '../api/lists';
import './ListsPage.css';

type SortField = 'place' | 'name' | 'club' | 'time';
type SortDir = 'asc' | 'desc';

const statusLabels: Record<number, string> = {
  0: 'OK',
  1: 'DNS',
  2: 'DNF',
  3: 'MP',
  4: 'DQ',
  5: 'OT',
};

function statusLabel(status: number): string {
  return statusLabels[status] ?? `Status ${status}`;
}

function ListsPage(): React.JSX.Element {
  const { data: listTypes, isLoading: listsLoading, isError: listsError, error: listsErr } = useLists();
  const { data: classes } = useClasses();

  const [selectedType, setSelectedType] = useState('');
  const [selectedClassId, setSelectedClassId] = useState<number | undefined>(undefined);
  const [search, setSearch] = useState('');
  const [sortField, setSortField] = useState<SortField>('place');
  const [sortDir, setSortDir] = useState<SortDir>('asc');

  const { data: listData, isLoading: listLoading } = useList(selectedType);
  const { data: results, isLoading: resultsLoading } = useResults(selectedClassId);

  const filtered = useMemo(() => {
    if (!results) return [];
    const q = search.toLowerCase();
    return results.filter((r) => {
      if (q && !r.name.toLowerCase().includes(q) && !r.club.toLowerCase().includes(q))
        return false;
      return true;
    });
  }, [results, search]);

  const sorted = useMemo(() => {
    const cmp = (a: ResultEntry, b: ResultEntry): number => {
      let r = 0;
      switch (sortField) {
        case 'place': r = a.place - b.place; break;
        case 'name': r = a.name.localeCompare(b.name, 'sv'); break;
        case 'club': r = a.club.localeCompare(b.club, 'sv'); break;
        case 'time': r = a.time.localeCompare(b.time); break;
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

  if (listsLoading) {
    return <div className="lists-page"><p>Laddar listor…</p></div>;
  }

  if (listsError) {
    return (
      <div className="lists-page">
        <p className="lists-error">Kunde inte ladda listor: {listsErr?.message}</p>
      </div>
    );
  }

  return (
    <div className="lists-page">
      <div className="lists-page__header">
        <h2>Listor</h2>
      </div>

      <div className="lists-toolbar">
        <div className="lists-toolbar__field">
          <label className="lists-toolbar__label" htmlFor="list-type">Listtyp</label>
          <select
            id="list-type"
            className="lists-toolbar__select"
            value={selectedType}
            onChange={(e) => setSelectedType(e.target.value)}
            aria-label="Välj listtyp"
          >
            <option value="">— Välj listtyp —</option>
            {listTypes?.map((lt) => (
              <option key={lt.type} value={lt.type}>{lt.name}</option>
            ))}
          </select>
        </div>
        <div className="lists-toolbar__field">
          <label className="lists-toolbar__label" htmlFor="list-class">Klass</label>
          <select
            id="list-class"
            className="lists-toolbar__select"
            value={selectedClassId ?? ''}
            onChange={(e) => setSelectedClassId(e.target.value ? Number(e.target.value) : undefined)}
            aria-label="Filtrera på klass"
          >
            <option value="">Alla klasser</option>
            {classes?.map((c) => (
              <option key={c.id} value={c.id}>{c.name}</option>
            ))}
          </select>
        </div>
      </div>

      {/* Generated list HTML */}
      {selectedType && (
        <div className="lists-generated">
          <h3 className="lists-generated__title">
            {listTypes?.find((lt) => lt.type === selectedType)?.name ?? selectedType}
          </h3>
          {listLoading ? (
            <p>Genererar lista…</p>
          ) : listData?.html ? (
            <div
              className="lists-generated__content"
              dangerouslySetInnerHTML={{ __html: listData.html }}
            />
          ) : (
            <p className="lists-empty">Ingen data för vald listtyp.</p>
          )}
        </div>
      )}

      {/* Results table */}
      <div className="lists-results">
        <h3 className="lists-results__title">Resultat</h3>
        <div className="lists-toolbar">
          <input
            className="lists-toolbar__search"
            type="text"
            placeholder="Sök namn eller klubb…"
            value={search}
            onChange={(e) => setSearch(e.target.value)}
            aria-label="Sök resultat"
          />
        </div>
        {resultsLoading ? (
          <p>Laddar resultat…</p>
        ) : (
          <>
            <div className="lists-table-wrap">
              <table className="lists-table">
                <thead>
                  <tr>
                    <th onClick={() => handleSort('place')}>
                      Plac<span className="sort-indicator">{sortIndicator('place')}</span>
                    </th>
                    <th onClick={() => handleSort('name')}>
                      Namn<span className="sort-indicator">{sortIndicator('name')}</span>
                    </th>
                    <th onClick={() => handleSort('club')}>
                      Klubb<span className="sort-indicator">{sortIndicator('club')}</span>
                    </th>
                    <th>Klass</th>
                    <th onClick={() => handleSort('time')}>
                      Tid<span className="sort-indicator">{sortIndicator('time')}</span>
                    </th>
                    <th>Status</th>
                  </tr>
                </thead>
                <tbody>
                  {sorted.length === 0 ? (
                    <tr>
                      <td colSpan={6} className="lists-table__empty">
                        {results && results.length > 0
                          ? 'Inga resultat matchar sökningen.'
                          : 'Inga resultat tillgängliga.'}
                      </td>
                    </tr>
                  ) : (
                    sorted.map((r) => (
                      <tr key={r.runnerId}>
                        <td>{r.place > 0 ? r.place : '—'}</td>
                        <td>{r.name}</td>
                        <td>{r.club || '—'}</td>
                        <td>{r.class}</td>
                        <td>{r.time || '—'}</td>
                        <td>
                          <span className={`lists-status lists-status--${r.status}`}>
                            {statusLabel(r.status)}
                          </span>
                        </td>
                      </tr>
                    ))
                  )}
                </tbody>
              </table>
            </div>
            <p className="lists-count">
              {sorted.length} av {results?.length ?? 0} resultat
            </p>
          </>
        )}
      </div>
    </div>
  );
}

export default ListsPage;
