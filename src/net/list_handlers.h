// SPDX-License-Identifier: GPL-3.0-or-later
// List and results REST API endpoints.
// Registers handlers for /api/lists and /api/results on an ApiRouter.
#pragma once

#include "api_router.h"
#include "json_serializers.h"
#include "oEvent.h"
#include "oRunner.h"
#include <string>

// Known standard list types exposed via the REST API.
struct ListTypeInfo {
  int         code;
  const char *id;   // URL-friendly identifier
  const char *name; // Human-readable name
  bool        isResultList;
  bool        isStartList;
};

static const ListTypeInfo kListTypes[] = {
  { EStdStartList,           "startlist",               "Start List",                          false, true  },
  { EStdResultList,          "resultlist",              "Result List",                          true,  false },
  { EGeneralResultList,      "general-result",          "General Result List",                  true,  false },
  { ERogainingInd,           "rogaining",               "Rogaining Individual",                 true,  false },
  { EStdTeamResultListAll,   "team-result-all",         "Team Result (All Legs)",               true,  false },
  { EStdTeamResultList,      "team-result",             "Team Result",                          true,  false },
  { EStdTeamStartList,       "team-startlist",          "Team Start List",                      false, true  },
  { EStdTeamStartListLeg,    "team-startlist-leg",      "Team Start List (Leg)",                false, true  },
  { EStdClubStartList,       "club-startlist",          "Club Start List",                      false, true  },
  { EStdClubResultList,      "club-result",             "Club Result List",                     true,  false },
  { EStdPatrolStartList,     "patrol-startlist",        "Patrol Start List",                    false, true  },
  { EStdPatrolResultList,    "patrol-result",           "Patrol Result List",                   true,  false },
  { EIndCourseList,          "course-list",             "Individual Course List",               false, false },
  { ETeamCourseList,         "team-course-list",        "Team Course List",                     false, false },
};

static const int kNumListTypes = static_cast<int>(sizeof(kListTypes) / sizeof(kListTypes[0]));

// Look up a list type by its URL-friendly id string. Returns nullptr if not found.
static const ListTypeInfo *findListTypeById(const std::string &id) {
  for (int i = 0; i < kNumListTypes; ++i)
    if (id == kListTypes[i].id)
      return &kListTypes[i];
  return nullptr;
}

// Serialise a single runner entry for a result/start list response.
static nlohmann::json runnerToResultJson(pRunner r, int place) {
  nlohmann::json j;
  to_json(j, *r);
  if (place > 0)
    j["place"] = place;
  return j;
}

// Serialise a single team entry.
static nlohmann::json teamToResultJson(pTeam t, int place) {
  nlohmann::json j;
  to_json(j, *t);
  if (place > 0)
    j["place"] = place;
  return j;
}

// Register list and results endpoints on the given router.
// `oe` must outlive the router (pointer is captured by handlers).
inline void registerListHandlers(ApiRouter &router, oEvent *oe) {

  // GET /api/lists – return all available list types.
  router.get("/api/lists", [](const ApiRequest &) -> ApiResponse {
    nlohmann::json arr = nlohmann::json::array();
    for (int i = 0; i < kNumListTypes; ++i) {
      nlohmann::json j;
      j["id"]            = kListTypes[i].id;
      j["name"]          = kListTypes[i].name;
      j["code"]          = kListTypes[i].code;
      j["isResultList"]  = kListTypes[i].isResultList;
      j["isStartList"]   = kListTypes[i].isStartList;
      arr.push_back(std::move(j));
    }
    return ApiResponse::ok(arr.dump());
  });

  // GET /api/lists/:type – generate a list of runner/team entries for the given list type.
  // Optional query parameters: ?class=<classId> to restrict to a single class.
  router.get("/api/lists/:type", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");

    auto typeIt = req.pathParams.find("type");
    if (typeIt == req.pathParams.end())
      return ApiResponse::badRequest("Missing type");

    const ListTypeInfo *info = findListTypeById(typeIt->second);
    if (!info)
      return ApiResponse::notFound("Unknown list type");

    // Optional class filter.
    int classId = 0;
    auto classIt = req.queryParams.find("class");
    if (classIt != req.queryParams.end()) {
      try { classId = std::stoi(classIt->second); } catch (...) {
        return ApiResponse::badRequest("Invalid class id");
      }
    }

    nlohmann::json result;
    result["type"]  = info->id;
    result["name"]  = info->name;

    bool isTeamList = (info->code == EStdTeamResultListAll ||
                       info->code == EStdTeamResultList    ||
                       info->code == EStdTeamStartList     ||
                       info->code == EStdTeamStartListLeg  ||
                       info->code == EStdPatrolStartList   ||
                       info->code == EStdPatrolResultList  ||
                       info->code == ETeamCourseList);

    if (isTeamList) {
      std::vector<pTeam> teams;
      SortOrder so = info->isResultList ? ClassResult : ClassStartTime;
      if (classId > 0) {
        oe->getTeams(classId, teams, false);
        oe->sortTeams(so, 0, true, teams);
      } else {
        oe->getTeams(0, teams, false);
        oe->sortTeams(so, 0, true, teams);
      }

      nlohmann::json arr = nlohmann::json::array();
      int place = 0;
      for (pTeam t : teams) {
        if (!t) continue;
        arr.push_back(teamToResultJson(t, info->isResultList ? ++place : 0));
      }
      result["entries"] = std::move(arr);
    } else {
      std::vector<pRunner> runners;
      SortOrder so = info->isResultList ? ClassResult : ClassStartTime;
      if (classId > 0) {
        oe->getRunners(classId, 0, runners, false);
        oe->sortRunners(so, runners);
      } else {
        oe->getRunners(0, 0, runners, false);
        oe->sortRunners(so, runners);
      }

      nlohmann::json arr = nlohmann::json::array();
      int place = 0;
      for (pRunner r : runners) {
        if (!r) continue;
        arr.push_back(runnerToResultJson(r, info->isResultList ? ++place : 0));
      }
      result["entries"] = std::move(arr);
    }

    return ApiResponse::ok(result.dump());
  });

  // GET /api/results – result list with optional filters.
  // Query parameters: ?class=<classId>&type=<listTypeId> (defaults to "resultlist").
  router.get("/api/results", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");

    // Determine list type (default: individual result list).
    std::string typeId = "resultlist";
    auto typeIt = req.queryParams.find("type");
    if (typeIt != req.queryParams.end())
      typeId = typeIt->second;

    const ListTypeInfo *info = findListTypeById(typeId);
    if (!info)
      return ApiResponse::notFound("Unknown list type: " + typeId);
    if (!info->isResultList)
      return ApiResponse::badRequest("List type is not a result list");

    // Optional class filter.
    int classId = 0;
    auto classIt = req.queryParams.find("class");
    if (classIt != req.queryParams.end()) {
      try { classId = std::stoi(classIt->second); } catch (...) {
        return ApiResponse::badRequest("Invalid class id");
      }
    }

    bool isTeamList = (info->code == EStdTeamResultListAll ||
                       info->code == EStdTeamResultList    ||
                       info->code == EStdPatrolResultList);

    nlohmann::json result;
    result["type"] = info->id;
    result["name"] = info->name;

    if (isTeamList) {
      std::vector<pTeam> teams;
      if (classId > 0) {
        oe->getTeams(classId, teams, false);
        oe->sortTeams(ClassResult, 0, true, teams);
      } else {
        oe->getTeams(0, teams, false);
        oe->sortTeams(ClassResult, 0, true, teams);
      }

      nlohmann::json arr = nlohmann::json::array();
      int place = 0;
      for (pTeam t : teams) {
        if (!t) continue;
        arr.push_back(teamToResultJson(t, ++place));
      }
      result["results"] = std::move(arr);
    } else {
      std::vector<pRunner> runners;
      if (classId > 0) {
        oe->getRunners(classId, 0, runners, false);
        oe->sortRunners(ClassResult, runners);
      } else {
        oe->getRunners(0, 0, runners, false);
        oe->sortRunners(ClassResult, runners);
      }

      nlohmann::json arr = nlohmann::json::array();
      int place = 0;
      for (pRunner r : runners) {
        if (!r) continue;
        arr.push_back(runnerToResultJson(r, ++place));
      }
      result["results"] = std::move(arr);
    }

    return ApiResponse::ok(result.dump());
  });
}
