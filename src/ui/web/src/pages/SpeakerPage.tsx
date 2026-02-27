import { useState, useMemo, type FormEvent } from 'react';
import { useSpeakerConfig, useUpdateSpeakerConfig, useSpeakerMonitor } from '../hooks';
import { useClasses } from '../hooks';
import './SpeakerPage.css';

interface SpeakerEvent {
  time: number;
  classId: number;
  priority: number;
  type: string;
  runnerId?: number;
  teamId?: number;
}

interface MonitorData {
  currentTime: number;
  nextEventTime: number;
  events: SpeakerEvent[];
}

interface ConfigData {
  classIds: number[];
  windowSeconds: number;
  classes?: { id: number; name: string }[];
}

const POLL_INTERVAL = 5000;

function formatTime(seconds: number): string {
  if (seconds <= 0) return '—';
  const h = Math.floor(seconds / 3600);
  const m = Math.floor((seconds % 3600) / 60);
  const s = seconds % 60;
  const mm = String(m).padStart(2, '0');
  const ss = String(s).padStart(2, '0');
  return h > 0 ? `${h}:${mm}:${ss}` : `${mm}:${ss}`;
}

function eventTypeLabel(type: string): { label: string; className: string } {
  switch (type) {
    case 'start':
      return { label: 'Start', className: 'speaker-event-type speaker-event-type--start' };
    case 'finish':
      return { label: 'Mål', className: 'speaker-event-type speaker-event-type--finish' };
    case 'radio':
      return { label: 'Radio', className: 'speaker-event-type speaker-event-type--radio' };
    case 'expected':
      return { label: 'Förväntas', className: 'speaker-event-type speaker-event-type--expected' };
    default:
      return { label: type, className: 'speaker-event-type speaker-event-type--unknown' };
  }
}

type SortField = 'time' | 'type' | 'classId' | 'source';
type SortDir = 'asc' | 'desc';

function SpeakerPage(): React.JSX.Element {
  const { data: config, isLoading: configLoading, isError: configError, error: configErr } =
    useSpeakerConfig() as { data: ConfigData | undefined; isLoading: boolean; isError: boolean; error: Error | null };
  const updateConfig = useUpdateSpeakerConfig();
  const { data: monitor } =
    useSpeakerMonitor(POLL_INTERVAL) as { data: MonitorData | undefined };
  const { data: allClasses } = useClasses();

  const [windowInput, setWindowInput] = useState('');
  const [search, setSearch] = useState('');
  const [sortField, setSortField] = useState<SortField>('time');
  const [sortDir, setSortDir] = useState<SortDir>('desc');

  const configClasses = config?.classes;
  const classNameMap = useMemo(() => {
    const map = new Map<number, string>();
    if (allClasses) {
      for (const c of allClasses) {
        map.set(c.id, c.name);
      }
    }
    if (configClasses) {
      for (const c of configClasses) {
        map.set(c.id, c.name);
      }
    }
    return map;
  }, [allClasses, configClasses]);

  const selectedClassIds = useMemo(() => new Set(config?.classIds ?? []), [config?.classIds]);

  function handleClassToggle(classId: number): void {
    const current = new Set(selectedClassIds);
    if (current.has(classId)) {
      current.delete(classId);
    } else {
      current.add(classId);
    }
    updateConfig.mutate({
      classIds: Array.from(current),
      windowSeconds: config?.windowSeconds ?? 600,
    });
  }

  function handleWindowSubmit(e: FormEvent): void {
    e.preventDefault();
    const ws = parseInt(windowInput, 10);
    if (isNaN(ws) || ws <= 0) return;
    updateConfig.mutate({
      classIds: config?.classIds ?? [],
      windowSeconds: ws,
    });
    setWindowInput('');
  }

  const events = useMemo(() => monitor?.events ?? [], [monitor?.events]);

  const filtered = useMemo(() => {
    const q = search.toLowerCase();
    if (!q) return events;
    return events.filter((ev) => {
      const className = classNameMap.get(ev.classId) ?? '';
      if (className.toLowerCase().includes(q)) return true;
      if (ev.type.toLowerCase().includes(q)) return true;
      if (String(ev.runnerId ?? ev.teamId ?? '').includes(q)) return true;
      return false;
    });
  }, [events, search, classNameMap]);

  const sorted = useMemo(() => {
    const cmp = (a: SpeakerEvent, b: SpeakerEvent): number => {
      let r = 0;
      switch (sortField) {
        case 'time':
          r = a.time - b.time;
          break;
        case 'type':
          r = a.type.localeCompare(b.type, 'sv');
          break;
        case 'classId': {
          const na = classNameMap.get(a.classId) ?? '';
          const nb = classNameMap.get(b.classId) ?? '';
          r = na.localeCompare(nb, 'sv');
          break;
        }
        case 'source':
          r = (a.runnerId ?? a.teamId ?? 0) - (b.runnerId ?? b.teamId ?? 0);
          break;
      }
      return sortDir === 'asc' ? r : -r;
    };
    return [...filtered].sort(cmp);
  }, [filtered, sortField, sortDir, classNameMap]);

  function handleSort(field: SortField): void {
    if (sortField === field) {
      setSortDir(sortDir === 'asc' ? 'desc' : 'asc');
    } else {
      setSortField(field);
      setSortDir(field === 'time' ? 'desc' : 'asc');
    }
  }

  function sortIndicator(field: SortField): string {
    if (sortField !== field) return '';
    return sortDir === 'asc' ? ' ▲' : ' ▼';
  }

  if (configLoading) {
    return <div className="speaker-page"><p>Laddar speaker-konfiguration…</p></div>;
  }

  if (configError) {
    return (
      <div className="speaker-page">
        <p className="speaker-error">Kunde inte ladda konfiguration: {configErr?.message}</p>
      </div>
    );
  }

  return (
    <div className="speaker-page">
      <div className="speaker-page__header">
        <h2>Speaker</h2>
        <div className="speaker-page__actions">
          <span className="speaker-page__poll-label">
            <span className="speaker-page__poll-dot" />
            Polling {POLL_INTERVAL / 1000}s
          </span>
        </div>
      </div>

      {/* Config section */}
      <div className="speaker-config">
        <h3 className="speaker-config__title">Konfiguration</h3>

        <div className="speaker-config__row">
          <div className="speaker-config__field">
            <label className="speaker-config__label">Tidsfönster (sekunder)</label>
            <form className="speaker-config__window-form" onSubmit={handleWindowSubmit}>
              <input
                className="speaker-config__input"
                type="number"
                placeholder={String(config?.windowSeconds ?? 600)}
                value={windowInput}
                onChange={(e) => setWindowInput(e.target.value)}
                min={1}
                aria-label="Tidsfönster i sekunder"
              />
              <button
                type="submit"
                className="speaker-btn speaker-btn--primary"
                disabled={updateConfig.isPending || !windowInput}
              >
                Spara
              </button>
            </form>
          </div>
        </div>

        <div className="speaker-config__classes">
          <label className="speaker-config__label">Bevakade klasser</label>
          {allClasses && allClasses.length > 0 ? (
            <div className="speaker-class-list">
              {allClasses.map((c) => (
                <label key={c.id} className="speaker-class-chip">
                  <input
                    type="checkbox"
                    checked={selectedClassIds.has(c.id)}
                    onChange={() => handleClassToggle(c.id)}
                  />
                  <span className="speaker-class-chip__label">{c.name}</span>
                </label>
              ))}
            </div>
          ) : (
            <p className="speaker-config__empty">Inga klasser tillgängliga.</p>
          )}
        </div>

        {updateConfig.isError && (
          <p className="speaker-error">
            Fel vid uppdatering: {updateConfig.error.message}
          </p>
        )}
      </div>

      {/* Monitor section */}
      <div className="speaker-monitor">
        <h3 className="speaker-monitor__title">Live-händelser</h3>

        <div className="speaker-toolbar">
          <input
            className="speaker-toolbar__search"
            type="text"
            placeholder="Sök klass, typ eller löpar-ID…"
            value={search}
            onChange={(e) => setSearch(e.target.value)}
            aria-label="Sök händelser"
          />
        </div>

        <div className="speaker-table-wrap">
          <table className="speaker-table">
            <thead>
              <tr>
                <th onClick={() => handleSort('time')}>
                  Tid<span className="sort-indicator">{sortIndicator('time')}</span>
                </th>
                <th onClick={() => handleSort('type')}>
                  Typ<span className="sort-indicator">{sortIndicator('type')}</span>
                </th>
                <th onClick={() => handleSort('classId')}>
                  Klass<span className="sort-indicator">{sortIndicator('classId')}</span>
                </th>
                <th onClick={() => handleSort('source')}>
                  Löpare/Lag<span className="sort-indicator">{sortIndicator('source')}</span>
                </th>
                <th>Prioritet</th>
              </tr>
            </thead>
            <tbody>
              {sorted.length === 0 ? (
                <tr>
                  <td colSpan={5} className="speaker-table__empty">
                    {events.length > 0
                      ? 'Inga händelser matchar sökningen.'
                      : selectedClassIds.size === 0
                        ? 'Välj klasser ovan för att se händelser.'
                        : 'Inga händelser just nu.'}
                  </td>
                </tr>
              ) : (
                sorted.map((ev, idx) => {
                  const et = eventTypeLabel(ev.type);
                  return (
                    <tr key={idx}>
                      <td>{formatTime(ev.time)}</td>
                      <td><span className={et.className}>{et.label}</span></td>
                      <td>{classNameMap.get(ev.classId) ?? `Klass ${ev.classId}`}</td>
                      <td>
                        {ev.runnerId != null
                          ? `Löpare #${ev.runnerId}`
                          : ev.teamId != null
                            ? `Lag #${ev.teamId}`
                            : '—'}
                      </td>
                      <td>{ev.priority}</td>
                    </tr>
                  );
                })
              )}
            </tbody>
          </table>
        </div>

        <p className="speaker-count">
          {sorted.length} av {events.length} händelser
          {monitor?.currentTime != null && (
            <> · Servertid: {formatTime(monitor.currentTime)}</>
          )}
        </p>
      </div>
    </div>
  );
}

export default SpeakerPage;
