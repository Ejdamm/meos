import { describe, it, expect, vi } from 'vitest';
import { renderHook } from '@testing-library/react';
import { useKeyboardShortcuts, useEscapeKey } from '../hooks/useKeyboardShortcuts';

function fireKey(
  key: string,
  opts: Partial<KeyboardEventInit> = {},
  target?: HTMLElement,
) {
  const event = new KeyboardEvent('keydown', {
    key,
    bubbles: true,
    cancelable: true,
    ...opts,
  });
  (target ?? document).dispatchEvent(event);
  return event;
}

describe('useKeyboardShortcuts', () => {
  it('calls handler on matching key', () => {
    const handler = vi.fn();
    renderHook(() =>
      useKeyboardShortcuts([{ key: 'Escape', handler, global: true }]),
    );
    fireKey('Escape');
    expect(handler).toHaveBeenCalledTimes(1);
  });

  it('does not call handler on non-matching key', () => {
    const handler = vi.fn();
    renderHook(() =>
      useKeyboardShortcuts([{ key: 'Escape', handler, global: true }]),
    );
    fireKey('Enter');
    expect(handler).not.toHaveBeenCalled();
  });

  it('matches Ctrl modifier', () => {
    const handler = vi.fn();
    renderHook(() =>
      useKeyboardShortcuts([{ key: 's', ctrl: true, handler }]),
    );
    fireKey('s', { ctrlKey: true });
    expect(handler).toHaveBeenCalledTimes(1);
  });

  it('does not fire Ctrl shortcut without Ctrl pressed', () => {
    const handler = vi.fn();
    renderHook(() =>
      useKeyboardShortcuts([{ key: 's', ctrl: true, handler }]),
    );
    fireKey('s');
    expect(handler).not.toHaveBeenCalled();
  });

  it('matches Meta key as Ctrl (macOS)', () => {
    const handler = vi.fn();
    renderHook(() =>
      useKeyboardShortcuts([{ key: 'n', ctrl: true, handler }]),
    );
    fireKey('n', { metaKey: true });
    expect(handler).toHaveBeenCalledTimes(1);
  });

  it('matches Shift modifier', () => {
    const handler = vi.fn();
    renderHook(() =>
      useKeyboardShortcuts([{ key: 'n', ctrl: true, shift: true, handler }]),
    );
    fireKey('n', { ctrlKey: true, shiftKey: true });
    expect(handler).toHaveBeenCalledTimes(1);
  });

  it('prevents default on matched shortcut', () => {
    const handler = vi.fn();
    renderHook(() =>
      useKeyboardShortcuts([{ key: 's', ctrl: true, handler }]),
    );
    const event = fireKey('s', { ctrlKey: true });
    expect(event.defaultPrevented).toBe(true);
  });

  it('skips non-global shortcut when target is input', () => {
    const handler = vi.fn();
    renderHook(() =>
      useKeyboardShortcuts([{ key: 'Escape', handler, global: false }]),
    );
    const input = document.createElement('input');
    document.body.appendChild(input);
    fireKey('Escape', {}, input);
    expect(handler).not.toHaveBeenCalled();
    document.body.removeChild(input);
  });

  it('fires Ctrl shortcuts even in inputs (global by default)', () => {
    const handler = vi.fn();
    renderHook(() =>
      useKeyboardShortcuts([{ key: 's', ctrl: true, handler }]),
    );
    const input = document.createElement('input');
    document.body.appendChild(input);
    fireKey('s', { ctrlKey: true }, input);
    expect(handler).toHaveBeenCalledTimes(1);
    document.body.removeChild(input);
  });

  it('skips textarea for non-global shortcuts', () => {
    const handler = vi.fn();
    renderHook(() =>
      useKeyboardShortcuts([{ key: 'a', handler, global: false }]),
    );
    const ta = document.createElement('textarea');
    document.body.appendChild(ta);
    fireKey('a', {}, ta);
    expect(handler).not.toHaveBeenCalled();
    document.body.removeChild(ta);
  });

  it('supports multiple shortcuts', () => {
    const save = vi.fn();
    const newItem = vi.fn();
    renderHook(() =>
      useKeyboardShortcuts([
        { key: 's', ctrl: true, handler: save },
        { key: 'n', ctrl: true, handler: newItem },
      ]),
    );
    fireKey('s', { ctrlKey: true });
    fireKey('n', { ctrlKey: true });
    expect(save).toHaveBeenCalledTimes(1);
    expect(newItem).toHaveBeenCalledTimes(1);
  });

  it('cleans up listener on unmount', () => {
    const handler = vi.fn();
    const { unmount } = renderHook(() =>
      useKeyboardShortcuts([{ key: 'Escape', handler, global: true }]),
    );
    unmount();
    fireKey('Escape');
    expect(handler).not.toHaveBeenCalled();
  });

  it('is case-insensitive for key matching', () => {
    const handler = vi.fn();
    renderHook(() =>
      useKeyboardShortcuts([{ key: 'S', ctrl: true, handler }]),
    );
    fireKey('s', { ctrlKey: true });
    expect(handler).toHaveBeenCalledTimes(1);
  });
});

describe('useEscapeKey', () => {
  it('calls onClose when Escape is pressed', () => {
    const onClose = vi.fn();
    renderHook(() => useEscapeKey(onClose));
    fireKey('Escape');
    expect(onClose).toHaveBeenCalledTimes(1);
  });

  it('fires even when input is focused (global)', () => {
    const onClose = vi.fn();
    renderHook(() => useEscapeKey(onClose));
    const input = document.createElement('input');
    document.body.appendChild(input);
    fireKey('Escape', {}, input);
    expect(onClose).toHaveBeenCalledTimes(1);
    document.body.removeChild(input);
  });

  it('does not fire on other keys', () => {
    const onClose = vi.fn();
    renderHook(() => useEscapeKey(onClose));
    fireKey('Enter');
    expect(onClose).not.toHaveBeenCalled();
  });
});
