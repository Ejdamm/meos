import type { ReactElement, ReactNode } from 'react';
import { render, type RenderOptions } from '@testing-library/react';
import { QueryClient, QueryClientProvider } from '@tanstack/react-query';
import { MemoryRouter, type MemoryRouterProps } from 'react-router-dom';
import { ToastProvider } from '../components/Toast';

interface WrapperOptions {
  routerProps?: MemoryRouterProps;
  queryClient?: QueryClient;
}

function createTestQueryClient() {
  return new QueryClient({
    defaultOptions: {
      queries: {
        retry: false,
        gcTime: 0,
      },
      mutations: {
        retry: false,
      },
    },
  });
}

function createWrapper({ routerProps, queryClient }: WrapperOptions = {}) {
  const client = queryClient ?? createTestQueryClient();
  return function Wrapper({ children }: { children: ReactNode }) {
    return (
      <QueryClientProvider client={client}>
        <ToastProvider>
          <MemoryRouter {...routerProps}>{children}</MemoryRouter>
        </ToastProvider>
      </QueryClientProvider>
    );
  };
}

export function renderWithProviders(
  ui: ReactElement,
  options?: Omit<RenderOptions, 'wrapper'> & { wrapperOptions?: WrapperOptions },
) {
  const { wrapperOptions, ...renderOptions } = options ?? {};
  return render(ui, {
    wrapper: createWrapper(wrapperOptions),
    ...renderOptions,
  });
}

export { createTestQueryClient };
