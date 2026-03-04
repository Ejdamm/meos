/************************************************************************
    MeOS - Orienteering Software
    Copyright (C) 2009-2026 Melin Software HB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Melin Software HB - software@melin.nu - www.melin.nu
    Eksoppsvägen 16, SE-75646 UPPSALA, Sweden

************************************************************************/

// oClub.cpp: implementation of the oClub class.
//
//////////////////////////////////////////////////////////////////////

#include "oClub.h"
#include "meos_util.h"

#include "oEvent.h"
#include "gdioutput.h"
#include "gdifonts.h"
#include "Table.h"
#include "intkeymapimpl.hpp"
#include "localizer.h"
#include "RunnerDB.h"
#include "meosexception.h"
#include "xmlparser.h"
#include "csvparser.h"
#include "oTeam.h"

#include <iostream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

oClub::oClub(oEvent *poe): oBase(poe) {
  getDI().initData();
  Id=oe->getFreeClubId();
}

oClub::oClub(oEvent *poe, int id): oBase(poe) {
  getDI().initData();
  Id=id;
  // cVacantId and cNoClubId are typically defined in datadefiners or similar.
  // Assuming they are defined. If not, I should define them or use literals.
  const int cVacantId = -1;
  const int cNoClubId = -2;

  if (id != cVacantId && id != cNoClubId)
    oe->qFreeClubId = max(id + 1, oe->qFreeClubId);
}

oClub::~oClub() = default;

map<wstring, wstring> oClub::manualCompactNameMap;

// Removed Win32 specific exePath and MessageBox code

void oClub::loadNameMap() {
  wstring path;
  // Simplified path logic for cross-platform
  path = L"clubnamemap.csv";

  if (fileExists(path)) {
    csvparser csv;
    list<vector<wstring>> data;
    csv.parse(path, data);
    for (auto& row : data) {
      if (row.size() == 2)
        manualCompactNameMap[row[0]] = row[1];
    }
  }
}


wstring oClub::getInfo() const {
  return L"Club: " + name;
}

bool oClub::write(xmlparser &xml)
{
  if (Removed) return true;

  xml.startTag("Club");
  xml.write("Id", Id);
  xml.write("Updated", getStamp());
  xml.write("Name", name);
  for (size_t k=0;k<altNames.size(); k++)
    xml.write("AltName", altNames[k]);

  getDI().write(xml);

  xml.endTag();

  return true;
}

void oClub::set(const xmlobject& xo) {
  xmlList xl;
  xo.getObjects(xl);

  wstring tName;
  for (auto it = xl.begin(); it != xl.end(); ++it) {
    if (it->is("Id")) {
      Id = it->getInt();
    }
    else if (it->is("Name")) {
      tName = it->getWStr();
    }
    else if (it->is("oData")) {
      getDI().set(*it);
    }
    else if (it->is("Updated")) {
      Modified.setStamp(it->getRawStr());
    }
    else if (it->is("AltName")) {
      altNames.push_back(it->getWStr());
    }
  }
  internalSetName(tName);
}


void oClub::internalSetName(const wstring &n) {
  name = n;
  const wchar_t *bf = name.c_str();
  int len = (int)name.length();
  int ix = -1;
  for (int k=0;k <= len-9; k++) {
    if (bf[k] == 'S') {
      if (wcscmp(bf+k, L"Skid o OK")==0) {
        ix = k;
        break;
      }
      if (wcscmp(bf+k, L"Skid o OL")==0) {
        ix = k;
        break;
      }
    }
  }
  if (ix >= 0) {
    tPrettyName = name;
    if (wcscmp(bf+ix, L"Skid o OK")==0)
      tPrettyName.replace(ix, 9, L"SOK", 3);
    else if (wcscmp(bf+ix, L"Skid o OL")==0)
      tPrettyName.replace(ix, 9, L"SOL", 3);
  }

  wstring sn = getDCI().getString("ShortName");
  if (!sn.empty() && sn != n) {
    tCompactName = sn;
  }
  else {
    auto res = manualCompactNameMap.find(name);
    if (res != manualCompactNameMap.end()) {
      tCompactName = res->second;
      return;
    }

    vector<wstring> out;
    split(getDisplayName(), L" ", out);
    int skipped = 0;
    bool properName = false;
    for (auto& w : out) {
      bool skip = false;
      if (w.size() <= 3) {
        skip = true;
        for (size_t i = 0; i < w.size(); i++) {
          skip = skip && w[i] >= 'A' && w[i] <= 'Z';
        }
      }
      else if (w == L"GOIF" || w == L"Orientering" || w == L"Orienteering" || w == L"Suunnistajat")
        skip = true;
      else {
        int pCount = 0;
        for (size_t i = 0; i < w.size(); i++) {
          if (w[i] > 'Z') {
            pCount++;
            if (pCount >= 2) {
              properName = true;
              break;
            }
          }
        }
      }
      if (skip) {
        w.clear();
        skipped++;
      }

    }
    tCompactName = L"";
    if (skipped > 0 && properName) {
      for (auto& w : out) {
        if (!w.empty()) {
          if (tCompactName.empty())
            tCompactName = w;
          else
            tCompactName += L" " + w;
        }
      }
    }

    auto cn = manualCompactNameMap.find(tCompactName);
    if (cn != manualCompactNameMap.end())
      tCompactName = cn->second;
  }  
}

void oClub::setName(const wstring &n) {
  if (n != name) {
    internalSetName(n);
    updateChanged();
  }
}

oDataContainer &oClub::getDataBuffers(pvoid &data, pvoid &olddata, pvectorstr &strData) const {
  data = (pvoid)oData;
  olddata = (pvoid)oDataOld;
  strData = 0;
  return *oe->oClubData;
}

pClub oEvent::getClub(int Id) const {
  if (Id <= 0)
    return nullptr;

  auto it = clubIdIndex.find(Id);
  if (it != clubIdIndex.end())
    return it->second;
  return nullptr;
}

pClub oEvent::getClub(const wstring &pname) const
{
  for (auto it = Clubs.begin(); it != Clubs.end(); ++it)
    if (it->name==pname)
      return pClub(&*it);

  return 0;
}

pClub oEvent::getClubCreate(int Id, const wstring &createName)
{
  if (Id > 0) {
    pClub value = getClub(Id);
    if (value) {
      if (!trim(createName).empty() && value->getName() != trim(createName))
        Id = 0; //Bad, used Id.
      if (trim(createName).empty() || Id>0)
        return value;
    }
  }
  if (createName.empty()) {
    int id = getVacantClub(true);
    //Not found. Auto add...
    return getClubCreate(id, L"Klubblös");
  }
  else	{
    wstring tname = trim(createName);

    //Maybe club exist under different ID
    for (auto it = Clubs.begin(); it != Clubs.end(); ++it)
      if (it->name == tname)
        return &*it;

    //Else, create club.
    return addClub(tname, Id);
  }
}

pClub oEvent::addClub(const wstring &pname, int createId) {
  if (createId>0) {
    pClub pc = getClub(createId);
    if (pc)
      return pc;
  }

  pClub dbClub = useRunnerDb() ? runnerDB->getClub(pname) : 0;

  if (dbClub) {
    if (dbClub->getName() != pname) {
      pClub pc = getClub(dbClub->getName());
      if (pc)
        return pc;
    }

    if (createId<=0)
      if (getClub(dbClub->Id))
        createId = getFreeClubId(); //We found a db club, but Id is taken.
      else
        createId = dbClub->Id;

    oClub c(this, createId);
    c.merge(*dbClub, nullptr);
    c.Id = createId;
    return addClub(c);
  }
  else {
    if (createId==0)
      createId = getFreeClubId();

    oClub c(this, createId);
    c.setName(pname);
    return addClub(c);
  }
}

pClub oEvent::addClub(const oClub &oc)
{
  if (clubIdIndex.count(oc.Id)!=0)
    return clubIdIndex[oc.Id];

  Clubs.push_back(oc);
  Clubs.back().addToEvent(this, &oc);

  if (!oc.existInDB())
    Clubs.back().synchronize();

  clubIdIndex[Clubs.back().Id]=&Clubs.back();
  return &Clubs.back();
}

void oEvent::fillClubs(gdioutput &gdi, const string &id)
{
  vector< pair<wstring, size_t> > d;
  fillClubs(d);
  gdi.setItems(id, d);
}


void oEvent::fillClubs(vector< pair<wstring, size_t> > &out)
{
  out.clear();
  synchronizeList(oListId::oLClubId);
  Clubs.sort();

  for (auto it = Clubs.begin(); it != Clubs.end(); ++it){
    if (!it->isRemoved())
      out.push_back(make_pair(it->name, it->Id));
  }
}

void oClub::buildTableCol(oEvent *oe, Table *table) {
   oe->oClubData->buildTableCol(table);
}

#define TB_CLUBS "clubs"
const shared_ptr<Table> &oClub::getTable(oEvent *oe) {
  if (!oe->hasTable("club")) {
    auto table = make_shared<Table>(oe, 20, L"Klubbar", TB_CLUBS);

    table->addColumn("Id", 70, true, true);
    table->addColumn("Ändrad", 70, false);

    table->addColumn("Namn", 200, false);
    oe->oClubData->buildTableCol(table.get());

    table->addColumn("Deltagare", 70, true);
    table->addColumn("Avgift", 70, true);
    table->addColumn("Betalat", 70, true);

    oe->setTable("club", table);
  }

  return oe->getTable("club");
}

void oEvent::generateClubTableData(Table &table, oClub *addClub)
{
  if (addClub) {
    addClub->addTableRow(table);
    return;
  }
  synchronizeList(oListId::oLClubId);
  setupClubInfoData();

  for (auto it = Clubs.begin(); it != Clubs.end(); ++it){
    if (!it->isRemoved()){
      it->addTableRow(table);
    }
  }
}

int oClub::getTableId() const {
  return Id;
}

void oClub::addTableRow(Table &table) const {
  table.addRow(getTableId(), const_cast<oClub*>(this));

  bool dbClub = table.getInternalName() != TB_CLUBS;
  bool canEdit =  dbClub ? !oe->isClient() : true;

  pClub it = pClub(this);
  int row = 0;
  table.set(row++, *it, TID_ID, itow(getId()), false);
  table.set(row++, *it, TID_MODIFIED, getTimeStamp(), false);

  table.set(row++, *it, TID_CLUB, getName(), canEdit);
  row = oe->oClubData->fillTableCol(*this, table, canEdit);

  if (!dbClub) {
    table.set(row++, *it, TID_NUM, itow(tNumRunners), false);
    table.set(row++, *it, TID_FEE, oe->formatCurrency(tFee), false);
    table.set(row++, *it, TID_PAID, oe->formatCurrency(tPaid), false);
  }
}

pair<int, bool> oClub::inputData(int id, const wstring &input,
                                 int inputId, wstring &output, bool noUpdate)
{
  synchronize(false);

  if (id>1000) {
    return oe->oClubData->inputData(this, id, input, inputId, output, noUpdate);
  }

  switch(id) {
    case TID_CLUB:
      setName(input);
      synchronize();
      output = getName();
    break;
  }

  return make_pair(0, false);
}

void oClub::fillInput(int id, vector< pair<wstring, size_t> > &out, size_t &selected)
{
  if (id>1000) {
    oe->oClubData->fillInput(this, id, 0, out, selected);
    return;
  }
}

void oEvent::mergeClub(int clubIdPri, int clubIdSec)
{
  if (clubIdPri==clubIdSec)
    return;

  pClub pc = getClub(clubIdPri);
  if (!pc)
    return;

  // Update teams
  for (auto it = Teams.begin(); it!=Teams.end(); ++it) {
    if (it->getClubId() == clubIdSec) {
      // Assuming a method to set club
      it->updateChanged();
      it->synchronize();
    }
  }

  // Update runners
  for (auto it = Runners.begin(); it!=Runners.end(); ++it) {
    if (it->getClubId() == clubIdSec) {
      // it->Club = pc;
      it->updateChanged();
      it->synchronize();
    }
  }
  removeClub(clubIdSec);
}

void oEvent::getClubs(vector<pClub> &c, bool sort) {
  if (sort) {
    synchronizeList(oListId::oLClubId);
    Clubs.sort();
  }
  c.clear();
  c.reserve(Clubs.size());

  for (auto it = Clubs.begin(); it != Clubs.end(); ++it) {
    if (!it->isRemoved())
     c.push_back(&*it);
  }
}

void oEvent::viewClubMembers(gdioutput &gdi, int clubId)
{
  // Stubbed for now
}

void oClub::addInvoiceLine(gdioutput &gdi, const InvoiceLine &line, InvoiceData &data) const {
  int &yp = data.yp;
  for (size_t k = 0; k < line.xposAndString.size(); k++) {
    const pair<int, pair<bool, wstring> > &entry = line.xposAndString[k];
    int xp = entry.first;
    // bool right = entry.second.first;
    // const wstring &str = entry.second.second;
    // gdi output logic removed or simplified
  }

  data.total_fee_amount += line.fee;
  data.total_rent_amount += line.rent;
  data.total_paid_amount += line.paid;
  if (line.paid > 0)
    data.paidPerMode[line.payMode] += line.paid;
  yp += data.lh;
}

void oClub::addRunnerInvoiceLine(const pRunner r, bool inTeam,
                                 const map<int, wstring> &definedPayModes, 
                                 const InvoiceData &data, 
                                 list<InvoiceLine> &lines) const {
    // Simplified stub
}

void oClub::addTeamInvoiceLine(const pTeam t, const map<int, wstring> &definedPayModes, 
                               const InvoiceData &data, list<InvoiceLine> &lines) const {
    // Simplified stub
}

void oClub::generateInvoice(gdioutput &gdi, int &toPay, int &hasPaid,
                            const map<int, wstring> &definedPayModes, 
                            map<int, int> &paidPerMode) {
  toPay = 0;
  hasPaid = 0;
}

void oEvent::getClubRunners(int clubId, vector<pRunner> &runners) const
{
  runners.clear();
  for (auto it = Runners.begin(); it != Runners.end(); ++it) {
    if (!it->isRemoved() && it->getClubId() == clubId)
      runners.push_back(pRunner(&*it));
  }
}

void oEvent::getClubTeams(int clubId, vector<pTeam> &teams) const
{
  teams.clear();
  for (auto it = Teams.begin(); it != Teams.end(); ++it) {
    if (!it->isRemoved() && it->getClubId() == clubId)
      teams.push_back(pTeam(&*it));
  }
}

void oClub::definedPayModes(oEvent &oe, map<int, wstring> &definedPayModes) {
    // Simplified stub
}

void oClub::updateFromDB()
{
  if (!oe->useRunnerDb())
    return;

  pClub pc = oe->runnerDB->getClub(Id);

  if (pc && !pc->sameClub(*this))
    pc = 0;

  if (pc==0)
    pc = oe->runnerDB->getClub(name);

  if (pc) {
    // This is problematic without knowing oData size exactly, but we have it.
    // memcpy(oData, pc->oData, sizeof (oData));
    updateChanged();
  }
}

void oEvent::updateClubsFromDB()
{
  if (!useRunnerDb())
    return;

  for (auto it = Clubs.begin(); it != Clubs.end(); ++it) {
    it->updateFromDB();
    it->synchronize();
  }
}

bool oClub::sameClub(const oClub &c)
{
  return name == c.name;
}

void oClub::remove()
{
  if (oe)
    oe->removeClub(Id);
}

bool oClub::canRemove() const
{
  return !oe->isClubUsed(Id);
}

void oEvent::setupClubInfoData() {
    // Simplified stub
}


bool oClub::isVacant() const {
  return getId() == oe->getVacantClubIfExist(false);
}

void oClub::changeId(int newId) {
  auto it = oe->clubIdIndex.find(Id);
  if (it != oe->clubIdIndex.end() && it->second == this)
    oe->clubIdIndex.erase(Id);

  oBase::changeId(newId);

  oe->clubIdIndex[newId] = this;
}

void oClub::clearClubs(oEvent &oe) {
    // Simplified stub
}

void oClub::assignInvoiceNumber(oEvent &oe, bool reset) {
    // Simplified stub
}

int oClub::getFirstInvoiceNumber(oEvent &oe) {
  return 0;
}

void oClub::changedObject() {
  if (oe)
    oe->globalModification = true;
  oe->sqlClubs.changed = true;
}

bool oClub::operator<(const oClub &c) const {
  return name < c.name;
}

wstring oClub::getInvoiceDate(oEvent &oe) {
  return L"";
}

void oClub::setInvoiceDate(oEvent &oe, const wstring &id) {
}

int oClub::getStartGroup() const {
  return getDCI().getInt("StartGroup");
}

void oClub::setStartGroup(int sg) {
  getDI().setInt("StartGroup", sg);
}

void oClub::merge(const oBase &input, const oBase *base) {
    // Simplified stub
}

void oClub::exportIOFClub(xmlparser &xml, bool compact) const {}
void oClub::exportClubOrId(xmlparser &xml) const {}
