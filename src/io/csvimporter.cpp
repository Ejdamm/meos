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

#include "StdAfx.h"
#include "csvimporter.h"
#include "oEvent.h"
#include "SportIdent.h"
#include "meos_util.h"
#include "localizer.h"
#include "meosexception.h"

using namespace std;

const int externalSourceId = 17000017;

static int wtoi(const wstring &sp) {
  return (int)std::wcstol(sp.c_str(), nullptr, 10);
}

class CSVLineWrapper {
  const vector<wstring> &data;
  int row;
public:
  CSVLineWrapper(int row, const vector<wstring>& data) : data(data), row(row) {
  }

  const wstring& operator[](size_t i) const {
    if (i >= data.size()) {
      throw meosException("Invalid CSV file. Incorrect data specification on line X" + itos(row));
    }
    return data[i];
  }

  size_t size() const { return data.size(); }
};

csvimporter::csvimporter() : csvparser() {
  nimport = 0;
}

RunnerStatus ConvertOEStatus(int i)
{
  switch(i)
  {
      case 0:
      return StatusOK;
      case 1:  // Ej start
      return StatusDNS;
      case 2:  // Utg.
      return StatusDNF;
      case 3:  // Felst.
      return StatusMP;
      case 4: //Disk
      return StatusDQ;
      case 5: //Maxtid
      return StatusMAX;
  }
  return StatusUnknown;
}

bool csvimporter::importOS_CSV(oEvent &oe, const wstring &file) {
  enum {OSstno=0, OSdesc=1, OSstart=4, OStime=5, OSstatus=6, OSclubno=7, OSclub=9,
    OSnat=10, OSclassno=11, OSclass=12, OSlegs=14, OSfee=21, OSpaid=22};

  const size_t Offset=23;
  const size_t PostSize=11;

  enum {OSRsname=0, OSRfname=1, OSRyb=2, OSRsex=3, OSRstart=4,
    OSRfinish=5, OSRstatus=7, OSRcard=8, OSRrentcard=9};
  

  oe.noReevaluateOperation([&]() {

    nimport = 0;
    list<vector<wstring>> allLines;
    parse(file, allLines);
    auto it = allLines.begin();
    if (it == allLines.end())
      throw meosException("Invalid CSV file");

    int line = 1;
    set<wstring> matchedClasses;
    // Skip first line
    while (++it != allLines.end()) {
      CSVLineWrapper sp(++line, *it);

      if (sp.size() > 20 && sp[OSclub].size() > 0)
      {
        nimport++;

        //Create club with this club number...
        int ClubId = wtoi(sp[OSclubno]);
        pClub pclub = oe.getClubCreate(ClubId, sp[OSclub]);

        if (pclub) {
          pclub->getDI().setString("Nationality", sp[OSnat]);
          pclub->synchronize(true);
        }

        //Create class with this class number...
        int ClassId = wtoi(sp[OSclassno]);
        oe.getClassCreate(ClassId, sp[OSclass], matchedClasses);

        //Club is autocreated...
        pTeam team = oe.addTeam(sp[OSclub] + L" " + sp[OSdesc], ClubId, ClassId);
        team->setEntrySource(externalSourceId);

        team->setStartNo(wtoi(sp[OSstno]), oBase::ChangeType::Update);

        if (sp[12].length() > 0)
          team->setStatus(ConvertOEStatus(wtoi(sp[OSstatus])), true, oBase::ChangeType::Update);

        team->setStartTime(oe.convertAbsoluteTime(sp[OSstart]), true, oBase::ChangeType::Update);

        if (sp[OStime].length() > 0)
          team->setFinishTime(oe.convertAbsoluteTime(sp[OSstart]) + oe.convertAbsoluteTime(sp[OStime]) - oe.getZeroTimeNum());

        if (team->getStatus() == StatusOK && team->getFinishTime() == 0)
          team->setStatus(StatusUnknown, true, oBase::ChangeType::Update);

        size_t rindex = Offset;

        oDataInterface teamDI = team->getDI();

        teamDI.setInt("Fee", wtoi(sp[OSfee]));
        teamDI.setInt("Paid", wtoi(sp[OSpaid]));
        teamDI.setString("Nationality", sp[OSnat]);

        //Import runners!
        int runner = 0;
        while ((rindex + OSRrentcard) < sp.size() && sp[rindex + OSRfname].length() > 0) {
          int cardNo = wtoi(sp[rindex + OSRcard]);
          wstring sname = sp[rindex + OSRsname] + L", " + sp[rindex + OSRfname];
          pRunner r = oe.addRunner(sname, ClubId,
            ClassId, cardNo, sp[rindex + OSRyb], false);

          r->setEntrySource(externalSourceId);
          oDataInterface DI = r->getDI();
          r->setSex(interpretSex(sp[rindex + OSRsex]));
          DI.setString("Nationality", sp[OSnat]);

          if (sp[rindex + OSRrentcard].length() > 0)
            r->setRentalCard(true);

          r->setStartTime(oe.convertAbsoluteTime(sp[rindex + OSRstart]), true, oBase::ChangeType::Update);
          r->storeDefaultStartTime();
          r->setFinishTime(oe.convertAbsoluteTime(sp[rindex + OSRfinish]));

          if (sp[rindex + OSRstatus].length() > 0)
            r->setStatus(ConvertOEStatus(wtoi(sp[rindex + OSRstatus])), true, oBase::ChangeType::Update, false);

          if (r->getStatus() == StatusOK && r->getRunningTime(false) == 0)
            r->setStatus(StatusUnknown, true, oBase::ChangeType::Update, false);

          r->addClassDefaultFee(false);

          team->setRunner(runner++, r, true);

          rindex += PostSize;
        }
        pClass pc = oe.getClass(ClassId);
        int teamId = team->getId();
        if (pc && runner > (int)pc->getNumStages()) {
          oe.setupRelay(*pc, oEvent::PRelay, runner, oe.getAbsTime(timeConstHour));
        }
        team = oe.getTeam(teamId);
        
        if (team)
          team->evaluate(oBase::ChangeType::Update);
      }
    }
  });

  oe.reEvaluateAll({}, true);

  return true;
}

bool csvimporter::importOE_CSV(oEvent &event, const wstring &file) {

  enum {OEstno=0, OEcard=1, OEid=2, OEsurname=3, OEfirstname=4,
      OEbirth=5, OEsex=6, OEstart=9,  OEfinish=10, OEstatus=12,
      OEclubno=13, OEclub=14, OEclubcity=15, OEnat=16, OEclassno=17, OEclass=18, OEbib=23,
      OEtextB = 24, OEtextC = 25,
      OErent=35, OEfee=36, OEpaid=37, OEcourseno=38, OEcourse=39,
      OElength=40};

  list<vector<wstring>> allLines;
  parse(file, allLines);
  auto it = allLines.begin();
  if (it == allLines.end())
    throw meosException("Invalid CSV file");

  int line = 0;

  set<wstring> matchedClasses;
  nimport=0;
  while (++it != allLines.end()) {
    CSVLineWrapper sp(++line, *it);

    if (sp.size()>20) {
      nimport++;

      int clubId = wtoi(sp[OEclubno]);
      wstring clubName;
      wstring shortClubName;

      clubName = sp[OEclubcity];
      shortClubName = sp[OEclub];

      if (clubName.empty() && !shortClubName.empty())
        swap(clubName, shortClubName);
    
      pClub pclub = event.getClubCreate(clubId, clubName);

      if (pclub) {
        if (sp[OEnat].length()>0)
          pclub->getDI().setString("Nationality", sp[OEnat]);

        pclub->getDI().setString("ShortName", shortClubName.substr(0, 8));
        pclub->setExtIdentifier(clubId);
        pclub->synchronize(true);
      }

      __int64 extId = oBase::converExtIdentifierString(sp[OEid]);
      int id = oBase::idFromExtId(extId);
      pRunner pr = 0;

      if (id>0)
        pr = event.getRunner(id, 0);

      while (pr) { 
        if (extId == pr->getExtIdentifier())
          break;
        id++;
        pr = event.getRunner(id, 0);
      }

      if (pr) {
        if (pr->getEntrySource() != externalSourceId) {
          pr = 0;
          id = 0;
        }
      }

      const bool newEntry = (pr == 0);

      if (pr == 0) {
        if (id==0) {
          oRunner r(&event);
          pr = event.addRunner(r, true);
        }
        else {
          oRunner r(&event, id);
          pr = event.addRunner(r, true);
        }
      }

      if (pr==0)
        continue;

      pr->setExtIdentifier(extId);
      pr->setEntrySource(externalSourceId);

      if (!pr->hasFlag(oAbstractRunner::FlagUpdateName)) {
        wstring name = sp[OEsurname] + L", " + sp[OEfirstname];
        pr->setName(name, false);
      }
      pr->setClubId(pclub ? pclub->getId():0);
      pr->setCardNo( wtoi(sp[OEcard]), false );

      pr->setStartTime(event.convertAbsoluteTime(sp[OEstart]), true, oBase::ChangeType::Update);
      pr->storeDefaultStartTime();
      pr->setFinishTime(event.convertAbsoluteTime(sp[OEfinish]));

      if (sp[OEstatus].length()>0)
        pr->setStatus( ConvertOEStatus( wtoi(sp[OEstatus]) ), true, oBase::ChangeType::Update);

      if (pr->getStatus()==StatusOK && pr->getRunningTime(false)==0)
        pr->setStatus(StatusUnknown, true, oBase::ChangeType::Update);

      int classId=wtoi(sp[OEclassno]);
      if (classId>0 && !pr->hasFlag(oAbstractRunner::FlagUpdateClass)) {
        pClass pc=event.getClassCreate(classId, sp[OEclass], matchedClasses);

        if (pc) {
          pc->synchronize();
          if (pr->getClassId(false) == 0 || !pr->hasFlag(oAbstractRunner::FlagUpdateClass))
            pr->setClassId(pc->getId(), false);
        }
      }
      int stno=wtoi(sp[OEstno]);
      bool needSno = pr->getStartNo() == 0 || newEntry;
      bool needBib = pr->getBib().empty();
      
      if (needSno || newEntry) {
        if (stno>0)
          pr->setStartNo(stno, oBase::ChangeType::Update);
        else
          pr->setStartNo(nimport, oBase::ChangeType::Update);
      }
      oDataInterface DI=pr->getDI();

      pr->setSex(interpretSex(sp[OEsex]));
      pr->setBirthDate(sp[OEbirth]);
      DI.setString("Nationality", sp[OEnat]);

      if (sp.size()>OEbib && needBib)
        pr->setBib(sp[OEbib], 0, false);

      if (sp.size() > OEtextB)
        DI.setString("TextA", sp[OEtextB]); 

      if (sp.size() > OEtextC)
        DI.setString("Annotation", sp[OEtextC]); 

      if (sp.size()>=38) {
        DI.setInt("Fee", wtoi(sp[OEfee]));
        if (wtoi(sp[OErent]))
          pr->setRentalCard(true);

        DI.setInt("Paid", wtoi(sp[OEpaid]));
      }

      if (sp.size()>=40) {
        if (pr->getCourse(false) == 0) {
          const wstring &cid=sp[OEcourseno];
          const int courseid=wtoi(cid);
          if (courseid>0) {
            pCourse course=event.getCourse(courseid);

            if (!course) {
              oCourse oc(&event, courseid);
              oc.setLength(int(_wtof(sp[OElength].c_str())*1000));
              oc.setName(sp[OEcourse]);
              course = event.addCourse(oc);
              if (course)
                course->synchronize();
            }
            if (course) {
              if (pr->getClassId(false) != 0)
                event.getClass(pr->getClassId(false))->setCourse(course);
              else
                pr->setCourseId(course->getId());
            }
          }
        }
      }
      if (pr)
        pr->synchronize();
    }
  }

  return true;
}

bool csvimporter::importOCAD_CSV(oEvent &event, const wstring &file, bool addClasses) {
  list< vector<wstring> > allLines;
  parse(file, allLines);
  auto it = allLines.begin();
  int line = 0;
  while(it != allLines.end()) {
    CSVLineWrapper sp(++line, *it);
    ++it;

    if (sp.size()>7) {
      size_t firstIndex = 7;
      bool hasLengths = true;
      int offset = 0;
      if (wtoi(sp[firstIndex]) < 30) {
        firstIndex = 6;
        offset = -1;
      }

      if (wtoi(sp[firstIndex])<30 || wtoi(sp[firstIndex])>1000) {
        wstring str = L"Ogiltig banfil. Kontroll förväntad på position X, men hittade 'Y'.#"
                      + itow((int)firstIndex+1) + L"#" + sp[firstIndex];
        throw meosException(str.c_str());
      }

      while (firstIndex > 0 && wtoi(sp[firstIndex])>30 && wtoi(sp[firstIndex])>1000) {
        firstIndex--;
      }
      wstring Start = lang.tl(L"Start ") + L"1";
      double Length = 0;
      wstring Course = event.getAutoCourseName();
      wstring Class = L"";

      if (firstIndex>=6) {
        Class = sp[0];
        Course = sp[1+(size_t)offset];
        Start = sp[5+(size_t)offset];
        Length = _wtof(sp[3+(size_t)offset].c_str());

        if (Start[0]=='S') {
          int num = wtoi(Start.substr(1));
          if (num>0)
            Start = lang.tl(L"Start ") + Start.substr(1);
        }
      }
      else
        hasLengths = false;

      pCourse pc=event.getCourse(Course);

      if (!pc) {
        pc = event.addCourse(Course, int(Length*1000));
      }
      else {
        pc->importControls("", true, false);
        pc->setLength(int(Length*1000));
      }


      vector<int> legLengths;

      if (hasLengths) {
        double legLen = _wtof(sp[firstIndex-1].c_str()); 
        if (legLen > 0.001 && legLen < 30)
          legLengths.push_back(int(legLen*1000));
      }

      if (pc) {
        for (size_t k=firstIndex; k<sp.size()-1;k+=(hasLengths ? 2 : 1)) {
          wstring ctrlStr = trim(sp[k]);
          if (!ctrlStr.empty()) {
            if (ctrlStr[0] == 'S')
              continue;

            int ctrl = wtoi(ctrlStr.c_str());

            if (ctrl == 0 && trim(sp[k+1]).length() == 0)
              break; // Done

            if (ctrl >= 30 && ctrl < 1000)
              pc->addControl(wtoi(sp[k]));
            else {
              wstring str = L"Oväntad kontroll 'X' i bana Y.#" + ctrlStr + L"#" + pc->getName();
              throw meosException(str);
            }
          }
          if (hasLengths) {
            double legLen = _wtof(sp[k+1].c_str()); 
            if (legLen > 0.001 && legLen < 30)
              legLengths.push_back(int(legLen*1000));
          }
        }
        pc->setLength(int(Length*1000));
        pc->setStart(Start, true);

        if (legLengths.size() == pc->getNumControls()+1)
          pc->setLegLengths(legLengths);

        if (!Class.empty() && addClasses) {
          pClass cls = event.getBestClassMatch(Class);
          if (!cls)
            cls = event.addClass(Class);

          if (cls->getNumStages()==0) {
            cls->setCourse(pc);
          }
          else {
            for (size_t i = 0; i<cls->getNumStages(); i++)
              cls->addStageCourse((int)i, pc->getId(), -1);
          }

          cls->synchronize();
        }

        pc->synchronize();
      }
    }
  }
  return true;
}

bool csvimporter::importRAID(oEvent &event, const wstring &file)
{
  enum {RAIDid=0, RAIDteam=1, RAIDcity=2, RAIDedate=3, RAIDclass=4,
        RAIDclassid=5, RAIDrunner1=6, RAIDrunner2=7, RAIDcanoe=8};

  list<vector<wstring>> allLines;
  parse(file, allLines);

  set<wstring> matchedClasses;
  auto it = allLines.begin();
  if (it == allLines.end())
    throw meosException("Invalid CSV file");

  nimport=0;
  int line = 1;
  while (++it != allLines.end()) {
    CSVLineWrapper sp(++line, *it);

    if (sp.size()>7) {
      nimport++;

      int ClubId=0;
      int ClassId=wtoi(sp[RAIDclassid]);
      pClass pc = event.getClassCreate(ClassId, sp[RAIDclass], matchedClasses);
      ClassId = pc->getId();

      pTeam team=event.addTeam(sp[RAIDteam], ClubId,  ClassId);

      team->setStartNo(wtoi(sp[RAIDid]), oBase::ChangeType::Update);
      if (sp.size()>8)
        team->getDI().setInt("SortIndex", wtoi(sp[RAIDcanoe]));
      oDataInterface teamDI=team->getDI();
      teamDI.setDate("EntryDate", sp[RAIDedate]);
      
      if (pc) {
        if (pc->getNumStages()<2)
          pc->setNumStages(2);

        pc->setLegType(0, LTNormal);
        pc->setLegType(1, LTIgnore);
      }

      pRunner r1=event.addRunner(sp[RAIDrunner1], ClubId, ClassId, 0, L"", false);
      team->setRunner(0, r1, false);

      pRunner r2=event.addRunner(sp[RAIDrunner2], ClubId, ClassId, 0, L"", false);
      team->setRunner(1, r2, false);

      team->evaluate(oBase::ChangeType::Update);
    }
  }

  return true;
}

int csvimporter::selectPunchIndex(const wstring &competitionDate, const CSVLineWrapper &sp,
                                int &cardIndex, int &timeIndex, int &dateIndex,
                                wstring &processedTime, wstring &processedDate) {
  int ci = -1;
  int ti = -1;
  int di = -1;
  wstring pt, date;
  int maxCardNo = 0;
  processedDate.clear();
  for (size_t k = 0; k < sp.size(); k++) {
    processGeneralTime(sp[k], pt, date);
    if (!pt.empty()) {
        ti = (int)k;
        pt.swap(processedTime);
      if (!date.empty()) {
        date.swap(processedDate);
        di = (int)k;
      }
    }
    else if (k == 2 && sp[k].length() == 2 && processedDate.empty()) {
      processedDate = sp[k]; 
      dateIndex = 2;
    }
    else {
      int cno = wtoi(sp[k]);
      if (cno > maxCardNo) {
        maxCardNo = cno;
        ci = (int)k;
      }  
    }
  }

  if (ti == -1)
    return 0; 
  if (ci == -1)
    return 0; 

  if (timeIndex >= 0 && timeIndex != ti)
    return -1; 

  if (cardIndex >= 0 && cardIndex != ci)
    return -1; 

  timeIndex = ti;
  cardIndex = ci;
  dateIndex = di;
  return 1;
}

bool csvimporter::importPunches(const oEvent &oe, const wstring &file, vector<PunchInfo> &punches)
{
  punches.clear();
  list<vector<wstring>> allLines;
  parse(file, allLines);
  auto it = allLines.begin();
  if (it == allLines.end())
    throw meosException("Invalid CSV file");

  nimport=0;
  int cardIndex = -1;
  int timeIndex = -1;
  int dateIndex = -1;

  wstring processedTime, processedDate;
  const wstring date = oe.getDate();
  int line = 0;
  while (++it != allLines.end()) {
    CSVLineWrapper sp(++line, *it);

    int ret = selectPunchIndex(date, sp, cardIndex, timeIndex, dateIndex,
                               processedTime, processedDate); 
    if (ret == -1)
      return false; 
    if (ret > 0) {
      const int card = wtoi(sp[(size_t)cardIndex]);
      const int time = oe.getRelativeTime(processedTime);

      if (card>0) {
        PunchInfo pi;
        pi.card = card;
        pi.time = time;
        string pd = narrow(processedDate);
        strncpy_s(pi.date, pd.c_str(), 26);
        pi.date[26] = 0;
        punches.push_back(pi);
        nimport++;
      }
    }
  }

  return true;
}

int analyseSITime(const wchar_t *dow, const wchar_t *time, bool &is12Hour)
{
  int t = oEvent::convertAbsoluteTime(time);
  if (t >= 0 && trim(dow).empty())
    is12Hour = true;
  return t;
}

void csvimporter::checkSIConfigHeader(const CSVLineWrapper &sp) {
  siconfigmap.clear();
  if (sp.size() < 200)
    return;

  wstring key;
  for (size_t k = 0; k < sp.size(); k++) {
    key = sp[k];
    if (key == L"SIID" || key == L"SI-Card")
      siconfigmap[sicSIID] = (int)k;
    else if (key == L"Check CN" || key == L"CHK_CN") {
      siconfigmap[sicCheck] = (int)k;
      siconfigmap[sicCheckDOW] = (int)k + 1;
      siconfigmap[sicCheckTime] = (int)k + 2;
    }
    else if (key == L"Check time") {
      siconfigmap[sicCheckTime] = (int)k;
    }
    else if (key == L"Start CN" || key == L"ST_CN") {
      siconfigmap[sicStart] = (int)k;
      siconfigmap[sicStartDOW] = (int)k + 1;
      siconfigmap[sicStartTime] = (int)k + 2;
    }
    else if (key == L"Start time") {
      siconfigmap[sicStartTime] = (int)k;
    }
    else if (key == L"Finish CN" || key == L"FI_CN") {
      siconfigmap[sicFinish] = (int)k;
      siconfigmap[sicFinishDOW] = (int)k + 1;
      siconfigmap[sicFinishTime] = (int)k + 2;
    }
    else if (key == L"Finish time") {
      siconfigmap[sicFinishTime] = (int)k;
    }
    else if (key == L"First name") {
      siconfigmap[sicFirstName] = (int)k;
    }
    else if (key == L"Last name" || key == L"name") {
      siconfigmap[sicLastName] = (int)k;
    }
    else if (key == L"No. of records" || key == L"No. of punches") {
      siconfigmap[sicNumPunch] = (int)k;
    }
    else if (siconfigmap.count(sicRecordStart) == 0) {
      size_t pos = key.find(L"Record 1");
      if (pos == string::npos) {
        pos = key.find(L"1.CN");
      }
      if (pos != string::npos) {
        siconfigmap[sicRecordStart] = (int)k;
        if (siconfigmap.count(sicRecordStart) == 0 && k > 0)
          siconfigmap[sicNumPunch] = (int)k-1;
      }
    }
  }
}

const wchar_t *csvimporter::getSIC(SIConfigFields sic, const CSVLineWrapper &sp) const {
  map<SIConfigFields, int>::const_iterator res = siconfigmap.find(sic);
  if (res == siconfigmap.end() || (size_t)res->second >= sp.size())
    return L"";

  return sp[(size_t)res->second].c_str();
}

bool csvimporter::checkSIConfigLine(const oEvent &oe, const CSVLineWrapper &sp, SICard &card) {
  if (siconfigmap.empty())
    return false;

  int startIx = siconfigmap[sicRecordStart];
  if (startIx == 0)
    return false;

  int cardNo = wtoi(getSIC(sicSIID, sp));

  if (cardNo < 1000 || cardNo > 99999999)
    return false;
  bool is12Hour = false;
  int check = analyseSITime(getSIC(sicCheckDOW, sp), getSIC(sicCheckTime, sp), is12Hour);
  int start = analyseSITime(getSIC(sicStartDOW, sp), getSIC(sicStartTime, sp), is12Hour);
  int finish = analyseSITime(getSIC(sicFinishDOW, sp), getSIC(sicFinishTime, sp), is12Hour);
  int startCode = wtoi(getSIC(sicStart, sp));
  int finishCode = wtoi(getSIC(sicFinish, sp));
  int checkCode = wtoi(getSIC(sicCheck, sp));
  wstring fname = getSIC(sicFirstName, sp);
  wstring lname = getSIC(sicLastName, sp);
  
  vector< pair<int, int> > punches;
  int np = wtoi(getSIC(sicNumPunch, sp));


  for (int k = 0; k < np; k++) {
    size_t ix = (size_t)startIx + (size_t)k * 3;
    if (ix + 2 >= sp.size())
      return false;
    int code = wtoi(sp[ix]);
    int time = analyseSITime(sp[ix + 1].c_str(), sp[ix + 2].c_str(), is12Hour);
    if (code > 0) {
      punches.push_back(make_pair(code, time));
    }
    else {
      return false;
    }
  }
 
  if ( (finish > 0 && finish != NOTIME) || punches.size() > 0 || (start > 0 && start != NOTIME)) {
    card.clear(0);
    card.CardNumber = cardNo;
    if (start > 0 && start != NOTIME) {
      card.StartPunch.Code = startCode;
      card.StartPunch.Time = start;
    }
    else {
      card.StartPunch.Time = 0;
      card.StartPunch.Code = -1;
    }

    if (finish > 0 && finish != NOTIME) {
      card.FinishPunch.Code = finishCode;
      card.FinishPunch.Time = finish;
    }
    else {
      card.FinishPunch.Time = 0;
      card.FinishPunch.Code = -1;
    }

    if (check > 0 && check != NOTIME) {
      card.CheckPunch.Code = checkCode;
      card.CheckPunch.Time = check;
    }
    else {
      card.CheckPunch.Time = 0;
      card.CheckPunch.Code = -1;
    }

    for (size_t k = 0; k<punches.size(); k++) {
      card.Punch[k].Code = punches[k].first;
      card.Punch[k].Time = punches[k].second;
    }
    
    wcsncpy(card.firstName, fname.c_str(), 20);
    card.firstName[20] = 0;
    wcsncpy(card.lastName, lname.c_str(), 20);
    card.lastName[20] = 0;
    card.nPunch = (int)punches.size();
    card.convertedTime = is12Hour ? ConvertedTimeStatus::Hour12 :  ConvertedTimeStatus::Hour24;
    return true;
  }

  return false;
}

bool csvimporter::checkSimanLine(const oEvent &oe, const CSVLineWrapper &sp, SICard &card) {
  if (sp.size() <= 11)
    return false;

  int cardNo = wtoi(sp[1]);
  
  if (wcschr(sp[1].c_str(), '-') != 0)
    cardNo = 0; 

  if (cardNo < 1000 || cardNo > 99999999)
    return false;

  int start = convertAbsoluteTimeMS(sp[5]);
  int finish = convertAbsoluteTimeMS(sp[6]);
  vector< pair<int, int> > punches;
  for (size_t k=10; k + 1<sp.size(); k+=2) {
    int code = wtoi(sp[k]);
    int time = convertAbsoluteTimeMS(sp[k+1]);
    if (code > 0) {
      punches.push_back(make_pair(code, time));
    }
    else {
      return false;
    }
  }

  if (punches.size() > 2) {
    card.clear(0);
    card.CardNumber = cardNo;
    if (start > 0) {
      card.StartPunch.Code = 0;
      card.StartPunch.Time = start;
    }
    else {
      card.StartPunch.Code = -1;
    }

    if (finish > 0) {
      card.FinishPunch.Code = 0;
      card.FinishPunch.Time = finish;
    }
    else {
      card.FinishPunch.Code = -1;
    }

    for (size_t k = 0; k<punches.size(); k++) {
      card.Punch[k].Code = punches[k].first;
      card.Punch[k].Time = punches[k].second;
    }
    card.nPunch = (int)punches.size();
    card.convertedTime = ConvertedTimeStatus::Hour24; 
    return true;
  }

  return false;
}

bool csvimporter::importCards(const oEvent &oe, const wstring &file, vector<SICard> &cards)
{
  cards.clear();
  list<vector<wstring>> allLines;
  parse(file, allLines);
  auto it = allLines.begin();
  if (it == allLines.end())
    return false;

  checkSIConfigHeader(CSVLineWrapper(1, *it));
  nimport=0;
  int line = 1;
  while (++it != allLines.end()) {
    CSVLineWrapper sp(++line, *it);

    SICard card(ConvertedTimeStatus::Unknown);

    if (checkSimanLine(oe, sp, card)) {
      cards.push_back(card);
      nimport++;
    }
    else if (checkSIConfigLine(oe,sp, card)) {
      cards.push_back(card);
      nimport++;
    }
    else if (sp.size()>28) {
      int no = wtoi(sp[0]);
      card.CardNumber = wtoi(sp[2]);
      wcsncpy(card.firstName, sp[5].c_str(), 20);
      card.firstName[19] = 0;
      wcsncpy(card.lastName, sp[6].c_str(), 20);
      card.lastName[19] = 0;
      wcsncpy(card.club, sp[7].c_str(), 40);
      card.club[39] = 0;
      bool hour12 = false;
      if (trim(sp[21]).length()>1) {
        card.CheckPunch.Code = wtoi(sp[19]);
        card.CheckPunch.Time = analyseSITime(sp[20].c_str(), sp[21].c_str(), hour12);
      }
      else {
        card.CheckPunch.Code = -1;
        card.CheckPunch.Time = 0;
      }

      if (trim(sp[24]).length()>1) {
        card.StartPunch.Code = wtoi(sp[22]);
        card.StartPunch.Time = analyseSITime(sp[23].c_str(), sp[24].c_str(), hour12);
      }
      else {
        card.StartPunch.Code = -1;
        card.StartPunch.Time = 0;
      }

      if (trim(sp[27]).length()>1) {
        card.FinishPunch.Code = wtoi(sp[25]);
        card.FinishPunch.Time = analyseSITime(sp[26].c_str(), sp[27].c_str(), hour12);
      }
      else  {
        card.FinishPunch.Code = -1;
        card.FinishPunch.Time = 0;
      }

      card.nPunch = wtoi(sp[28]);
      if (no>0 && card.CardNumber>0 && card.nPunch>0 && card.nPunch < 200) {
        if (sp.size()>28+3*(size_t)card.nPunch) {
          for (unsigned k=0;k<card.nPunch;k++) {
            card.Punch[k].Code = wtoi(sp[29+k*3]);
            card.Punch[k].Time = analyseSITime(sp[30+k*3].c_str(), sp[31+k*3].c_str(), hour12);
          }
          card.punchOnly = false;
          nimport++;
          card.convertedTime = hour12 ? ConvertedTimeStatus::Hour12 : ConvertedTimeStatus::Hour24;
          cards.push_back(card);
        }
      }
    }
  }

  return true;
}

void csvimporter::importTeamLineup(const wstring &file,
                                 const map<wstring, int> &classNameToNumber,
                                 vector<TeamLineup> &teams) {
  list< vector<wstring> > data;
  parse(file, data);
  teams.clear();
  teams.reserve(data.size()/3);
  int membersToRead = 0;
  int lineNo = 1;
  while (!data.empty()) {
    vector<wstring> &line = data.front();
    if (!line.empty()) {
      if (membersToRead == 0) {
        if (line.size() < 2 || line.size() > 3)
          throw meosException(L"Ogiltigt lag på rad X.#" + itow(lineNo) + L": " + line[0]);
        const wstring cls = trim(line[0]);
        map<wstring, int>::const_iterator res = classNameToNumber.find(cls);
        if (res == classNameToNumber.end())
          throw meosException(L"Okänd klass på rad X.#" + itow(lineNo) + L": " + cls);
        if (res->second <= 1)
          throw meosException(L"Klassen X är individuell.#" + cls);

        membersToRead = res->second;
        teams.push_back(TeamLineup());
        teams.back().teamClass = cls;
        teams.back().teamName = trim(line[1]);
        if (line.size() >= 3)
          teams.back().teamClub = trim(line[2]);
      }
      else {
        membersToRead--;
        teams.back().members.push_back(TeamLineup::TeamMember());
        TeamLineup::TeamMember &member = teams.back().members.back();
        member.name = trim(line[0]);
        if (line.size()>1)
          member.cardNo = _wtoi(line[1].c_str());
        else
          member.cardNo = 0;

        if (line.size()>2)
          member.club = trim(line[2]);

        if (line.size() > 3)
          member.course = trim(line[3]);

        if (line.size() > 4)
          member.cls = trim(line[4]);
      }
    }
    else if (membersToRead>0) {
      membersToRead--;
      teams.back().members.push_back(TeamLineup::TeamMember());
    }
    lineNo++;
    data.pop_front();
  }
}

int csvimporter::importRanking(oEvent &oe, const wstring &file, vector<wstring> &problems) {
  list< vector<wstring> > data;
  parse(file, data);

  size_t idIx = (size_t)-1;
  size_t nameIx = 1;
  size_t lastNameIx = (size_t)-1;
  size_t rankIx = (size_t)-1;
  map<int64_t, pair<wstring, int> > id2Rank;
  map<wstring, pair<int, bool> > name2RankDup;
  bool first = true;
  for (auto &rank : data) {
    if (first) {
      first = false;
      bool any = false;

      for (size_t i = 0; i < rank.size(); i++) {
        wstring s = canonizeName(rank[i].c_str());
        
        if (s.find(L"name")  != wstring::npos && (s.find(L"last") != wstring::npos || s.find(L"family") != wstring::npos) && lastNameIx == (size_t)-1) {
          lastNameIx = i;
          any = true;
        }
        else if (s.find(L"name") != wstring::npos && (s.find(L"first") != wstring::npos || s.find(L"given") != wstring::npos) && nameIx == (size_t)-1) {
          nameIx = i;
          any = true;
        }
        else if (s.find(L" id") != wstring::npos && idIx == (size_t)-1) {
          idIx = i;
          any = true;
        }
        else if (s.find(L"position") != wstring::npos && rankIx == (size_t)-1) {
          rankIx = i;
          any = true;
        }
      }

      if (idIx == (size_t)-1)
        idIx = 0;

      if (nameIx == (size_t)-1)
        nameIx = 1;

      if (lastNameIx == (size_t)-1)
        lastNameIx = 2;

      if (rankIx == (size_t)-1)
        rankIx = 4;

      if (any)
        continue;
    }
    
    if (rank.size() <= rankIx)
      continue;

    int rpos = _wtoi(rank[rankIx].c_str());
    if (rpos <= 0)
      continue;

    wstring name;
    if (nameIx < rank.size()) {
      name = rank[nameIx];

      if (lastNameIx < rank.size()) {
        name += L" " + rank[lastNameIx];
      }
    }
    if (name.empty())
      continue;

    auto res = name2RankDup.emplace(name, make_pair(rpos, false));

    if (!res.second)
      res.first->second.second = true; 

    if (idIx < rank.size()) {
      int64_t id = oBase::converExtIdentifierString(rank[idIx]);
      if (id != 0)
        id2Rank[id] = make_pair(name, rpos);
    }
  }

  if ((name2RankDup.size() < data.size() / 2 || name2RankDup.empty()) &&
    (id2Rank.size() < data.size() / 2 || id2Rank.empty())) {
    throw meosException(L"Felaktigt rankingformat i X. Förväntat: Y#" + file + L"#ID; First name; Last name; Rank");
  }

  vector<pRunner> runners;
  oe.getRunners(-1, -1, runners);
  
  int count = 0;
  vector<pRunner> remRunners;
  for (pRunner r : runners) {
    int64_t id = r->getExtIdentifier();
    auto res = id2Rank.find(id);
    if (res != id2Rank.end() && r->matchName(res->second.first)) {
      r->getDI().setInt("Rank", res->second.second);
      r->synchronize(true);
      count++;
    }
    else
      remRunners.push_back(r);
  }

  for (pRunner r : remRunners) {
    auto res = name2RankDup.find(r->getName());
    if (r->getRaceNo() > 0)
      continue;
    if (res != name2RankDup.end()) {
      if (res->second.second)
        problems.push_back(r->getCompleteIdentification(oRunner::IDType::OnlyThis));
      else {
        res->second.second = true;
        r->getDI().setInt("Rank", res->second.first);
        r->synchronize(true);
        count++;
      }
    }
  }

  return count;
}
