import { NavLink, Outlet } from 'react-router-dom';
import './AppShell.css';

interface NavItem {
  to: string;
  label: string;
}

const navItems: NavItem[] = [
  { to: '/competition', label: 'Tävling' },
  { to: '/runners', label: 'Deltagare' },
  { to: '/teams', label: 'Lag' },
  { to: '/classes', label: 'Klasser' },
  { to: '/courses', label: 'Banor' },
  { to: '/controls', label: 'Kontroller' },
  { to: '/clubs', label: 'Klubbar' },
  { to: '/si', label: 'SI' },
  { to: '/lists', label: 'Listor' },
  { to: '/speaker', label: 'Speaker' },
  { to: '/automations', label: 'Automatik' },
];

function AppShell(): React.JSX.Element {
  return (
    <div className="app-shell">
      <a href="#main-content" className="skip-nav">
        Hoppa till huvudinnehåll
      </a>
      <header className="app-header">
        <h1 className="app-header__title">MeOS</h1>
      </header>

      <nav className="app-nav" aria-label="Huvudnavigering">
        {navItems.map((item) => (
          <NavLink
            key={item.to}
            to={item.to}
            className={({ isActive }) =>
              `app-nav__link${isActive ? ' app-nav__link--active' : ''}`
            }
          >
            {item.label}
          </NavLink>
        ))}
      </nav>

      <main id="main-content" className="app-main">
        <Outlet />
      </main>
    </div>
  );
}

export default AppShell;
