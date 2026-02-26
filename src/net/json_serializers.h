#pragma once
#include <nlohmann/json.hpp>
#include "oRunner.h"
#include "oClub.h"

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
