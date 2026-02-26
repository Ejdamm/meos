// Stub implementations of MeosSQL when MEOS_USE_MYSQL is disabled.
// All methods return failure/no-op — the application runs without database sync.
#ifndef MEOS_USE_MYSQL

#include "MeosSQL.h"

MeosSQL::MeosSQL() : warnedOldVersion(false), monitorId(0), buildVersion(0), serverPort(0) {}
MeosSQL::~MeosSQL() {}

OpFailStatus MeosSQL::enumerateImages(vector<pair<wstring, uint64_t>> &) { return opStatusFail; }
OpFailStatus MeosSQL::getImage(uint64_t, wstring &, vector<uint8_t> &) { return opStatusFail; }
OpFailStatus MeosSQL::storeImage(uint64_t, const wstring &, const vector<uint8_t> &) { return opStatusFail; }

bool MeosSQL::synchronizeList(oEvent *, oListId) { return false; }
OpFailStatus MeosSQL::synchronizeUpdate(oBase *) { return opStatusFail; }

bool MeosSQL::checkConsistency(oEvent *, bool) { return false; }
void MeosSQL::clearReadTimes() {}

bool MeosSQL::dropDatabase(oEvent *) { return false; }
bool MeosSQL::checkConnection(oEvent *) { return false; }
void MeosSQL::processMissingObjects() {}

bool MeosSQL::repairTables(const string &, vector<string> &) { return false; }
bool MeosSQL::getErrorMessage(string &err) { err = "MySQL support not compiled in"; return true; }
bool MeosSQL::reConnect() { return false; }
bool MeosSQL::listCompetitions(oEvent *, bool) { return false; }
bool MeosSQL::remove(oBase *) { return false; }

bool MeosSQL::createRunnerDB(oEvent *, sqlwrapper::QueryWrapper &) { return false; }
OpFailStatus MeosSQL::uploadRunnerDB(oEvent *) { return opStatusFail; }

MeosSQL::OpenStatus MeosSQL::openDB(oEvent *, bool) { return OpenStatus::Fail; }
bool MeosSQL::closeDB() { return false; }
string MeosSQL::serverVersion() const { return ""; }

OpFailStatus MeosSQL::syncRead(bool, oBase *) { return opStatusFail; }
int MeosSQL::getModifiedMask(oEvent &) { return 0; }

#endif // !MEOS_USE_MYSQL
