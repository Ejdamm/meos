// SPDX-License-Identifier: GPL-3.0-or-later
// Speaker REST API endpoints.
// Registers GET/PUT handlers for /api/speaker/config and /api/speaker/monitor on an ApiRouter.
#pragma once

#include "api_router.h"
#include "oEvent.h"
#include "oClass.h"
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <vector>

// In-process speaker configuration (per-server instance, reset on restart).
// Stores which class IDs the speaker monitor should track.
namespace {
  std::set<int> g_speakerClassIds;
  int           g_speakerWindowSeconds = 600; // Default 10 min look-back window
}

// Register speaker endpoints on the given router.
// `oe` must outlive the router (pointer is captured by handlers).
inline void registerSpeakerHandlers(ApiRouter &router, oEvent *oe) {

  // GET /api/speaker/config – return current speaker configuration.
  // Response: {"classIds":[...], "windowSeconds": N}
  router.get("/api/speaker/config", [oe](const ApiRequest &) -> ApiResponse {
    nlohmann::json j;
    j["classIds"]      = nlohmann::json::array();
    for (int id : g_speakerClassIds)
      j["classIds"].push_back(id);
    j["windowSeconds"] = g_speakerWindowSeconds;

    // Include class names when a competition is loaded.
    if (oe) {
      nlohmann::json classInfo = nlohmann::json::array();
      for (int id : g_speakerClassIds) {
        pClass cls = oe->getClass(id);
        if (cls) {
          nlohmann::json ci;
          ci["id"]   = id;
          ci["name"] = cls->getName();
          classInfo.push_back(std::move(ci));
        }
      }
      j["classes"] = std::move(classInfo);
    }
    return ApiResponse::ok(j.dump());
  });

  // PUT /api/speaker/config – update speaker configuration.
  // Body: {"classIds":[1,2,3], "windowSeconds": 600}
  router.put("/api/speaker/config", [](const ApiRequest &req) -> ApiResponse {
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();

    if (body.contains("classIds") && body["classIds"].is_array()) {
      g_speakerClassIds.clear();
      for (const auto &v : body["classIds"]) {
        if (v.is_number_integer())
          g_speakerClassIds.insert(v.get<int>());
      }
    }
    if (body.contains("windowSeconds") && body["windowSeconds"].is_number_integer()) {
      int ws = body["windowSeconds"].get<int>();
      if (ws > 0)
        g_speakerWindowSeconds = ws;
    }

    nlohmann::json j;
    j["classIds"]      = nlohmann::json::array();
    for (int id : g_speakerClassIds)
      j["classIds"].push_back(id);
    j["windowSeconds"] = g_speakerWindowSeconds;
    return ApiResponse::ok(j.dump());
  });

  // GET /api/speaker/monitor – return live timeline events for configured classes.
  // Optional query params: ?class=1,2,3 (override config), ?since=<epoch_seconds>
  // Response: {"currentTime": T, "events": [...]}
  router.get("/api/speaker/monitor", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");

    // Determine which classes to monitor (query param overrides config).
    std::set<int> classes = g_speakerClassIds;
    auto classIt = req.queryParams.find("class");
    if (classIt != req.queryParams.end()) {
      classes.clear();
      // Support comma-separated list: ?class=1,2,3
      const std::string &val = classIt->second;
      std::string buf;
      for (char c : val) {
        if (c == ',') {
          if (!buf.empty()) {
            try { classes.insert(std::stoi(buf)); } catch (...) {}
            buf.clear();
          }
        } else {
          buf += c;
        }
      }
      if (!buf.empty()) {
        try { classes.insert(std::stoi(buf)); } catch (...) {}
      }
    }

    // Fetch timeline events.
    std::vector<oTimeLine> events;
    std::set<__int64>      stored;
    int nextEvent = oe->getTimeLineEvents(classes, events, stored, 0);

    nlohmann::json evArr = nlohmann::json::array();
    for (const oTimeLine &ev : events) {
      nlohmann::json e;
      e["time"]     = ev.getTime();
      e["classId"]  = ev.getClassId();
      e["priority"] = static_cast<int>(ev.getPriority());

      // Type as string
      switch (ev.getType()) {
        case oTimeLine::TLTStart:    e["type"] = "start";    break;
        case oTimeLine::TLTFinish:   e["type"] = "finish";   break;
        case oTimeLine::TLTRadio:    e["type"] = "radio";    break;
        case oTimeLine::TLTExpected: e["type"] = "expected"; break;
        default:                     e["type"] = "unknown";  break;
      }

      // Runner or team source
      auto src = ev.getSource();
      if (src.first)
        e["teamId"]   = src.second;
      else
        e["runnerId"] = src.second;

      evArr.push_back(std::move(e));
    }

    nlohmann::json j;
    j["currentTime"]   = oe->getComputerTime();
    j["nextEventTime"] = nextEvent;
    j["events"]        = std::move(evArr);
    return ApiResponse::ok(j.dump());
  });
}
