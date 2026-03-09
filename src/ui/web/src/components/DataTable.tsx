import React, { useState, useMemo } from 'react';
import { ChevronUp, ChevronDown, ChevronsUpDown, Search, ChevronLeft, ChevronRight } from 'lucide-react';
import { cn } from '../lib/utils';
import { Checkbox } from './ui/Checkbox';

export interface Column<T> {
  header: string;
  accessorKey: keyof T;
  cell?: (item: T) => React.ReactNode;
  sortable?: boolean;
  className?: string;
}

interface DataTableProps<T> {
  columns: Column<T>[];
  data: T[];
  pageSize?: number;
  onRowClick?: (item: T) => void;
  isLoading?: boolean;
  className?: string;
  enableSelection?: boolean;
  selectedItems?: T[];
  onSelectionChange?: (items: T[]) => void;
  getItemId?: (item: T) => string | number;
}

type SortConfig<T> = {
  key: keyof T;
  direction: 'asc' | 'desc';
} | null;

export function DataTable<T>({
  columns,
  data,
  pageSize = 10,
  onRowClick,
  isLoading,
  className,
  enableSelection,
  selectedItems = [],
  onSelectionChange,
  getItemId = (item: any) => item.id,
}: DataTableProps<T>) {
  const [sortConfig, setSortConfig] = useState<SortConfig<T>>(null);
  const [filterQuery, setFilterQuery] = useState('');
  const [currentPage, setCurrentPage] = useState(1);

  // Sorting logic
  const handleSort = (key: keyof T) => {
    let direction: 'asc' | 'desc' = 'asc';
    if (sortConfig && sortConfig.key === key && sortConfig.direction === 'asc') {
      direction = 'desc';
    }
    setSortConfig({ key, direction });
  };

  // Filtering and Sorting logic combined
  const filteredAndSortedData = useMemo(() => {
    let result = [...data];

    // Filter
    if (filterQuery) {
      result = result.filter((item) =>
        Object.values(item as object).some((val) =>
          String(val).toLowerCase().includes(filterQuery.toLowerCase())
        )
      );
    }

    // Sort
    if (sortConfig) {
      result.sort((a, b) => {
        const aVal = a[sortConfig.key];
        const bVal = b[sortConfig.key];

        if (aVal < bVal) return sortConfig.direction === 'asc' ? -1 : 1;
        if (aVal > bVal) return sortConfig.direction === 'asc' ? 1 : -1;
        return 0;
      });
    }

    return result;
  }, [data, filterQuery, sortConfig]);

  // Selection logic
  const isAllSelected = useMemo(() => {
    if (filteredAndSortedData.length === 0) return false;
    return filteredAndSortedData.every(item => 
      selectedItems.some(selected => getItemId(selected) === getItemId(item))
    );
  }, [filteredAndSortedData, selectedItems, getItemId]);

  const toggleSelectAll = () => {
    if (!onSelectionChange) return;
    if (isAllSelected) {
      const remainingSelected = selectedItems.filter(selected => 
        !filteredAndSortedData.some(item => getItemId(item) === getItemId(selected))
      );
      onSelectionChange(remainingSelected);
    } else {
      const newItems = filteredAndSortedData.filter(item => 
        !selectedItems.some(selected => getItemId(selected) === getItemId(item))
      );
      onSelectionChange([...selectedItems, ...newItems]);
    }
  };

  const toggleSelectItem = (item: T) => {
    if (!onSelectionChange) return;
    const isSelected = selectedItems.some(selected => getItemId(selected) === getItemId(item));
    if (isSelected) {
      onSelectionChange(selectedItems.filter(selected => getItemId(selected) !== getItemId(item)));
    } else {
      onSelectionChange([...selectedItems, item]);
    }
  };

  // Pagination logic
  const totalPages = Math.ceil(filteredAndSortedData.length / pageSize);
  const paginatedData = useMemo(() => {
    const start = (currentPage - 1) * pageSize;
    return filteredAndSortedData.slice(start, start + pageSize);
  }, [filteredAndSortedData, currentPage, pageSize]);

  // Reset to first page when filtering or sorting changes
  useMemo(() => {
    setCurrentPage(1);
  }, [filterQuery, sortConfig]);

  return (
    <div className={cn("space-y-4", className)}>
      {/* Search Input */}
      <div className="relative">
        <div className="absolute inset-y-0 left-0 flex items-center pl-3 pointer-events-none">
          <Search className="w-4 h-4 text-gray-500" />
        </div>
        <input
          type="text"
          className="block w-full p-2 pl-10 text-sm text-gray-900 border border-gray-300 rounded-lg bg-gray-50 focus:ring-blue-500 focus:border-blue-500 dark:bg-gray-700 dark:border-gray-600 dark:placeholder-gray-400 dark:text-white dark:focus:ring-blue-500 dark:focus:border-blue-500"
          placeholder="Search..."
          value={filterQuery}
          onChange={(e) => setFilterQuery(e.target.value)}
        />
      </div>

      {/* Table Container */}
      <div className="relative overflow-x-auto shadow-md sm:rounded-lg">
        <table className="w-full text-sm text-left text-gray-500 dark:text-gray-400">
          <thead className="text-xs text-gray-700 uppercase bg-gray-50 dark:bg-gray-700 dark:text-gray-400">
            <tr>
              {enableSelection && (
                <th scope="col" className="p-4">
                  <div className="flex items-center">
                    <Checkbox 
                      checked={isAllSelected} 
                      onChange={toggleSelectAll}
                    />
                  </div>
                </th>
              )}
              {columns.map((column) => (
                <th
                  key={String(column.accessorKey)}
                  scope="col"
                  className={cn(
                    "px-6 py-3",
                    column.sortable && "cursor-pointer select-none"
                  )}
                  onClick={() => column.sortable && handleSort(column.accessorKey)}
                >
                  <div className="flex items-center">
                    {column.header}
                    {column.sortable && (
                      <span className="ml-1">
                        {sortConfig?.key === column.accessorKey ? (
                          sortConfig.direction === 'asc' ? <ChevronUp className="w-4 h-4" /> : <ChevronDown className="w-4 h-4" />
                        ) : (
                          <ChevronsUpDown className="w-4 h-4 text-gray-400" />
                        )}
                      </span>
                    )}
                  </div>
                </th>
              ))}
            </tr>
          </thead>
          <tbody>
            {isLoading ? (
              <tr>
                <td colSpan={columns.length + (enableSelection ? 1 : 0)} className="px-6 py-4 text-center">
                  Loading...
                </td>
              </tr>
            ) : paginatedData.length > 0 ? (
              paginatedData.map((item, idx) => {
                const isSelected = selectedItems.some(selected => getItemId(selected) === getItemId(item));
                return (
                  <tr
                    key={idx}
                    className={cn(
                      "bg-white border-b dark:bg-gray-800 dark:border-gray-700 hover:bg-gray-50 dark:hover:bg-gray-600",
                      onRowClick && "cursor-pointer",
                      isSelected && "bg-blue-50 dark:bg-blue-900/20"
                    )}
                    onClick={() => onRowClick && onRowClick(item)}
                  >
                    {enableSelection && (
                      <td className="w-4 p-4" onClick={(e) => e.stopPropagation()}>
                        <div className="flex items-center">
                          <Checkbox 
                            checked={isSelected}
                            onChange={() => toggleSelectItem(item)}
                          />
                        </div>
                      </td>
                    )}
                    {columns.map((column) => (
                      <td key={String(column.accessorKey)} className="px-6 py-4">
                        {column.cell ? column.cell(item) : String((item as any)[column.accessorKey] ?? '')}
                      </td>
                    ))}
                  </tr>
                );
              })
            ) : (
              <tr>
                <td colSpan={columns.length + (enableSelection ? 1 : 0)} className="px-6 py-4 text-center">
                  No data found.
                </td>
              </tr>
            )}
          </tbody>
        </table>
      </div>

      {/* Pagination Controls */}
      {totalPages > 1 && (
        <div className="flex items-center justify-between px-2">
          <div className="text-sm text-gray-700 dark:text-gray-400">
            Showing <span className="font-semibold">{Math.min((currentPage - 1) * pageSize + 1, filteredAndSortedData.length)}</span> to <span className="font-semibold">{Math.min(currentPage * pageSize, filteredAndSortedData.length)}</span> of <span className="font-semibold">{filteredAndSortedData.length}</span> entries
          </div>
          <div className="inline-flex -space-x-px">
            <button
              disabled={currentPage === 1}
              onClick={() => setCurrentPage(currentPage - 1)}
              className="px-3 py-2 ml-0 leading-tight text-gray-500 bg-white border border-gray-300 rounded-l-lg hover:bg-gray-100 hover:text-gray-700 disabled:opacity-50 disabled:cursor-not-allowed dark:bg-gray-800 dark:border-gray-700 dark:text-gray-400 dark:hover:bg-gray-700 dark:hover:text-white"
            >
              <ChevronLeft className="w-4 h-4" />
            </button>
            <button
              disabled={currentPage === totalPages}
              onClick={() => setCurrentPage(currentPage + 1)}
              className="px-3 py-2 leading-tight text-gray-500 bg-white border border-gray-300 rounded-r-lg hover:bg-gray-100 hover:text-gray-700 disabled:opacity-50 disabled:cursor-not-allowed dark:bg-gray-800 dark:border-gray-700 dark:text-gray-400 dark:hover:bg-gray-700 dark:hover:text-white"
            >
              <ChevronRight className="w-4 h-4" />
            </button>
          </div>
        </div>
      )}
    </div>
  );
}
