import { describe, it, expect, vi, beforeEach, afterEach } from 'vitest';
import { screen, fireEvent, act } from '@testing-library/react';
import { renderWithProviders } from '../test';
import { useToast } from '../components/Toast';

function ToastTrigger() {
  const { addToast } = useToast();
  return (
    <div>
      <button onClick={() => addToast('success', 'Sparad!')}>Success</button>
      <button onClick={() => addToast('error', 'Fel uppstod')}>Error</button>
      <button onClick={() => addToast('info', 'Information')}>Info</button>
    </div>
  );
}

describe('Toast notifications', () => {
  beforeEach(() => {
    vi.useFakeTimers();
  });

  afterEach(() => {
    vi.useRealTimers();
  });

  it('renders a success toast', () => {
    renderWithProviders(<ToastTrigger />);
    fireEvent.click(screen.getByText('Success'));

    expect(screen.getByText('Sparad!')).toBeInTheDocument();
    expect(screen.getByText('Sparad!').closest('.toast')).toHaveClass('toast--success');
  });

  it('renders an error toast', () => {
    renderWithProviders(<ToastTrigger />);
    fireEvent.click(screen.getByText('Error'));

    expect(screen.getByText('Fel uppstod')).toBeInTheDocument();
    expect(screen.getByText('Fel uppstod').closest('.toast')).toHaveClass('toast--error');
  });

  it('renders an info toast', () => {
    renderWithProviders(<ToastTrigger />);
    fireEvent.click(screen.getByText('Info'));

    expect(screen.getByText('Information')).toBeInTheDocument();
    expect(screen.getByText('Information').closest('.toast')).toHaveClass('toast--info');
  });

  it('auto-dismisses after 4 seconds', () => {
    renderWithProviders(<ToastTrigger />);
    fireEvent.click(screen.getByText('Success'));
    expect(screen.getByText('Sparad!')).toBeInTheDocument();

    act(() => {
      vi.advanceTimersByTime(4100);
    });

    expect(screen.queryByText('Sparad!')).not.toBeInTheDocument();
  });

  it('dismisses on close button click', () => {
    renderWithProviders(<ToastTrigger />);
    fireEvent.click(screen.getByText('Success'));
    expect(screen.getByText('Sparad!')).toBeInTheDocument();

    fireEvent.click(screen.getByLabelText('Stäng'));
    expect(screen.queryByText('Sparad!')).not.toBeInTheDocument();
  });

  it('can show multiple toasts simultaneously', () => {
    renderWithProviders(<ToastTrigger />);
    fireEvent.click(screen.getByText('Success'));
    fireEvent.click(screen.getByText('Error'));

    expect(screen.getByText('Sparad!')).toBeInTheDocument();
    expect(screen.getByText('Fel uppstod')).toBeInTheDocument();
  });

  it('has aria-live polite container', () => {
    renderWithProviders(<ToastTrigger />);
    fireEvent.click(screen.getByText('Success'));

    const container = screen.getByText('Sparad!').closest('.toast-container');
    expect(container).toHaveAttribute('aria-live', 'polite');
  });
});
