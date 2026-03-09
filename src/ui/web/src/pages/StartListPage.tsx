import * as React from 'react';
import { useStartList } from '../hooks/useStartList';
import { useClasses } from '../hooks/useClasses';
import { DataTable } from '../components/DataTable';
import type { Column } from '../components/DataTable';
import type { StartListEntry } from '../api/types';
import { FormSelect } from '../components/FormSelect';

export default function StartListPage() {
  const [selectedClassId, setSelectedClassId] = React.useState<number | undefined>(undefined);
  const { startList, isLoading: isLoadingStartList } = useStartList({ classId: selectedClassId });
  const { data: classes } = useClasses();

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
      <div className="flex justify-between items-center mb-6 print:hidden">
        <h1 className="text-3xl font-bold">Start List</h1>
        <div className="w-64">
          <FormSelect
            label="Filter by Class"
            options={classOptions}
            value={selectedClassId !== undefined ? String(selectedClassId) : ''}
            onValueChange={(val) => setSelectedClassId(val ? Number(val) : undefined)}
          />
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
