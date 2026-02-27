import axios, { type AxiosError, type AxiosResponse } from 'axios';

export interface ApiError {
  status: number;
  message: string;
  detail?: string;
}

export class ApiRequestError extends Error {
  readonly status: number;
  readonly detail?: string;

  constructor({ status, message, detail }: ApiError) {
    super(message);
    this.name = 'ApiRequestError';
    this.status = status;
    this.detail = detail;
  }
}

const apiClient = axios.create({
  baseURL: '/api',
  timeout: 15_000,
  headers: {
    'Content-Type': 'application/json',
    Accept: 'application/json',
  },
});

function extractErrorMessage(status: number, data: unknown): string {
  if (data && typeof data === 'object' && 'message' in data && typeof data.message === 'string') {
    return data.message;
  }
  switch (status) {
    case 400:
      return 'Ogiltig förfrågan';
    case 401:
      return 'Ej autentiserad';
    case 403:
      return 'Åtkomst nekad';
    case 404:
      return 'Resursen hittades inte';
    case 409:
      return 'Konflikt';
    case 500:
      return 'Internt serverfel';
    default:
      return `Oväntat fel (${String(status)})`;
  }
}

function extractDetail(data: unknown): string | undefined {
  if (data && typeof data === 'object' && 'detail' in data && typeof data.detail === 'string') {
    return data.detail;
  }
  return undefined;
}

apiClient.interceptors.response.use(
  (response: AxiosResponse) => response,
  (error: AxiosError) => {
    if (error.response) {
      const { status, data } = error.response;
      throw new ApiRequestError({
        status,
        message: extractErrorMessage(status, data),
        detail: extractDetail(data),
      });
    }

    if (error.request) {
      throw new ApiRequestError({
        status: 0,
        message: 'Kunde inte nå servern',
      });
    }

    throw new ApiRequestError({
      status: 0,
      message: error.message || 'Okänt fel',
    });
  },
);

export default apiClient;
