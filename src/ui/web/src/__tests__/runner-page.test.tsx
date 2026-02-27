import { describe, it, expect } from 'vitest';
import { screen, waitFor, within } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { http, HttpResponse } from 'msw';
import { renderWithProviders, fixtures, server } from '../test';
import RunnersPage from '../pages/RunnersPage';

const runner1 = fixtures.runners[0]!;
const runner2 = fixtures.runners[1]!;

describe('RunnersPage', () => {
  it('shows loading state initially', () => {
    renderWithProviders(<RunnersPage />);
    expect(screen.getByText('Laddar deltagare…')).toBeInTheDocument();
  });

  it('renders runner list after loading', async () => {
    renderWithProviders(<RunnersPage />);

    await waitFor(() => {
      expect(screen.getByText(runner1.name)).toBeInTheDocument();
    });
    expect(screen.getByText(runner2.name)).toBeInTheDocument();
  });

  it('renders runner details in table', async () => {
    renderWithProviders(<RunnersPage />);

    await waitFor(() => {
      expect(screen.getByText(runner1.name)).toBeInTheDocument();
    });
    expect(screen.getByText(runner1.bib)).toBeInTheDocument();
    expect(screen.getByText(String(runner1.cardNo))).toBeInTheDocument();
    expect(screen.getAllByText('OK')).toHaveLength(2); // both runners have status 1
  });

  it('shows count of runners', async () => {
    renderWithProviders(<RunnersPage />);

    await waitFor(() => {
      expect(screen.getByText('2 av 2 deltagare')).toBeInTheDocument();
    });
  });

  it('shows error message on API failure', async () => {
    server.use(
      http.get('/api/runners', () =>
        HttpResponse.json({ message: 'Server error' }, { status: 500 }),
      ),
    );

    renderWithProviders(<RunnersPage />);

    await waitFor(() => {
      expect(screen.getByText(/Kunde inte ladda deltagare/)).toBeInTheDocument();
    });
  });

  it('shows empty state when no runners exist', async () => {
    server.use(http.get('/api/runners', () => HttpResponse.json([])));

    renderWithProviders(<RunnersPage />);

    await waitFor(() => {
      expect(screen.getByText('Inga deltagare registrerade.')).toBeInTheDocument();
    });
  });

  describe('Search and filter', () => {
    it('filters runners by name search', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText(runner1.name)).toBeInTheDocument();
      });

      await user.type(screen.getByPlaceholderText('Sök namn eller nummerlapp…'), 'Anna');

      expect(screen.getByText(runner1.name)).toBeInTheDocument();
      expect(screen.queryByText(runner2.name)).not.toBeInTheDocument();
      expect(screen.getByText('1 av 2 deltagare')).toBeInTheDocument();
    });

    it('filters runners by bib search', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText(runner1.name)).toBeInTheDocument();
      });

      await user.type(screen.getByPlaceholderText('Sök namn eller nummerlapp…'), '102');

      expect(screen.getByText(runner2.name)).toBeInTheDocument();
      expect(screen.queryByText(runner1.name)).not.toBeInTheDocument();
    });

    it('filters runners by class', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText(runner1.name)).toBeInTheDocument();
      });

      await user.selectOptions(screen.getByLabelText('Filtrera klass'), runner1.class);

      expect(screen.getByText(runner1.name)).toBeInTheDocument();
      expect(screen.queryByText(runner2.name)).not.toBeInTheDocument();
    });

    it('filters runners by club', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText(runner1.name)).toBeInTheDocument();
      });

      // Both runners are in the same club, so both should still appear
      await user.selectOptions(screen.getByLabelText('Filtrera klubb'), runner1.club);

      expect(screen.getByText(runner1.name)).toBeInTheDocument();
      expect(screen.getByText(runner2.name)).toBeInTheDocument();
    });

    it('shows no-match message when filter excludes all', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText(runner1.name)).toBeInTheDocument();
      });

      await user.type(screen.getByPlaceholderText('Sök namn eller nummerlapp…'), 'nonexistent');

      expect(screen.getByText('Inga deltagare matchar filtret.')).toBeInTheDocument();
      expect(screen.getByText('0 av 2 deltagare')).toBeInTheDocument();
    });
  });

  describe('Sorting', () => {
    it('sorts by name ascending by default', async () => {
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText(runner1.name)).toBeInTheDocument();
      });

      const rows = screen.getAllByRole('row');
      // Row 0 is header, rows 1+ are data
      expect(within(rows[1]!).getByText(runner1.name)).toBeInTheDocument();
      expect(within(rows[2]!).getByText(runner2.name)).toBeInTheDocument();
    });

    it('toggles sort direction on column click', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText(runner1.name)).toBeInTheDocument();
      });

      // Click Namn column to toggle to desc
      await user.click(screen.getByText(/^Namn/));

      const rows = screen.getAllByRole('row');
      // Now descending: Erik before Anna
      expect(within(rows[1]!).getByText(runner2.name)).toBeInTheDocument();
      expect(within(rows[2]!).getByText(runner1.name)).toBeInTheDocument();
    });
  });

  describe('Create form', () => {
    it('opens create form when clicking Ny deltagare', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny deltagare')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny deltagare'));

      expect(screen.getByLabelText('Namn')).toBeInTheDocument();
      expect(screen.getByLabelText('Klass')).toBeInTheDocument();
      expect(screen.getByLabelText('Klubb')).toBeInTheDocument();
      expect(screen.getByLabelText('Bricknummer')).toBeInTheDocument();
    });

    it('submits new runner via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny deltagare')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny deltagare'));

      await user.type(screen.getByLabelText('Namn'), 'Ny Löpare');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows validation error for empty name', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny deltagare')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny deltagare'));
      await user.click(screen.getByText('Spara'));

      expect(screen.getByText('Namn krävs')).toBeInTheDocument();
    });

    it('closes create form on cancel', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny deltagare')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny deltagare'));
      expect(screen.getByLabelText('Namn')).toBeInTheDocument();

      await user.click(screen.getByText('Avbryt'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows API error on create failure', async () => {
      server.use(
        http.post('/api/runners', () =>
          HttpResponse.json({ message: 'Create failed' }, { status: 500 }),
        ),
      );

      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny deltagare')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny deltagare'));
      await user.type(screen.getByLabelText('Namn'), 'Test');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.getByText('Create failed')).toBeInTheDocument();
      });
    });
  });

  describe('Edit form', () => {
    it('shows edit/delete buttons when a runner is selected', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText(runner1.name)).toBeInTheDocument();
      });

      // Click on a runner row to select
      await user.click(screen.getByText(runner1.name));

      expect(screen.getByText('Redigera')).toBeInTheDocument();
      expect(screen.getByText('Ta bort')).toBeInTheDocument();
    });

    it('opens edit form with runner data', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText(runner1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(runner1.name));
      await user.click(screen.getByText('Redigera'));

      expect(screen.getByLabelText('Namn')).toHaveValue(runner1.name);
    });

    it('submits updated runner via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText(runner1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(runner1.name));
      await user.click(screen.getByText('Redigera'));

      const nameInput = screen.getByLabelText('Namn');
      await user.clear(nameInput);
      await user.type(nameInput, 'Updated Runner');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows validation error for empty name on edit', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText(runner1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(runner1.name));
      await user.click(screen.getByText('Redigera'));

      const nameInput = screen.getByLabelText('Namn');
      await user.clear(nameInput);
      await user.click(screen.getByText('Spara'));

      expect(screen.getByText('Namn krävs')).toBeInTheDocument();
    });

    it('cancels editing', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText(runner1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(runner1.name));
      await user.click(screen.getByText('Redigera'));
      expect(screen.getByLabelText('Namn')).toBeInTheDocument();

      await user.click(screen.getByText('Avbryt'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows API error on update failure', async () => {
      server.use(
        http.put('/api/runners/:id', () =>
          HttpResponse.json({ message: 'Update failed' }, { status: 500 }),
        ),
      );

      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText(runner1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(runner1.name));
      await user.click(screen.getByText('Redigera'));
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.getByText('Update failed')).toBeInTheDocument();
      });
    });
  });

  describe('Delete', () => {
    it('deletes a runner via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<RunnersPage />);

      await waitFor(() => {
        expect(screen.getByText(runner1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(runner1.name));
      await user.click(screen.getByText('Ta bort'));

      // After deletion, the edit/delete buttons should disappear (selection cleared)
      await waitFor(() => {
        expect(screen.queryByText('Redigera')).not.toBeInTheDocument();
      });
    });
  });
});
