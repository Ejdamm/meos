#include "StdAfx.h"
#include "oEvent.h"
#include "gdioutput.h"
#include "Table.h"
#include "MeOSFeatures.h"
#include "oCourse.h"
#include "oClass.h"
#include "oRunner.h"
#include "oTeam.h"
#include "oFreePunch.h"

// Define types that are forward declared in oEvent.h
struct GeneralResultCtr { ~GeneralResultCtr() {} };
class MachineContainer { public: ~MachineContainer() {} };
class RunnerDB { public: ~RunnerDB() {} };
class DirectSocket { public: ~DirectSocket() {} };
class MetaListContainer { public: ~MetaListContainer() {} };
class MapDataContainer { public: ~MapDataContainer() {} };
class ClassConfigInfo { public: ~ClassConfigInfo() {} };
class oFreeImport { public: ~oFreeImport() {} };

// oAbstractRunner::TempResult stubs
oAbstractRunner::TempResult::TempResult() : startTime(0), runningTime(0), timeAfter(0), points(0), place(0), status(StatusOK) {}
oAbstractRunner::TempResult::TempResult(RunnerStatus statusIn, int startTime, int runningTime, int points) : startTime(startTime), runningTime(runningTime), points(points), status(statusIn), timeAfter(0), place(0) {}
void oAbstractRunner::TempResult::reset() {}
const wstring &oAbstractRunner::TempResult::getStatusS(RunnerStatus inputStatus) const { static wstring s; return s; }
const wstring &oAbstractRunner::TempResult::getPrintPlaceS(bool withDot) const { static wstring s; return s; }
const wstring &oAbstractRunner::TempResult::getRunningTimeS(int inputTime, SubSecond mode) const { static wstring s; return s; }
const wstring &oAbstractRunner::TempResult::getFinishTimeS(const oEvent *oe, SubSecond mode) const { static wstring s; return s; }
const wstring &oAbstractRunner::TempResult::getStartTimeS(const oEvent *oe, SubSecond mode) const { static wstring s; return s; }
const wstring &oAbstractRunner::TempResult::getOutputTime(int ix) const { static wstring s; return s; }
int oAbstractRunner::TempResult::getOutputNumber(int ix) const { return 0; }

// oAbstractRunner stubs
oAbstractRunner::oAbstractRunner(oEvent *poe, bool loading) : oBase(poe) {}
void oAbstractRunner::merge(const oBase &input, const oBase *base) {}
bool oAbstractRunner::startTimeAvailable() const { return false; }
int oAbstractRunner::getSpeakerPriority() const { return 0; }
int oAbstractRunner::getBirthAge() const { return 0; }
void oAbstractRunner::setName(const wstring &n, bool manualChange) {}
void oAbstractRunner::setFinishTime(int t) {}
void oAbstractRunner::setClub(const wstring &clubName) {}
pClub oAbstractRunner::setClubId(int clubId) { return nullptr; }
void oAbstractRunner::setClassId(int id, bool isManualUpdate) {}
void oAbstractRunner::setStartNo(int no, ChangeType changeType) {}
wstring oAbstractRunner::getEntryDate(bool useTeamEntryDate) const { return L""; }
wstring oAbstractRunner::getInfo() const { return L"oAbstractRunner"; }
bool oAbstractRunner::isVacant() const { return false; }
int oAbstractRunner::getRunningTime(bool computedTime) const { return 0; }
const wstring& oAbstractRunner::getStartTimeS() const { static wstring s; return s; }
const wstring& oAbstractRunner::getStartTimeCompact() const { static wstring s; return s; }
int oAbstractRunner::getTotalRunningTime() const { return 0; }
int oAbstractRunner::getPrelRunningTime() const { return 0; }
const wstring& oAbstractRunner::getTotalStatusS(bool b) const { static wstring s; return s; }
RunnerStatus oAbstractRunner::getTotalStatus(bool b) const { return StatusOK; }
void oAbstractRunner::hasManuallyUpdatedTimeStatus() {}
bool oAbstractRunner::preventRestart() const { return false; }
void oAbstractRunner::preventRestart(bool state) {}
int oAbstractRunner::getEntrySource() const { return 0; }
void oAbstractRunner::setEntrySource(int src) {}
void oAbstractRunner::flagEntryTouched(bool flag) {}
bool oAbstractRunner::isEntryTouched() const { return false; }
int oAbstractRunner::getPaymentMode() const { return 0; }
void oAbstractRunner::setPaymentMode(int mode) {}
bool oAbstractRunner::hasFlag(TransferFlags flag) const { return false; }
void oAbstractRunner::setFlag(TransferFlags flag, bool state) {}
const oAbstractRunner::TempResult &oAbstractRunner::getTempResult(int tempResultIndex) const { return tmpResult; }
oAbstractRunner::TempResult &oAbstractRunner::getTempResult() { return tmpResult; }
void oAbstractRunner::setTempResultZero(const TempResult &tr) {}
void oAbstractRunner::updateComputedResultFromTemp() {}
void oAbstractRunner::setInputTime(const wstring &time) {}
wstring oAbstractRunner::getInputTimeS() const { return L""; }
void oAbstractRunner::setInputStatus(RunnerStatus s) {}
wstring oAbstractRunner::getInputStatusS() const { return L""; }
void oAbstractRunner::setInputPoints(int p) {}
void oAbstractRunner::setInputPlace(int p) {}
const wstring& oAbstractRunner::getStatusS(bool b1, bool b2) const { static wstring s; return s; }
wstring oAbstractRunner::getIOFStatusS() const { return L""; }
wstring oAbstractRunner::getIOFTotalStatusS() const { return L""; }
void oAbstractRunner::setSpeakerPriority(int pri) {}
int oAbstractRunner::getTimeAdjustment(bool b) const { return 0; }
int oAbstractRunner::getPointAdjustment() const { return 0; }
void oAbstractRunner::setTimeAdjustment(int adjust) {}
void oAbstractRunner::setPointAdjustment(int adjust) {}
void oAbstractRunner::setFinishTimeS(const wstring &t) {}
void oAbstractRunner::setStartTimeS(const wstring &t) {}
bool oAbstractRunner::setStartTime(int t, bool b1, ChangeType ct, bool b2) { return false; }
const wstring& oAbstractRunner::getClass(bool b) const { static wstring s; return s; }
const wstring& oAbstractRunner::getBib() const { static wstring s; return s; }
int oAbstractRunner::getEncodedBib() const { return 0; }
void oAbstractRunner::addToInputResult(int i, const oAbstractRunner *s) {}
int oAbstractRunner::getDefaultFee() const { return 0; }
int oAbstractRunner::getEntryFee() const { return 0; }
bool oAbstractRunner::hasLateEntryFee() const { return false; }
void oAbstractRunner::resetInputData() {}
void oAbstractRunner::addClassDefaultFee(bool b) {}

// oRunner stubs
oRunner::oRunner(oEvent *oe) : oAbstractRunner(oe, false) {}
oRunner::~oRunner() {}
void oRunner::changedObject() {}
oDataContainer &oRunner::getDataBuffers(pvoid &data, pvoid &olddata, pvectorstr &strData) const { static oDataContainer dc(256); return dc; }
void oRunner::merge(const oBase &input, const oBase *base) {}
void oRunner::remove() {}
bool oRunner::canRemove() const { return false; }
void oRunner::markClassChanged(int controlId) {}
void oRunner::apply(ChangeType ct, pRunner src) {}
int oRunner::getTimeAfter(int leg, bool allowUpdate) const { return 0; }
void oRunner::fillSpeakerObject(int leg, int previousControlCourseId, const vector<int> &controlIds, bool totalResult, oSpeakerObject &spk) const {}
int oRunner::classInstance() const { return 0; }
void oRunner::setBib(const wstring &bib, int numericalBib, bool updateStartNo) {}
bool oRunner::matchAbstractRunner(const oAbstractRunner *target) const { return false; }
bool oRunner::isResultUpdated(bool totalResult) const { return false; }
oCourse* oRunner::getCourse(bool calculate) const { return nullptr; }
void oRunner::getSplitAnalysis(std::vector<int>& out) const {}
int oRunner::getPunchTime(int controlId, bool adjusted, bool unitAdjustment, bool controlAdjustment) const { return 0; }
void oRunner::changeId(int newId) {}
pair<int, bool> oRunner::inputData(int id, const wstring &input, int inputId, wstring &output, bool noUpdate) { return make_pair(0, false); }
void oRunner::fillInput(int id, vector< pair<wstring, size_t> > &elements, size_t &selected) {}
int oRunner::getBuiltinAdjustment() const { return 0; }
bool oRunner::startTimeAvailable() const { return false; }
int oRunner::getNumShortening() const { return 0; }
wstring oRunner::getEntryDate(bool b) const { return L""; }
int oRunner::getBirthAge() const { return 0; }
void oRunner::setName(const wstring &n, bool manual) {}
const wstring& oRunner::getName() const { static wstring s; return s; }
void oRunner::setFinishTime(int t) {}
void oRunner::setClub(const wstring &club) {}
pClub oRunner::setClubId(int id) { return nullptr; }
void oRunner::setClassId(int id, bool manual) {}
void oRunner::setStartNo(int no, ChangeType ct) {}
int oRunner::getRogainingPoints(bool b1, bool b2) const { return 0; }
int oRunner::getRogainingReduction(bool b) const { return 0; }
int oRunner::getRogainingOvertime(bool b) const { return 0; }
int oRunner::getRogainingPointsGross(bool b) const { return 0; }
int oRunner::getTotalRunningTime() const { return 0; }
const pair<wstring, int> oRunner::getRaceInfo() { return make_pair(L"", 0); }
int oRunner::getPlace(bool b) const { return 0; }
int oRunner::getTotalPlace(bool b) const { return 0; }
RunnerStatus oRunner::getStatusComputed(bool b) const { return StatusOK; }
DynamicRunnerStatus oRunner::getDynamicStatus() const { return DynamicRunnerStatus(); }
int oRunner::getRanking() const { return 0; }
RunnerStatus oRunner::getTotalStatus(bool b) const { return StatusOK; }
int oRunner::getSpeakerPriority() const { return 0; }

// oTeam stubs
oTeam::oTeam(oEvent *oe, int id) : oAbstractRunner(oe, false) {}
oTeam::oTeam(oEvent *oe) : oAbstractRunner(oe, false) {}
oTeam::~oTeam() {}
void oTeam::changedObject() {}
oDataContainer &oTeam::getDataBuffers(pvoid &data, pvoid &olddata, pvectorstr &strData) const { static oDataContainer dc(256); return dc; }
void oTeam::merge(const oBase &input, const oBase *base) {}
void oTeam::remove() {}
bool oTeam::canRemove() const { return false; }
void oTeam::markClassChanged(int controlId) {}
void oTeam::apply(ChangeType ct, pRunner source) {}
int oTeam::getTimeAfter(int leg, bool allowUpdate) const { return 0; }
void oTeam::fillSpeakerObject(int leg, int previousControlCourseId, const vector<int> &courseControlIds, bool totalResult, oSpeakerObject &spk) const { }
void oTeam::setBib(const wstring &bib, int numericalBib, bool updateStartNo) {}
bool oTeam::matchAbstractRunner(const oAbstractRunner *target) const { return false; }
bool oTeam::isResultUpdated(bool totalResult) const { return false; }
int oTeam::getRunningTime(bool computedTime) const { return 0; }
int oTeam::getRogainingPoints(bool computed, bool multidayTotal) const { return 0; }
int oTeam::getRogainingReduction(bool computed) const { return 0; }
int oTeam::getRogainingOvertime(bool computed) const { return 0; }
int oTeam::getRogainingPointsGross(bool computed) const { return 0; }
RunnerStatus oTeam::getStatusComputed(bool allowUpdate) const { return StatusUnknown; }
DynamicRunnerStatus oTeam::getDynamicStatus() const { return DynamicRunnerStatus(); }
const pair<wstring, int> oTeam::getRaceInfo() { return make_pair(L"", 0); }
int oTeam::getNumShortening() const { return 0; }
wstring oTeam::getEntryDate(bool b) const { return L""; }
void oTeam::setClub(const wstring &n) {}
pClub oTeam::setClubId(int id) { return nullptr; }
void oTeam::changeId(int newId) {}
pair<int, bool> oTeam::inputData(int id, const wstring &input, int inputId, wstring &output, bool noUpdate) { return make_pair(0, false); }
void oTeam::fillInput(int id, vector< pair<wstring, size_t> > &out, size_t &selected) {}
void oTeam::prepareRemove() {}
void oTeam::evaluate(ChangeType changeType) {}
void oTeam::adjustMultiRunners() {}
void oTeam::applyBibs() {}
void oTeam::quickApply() {}
void oTeam::fillInSortData(SortOrder so, int leg, bool linearLeg, map<int, int> &classId2Linear, bool &hasRunner) const {}
void oTeam::addTableRow(Table &table) const {}
int oTeam::getLegPlace(int leg, bool multidayTotal, bool allowUpdate) const { return 0; }
int oTeam::getRanking() const { return 0; }

// oFreePunch stubs
oFreePunch::oFreePunch(oEvent *oe, int card, int time, int type, int unit) : oPunch(oe) {}
oFreePunch::oFreePunch(oEvent *oe, int id) : oPunch(oe) {}
oFreePunch::~oFreePunch() {}
void oFreePunch::changedObject() {}
void oFreePunch::merge(const oBase &input, const oBase *base) {}
void oFreePunch::remove() {}
bool oFreePunch::canRemove() const { return false; }
void oFreePunch::rehashPunches(oEvent& oe, int id, oFreePunch* punch) {}
void oFreePunch::fillInput(int id, vector< pair<wstring, size_t> > &out, size_t &selected) {}
pair<int, bool> oFreePunch::inputData(int id, const wstring &input, int inputId, wstring &output, bool noUpdate) { return make_pair(0, false); }
void oFreePunch::setTimeInt(int newTime, bool databaseUpdate) {}
int oFreePunch::getControlIdFromHash(int hash, bool courseControlId) { return 0; }

// oTimeLine stub
oTimeLine::oTimeLine(int time, TimeLineType type, Priority priority, int classId, int id, oRunner *source) : time(time), type(type), priority(priority), classId(classId), ID(id) {}
oTimeLine::~oTimeLine() {}

// MeOSFeatures stubs
MeOSFeatures::MeOSFeatures() {}
MeOSFeatures::~MeOSFeatures() {}
static MeOSFeatures g_features;
const MeOSFeatures& getMeOSFeatures() { return g_features; }
bool MeOSFeatures::hasFeature(MeOSFeatures::Feature f) const { return false; }

// oEvent stubs for domain foundation
oEvent::oEvent(gdioutput &gdi) : oBase(nullptr), gdibase(gdi) {
    oEventData = new oDataContainer(256);
    oControlData = new oDataContainer(256);
    oCourseData = new oDataContainer(256);
    oClassData = new oDataContainer(256);
    oClubData = new oDataContainer(256);
    oRunnerData = new oDataContainer(256);
    oTeamData = new oDataContainer(256);
}

oEvent::~oEvent() {
    delete oEventData;
    delete oControlData;
    delete oCourseData;
    delete oClassData;
    delete oClubData;
    delete oRunnerData;
    delete oTeamData;
}

// oBase virtual methods for oEvent
void oEvent::changedObject() {}
oDataContainer &oEvent::getDataBuffers(pvoid &data, pvoid &olddata, pvectorstr &strData) const {
    data = nullptr; olddata = nullptr; strData = nullptr; return *oEventData;
}
void oEvent::merge(const oBase &input, const oBase *base) {}
void oEvent::remove() {}
bool oEvent::canRemove() const { return false; }

bool oEvent::msSynchronize(oBase* ob) { return false; }
void oEvent::updateFreeId(oBase* ob) {}
wstring oEvent::formatCurrency(int c, bool includeSymbol) const { return L""; }
int oEvent::interpretCurrency(const wstring& c) const { return 0; }
const wstring& oEvent::formatScore(int score) const { 
    static wstring score_str; score_str = itow(score); return score_str; 
}
int oEvent::convertScore(const wstring& s) const { return (int)convertAbsoluteTimeMS(s); }
void oEvent::setTable(const string& name, const shared_ptr<Table>& table) {}
const shared_ptr<Table>& oEvent::getTable(const string& name) const { static shared_ptr<Table> empty; return empty; }
void oEvent::removeControl(int id) {}
bool oEvent::isControlUsed(int id) const { return false; }
bool oEvent::synchronizeList(std::initializer_list<oListId> ids) { return false; }
bool oEvent::synchronizeList(oListId id, bool b1, bool b2) { return false; }
pControl oEvent::addControl(int id, int number, const wstring& name) { return nullptr; }
const wstring& oEvent::getAbsTime(uint32_t time, SubSecond mode) const { static wstring empty; return empty; }
int oEvent::getRelativeTime(const wstring& s) const { return 0; }

// gdioutput stubs for domain foundation 
// (Some are in util/gdioutput_stub.cpp, these are missing)
BaseInfo* gdioutput::setText(const char* id, const wstring& text, bool update, int requireExtraMatch, bool updateOriginal) { return nullptr; }
bool gdioutput::hasWidget(const string& id) const { return false; }
pair<int, bool> gdioutput::getSelectedItem(const string& id) { return make_pair(-1, false); }

ListBoxInfo& gdioutput::addSelection(const string& id, int width, int height, GUICALLBACK cb, const wstring& explanation, const wstring& tooltip) { 
    static ListBoxInfo lbi; return lbi; 
}
bool gdioutput::setItems(const string& id, const vector< pair<wstring, size_t> >& items) { return false; }
InputInfo& gdioutput::addInput(const string& id, const wstring& text, int length, GUICALLBACK cb, const wstring& explanation, const wstring& tooltip) {
    static InputInfo ii; return ii;
}
gdioutput::AskAnswer gdioutput::askOkCancel(const wstring& s) { return gdioutput::AskAnswer::AnswerCancel; }
bool gdioutput::selectItemByData(const char* id, int data) { return false; }
int gdioutput::getTextNo(const char* id, bool acceptMissing) const { return 0; }
const wstring& gdioutput::getText(const char* id, bool acceptMissing, int requireExtraMatch) const { static wstring empty; return empty; }

// oCourse stubs
int oCourse::getStartPunchType() const { return oPunch::PunchStart; }
int oCourse::getFinishPunchType() const { return oPunch::PunchFinish; }
bool oCourse::hasControl(const oControl* ctrl) const { return false; }
pControl oCourse::getControl(int id) const { return nullptr; }
int oCourse::getCommonControl() const { return 0; }

// oClass stubs
bool oClass::hasAnyCourse(const std::set<int>& courseIds) const { return false; }
int oClass::getAccLegControlLeader(int leg, int controlId) const { return 0; }
int oClass::getAccLegControlPlace(int leg, int controlId, int time) const { return 0; }

// Table stubs for domain foundation
void Table::addDataDefiner(const string& key, const oDataDefiner* definer) {}
TableColSpec Table::addColumn(const string& Title, int width, bool isnum, bool formatRight) { return TableColSpec(); }
int Table::addColumnPaddedSort(const string& title, int width, int padding, bool formatRight) { return 0; }
void Table::set(int column, oBase& owner, int id, const wstring& data, bool canEdit, CellType type) {}
void Table::addRow(int row, oBase* ob) {}
Table::Table(oEvent* oe, int width, const wstring& title, const string& id) {}
Table::~Table() {}

// InputInfo stub
#include "gdistructures.h"
InputInfo::InputInfo() : hWnd(0), callBack(nullptr), updateLastData(nullptr), xp(0), yp(0), width(0), height(0), bgColor(GDICOLOR(0)), fgColor(GDICOLOR(0)), isEditControl(false), writeLock(false), ignoreCheck(false) {}
