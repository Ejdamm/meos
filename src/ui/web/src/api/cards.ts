import apiClient from './client';
import type { Card } from '../types';

export interface ReadCardRequest {
  cardNo: number;
}

export async function getCards(): Promise<Card[]> {
  const { data } = await apiClient.get<Card[]>('/cards');
  return data;
}

export async function getCard(id: number): Promise<Card> {
  const { data } = await apiClient.get<Card>(`/cards/${id}`);
  return data;
}

export async function readCard(request: ReadCardRequest): Promise<Card> {
  const { data } = await apiClient.post<Card>('/cards/read', request);
  return data;
}
