import { describe, it, expect } from 'vitest';
import { screen, waitFor, within } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { http, HttpResponse } from 'msw';
import { renderWithProviders, fixtures, server } from '../test';
import CoursesPage from '../pages/CoursesPage';
import ControlsPage from '../pages/ControlsPage';

const course1 = fixtures.courses[0]!; // Bana 1, length 5200, 12 controls
const course2 = fixtures.courses[1]!; // Bana 2, length 3800, 8 controls

const ctrl1 = fixtures.controls[0]!; // Kontroll 31
const ctrl2 = fixtures.controls[1]!; // Kontroll 32
const ctrl3 = fixtures.controls[2]!; // Kontroll 33
const ctrl4 = fixtures.controls[3]!; // Kontroll 34

// ─── CoursesPage ────────────────────────────────────────────────────────────

describe('CoursesPage', () => {
  it('shows loading state initially', () => {
    renderWithProviders(<CoursesPage />);
    expect(screen.getByText('Laddar banor…')).toBeInTheDocument();
  });

  it('renders course list after loading', async () => {
    renderWithProviders(<CoursesPage />);

    await waitFor(() => {
      expect(screen.getByText(course1.name)).toBeInTheDocument();
    });
    expect(screen.getByText(course2.name)).toBeInTheDocument();
  });

  it('renders course details in table', async () => {
    renderWithProviders(<CoursesPage />);

    await waitFor(() => {
      expect(screen.getByText(course1.name)).toBeInTheDocument();
    });

    const rows = screen.getAllByRole('row');
    // Row 0 = header, sorted by name asc: Bana 1, Bana 2
    expect(within(rows[1]!).getByText(course1.name)).toBeInTheDocument();
    expect(within(rows[1]!).getByText(String(course1.length))).toBeInTheDocument();
    expect(within(rows[1]!).getByText(String(course1.numControls))).toBeInTheDocument();
    expect(within(rows[2]!).getByText(course2.name)).toBeInTheDocument();
  });

  it('shows count of courses', async () => {
    renderWithProviders(<CoursesPage />);

    await waitFor(() => {
      expect(screen.getByText('2 av 2 banor')).toBeInTheDocument();
    });
  });

  it('shows error message on API failure', async () => {
    server.use(
      http.get('/api/courses', () =>
        HttpResponse.json({ message: 'Server error' }, { status: 500 }),
      ),
    );

    renderWithProviders(<CoursesPage />);

    await waitFor(() => {
      expect(screen.getByText(/Kunde inte ladda banor/)).toBeInTheDocument();
    });
  });

  it('shows empty state when no courses exist', async () => {
    server.use(http.get('/api/courses', () => HttpResponse.json([])));

    renderWithProviders(<CoursesPage />);

    await waitFor(() => {
      expect(screen.getByText('Inga banor registrerade.')).toBeInTheDocument();
    });
  });

  describe('Search', () => {
    it('filters courses by name search', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText(course1.name)).toBeInTheDocument();
      });

      await user.type(screen.getByPlaceholderText('Sök bannamn…'), 'Bana 1');

      expect(screen.getByText(course1.name)).toBeInTheDocument();
      expect(screen.queryByText(course2.name)).not.toBeInTheDocument();
      expect(screen.getByText('1 av 2 banor')).toBeInTheDocument();
    });

    it('shows no-match message when filter excludes all', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText(course1.name)).toBeInTheDocument();
      });

      await user.type(screen.getByPlaceholderText('Sök bannamn…'), 'nonexistent');

      expect(screen.getByText('Inga banor matchar filtret.')).toBeInTheDocument();
      expect(screen.getByText('0 av 2 banor')).toBeInTheDocument();
    });
  });

  describe('Sorting', () => {
    it('sorts by name ascending by default', async () => {
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText(course1.name)).toBeInTheDocument();
      });

      const rows = screen.getAllByRole('row');
      expect(within(rows[1]!).getByText(course1.name)).toBeInTheDocument();
      expect(within(rows[2]!).getByText(course2.name)).toBeInTheDocument();
    });

    it('toggles sort direction on column click', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText(course1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(/^Namn/));

      const rows = screen.getAllByRole('row');
      // Now descending: Bana 2, Bana 1
      expect(within(rows[1]!).getByText(course2.name)).toBeInTheDocument();
      expect(within(rows[2]!).getByText(course1.name)).toBeInTheDocument();
    });
  });

  describe('Create form', () => {
    it('opens create form when clicking Ny bana', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny bana')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny bana'));

      expect(screen.getByLabelText('Namn')).toBeInTheDocument();
      expect(screen.getByLabelText('Längd (m)')).toBeInTheDocument();
      expect(screen.getByLabelText('Lägg till kontroll')).toBeInTheDocument();
    });

    it('submits new course via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny bana')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny bana'));

      await user.type(screen.getByLabelText('Namn'), 'Ny testbana');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows validation error for empty name', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny bana')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny bana'));
      await user.click(screen.getByText('Spara'));

      expect(screen.getByText('Namn krävs')).toBeInTheDocument();
    });

    it('closes create form on cancel', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny bana')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny bana'));
      expect(screen.getByLabelText('Namn')).toBeInTheDocument();

      await user.click(screen.getByText('Avbryt'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows control sequence editor with available controls', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny bana')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny bana'));

      const addSelect = screen.getByLabelText('Lägg till kontroll');
      expect(addSelect).toBeInTheDocument();

      // Should have "— Lägg till kontroll —" + 4 control options
      await waitFor(() => {
        const options = within(addSelect).getAllByRole('option');
        expect(options).toHaveLength(5);
        expect(options[0]).toHaveTextContent('— Lägg till kontroll —');
      });
    });

    it('shows API error on create failure', async () => {
      server.use(
        http.post('/api/courses', () =>
          HttpResponse.json({ message: 'Create failed' }, { status: 500 }),
        ),
      );

      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny bana')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny bana'));
      await user.type(screen.getByLabelText('Namn'), 'Test');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.getByText('Create failed')).toBeInTheDocument();
      });
    });
  });

  describe('Edit form', () => {
    it('shows edit/delete buttons when a course is selected', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText(course1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(course1.name));

      expect(screen.getByText('Redigera')).toBeInTheDocument();
      expect(screen.getByText('Ta bort')).toBeInTheDocument();
    });

    it('opens edit form with course data', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText(course1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(course1.name));
      await user.click(screen.getByText('Redigera'));

      expect(screen.getByLabelText('Namn')).toHaveValue(course1.name);
      expect(screen.getByLabelText('Längd (m)')).toHaveValue(course1.length);
    });

    it('shows control sequence from course data', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText(course1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(course1.name));
      await user.click(screen.getByText('Redigera'));

      // course1 has controls [31, 32, 33] which map to Kontroll 31, 32, 33
      // Each name appears both in the sequence list and in the add-control dropdown
      await waitFor(() => {
        const list = screen.getByRole('list');
        expect(within(list).getByText('Kontroll 31')).toBeInTheDocument();
        expect(within(list).getByText('Kontroll 32')).toBeInTheDocument();
        expect(within(list).getByText('Kontroll 33')).toBeInTheDocument();
      });
    });

    it('submits updated course via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText(course1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(course1.name));
      await user.click(screen.getByText('Redigera'));

      const nameInput = screen.getByLabelText('Namn');
      await user.clear(nameInput);
      await user.type(nameInput, 'Bana 1 Updated');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows validation error for empty name on edit', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText(course1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(course1.name));
      await user.click(screen.getByText('Redigera'));

      const nameInput = screen.getByLabelText('Namn');
      await user.clear(nameInput);
      await user.click(screen.getByText('Spara'));

      expect(screen.getByText('Namn krävs')).toBeInTheDocument();
    });

    it('cancels editing', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText(course1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(course1.name));
      await user.click(screen.getByText('Redigera'));
      expect(screen.getByLabelText('Namn')).toBeInTheDocument();

      await user.click(screen.getByText('Avbryt'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows API error on update failure', async () => {
      server.use(
        http.put('/api/courses/:id', () =>
          HttpResponse.json({ message: 'Update failed' }, { status: 500 }),
        ),
      );

      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText(course1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(course1.name));
      await user.click(screen.getByText('Redigera'));
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.getByText('Update failed')).toBeInTheDocument();
      });
    });
  });

  describe('Delete', () => {
    it('deletes a course via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<CoursesPage />);

      await waitFor(() => {
        expect(screen.getByText(course1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(course1.name));
      await user.click(screen.getByText('Ta bort'));

      await waitFor(() => {
        expect(screen.queryByText('Redigera')).not.toBeInTheDocument();
      });
    });
  });
});

// ─── ControlsPage ───────────────────────────────────────────────────────────

describe('ControlsPage', () => {
  it('shows loading state initially', () => {
    renderWithProviders(<ControlsPage />);
    expect(screen.getByText('Laddar kontroller…')).toBeInTheDocument();
  });

  it('renders control list after loading', async () => {
    renderWithProviders(<ControlsPage />);

    await waitFor(() => {
      expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
    });
    expect(screen.getByText(ctrl2.name)).toBeInTheDocument();
    expect(screen.getByText(ctrl3.name)).toBeInTheDocument();
    expect(screen.getByText(ctrl4.name)).toBeInTheDocument();
  });

  it('renders control details in table', async () => {
    renderWithProviders(<ControlsPage />);

    await waitFor(() => {
      expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
    });

    const rows = screen.getAllByRole('row');
    // Row 0 = header; controls sorted by name asc
    expect(within(rows[1]!).getByText(ctrl1.name)).toBeInTheDocument();
    // Status 0 = Normal
    expect(within(rows[1]!).getByText('Normal')).toBeInTheDocument();
    // Codes displayed as comma-separated
    expect(within(rows[1]!).getByText('31')).toBeInTheDocument();
  });

  it('shows count of controls', async () => {
    renderWithProviders(<ControlsPage />);

    await waitFor(() => {
      expect(screen.getByText('4 av 4 kontroller')).toBeInTheDocument();
    });
  });

  it('shows error message on API failure', async () => {
    server.use(
      http.get('/api/controls', () =>
        HttpResponse.json({ message: 'Server error' }, { status: 500 }),
      ),
    );

    renderWithProviders(<ControlsPage />);

    await waitFor(() => {
      expect(screen.getByText(/Kunde inte ladda kontroller/)).toBeInTheDocument();
    });
  });

  it('shows empty state when no controls exist', async () => {
    server.use(http.get('/api/controls', () => HttpResponse.json([])));

    renderWithProviders(<ControlsPage />);

    await waitFor(() => {
      expect(screen.getByText('Inga kontroller registrerade.')).toBeInTheDocument();
    });
  });

  describe('Search', () => {
    it('filters controls by name search', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
      });

      await user.type(screen.getByPlaceholderText('Sök kontroll…'), 'Kontroll 31');

      expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
      expect(screen.queryByText(ctrl2.name)).not.toBeInTheDocument();
      expect(screen.getByText('1 av 4 kontroller')).toBeInTheDocument();
    });

    it('filters controls by code search', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
      });

      await user.type(screen.getByPlaceholderText('Sök kontroll…'), '34');

      expect(screen.getByText(ctrl4.name)).toBeInTheDocument();
      expect(screen.queryByText(ctrl1.name)).not.toBeInTheDocument();
      expect(screen.getByText('1 av 4 kontroller')).toBeInTheDocument();
    });

    it('shows no-match message when filter excludes all', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
      });

      await user.type(screen.getByPlaceholderText('Sök kontroll…'), 'nonexistent');

      expect(screen.getByText('Inga kontroller matchar filtret.')).toBeInTheDocument();
      expect(screen.getByText('0 av 4 kontroller')).toBeInTheDocument();
    });
  });

  describe('Sorting', () => {
    it('sorts by name ascending by default', async () => {
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
      });

      const rows = screen.getAllByRole('row');
      expect(within(rows[1]!).getByText(ctrl1.name)).toBeInTheDocument();
      expect(within(rows[2]!).getByText(ctrl2.name)).toBeInTheDocument();
      expect(within(rows[3]!).getByText(ctrl3.name)).toBeInTheDocument();
      expect(within(rows[4]!).getByText(ctrl4.name)).toBeInTheDocument();
    });

    it('toggles sort direction on column click', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(/^Namn/));

      const rows = screen.getAllByRole('row');
      // Now descending
      expect(within(rows[1]!).getByText(ctrl4.name)).toBeInTheDocument();
      expect(within(rows[4]!).getByText(ctrl1.name)).toBeInTheDocument();
    });
  });

  describe('Create form', () => {
    it('opens create form when clicking Ny kontroll', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny kontroll')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny kontroll'));

      expect(screen.getByLabelText('Namn')).toBeInTheDocument();
      expect(screen.getByLabelText(/Kontrollkoder/)).toBeInTheDocument();
      expect(screen.getByLabelText('Rogaining-poäng')).toBeInTheDocument();
    });

    it('submits new control via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny kontroll')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny kontroll'));

      await user.type(screen.getByLabelText('Namn'), 'Kontroll 99');
      await user.type(screen.getByLabelText(/Kontrollkoder/), '99');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows validation error for empty name', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny kontroll')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny kontroll'));
      await user.click(screen.getByText('Spara'));

      expect(screen.getByText('Namn krävs')).toBeInTheDocument();
    });

    it('closes create form on cancel', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny kontroll')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny kontroll'));
      expect(screen.getByLabelText('Namn')).toBeInTheDocument();

      await user.click(screen.getByText('Avbryt'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows API error on create failure', async () => {
      server.use(
        http.post('/api/controls', () =>
          HttpResponse.json({ message: 'Create failed' }, { status: 500 }),
        ),
      );

      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny kontroll')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny kontroll'));
      await user.type(screen.getByLabelText('Namn'), 'Test');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.getByText('Create failed')).toBeInTheDocument();
      });
    });
  });

  describe('Edit form', () => {
    it('shows edit/delete buttons when a control is selected', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(ctrl1.name));

      expect(screen.getByText('Redigera')).toBeInTheDocument();
      expect(screen.getByText('Ta bort')).toBeInTheDocument();
    });

    it('opens edit form with control data', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(ctrl1.name));
      await user.click(screen.getByText('Redigera'));

      expect(screen.getByLabelText('Namn')).toHaveValue(ctrl1.name);
      expect(screen.getByLabelText(/Kontrollkoder/)).toHaveValue(ctrl1.codes.join(', '));
    });

    it('submits updated control via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(ctrl1.name));
      await user.click(screen.getByText('Redigera'));

      const nameInput = screen.getByLabelText('Namn');
      await user.clear(nameInput);
      await user.type(nameInput, 'Kontroll 31 Updated');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows validation error for empty name on edit', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(ctrl1.name));
      await user.click(screen.getByText('Redigera'));

      const nameInput = screen.getByLabelText('Namn');
      await user.clear(nameInput);
      await user.click(screen.getByText('Spara'));

      expect(screen.getByText('Namn krävs')).toBeInTheDocument();
    });

    it('cancels editing', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(ctrl1.name));
      await user.click(screen.getByText('Redigera'));
      expect(screen.getByLabelText('Namn')).toBeInTheDocument();

      await user.click(screen.getByText('Avbryt'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows API error on update failure', async () => {
      server.use(
        http.put('/api/controls/:id', () =>
          HttpResponse.json({ message: 'Update failed' }, { status: 500 }),
        ),
      );

      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(ctrl1.name));
      await user.click(screen.getByText('Redigera'));
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.getByText('Update failed')).toBeInTheDocument();
      });
    });
  });

  describe('Delete', () => {
    it('deletes a control via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ControlsPage />);

      await waitFor(() => {
        expect(screen.getByText(ctrl1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(ctrl1.name));
      await user.click(screen.getByText('Ta bort'));

      await waitFor(() => {
        expect(screen.queryByText('Redigera')).not.toBeInTheDocument();
      });
    });
  });
});
