import { describe, it, expect } from 'vitest';
import { renderWithProviders, fixtures } from '../test';

describe('Test infrastructure smoke test', () => {
  it('renders a simple component with providers', () => {
    const { getByText } = renderWithProviders(<div>Hello MeOS</div>);
    expect(getByText('Hello MeOS')).toBeInTheDocument();
  });

  it('fixtures contain expected data', () => {
    expect(fixtures.runners.length).toBeGreaterThan(0);
    expect(fixtures.competitions.length).toBeGreaterThan(0);
    expect(fixtures.classes.length).toBeGreaterThan(0);
  });
});
