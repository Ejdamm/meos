import { useState, useRef, useCallback, type FormEvent, type DragEvent, type ChangeEvent } from 'react';
import { useCompetitions, useUpdateCompetition, useCreateCompetition, useImportCsv, useImportIof, useExportCsv, useExportIof } from '../hooks';
import type { Competition } from '../types';
import './CompetitionPage.css';

interface CompetitionFormData {
  name: string;
  date: string;
  zeroTime: string;
}

interface FormErrors {
  name?: string;
  date?: string;
  zeroTime?: string;
}

function validateForm(data: CompetitionFormData): FormErrors {
  const errors: FormErrors = {};
  if (!data.name.trim()) {
    errors.name = 'Namn krävs';
  }
  if (data.date && !/^\d{4}-\d{2}-\d{2}$/.test(data.date)) {
    errors.date = 'Ogiltigt datumformat (ÅÅÅÅ-MM-DD)';
  }
  if (data.zeroTime && !/^\d{2}:\d{2}(:\d{2})?$/.test(data.zeroTime)) {
    errors.zeroTime = 'Ogiltigt tidsformat (HH:MM eller HH:MM:SS)';
  }
  return errors;
}

function CompetitionInfo({ competition }: { competition: Competition }): React.JSX.Element {
  return (
    <div className="comp-stats">
      <div className="comp-stats__item">
        <span className="comp-stats__value">{competition.numRunners}</span>
        <span className="comp-stats__label">Deltagare</span>
      </div>
      <div className="comp-stats__item">
        <span className="comp-stats__value">{competition.numClasses}</span>
        <span className="comp-stats__label">Klasser</span>
      </div>
      <div className="comp-stats__item">
        <span className="comp-stats__value">{competition.numCourses}</span>
        <span className="comp-stats__label">Banor</span>
      </div>
      <div className="comp-stats__item">
        <span className="comp-stats__value">{competition.numCards}</span>
        <span className="comp-stats__label">Brickor</span>
      </div>
    </div>
  );
}

function EditForm({
  competition,
  onCancel,
  onSaved,
}: {
  competition: Competition;
  onCancel: () => void;
  onSaved: () => void;
}): React.JSX.Element {
  const [formData, setFormData] = useState<CompetitionFormData>({
    name: competition.name,
    date: competition.date,
    zeroTime: competition.zeroTime,
  });
  const [errors, setErrors] = useState<FormErrors>({});
  const updateMutation = useUpdateCompetition();

  function handleSubmit(e: FormEvent): void {
    e.preventDefault();
    const validationErrors = validateForm(formData);
    if (Object.keys(validationErrors).length > 0) {
      setErrors(validationErrors);
      return;
    }
    setErrors({});
    updateMutation.mutate(
      { id: competition.id, competition: formData },
      { onSuccess: () => onSaved() }
    );
  }

  return (
    <form className="comp-form" onSubmit={handleSubmit}>
      <div className="comp-form__field">
        <label className="comp-form__label" htmlFor="comp-name">Namn</label>
        <input
          id="comp-name"
          className={`comp-form__input ${errors.name ? 'comp-form__input--error' : ''}`}
          type="text"
          value={formData.name}
          onChange={(e) => setFormData({ ...formData, name: e.target.value })}
        />
        {errors.name && <span className="comp-form__error">{errors.name}</span>}
      </div>
      <div className="comp-form__field">
        <label className="comp-form__label" htmlFor="comp-date">Datum</label>
        <input
          id="comp-date"
          className={`comp-form__input ${errors.date ? 'comp-form__input--error' : ''}`}
          type="date"
          value={formData.date}
          onChange={(e) => setFormData({ ...formData, date: e.target.value })}
        />
        {errors.date && <span className="comp-form__error">{errors.date}</span>}
      </div>
      <div className="comp-form__field">
        <label className="comp-form__label" htmlFor="comp-zero-time">Nolltid</label>
        <input
          id="comp-zero-time"
          className={`comp-form__input ${errors.zeroTime ? 'comp-form__input--error' : ''}`}
          type="time"
          step="1"
          value={formData.zeroTime}
          onChange={(e) => setFormData({ ...formData, zeroTime: e.target.value })}
        />
        {errors.zeroTime && <span className="comp-form__error">{errors.zeroTime}</span>}
      </div>
      <div className="comp-form__actions">
        <button type="submit" className="comp-btn comp-btn--primary" disabled={updateMutation.isPending}>
          {updateMutation.isPending ? 'Sparar…' : 'Spara'}
        </button>
        <button type="button" className="comp-btn comp-btn--secondary" onClick={onCancel}>
          Avbryt
        </button>
      </div>
      {updateMutation.isError && (
        <p className="comp-form__error">{updateMutation.error.message}</p>
      )}
    </form>
  );
}

function CreateForm({ onCreated }: { onCreated: () => void }): React.JSX.Element {
  const [formData, setFormData] = useState<CompetitionFormData>({
    name: '',
    date: new Date().toISOString().slice(0, 10),
    zeroTime: '10:00:00',
  });
  const [errors, setErrors] = useState<FormErrors>({});
  const createMutation = useCreateCompetition();

  function handleSubmit(e: FormEvent): void {
    e.preventDefault();
    const validationErrors = validateForm(formData);
    if (Object.keys(validationErrors).length > 0) {
      setErrors(validationErrors);
      return;
    }
    setErrors({});
    createMutation.mutate(formData, { onSuccess: () => onCreated() });
  }

  return (
    <form className="comp-form" onSubmit={handleSubmit}>
      <h3 className="comp-form__title">Skapa ny tävling</h3>
      <div className="comp-form__field">
        <label className="comp-form__label" htmlFor="new-comp-name">Namn</label>
        <input
          id="new-comp-name"
          className={`comp-form__input ${errors.name ? 'comp-form__input--error' : ''}`}
          type="text"
          value={formData.name}
          placeholder="Tävlingens namn"
          onChange={(e) => setFormData({ ...formData, name: e.target.value })}
        />
        {errors.name && <span className="comp-form__error">{errors.name}</span>}
      </div>
      <div className="comp-form__field">
        <label className="comp-form__label" htmlFor="new-comp-date">Datum</label>
        <input
          id="new-comp-date"
          className={`comp-form__input ${errors.date ? 'comp-form__input--error' : ''}`}
          type="date"
          value={formData.date}
          onChange={(e) => setFormData({ ...formData, date: e.target.value })}
        />
        {errors.date && <span className="comp-form__error">{errors.date}</span>}
      </div>
      <div className="comp-form__field">
        <label className="comp-form__label" htmlFor="new-comp-zero-time">Nolltid</label>
        <input
          id="new-comp-zero-time"
          className={`comp-form__input ${errors.zeroTime ? 'comp-form__input--error' : ''}`}
          type="time"
          step="1"
          value={formData.zeroTime}
          onChange={(e) => setFormData({ ...formData, zeroTime: e.target.value })}
        />
        {errors.zeroTime && <span className="comp-form__error">{errors.zeroTime}</span>}
      </div>
      <div className="comp-form__actions">
        <button type="submit" className="comp-btn comp-btn--primary" disabled={createMutation.isPending}>
          {createMutation.isPending ? 'Skapar…' : 'Skapa tävling'}
        </button>
      </div>
      {createMutation.isError && (
        <p className="comp-form__error">{createMutation.error.message}</p>
      )}
    </form>
  );
}

function ImportExportSection(): React.JSX.Element {
  const [isDragging, setIsDragging] = useState(false);
  const [importFormat, setImportFormat] = useState<'iof' | 'csv'>('iof');
  const [statusMessage, setStatusMessage] = useState<{ text: string; type: 'success' | 'error' } | null>(null);
  const fileInputRef = useRef<HTMLInputElement>(null);

  const importCsvMutation = useImportCsv();
  const importIofMutation = useImportIof();
  const { refetch: refetchCsv, isFetching: isFetchingCsv } = useExportCsv();
  const { refetch: refetchIof, isFetching: isFetchingIof } = useExportIof();

  const isImporting = importCsvMutation.isPending || importIofMutation.isPending;

  const handleFile = useCallback((file: File) => {
    setStatusMessage(null);
    const mutation = importFormat === 'iof' ? importIofMutation : importCsvMutation;
    mutation.mutate(file, {
      onSuccess: (result) => {
        const count = result.imported != null ? ` (${result.imported} importerade)` : '';
        setStatusMessage({ text: `Import lyckades${count}`, type: 'success' });
      },
      onError: (err) => {
        setStatusMessage({ text: `Import misslyckades: ${err.message}`, type: 'error' });
      },
    });
  }, [importFormat, importCsvMutation, importIofMutation]);

  function handleDragOver(e: DragEvent): void {
    e.preventDefault();
    setIsDragging(true);
  }

  function handleDragLeave(e: DragEvent): void {
    e.preventDefault();
    setIsDragging(false);
  }

  function handleDrop(e: DragEvent): void {
    e.preventDefault();
    setIsDragging(false);
    const file = e.dataTransfer.files[0];
    if (file) handleFile(file);
  }

  function handleFileChange(e: ChangeEvent<HTMLInputElement>): void {
    const file = e.target.files?.[0];
    if (file) handleFile(file);
    if (fileInputRef.current) fileInputRef.current.value = '';
  }

  async function handleExport(format: 'iof' | 'csv'): Promise<void> {
    setStatusMessage(null);
    try {
      const refetch = format === 'iof' ? refetchIof : refetchCsv;
      const result = await refetch();
      if (result.data) {
        const blob = new Blob([result.data.data], { type: 'application/octet-stream' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = format === 'iof' ? 'export.xml' : 'export.csv';
        a.click();
        URL.revokeObjectURL(url);
        setStatusMessage({ text: `Export (${format.toUpperCase()}) klar`, type: 'success' });
      }
    } catch (err) {
      setStatusMessage({ text: `Export misslyckades: ${err instanceof Error ? err.message : 'Okänt fel'}`, type: 'error' });
    }
  }

  return (
    <div className="comp-io">
      <h3 className="comp-io__title">Import / Export</h3>

      <div className="comp-io__row">
        <div className="comp-io__import">
          <div className="comp-io__format-select">
            <label className="comp-form__label">Importformat</label>
            <div className="comp-io__format-btns">
              <button
                type="button"
                className={`comp-btn ${importFormat === 'iof' ? 'comp-btn--primary' : 'comp-btn--secondary'}`}
                onClick={() => setImportFormat('iof')}
              >
                IOF XML
              </button>
              <button
                type="button"
                className={`comp-btn ${importFormat === 'csv' ? 'comp-btn--primary' : 'comp-btn--secondary'}`}
                onClick={() => setImportFormat('csv')}
              >
                CSV
              </button>
            </div>
          </div>

          <div
            className={`comp-io__dropzone ${isDragging ? 'comp-io__dropzone--active' : ''}`}
            onDragOver={handleDragOver}
            onDragLeave={handleDragLeave}
            onDrop={handleDrop}
            onClick={() => fileInputRef.current?.click()}
            role="button"
            tabIndex={0}
            onKeyDown={(e) => { if (e.key === 'Enter' || e.key === ' ') fileInputRef.current?.click(); }}
          >
            <input
              ref={fileInputRef}
              type="file"
              className="comp-io__file-input"
              accept={importFormat === 'iof' ? '.xml,.iof' : '.csv'}
              onChange={handleFileChange}
            />
            {isImporting ? (
              <span>Importerar…</span>
            ) : (
              <>
                <span className="comp-io__dropzone-icon">📂</span>
                <span>Dra och släpp fil här, eller klicka för att välja</span>
                <span className="comp-io__dropzone-hint">{importFormat === 'iof' ? '.xml / .iof' : '.csv'}</span>
              </>
            )}
          </div>
        </div>

        <div className="comp-io__export">
          <label className="comp-form__label">Exportera</label>
          <div className="comp-io__export-btns">
            <button
              type="button"
              className="comp-btn comp-btn--primary"
              disabled={isFetchingIof}
              onClick={() => void handleExport('iof')}
            >
              {isFetchingIof ? 'Exporterar…' : 'IOF XML'}
            </button>
            <button
              type="button"
              className="comp-btn comp-btn--primary"
              disabled={isFetchingCsv}
              onClick={() => void handleExport('csv')}
            >
              {isFetchingCsv ? 'Exporterar…' : 'CSV'}
            </button>
          </div>
        </div>
      </div>

      {statusMessage && (
        <p className={`comp-io__status comp-io__status--${statusMessage.type}`}>
          {statusMessage.text}
        </p>
      )}
    </div>
  );
}

function CompetitionPage(): React.JSX.Element {
  const { data: competitions, isLoading, isError, error } = useCompetitions();
  const [isEditing, setIsEditing] = useState(false);
  const [showCreate, setShowCreate] = useState(false);

  const competition = competitions?.[0];
  const canEdit = isEditing && competition;

  if (isLoading) {
    return <div className="comp-page"><p aria-live="polite">Laddar tävlingsdata…</p></div>;
  }

  if (isError) {
    return (
      <div className="comp-page">
        <p className="comp-form__error">Kunde inte ladda tävling: {error.message}</p>
      </div>
    );
  }

  return (
    <div className="comp-page">
      <div className="comp-page__header">
        <h2>Tävling</h2>
        <div className="comp-page__actions">
          {competition && !canEdit && (
            <button className="comp-btn comp-btn--primary" onClick={() => setIsEditing(true)}>
              Redigera
            </button>
          )}
          <button
            className="comp-btn comp-btn--secondary"
            onClick={() => setShowCreate(!showCreate)}
          >
            {showCreate ? 'Avbryt' : 'Ny tävling'}
          </button>
        </div>
      </div>

      {showCreate && (
        <CreateForm onCreated={() => setShowCreate(false)} />
      )}

      {competition && !canEdit && (
        <div className="comp-detail">
          <div className="comp-detail__row">
            <span className="comp-detail__label">Namn</span>
            <span className="comp-detail__value">{competition.name}</span>
          </div>
          <div className="comp-detail__row">
            <span className="comp-detail__label">Datum</span>
            <span className="comp-detail__value">{competition.date}</span>
          </div>
          <div className="comp-detail__row">
            <span className="comp-detail__label">Nolltid</span>
            <span className="comp-detail__value">{competition.zeroTime}</span>
          </div>
          <CompetitionInfo competition={competition} />
        </div>
      )}

      {canEdit && (
        <EditForm
          competition={competition}
          onCancel={() => setIsEditing(false)}
          onSaved={() => setIsEditing(false)}
        />
      )}

      {competition && <ImportExportSection />}

      {!competition && !showCreate && (
        <p className="comp-page__empty">Ingen tävling laddad. Skapa en ny tävling för att komma igång.</p>
      )}
    </div>
  );
}

export default CompetitionPage;
