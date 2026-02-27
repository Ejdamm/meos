import { useState, useMemo, type FormEvent } from 'react';
import { useCards, useReadCard, useRunners } from '../hooks';
import type { Card, Punch, Runner } from '../types';
import './SiPage.css';

type SortField = 'cardNo' | 'punches' | 'runner';
type SortDir = 'asc' | 'desc';

const POLL_INTERVAL = 5000;

function punchTypeLabel(type: number): { label: string; className: string } {
  switch (type) {
    case 0:
      return { label: 'Kontroll', className: 'si-punch-type si-punch-type--control' };
    case 1:
      return { label: 'Start', className: 'si-punch-type si-punch-type--start' };
    case 2:
      return { label: 'Mål', className: 'si-punch-type si-punch-type--finish' };
    default:
      return { label: `Typ ${type}`, className: 'si-punch-type si-punch-type--control' };
  }
}

function formatTime(seconds: number): string {
  if (seconds <= 0) return '—';
  const h = Math.floor(seconds / 3600);
  const m = Math.floor((seconds % 3600) / 60);
  const s = seconds % 60;
  const mm = String(m).padStart(2, '0');
  const ss = String(s).padStart(2, '0');
  return h > 0 ? `${h}:${mm}:${ss}` : `${mm}:${ss}`;
}

function buildRunnerByCard(runners: Runner[] | undefined): Map<number, Runner> {
  const map = new Map<number, Runner>();
  if (!runners) return map;
  for (const r of runners) {
    if (r.cardNo > 0) {
      map.set(r.cardNo, r);
    }
  }
  return map;
}

function PunchDetail({ card, runner }: { card: Card; runner?: Runner }): React.JSX.Element {
  return (
    <div className="si-detail">
      <h3 className="si-detail__title">
        Bricka {card.cardNo}
        {runner ? ` — ${runner.name}` : ''}
      </h3>
      {card.punches.length === 0 ? (
        <p style={{ color: '#718096', fontStyle: 'italic' }}>Inga stämplingar.</p>
      ) : (
        <table className="si-punch-table">
          <thead>
            <tr>
              <th>#</th>
              <th>Typ</th>
              <th>Kontroll</th>
              <th>Tid</th>
            </tr>
          </thead>
          <tbody>
            {card.punches.map((p: Punch, idx: number) => {
              const pt = punchTypeLabel(p.type);
              return (
                <tr key={idx}>
                  <td>{idx + 1}</td>
                  <td><span className={pt.className}>{pt.label}</span></td>
                  <td>{p.controlId}</td>
                  <td>{formatTime(p.time)}</td>
                </tr>
              );
            })}
          </tbody>
        </table>
      )}
    </div>
  );
}

function SiPage(): React.JSX.Element {
  const { data: cards, isLoading, isError, error } = useCards(POLL_INTERVAL);
  const { data: runners } = useRunners();
  const readCardMutation = useReadCard();

  const [search, setSearch] = useState('');
  const [sortField, setSortField] = useState<SortField>('cardNo');
  const [sortDir, setSortDir] = useState<SortDir>('asc');
  const [selectedId, setSelectedId] = useState<number | null>(null);
  const [readCardNo, setReadCardNo] = useState('');

  const runnerByCard = useMemo(() => buildRunnerByCard(runners), [runners]);

  const filtered = useMemo(() => {
    if (!cards) return [];
    const q = search.toLowerCase();
    return cards.filter((c) => {
      if (!q) return true;
      if (String(c.cardNo).includes(q)) return true;
      const r = runnerByCard.get(c.cardNo);
      if (r && r.name.toLowerCase().includes(q)) return true;
      return false;
    });
  }, [cards, search, runnerByCard]);

  const sorted = useMemo(() => {
    const cmp = (a: Card, b: Card): number => {
      let r: number;
      switch (sortField) {
        case 'cardNo':
          r = a.cardNo - b.cardNo;
          break;
        case 'punches':
          r = a.punches.length - b.punches.length;
          break;
        case 'runner': {
          const ra = runnerByCard.get(a.cardNo)?.name ?? '';
          const rb = runnerByCard.get(b.cardNo)?.name ?? '';
          r = ra.localeCompare(rb, 'sv');
          break;
        }
      }
      return sortDir === 'asc' ? r : -r;
    };
    return [...filtered].sort(cmp);
  }, [filtered, sortField, sortDir, runnerByCard]);

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

  function handleReadCard(e: FormEvent): void {
    e.preventDefault();
    const num = parseInt(readCardNo, 10);
    if (isNaN(num) || num <= 0) return;
    readCardMutation.mutate({ cardNo: num }, {
      onSuccess: () => setReadCardNo(''),
    });
  }

  if (isLoading) {
    return <div className="si-page"><p>Laddar brickor…</p></div>;
  }

  if (isError) {
    return (
      <div className="si-page">
        <p className="si-form__error">Kunde inte ladda brickor: {error.message}</p>
      </div>
    );
  }

  const selectedCard = selectedId != null ? cards?.find((c) => c.id === selectedId) : null;

  return (
    <div className="si-page">
      <div className="si-page__header">
        <h2>SI-brickor</h2>
        <div className="si-page__actions">
          <span className="si-page__poll-label">
            <span className="si-page__poll-dot" />
            Polling {POLL_INTERVAL / 1000}s
          </span>
        </div>
      </div>

      <div className="si-toolbar">
        <input
          className="si-toolbar__search"
          type="text"
          placeholder="Sök bricknummer eller löpare…"
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          aria-label="Sök bricka"
        />
        <form className="si-read-form" onSubmit={handleReadCard}>
          <input
            className="si-read-form__input"
            type="number"
            placeholder="Bricknr"
            value={readCardNo}
            onChange={(e) => setReadCardNo(e.target.value)}
            min={1}
            aria-label="Bricknummer att läsa"
          />
          <button
            type="submit"
            className="si-btn si-btn--primary"
            disabled={readCardMutation.isPending || !readCardNo}
          >
            {readCardMutation.isPending ? 'Läser…' : 'Läs bricka'}
          </button>
        </form>
      </div>

      {readCardMutation.isError && (
        <p className="si-form__error">Fel vid brickavläsning: {readCardMutation.error.message}</p>
      )}

      <div className="si-table-wrap">
        <table className="si-table">
          <thead>
            <tr>
              <th onClick={() => handleSort('cardNo')}>
                Bricknr<span className="sort-indicator">{sortIndicator('cardNo')}</span>
              </th>
              <th onClick={() => handleSort('runner')}>
                Löpare<span className="sort-indicator">{sortIndicator('runner')}</span>
              </th>
              <th onClick={() => handleSort('punches')}>
                Stämplingar<span className="sort-indicator">{sortIndicator('punches')}</span>
              </th>
            </tr>
          </thead>
          <tbody>
            {sorted.length === 0 ? (
              <tr>
                <td colSpan={3} className="si-table__empty">
                  {cards && cards.length > 0
                    ? 'Inga brickor matchar filtret.'
                    : 'Inga brickor registrerade.'}
                </td>
              </tr>
            ) : (
              sorted.map((c) => {
                const runner = runnerByCard.get(c.cardNo);
                return (
                  <tr
                    key={c.id}
                    className={selectedId === c.id ? 'si-table__row--selected' : ''}
                    onClick={() => setSelectedId(selectedId === c.id ? null : c.id)}
                  >
                    <td>{c.cardNo}</td>
                    <td>{runner ? runner.name : '—'}</td>
                    <td>{c.punches.length}</td>
                  </tr>
                );
              })
            )}
          </tbody>
        </table>
      </div>

      <p className="si-count">
        {sorted.length} av {cards?.length ?? 0} brickor
      </p>

      {selectedCard && (
        <PunchDetail
          card={selectedCard}
          runner={runnerByCard.get(selectedCard.cardNo)}
        />
      )}
    </div>
  );
}

export default SiPage;
