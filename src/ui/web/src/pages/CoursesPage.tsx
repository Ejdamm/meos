import { useState, useMemo, useCallback, type FormEvent } from 'react';
import {
  useCourses,
  useCreateCourse,
  useUpdateCourse,
  useDeleteCourse,
  useControls,
} from '../hooks';
import { Modal } from '../components/Modal';
import type { Course, Control } from '../types';
import './CoursesPage.css';

type SortField = 'name' | 'length' | 'numControls';
type SortDir = 'asc' | 'desc';

interface CourseFormData {
  name: string;
  length: number;
  controls: number[];
}

function emptyForm(): CourseFormData {
  return { name: '', length: 0, controls: [] };
}

function formFromCourse(c: Course): CourseFormData {
  return {
    name: c.name,
    length: c.length,
    controls: [...c.controls],
  };
}

function ControlSequence({
  controls,
  availableControls,
  onChange,
}: {
  controls: number[];
  availableControls: Control[];
  onChange: (controls: number[]) => void;
}): React.JSX.Element {
  const controlMap = useMemo(() => {
    const m = new Map<number, string>();
    for (const c of availableControls) m.set(c.id, c.name);
    return m;
  }, [availableControls]);

  function moveUp(idx: number): void {
    if (idx === 0) return;
    const next = [...controls];
    [next[idx - 1], next[idx]] = [next[idx], next[idx - 1]];
    onChange(next);
  }

  function moveDown(idx: number): void {
    if (idx === controls.length - 1) return;
    const next = [...controls];
    [next[idx], next[idx + 1]] = [next[idx + 1], next[idx]];
    onChange(next);
  }

  function remove(idx: number): void {
    onChange(controls.filter((_, i) => i !== idx));
  }

  function add(controlId: number): void {
    if (controlId > 0) onChange([...controls, controlId]);
  }

  return (
    <div className="courses-controls">
      <div className="courses-controls__label">Kontrollsekvens</div>
      {controls.length === 0 && (
        <p className="courses-controls__empty">Inga kontroller tillagda.</p>
      )}
      <ol className="courses-controls__list">
        {controls.map((cId, idx) => (
          <li key={`${cId}-${idx}`} className="courses-controls__item">
            <span className="courses-controls__name">
              {controlMap.get(cId) ?? `#${cId}`}
            </span>
            <span className="courses-controls__actions">
              <button
                type="button"
                className="courses-controls__btn"
                onClick={() => moveUp(idx)}
                disabled={idx === 0}
                aria-label="Flytta upp"
              >
                ▲
              </button>
              <button
                type="button"
                className="courses-controls__btn"
                onClick={() => moveDown(idx)}
                disabled={idx === controls.length - 1}
                aria-label="Flytta ner"
              >
                ▼
              </button>
              <button
                type="button"
                className="courses-controls__btn courses-controls__btn--remove"
                onClick={() => remove(idx)}
                aria-label="Ta bort"
              >
                ✕
              </button>
            </span>
          </li>
        ))}
      </ol>
      <select
        className="courses-form__input"
        onChange={(e) => {
          add(Number(e.target.value));
          e.target.value = '0';
        }}
        defaultValue={0}
        aria-label="Lägg till kontroll"
      >
        <option value={0}>— Lägg till kontroll —</option>
        {availableControls.map((c) => (
          <option key={c.id} value={c.id}>
            {c.name}
          </option>
        ))}
      </select>
    </div>
  );
}

function CourseFormModal({
  title,
  initial,
  onClose,
  onSubmit,
  isPending,
  error,
  availableControls,
}: {
  title: string;
  initial: CourseFormData;
  onClose: () => void;
  onSubmit: (data: CourseFormData) => void;
  isPending: boolean;
  error: string | null;
  availableControls: Control[];
}): React.JSX.Element {
  const [form, setForm] = useState<CourseFormData>(initial);
  const [nameError, setNameError] = useState('');

  const handleControlsChange = useCallback(
    (controls: number[]) => setForm((f) => ({ ...f, controls })),
    [],
  );

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
          <div className="courses-form__field">
            <label className="courses-form__label" htmlFor="course-name">Namn</label>
            <input
              id="course-name"
              className={`courses-form__input ${nameError ? 'courses-form__input--error' : ''}`}
              type="text"
              value={form.name}
              onChange={(e) => setForm({ ...form, name: e.target.value })}
              aria-invalid={nameError ? true : undefined}
              aria-describedby={nameError ? 'course-name-error' : undefined}
            />
            {nameError && <span id="course-name-error" className="courses-form__error" role="alert">{nameError}</span>}
          </div>
          <div className="courses-form__field">
            <label className="courses-form__label" htmlFor="course-length">Längd (m)</label>
            <input
              id="course-length"
              className="courses-form__input"
              type="number"
              min={0}
              value={form.length}
              onChange={(e) => setForm({ ...form, length: Math.max(0, Number(e.target.value)) })}
            />
          </div>
          <ControlSequence
            controls={form.controls}
            availableControls={availableControls}
            onChange={handleControlsChange}
          />
          <div className="courses-form__actions">
            <button type="submit" className="courses-btn courses-btn--primary" disabled={isPending}>
              {isPending ? 'Sparar…' : 'Spara'}
            </button>
            <button type="button" className="courses-btn courses-btn--secondary" onClick={onClose}>
              Avbryt
            </button>
          </div>
          {error && <p className="courses-form__error" role="alert">{error}</p>}
        </form>
    </Modal>
  );
}

function CoursesPage(): React.JSX.Element {
  const { data: courses, isLoading, isError, error } = useCourses();
  const { data: controls } = useControls();
  const createMutation = useCreateCourse();
  const updateMutation = useUpdateCourse();
  const deleteMutation = useDeleteCourse();

  const [search, setSearch] = useState('');
  const [sortField, setSortField] = useState<SortField>('name');
  const [sortDir, setSortDir] = useState<SortDir>('asc');
  const [selectedId, setSelectedId] = useState<number | null>(null);
  const [showCreate, setShowCreate] = useState(false);
  const [editCourse, setEditCourse] = useState<Course | null>(null);

  const controlList = useMemo(() => controls ?? [], [controls]);

  const filtered = useMemo(() => {
    if (!courses) return [];
    const q = search.toLowerCase();
    return courses.filter((c) => {
      if (q && !c.name.toLowerCase().includes(q)) return false;
      return true;
    });
  }, [courses, search]);

  const sorted = useMemo(() => {
    const cmp = (a: Course, b: Course): number => {
      let va: string | number;
      let vb: string | number;
      switch (sortField) {
        case 'name': va = a.name; vb = b.name; break;
        case 'length': va = a.length; vb = b.length; break;
        case 'numControls': va = a.numControls; vb = b.numControls; break;
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

  function handleCreate(data: CourseFormData): void {
    createMutation.mutate(
      { ...data, numControls: data.controls.length } as Omit<Course, 'id'>,
      { onSuccess: () => setShowCreate(false) },
    );
  }

  function handleUpdate(data: CourseFormData): void {
    if (!editCourse) return;
    updateMutation.mutate(
      { id: editCourse.id, course: { ...data, numControls: data.controls.length } },
      { onSuccess: () => setEditCourse(null) },
    );
  }

  function handleDelete(id: number): void {
    deleteMutation.mutate(id, {
      onSuccess: () => {
        if (selectedId === id) setSelectedId(null);
        if (editCourse?.id === id) setEditCourse(null);
      },
    });
  }

  if (isLoading) {
    return <div className="courses-page"><p aria-live="polite">Laddar banor…</p></div>;
  }

  if (isError) {
    return (
      <div className="courses-page">
        <p className="courses-form__error">Kunde inte ladda banor: {error.message}</p>
      </div>
    );
  }

  const selectedCourse = selectedId != null ? courses?.find((c) => c.id === selectedId) : null;

  return (
    <div className="courses-page">
      <div className="courses-page__header">
        <h2>Banor</h2>
        <div className="courses-page__actions">
          <button className="courses-btn courses-btn--primary" onClick={() => setShowCreate(true)}>
            Ny bana
          </button>
          {selectedCourse && (
            <>
              <button
                className="courses-btn courses-btn--secondary"
                onClick={() => setEditCourse(selectedCourse)}
              >
                Redigera
              </button>
              <button
                className="courses-btn courses-btn--danger courses-btn--sm"
                onClick={() => handleDelete(selectedCourse.id)}
                disabled={deleteMutation.isPending}
              >
                Ta bort
              </button>
            </>
          )}
        </div>
      </div>

      <div className="courses-toolbar">
        <input
          className="courses-toolbar__search"
          type="text"
          placeholder="Sök bannamn…"
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          aria-label="Sök bana"
        />
      </div>

      <div className="courses-table-wrap">
        <table className="courses-table">
          <thead>
            <tr>
              <th onClick={() => handleSort('name')}>
                Namn<span className="sort-indicator">{sortIndicator('name')}</span>
              </th>
              <th onClick={() => handleSort('length')}>
                Längd (m)<span className="sort-indicator">{sortIndicator('length')}</span>
              </th>
              <th onClick={() => handleSort('numControls')}>
                Kontroller<span className="sort-indicator">{sortIndicator('numControls')}</span>
              </th>
            </tr>
          </thead>
          <tbody>
            {sorted.length === 0 ? (
              <tr>
                <td colSpan={3} className="courses-table__empty">
                  {courses && courses.length > 0
                    ? 'Inga banor matchar filtret.'
                    : 'Inga banor registrerade.'}
                </td>
              </tr>
            ) : (
              sorted.map((c) => (
                <tr
                  key={c.id}
                  className={selectedId === c.id ? 'courses-table__row--selected' : ''}
                  onClick={() => setSelectedId(selectedId === c.id ? null : c.id)}
                >
                  <td>{c.name}</td>
                  <td>{c.length > 0 ? c.length : '—'}</td>
                  <td>{c.numControls}</td>
                </tr>
              ))
            )}
          </tbody>
        </table>
      </div>

      <p className="courses-count" role="status">
        {sorted.length} av {courses?.length ?? 0} banor
      </p>

      {showCreate && (
        <CourseFormModal
          title="Ny bana"
          initial={emptyForm()}
          onClose={() => { setShowCreate(false); createMutation.reset(); }}
          onSubmit={handleCreate}
          isPending={createMutation.isPending}
          error={createMutation.isError ? createMutation.error.message : null}
          availableControls={controlList}
        />
      )}

      {editCourse && (
        <CourseFormModal
          title="Redigera bana"
          initial={formFromCourse(editCourse)}
          onClose={() => { setEditCourse(null); updateMutation.reset(); }}
          onSubmit={handleUpdate}
          isPending={updateMutation.isPending}
          error={updateMutation.isError ? updateMutation.error.message : null}
          availableControls={controlList}
        />
      )}
    </div>
  );
}

export default CoursesPage;
