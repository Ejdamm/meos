// SPDX-License-Identifier: GPL-3.0-or-later
// Control REST API endpoints.
// Registers GET/POST/PUT/DELETE handlers for /api/controls[/:id] on an ApiRouter.
#pragma once

#include "api_router.h"
#include "json_serializers.h"
#include "oEvent.h"
#include <string>

// Register control endpoints on the given router.
// `oe` must outlive the router (pointer is captured by handlers).
inline void registerControlHandlers(ApiRouter &router, oEvent *oe) {

  // GET /api/controls – list all controls.
  router.get("/api/controls", [oe](const ApiRequest &) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");

    std::vector<pControl> controls;
    oe->getControls(controls, false);

    nlohmann::json arr = nlohmann::json::array();
    for (pControl c : controls) {
      if (!c) continue;
      nlohmann::json j;
      to_json(j, *c);
      arr.push_back(std::move(j));
    }
    return ApiResponse::ok(arr.dump());
  });

  // GET /api/controls/:id – return a single control by id.
  router.get("/api/controls/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pControl c = oe->getControl(id);
    if (!c)
      return ApiResponse::notFound("Control not found");
    nlohmann::json j;
    to_json(j, *c);
    return ApiResponse::ok(j.dump());
  });

  // POST /api/controls – create a new control.
  // Body: {"name": "...", "codes": [31, 32]}
  router.post("/api/controls", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();
    if (!body.contains("name") || !body["name"].is_string())
      return ApiResponse::badRequest("Field 'name' is required");

    ControlDTO dto;
    from_json(body, dto);

    // Use first code as the control number (0 if no codes provided).
    int number = dto.codes.empty() ? 0 : dto.codes[0];
    pControl c = oe->addControl(0, number, dto.name);
    if (!c)
      return ApiResponse::internalError("Failed to create control");

    // Apply all codes via setNumbers if more than one code.
    if (dto.codes.size() > 1) {
      std::wstring codeStr;
      for (size_t i = 0; i < dto.codes.size(); ++i) {
        if (i) codeStr += L';';
        codeStr += std::to_wstring(dto.codes[i]);
      }
      c->setNumbers(codeStr);
    }

    nlohmann::json j;
    to_json(j, *c);
    return ApiResponse::created(j.dump());
  });

  // PUT /api/controls/:id – update an existing control.
  router.put("/api/controls/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pControl c = oe->getControl(id);
    if (!c)
      return ApiResponse::notFound("Control not found");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();

    if (body.contains("name") && body["name"].is_string())
      c->setName(gdioutput::fromUTF8(body["name"].get<std::string>()));
    if (body.contains("status") && body["status"].is_number_integer())
      c->setStatus(static_cast<oControl::ControlStatus>(body["status"].get<int>()));
    if (body.contains("codes") && body["codes"].is_array()) {
      std::wstring codeStr;
      bool first = true;
      for (auto &code : body["codes"]) {
        if (!first) codeStr += L';';
        first = false;
        codeStr += std::to_wstring(code.get<int>());
      }
      c->setNumbers(codeStr);
    }

    nlohmann::json j;
    to_json(j, *c);
    return ApiResponse::ok(j.dump());
  });

  // DELETE /api/controls/:id – remove a control.
  router.del("/api/controls/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pControl c = oe->getControl(id);
    if (!c)
      return ApiResponse::notFound("Control not found");
    c->remove();
    return ApiResponse::noContent();
  });
}
