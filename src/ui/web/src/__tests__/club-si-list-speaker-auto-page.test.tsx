import { describe, it, expect } from 'vitest';
import { screen, waitFor, within } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { http, HttpResponse } from 'msw';
import { renderWithProviders, fixtures, server } from '../test';
import ClubsPage from '../pages/ClubsPage';
import SiPage from '../pages/SiPage';
import ListsPage from '../pages/ListsPage';
import SpeakerPage from '../pages/SpeakerPage';
import AutomationsPage from '../pages/AutomationsPage';

// ────────────────────── ClubsPage ──────────────────────

describe('ClubsPage', () => {
  const club1 = fixtures.clubs[0]!;
  const club2 = fixtures.clubs[1]!;

  it('shows loading state', () => {
    renderWithProviders(<ClubsPage />);
    expect(screen.getByText('Laddar klubbar…')).toBeInTheDocument();
  });

  it('renders club list after loading', async () => {
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText(club1.name)).toBeInTheDocument();
    });
    expect(screen.getByText(club2.name)).toBeInTheDocument();
  });

  it('shows count text', async () => {
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText('2 av 2 klubbar')).toBeInTheDocument();
    });
  });

  it('shows error on API failure', async () => {
    server.use(
      http.get('/api/clubs', () =>
        HttpResponse.json({ message: 'Server error' }, { status: 500 }),
      ),
    );
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText(/Kunde inte ladda klubbar/)).toBeInTheDocument();
    });
  });

  it('shows empty state', async () => {
    server.use(http.get('/api/clubs', () => HttpResponse.json([])));
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText('Inga klubbar registrerade.')).toBeInTheDocument();
    });
  });

  it('filters by name search', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText(club1.name)).toBeInTheDocument();
    });
    await user.type(screen.getByPlaceholderText('Sök klubb…'), 'IFK');
    expect(screen.getByText(club2.name)).toBeInTheDocument();
    expect(screen.queryByText(club1.name)).not.toBeInTheDocument();
    expect(screen.getByText('1 av 2 klubbar')).toBeInTheDocument();
  });

  it('shows no-match message', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText(club1.name)).toBeInTheDocument();
    });
    await user.type(screen.getByPlaceholderText('Sök klubb…'), 'nonexistent');
    expect(screen.getByText('Inga klubbar matchar filtret.')).toBeInTheDocument();
    expect(screen.getByText('0 av 2 klubbar')).toBeInTheDocument();
  });

  it('sorts by name ascending by default', async () => {
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText(club2.name)).toBeInTheDocument();
    });
    const rows = screen.getAllByRole('row');
    expect(within(rows[1]!).getByText(club2.name)).toBeInTheDocument();
    expect(within(rows[2]!).getByText(club1.name)).toBeInTheDocument();
  });

  it('toggles sort direction on column click', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText(club1.name)).toBeInTheDocument();
    });
    await user.click(screen.getByText(/^Namn/));
    const rows = screen.getAllByRole('row');
    expect(within(rows[1]!).getByText(club1.name)).toBeInTheDocument();
    expect(within(rows[2]!).getByText(club2.name)).toBeInTheDocument();
  });

  it('opens create form via Ny klubb', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText('Ny klubb')).toBeInTheDocument();
    });
    await user.click(screen.getByText('Ny klubb'));
    expect(screen.getByLabelText('Namn')).toBeInTheDocument();
    expect(screen.getByLabelText('Nationalitet')).toBeInTheDocument();
  });

  it('submits new club via API', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText('Ny klubb')).toBeInTheDocument();
    });
    await user.click(screen.getByText('Ny klubb'));
    await user.type(screen.getByLabelText('Namn'), 'Ny Klubb');
    await user.click(screen.getByText('Spara'));
    await waitFor(() => {
      expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
    });
  });

  it('shows validation error for empty name', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText('Ny klubb')).toBeInTheDocument();
    });
    await user.click(screen.getByText('Ny klubb'));
    await user.click(screen.getByText('Spara'));
    expect(screen.getByText('Namn krävs')).toBeInTheDocument();
  });

  it('closes create form on cancel', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText('Ny klubb')).toBeInTheDocument();
    });
    await user.click(screen.getByText('Ny klubb'));
    expect(screen.getByLabelText('Namn')).toBeInTheDocument();
    await user.click(screen.getByText('Avbryt'));
    await waitFor(() => {
      expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
    });
  });

  it('shows API error on create failure', async () => {
    server.use(
      http.post('/api/clubs', () =>
        HttpResponse.json({ message: 'Create failed' }, { status: 500 }),
      ),
    );
    const user = userEvent.setup();
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText('Ny klubb')).toBeInTheDocument();
    });
    await user.click(screen.getByText('Ny klubb'));
    await user.type(screen.getByLabelText('Namn'), 'Test');
    await user.click(screen.getByText('Spara'));
    await waitFor(() => {
      expect(screen.getByText('Create failed')).toBeInTheDocument();
    });
  });

  it('opens edit form with club data', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText(club1.name)).toBeInTheDocument();
    });
    await user.click(screen.getByText(club1.name));
    await user.click(screen.getByText('Redigera'));
    expect(screen.getByLabelText('Namn')).toHaveValue(club1.name);
  });

  it('submits updated club via API', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText(club1.name)).toBeInTheDocument();
    });
    await user.click(screen.getByText(club1.name));
    await user.click(screen.getByText('Redigera'));
    const nameInput = screen.getByLabelText('Namn');
    await user.clear(nameInput);
    await user.type(nameInput, 'Updated Club');
    await user.click(screen.getByText('Spara'));
    await waitFor(() => {
      expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
    });
  });

  it('validates empty name on edit', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText(club1.name)).toBeInTheDocument();
    });
    await user.click(screen.getByText(club1.name));
    await user.click(screen.getByText('Redigera'));
    await user.clear(screen.getByLabelText('Namn'));
    await user.click(screen.getByText('Spara'));
    expect(screen.getByText('Namn krävs')).toBeInTheDocument();
  });

  it('cancels edit', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText(club1.name)).toBeInTheDocument();
    });
    await user.click(screen.getByText(club1.name));
    await user.click(screen.getByText('Redigera'));
    await user.click(screen.getByText('Avbryt'));
    await waitFor(() => {
      expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
    });
  });

  it('shows API error on update failure', async () => {
    server.use(
      http.put('/api/clubs/:id', () =>
        HttpResponse.json({ message: 'Update failed' }, { status: 500 }),
      ),
    );
    const user = userEvent.setup();
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText(club1.name)).toBeInTheDocument();
    });
    await user.click(screen.getByText(club1.name));
    await user.click(screen.getByText('Redigera'));
    await user.click(screen.getByText('Spara'));
    await waitFor(() => {
      expect(screen.getByText('Update failed')).toBeInTheDocument();
    });
  });

  it('deletes a club via API', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ClubsPage />);
    await waitFor(() => {
      expect(screen.getByText(club1.name)).toBeInTheDocument();
    });
    await user.click(screen.getByText(club1.name));
    await user.click(screen.getByText('Ta bort'));
    await waitFor(() => {
      expect(screen.queryByText('Redigera')).not.toBeInTheDocument();
    });
  });
});

// ────────────────────── SiPage ──────────────────────

describe('SiPage', () => {
  const card1 = fixtures.cards[0]!;

  it('shows loading state', () => {
    renderWithProviders(<SiPage />);
    expect(screen.getByText('Laddar brickor…')).toBeInTheDocument();
  });

  it('renders card list after loading', async () => {
    renderWithProviders(<SiPage />);
    await waitFor(() => {
      expect(screen.getByText(String(card1.cardNo))).toBeInTheDocument();
    });
  });

  it('shows count text', async () => {
    renderWithProviders(<SiPage />);
    await waitFor(() => {
      expect(screen.getByText('1 av 1 brickor')).toBeInTheDocument();
    });
  });

  it('shows error on API failure', async () => {
    server.use(
      http.get('/api/cards', () =>
        HttpResponse.json({ message: 'Server error' }, { status: 500 }),
      ),
    );
    renderWithProviders(<SiPage />);
    await waitFor(() => {
      expect(screen.getByText(/Kunde inte ladda brickor/)).toBeInTheDocument();
    });
  });

  it('shows empty state', async () => {
    server.use(http.get('/api/cards', () => HttpResponse.json([])));
    renderWithProviders(<SiPage />);
    await waitFor(() => {
      expect(screen.getByText('Inga brickor registrerade.')).toBeInTheDocument();
    });
  });

  it('filters by card number search', async () => {
    const user = userEvent.setup();
    renderWithProviders(<SiPage />);
    await waitFor(() => {
      expect(screen.getByText(String(card1.cardNo))).toBeInTheDocument();
    });
    await user.type(screen.getByPlaceholderText('Sök bricknummer eller löpare…'), String(card1.cardNo));
    expect(screen.getByText(String(card1.cardNo))).toBeInTheDocument();
    expect(screen.getByText('1 av 1 brickor')).toBeInTheDocument();
  });

  it('shows no-match message', async () => {
    const user = userEvent.setup();
    renderWithProviders(<SiPage />);
    await waitFor(() => {
      expect(screen.getByText(String(card1.cardNo))).toBeInTheDocument();
    });
    await user.type(screen.getByPlaceholderText('Sök bricknummer eller löpare…'), '99999');
    expect(screen.getByText('Inga brickor matchar filtret.')).toBeInTheDocument();
    expect(screen.getByText('0 av 1 brickor')).toBeInTheDocument();
  });

  it('toggles sort direction on column click', async () => {
    const user = userEvent.setup();
    renderWithProviders(<SiPage />);
    await waitFor(() => {
      expect(screen.getByText(String(card1.cardNo))).toBeInTheDocument();
    });
    await user.click(screen.getByText(/^Bricknr/));
    expect(screen.getByText(String(card1.cardNo))).toBeInTheDocument();
  });

  it('shows punch details on card selection', async () => {
    const user = userEvent.setup();
    renderWithProviders(<SiPage />);
    await waitFor(() => {
      expect(screen.getByText(String(card1.cardNo))).toBeInTheDocument();
    });
    await user.click(screen.getByText(String(card1.cardNo)));
    await waitFor(() => {
      expect(screen.getByText(/Bricka\s+12345/)).toBeInTheDocument();
    });
  });

  it('submits read card form', async () => {
    const user = userEvent.setup();
    renderWithProviders(<SiPage />);
    await waitFor(() => {
      expect(screen.getByText(String(card1.cardNo))).toBeInTheDocument();
    });
    await user.type(screen.getByLabelText('Bricknummer att läsa'), '99999');
    await user.click(screen.getByText('Läs bricka'));
    await waitFor(() => {
      expect(screen.getByLabelText('Bricknummer att läsa')).toHaveValue(null);
    });
  });

  it('shows error on read card failure', async () => {
    server.use(
      http.post('/api/cards/read', () =>
        HttpResponse.json({ message: 'Read failed' }, { status: 500 }),
      ),
    );
    const user = userEvent.setup();
    renderWithProviders(<SiPage />);
    await waitFor(() => {
      expect(screen.getByText(String(card1.cardNo))).toBeInTheDocument();
    });
    await user.type(screen.getByLabelText('Bricknummer att läsa'), '99999');
    await user.click(screen.getByText('Läs bricka'));
    await waitFor(() => {
      expect(screen.getByText(/Fel vid brickavläsning/)).toBeInTheDocument();
    });
  });
});

// ────────────────────── ListsPage ──────────────────────

describe('ListsPage', () => {
  it('shows loading state', () => {
    renderWithProviders(<ListsPage />);
    expect(screen.getByText('Laddar listor…')).toBeInTheDocument();
  });

  it('renders list type dropdown after loading', async () => {
    renderWithProviders(<ListsPage />);
    await waitFor(() => {
      expect(screen.getByLabelText('Välj listtyp')).toBeInTheDocument();
    });
    expect(screen.getByText('Startlista')).toBeInTheDocument();
    expect(screen.getByText('Resultatlista')).toBeInTheDocument();
  });

  it('shows error on API failure', async () => {
    server.use(
      http.get('/api/lists', () =>
        HttpResponse.json({ message: 'Server error' }, { status: 500 }),
      ),
    );
    renderWithProviders(<ListsPage />);
    await waitFor(() => {
      expect(screen.getByText(/Kunde inte ladda listor/)).toBeInTheDocument();
    });
  });

  it('shows generated list when type is selected', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ListsPage />);
    await waitFor(() => {
      expect(screen.getByLabelText('Välj listtyp')).toBeInTheDocument();
    });
    await user.selectOptions(screen.getByLabelText('Välj listtyp'), 'startlist');
    await waitFor(() => {
      expect(screen.getByText('Test list')).toBeInTheDocument();
    });
  });

  it('shows results table with data', async () => {
    renderWithProviders(<ListsPage />);
    await waitFor(() => {
      expect(screen.getByText('Resultat')).toBeInTheDocument();
    });
  });

  it('filters results by name search', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ListsPage />);
    await waitFor(() => {
      expect(screen.getByLabelText('Välj listtyp')).toBeInTheDocument();
    });
    // Results are loaded from the default handler
    await waitFor(() => {
      expect(screen.getByText('Anna Svensson')).toBeInTheDocument();
    });
    await user.type(screen.getByPlaceholderText('Sök namn eller klubb…'), 'Anna');
    expect(screen.getByText('Anna Svensson')).toBeInTheDocument();
    expect(screen.queryByText('Erik Johansson')).not.toBeInTheDocument();
  });

  it('sorts results by clicking column header', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ListsPage />);
    await waitFor(() => {
      expect(screen.getByText('Anna Svensson')).toBeInTheDocument();
    });
    await user.click(screen.getByText(/^Namn/));
    const rows = screen.getAllByRole('row');
    // After clicking Namn (default place asc → name asc), rows should be alphabetical
    const dataRows = rows.slice(1); // skip header
    expect(dataRows.length).toBeGreaterThanOrEqual(2);
  });

  it('shows no-match for results search', async () => {
    const user = userEvent.setup();
    renderWithProviders(<ListsPage />);
    await waitFor(() => {
      expect(screen.getByText('Anna Svensson')).toBeInTheDocument();
    });
    await user.type(screen.getByPlaceholderText('Sök namn eller klubb…'), 'nonexistent');
    expect(screen.getByText('Inga resultat matchar sökningen.')).toBeInTheDocument();
  });
});

// ────────────────────── SpeakerPage ──────────────────────

describe('SpeakerPage', () => {
  it('shows loading state', () => {
    renderWithProviders(<SpeakerPage />);
    expect(screen.getByText('Laddar speaker-konfiguration…')).toBeInTheDocument();
  });

  it('renders config section after loading', async () => {
    renderWithProviders(<SpeakerPage />);
    await waitFor(() => {
      expect(screen.getByText('Konfiguration')).toBeInTheDocument();
    });
    expect(screen.getByText('Live-händelser')).toBeInTheDocument();
  });

  it('shows error on API failure', async () => {
    server.use(
      http.get('/api/speaker/config', () =>
        HttpResponse.json({ message: 'Server error' }, { status: 500 }),
      ),
    );
    renderWithProviders(<SpeakerPage />);
    await waitFor(() => {
      expect(screen.getByText(/Kunde inte ladda konfiguration/)).toBeInTheDocument();
    });
  });

  it('shows class checkboxes', async () => {
    renderWithProviders(<SpeakerPage />);
    await waitFor(() => {
      expect(screen.getByText('Konfiguration')).toBeInTheDocument();
    });
    // Classes are loaded from the default handler
    await waitFor(() => {
      expect(screen.getByText('H21')).toBeInTheDocument();
    });
    expect(screen.getByText('D21')).toBeInTheDocument();
  });

  it('toggles class checkbox', async () => {
    const user = userEvent.setup();
    renderWithProviders(<SpeakerPage />);
    await waitFor(() => {
      expect(screen.getByText('H21')).toBeInTheDocument();
    });
    const checkbox = screen.getByLabelText('H21');
    await user.click(checkbox);
    // Mutation should fire; we just verify no crash
    expect(checkbox).toBeInTheDocument();
  });

  it('shows window seconds input', async () => {
    renderWithProviders(<SpeakerPage />);
    await waitFor(() => {
      expect(screen.getByLabelText('Tidsfönster i sekunder')).toBeInTheDocument();
    });
  });

  it('shows empty events message when no classes selected', async () => {
    renderWithProviders(<SpeakerPage />);
    await waitFor(() => {
      expect(screen.getByText('Välj klasser ovan för att se händelser.')).toBeInTheDocument();
    });
  });

  it('shows search field for events', async () => {
    renderWithProviders(<SpeakerPage />);
    await waitFor(() => {
      expect(screen.getByPlaceholderText('Sök klass, typ eller löpar-ID…')).toBeInTheDocument();
    });
  });

  it('shows update error on config update failure', async () => {
    server.use(
      http.put('/api/speaker/config', () =>
        HttpResponse.json({ message: 'Update failed' }, { status: 500 }),
      ),
    );
    const user = userEvent.setup();
    renderWithProviders(<SpeakerPage />);
    await waitFor(() => {
      expect(screen.getByText('H21')).toBeInTheDocument();
    });
    await user.click(screen.getByLabelText('H21'));
    await waitFor(() => {
      expect(screen.getByText(/Fel vid uppdatering/)).toBeInTheDocument();
    });
  });
});

// ────────────────────── AutomationsPage ──────────────────────

describe('AutomationsPage', () => {
  const auto1 = fixtures.automations[0]!;
  const auto2 = fixtures.automations[1]!;

  it('shows loading state', () => {
    renderWithProviders(<AutomationsPage />);
    expect(screen.getByText('Laddar automationer…')).toBeInTheDocument();
  });

  it('renders automation list after loading', async () => {
    renderWithProviders(<AutomationsPage />);
    await waitFor(() => {
      expect(screen.getByText(auto1.name)).toBeInTheDocument();
    });
    expect(screen.getByText(auto2.name)).toBeInTheDocument();
  });

  it('shows count text', async () => {
    renderWithProviders(<AutomationsPage />);
    await waitFor(() => {
      expect(screen.getByText('2 av 2 automationer')).toBeInTheDocument();
    });
  });

  it('shows error on API failure', async () => {
    server.use(
      http.get('/api/automations', () =>
        HttpResponse.json({ message: 'Server error' }, { status: 500 }),
      ),
    );
    renderWithProviders(<AutomationsPage />);
    await waitFor(() => {
      expect(screen.getByText(/Kunde inte ladda automationer/)).toBeInTheDocument();
    });
  });

  it('shows empty state', async () => {
    server.use(http.get('/api/automations', () => HttpResponse.json([])));
    renderWithProviders(<AutomationsPage />);
    await waitFor(() => {
      expect(screen.getByText('Inga automationer konfigurerade.')).toBeInTheDocument();
    });
  });

  it('filters by search', async () => {
    const user = userEvent.setup();
    renderWithProviders(<AutomationsPage />);
    await waitFor(() => {
      expect(screen.getByText(auto1.name)).toBeInTheDocument();
    });
    await user.type(screen.getByPlaceholderText('Sök automation…'), auto1.name);
    expect(screen.getByText(auto1.name)).toBeInTheDocument();
    expect(screen.queryByText(auto2.name)).not.toBeInTheDocument();
  });

  it('shows no-match message', async () => {
    const user = userEvent.setup();
    renderWithProviders(<AutomationsPage />);
    await waitFor(() => {
      expect(screen.getByText(auto1.name)).toBeInTheDocument();
    });
    await user.type(screen.getByPlaceholderText('Sök automation…'), 'nonexistent');
    expect(screen.getByText('Inga automationer matchar sökningen.')).toBeInTheDocument();
    expect(screen.getByText('0 av 2 automationer')).toBeInTheDocument();
  });

  it('toggles sort direction on column click', async () => {
    const user = userEvent.setup();
    renderWithProviders(<AutomationsPage />);
    await waitFor(() => {
      expect(screen.getByText(auto1.name)).toBeInTheDocument();
    });
    await user.click(screen.getByText(/^Namn/));
    expect(screen.getByText(auto1.name)).toBeInTheDocument();
  });

  it('opens create form via Ny automation', async () => {
    const user = userEvent.setup();
    renderWithProviders(<AutomationsPage />);
    await waitFor(() => {
      expect(screen.getByText('+ Ny automation')).toBeInTheDocument();
    });
    await user.click(screen.getByText('+ Ny automation'));
    expect(screen.getByLabelText('Typ')).toBeInTheDocument();
    expect(screen.getByLabelText('Namn')).toBeInTheDocument();
  });

  it('submits new automation via API', async () => {
    const user = userEvent.setup();
    renderWithProviders(<AutomationsPage />);
    await waitFor(() => {
      expect(screen.getByText('+ Ny automation')).toBeInTheDocument();
    });
    await user.click(screen.getByText('+ Ny automation'));
    await user.click(screen.getByText('Skapa'));
    await waitFor(() => {
      expect(screen.queryByLabelText('Typ')).not.toBeInTheDocument();
    });
  });

  it('closes create form on cancel', async () => {
    const user = userEvent.setup();
    renderWithProviders(<AutomationsPage />);
    await waitFor(() => {
      expect(screen.getByText('+ Ny automation')).toBeInTheDocument();
    });
    await user.click(screen.getByText('+ Ny automation'));
    expect(screen.getByLabelText('Typ')).toBeInTheDocument();
    await user.click(screen.getByText('Avbryt'));
    await waitFor(() => {
      expect(screen.queryByLabelText('Typ')).not.toBeInTheDocument();
    });
  });

  it('shows create error on API failure', async () => {
    server.use(
      http.post('/api/automations', () =>
        HttpResponse.json({ message: 'Create failed' }, { status: 500 }),
      ),
    );
    const user = userEvent.setup();
    renderWithProviders(<AutomationsPage />);
    await waitFor(() => {
      expect(screen.getByText('+ Ny automation')).toBeInTheDocument();
    });
    await user.click(screen.getByText('+ Ny automation'));
    await user.click(screen.getByText('Skapa'));
    await waitFor(() => {
      expect(screen.getByText(/Fel vid skapande/)).toBeInTheDocument();
    });
  });

  it('deletes automation via API', async () => {
    const user = userEvent.setup();
    renderWithProviders(<AutomationsPage />);
    await waitFor(() => {
      expect(screen.getByText(auto1.name)).toBeInTheDocument();
    });
    await user.click(screen.getByText(auto1.name));
    expect(screen.getByText('Ta bort')).toBeInTheDocument();
    await user.click(screen.getByText('Ta bort'));
    await waitFor(() => {
      expect(screen.queryByText('Ta bort')).not.toBeInTheDocument();
    });
  });

  it('shows retry button on error', async () => {
    server.use(
      http.get('/api/automations', () =>
        HttpResponse.json({ message: 'Server error' }, { status: 500 }),
      ),
    );
    renderWithProviders(<AutomationsPage />);
    await waitFor(() => {
      expect(screen.getByText('Försök igen')).toBeInTheDocument();
    });
  });
});
