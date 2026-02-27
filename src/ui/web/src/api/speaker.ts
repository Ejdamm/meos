import apiClient from './client';

export interface SpeakerConfig {
  [key: string]: unknown;
}

export interface SpeakerMonitorData {
  [key: string]: unknown;
}

export async function getSpeakerConfig(): Promise<SpeakerConfig> {
  const { data } = await apiClient.get<SpeakerConfig>('/speaker/config');
  return data;
}

export async function updateSpeakerConfig(config: SpeakerConfig): Promise<SpeakerConfig> {
  const { data } = await apiClient.put<SpeakerConfig>('/speaker/config', config);
  return data;
}

export async function getSpeakerMonitor(): Promise<SpeakerMonitorData> {
  const { data } = await apiClient.get<SpeakerMonitorData>('/speaker/monitor');
  return data;
}
