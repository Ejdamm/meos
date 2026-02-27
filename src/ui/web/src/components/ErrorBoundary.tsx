import { Component } from 'react';
import type { ErrorInfo, ReactNode } from 'react';

interface ErrorBoundaryProps {
  children: ReactNode;
}

interface ErrorBoundaryState {
  hasError: boolean;
  error: Error | null;
}

class ErrorBoundary extends Component<ErrorBoundaryProps, ErrorBoundaryState> {
  constructor(props: ErrorBoundaryProps) {
    super(props);
    this.state = { hasError: false, error: null };
  }

  static getDerivedStateFromError(error: Error): ErrorBoundaryState {
    return { hasError: true, error };
  }

  componentDidCatch(error: Error, errorInfo: ErrorInfo): void {
    console.error('ErrorBoundary caught an error:', error, errorInfo);
  }

  private handleRetry = (): void => {
    this.setState({ hasError: false, error: null });
  };

  render(): ReactNode {
    if (this.state.hasError) {
      return (
        <div role="alert" style={styles.container}>
          <h2 style={styles.heading}>Något gick fel</h2>
          <p style={styles.message}>
            {this.state.error?.message ?? 'Ett oväntat fel inträffade.'}
          </p>
          <button onClick={this.handleRetry} style={styles.button}>
            Försök igen
          </button>
        </div>
      );
    }

    return this.props.children;
  }
}

const styles: Record<string, React.CSSProperties> = {
  container: {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'center',
    justifyContent: 'center',
    padding: '2rem',
    minHeight: '50vh',
    textAlign: 'center',
  },
  heading: {
    fontSize: '1.5rem',
    marginBottom: '0.5rem',
  },
  message: {
    color: '#666',
    marginBottom: '1.5rem',
    maxWidth: '480px',
  },
  button: {
    padding: '0.5rem 1.5rem',
    fontSize: '1rem',
    cursor: 'pointer',
    border: '1px solid #ccc',
    borderRadius: '4px',
    background: '#fff',
  },
};

export default ErrorBoundary;
