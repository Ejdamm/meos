import * as React from 'react';
import { useStartList } from '../hooks/useStartList';
import { useClasses } from '../hooks/useClasses';
import { useActiveCompetition } from '../hooks/useCompetition';
import { DataTable } from '../components/DataTable';
import type { Column } from '../components/DataTable';
import type { StartListEntry } from '../api/types';
import { FormSelect } from '../components/FormSelect';
import { Button } from '../components/ui/Button';
import { FileJson, FileSpreadsheet } from 'lucide-react';
import { api } from '../api/client';
import Papa from 'papaparse';

export default function StartListPage() {
  const [selectedClassId, setSelectedClassId] = React.useState<number | undefined>(undefined);
  const { startList, isLoading: isLoadingStartList } = useStartList({ classId: selectedClassId });
  const { data: classes } = useClasses();
  const { data: competition } = useActiveCompetition();

  const exportCSV = () => {
    const csvData = startList.map(r => ({
      'Start Time': r.startTime,
      Name: r.name,
      Club: r.clubName || '',
      Class: r.className || '',
      Card: r.cardNumber || ''
    }));

    const csv = Papa.unparse(csvData);
    const blob = new Blob([csv], { type: 'text/csv;charset=utf-8;' });
    const link = document.createElement('a');
    const filename = `startlist_${competition?.name || 'competition'}_${competition?.date || ''}.csv`.replace(/\s+/g, '_').toLowerCase();
    
    const url = URL.createObjectURL(blob);
    link.setAttribute('href', url);
    link.setAttribute('download', filename);
    link.style.visibility = 'hidden';
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  };

  const exportXML = () => {
    const url = api.exportStartListXML({ classId: selectedClassId });
    window.open(url, '_blank');
  };

  const classOptions = [
    { label: 'All Classes', value: '' },
    ...(classes?.map(c => ({ label: c.name, value: String(c.id) })) || [])
  ];

  const columns: Column<StartListEntry>[] = [
    { header: 'Start Time', accessorKey: 'startTime', sortable: true },
    { header: 'Name', accessorKey: 'name', sortable: true },
    { header: 'Club', accessorKey: 'clubName', sortable: true },
    { header: 'Card', accessorKey: 'cardNumber', sortable: true },
  ];

  // Group by class if "All Classes" is selected
  const startListByClass = React.useMemo(() => {
    if (selectedClassId !== undefined) return null;
    
    const groups: Record<string, StartListEntry[]> = {};
    startList.forEach(r => {
      const className = r.className || 'Unknown';
      if (!groups[className]) groups[className] = [];
      groups[className].push(r);
    });
    return groups;
  }, [startList, selectedClassId]);

  return (
    <div className="container mx-auto py-8 px-4 sm:px-6 lg:px-8 print:p-0 print:m-0">
      <div className="flex flex-col md:flex-row justify-between items-start md:items-center mb-6 gap-4 print:hidden">
        <h1 className="text-3xl font-bold">Start List</h1>
        <div className="flex flex-wrap items-center gap-4">
          <div className="flex items-center gap-2">
            <Button variant="outline" size="sm" onClick={exportCSV}>
              <FileSpreadsheet className="w-4 h-4 mr-2" />
              CSV
            </Button>
            <Button variant="outline" size="sm" onClick={exportXML}>
              <FileJson className="w-4 h-4 mr-2" />
              IOF XML
            </Button>
          </div>
          <div className="w-64">
            <FormSelect
              label=""
              options={classOptions}
              value={selectedClassId !== undefined ? String(selectedClassId) : ''}
              onValueChange={(val) => setSelectedClassId(val ? Number(val) : undefined)}
            />
          </div>
        </div>
      </div>

      <div className="print:block">
        <h2 className="text-2xl font-bold mb-4 hidden print:block">
          Start List - {selectedClassId ? classes?.find(c => c.id === selectedClassId)?.name : 'All Classes'}
        </h2>

        {selectedClassId !== undefined ? (
          <DataTable
            columns={columns}
            data={startList}
            isLoading={isLoadingStartList}
          />
        ) : (
          Object.entries(startListByClass || {}).map(([className, entries]) => (
            <div key={className} className="mb-8 break-inside-avoid">
              <h3 className="text-xl font-semibold mb-2 border-b pb-1">{className}</h3>
              <DataTable
                columns={columns}
                data={entries}
                isLoading={isLoadingStartList}
              />
            </div>
          ))
        )}
      </div>

      <style dangerouslySetInnerHTML={{ __html: `
        @media print {
          body { color: black; background: white; }
          .container { max-width: none; width: 100%; }
          button { display: none !important; }
          nav { display: none !important; }
          header { display: none !important; }
          .print\\:hidden { display: none !important; }
          .DataTable-header { background-color: transparent !important; border-bottom: 2px solid black; }
          .DataTable-row { border-bottom: 1px solid #eee; page-break-inside: avoid; }
        }
      `}} />
    </div>
  );
}
