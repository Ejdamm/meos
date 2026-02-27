import { describe, it, expect, vi } from 'vitest';
import { screen, within } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { renderWithProviders } from '../test';
import { Modal } from '../components/Modal';

describe('Modal', () => {
  it('renders with role="dialog" and aria-modal', () => {
    renderWithProviders(
      <Modal title="Test Dialog" onClose={vi.fn()}>
        <p>Content</p>
      </Modal>,
    );

    const dialog = screen.getByRole('dialog');
    expect(dialog).toBeInTheDocument();
    expect(dialog).toHaveAttribute('aria-modal', 'true');
  });

  it('renders title with aria-labelledby', () => {
    renderWithProviders(
      <Modal title="My Title" onClose={vi.fn()}>
        <p>Content</p>
      </Modal>,
    );

    const dialog = screen.getByRole('dialog');
    const titleId = dialog.getAttribute('aria-labelledby');
    expect(titleId).toBeTruthy();

    const titleEl = document.getElementById(titleId!);
    expect(titleEl).toHaveTextContent('My Title');
  });

  it('calls onClose when Escape is pressed', async () => {
    const user = userEvent.setup();
    const onClose = vi.fn();

    renderWithProviders(
      <Modal title="Escape Test" onClose={onClose}>
        <button>Focusable</button>
      </Modal>,
    );

    await user.keyboard('{Escape}');
    expect(onClose).toHaveBeenCalledTimes(1);
  });

  it('calls onClose when overlay is clicked', async () => {
    const user = userEvent.setup();
    const onClose = vi.fn();

    const { container } = renderWithProviders(
      <Modal title="Overlay Test" onClose={onClose}>
        <p>Inside</p>
      </Modal>,
    );

    // Click overlay (the outermost modal div)
    const overlay = container.querySelector('.modal-overlay')!;
    await user.click(overlay);
    expect(onClose).toHaveBeenCalled();
  });

  it('does NOT call onClose when dialog content is clicked', async () => {
    const user = userEvent.setup();
    const onClose = vi.fn();

    renderWithProviders(
      <Modal title="Content Click" onClose={onClose}>
        <p>Click me</p>
      </Modal>,
    );

    await user.click(screen.getByText('Click me'));
    expect(onClose).not.toHaveBeenCalled();
  });

  it('traps focus within the modal (Tab wraps)', async () => {
    const user = userEvent.setup();

    renderWithProviders(
      <Modal title="Focus Trap" onClose={vi.fn()}>
        <input data-testid="first" />
        <button data-testid="last">OK</button>
      </Modal>,
    );

    const firstInput = screen.getByTestId('first');
    const lastButton = screen.getByTestId('last');

    // Focus should start on first focusable element
    expect(document.activeElement).toBe(firstInput);

    // Tab to last element
    await user.tab();
    expect(document.activeElement).toBe(lastButton);

    // Tab again should wrap to first
    await user.tab();
    expect(document.activeElement).toBe(firstInput);
  });

  it('traps focus within the modal (Shift+Tab wraps)', async () => {
    const user = userEvent.setup();

    renderWithProviders(
      <Modal title="Focus Trap Reverse" onClose={vi.fn()}>
        <input data-testid="first" />
        <button data-testid="last">OK</button>
      </Modal>,
    );

    const firstInput = screen.getByTestId('first');
    const lastButton = screen.getByTestId('last');

    // Focus starts on first
    expect(document.activeElement).toBe(firstInput);

    // Shift+Tab should wrap to last
    await user.tab({ shift: true });
    expect(document.activeElement).toBe(lastButton);
  });

  it('renders children inside the dialog', () => {
    renderWithProviders(
      <Modal title="Children" onClose={vi.fn()}>
        <span data-testid="child">Hello</span>
      </Modal>,
    );

    const dialog = screen.getByRole('dialog');
    expect(within(dialog).getByTestId('child')).toHaveTextContent('Hello');
  });
});
