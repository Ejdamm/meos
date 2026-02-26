// SPDX-License-Identifier: GPL-3.0-or-later
// Automation REST API endpoints.
// Registers GET/POST/DELETE/status handlers for /api/automations on an ApiRouter.
// Automations are persisted via MachineContainer (keyed by type + name).
// Known automation types: onlineinput, onlineresults, printresult, backup,
//   splits, infoserver, punchtest, prewarning, reconnect.
#pragma once

#include "api_router.h"
#include "oEvent.h"
#include "machinecontainer.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace {

// Convert a MachineContainer::AbstractMachine's visible props to JSON.
// The AbstractMachine does not expose its internal map directly, so we represent
// each automation with its type and name as primary keys.
inline nlohmann::json automationToJson(const std::string &type,
                                       const std::wstring &name) {
  nlohmann::json j;
  j["type"] = type;
  j["name"] = gdioutput::toUTF8(name);
  // Composite id: "type/name" (clients must URL-encode the slash when used in path)
  j["id"] = type + "/" + gdioutput::toUTF8(name);
  return j;
}

// Decode a composite id string "type/name" → (type, wstring name).
// Returns false if the format is invalid.
inline bool decodeAutomationId(const std::string &id, std::string &type,
                                std::wstring &name) {
  auto slash = id.find('/');
  if (slash == std::string::npos || slash == 0 || slash == id.size() - 1)
    return false;
  type = id.substr(0, slash);
  std::string nameUtf8 = id.substr(slash + 1);
  name = gdioutput::fromUTF8(nameUtf8);
  return true;
}

} // namespace

// Register automation endpoints on the given router.
// `oe` must outlive the router (pointer is captured by handlers).
inline void registerAutomationHandlers(ApiRouter &router, oEvent *oe) {

  // GET /api/automations – list all persisted automation configurations.
  // Response: [{"id": "onlineresults/default", "type": "onlineresults", "name": "default"}, ...]
  router.get("/api/automations", [oe](const ApiRequest &) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");
    auto entries = oe->getMachineContainer().enumerate();
    nlohmann::json arr = nlohmann::json::array();
    for (const auto &e : entries) {
      arr.push_back(automationToJson(e.first, e.second));
    }
    return ApiResponse::ok(arr.dump());
  });

  // POST /api/automations – create or overwrite an automation configuration.
  // Body: {"type": "onlineresults", "name": "default", "props": {"url": "...", "interval": "60"}}
  // The "name" field defaults to "default" if omitted.
  router.post("/api/automations", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();
    if (!body.contains("type") || !body["type"].is_string())
      return ApiResponse::badRequest("Field 'type' is required");

    std::string type = body["type"].get<std::string>();
    // Validate known types.
    static const std::vector<std::string> knownTypes = {
      "onlineinput", "onlineresults", "printresult", "backup",
      "splits", "infoserver", "punchtest", "prewarning", "reconnect"
    };
    bool valid = false;
    for (const auto &t : knownTypes)
      if (t == type) { valid = true; break; }
    if (!valid)
      return ApiResponse::badRequest("Unknown automation type: " + type);

    std::wstring name = L"default";
    if (body.contains("name") && body["name"].is_string())
      name = gdioutput::fromUTF8(body["name"].get<std::string>());

    auto &machine = oe->getMachineContainer().set(type, name);

    // Apply optional props.
    if (body.contains("props") && body["props"].is_object()) {
      for (auto &[key, val] : body["props"].items()) {
        if (val.is_string())
          machine.set(key, gdioutput::fromUTF8(val.get<std::string>()));
        else if (val.is_number_integer())
          machine.set(key, val.get<int>());
      }
    }

    nlohmann::json j = automationToJson(type, name);
    return ApiResponse::created(j.dump());
  });

  // DELETE /api/automations/:id – remove a persisted automation configuration.
  // :id is the composite "type/name" (URL-encoded as needed by the client).
  router.del("/api/automations/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");

    std::string type;
    std::wstring name;
    if (!decodeAutomationId(it->second, type, name))
      return ApiResponse::badRequest("Invalid id format; expected 'type/name'");

    const auto *existing = oe->getMachineContainer().get(type, name);
    if (!existing)
      return ApiResponse::notFound("Automation not found");

    oe->getMachineContainer().erase(type, name);
    return ApiResponse::noContent();
  });

  // GET /api/automations/:id/status – return status of a persisted automation.
  // Since runtime status (isRunning, lastError) is held by AutoMachine in the UI layer,
  // this endpoint reports whether the config exists and its stored properties.
  // Response: {"id": "...", "type": "...", "name": "...", "configured": true}
  router.get("/api/automations/:id/status", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");

    std::string type;
    std::wstring name;
    if (!decodeAutomationId(it->second, type, name))
      return ApiResponse::badRequest("Invalid id format; expected 'type/name'");

    const auto *existing = oe->getMachineContainer().get(type, name);
    if (!existing)
      return ApiResponse::notFound("Automation not found");

    nlohmann::json j = automationToJson(type, name);
    j["configured"] = true;
    return ApiResponse::ok(j.dump());
  });
}
