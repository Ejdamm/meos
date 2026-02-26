#pragma once
#include <nlohmann/json.hpp>
#include "oRunner.h"
#include "oClub.h"
#include "oTeam.h"
#include "oClass.h"
#include "oCourse.h"
#include "oControl.h"
#include "oCard.h"
#include "oFreePunch.h"
#include "oEvent.h"

// ---------------------------------------------------------------------------
// oClub
// ---------------------------------------------------------------------------

inline void to_json(nlohmann::json &j, const oClub &c) {
  j = nlohmann::json{
    {"id",   c.getId()},
    {"name", gdioutput::toUTF8(c.getName())}
  };
  // Optional DCI fields
  auto country = c.getDCI().getString("Country");
  if (!country.empty())
    j["country"] = gdioutput::toUTF8(country);
}

// from_json for oClub is intentionally omitted: oClub is managed by oEvent
// and must be created/modified via oEvent::getClubCreate / setName.

// ---------------------------------------------------------------------------
// oRunner
// ---------------------------------------------------------------------------

inline void to_json(nlohmann::json &j, const oRunner &r) {
  j = nlohmann::json{
    {"id",        r.getId()},
    {"name",      gdioutput::toUTF8(r.getName())},
    {"clubId",    r.getClubId()},
    {"club",      gdioutput::toUTF8(r.getClub())},
    {"classId",   r.getClassId(false)},
    {"class",     gdioutput::toUTF8(r.getClass(false))},
    {"cardNo",    r.getCardNo()},
    {"bib",       gdioutput::toUTF8(r.getBib())},
    {"startTime", r.getStartTime()},
    {"finishTime",r.getFinishTime()},
    {"runningTime",r.getRunningTime(false)},
    {"status",    static_cast<int>(r.getStatus())},
    {"birthYear", r.getBirthYear()},
    {"nationality", gdioutput::toUTF8(r.getNationality())},
    {"sex",       static_cast<int>(r.getSex())}
  };
}

// from_json populates a plain data-transfer struct; actual object mutation
// goes through oEvent methods.
struct RunnerDTO {
  int         id         = 0;
  std::wstring name;
  int         clubId     = 0;
  int         classId    = 0;
  int         cardNo     = 0;
  std::wstring bib;
  int         startTime  = 0;
  int         finishTime = 0;
  int         status     = 0;
  int         birthYear  = 0;
  std::wstring nationality;
  int         sex        = 0;
};

inline void from_json(const nlohmann::json &j, RunnerDTO &r) {
  if (j.contains("id"))          r.id          = j.at("id").get<int>();
  if (j.contains("name"))        r.name        = gdioutput::fromUTF8(j.at("name").get<std::string>());
  if (j.contains("clubId"))      r.clubId      = j.at("clubId").get<int>();
  if (j.contains("classId"))     r.classId     = j.at("classId").get<int>();
  if (j.contains("cardNo"))      r.cardNo      = j.at("cardNo").get<int>();
  if (j.contains("bib"))         r.bib         = gdioutput::fromUTF8(j.at("bib").get<std::string>());
  if (j.contains("startTime"))   r.startTime   = j.at("startTime").get<int>();
  if (j.contains("finishTime"))  r.finishTime  = j.at("finishTime").get<int>();
  if (j.contains("status"))      r.status      = j.at("status").get<int>();
  if (j.contains("birthYear"))   r.birthYear   = j.at("birthYear").get<int>();
  if (j.contains("nationality")) r.nationality = gdioutput::fromUTF8(j.at("nationality").get<std::string>());
  if (j.contains("sex"))         r.sex         = j.at("sex").get<int>();
}

struct ClubDTO {
  int          id      = 0;
  std::wstring name;
  std::wstring country;
};

inline void from_json(const nlohmann::json &j, ClubDTO &c) {
  if (j.contains("id"))      c.id      = j.at("id").get<int>();
  if (j.contains("name"))    c.name    = gdioutput::fromUTF8(j.at("name").get<std::string>());
  if (j.contains("country")) c.country = gdioutput::fromUTF8(j.at("country").get<std::string>());
}

// ---------------------------------------------------------------------------
// oTeam
// ---------------------------------------------------------------------------

inline void to_json(nlohmann::json &j, const oTeam &t) {
  // Basic identity (inherited from oAbstractRunner / oBase)
  j = nlohmann::json{
    {"id",          t.getId()},
    {"name",        gdioutput::toUTF8(t.getName())},
    {"clubId",      t.getClubId()},
    {"club",        gdioutput::toUTF8(t.getClub())},
    {"classId",     t.getClassId(false)},
    {"class",       gdioutput::toUTF8(t.getClass(false))},
    {"bib",         gdioutput::toUTF8(t.getBib())},
    {"startTime",   t.getStartTime()},
    {"finishTime",  t.getFinishTime()},
    {"runningTime", t.getRunningTime(false)},
    {"status",      static_cast<int>(t.getStatusComputed(false))}
  };

  // Runner id list (one entry per leg, 0 if unassigned)
  nlohmann::json runners = nlohmann::json::array();
  int n = t.getNumRunners();
  for (int leg = 0; leg < n; ++leg) {
    pRunner r = t.getRunner(leg);
    runners.push_back(r ? r->getId() : 0);
  }
  j["runners"] = runners;

  // Per-leg results
  nlohmann::json legResults = nlohmann::json::array();
  for (int leg = 0; leg < n; ++leg) {
    legResults.push_back({
      {"leg",         leg},
      {"runningTime", t.getLegRunningTime(leg, false, false)},
      {"status",      static_cast<int>(t.getLegStatus(leg, false, false))}
    });
  }
  j["legResults"] = legResults;
}

struct TeamDTO {
  int          id          = 0;
  std::wstring name;
  int          clubId      = 0;
  int          classId     = 0;
  std::wstring bib;
  int          startTime   = 0;
  int          finishTime  = 0;
  int          status      = 0;
  std::vector<int> runnerIds;
};

inline void from_json(const nlohmann::json &j, TeamDTO &t) {
  if (j.contains("id"))         t.id         = j.at("id").get<int>();
  if (j.contains("name"))       t.name       = gdioutput::fromUTF8(j.at("name").get<std::string>());
  if (j.contains("clubId"))     t.clubId     = j.at("clubId").get<int>();
  if (j.contains("classId"))    t.classId    = j.at("classId").get<int>();
  if (j.contains("bib"))        t.bib        = gdioutput::fromUTF8(j.at("bib").get<std::string>());
  if (j.contains("startTime"))  t.startTime  = j.at("startTime").get<int>();
  if (j.contains("finishTime")) t.finishTime = j.at("finishTime").get<int>();
  if (j.contains("status"))     t.status     = j.at("status").get<int>();
  if (j.contains("runners")) {
    for (auto &rid : j.at("runners"))
      t.runnerIds.push_back(rid.get<int>());
  }
}

// ---------------------------------------------------------------------------
// oCourse
// ---------------------------------------------------------------------------

inline void to_json(nlohmann::json &j, const oCourse &c) {
  j = nlohmann::json{
    {"id",          c.getId()},
    {"name",        gdioutput::toUTF8(c.getName())},
    {"length",      c.getLength()},
    {"numControls", c.getNumControls()}
  };
  // Control id list
  nlohmann::json controls = nlohmann::json::array();
  for (int i = 0; i < c.getNumControls(); ++i) {
    oControl *ctrl = c.getControl(i);
    controls.push_back(ctrl ? ctrl->getId() : 0);
  }
  j["controls"] = controls;
  int startId = c.getStartId();
  if (startId)
    j["startId"] = startId;
  int finishId = c.getFinishId();
  if (finishId)
    j["finishId"] = finishId;
}

struct CourseDTO {
  int          id         = 0;
  std::wstring name;
  int          length     = 0;
  std::vector<int> controlIds;
};

inline void from_json(const nlohmann::json &j, CourseDTO &c) {
  if (j.contains("id"))     c.id     = j.at("id").get<int>();
  if (j.contains("name"))   c.name   = gdioutput::fromUTF8(j.at("name").get<std::string>());
  if (j.contains("length")) c.length = j.at("length").get<int>();
  if (j.contains("controls")) {
    for (auto &cid : j.at("controls"))
      c.controlIds.push_back(cid.get<int>());
  }
}

// ---------------------------------------------------------------------------
// oClass
// ---------------------------------------------------------------------------

inline void to_json(nlohmann::json &j, const oClass &c) {
  j = nlohmann::json{
    {"id",         c.getId()},
    {"name",       gdioutput::toUTF8(c.getName())},
    {"type",       gdioutput::toUTF8(c.getType())},
    {"numStages",  static_cast<int>(c.getNumStages())},
    {"courseId",   c.getCourseId()}
  };
}

struct ClassDTO {
  int          id        = 0;
  std::wstring name;
  int          courseId  = 0;
};

inline void from_json(const nlohmann::json &j, ClassDTO &c) {
  if (j.contains("id"))       c.id       = j.at("id").get<int>();
  if (j.contains("name"))     c.name     = gdioutput::fromUTF8(j.at("name").get<std::string>());
  if (j.contains("courseId")) c.courseId = j.at("courseId").get<int>();
}

// ---------------------------------------------------------------------------
// oControl
// ---------------------------------------------------------------------------

inline void to_json(nlohmann::json &j, const oControl &c) {
  j = nlohmann::json{
    {"id",     c.getId()},
    {"name",   gdioutput::toUTF8(c.getName())},
    {"status", static_cast<int>(c.getStatus())}
  };
  std::vector<int> numbers;
  c.getNumbers(numbers);
  j["codes"] = numbers;
  int pts = c.getRogainingPoints();
  if (pts)
    j["rogainingPoints"] = pts;
}

struct ControlDTO {
  int              id     = 0;
  std::wstring     name;
  std::vector<int> codes;
};

inline void from_json(const nlohmann::json &j, ControlDTO &c) {
  if (j.contains("id"))    c.id   = j.at("id").get<int>();
  if (j.contains("name"))  c.name = gdioutput::fromUTF8(j.at("name").get<std::string>());
  if (j.contains("codes")) {
    for (auto &code : j.at("codes"))
      c.codes.push_back(code.get<int>());
  }
}

// ---------------------------------------------------------------------------
// oPunch
// ---------------------------------------------------------------------------

inline void to_json(nlohmann::json &j, const oPunch &p) {
  j = nlohmann::json{
    {"type",      p.getTypeCode()},
    {"time",      p.getTimeInt()},
    {"controlId", p.getControlId()}
  };
}

// ---------------------------------------------------------------------------
// oCard
// ---------------------------------------------------------------------------

inline void to_json(nlohmann::json &j, const oCard &card) {
  j = nlohmann::json{
    {"id",     card.getId()},
    {"cardNo", card.getCardNo()}
  };
  nlohmann::json punches = nlohmann::json::array();
  int n = card.getNumPunches();
  for (int i = 0; i < n; ++i) {
    oPunch *p = card.getPunchByIndex(i);
    if (p)
      punches.push_back(*p);
  }
  j["punches"] = punches;
}

// ---------------------------------------------------------------------------
// oFreePunch
// ---------------------------------------------------------------------------

inline void to_json(nlohmann::json &j, const oFreePunch &p) {
  j = nlohmann::json{
    {"id",        p.getId()},
    {"cardNo",    p.getCardNo()},
    {"controlId", p.getControlId()},
    {"type",      p.getTypeCode()},
    {"time",      p.getTimeInt()}
  };
}

struct FreePunchDTO {
  int id        = 0;
  int cardNo    = 0;
  int controlId = 0;
  int type      = 0;
  int time      = 0;
};

inline void from_json(const nlohmann::json &j, FreePunchDTO &p) {
  if (j.contains("id"))        p.id        = j.at("id").get<int>();
  if (j.contains("cardNo"))    p.cardNo    = j.at("cardNo").get<int>();
  if (j.contains("controlId")) p.controlId = j.at("controlId").get<int>();
  if (j.contains("type"))      p.type      = j.at("type").get<int>();
  if (j.contains("time"))      p.time      = j.at("time").get<int>();
}

// ---------------------------------------------------------------------------
// oEvent (summary/metadata)
// ---------------------------------------------------------------------------

inline void to_json(nlohmann::json &j, const oEvent &e) {
  j = nlohmann::json{
    {"id",          e.getId()},
    {"name",        gdioutput::toUTF8(e.getName())},
    {"date",        gdioutput::toUTF8(e.getDate())},
    {"zeroTime",    gdioutput::toUTF8(e.getZeroTime())},
    {"numRunners",  e.getNumRunners()},
    {"numClasses",  e.getNumClasses()},
    {"numCourses",  e.getNumCourses()},
    {"numCards",    e.getNumCards()}
  };
}
