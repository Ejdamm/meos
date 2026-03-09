import * as React from 'react';
import { useResults } from '../hooks/useResults';
import { useClasses } from '../hooks/useClasses';
import type { Column } from '../components/DataTable';
import { FormSelect } from '../components/FormSelect';
import { RunnerStatus } from '../api/types';
import type { Result, Split } from '../api/types';
import { ChevronDown, ChevronRight } from 'lucide-react';
import { cn } from '../lib/utils';

export default function ResultsPage() {
  const [selectedClassId, setSelectedClassId] = React.useState<number | undefined>(undefined);
  const [expandedRunners, setExpandedRunners] = React.useState<Set<number>>(new Set());
  const { results, isLoading: isLoadingResults } = useResults({ classId: selectedClassId });
  const { data: classes } = useClasses();

  const classOptions = [
    { label: 'All Classes', value: '' },
    ...(classes?.map(c => ({ label: c.name, value: String(c.id) })) || [])
  ];

  const toggleRunner = (runnerId: number) => {
    const newExpanded = new Set(expandedRunners);
    if (newExpanded.has(runnerId)) {
      newExpanded.delete(runnerId);
    } else {
      newExpanded.add(runnerId);
    }
    setExpandedRunners(newExpanded);
  };

  const getStatusText = (status: RunnerStatus) => {
    switch (status) {
      case RunnerStatus.OK: return '';
      case RunnerStatus.MP: return 'MP';
      case RunnerStatus.DNF: return 'DNF';
      case RunnerStatus.DQ: return 'DQ';
      case RunnerStatus.DNS: return 'DNS';
      default: return 'Other';
    }
  };

  const columns: Column<Result>[] = [
    { 
      header: '', 
      accessorKey: 'runnerId',
      cell: (r) => (
        r.splits && r.splits.length > 0 ? (
          expandedRunners.has(r.runnerId) ? <ChevronDown className="w-4 h-4" /> : <ChevronRight className="w-4 h-4" />
        ) : null
      ),
      className: 'w-8 print:hidden'
    },
    { 
      header: 'Place', 
      accessorKey: 'place',
      cell: (r) => r.status === RunnerStatus.OK ? r.place : getStatusText(r.status),
      className: 'w-16'
    },
    { header: 'Name', accessorKey: 'name', sortable: true },
    { header: 'Club', accessorKey: 'clubName', sortable: true },
    { 
      header: 'Time', 
      accessorKey: 'runningTime',
      cell: (r) => r.status === RunnerStatus.OK ? r.runningTime : '',
    },
    { 
      header: 'After', 
      accessorKey: 'timeAfter',
      cell: (r) => (r.status === RunnerStatus.OK && r.timeAfter !== '00:00:00') ? `+${r.timeAfter}` : '',
    },
  ];

  const renderSplits = (splits: Split[]) => {
    return (
      <div className="bg-gray-50 dark:bg-gray-900/50 p-4 rounded-lg mt-2 mb-4 overflow-x-auto print:bg-transparent print:p-0 print:m-0">
        <table className="w-full text-xs text-left">
          <thead>
            <tr className="border-b dark:border-gray-700">
              <th className="py-1 pr-4">Control</th>
              <th className="py-1 pr-4">Time</th>
              <th className="py-1 pr-4">Place</th>
              <th className="py-1">After</th>
            </tr>
          </thead>
          <tbody>
            {splits.map((s, idx) => (
              <tr key={idx} className="border-b border-gray-100 dark:border-gray-800 last:border-0">
                <td className="py-1 pr-4 font-mono">{s.controlName}</td>
                <td className="py-1 pr-4 font-mono">{s.time}</td>
                <td className="py-1 pr-4">{s.place}</td>
                <td className="py-1 font-mono text-gray-500">{s.timeAfter && s.timeAfter !== '00:00:00' ? `+${s.timeAfter}` : ''}</td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    );
  };

  // Custom row renderer to handle expansion
  const renderRow = (r: Result) => {
    const isExpanded = expandedRunners.has(r.runnerId);
    return (
      <React.Fragment key={r.runnerId}>
        <tr 
          className={cn(
            "bg-white border-b dark:bg-gray-800 dark:border-gray-700 hover:bg-gray-50 dark:hover:bg-gray-600 cursor-pointer",
            isExpanded && "bg-blue-50/30 dark:bg-blue-900/10"
          )}
          onClick={() => r.splits && r.splits.length > 0 && toggleRunner(r.runnerId)}
        >
          {columns.map((col, idx) => (
            <td key={idx} className={cn("px-6 py-4", col.className)}>
              {col.cell ? col.cell(r) : String((r as any)[col.accessorKey] ?? '')}
            </td>
          ))}
        </tr>
        {isExpanded && r.splits && (
          <tr className="print:table-row">
            <td colSpan={columns.length} className="px-6 py-0">
              {renderSplits(r.splits)}
            </td>
          </tr>
        )}
      </React.Fragment>
    );
  };

  // Group by class if "All Classes" is selected
  const resultsByClass = React.useMemo(() => {
    if (selectedClassId !== undefined) return null;
    
    const groups: Record<string, Result[]> = {};
    results.forEach(r => {
      const className = r.className || 'Unknown';
      if (!groups[className]) groups[className] = [];
      groups[className].push(r);
    });
    return groups;
  }, [results, selectedClassId]);

  return (
    <div className="container mx-auto py-8 px-4 sm:px-6 lg:px-8 print:p-0 print:m-0">
      <div className="flex justify-between items-center mb-6 print:hidden">
        <h1 className="text-3xl font-bold">Results</h1>
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
          Results - {selectedClassId ? classes?.find(c => c.id === selectedClassId)?.name : 'All Classes'}
        </h2>

        {selectedClassId !== undefined ? (
          <div className="relative overflow-x-auto shadow-md sm:rounded-lg">
            <table className="w-full text-sm text-left text-gray-500 dark:text-gray-400">
              <thead className="text-xs text-gray-700 uppercase bg-gray-50 dark:bg-gray-700 dark:text-gray-400">
                <tr>
                  {columns.map((col, idx) => (
                    <th key={idx} className={cn("px-6 py-3", col.className)}>{col.header}</th>
                  ))}
                </tr>
              </thead>
              <tbody>
                {isLoadingResults ? (
                  <tr><td colSpan={columns.length} className="px-6 py-4 text-center">Loading...</td></tr>
                ) : results.length > 0 ? (
                  results.map(renderRow)
                ) : (
                  <tr><td colSpan={columns.length} className="px-6 py-4 text-center">No results found.</td></tr>
                )}
              </tbody>
            </table>
          </div>
        ) : (
          Object.entries(resultsByClass || {}).map(([className, classResults]) => (
            <div key={className} className="mb-8 break-inside-avoid">
              <h3 className="text-xl font-semibold mb-2 border-b pb-1">{className}</h3>
              <div className="relative overflow-x-auto shadow-md sm:rounded-lg">
                <table className="w-full text-sm text-left text-gray-500 dark:text-gray-400">
                  <thead className="text-xs text-gray-700 uppercase bg-gray-50 dark:bg-gray-700 dark:text-gray-400">
                    <tr>
                      {columns.map((col, idx) => (
                        <th key={idx} className={cn("px-6 py-3", col.className)}>{col.header}</th>
                      ))}
                    </tr>
                  </thead>
                  <tbody>
                    {classResults.map(renderRow)}
                  </tbody>
                </table>
              </div>
            </div>
          ))
        )}
      </div>

      <style dangerouslySetInnerHTML={{ __html: `
        @media print {
          body { color: black; background: white; font-size: 10pt; }
          .container { max-width: none; width: 100%; padding: 0; margin: 0; }
          .shadow-md { shadow: none !important; box-shadow: none !important; }
          .rounded-lg { border-radius: 0 !important; }
          button { display: none !important; }
          nav { display: none !important; }
          header { display: none !important; }
          .print\\:hidden { display: none !important; }
          th { background-color: #f3f4f6 !important; border-bottom: 2px solid black; -webkit-print-color-adjust: exact; }
          tr { border-bottom: 1px solid #eee; page-break-inside: avoid; }
          .bg-gray-50 { background-color: transparent !important; }
          .break-inside-avoid { page-break-inside: avoid; }
        }
      `}} />
    </div>
  );
}
