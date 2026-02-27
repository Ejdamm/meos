import apiClient from './client';

export interface ImportResult {
  success: boolean;
  message: string;
  imported?: number;
}

export interface ExportResult {
  data: string;
  format: string;
}

export async function importCsv(file: File): Promise<ImportResult> {
  const formData = new FormData();
  formData.append('file', file);
  const { data } = await apiClient.post<ImportResult>('/import/csv', formData, {
    headers: { 'Content-Type': 'multipart/form-data' },
  });
  return data;
}

export async function importIof(file: File): Promise<ImportResult> {
  const formData = new FormData();
  formData.append('file', file);
  const { data } = await apiClient.post<ImportResult>('/import/iof', formData, {
    headers: { 'Content-Type': 'multipart/form-data' },
  });
  return data;
}

export async function exportCsv(): Promise<ExportResult> {
  const { data } = await apiClient.get<ExportResult>('/export/csv');
  return data;
}

export async function exportIof(): Promise<ExportResult> {
  const { data } = await apiClient.get<ExportResult>('/export/iof');
  return data;
}
