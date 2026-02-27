import { describe, it, expect, beforeEach } from 'vitest';
import { http, HttpResponse } from 'msw';
import { server } from '../test/server';
import apiClient, { ApiRequestError } from '../api/client';

describe('API client', () => {
  beforeEach(() => {
    server.resetHandlers();
  });

  describe('successful responses', () => {
    it('returns data on 200 OK', async () => {
      server.use(
        http.get('/api/test', () => HttpResponse.json({ value: 42 })),
      );
      const res = await apiClient.get('/test');
      expect(res.data).toEqual({ value: 42 });
    });

    it('sends correct default headers', async () => {
      let capturedAccept = '';
      let capturedContentType = '';
      server.use(
        http.get('/api/test-headers', ({ request }) => {
          capturedAccept = request.headers.get('accept') ?? '';
          return HttpResponse.json({ ok: true });
        }),
        http.post('/api/test-headers', ({ request }) => {
          capturedContentType = request.headers.get('content-type') ?? '';
          return HttpResponse.json({ ok: true });
        }),
      );
      await apiClient.get('/test-headers');
      expect(capturedAccept).toBe('application/json');
      await apiClient.post('/test-headers', { x: 1 });
      expect(capturedContentType).toContain('application/json');
    });

    it('uses /api as base URL', async () => {
      let capturedUrl = '';
      server.use(
        http.get('/api/some/path', ({ request }) => {
          capturedUrl = new URL(request.url).pathname;
          return HttpResponse.json({});
        }),
      );
      await apiClient.get('/some/path');
      expect(capturedUrl).toBe('/api/some/path');
    });
  });

  describe('error interceptor', () => {
    it('throws ApiRequestError on 401', async () => {
      server.use(
        http.get('/api/protected', () =>
          new HttpResponse(null, { status: 401 }),
        ),
      );
      await expect(apiClient.get('/protected')).rejects.toThrow(ApiRequestError);
      try {
        await apiClient.get('/protected');
      } catch (err) {
        const e = err as ApiRequestError;
        expect(e.status).toBe(401);
        expect(e.message).toBe('Ej autentiserad');
      }
    });

    it('throws ApiRequestError on 403', async () => {
      server.use(
        http.get('/api/forbidden', () =>
          new HttpResponse(null, { status: 403 }),
        ),
      );
      try {
        await apiClient.get('/forbidden');
        expect.unreachable('Should have thrown');
      } catch (err) {
        const e = err as ApiRequestError;
        expect(e.status).toBe(403);
        expect(e.message).toBe('Åtkomst nekad');
      }
    });

    it('throws ApiRequestError on 404', async () => {
      server.use(
        http.get('/api/missing', () =>
          new HttpResponse(null, { status: 404 }),
        ),
      );
      try {
        await apiClient.get('/missing');
        expect.unreachable('Should have thrown');
      } catch (err) {
        const e = err as ApiRequestError;
        expect(e.status).toBe(404);
        expect(e.message).toBe('Resursen hittades inte');
      }
    });

    it('throws ApiRequestError on 500 with generic message', async () => {
      server.use(
        http.get('/api/error', () =>
          new HttpResponse(null, { status: 500 }),
        ),
      );
      try {
        await apiClient.get('/error');
        expect.unreachable('Should have thrown');
      } catch (err) {
        const e = err as ApiRequestError;
        expect(e.status).toBe(500);
        expect(e.message).toBe('Internt serverfel');
      }
    });

    it('extracts error message from response body when available', async () => {
      server.use(
        http.get('/api/custom-error', () =>
          HttpResponse.json(
            { message: 'Anpassat felmeddelande', detail: 'Extra detaljer' },
            { status: 422 },
          ),
        ),
      );
      try {
        await apiClient.get('/custom-error');
        expect.unreachable('Should have thrown');
      } catch (err) {
        const e = err as ApiRequestError;
        expect(e.status).toBe(422);
        expect(e.message).toBe('Anpassat felmeddelande');
        expect(e.detail).toBe('Extra detaljer');
      }
    });

    it('uses default message for unknown status codes without body message', async () => {
      server.use(
        http.get('/api/unknown', () =>
          new HttpResponse(null, { status: 418 }),
        ),
      );
      try {
        await apiClient.get('/unknown');
        expect.unreachable('Should have thrown');
      } catch (err) {
        const e = err as ApiRequestError;
        expect(e.status).toBe(418);
        expect(e.message).toContain('418');
      }
    });

    it('throws ApiRequestError on 400 Bad Request', async () => {
      server.use(
        http.post('/api/bad', () =>
          new HttpResponse(null, { status: 400 }),
        ),
      );
      try {
        await apiClient.post('/bad', {});
        expect.unreachable('Should have thrown');
      } catch (err) {
        const e = err as ApiRequestError;
        expect(e.status).toBe(400);
        expect(e.message).toBe('Ogiltig förfrågan');
      }
    });

    it('throws ApiRequestError on 409 Conflict', async () => {
      server.use(
        http.put('/api/conflict', () =>
          new HttpResponse(null, { status: 409 }),
        ),
      );
      try {
        await apiClient.put('/conflict', {});
        expect.unreachable('Should have thrown');
      } catch (err) {
        const e = err as ApiRequestError;
        expect(e.status).toBe(409);
        expect(e.message).toBe('Konflikt');
      }
    });

    it('handles network error (no response)', async () => {
      server.use(
        http.get('/api/network-fail', () => HttpResponse.error()),
      );
      try {
        await apiClient.get('/network-fail');
        expect.unreachable('Should have thrown');
      } catch (err) {
        const e = err as ApiRequestError;
        expect(e.status).toBe(0);
        expect(e.message).toBeTruthy();
      }
    });
  });

  describe('ApiRequestError class', () => {
    it('has correct name property', () => {
      const err = new ApiRequestError({ status: 500, message: 'test' });
      expect(err.name).toBe('ApiRequestError');
    });

    it('is an instance of Error', () => {
      const err = new ApiRequestError({ status: 404, message: 'not found' });
      expect(err).toBeInstanceOf(Error);
    });

    it('preserves detail when provided', () => {
      const err = new ApiRequestError({
        status: 400,
        message: 'bad',
        detail: 'field x invalid',
      });
      expect(err.detail).toBe('field x invalid');
    });

    it('detail is undefined when not provided', () => {
      const err = new ApiRequestError({ status: 400, message: 'bad' });
      expect(err.detail).toBeUndefined();
    });
  });
});
