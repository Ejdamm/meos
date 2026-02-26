// SPDX-License-Identifier: GPL-3.0-or-later
// Team REST API endpoints.
// Registers GET/POST/PUT/DELETE handlers for /api/teams[/:id] on an ApiRouter.
#pragma once

#include "api_router.h"
#include "json_serializers.h"
#include "oEvent.h"
#include <string>

// Register team endpoints on the given router.
// `oe` must outlive the router (pointer is captured by handlers).
inline void registerTeamHandlers(ApiRouter &router, oEvent *oe) {

  // GET /api/teams – list all teams, with optional filtering.
  // Query params: ?class=<classId>&club=<clubId>
  router.get("/api/teams", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");

    int filterClass = 0, filterClub = 0;
    auto it = req.queryParams.find("class");
    if (it != req.queryParams.end()) {
      try { filterClass = std::stoi(it->second); } catch (...) {}
    }
    it = req.queryParams.find("club");
    if (it != req.queryParams.end()) {
      try { filterClub = std::stoi(it->second); } catch (...) {}
    }

    std::vector<pTeam> teams;
    oe->getTeams(filterClass, teams);

    nlohmann::json arr = nlohmann::json::array();
    for (pTeam t : teams) {
      if (!t) continue;
      if (filterClub != 0 && t->getClubId() != filterClub) continue;
      nlohmann::json j;
      to_json(j, *t);
      arr.push_back(std::move(j));
    }
    return ApiResponse::ok(arr.dump());
  });

  // GET /api/teams/:id – return a single team by id.
  router.get("/api/teams/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pTeam t = oe->getTeam(id);
    if (!t)
      return ApiResponse::notFound("Team not found");
    nlohmann::json j;
    to_json(j, *t);
    return ApiResponse::ok(j.dump());
  });

  // POST /api/teams – create a new team.
  // Body: {"name": "...", "classId": 1, "clubId": 2, "bib": "42"}
  router.post("/api/teams", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();
    if (!body.contains("name") || !body["name"].is_string())
      return ApiResponse::badRequest("Field 'name' is required");

    TeamDTO dto;
    from_json(body, dto);

    pTeam t = oe->addTeam(dto.name, dto.clubId, dto.classId);
    if (!t)
      return ApiResponse::internalError("Failed to create team");

    if (!dto.bib.empty())
      t->setBib(dto.bib, 0, false);
    if (dto.startTime > 0)
      t->setStartTime(dto.startTime, true, ChangeType::Update);
    if (dto.status != 0)
      t->setStatus(static_cast<RunnerStatus>(dto.status), true, ChangeType::Update);

    nlohmann::json j;
    to_json(j, *t);
    return ApiResponse::created(j.dump());
  });

  // PUT /api/teams/:id – update an existing team.
  router.put("/api/teams/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pTeam t = oe->getTeam(id);
    if (!t)
      return ApiResponse::notFound("Team not found");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();

    if (body.contains("name") && body["name"].is_string())
      t->setName(gdioutput::fromUTF8(body["name"].get<std::string>()), true);
    if (body.contains("classId") && body["classId"].is_number_integer())
      t->setClassId(body["classId"].get<int>(), true);
    if (body.contains("clubId") && body["clubId"].is_number_integer())
      t->setClubId(body["clubId"].get<int>());
    if (body.contains("bib") && body["bib"].is_string()) {
      std::wstring bib = gdioutput::fromUTF8(body["bib"].get<std::string>());
      t->setBib(bib, 0, false);
    }
    if (body.contains("startTime") && body["startTime"].is_number_integer())
      t->setStartTime(body["startTime"].get<int>(), true, ChangeType::Update);
    if (body.contains("status") && body["status"].is_number_integer())
      t->setStatus(static_cast<RunnerStatus>(body["status"].get<int>()), true, ChangeType::Update);

    nlohmann::json j;
    to_json(j, *t);
    return ApiResponse::ok(j.dump());
  });

  // DELETE /api/teams/:id – remove a team.
  router.del("/api/teams/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pTeam t = oe->getTeam(id);
    if (!t)
      return ApiResponse::notFound("Team not found");
    t->remove();
    return ApiResponse::noContent();
  });
}
