import * as React from 'react';
import { Upload, AlertCircle, ArrowLeft } from 'lucide-react';
import Papa from 'papaparse';
import {
  Dialog,
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
} from './ui/Dialog';
import { Button } from './ui/Button';
import { DataTable } from './DataTable';
import type { Column } from './DataTable';
import { useBulkCreateRunners, useImportXMLRunners } from '../hooks/useRunners';
import { RunnerStatus } from '../api/types';
import type { Runner } from '../api/types';
import { cn } from '../lib/utils';

interface ImportRunnersDialogProps {
  open: boolean;
  onOpenChange: (open: boolean) => void;
  onImportComplete: () => void;
}

type Step = 'upload' | 'preview';

export function ImportRunnersDialog({ open, onOpenChange, onImportComplete }: ImportRunnersDialogProps) {
  const [step, setStep] = React.useState<Step>('upload');
  const [file, setFile] = React.useState<File | null>(null);
  const [importedRunners, setImportedRunners] = React.useState<Partial<Runner>[]>([]);
  const [error, setError] = React.useState<string | null>(null);

  const bulkCreateMutation = useBulkCreateRunners();
  const importXMLMutation = useImportXMLRunners();

  const reset = () => {
    setStep('upload');
    setFile(null);
    setImportedRunners([]);
    setError(null);
  };

  React.useEffect(() => {
    if (!open) {
      reset();
    }
  }, [open]);

  const handleFileChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const selectedFile = e.target.files?.[0];
    if (selectedFile) {
      setFile(selectedFile);
      setError(null);
    }
  };

  const parseCSV = (file: File) => {
    Papa.parse(file, {
      header: true,
      skipEmptyLines: true,
      complete: (results) => {
        const data = results.data as any[];
        const mapped: Partial<Runner>[] = data.map((row) => ({
          name: row.name || row.Name || row.full_name || row['Full Name'] || '',
          bib: row.bib || row.Bib || row.number || row.Number || '',
          clubName: row.club || row.Club || row.organization || row.Organization || '',
          className: row.class || row.Class || row.category || row.Category || '',
          cardNumber: row.card || row.Card || row.si_card || row['SI Card'] ? Number(row.card || row.Card || row.si_card || row['SI Card']) : undefined,
          status: RunnerStatus.OK,
        }));

        const validRunners = mapped.filter(r => r.name);
        if (validRunners.length === 0) {
          setError('No valid runners found in CSV. Make sure you have a "name" column.');
          return;
        }

        setImportedRunners(validRunners);
        setStep('preview');
      },
      error: (err) => {
        setError(`Error parsing CSV: ${err.message}`);
      }
    });
  };

  const handleUpload = async () => {
    if (!file) return;

    setError(null);
    if (file.name.endsWith('.csv')) {
      parseCSV(file);
    } else if (file.name.endsWith('.xml')) {
      try {
        const result = await importXMLMutation.mutate(file);
        setImportedRunners(result);
        setStep('preview');
      } catch (err: any) {
        setError(`Error importing XML: ${err.message}`);
      }
    } else {
      setError('Unsupported file format. Please upload a .csv or .xml file.');
    }
  };

  const handleImport = async () => {
    try {
      await bulkCreateMutation.mutate(importedRunners);
      onImportComplete();
      onOpenChange(false);
    } catch (err: any) {
      setError(`Error during bulk import: ${err.message}`);
    }
  };

  const columns: Column<Partial<Runner>>[] = [
    { header: 'Name', accessorKey: 'name' },
    { header: 'Club', accessorKey: 'clubName' },
    { header: 'Class', accessorKey: 'className' },
    { header: 'Bib', accessorKey: 'bib' },
    { header: 'Card', accessorKey: 'cardNumber' },
  ];

  return (
    <Dialog open={open} onOpenChange={onOpenChange}>
      <DialogContent className={cn(step === 'preview' ? 'sm:max-w-[1000px]' : 'sm:max-w-[500px]')}>
        <DialogHeader>
          <DialogTitle>Import Runners</DialogTitle>
          <DialogDescription>
            {step === 'upload' 
              ? 'Upload a CSV or IOF XML file to import runners.' 
              : `Review the ${importedRunners.length} runners to be imported.`}
          </DialogDescription>
        </DialogHeader>

        {step === 'upload' ? (
          <div className="py-6">
            <div 
              className="border-2 border-dashed rounded-lg p-10 flex flex-col items-center justify-center space-y-4 cursor-pointer hover:bg-accent transition-colors"
              onClick={() => document.getElementById('file-upload')?.click()}
            >
              <Upload className="w-12 h-12 text-muted-foreground" />
              <div className="text-center">
                <p className="font-medium">{file ? file.name : 'Click to select or drag and drop'}</p>
                <p className="text-sm text-muted-foreground">CSV or IOF XML (up to 10MB)</p>
              </div>
              <input 
                id="file-upload" 
                type="file" 
                className="hidden" 
                accept=".csv,.xml" 
                onChange={handleFileChange} 
              />
            </div>

            {error && (
              <div className="mt-4 p-3 rounded-md bg-destructive/10 text-destructive text-sm flex items-start space-x-2">
                <AlertCircle className="w-4 h-4 mt-0.5" />
                <span>{error}</span>
              </div>
            )}

            <div className="mt-6 flex flex-col space-y-2">
              <h4 className="text-sm font-semibold">CSV Format Guide</h4>
              <p className="text-xs text-muted-foreground">
                Headers supported: name, club, class, bib, card. "name" is required.
              </p>
            </div>
          </div>
        ) : (
          <div className="py-4">
            <div className="max-h-[500px] overflow-auto border rounded-md">
              <DataTable 
                columns={columns as any} 
                data={importedRunners as any} 
                pageSize={10}
              />
            </div>
            {error && (
              <div className="mt-4 p-3 rounded-md bg-destructive/10 text-destructive text-sm flex items-start space-x-2">
                <AlertCircle className="w-4 h-4 mt-0.5" />
                <span>{error}</span>
              </div>
            )}
          </div>
        )}

        <DialogFooter>
          {step === 'upload' ? (
            <>
              <Button variant="outline" onClick={() => onOpenChange(false)}>Cancel</Button>
              <Button onClick={handleUpload} disabled={!file || importXMLMutation.isLoading}>
                {importXMLMutation.isLoading ? 'Processing...' : 'Next: Preview'}
              </Button>
            </>
          ) : (
            <>
              <Button variant="ghost" onClick={() => setStep('upload')} disabled={bulkCreateMutation.isLoading}>
                <ArrowLeft className="w-4 h-4 mr-2" />
                Back
              </Button>
              <div className="flex-1" />
              <Button variant="outline" onClick={() => onOpenChange(false)} disabled={bulkCreateMutation.isLoading}>Cancel</Button>
              <Button onClick={handleImport} disabled={bulkCreateMutation.isLoading}>
                {bulkCreateMutation.isLoading ? 'Importing...' : 'Confirm Import'}
              </Button>
            </>
          )}
        </DialogFooter>
      </DialogContent>
    </Dialog>
  );
}
