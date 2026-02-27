import { useState, useMemo, type FormEvent } from 'react';
import {
  useClasses,
  useCreateClass,
  useUpdateClass,
  useDeleteClass,
  useCourses,
} from '../hooks';
import { Modal } from '../components/Modal';
import type { Class } from '../types';
import './ClassesPage.css';

type SortField = 'name' | 'type' | 'numStages' | 'course';
type SortDir = 'asc' | 'desc';

interface ClassFormData {
  name: string;
  type: string;
  numStages: number;
  courseId: number;
}

function emptyForm(): ClassFormData {
  return { name: '', type: '', numStages: 1, courseId: 0 };
}

function formFromClass(c: Class): ClassFormData {
  return {
    name: c.name,
    type: c.type,
    numStages: c.numStages,
    courseId: c.courseId,
  };
}

function ClassFormModal({
  title,
  initial,
  onClose,
  onSubmit,
  isPending,
  error,
  courses,
}: {
  title: string;
  initial: ClassFormData;
  onClose: () => void;
  onSubmit: (data: ClassFormData) => void;
  isPending: boolean;
  error: string | null;
  courses: { id: number; name: string }[];
}): React.JSX.Element {
  const [form, setForm] = useState<ClassFormData>(initial);
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
          <div className="classes-form__field">
            <label className="classes-form__label" htmlFor="class-name">Namn</label>
            <input
              id="class-name"
              className={`classes-form__input ${nameError ? 'classes-form__input--error' : ''}`}
              type="text"
              value={form.name}
              onChange={(e) => setForm({ ...form, name: e.target.value })}
              aria-invalid={nameError ? true : undefined}
              aria-describedby={nameError ? 'class-name-error' : undefined}
            />
            {nameError && <span id="class-name-error" className="classes-form__error" role="alert">{nameError}</span>}
          </div>
          <div className="classes-form__field">
            <label className="classes-form__label" htmlFor="class-type">Typ</label>
            <input
              id="class-type"
              className="classes-form__input"
              type="text"
              value={form.type}
              onChange={(e) => setForm({ ...form, type: e.target.value })}
              placeholder="t.ex. Individuell, Stafett"
            />
          </div>
          <div className="classes-form__field">
            <label className="classes-form__label" htmlFor="class-stages">Antal sträckor</label>
            <input
              id="class-stages"
              className="classes-form__input"
              type="number"
              min={1}
              value={form.numStages}
              onChange={(e) => setForm({ ...form, numStages: Math.max(1, Number(e.target.value)) })}
            />
          </div>
          <div className="classes-form__field">
            <label className="classes-form__label" htmlFor="class-course">Bana</label>
            <select
              id="class-course"
              className="classes-form__input"
              value={form.courseId}
              onChange={(e) => setForm({ ...form, courseId: Number(e.target.value) })}
            >
              <option value={0}>— Ingen bana —</option>
              {courses.map((c) => (
                <option key={c.id} value={c.id}>{c.name}</option>
              ))}
            </select>
          </div>
          <div className="classes-form__actions">
            <button type="submit" className="classes-btn classes-btn--primary" disabled={isPending}>
              {isPending ? 'Sparar…' : 'Spara'}
            </button>
            <button type="button" className="classes-btn classes-btn--secondary" onClick={onClose}>
              Avbryt
            </button>
          </div>
          {error && <p className="classes-form__error" role="alert">{error}</p>}
        </form>
    </Modal>
  );
}

function ClassesPage(): React.JSX.Element {
  const { data: classes, isLoading, isError, error } = useClasses();
  const { data: courses } = useCourses();
  const createMutation = useCreateClass();
  const updateMutation = useUpdateClass();
  const deleteMutation = useDeleteClass();

  const [search, setSearch] = useState('');
  const [filterType, setFilterType] = useState('');
  const [sortField, setSortField] = useState<SortField>('name');
  const [sortDir, setSortDir] = useState<SortDir>('asc');
  const [selectedId, setSelectedId] = useState<number | null>(null);
  const [showCreate, setShowCreate] = useState(false);
  const [editClass, setEditClass] = useState<Class | null>(null);

  const courseList = useMemo(
    () => (courses ?? []).map((c) => ({ id: c.id, name: c.name })),
    [courses],
  );

  const courseMap = useMemo(() => {
    const m = new Map<number, string>();
    for (const c of courses ?? []) m.set(c.id, c.name);
    return m;
  }, [courses]);

  const uniqueTypes = useMemo(() => {
    if (!classes) return [];
    const seen = new Set<string>();
    return classes
      .map((c) => c.type)
      .filter((t) => {
        if (!t || seen.has(t)) return false;
        seen.add(t);
        return true;
      })
      .sort();
  }, [classes]);

  const filtered = useMemo(() => {
    if (!classes) return [];
    const q = search.toLowerCase();
    return classes.filter((c) => {
      if (q && !c.name.toLowerCase().includes(q)) return false;
      if (filterType && c.type !== filterType) return false;
      return true;
    });
  }, [classes, search, filterType]);

  const sorted = useMemo(() => {
    const cmp = (a: Class, b: Class): number => {
      let va: string | number;
      let vb: string | number;
      switch (sortField) {
        case 'name': va = a.name; vb = b.name; break;
        case 'type': va = a.type; vb = b.type; break;
        case 'numStages': va = a.numStages; vb = b.numStages; break;
        case 'course':
          va = courseMap.get(a.courseId) ?? '';
          vb = courseMap.get(b.courseId) ?? '';
          break;
      }
      if (typeof va === 'string') {
        const r = va.localeCompare(vb as string, 'sv');
        return sortDir === 'asc' ? r : -r;
      }
      const r = (va as number) - (vb as number);
      return sortDir === 'asc' ? r : -r;
    };
    return [...filtered].sort(cmp);
  }, [filtered, sortField, sortDir, courseMap]);

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

  function handleCreate(data: ClassFormData): void {
    createMutation.mutate(data as Omit<Class, 'id'>, {
      onSuccess: () => setShowCreate(false),
    });
  }

  function handleUpdate(data: ClassFormData): void {
    if (!editClass) return;
    updateMutation.mutate(
      { id: editClass.id, cls: data },
      { onSuccess: () => setEditClass(null) },
    );
  }

  function handleDelete(id: number): void {
    deleteMutation.mutate(id, {
      onSuccess: () => {
        if (selectedId === id) setSelectedId(null);
        if (editClass?.id === id) setEditClass(null);
      },
    });
  }

  if (isLoading) {
    return <div className="classes-page"><p aria-live="polite">Laddar klasser…</p></div>;
  }

  if (isError) {
    return (
      <div className="classes-page">
        <p className="classes-form__error">Kunde inte ladda klasser: {error.message}</p>
      </div>
    );
  }

  const selectedClass = selectedId != null ? classes?.find((c) => c.id === selectedId) : null;

  return (
    <div className="classes-page">
      <div className="classes-page__header">
        <h2>Klasser</h2>
        <div className="classes-page__actions">
          <button className="classes-btn classes-btn--primary" onClick={() => setShowCreate(true)}>
            Ny klass
          </button>
          {selectedClass && (
            <>
              <button
                className="classes-btn classes-btn--secondary"
                onClick={() => setEditClass(selectedClass)}
              >
                Redigera
              </button>
              <button
                className="classes-btn classes-btn--danger classes-btn--sm"
                onClick={() => handleDelete(selectedClass.id)}
                disabled={deleteMutation.isPending}
              >
                Ta bort
              </button>
            </>
          )}
        </div>
      </div>

      <div className="classes-toolbar">
        <input
          className="classes-toolbar__search"
          type="text"
          placeholder="Sök klassnamn…"
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          aria-label="Sök klass"
        />
        <select
          className="classes-toolbar__select"
          value={filterType}
          onChange={(e) => setFilterType(e.target.value)}
          aria-label="Filtrera typ"
        >
          <option value="">Alla typer</option>
          {uniqueTypes.map((t) => (
            <option key={t} value={t}>{t}</option>
          ))}
        </select>
      </div>

      <div className="classes-table-wrap">
        <table className="classes-table">
          <thead>
            <tr>
              <th onClick={() => handleSort('name')}>
                Namn<span className="sort-indicator">{sortIndicator('name')}</span>
              </th>
              <th onClick={() => handleSort('type')}>
                Typ<span className="sort-indicator">{sortIndicator('type')}</span>
              </th>
              <th onClick={() => handleSort('numStages')}>
                Sträckor<span className="sort-indicator">{sortIndicator('numStages')}</span>
              </th>
              <th onClick={() => handleSort('course')}>
                Bana<span className="sort-indicator">{sortIndicator('course')}</span>
              </th>
            </tr>
          </thead>
          <tbody>
            {sorted.length === 0 ? (
              <tr>
                <td colSpan={4} className="classes-table__empty">
                  {classes && classes.length > 0
                    ? 'Inga klasser matchar filtret.'
                    : 'Inga klasser registrerade.'}
                </td>
              </tr>
            ) : (
              sorted.map((c) => (
                <tr
                  key={c.id}
                  className={selectedId === c.id ? 'classes-table__row--selected' : ''}
                  onClick={() => setSelectedId(selectedId === c.id ? null : c.id)}
                >
                  <td>{c.name}</td>
                  <td>{c.type || '—'}</td>
                  <td>{c.numStages}</td>
                  <td>{courseMap.get(c.courseId) ?? '—'}</td>
                </tr>
              ))
            )}
          </tbody>
        </table>
      </div>

      <p className="classes-count" role="status">
        {sorted.length} av {classes?.length ?? 0} klasser
      </p>

      {showCreate && (
        <ClassFormModal
          title="Ny klass"
          initial={emptyForm()}
          onClose={() => { setShowCreate(false); createMutation.reset(); }}
          onSubmit={handleCreate}
          isPending={createMutation.isPending}
          error={createMutation.isError ? createMutation.error.message : null}
          courses={courseList}
        />
      )}

      {editClass && (
        <ClassFormModal
          title="Redigera klass"
          initial={formFromClass(editClass)}
          onClose={() => { setEditClass(null); updateMutation.reset(); }}
          onSubmit={handleUpdate}
          isPending={updateMutation.isPending}
          error={updateMutation.isError ? updateMutation.error.message : null}
          courses={courseList}
        />
      )}
    </div>
  );
}

export default ClassesPage;
