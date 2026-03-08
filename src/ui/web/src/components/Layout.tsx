import { useState } from 'react';
import { NavLink, Outlet } from 'react-router-dom';
import { 
  Menu, X, Trophy, Users, Route, Target, 
  Flag, User, Users2, BarChart2, List 
} from 'lucide-react';
import { cn } from '../lib/utils';

const navItems = [
  { path: '/competition', label: 'Competition', icon: Trophy },
  { path: '/classes', label: 'Classes', icon: Users },
  { path: '/courses', label: 'Courses', icon: Route },
  { path: '/controls', label: 'Controls', icon: Target },
  { path: '/clubs', label: 'Clubs', icon: Flag },
  { path: '/runners', label: 'Runners', icon: User },
  { path: '/teams', label: 'Teams', icon: Users2 },
  { path: '/results', label: 'Results', icon: BarChart2 },
  { path: '/startlist', label: 'Start List', icon: List },
];

export default function Layout() {
  const [isSidebarOpen, setIsSidebarOpen] = useState(false);

  return (
    <div className="flex min-h-screen bg-slate-50">
      {/* Sidebar for desktop */}
      <aside className={cn(
        "fixed inset-y-0 left-0 z-50 w-64 bg-white border-r border-slate-200 transition-transform duration-300 lg:translate-x-0 lg:static lg:inset-0",
        isSidebarOpen ? "translate-x-0" : "-translate-x-full"
      )}>
        <div className="flex items-center justify-between h-16 px-6 border-b border-slate-200">
          <span className="text-xl font-bold text-slate-900">MeOS Web</span>
          <button 
            className="lg:hidden p-2 text-slate-500 hover:bg-slate-100 rounded-md"
            onClick={() => setIsSidebarOpen(false)}
          >
            <X size={20} />
          </button>
        </div>
        <nav className="p-4 space-y-1">
          {navItems.map((item) => (
            <NavLink
              key={item.path}
              to={item.path}
              onClick={() => setIsSidebarOpen(false)}
              className={({ isActive }) => cn(
                "flex items-center gap-3 px-3 py-2 text-sm font-medium rounded-md transition-colors",
                isActive 
                  ? "bg-slate-100 text-slate-900" 
                  : "text-slate-600 hover:bg-slate-50 hover:text-slate-900"
              )}
            >
              <item.icon size={18} />
              {item.label}
            </NavLink>
          ))}
        </nav>
      </aside>

      {/* Overlay for mobile */}
      {isSidebarOpen && (
        <div 
          className="fixed inset-0 z-40 bg-slate-900/50 lg:hidden" 
          onClick={() => setIsSidebarOpen(false)}
        />
      )}

      {/* Main content area */}
      <div className="flex-1 flex flex-col min-w-0 overflow-hidden">
        {/* Header with hamburger menu */}
        <header className="flex items-center h-16 px-4 bg-white border-b border-slate-200 lg:hidden">
          <button 
            className="p-2 -ml-2 text-slate-500 hover:bg-slate-100 rounded-md"
            onClick={() => setIsSidebarOpen(true)}
          >
            <Menu size={24} />
          </button>
          <span className="ml-4 text-lg font-bold text-slate-900">MeOS Web</span>
        </header>

        <main className="flex-1 overflow-y-auto p-6">
          <Outlet />
        </main>
      </div>
    </div>
  );
}
