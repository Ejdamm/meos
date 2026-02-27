export { default as apiClient } from './client';
export { ApiRequestError } from './client';
export type { ApiError } from './client';

export { getRunners, getRunner, createRunner, updateRunner, deleteRunner } from './runners';
export { getTeams, getTeam, createTeam, updateTeam, deleteTeam } from './teams';
export { getClasses, getClass, createClass, updateClass, deleteClass } from './classes';
export { getCourses, getCourse, createCourse, updateCourse, deleteCourse } from './courses';
export { getControls, getControl, createControl, updateControl, deleteControl } from './controls';
export { getClubs, getClub, createClub, updateClub, deleteClub } from './clubs';
export { getCards, getCard, readCard } from './cards';
export type { ReadCardRequest } from './cards';
export {
  getCompetitions,
  getCompetition,
  createCompetition,
  updateCompetition,
} from './competitions';
export type { CreateCompetitionRequest } from './competitions';
export { getLists, getList, getResults } from './lists';
export type { ListType, ListData, ResultEntry } from './lists';
export {
  getSpeakerConfig,
  updateSpeakerConfig,
  getSpeakerMonitor,
} from './speaker';
export type { SpeakerConfig, SpeakerMonitorData } from './speaker';
export {
  getAutomations,
  getAutomation,
  createAutomation,
  updateAutomation,
  deleteAutomation,
  getAutomationStatus,
} from './automations';
export type { Automation, AutomationStatus } from './automations';
export { importCsv, importIof, exportCsv, exportIof } from './importExport';
export type { ImportResult, ExportResult } from './importExport';
export { getPunches } from './punches';
