// SPDX-License-Identifier: GPL-3.0-or-later
// Competition REST API endpoints.
// Registers GET/POST/PUT handlers for /api/competitions[/:id] on an ApiRouter.
#pragma once

#include "api_router.h"
#include "json_serializers.h"
#include "oEvent.h"
#include <string>

// Register competition endpoints on the given router.
// `oe` must outlive the router (pointer is captured by handlers).
inline void registerCompetitionHandlers(ApiRouter &router, oEvent *oe) {

  // GET /api/competitions – return the current competition as a single-element array.
  // (MeOS manages one competition at a time; the list always contains that one entry.)
  router.get("/api/competitions", [oe](const ApiRequest &) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");
    nlohmann::json j;
    to_json(j, *oe);
    nlohmann::json arr = nlohmann::json::array({j});
    return ApiResponse::ok(arr.dump());
  });

  // GET /api/competitions/:id – return the current competition if its id matches.
  router.get("/api/competitions/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    try {
      int id = std::stoi(it->second);
      if (id != oe->getId())
        return ApiResponse::notFound("Competition not found");
    } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    nlohmann::json j;
    to_json(j, *oe);
    return ApiResponse::ok(j.dump());
  });

  // POST /api/competitions – create (reset) a competition with a given name.
  // Body: {"name": "My Competition", "date": "2024-06-01", "zeroTime": "10:00:00"}
  router.post("/api/competitions", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();
    if (!body.contains("name") || !body["name"].is_string())
      return ApiResponse::badRequest("Field 'name' is required");

    std::string nameUtf8 = body["name"].get<std::string>();
    std::wstring name = gdioutput::fromUTF8(nameUtf8);

    oe->newCompetition(name);

    if (body.contains("date") && body["date"].is_string()) {
      std::wstring date = gdioutput::fromUTF8(body["date"].get<std::string>());
      oe->setDate(date, true);
    }
    if (body.contains("zeroTime") && body["zeroTime"].is_string()) {
      std::wstring zt = gdioutput::fromUTF8(body["zeroTime"].get<std::string>());
      oe->setZeroTime(zt, true);
    }

    nlohmann::json j;
    to_json(j, *oe);
    return ApiResponse::created(j.dump());
  });

  // PUT /api/competitions/:id – update name/date/zeroTime of the current competition.
  router.put("/api/competitions/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    try {
      int id = std::stoi(it->second);
      if (id != oe->getId())
        return ApiResponse::notFound("Competition not found");
    } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();

    if (body.contains("name") && body["name"].is_string()) {
      std::wstring name = gdioutput::fromUTF8(body["name"].get<std::string>());
      oe->setName(name, true);
    }
    if (body.contains("date") && body["date"].is_string()) {
      std::wstring date = gdioutput::fromUTF8(body["date"].get<std::string>());
      oe->setDate(date, true);
    }
    if (body.contains("zeroTime") && body["zeroTime"].is_string()) {
      std::wstring zt = gdioutput::fromUTF8(body["zeroTime"].get<std::string>());
      oe->setZeroTime(zt, true);
    }

    nlohmann::json j;
    to_json(j, *oe);
    return ApiResponse::ok(j.dump());
  });
}
