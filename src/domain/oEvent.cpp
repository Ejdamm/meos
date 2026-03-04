#include "oEvent.h"
#include "gdioutput.h"
#include "MeOSFeatures.h"
#include "RunnerDB.h"
#include "metalist.h"
#include "cardsystem.h"
#include <assert.h>

oEvent::oEvent(gdioutput &gdi) : gdibase(gdi) {
    meosFeatures = new MeOSFeatures();
    runnerDB = make_shared<RunnerDB>();
    listContainer = new MetaListContainer();
}

oEvent::~oEvent() {
    delete meosFeatures;
    delete listContainer;
}

void oEvent::getRunners(int classId, int courseId, vector<pRunner> &r, bool sortRunners) {}
void oEvent::getRunners(const set<int> &classId, vector<pRunner> &r, bool synchRunners) {}
void oEvent::getRunners(const set<int> &classId, vector<pRunner> &r) const {}

void oEvent::getTeams(int classId, vector<pTeam> &t, bool sortTeams) {}
void oEvent::getTeams(const set<int> &classId, vector<pTeam> &t, bool synchTeams) {}
void oEvent::getTeams(const set<int> &classId, vector<pTeam> &t) const {}

void oEvent::classChanged(pClass cls, bool punchOnly) {}
void oEvent::reCalculateLeaderTimes(int classId) {}

const string& oEvent::getServerName() const { static string s; return s; }
void oEvent::closeDBConnection() {}

void oEvent::getLatestPunches(int firstTime, vector<const oFreePunch *> &punches) const {}
void oEvent::getFreeControls(set<int> &controlId) const {}

pFreePunch oEvent::addFreePunch(int time, int type, int unit, int card, bool updateRunner, bool isOriginal) { return nullptr; }
pFreePunch oEvent::addFreePunch(oFreePunch &fp) { return nullptr; }

void oEvent::computeRogainingStatistics() const {}
void oEvent::setupControlStatistics() const {}

int oEvent::setupTimeLineEvents(int currentTime) { return 0; }
void oEvent::renderTimeLineEvents(gdioutput &gdi) const {}

void oEvent::generateList(gdioutput &gdi, bool reEvaluate, const oListInfo &li, bool updateScrollBars) {}
void oEvent::formatHeader(gdioutput& gdi, const oListInfo& li, const pRunner rInput) {}

void oEvent::calculateResults(const set<int> &classes, ResultType result, bool includePreliminary) const {}
void oEvent::calculateTeamResults(const set<int> &cls, ResultType resultType) {}

void oEvent::convertTimes(pRunner runner, void *sic) const {}
void oEvent::generateTestCard(void *sic) const {}

oRunner* oEvent::addRunner(const std::wstring& pname, const std::wstring& pclub, int classId, int cardNo, const std::wstring& birthDate, bool autoAdd) { return nullptr; }
oRunner* oEvent::getRunner(int Id, int stage) const { return nullptr; }

void oEvent::getPredefinedClassTypes(map<wstring, ClassMetaType> &types) const {}

MetaListContainer &oEvent::getListContainer() const { return *listContainer; }

void oEvent::setRunnerIdTypes(const pair<string, string> &preferredIdType) {}
pair<wstring, wstring> oEvent::getRunnerIdTypes() const { return make_pair(L"", L""); }

void oEvent::updateListReferences(const string &oldId, const string &newId) {}
void oEvent::supportSubSeconds(bool use) {}
void oEvent::readStartGroups() const {}

bool oEvent::listBackups(gdioutput& gdi, GUICALLBACK cb) { return false; }
bool oEvent::enumerateBackups(const std::wstring& path) { return false; }
bool oEvent::enumerateBackups(const std::wstring &file, const std::wstring &filetype, int type) { return false; }

void oEvent::assignCardInteractive(gdioutput& gdi, GUICALLBACK cb, SortOrder& orderRunners) {}

pControl oEvent::getControlByType(int type) const { return nullptr; }
void oEvent::getExistingUnits(vector<pair<SpecialPunch, int>>& typeUnit) {}

const CardSystem& oEvent::getCardSystem() const { static CardSystem cs; return cs; }

void oEvent::changedObject() {}
oDataContainer &oEvent::getDataBuffers(pvoid &data, pvoid &olddata, pvectorstr &strData) const { static oDataContainer dummy(0); return dummy; }
void oEvent::merge(const oBase &input, const oBase *base) {}
wstring oEvent::getInfo() const { return L""; }
void oEvent::remove() {}
bool oEvent::canRemove() const { return true; }
