import { describe, it, expect } from 'vitest';
import { screen, waitFor, within } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { http, HttpResponse } from 'msw';
import { renderWithProviders, fixtures, server } from '../test';
import ClassesPage from '../pages/ClassesPage';

const cls1 = fixtures.classes[0]!; // H21, individual, courseId 1
const cls2 = fixtures.classes[1]!; // D21, individual, courseId 2
const cls3 = fixtures.classes[2]!; // Stafett, relay, courseId 1

describe('ClassesPage', () => {
  it('shows loading state initially', () => {
    renderWithProviders(<ClassesPage />);
    expect(screen.getByText('Laddar klasser…')).toBeInTheDocument();
  });

  it('renders class list after loading', async () => {
    renderWithProviders(<ClassesPage />);

    await waitFor(() => {
      expect(screen.getByText(cls1.name)).toBeInTheDocument();
    });
    expect(screen.getByText(cls2.name)).toBeInTheDocument();
    expect(screen.getByText(cls3.name)).toBeInTheDocument();
  });

  it('renders class details in table', async () => {
    renderWithProviders(<ClassesPage />);

    await waitFor(() => {
      expect(screen.getByText(cls1.name)).toBeInTheDocument();
    });
    // Table rows contain type values; filter dropdown also has them
    const rows = screen.getAllByRole('row');
    // Row 0 is header; check data rows have correct type
    expect(within(rows[1]!).getByText(cls2.type)).toBeInTheDocument(); // D21 - individual
    expect(within(rows[3]!).getByText(cls3.type)).toBeInTheDocument(); // Stafett - relay
    // Course names resolved from courses fixture
    expect(screen.getAllByText('Bana 1').length).toBeGreaterThanOrEqual(2); // cls1 + cls3
    expect(screen.getByText('Bana 2')).toBeInTheDocument();
  });

  it('shows count of classes', async () => {
    renderWithProviders(<ClassesPage />);

    await waitFor(() => {
      expect(screen.getByText('3 av 3 klasser')).toBeInTheDocument();
    });
  });

  it('shows error message on API failure', async () => {
    server.use(
      http.get('/api/classes', () =>
        HttpResponse.json({ message: 'Server error' }, { status: 500 }),
      ),
    );

    renderWithProviders(<ClassesPage />);

    await waitFor(() => {
      expect(screen.getByText(/Kunde inte ladda klasser/)).toBeInTheDocument();
    });
  });

  it('shows empty state when no classes exist', async () => {
    server.use(http.get('/api/classes', () => HttpResponse.json([])));

    renderWithProviders(<ClassesPage />);

    await waitFor(() => {
      expect(screen.getByText('Inga klasser registrerade.')).toBeInTheDocument();
    });
  });

  describe('Search and filter', () => {
    it('filters classes by name search', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText(cls1.name)).toBeInTheDocument();
      });

      await user.type(screen.getByPlaceholderText('Sök klassnamn…'), 'H21');

      expect(screen.getByText(cls1.name)).toBeInTheDocument();
      expect(screen.queryByText(cls2.name)).not.toBeInTheDocument();
      expect(screen.queryByText(cls3.name)).not.toBeInTheDocument();
      expect(screen.getByText('1 av 3 klasser')).toBeInTheDocument();
    });

    it('filters classes by type', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText(cls1.name)).toBeInTheDocument();
      });

      await user.selectOptions(screen.getByLabelText('Filtrera typ'), 'relay');

      expect(screen.getByText(cls3.name)).toBeInTheDocument();
      expect(screen.queryByText(cls1.name)).not.toBeInTheDocument();
      expect(screen.queryByText(cls2.name)).not.toBeInTheDocument();
      expect(screen.getByText('1 av 3 klasser')).toBeInTheDocument();
    });

    it('shows no-match message when filter excludes all', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText(cls1.name)).toBeInTheDocument();
      });

      await user.type(screen.getByPlaceholderText('Sök klassnamn…'), 'nonexistent');

      expect(screen.getByText('Inga klasser matchar filtret.')).toBeInTheDocument();
      expect(screen.getByText('0 av 3 klasser')).toBeInTheDocument();
    });
  });

  describe('Sorting', () => {
    it('sorts by name ascending by default', async () => {
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText(cls1.name)).toBeInTheDocument();
      });

      const rows = screen.getAllByRole('row');
      // Row 0 is header; sorted by name asc: D21, H21, Stafett
      expect(within(rows[1]!).getByText(cls2.name)).toBeInTheDocument();
      expect(within(rows[2]!).getByText(cls1.name)).toBeInTheDocument();
      expect(within(rows[3]!).getByText(cls3.name)).toBeInTheDocument();
    });

    it('toggles sort direction on column click', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText(cls1.name)).toBeInTheDocument();
      });

      // Click Namn column to toggle to desc
      await user.click(screen.getByText(/^Namn/));

      const rows = screen.getAllByRole('row');
      // Now descending: Stafett, H21, D21
      expect(within(rows[1]!).getByText(cls3.name)).toBeInTheDocument();
      expect(within(rows[2]!).getByText(cls1.name)).toBeInTheDocument();
      expect(within(rows[3]!).getByText(cls2.name)).toBeInTheDocument();
    });
  });

  describe('Create form', () => {
    it('opens create form when clicking Ny klass', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny klass')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny klass'));

      expect(screen.getByLabelText('Namn')).toBeInTheDocument();
      expect(screen.getByLabelText('Typ')).toBeInTheDocument();
      expect(screen.getByLabelText('Antal sträckor')).toBeInTheDocument();
      expect(screen.getByLabelText('Bana')).toBeInTheDocument();
    });

    it('submits new class via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny klass')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny klass'));

      await user.type(screen.getByLabelText('Namn'), 'H16');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows validation error for empty name', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny klass')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny klass'));
      await user.click(screen.getByText('Spara'));

      expect(screen.getByText('Namn krävs')).toBeInTheDocument();
    });

    it('closes create form on cancel', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny klass')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny klass'));
      expect(screen.getByLabelText('Namn')).toBeInTheDocument();

      await user.click(screen.getByText('Avbryt'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows course options in form', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny klass')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny klass'));

      const courseSelect = screen.getByLabelText('Bana');
      expect(courseSelect).toBeInTheDocument();
      // Should have "— Ingen bana —" + 2 course options
      const options = within(courseSelect).getAllByRole('option');
      expect(options).toHaveLength(3);
      expect(options[0]).toHaveTextContent('— Ingen bana —');
      expect(options[1]).toHaveTextContent('Bana 1');
      expect(options[2]).toHaveTextContent('Bana 2');
    });

    it('shows API error on create failure', async () => {
      server.use(
        http.post('/api/classes', () =>
          HttpResponse.json({ message: 'Create failed' }, { status: 500 }),
        ),
      );

      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText('Ny klass')).toBeInTheDocument();
      });
      await user.click(screen.getByText('Ny klass'));
      await user.type(screen.getByLabelText('Namn'), 'Test');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.getByText('Create failed')).toBeInTheDocument();
      });
    });
  });

  describe('Edit form', () => {
    it('shows edit/delete buttons when a class is selected', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText(cls1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(cls1.name));

      expect(screen.getByText('Redigera')).toBeInTheDocument();
      expect(screen.getByText('Ta bort')).toBeInTheDocument();
    });

    it('opens edit form with class data', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText(cls1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(cls1.name));
      await user.click(screen.getByText('Redigera'));

      expect(screen.getByLabelText('Namn')).toHaveValue(cls1.name);
      expect(screen.getByLabelText('Typ')).toHaveValue(cls1.type);
      expect(screen.getByLabelText('Antal sträckor')).toHaveValue(cls1.numStages);
      expect(screen.getByLabelText('Bana')).toHaveValue(String(cls1.courseId));
    });

    it('submits updated class via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText(cls1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(cls1.name));
      await user.click(screen.getByText('Redigera'));

      const nameInput = screen.getByLabelText('Namn');
      await user.clear(nameInput);
      await user.type(nameInput, 'H21 Elite');
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows validation error for empty name on edit', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText(cls1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(cls1.name));
      await user.click(screen.getByText('Redigera'));

      const nameInput = screen.getByLabelText('Namn');
      await user.clear(nameInput);
      await user.click(screen.getByText('Spara'));

      expect(screen.getByText('Namn krävs')).toBeInTheDocument();
    });

    it('cancels editing', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText(cls1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(cls1.name));
      await user.click(screen.getByText('Redigera'));
      expect(screen.getByLabelText('Namn')).toBeInTheDocument();

      await user.click(screen.getByText('Avbryt'));

      await waitFor(() => {
        expect(screen.queryByLabelText('Namn')).not.toBeInTheDocument();
      });
    });

    it('shows API error on update failure', async () => {
      server.use(
        http.put('/api/classes/:id', () =>
          HttpResponse.json({ message: 'Update failed' }, { status: 500 }),
        ),
      );

      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText(cls1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(cls1.name));
      await user.click(screen.getByText('Redigera'));
      await user.click(screen.getByText('Spara'));

      await waitFor(() => {
        expect(screen.getByText('Update failed')).toBeInTheDocument();
      });
    });
  });

  describe('Delete', () => {
    it('deletes a class via API', async () => {
      const user = userEvent.setup();
      renderWithProviders(<ClassesPage />);

      await waitFor(() => {
        expect(screen.getByText(cls1.name)).toBeInTheDocument();
      });

      await user.click(screen.getByText(cls1.name));
      await user.click(screen.getByText('Ta bort'));

      await waitFor(() => {
        expect(screen.queryByText('Redigera')).not.toBeInTheDocument();
      });
    });
  });
});
