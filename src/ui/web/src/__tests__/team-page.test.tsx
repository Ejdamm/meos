import { describe, it, expect } from 'vitest';
import { screen, waitFor, within } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { http, HttpResponse } from 'msw';
import { renderWithProviders, fixtures, server } from '../test';
import TeamsPage from '../pages/TeamsPage';

const team1 = fixtures.teams[0]!;

describe('TeamsPage', () => {
  it('shows loading state initially', () => {
    renderWithProviders(<TeamsPage />);
    expect(screen.getByText('Laddar lag…')).toBeInTheDocument();
  });

  it('renders team list after loading', async () => {
    renderWithProviders(<TeamsPage />);

    await waitFor(() => {
      expect(screen.getByText(team1.name)).toBeInTheDocument();
    });
  });

  it('renders team details in table', async () => {
    renderWithProviders(<TeamsPage />);

    await waitFor(() => {
      expect(screen.getByText(team1.name)).toBeInTheDocument();
    });
    // class and club appear in both filter dropdowns and table cells
    expect(screen.getAllByText(team1.class).length).toBeGreaterThanOrEqual(1);
    expect(screen.getAllByText(team1.club).length).toBeGreaterThanOrEqual(1);
    expect(screen.getByText(team1.bib)).toBeInTheDocument();
    expect(screen.getByText('OK')).toBeInTheDocument();
  });

  it('shows count of teams', async () => {
    renderWithProviders(<TeamsPage />);

    await waitFor(() => {
      expect(screen.getByText('1 av 1 lag')).toBeInTheDocument();
    });
  });

  it('shows error message on API failure', async () => {
    server.use(
      http.get('/api/teams', () =>
        HttpResponse.json({ message: 'Server error' }, { status: 500 }),
      ),
    );

    renderWithProviders(<TeamsPage />);

    await waitFor(() => {
      expect(screen.getByText(/Kunde inte ladda lag/)).toBeInTheDocument();
    });
  });

  it('shows empty state when no teams exist', async () => {
    server.use(http.get('/api/teams', () => HttpResponse.json([])));

    renderWithProviders(<TeamsPage />);

    await waitFor(() => {
      expect(screen.getByText('Inga lag registrerade.')).toBeInTheDocument();
    });
  });

  it('shows number of team members', async () => {
    renderWithProviders(<TeamsPage />);

    await waitFor(() => {
      expect(screen.getByText(team1.name)).toBeInTheDocument();
    });
    // team1 has 2 runners
    expect(screen.getByText('2')).toBeInTheDocument();
  });

  describe('Search and filter', () => {
    it('filters teams by name search', async () => {
      const user = userEvent.setup();

      // Add a second team so we can filter
      server.use(
        http.get('/api/teams', () =>
          HttpResponse.json([
            team1,
            { ...team1, id: 2, name: 'Team Beta', bib: '2', class: 'Open', club: 'IFK Göteborg' },
          ]),
        ),
      );

      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText('Team Alpha')).toBeInTheDocument();
      });
      expect(screen.getByText('Team Beta')).toBeInTheDocument();

      await user.type(screen.getByPlaceholderText('Sök lagnamn eller nummerlapp…'), 'Alpha');

      expect(screen.getByText('Team Alpha')).toBeInTheDocument();
      expect(screen.queryByText('Team Beta')).not.toBeInTheDocument();
      expect(screen.getByText('1 av 2 lag')).toBeInTheDocument();
    });

    it('filters teams by bib search', async () => {
      const user = userEvent.setup();

      server.use(
        http.get('/api/teams', () =>
          HttpResponse.json([
            team1,
            { ...team1, id: 2, name: 'Team Beta', bib: '2', class: 'Open', club: 'IFK Göteborg' },
          ]),
        ),
      );

      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText('Team Alpha')).toBeInTheDocument();
      });

      await user.type(screen.getByPlaceholderText('Sök lagnamn eller nummerlapp…'), '2');

      expect(screen.getByText('Team Beta')).toBeInTheDocument();
      expect(screen.queryByText('Team Alpha')).not.toBeInTheDocument();
    });

    it('filters teams by class', async () => {
      const user = userEvent.setup();

      server.use(
        http.get('/api/teams', () =>
          HttpResponse.json([
            team1,
            { ...team1, id: 2, name: 'Team Beta', bib: '2', class: 'Open', club: 'IFK Göteborg' },
          ]),
        ),
      );

      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText('Team Alpha')).toBeInTheDocument();
      });

      await user.selectOptions(screen.getByLabelText('Filtrera klass'), 'Stafett');

      expect(screen.getByText('Team Alpha')).toBeInTheDocument();
      expect(screen.queryByText('Team Beta')).not.toBeInTheDocument();
    });

    it('filters teams by club', async () => {
      const user = userEvent.setup();

      server.use(
        http.get('/api/teams', () =>
          HttpResponse.json([
            team1,
            { ...team1, id: 2, name: 'Team Beta', bib: '2', class: 'Open', club: 'IFK Göteborg' },
          ]),
        ),
      );

      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText('Team Alpha')).toBeInTheDocument();
      });

      await user.selectOptions(screen.getByLabelText('Filtrera klubb'), 'OK Test');

      expect(screen.getByText('Team Alpha')).toBeInTheDocument();
      expect(screen.queryByText('Team Beta')).not.toBeInTheDocument();
    });

    it('shows no-match message when filter excludes all', async () => {
      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText(team1.name)).toBeInTheDocument();
      });

      await user.type(screen.getByPlaceholderText('Sök lagnamn eller nummerlapp…'), 'nonexistent');

      expect(screen.getByText('Inga lag matchar filtret.')).toBeInTheDocument();
      expect(screen.getByText('0 av 1 lag')).toBeInTheDocument();
    });
  });

  describe('Sorting', () => {
    it('sorts by name ascending by default', async () => {
      server.use(
        http.get('/api/teams', () =>
          HttpResponse.json([
            team1,
            { ...team1, id: 2, name: 'Team Beta', bib: '2' },
          ]),
        ),
      );

      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText('Team Alpha')).toBeInTheDocument();
      });

      const rows = screen.getAllByRole('row');
      expect(within(rows[1]!).getByText('Team Alpha')).toBeInTheDocument();
      expect(within(rows[2]!).getByText('Team Beta')).toBeInTheDocument();
    });

    it('toggles sort direction on column click', async () => {
      const user = userEvent.setup();

      server.use(
        http.get('/api/teams', () =>
          HttpResponse.json([
            team1,
            { ...team1, id: 2, name: 'Team Beta', bib: '2' },
          ]),
        ),
      );

      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText('Team Alpha')).toBeInTheDocument();
      });

      await user.click(screen.getByText(/^Namn/));

      const rows = screen.getAllByRole('row');
      expect(within(rows[1]!).getByText('Team Beta')).toBeInTheDocument();
      expect(within(rows[2]!).getByText('Team Alpha')).toBeInTheDocument();
    });
  });

  describe('Create form', () => {
    it('opens create form when clicking Nytt lag', async () => {
      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText('Nytt lag')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Nytt lag'));

      expect(screen.getByLabelText('Namn')).toBeInTheDocument();
      expect(screen.getByLabelText('Klass')).toBeInTheDocument();
      expect(screen.getByLabelText('Klubb')).toBeInTheDocument();
      expect(screen.getByLabelText('Nummerlapp')).toBeInTheDocument();
    });

    it('submits new team via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText('Nytt lag')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Nytt lag'));

      await user.type(screen.getByLabelText('Namn'), 'Nytt Lag');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows validation error for empty name', async () => {
      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText('Nytt lag')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Nytt lag'));
      await user.click(screen.getByText('Spara'));

      expect(screen.getByText('Namn krävs')).toBeInTheDocument();
    });

    it('closes create form on cancel', async () => {
      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText('Nytt lag')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Nytt lag'));
      expect(screen.getByLabelText('Namn')).toBeInTheDocument();

      await user.click(screen.getByText('Avbryt'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows API error on create failure', async () => {
      server.use(
        http.post('/api/teams', () =>
          HttpResponse.json({ message: 'Create failed' }, { status: 500 }),
        ),
      );

      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText('Nytt lag')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Nytt lag'));
      await user.type(screen.getByLabelText('Namn'), 'Test');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.getByText('Create failed')).toBeInTheDocument();
      });
    });

    it('can add and select team members', async () => {
      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText('Nytt lag')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Nytt lag'));

      await user.click(screen.getByText('+ Lägg till sträcka'));

      expect(screen.getByLabelText('Sträcka 1 löpare')).toBeInTheDocument();
    });

    it('can remove team member slot', async () => {
      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText('Nytt lag')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Nytt lag'));

      await user.click(screen.getByText('+ Lägg till sträcka'));
      expect(screen.getByLabelText('Sträcka 1 löpare')).toBeInTheDocument();

      await user.click(screen.getByLabelText('Ta bort sträcka 1'));

      expect(screen.queryByLabelText('Sträcka 1 löpare')).not.toBeInTheDocument();
    });
  });

  describe('Edit form', () => {
    it('shows edit/delete buttons when a team is selected', async () => {
      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText(team1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(team1.name));

      expect(screen.getByText('Redigera')).toBeInTheDocument();
      expect(screen.getByText('Ta bort')).toBeInTheDocument();
    });

    it('opens edit form with team data', async () => {
      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText(team1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(team1.name));
      await user.click(screen.getByText('Redigera'));

      expect(screen.getByLabelText('Namn')).toHaveValue(team1.name);
      expect(screen.getByLabelText('Nummerlapp')).toHaveValue(team1.bib);
      // team1 has 2 runners, so 2 leg selects
      expect(screen.getByLabelText('Sträcka 1 löpare')).toBeInTheDocument();
      expect(screen.getByLabelText('Sträcka 2 löpare')).toBeInTheDocument();
    });

    it('submits updated team via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText(team1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(team1.name));
      await user.click(screen.getByText('Redigera'));

      const nameInput = screen.getByLabelText('Namn');
      await user.clear(nameInput);
      await user.type(nameInput, 'Updated Team');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows validation error for empty name on edit', async () => {
      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText(team1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(team1.name));
      await user.click(screen.getByText('Redigera'));

      const nameInput = screen.getByLabelText('Namn');
      await user.clear(nameInput);
      await user.click(screen.getByText('Spara'));

      expect(screen.getByText('Namn krävs')).toBeInTheDocument();
    });

    it('cancels editing', async () => {
      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText(team1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(team1.name));
      await user.click(screen.getByText('Redigera'));
      expect(screen.getByLabelText('Namn')).toBeInTheDocument();

      await user.click(screen.getByText('Avbryt'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows API error on update failure', async () => {
      server.use(
        http.put('/api/teams/:id', () =>
          HttpResponse.json({ message: 'Update failed' }, { status: 500 }),
        ),
      );

      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText(team1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(team1.name));
      await user.click(screen.getByText('Redigera'));
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.getByText('Update failed')).toBeInTheDocument();
      });
    });
  });

  describe('Delete', () => {
    it('deletes a team via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText(team1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(team1.name));
      await user.click(screen.getByText('Ta bort'));

      await waitFor(() => {
        expect(screen.queryByText('Redigera')).not.toBeInTheDocument();
      });
    });
  });

  describe('Leg results panel', () => {
    it('shows leg results when team is selected', async () => {
      const user = userEvent.setup();
      renderWithProviders(<TeamsPage />);

      await waitFor(() => {
        expect(screen.getByText(team1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(team1.name));

      await waitFor(() => {
        expect(screen.getByText(`Sträckresultat — ${team1.name}`)).toBeInTheDocument();
      });
      // Leg result times: 1800 seconds = 30:00
      expect(screen.getAllByText('30:00')).toHaveLength(2);
    });
  });
});
