import type { ReactNode } from 'react';
import { useEscapeKey } from '../hooks/useKeyboardShortcuts';
import { useFocusTrap } from '../hooks/useFocusTrap';
import './Modal.css';

interface ModalProps {
  /** Visible title rendered in the modal header */
  title: string;
  /** Called when the modal should close (ESC, backdrop click, cancel) */
  onClose: () => void;
  children: ReactNode;
}

/**
 * Accessible modal dialog with focus trap, ESC-to-close, backdrop click,
 * and focus restoration on unmount.
 */
export function Modal({ title, onClose, children }: ModalProps): React.JSX.Element {
  useEscapeKey(onClose);
  const trapRef = useFocusTrap<HTMLDivElement>();

  const titleId = 'modal-title';

  return (
    <div className="modal-overlay" onClick={onClose}>
      <div
        className="modal-dialog"
        ref={trapRef}
        role="dialog"
        aria-modal="true"
        aria-labelledby={titleId}
        onClick={(e) => e.stopPropagation()}
      >
        <h3 id={titleId} className="modal-dialog__title">{title}</h3>
        {children}
      </div>
    </div>
  );
}
