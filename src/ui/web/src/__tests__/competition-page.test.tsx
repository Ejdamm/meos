import { describe, it, expect } from 'vitest';
import { screen, waitFor } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { http, HttpResponse } from 'msw';
import { renderWithProviders, fixtures, server } from '../test';
import CompetitionPage from '../pages/CompetitionPage';

const competition = fixtures.competitions[0]!;

describe('CompetitionPage', () => {
  it('shows loading state initially', () => {
    renderWithProviders(<CompetitionPage />);
    expect(screen.getByText('Laddar tävlingsdata…')).toBeInTheDocument();
  });

  it('renders competition details after loading', async () => {
    renderWithProviders(<CompetitionPage />);

    await waitFor(() => {
      expect(screen.getByText(competition.name)).toBeInTheDocument();
    });
    expect(screen.getByText(competition.date)).toBeInTheDocument();
    expect(screen.getByText(competition.zeroTime)).toBeInTheDocument();
  });

  it('renders competition stats', async () => {
    renderWithProviders(<CompetitionPage />);

    await waitFor(() => {
      expect(screen.getByText(String(competition.numRunners))).toBeInTheDocument();
    });
    expect(screen.getByText('Deltagare')).toBeInTheDocument();
    expect(screen.getByText(String(competition.numClasses))).toBeInTheDocument();
    expect(screen.getByText('Klasser')).toBeInTheDocument();
    expect(screen.getByText(String(competition.numCourses))).toBeInTheDocument();
    expect(screen.getByText('Banor')).toBeInTheDocument();
    expect(screen.getByText(String(competition.numCards))).toBeInTheDocument();
    expect(screen.getByText('Brickor')).toBeInTheDocument();
  });

  it('shows error message on API failure', async () => {
    server.use(
      http.get('/api/competitions', () => HttpResponse.json({ message: 'Server error' }, { status: 500 })),
    );

    renderWithProviders(<CompetitionPage />);

    await waitFor(() => {
      expect(screen.getByText(/Kunde inte ladda tävling/)).toBeInTheDocument();
    });
  });

  it('shows empty state when no competitions exist', async () => {
    server.use(
      http.get('/api/competitions', () => HttpResponse.json([])),
    );

    renderWithProviders(<CompetitionPage />);

    await waitFor(() => {
      expect(screen.getByText(/Ingen tävling laddad/)).toBeInTheDocument();
    });
  });

  describe('Edit form', () => {
    it('opens edit form when clicking Redigera', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CompetitionPage />);

      await waitFor(() => {
        expect(screen.getByText('Redigera')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Redigera'));

      expect(screen.getByLabelText('Namn')).toBeInTheDocument();
      expect(screen.getByLabelText('Datum')).toBeInTheDocument();
      expect(screen.getByLabelText('Nolltid')).toBeInTheDocument();
    });

    it('populates edit form with existing data', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CompetitionPage />);

      await waitFor(() => {
        expect(screen.getByText('Redigera')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Redigera'));

      expect(screen.getByLabelText('Namn')).toHaveValue(competition.name);
      expect(screen.getByLabelText('Datum')).toHaveValue(competition.date);
      expect(screen.getByLabelText('Nolltid')).toHaveValue(competition.zeroTime);
    });

    it('submits updated competition via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CompetitionPage />);

      await waitFor(() => {
        expect(screen.getByText('Redigera')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Redigera'));

      const nameInput = screen.getByLabelText('Namn');
      await user.clear(nameInput);
      await user.type(nameInput, 'Updated Competition');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows validation error for empty name', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CompetitionPage />);

      await waitFor(() => {
        expect(screen.getByText('Redigera')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Redigera'));

      const nameInput = screen.getByLabelText('Namn');
      await user.clear(nameInput);
      await user.click(screen.getByText('Spara'));

      expect(screen.getByText('Namn krävs')).toBeInTheDocument();
    });

    it('cancels editing', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CompetitionPage />);

      await waitFor(() => {
        expect(screen.getByText('Redigera')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Redigera'));
      expect(screen.getByLabelText('Namn')).toBeInTheDocument();

      await user.click(screen.getByText('Avbryt'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
      expect(screen.getByText(competition.name)).toBeInTheDocument();
    });

    it('shows API error on update failure', async () => {
      server.use(
        http.put('/api/competitions/:id', () =>
          HttpResponse.json({ message: 'Update failed' }, { status: 500 }),
        ),
      );

      const user = userEvent.setup();
      renderWithProviders(<CompetitionPage />);

      await waitFor(() => {
        expect(screen.getByText('Redigera')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Redigera'));
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.getByText('Update failed')).toBeInTheDocument();
      });
    });
  });

  describe('Create form', () => {
    it('shows create form when clicking Ny tävling', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CompetitionPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny tävling')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny tävling'));

      expect(screen.getByText('Skapa ny tävling')).toBeInTheDocument();
      expect(screen.getByLabelText('Namn')).toBeInTheDocument();
    });

    it('submits new competition via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CompetitionPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny tävling')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny tävling'));

      await user.type(screen.getByLabelText('Namn'), 'New Competition');
      await user.click(screen.getByText('Skapa tävling'));

      await waitFor(() => {
        expect(screen.queryByText('Skapa ny tävling')).not.toBeInTheDocument();
      });
    });

    it('shows validation error on empty name in create form', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CompetitionPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny tävling')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny tävling'));
      await user.click(screen.getByText('Skapa tävling'));

      expect(screen.getByText('Namn krävs')).toBeInTheDocument();
    });

    it('hides create form when clicking Avbryt', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CompetitionPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny tävling')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny tävling'));
      expect(screen.getByText('Skapa ny tävling')).toBeInTheDocument();

      await user.click(screen.getByText('Avbryt'));
      expect(screen.queryByText('Skapa ny tävling')).not.toBeInTheDocument();
    });
  });

  describe('Import/Export section', () => {
    it('renders import/export section when competition exists', async () => {
      renderWithProviders(<CompetitionPage />);

      await waitFor(() => {
        expect(screen.getByText('Import / Export')).toBeInTheDocument();
      });
      expect(screen.getAllByText('IOF XML').length).toBeGreaterThanOrEqual(1);
      expect(screen.getAllByText('CSV').length).toBeGreaterThanOrEqual(1);
    });

    it('does not render import/export when no competition', async () => {
      server.use(
        http.get('/api/competitions', () => HttpResponse.json([])),
      );

      renderWithProviders(<CompetitionPage />);

      await waitFor(() => {
        expect(screen.getByText(/Ingen tävling laddad/)).toBeInTheDocument();
      });
      expect(screen.queryByText('Import / Export')).not.toBeInTheDocument();
    });

    it('switches import format between IOF and CSV', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CompetitionPage />);

      await waitFor(() => {
        expect(screen.getByText('Import / Export')).toBeInTheDocument();
      });

      const formatButtons = screen.getAllByText('CSV');
      const csvFormatButton = formatButtons.find(
        (btn) => btn.closest('.comp-io__format-btns'),
      );
      expect(csvFormatButton).toBeDefined();

      if (csvFormatButton) {
        await user.click(csvFormatButton);
        expect(screen.getByText('.csv')).toBeInTheDocument();
      }
    });
  });
});
