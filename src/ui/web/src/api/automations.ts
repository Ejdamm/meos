import apiClient from './client';

export interface Automation {
  id: number;
  name: string;
  type: string;
  enabled: boolean;
  [key: string]: unknown;
}

export interface AutomationStatus {
  id: number;
  running: boolean;
  lastRun?: string;
  [key: string]: unknown;
}

export async function getAutomations(): Promise<Automation[]> {
  const { data } = await apiClient.get<Automation[]>('/automations');
  return data;
}

export async function getAutomation(id: number): Promise<Automation> {
  const { data } = await apiClient.get<Automation>(`/automations/${id}`);
  return data;
}

export async function createAutomation(automation: Omit<Automation, 'id'>): Promise<Automation> {
  const { data } = await apiClient.post<Automation>('/automations', automation);
  return data;
}

export async function updateAutomation(
  id: number,
  automation: Partial<Automation>
): Promise<Automation> {
  const { data } = await apiClient.put<Automation>(`/automations/${id}`, automation);
  return data;
}

export async function deleteAutomation(id: number): Promise<void> {
  await apiClient.delete(`/automations/${id}`);
}

export async function getAutomationStatus(id: number): Promise<AutomationStatus> {
  const { data } = await apiClient.get<AutomationStatus>(`/automations/${id}/status`);
  return data;
}
