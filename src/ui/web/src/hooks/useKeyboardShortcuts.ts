import { useEffect, useCallback, useRef, useLayoutEffect } from 'react';

export interface KeyboardShortcut {
  key: string;
  ctrl?: boolean;
  shift?: boolean;
  alt?: boolean;
  handler: () => void;
  /** If true, shortcut fires even when an input/textarea/select is focused */
  global?: boolean;
}

function isEditableTarget(target: EventTarget | null): boolean {
  if (!target || !(target instanceof HTMLElement)) return false;
  const tag = target.tagName;
  if (tag === 'INPUT' || tag === 'TEXTAREA' || tag === 'SELECT') return true;
  return target.isContentEditable;
}

/**
 * Register keyboard shortcuts. Shortcuts with ctrl/alt modifiers default to
 * global (fire even in inputs). Plain key shortcuts (like Escape) skip
 * editable elements unless `global` is explicitly set.
 */
export function useKeyboardShortcuts(shortcuts: KeyboardShortcut[]): void {
  const shortcutsRef = useRef(shortcuts);
  useLayoutEffect(() => {
    shortcutsRef.current = shortcuts;
  }, [shortcuts]);

  const handleKeyDown = useCallback((e: KeyboardEvent) => {
    for (const shortcut of shortcutsRef.current) {
      const wantCtrl = shortcut.ctrl ?? false;
      const wantShift = shortcut.shift ?? false;
      const wantAlt = shortcut.alt ?? false;

      if (e.key.toLowerCase() !== shortcut.key.toLowerCase()) continue;
      if (e.ctrlKey !== wantCtrl && e.metaKey !== wantCtrl) continue;
      if (e.shiftKey !== wantShift) continue;
      if (e.altKey !== wantAlt) continue;

      const isGlobal = shortcut.global ?? (wantCtrl || wantAlt);
      if (!isGlobal && isEditableTarget(e.target)) continue;

      e.preventDefault();
      shortcut.handler();
      return;
    }
  }, []);

  useEffect(() => {
    document.addEventListener('keydown', handleKeyDown);
    return () => document.removeEventListener('keydown', handleKeyDown);
  }, [handleKeyDown]);
}

/**
 * Escape key handler for modals/dialogs. Calls `onClose` when Escape is
 * pressed anywhere (global by default).
 */
export function useEscapeKey(onClose: () => void): void {
  useKeyboardShortcuts([
    { key: 'Escape', handler: onClose, global: true },
  ]);
}
