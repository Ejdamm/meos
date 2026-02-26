// SPDX-License-Identifier: GPL-3.0-or-later
// Runner REST API endpoints.
// Registers GET/POST/PUT/DELETE handlers for /api/runners[/:id] on an ApiRouter.
#pragma once

#include "api_router.h"
#include "json_serializers.h"
#include "oEvent.h"
#include <string>

// Register runner endpoints on the given router.
// `oe` must outlive the router (pointer is captured by handlers).
inline void registerRunnerHandlers(ApiRouter &router, oEvent *oe) {

  // GET /api/runners – list all runners, with optional filtering.
  // Query params: ?class=<classId>&club=<clubId>&card=<cardNo>
  router.get("/api/runners", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");

    // Collect all runners
    std::vector<pRunner> runners;
    oe->getRunners(0, 0, runners, false);

    // Apply optional filters
    int filterClass = 0, filterClub = 0, filterCard = 0;
    auto it = req.queryParams.find("class");
    if (it != req.queryParams.end()) {
      try { filterClass = std::stoi(it->second); } catch (...) {}
    }
    it = req.queryParams.find("club");
    if (it != req.queryParams.end()) {
      try { filterClub = std::stoi(it->second); } catch (...) {}
    }
    it = req.queryParams.find("card");
    if (it != req.queryParams.end()) {
      try { filterCard = std::stoi(it->second); } catch (...) {}
    }

    nlohmann::json arr = nlohmann::json::array();
    for (pRunner r : runners) {
      if (!r) continue;
      if (filterClass != 0 && r->getClassId(false) != filterClass) continue;
      if (filterClub  != 0 && r->getClubId()                != filterClub)  continue;
      if (filterCard  != 0 && r->getCardNo()                != filterCard)  continue;
      nlohmann::json j;
      to_json(j, *r);
      arr.push_back(std::move(j));
    }
    return ApiResponse::ok(arr.dump());
  });

  // GET /api/runners/:id – return a single runner by id.
  router.get("/api/runners/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pRunner r = oe->getRunner(id, 0);
    if (!r)
      return ApiResponse::notFound("Runner not found");
    nlohmann::json j;
    to_json(j, *r);
    return ApiResponse::ok(j.dump());
  });

  // POST /api/runners – create a new runner.
  // Body: {"name": "...", "classId": 1, "clubId": 2, "cardNo": 12345, "bib": "42"}
  router.post("/api/runners", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();
    if (!body.contains("name") || !body["name"].is_string())
      return ApiResponse::badRequest("Field 'name' is required");

    RunnerDTO dto;
    from_json(body, dto);

    pRunner r = oe->addRunner(dto.name, dto.clubId, dto.classId,
                              dto.cardNo, L"", false);
    if (!r)
      return ApiResponse::internalError("Failed to create runner");

    if (!dto.bib.empty())
      r->setBib(dto.bib, 0, false);
    if (dto.startTime > 0)
      r->setStartTime(dto.startTime, true, ChangeType::Update);
    if (dto.status != 0)
      r->setStatus(static_cast<RunnerStatus>(dto.status), true, ChangeType::Update);

    nlohmann::json j;
    to_json(j, *r);
    return ApiResponse::created(j.dump());
  });

  // PUT /api/runners/:id – update an existing runner.
  router.put("/api/runners/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pRunner r = oe->getRunner(id, 0);
    if (!r)
      return ApiResponse::notFound("Runner not found");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();

    if (body.contains("name") && body["name"].is_string())
      r->setName(gdioutput::fromUTF8(body["name"].get<std::string>()), true);
    if (body.contains("classId") && body["classId"].is_number_integer())
      r->setClassId(body["classId"].get<int>(), true);
    if (body.contains("clubId") && body["clubId"].is_number_integer())
      r->setClubId(body["clubId"].get<int>());
    if (body.contains("cardNo") && body["cardNo"].is_number_integer())
      r->setCardNo(body["cardNo"].get<int>(), true);
    if (body.contains("bib") && body["bib"].is_string()) {
      std::wstring bib = gdioutput::fromUTF8(body["bib"].get<std::string>());
      r->setBib(bib, 0, false);
    }
    if (body.contains("startTime") && body["startTime"].is_number_integer())
      r->setStartTime(body["startTime"].get<int>(), true, ChangeType::Update);
    if (body.contains("finishTime") && body["finishTime"].is_number_integer())
      r->setFinishTime(body["finishTime"].get<int>());
    if (body.contains("status") && body["status"].is_number_integer())
      r->setStatus(static_cast<RunnerStatus>(body["status"].get<int>()), true, ChangeType::Update);

    nlohmann::json j;
    to_json(j, *r);
    return ApiResponse::ok(j.dump());
  });

  // DELETE /api/runners/:id – remove a runner.
  router.del("/api/runners/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pRunner r = oe->getRunner(id, 0);
    if (!r)
      return ApiResponse::notFound("Runner not found");
    r->remove();
    return ApiResponse::noContent();
  });
}
