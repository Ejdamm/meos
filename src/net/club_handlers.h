// SPDX-License-Identifier: GPL-3.0-or-later
// Club REST API endpoints.
// Registers GET/POST/PUT/DELETE handlers for /api/clubs[/:id] on an ApiRouter.
#pragma once

#include "api_router.h"
#include "json_serializers.h"
#include "oEvent.h"
#include <string>

// Register club endpoints on the given router.
// `oe` must outlive the router (pointer is captured by handlers).
inline void registerClubHandlers(ApiRouter &router, oEvent *oe) {

  // GET /api/clubs – list all clubs.
  router.get("/api/clubs", [oe](const ApiRequest &) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");

    std::vector<pClub> clubs;
    oe->getClubs(clubs, false);

    nlohmann::json arr = nlohmann::json::array();
    for (pClub c : clubs) {
      if (!c) continue;
      nlohmann::json j;
      to_json(j, *c);
      arr.push_back(std::move(j));
    }
    return ApiResponse::ok(arr.dump());
  });

  // GET /api/clubs/:id – return a single club by id.
  router.get("/api/clubs/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pClub c = oe->getClub(id);
    if (!c)
      return ApiResponse::notFound("Club not found");
    nlohmann::json j;
    to_json(j, *c);
    return ApiResponse::ok(j.dump());
  });

  // POST /api/clubs – create a new club.
  // Body: {"name": "...", "country": "SWE"}
  router.post("/api/clubs", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();
    if (!body.contains("name") || !body["name"].is_string())
      return ApiResponse::badRequest("Field 'name' is required");

    ClubDTO dto;
    from_json(body, dto);

    pClub c = oe->addClub(dto.name);
    if (!c)
      return ApiResponse::internalError("Failed to create club");

    if (!dto.country.empty())
      c->getDCI().setString("Country", dto.country);

    nlohmann::json j;
    to_json(j, *c);
    return ApiResponse::created(j.dump());
  });

  // PUT /api/clubs/:id – update an existing club.
  // Body: {"name": "...", "country": "SWE"}
  router.put("/api/clubs/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pClub c = oe->getClub(id);
    if (!c)
      return ApiResponse::notFound("Club not found");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();

    if (body.contains("name") && body["name"].is_string())
      c->setName(gdioutput::fromUTF8(body["name"].get<std::string>()));
    if (body.contains("country") && body["country"].is_string())
      c->getDCI().setString("Country", gdioutput::fromUTF8(body["country"].get<std::string>()));

    nlohmann::json j;
    to_json(j, *c);
    return ApiResponse::ok(j.dump());
  });

  // DELETE /api/clubs/:id – remove a club by id.
  router.del("/api/clubs/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    if (!oe->getClub(id))
      return ApiResponse::notFound("Club not found");

    oe->removeClub(id);
    return ApiResponse::noContent();
  });
}
