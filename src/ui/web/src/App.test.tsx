import { render, screen } from '@testing-library/react';
import { expect, test } from 'vitest';
import App from './App';

test('renders placeholder text', () => {
  render(<App />);
  const linkElement = screen.getByText(/MeOS Modernization/i);
  expect(linkElement).toBeInTheDocument();
});
