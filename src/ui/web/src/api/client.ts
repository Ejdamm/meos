import * as T from './types';

const API_BASE = '/api/v1';

async function request<T>(path: string, options?: RequestInit): Promise<T> {
  const headers = new Headers(options?.headers || {});
  
  // Set Content-Type only if it hasn't been set to something else (e.g., undefined or a specific type)
  if (!headers.has('Content-Type') && !(options?.body instanceof FormData)) {
    headers.set('Content-Type', 'application/json');
  } else if (headers.get('Content-Type') === 'undefined') {
    // This is a trick to delete the header so the browser can set it (e.g., for FormData)
    headers.delete('Content-Type');
  }

  const response = await fetch(`${API_BASE}${path}`, {
    ...options,
    headers,
  });

  if (!response.ok) {
    throw new Error(`API error: ${response.status} ${response.statusText}`);
  }

  // Handle empty responses
  if (response.status === 204) {
    return {} as T;
  }

  return response.json();
}

export const api = {
  // Competitions
  getCompetitions: () => request<T.Competition[]>('/competitions'),
  getCompetition: (id: number) => request<T.Competition>(`/competitions/${id}`),
  createCompetition: (data: Partial<T.Competition>) => request<T.Competition>('/competitions', { method: 'POST', body: JSON.stringify(data) }),
  updateCompetition: (id: number, data: Partial<T.Competition>) => request<T.Competition>(`/competitions/${id}`, { method: 'PUT', body: JSON.stringify(data) }),
  deleteCompetition: (id: number) => request<void>(`/competitions/${id}`, { method: 'DELETE' }),

  // Classes
  getClasses: () => request<T.Class[]>('/classes'),
  getClass: (id: number) => request<T.Class>(`/classes/${id}`),
  createClass: (data: Partial<T.Class>) => request<T.Class>('/classes', { method: 'POST', body: JSON.stringify(data) }),
  updateClass: (id: number, data: Partial<T.Class>) => request<T.Class>(`/classes/${id}`, { method: 'PUT', body: JSON.stringify(data) }),
  deleteClass: (id: number) => request<void>(`/classes/${id}`, { method: 'DELETE' }),

  // Courses
  getCourses: () => request<T.Course[]>('/courses'),
  getCourse: (id: number) => request<T.Course>(`/courses/${id}`),
  createCourse: (data: Partial<T.Course>) => request<T.Course>('/courses', { method: 'POST', body: JSON.stringify(data) }),
  updateCourse: (id: number, data: Partial<T.Course>) => request<T.Course>(`/courses/${id}`, { method: 'PUT', body: JSON.stringify(data) }),
  deleteCourse: (id: number) => request<void>(`/courses/${id}`, { method: 'DELETE' }),

  // Controls
  getControls: () => request<T.Control[]>('/controls'),
  getControl: (id: number) => request<T.Control>(`/controls/${id}`),
  createControl: (data: Partial<T.Control>) => request<T.Control>('/controls', { method: 'POST', body: JSON.stringify(data) }),
  updateControl: (id: number, data: Partial<T.Control>) => request<T.Control>(`/controls/${id}`, { method: 'PUT', body: JSON.stringify(data) }),
  deleteControl: (id: number) => request<void>(`/controls/${id}`, { method: 'DELETE' }),

  // Clubs
  getClubs: () => request<T.Club[]>('/clubs'),
  getClub: (id: number) => request<T.Club>(`/clubs/${id}`),
  createClub: (data: Partial<T.Club>) => request<T.Club>('/clubs', { method: 'POST', body: JSON.stringify(data) }),
  updateClub: (id: number, data: Partial<T.Club>) => request<T.Club>(`/clubs/${id}`, { method: 'PUT', body: JSON.stringify(data) }),
  deleteClub: (id: number) => request<void>(`/clubs/${id}`, { method: 'DELETE' }),

  // Runners
  getRunners: (params?: { classId?: number; clubId?: number; search?: string }) => {
    const query = new URLSearchParams();
    if (params?.classId) query.append('classId', params.classId.toString());
    if (params?.clubId) query.append('clubId', params.clubId.toString());
    if (params?.search) query.append('search', params.search);
    const queryString = query.toString();
    return request<T.Runner[]>(`/runners${queryString ? `?${queryString}` : ''}`);
  },
  getRunner: (id: number) => request<T.Runner>(`/runners/${id}`),
  createRunner: (data: Partial<T.Runner>) => request<T.Runner>('/runners', { method: 'POST', body: JSON.stringify(data) }),
  updateRunner: (id: number, data: Partial<T.Runner>) => request<T.Runner>(`/runners/${id}`, { method: 'PUT', body: JSON.stringify(data) }),
  deleteRunner: (id: number) => request<void>(`/runners/${id}`, { method: 'DELETE' }),
  createRunnersBulk: (data: Partial<T.Runner>[]) => request<T.Runner[]>('/runners/bulk', { method: 'POST', body: JSON.stringify(data) }),
  updateRunnersBulk: (ids: number[], data: Partial<T.Runner>) => request<void>('/runners/bulk', { method: 'PATCH', body: JSON.stringify({ ids, data }) }),
  assignStartTimesBulk: (ids: number[]) => request<void>('/runners/bulk/start-times', { method: 'POST', body: JSON.stringify({ ids }) }),
  setRunnerStatus: (id: number, status: T.RunnerStatus) => request<T.Runner>(`/runners/${id}/status`, { method: 'POST', body: JSON.stringify({ status }) }),

  // Import
  importRunnersFromXML: (file: File) => {
    const formData = new FormData();
    formData.append('file', file);
    return request<T.Runner[]>('/import/iof-xml', { 
      method: 'POST', 
      body: formData,
      headers: {
        // Let the browser set the Content-Type with the boundary
        'Content-Type': undefined as any,
      } 
    });
  },

  // Teams
  getTeams: (params?: { classId?: number; clubId?: number }) => {
    const query = new URLSearchParams();
    if (params?.classId) query.append('classId', params.classId.toString());
    if (params?.clubId) query.append('clubId', params.clubId.toString());
    const queryString = query.toString();
    return request<T.Team[]>(`/teams${queryString ? `?${queryString}` : ''}`);
  },
  getTeam: (id: number) => request<T.Team>(`/teams/${id}`),
  createTeam: (data: Partial<T.Team>) => request<T.Team>('/teams', { method: 'POST', body: JSON.stringify(data) }),
  updateTeam: (id: number, data: Partial<T.Team>) => request<T.Team>(`/teams/${id}`, { method: 'PUT', body: JSON.stringify(data) }),
  deleteTeam: (id: number) => request<void>(`/teams/${id}`, { method: 'DELETE' }),

  // Results
  getResults: (params?: { classId?: number }) => {
    const query = new URLSearchParams();
    if (params?.classId) query.append('classId', params.classId.toString());
    const queryString = query.toString();
    return request<T.Result[]>(`/results${queryString ? `?${queryString}` : ''}`);
  },

  // StartList
  getStartList: (params?: { classId?: number }) => {
    const query = new URLSearchParams();
    if (params?.classId) query.append('classId', params.classId.toString());
    const queryString = query.toString();
    return request<T.StartListEntry[]>(`/startlist${queryString ? `?${queryString}` : ''}`);
  },

  // Export
  exportResultsXML: (params?: { classId?: number }) => {
    const query = new URLSearchParams();
    if (params?.classId) query.append('classId', params.classId.toString());
    const queryString = query.toString();
    return `${API_BASE}/results/export/iof-xml${queryString ? `?${queryString}` : ''}`;
  },
  exportStartListXML: (params?: { classId?: number }) => {
    const query = new URLSearchParams();
    if (params?.classId) query.append('classId', params.classId.toString());
    const queryString = query.toString();
    return `${API_BASE}/startlist/export/iof-xml${queryString ? `?${queryString}` : ''}`;
  },

  // Cards
  registerCard: (data: { cardNumber: number; runnerId?: number }) => request<void>('/cards', { method: 'POST', body: JSON.stringify(data) }),
};
