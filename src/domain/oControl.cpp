#include <algorithm>
#include <limits>
#include <cstring>
#include <strings.h>
#include <cmath>

#include "oControl.h"
#include "oEvent.h"
#include "gdioutput.h"
#include "meos_util.h"
#include <cassert>
#include "localizer.h"
#include "Table.h"
#include "MeOSFeatures.h"
#include <set>
#include <unordered_map>
#include "xmlparser.h"
#include "oDataContainer.h"
#include "oCourse.h"
#include "oClass.h"
#include "oRunner.h"
#include "oCard.h"
#include "oFreePunch.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace std;

oControl::oControl(oEvent* poe) : oBase(poe)
{
  getDI().initData();
  nNumbers = 0;
  Status = ControlStatus::StatusOK;
  tMissedTimeMax = 0;
  tMissedTimeTotal = 0;
  tNumVisitorsActual = 0;
  tNumVisitorsExpected = 0;
  tMissedTimeMedian = 0;
  tMistakeQuotient = 0;
  tNumRunnersRemaining = 0;
  tStatDataRevision = -1;

  tHasFreePunchLabel = false;
  tNumberDuplicates = 0;
}

oControl::oControl(oEvent* poe, int id) : oBase(poe)
{
  Id = id;
  getDI().initData();
  nNumbers = 0;
  Status = ControlStatus::StatusOK;

  tMissedTimeMax = 0;
  tMissedTimeTotal = 0;
  tNumVisitorsActual = 0;
  tNumVisitorsExpected = 0;
  tMistakeQuotient = 0;
  tMissedTimeMedian = 0;
  tNumRunnersRemaining = 0;
  tStatDataRevision = -1;

  tHasFreePunchLabel = false;
  tNumberDuplicates = 0;
}

oControl::~oControl()
{
}

pair<int, int> oControl::getIdIndexFromCourseControlId(int courseControlId) {
  return make_pair(courseControlId % 100000, courseControlId / 100000);
}

int oControl::getCourseControlIdFromIdIndex(int controlId, int index) {
  assert(controlId < 100000);
  return controlId + index * 100000;
}


bool oControl::write(xmlparser &xml)
{
  if (Removed) return true;
  
  xml.startTag("Control");

  xml.write("Id", Id);
  xml.write("Updated", getStamp());
  xml.write("Name", Name);
  xml.write("Numbers", codeNumbers());
  xml.write("Status", int(Status));

  getDI().write(xml);
  xml.endTag();

  return true;
}

void oControl::set(int pId, int pNumber, wstring pName)
{
  Id=pId;
  Numbers[0]=pNumber;
  nNumbers=1;
  Name=pName;

  updateChanged();
}


void oControl::setStatus(ControlStatus st){
  if (st!=Status){
    Status=st;
    updateChanged();
  }
}

void oControl::setName(const wstring &name) {
  if (name == getDefaultName()) {
    if (!Name.empty()) {
      Name = L"";
      updateChanged();
    }
  }
  else if (name != getName()) {
    Name = name;
    updateChanged();
  }
}

void oControl::merge(const oBase &input, const oBase *base) {
  // Stub implementation
}

void oControl::set(const xmlobject* xo) {
  xmlList xl;
  xo->getObjects(xl);
  nNumbers = 0;
  Numbers[0] = 0;

  xmlList::const_iterator it;

  for (it = xl.begin(); it != xl.end(); ++it) {
    if (it->is("Id")) {
      Id = it->getInt();
    }
    else if (it->is("Number")) {
      Numbers[0] = it->getInt();
      nNumbers = 1;
    }
    else if (it->is("Numbers")) {
      decodeNumbers(it->getRawStr());
    }
    else if (it->is("Status")) {
      Status = (ControlStatus)it->getInt();
    }
    else if (it->is("Name")) {
      Name = it->getWStr();
      if (Name.size() > 1 && Name.at(0) == '%') {
        Name = lang.tl(Name.substr(1));
      }
    }
    else if (it->is("Updated")) {
      Modified.setStamp(it->getRawStr());
    }
    else if (it->is("oData")) {
      getDI().set(*it);
    }
  }
}

int oControl::getFirstNumber() const {
  if (nNumbers > 0)
    return Numbers[0];
  else
    return 0;
}

wstring oControl::getString() {
  wstring num;
  if (Status == ControlStatus::StatusMultiple)
    num = codeNumbers('+');
  else if (Status == ControlStatus::StatusRogaining || Status == ControlStatus::StatusRogainingRequired)
    num = codeNumbers('|') + L" (" + (getRogainingPoints() != 0 ? oe->formatScore(getRogainingPoints()) : L"0") + L"p)";
  else
    num = codeNumbers('|');

  if (Status == ControlStatus::StatusBad || Status == ControlStatus::StatusBadNoTiming)
    return L"\u26A0" + num;

  if (Status == ControlStatus::StatusOptional)
    return L"\u2b59" + num;

  return num;
}

wstring oControl::getLongString()
{
  if (Status == ControlStatus::StatusOK || Status == ControlStatus::StatusNoTiming) {
    if (nNumbers == 1)
      return codeNumbers('|');
    else
      return wstring(lang.tl("VALFRI(")) + codeNumbers(',') + L")";
  }
  else if (Status == ControlStatus::StatusMultiple) {
    return wstring(lang.tl("ALLA(")) + codeNumbers(',') + L")";
  }
  else if (Status == ControlStatus::StatusRogaining || Status == ControlStatus::StatusRogainingRequired)
    return wstring(lang.tl("RG(")) + codeNumbers(',') + L"|" + (getRogainingPoints() != 0 ? oe->formatScore(getRogainingPoints()) : L"0") + L"p)";
  else
    return wstring(lang.tl("TRASIG(")) + codeNumbers(',') + L")";
}

bool oControl::hasNumber(int i) {
  for (int n = 0; n < nNumbers; n++)
    if (Numbers[n] == i) {
      // Mark this number as checked
      checkedNumbers[n] = true;
      return true;
    }
  if (nNumbers > 0)
    return false;
  else return true;
}

bool oControl::uncheckNumber(int i)
{
  for(int n=0;n<nNumbers;n++)
    if (Numbers[n]==i) {
      // Mark this number as checked
      checkedNumbers[n]=false;
      return true;
    }
  return false;
}

bool oControl::hasNumberUnchecked(int i)
{
  for(int n=0;n<nNumbers;n++)
    if (Numbers[n]==i && checkedNumbers[n]==0) {
      // Mark this number as checked
      checkedNumbers[n]=true;
      return true;
    }
  if (nNumbers>0)
    return false;
  else return true;
}

int oControl::getNumMulti() {
  if (Status== ControlStatus::StatusMultiple)
    return nNumbers;
  else
    return 1;
}

wstring oControl::codeNumbers(char sep) const
{
  wstring n;
  wchar_t bf[16];

  for(int i=0;i<nNumbers;i++){
    swprintf(bf, 16, L"%d", Numbers[i]);
    n+=bf;
    if (i+1<nNumbers)
      n+=sep;
  }
  return n;
}

bool oControl::decodeNumbers(string s)
{
  const char *str=s.c_str();

  nNumbers=0;

  while(*str){
    int cid=atoi(str);

    while(*str && (*str!=';' && *str!=',' && *str!=' ')) str++;
    while(*str && (*str==';' || *str==',' || *str==' ')) str++;

    if (cid>0 && cid<1024 && nNumbers<32)
      Numbers[nNumbers++]=cid;
  }

  if (nNumbers==0){
    Numbers[0] = Id;
    nNumbers = 1;
    return false;
  }
  else return true;
}

bool oControl::setNumbers(const wstring &numbers)
{
  int nn=nNumbers;
  int bf[32];

  if (unsigned(nNumbers)<32)
    memcpy(bf, Numbers, sizeof(int)*nNumbers);
  string nnumbers(numbers.begin(), numbers.end());
  bool success=decodeNumbers(nnumbers);

  if (!success) {
    memcpy(Numbers, bf, sizeof(int)*nn);
    nNumbers = nn;
  }

  if (nNumbers!=nn || memcmp(bf, Numbers, sizeof(int)*nNumbers)!=0) {
    updateChanged();
    oe->punchIndex.clear();
  }

  return success;
}

const wstring &oControl::getName() const {
  if (!Name.empty())
    return Name;
  else
    return getDefaultName();
}

/// Get name or [id]
const wstring &oControl::getDefaultName() const {
  wchar_t bf[16];
  swprintf(bf, 16, L"[%d]", Id);
  wstring &res = StringCache::getInstance().wget();
  res = bf;
  return res;
}


wstring oControl::getIdS() const
{
	if (!Name.empty())
		return Name;
	else {
		wchar_t bf[16];
		swprintf(bf, 16, L"%d", Id);
		return bf;
	}
}

oDataContainer &oControl::getDataBuffers(void* &data, void* &olddata, vector<vector<wstring>>* &strData) const {
  data = (void*)oData;
  olddata = (void*)oDataOld;
  strData = 0;
  return *oe->oControlData;
}

const vector<pair<wstring, size_t>>& oEvent::fillControls(vector< pair<wstring, size_t>>& out,
  ControlType type) {
  out.clear();
  synchronizeList({ oListId::oLControlId, oListId::oLCardId, oListId::oLPunchId });
  Controls.sort();

  if (type == ControlType::CourseControl) {
    vector<oControl*> dmy;
    getControls(dmy, true); // Update data
  }

  map<pair<SpecialPunch, int>, oControl*> existingTypeUnits;

  wstring b;
  wchar_t bf[256];
  for (auto it = Controls.begin(); it != Controls.end(); ++it) {
    if (!it->Removed) {
      b.clear();

      if (type == ControlType::All) {
        if (it->isUnit()) {
          existingTypeUnits.emplace(make_pair(it->getUnitType(), it->getUnitCode()), &*it);
          continue;
        }
        if (oControl::isSpecialControl(it->Status)) {
          b += it->Name;
        }
        else {
          if (it->Status == oControl::ControlStatus::StatusOK)
            b += L"OK\t";
          else if (it->Status == oControl::ControlStatus::StatusNoTiming)
            b += L"\u231B\t";
          else if (it->Status == oControl::ControlStatus::StatusMultiple)
            b += L"\u25ef\u25ef\t";
          else if (it->Status == oControl::ControlStatus::StatusRogaining)
            b += L"R\u25ef\t";
          else if (it->Status == oControl::ControlStatus::StatusRogainingRequired)
            b += L"R!\u25ef\t";
          else if (it->Status == oControl::ControlStatus::StatusBad)
            b += L"\u26A0\t";
          else if (it->Status == oControl::ControlStatus::StatusBadNoTiming)
            b += L"\u26A0\u231B\t";
          else if (it->Status == oControl::ControlStatus::StatusOptional)
            b += L"\u26aa\u2b59\t";
          else b += L"[ ]\t";

          b += it->codeNumbers(' ');
          if (it->nNumbers == 0 || it->Id != it->Numbers[0]) {
            b += L" (" + itow(it->Id) + L")";
          }

          if (it->Status == oControl::ControlStatus::StatusRogaining || it->Status == oControl::ControlStatus::StatusRogainingRequired)
            b += L"\t(" + (it->getRogainingPoints() != 0 ? formatScore(it->getRogainingPoints()) : L"0") + L"p)";
          else if (it->Name.length() > 0) {
            b += L"\t(" + it->Name + L")";
          }

        }
        out.emplace_back(b, it->Id);
      }
      else if (type == ControlType::RealControl) {
        if (oControl::isSpecialControl(it->Status))
          continue;

        swprintf(bf, 256, lang.tl("Kontroll %s").c_str(), it->codeNumbers(' ').c_str());
        b = bf;

        if (!it->Name.empty())
          b += L" (" + it->Name + L")";

        out.emplace_back(b, it->Id);
      }
      else if (type == ControlType::CourseControl) {
        if (oControl::isSpecialControl(it->Status))
          continue;

        for (int i = 0; i < it->getNumberDuplicates(); i++) {
          swprintf(bf, 256, lang.tl("Kontroll %s").c_str(), it->codeNumbers(' ').c_str());
          b = bf;

          if (it->getNumberDuplicates() > 1)
            b += L"-" + itow(i + 1);

          if (!it->Name.empty())
            b += L" (" + it->Name + L")";

          out.emplace_back(b, oControl::getCourseControlIdFromIdIndex(it->Id, i));
        }
      }
    }
  }
  if (type == ControlType::All) {
    vector<pair<SpecialPunch, int>> typeUnit;
    getExistingUnits(typeUnit);
    for (auto& tu : typeUnit) {
      auto res = existingTypeUnits.find(tu);
      if (res == existingTypeUnits.end()) {
        wstring name;
        if (tu.first == PunchFinish)
          name = lang.tl(L"Målenhet", true) + L" " + itow(tu.second);
        else if (tu.first == PunchStart)
          name = lang.tl(L"Startenhet", true) + L" " + itow(tu.second);
        else if (tu.first == PunchCheck)
          name = lang.tl(L"Checkenhet", true) + L" " + itow(tu.second);
        out.emplace_back(name, int(tu.first) * 1100000 + tu.second);
      }
      else {
        wstring name = res->second->getName() + L"\t" + res->second->getTimeAdjustS();
        out.emplace_back(name, res->second->getId());
      }
    }
  }

  return out;
}

const vector< pair<wstring, size_t> > &oEvent::fillControlTypes(vector< pair<wstring, size_t> > &out)
{
  synchronizeList(oListId::oLControlId);
  out.clear();
  set<int> sicodes;

  for (auto it=Controls.begin(); it != Controls.end(); ++it){
    if (!it->Removed) {
      for (int k=0;k<it->nNumbers;k++)
        sicodes.insert(it->Numbers[k]);
    }
  }

  wchar_t bf[32];
  out.push_back(make_pair(lang.tl("Check"), int(PunchCheck)));
  out.push_back(make_pair(lang.tl("Start"), int(PunchStart)));
  out.push_back(make_pair(lang.tl("Mål"), int(PunchFinish)));

  for (auto sit = sicodes.begin(); sit!=sicodes.end(); ++sit) {
    swprintf(bf, 32, lang.tl("Kontroll %s").c_str(), itow(*sit).c_str());
    out.push_back(make_pair(bf, *sit));
  }
  return out;
}

void oControl::setupCache() const {
  if (tCache.dataRevision != oe->dataRevision) {
    tCache.timeAdjust = getDCI().getInt("TimeAdjust");
    tCache.minTime = getDCI().getInt("MinTime");
    tCache.dataRevision = oe->dataRevision;
  }
}

void oControl::clearCache() {
  tCache.dataRevision = -1;
}

int oControl::getMinTime() const
{
  if (Status == ControlStatus::StatusNoTiming || Status == ControlStatus::StatusBadNoTiming)
    return 0;
  setupCache();
  return tCache.minTime;
}

int oControl::getTimeAdjust() const
{
  setupCache();
  return tCache.timeAdjust;
}

wstring oControl::getTimeAdjustS() const
{
  return formatTimeMS(getTimeAdjust(), false, SubSecond::Auto);
}

wstring oControl::getMinTimeS() const
{
  if (getMinTime()>0)
    return formatTimeMS(getMinTime(), false, SubSecond::Auto);
  else
    return makeDash(L"-");
}

int oControl::getRogainingPoints() const
{
  return getDCI().getInt("Rogaining");
}

wstring oControl::getRogainingPointsS() const {
  return oe->formatScore(getRogainingPoints());
}

bool oControl::setTimeAdjust(int v) {
  if (v == NOTIME)
    v = 0;
  return getDI().setInt("TimeAdjust", v);
}

void oControl::setRadio(bool r)
{
  // 1 means radio, 2 means no radio, 0 means default
  getDI().setInt("Radio", r ? 1 : 2);
}

bool oControl::isValidRadio() const
{
  int flag = getDCI().getInt("Radio");
  if (flag == 0)
    return (tHasFreePunchLabel || hasName()) && getStatus() == oControl::ControlStatus::StatusOK;
  else
    return flag == 1;
}

bool oControl::setTimeAdjust(const wstring &s) {
  return setTimeAdjust(convertAbsoluteTimeMS(s));
}

void oControl::setMinTime(int v)
{
  if (v<0 || v == NOTIME)
    v = 0;
  getDI().setInt("MinTime", v);
}

void oControl::setMinTime(const wstring &s)
{
  setMinTime(convertAbsoluteTimeMS(s));
}

void oControl::setRogainingPoints(int v)
{
  getDI().setInt("Rogaining", v);
}

void oControl::setRogainingPoints(const wstring &s) {
  setRogainingPoints(oe->convertScore(s));
}

void oControl::startCheckControl()
{
  //Mark all numbers as unchecked.
  for (int k=0;k<nNumbers;k++)
    checkedNumbers[k]=false;
}

wstring oControl::getInfo() const
{
  return getName();
}

void oControl::addUncheckedPunches(vector<pair<int, oControl*>> &mp, bool supportRogaining) const
{
  if (controlCompleted(supportRogaining))
    return;

  for (int k=0;k<nNumbers;k++)
    if (!checkedNumbers[k]) {
      mp.emplace_back(Numbers[k], const_cast<oControl*>(this));

      if (Status!= ControlStatus::StatusMultiple)
        return;
    }
}

int oControl::getMissingNumber() const
{
  for (int k=0;k<nNumbers;k++)
    if (!checkedNumbers[k])
      return Numbers[k];

  assert(false);
  return Numbers[0];//This should not happen
}

bool oControl::controlCompleted(bool supportRogaining) const
{
  if (Status== ControlStatus::StatusOK || Status== ControlStatus::StatusNoTiming || ((Status == ControlStatus::StatusRogaining || Status == ControlStatus::StatusRogainingRequired) && !supportRogaining)) {
    //Check if any number is used.
    for (int k=0;k<nNumbers;k++)
      if (checkedNumbers[k])
        return true;

    //Return true only if there is no control
    return nNumbers==0;
  }
  else if (Status== ControlStatus::StatusMultiple) {
    //Check if al numbers are used.
    for (int k=0;k<nNumbers;k++)
      if (!checkedNumbers[k])
        return false;

    return true;
  }
  else return true;
}

int oControl::getMissedTimeTotal() const {
  if (tStatDataRevision != (int)oe->getRevision())
    oe->setupControlStatistics();

  return tMissedTimeTotal;
}

int oControl::getMissedTimeMax() const {
  if (tStatDataRevision != (int)oe->getRevision())
    oe->setupControlStatistics();

  return tMissedTimeMax;
}

int oControl::getMissedTimeMedian() const {
  if (tStatDataRevision != (int)oe->getRevision())
    oe->setupControlStatistics();

  return tMissedTimeMedian;
}

int oControl::getMistakeQuotient() const {
  if (tStatDataRevision != (int)oe->getRevision())
    oe->setupControlStatistics();

  return tMistakeQuotient;
}


int oControl::getNumVisitors(bool actulaVisits) const {
  if (tStatDataRevision != (int)oe->getRevision())
    oe->setupControlStatistics();

  if (actulaVisits)
    return tNumVisitorsActual;
  else
    return tNumVisitorsExpected;
}

int oControl::getNumRunnersRemaining() const {
  if (tStatDataRevision != (int)oe->getRevision())
    oe->setupControlStatistics();

  return tNumRunnersRemaining;
}

void oEvent::setupControlStatistics() {
  // Reset all times
  for (auto &ctrl : Controls) {
    ctrl.tMissedTimeMax = 0;
    ctrl.tMissedTimeTotal = 0;
    ctrl.tNumVisitorsActual = 0;
    ctrl.tNumVisitorsExpected = 0;
    ctrl.tNumRunnersRemaining = 0;
    ctrl.tMissedTimeMedian = 0;
    ctrl.tMistakeQuotient = 0;
    ctrl.tStatDataRevision = dataRevision; // Mark as up-to-date
  }

  map<int, pair<int, vector<int>>> lostPerControl; // First is "actual" misses,
  vector<int> delta;
  for (auto &r : Runners) {
    if (r.isRemoved())
      continue;
    oCourse* pc = r.getCourse(true);
    if (!pc)
      continue;
    r.getSplitAnalysis(delta);

    int nc = pc->nControls();
    if (unsigned(nc) < delta.size()) {
      for (int i = 0; i<nc; i++) {
        oControl* ctrl = pc->getControl(i);
        if (ctrl && delta[i]>0) {
          if (delta[i] < 10 * timeConstMinute)
            ctrl->tMissedTimeTotal += delta[i];
          else
            ctrl->tMissedTimeTotal += 10 * timeConstMinute; // Use max 10 minutes

          ctrl->tMissedTimeMax = max(ctrl->tMissedTimeMax, delta[i]);
        }

        if (delta[i] > 0) {
          lostPerControl[ctrl->getId()].second.push_back(delta[i]);
          ++lostPerControl[ctrl->getId()].first;
        }

        ctrl->tNumVisitorsActual++;
      }
    }

    if (!r.isVacant() && r.getStatus() != 5 && r.getStatus() != 8
                        && r.getStatus() != 9) {
      bool foundRadio = false;
      bool unordered = pc->getCommonControl() != false;
      
      for (int i = nc - 1; i >= 0; i--) {
        oControl* ctrl = pc->getControl(i);
        ctrl->tNumVisitorsExpected++;

        if (r.getStatus() == 0) {
          if (!foundRadio && r.getPunchTime(i, false, false, false) == -1)
            ctrl->tNumRunnersRemaining++;
          else if (!unordered) {
            foundRadio = true;
          }
        }
      }
    }
  }

  for (auto &ctrl : Controls) {
    if (!ctrl.isRemoved()) {
      int id = ctrl.getId();

      auto res = lostPerControl.find(id);
      if (res != lostPerControl.end()) {
        if (!res->second.second.empty()) {
          sort(res->second.second.begin(), res->second.second.end());
          int nMistakes = res->second.second.size();
          int avg;
          if (nMistakes % 2 == 1)
            avg = res->second.second[nMistakes / 2];
          else
            avg = (res->second.second[nMistakes / 2] + res->second.second[nMistakes / 2 -1]) / 2;
          ctrl.tMissedTimeMedian = avg;
        }
        if (ctrl.tNumVisitorsActual > 0)
          ctrl.tMistakeQuotient = (int)round((100.00 * res->second.first) / double(ctrl.tNumVisitorsActual));
        else
          ctrl.tMistakeQuotient = 0;
      }
      else {
        ctrl.tMistakeQuotient = 0;
        ctrl.tMissedTimeMedian = 0;
      }
    }
  }
}

bool oEvent::hasRogaining()
{
  for (auto it=Controls.begin(); it != Controls.end(); ++it) {
    if (!it->Removed && it->getStatus() == oControl::ControlStatus::StatusRogaining)
      return true;
  }
  return false;
}

const wstring oControl::getStatusS() const {
  //enum ControlStatus {StatusOK=0, StatusBad=1, StatusMultiple=2,
  //                    StatusStart = 4, StatusFinish = 5, StatusRogaining = 6};

  switch (getStatus()) {
    case ControlStatus::StatusOK:
      return lang.tl("OK");
    case ControlStatus::StatusBad:
      return lang.tl("Trasig");
    case ControlStatus::StatusOptional:
      return lang.tl("Valfri");
    case ControlStatus::StatusMultiple:
      return lang.tl("Multipel");
    case ControlStatus::StatusRogaining:
      return lang.tl("Rogaining");
    case ControlStatus::StatusRogainingRequired:
      return lang.tl("Rogaining Obligatorisk");
    case ControlStatus::StatusStart:
      return lang.tl("Start");
    case ControlStatus::StatusCheck:
      return lang.tl("Check");
    case ControlStatus::StatusFinish:
      return lang.tl("Mål");
    case ControlStatus::StatusNoTiming:
      return lang.tl("Utan tidtagning");
    case ControlStatus::StatusBadNoTiming:
      return lang.tl("Försvunnen");
    default:
      return lang.tl("Okänd");
  }
}

void oEvent::fillControlStatus(gdioutput &gdi, const string& id)
{
  vector< pair<wstring, size_t> > d;
  fillControlStatus(d);
  gdi.setItems(id, d);
}


const vector< pair<wstring, size_t> > &oEvent::fillControlStatus(vector< pair<wstring, size_t> > &out)
{
  out.clear();
  out.emplace_back(lang.tl(L"OK"), size_t(oControl::ControlStatus::StatusOK));
  out.emplace_back(lang.tl(L"Multipel"), size_t(oControl::ControlStatus::StatusMultiple));

  if (getMeOSFeatures().hasFeature(MeOSFeatures::Rogaining)) {
    out.emplace_back(lang.tl(L"Rogaining"), size_t(oControl::ControlStatus::StatusRogaining));
    out.emplace_back(lang.tl(L"Rogaining Obligatorisk"), size_t(oControl::ControlStatus::StatusRogainingRequired));
  }
  out.emplace_back(lang.tl(L"Utan tidtagning"), size_t(oControl::ControlStatus::StatusNoTiming));
  out.emplace_back(lang.tl(L"Trasig"), size_t(oControl::ControlStatus::StatusBad));
  out.emplace_back(lang.tl(L"Försvunnen"), size_t(oControl::ControlStatus::StatusBadNoTiming));
  out.emplace_back(lang.tl(L"Valfri"), size_t(oControl::ControlStatus::StatusOptional));

  return out;
}


const shared_ptr<Table> &oControl::getTable(oEvent *oe) {
  if (!oe->hasTable("control")) {
    auto table = make_shared<Table>(oe, 20, L"Kontroller", "controls");

    table->addColumn("Id", 70, true, true);
    table->addColumn("Ändrad", 70, false);

    table->addColumn("Namn", 150, false);
    table->addColumn("Status", 70, false);
    table->addColumn("Stämpelkoder", 100, true);
    table->addColumn("Antal löpare", 70, true, true);
    table->addColumn("Kvar-i-skogen", 70, true, true);

    table->addColumn("Bomtid (max)", 70, true, true);
    table->addColumn("Bomtid (medel)", 70, true, true);
    table->addColumn("Bomtid (median)", 70, true, true);

    oe->oControlData->buildTableCol(table.get());
    oe->setTable("control", table);

    table->setTableProp(Table::CAN_DELETE | Table::CAN_PASTE);
  }

  return oe->getTable("control");
}

void oEvent::generateControlTableData(Table &table, oControl *addControl)
{
  if (addControl) {
    addControl->addTableRow(table);
    return;
  }

  synchronizeList(oListId::oLControlId);
  for (auto it=Controls.begin(); it != Controls.end(); ++it){
    if (!it->Removed){
      it->addTableRow(table);
    }
  }
}

void oControl::addTableRow(Table &table) const {
  oControl &it = *const_cast<oControl*>(this);
  table.addRow(getId(), &it);

  int row = 0;
  table.set(row++, it, TID_ID, itow(getId()), false);
  table.set(row++, it, TID_MODIFIED, getTimeStamp(), false);

  table.set(row++, it, TID_CONTROL, getName(), true);
  bool canEdit = !isSpecialControl(getStatus());
  table.set(row++, it, TID_STATUS, getStatusS(), canEdit, cellSelection);
  table.set(row++, it, TID_CODES, codeNumbers(), true);

  table.set(row++, it, 50, itow(getNumVisitors(false)), false);
  table.set(row++, it, 50, itow(getNumRunnersRemaining()), false);

  int nv = getNumVisitors(true);
  table.set(row++, it, 51, nv > 0 ? formatTime(getMissedTimeMax(), SubSecond::Off) : L"-", false);
  table.set(row++, it, 52, nv > 0 ? formatTime(getMissedTimeTotal()/nv, SubSecond::Off) : L"-", false);
  table.set(row++, it, 53, nv > 0 ? formatTime(getMissedTimeMedian(),  SubSecond::Off) : L"-", false);

  oe->oControlData->fillTableCol(it, table, true);
}

pair<int, bool> oControl::inputData(int id, const wstring &input,
                       int inputId, wstring &output, bool noUpdate)
{
  synchronize(false);

  if (id>1000) {
    return oe->oControlData->inputData(this, id, input, inputId, output, noUpdate);
  }
  switch(id) {
    case TID_CONTROL:
      setName(input);
      synchronize();
      output=getName();
      break;
    case TID_STATUS:
      setStatus(ControlStatus(inputId));
      synchronize(true);
      output = getStatusS();
      break;
    case TID_CODES:
      setNumbers(input);
      synchronize(true);
      output = codeNumbers();
      break;
  }

  return make_pair(0, false);
}

void oControl::fillInput(int id, vector< pair<wstring, size_t> > &out, size_t &selected)
{
  if (id>1000) {
    oe->oControlData->fillInput(this, id, nullptr, out, selected);
    return;
  }

  if (id==TID_STATUS) {
    oe->fillControlStatus(out);
    selected = size_t(getStatus());
  }
}

void oControl::remove()
{
  if (oe)
    oe->removeControl(Id);
}

bool oControl::canRemove() const
{
  return !oe->isControlUsed(Id);
}

void oEvent::getControls(vector<oControl*> &c, bool calculateCourseControls) {
  c.clear();

  if (calculateCourseControls) {
    unordered_map<int, oControl*> cById;
    for (auto it = Controls.begin(); it != Controls.end(); ++it) {
      if (it->Removed)
        continue;
      it->tNumberDuplicates = 0;
      cById[it->Id] = &*it;
    }
    for (auto it = Courses.begin(); it != Courses.end(); ++it) {
      map<int, int> count;
      for (int i = 0; i < it->nControls(); i++) {
        oControl* ctrl = it->getControl(i);
        if (ctrl)
          ++count[ctrl->getId()];
      }
      for (auto it2 = count.begin(); it2 != count.end(); ++it2) {
        auto res = cById.find(it2->first);
        if (res != cById.end()) {
          res->second->tNumberDuplicates = max(res->second->tNumberDuplicates, it2->second);
        }
      }
    }
  }

  for (auto it = Controls.begin(); it != Controls.end(); ++it) {
    if (it->Removed)
      continue;
    c.push_back(&*it);
  }
}

void oControl::getNumbers(vector<int> &numbers) const {
  numbers.resize(nNumbers);
  for (int i = 0; i < nNumbers; i++) {
    numbers[i] = Numbers[i];
  }
}

void oControl::changedObject() {
  if (oe)
    oe->globalModification = true;

  oe->sqlControls.changed = true;
}

int oControl::getNumberDuplicates() const {
  return tNumberDuplicates;
}

void oControl::getCourseControls(vector<int> &cc) const {
  cc.resize(tNumberDuplicates);
  for (int i = 0; i < tNumberDuplicates; i++) {
    cc[i] = getCourseControlIdFromIdIndex(Id, i);
  }
}

void oControl::getCourses(vector<oCourse*> &crs) const {
  crs.clear();
  for (auto it = oe->Courses.begin(); it != oe->Courses.end(); it++) {
    if (it->isRemoved())
      continue;

    if (it->hasControl(this))
      crs.push_back(&*it);
  }
}

void oControl::getClasses(vector<oClass*> &cls) const {
  vector<oCourse*> crs;
  getCourses(crs);
  std::set<int> cid;
  for (size_t k = 0; k< crs.size(); k++) {
    cid.insert(crs[k]->getId());
  }

  for (auto it = oe->Classes.begin(); it != oe->Classes.end(); it++) {
    if (it->isRemoved())
      continue;

    if (it->hasAnyCourse(cid))
      cls.push_back(&*it);
  }
}

int oControl::getControlIdByName(const oEvent &oe, const string &name) {
  if (wcscasecmp(oe.gdiBase().widen(name).c_str(), L"finish") == 0)
    return int(PunchFinish);
  if (wcscasecmp(oe.gdiBase().widen(name).c_str(), L"start") == 0)
    return int(PunchStart);

  vector<oControl*> ac;
  const_cast<oEvent&>(oe).getControls(ac, true);
  wstring wname = oe.gdiBase().recodeToWide(name);
  for (oControl* c : ac) {
    if (wcscasecmp(c->getName().c_str(), wname.c_str()) == 0)
      return c->getId();
  }

  return 0;
}

bool oControl::isUnit() const {
  if (isSpecialControl(getStatus())) {
    return getUnitCode() > 0;
  }
  return false;
}

int oControl::getUnitCode() const {
  return getDCI().getInt("Unit");
}

SpecialPunch oControl::getUnitType() const {
  switch (getStatus()) {
    case ControlStatus::StatusFinish:
      return PunchFinish;
    case ControlStatus::StatusStart:
      return PunchStart;
    case ControlStatus::StatusCheck:
      return PunchCheck;
    default:
        break;
  }
  throw std::runtime_error("Invalid unit type");
}

void oEvent::clearUnitAdjustmentCache() {
}

oControl* oEvent::getControl(int Id) const {
    for (auto it = Controls.begin(); it != Controls.end(); ++it) {
        if (it->getId() == Id && !it->isRemoved())
            return const_cast<oControl*>(&*it);
    }
    return nullptr;
}

oControl* oEvent::getControlByType(int type) {
  for (auto& c : Controls) {
    if (!c.isRemoved() && c.getFirstNumber() == type)
      return &c;
  }
  return nullptr;
}

oControl* oEvent::getControl(int Id, bool create, bool includeVirtual) {
  for (auto it = Controls.begin(); it != Controls.end(); ++it) {
    if (it->getId() == Id && !it->isRemoved())
      return &*it;
  }

  if (!create || Id <= 0)
    return nullptr;

  //Not found. Auto add...
  return addControl(Id, Id, L"");
}

void oEvent::getExistingUnits(vector<pair<SpecialPunch, int>>& typeUnit) {
  oFreePunch::rehashPunches(*this, 0, nullptr);
}
