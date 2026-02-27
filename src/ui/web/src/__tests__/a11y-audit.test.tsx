import { describe, it, expect } from 'vitest';
import { screen, waitFor } from '@testing-library/react';
import { axe } from 'vitest-axe';
import * as matchers from 'vitest-axe/matchers';
import { renderWithProviders } from '../test';

expect.extend(matchers as never);
import CompetitionPage from '../pages/CompetitionPage';
import RunnersPage from '../pages/RunnersPage';
import TeamsPage from '../pages/TeamsPage';
import ClassesPage from '../pages/ClassesPage';
import CoursesPage from '../pages/CoursesPage';
import ControlsPage from '../pages/ControlsPage';
import ClubsPage from '../pages/ClubsPage';
import SiPage from '../pages/SiPage';
import ListsPage from '../pages/ListsPage';
import SpeakerPage from '../pages/SpeakerPage';
import AutomationsPage from '../pages/AutomationsPage';

const AXE_OPTIONS = {
  rules: {
    // Disable color-contrast in jsdom (can't compute styles reliably)
    'color-contrast': { enabled: false },
  },
};

async function waitForLoaded() {
  await waitFor(() => {
    expect(screen.queryByText(/Laddar/)).not.toBeInTheDocument();
  });
}

describe('Accessibility audit (axe-core)', () => {
  it('CompetitionPage has no serious a11y violations', async () => {
    const { container } = renderWithProviders(<CompetitionPage />);
    await waitForLoaded();
    const results = await axe(container, AXE_OPTIONS);
    expect(results).toHaveNoViolations();
  });

  it('RunnersPage has no serious a11y violations', async () => {
    const { container } = renderWithProviders(<RunnersPage />);
    await waitForLoaded();
    const results = await axe(container, AXE_OPTIONS);
    expect(results).toHaveNoViolations();
  });

  it('TeamsPage has no serious a11y violations', async () => {
    const { container } = renderWithProviders(<TeamsPage />);
    await waitForLoaded();
    const results = await axe(container, AXE_OPTIONS);
    expect(results).toHaveNoViolations();
  });

  it('ClassesPage has no serious a11y violations', async () => {
    const { container } = renderWithProviders(<ClassesPage />);
    await waitForLoaded();
    const results = await axe(container, AXE_OPTIONS);
    expect(results).toHaveNoViolations();
  });

  it('CoursesPage has no serious a11y violations', async () => {
    const { container } = renderWithProviders(<CoursesPage />);
    await waitForLoaded();
    const results = await axe(container, AXE_OPTIONS);
    expect(results).toHaveNoViolations();
  });

  it('ControlsPage has no serious a11y violations', async () => {
    const { container } = renderWithProviders(<ControlsPage />);
    await waitForLoaded();
    const results = await axe(container, AXE_OPTIONS);
    expect(results).toHaveNoViolations();
  });

  it('ClubsPage has no serious a11y violations', async () => {
    const { container } = renderWithProviders(<ClubsPage />);
    await waitForLoaded();
    const results = await axe(container, AXE_OPTIONS);
    expect(results).toHaveNoViolations();
  });

  it('SiPage has no serious a11y violations', async () => {
    const { container } = renderWithProviders(<SiPage />);
    await waitForLoaded();
    const results = await axe(container, AXE_OPTIONS);
    expect(results).toHaveNoViolations();
  });

  it('ListsPage has no serious a11y violations', async () => {
    const { container } = renderWithProviders(<ListsPage />);
    await waitForLoaded();
    const results = await axe(container, AXE_OPTIONS);
    expect(results).toHaveNoViolations();
  });

  it('SpeakerPage has no serious a11y violations', async () => {
    const { container } = renderWithProviders(<SpeakerPage />);
    await waitForLoaded();
    const results = await axe(container, AXE_OPTIONS);
    expect(results).toHaveNoViolations();
  });

  it('AutomationsPage has no serious a11y violations', async () => {
    const { container } = renderWithProviders(<AutomationsPage />);
    await waitForLoaded();
    const results = await axe(container, AXE_OPTIONS);
    expect(results).toHaveNoViolations();
  });
});
