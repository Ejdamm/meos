import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query';
import { importCsv, importIof, exportCsv, exportIof } from '../api';

const importExportKeys = {
  exportCsv: ['export', 'csv'] as const,
  exportIof: ['export', 'iof'] as const,
};

export function useImportCsv() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (file: File) => importCsv(file),
    onSuccess: () => {
      queryClient.invalidateQueries();
    },
  });
}

export function useImportIof() {
  const queryClient = useQueryClient();
  return useMutation({
    mutationFn: (file: File) => importIof(file),
    onSuccess: () => {
      queryClient.invalidateQueries();
    },
  });
}

export function useExportCsv() {
  return useQuery({
    queryKey: importExportKeys.exportCsv,
    queryFn: exportCsv,
    enabled: false,
  });
}

export function useExportIof() {
  return useQuery({
    queryKey: importExportKeys.exportIof,
    queryFn: exportIof,
    enabled: false,
  });
}
