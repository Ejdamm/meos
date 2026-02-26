// SPDX-License-Identifier: GPL-3.0-or-later
// Card and SI-punch REST API endpoints.
// Registers GET handlers for /api/cards[/:id] and /api/punches,
// plus POST /api/cards/read for simulating a card read-out.
#pragma once

#include "api_router.h"
#include "json_serializers.h"
#include "oEvent.h"
#include <string>

// Register card and punch endpoints on the given router.
// `oe` must outlive the router (pointer is captured by handlers).
inline void registerCardHandlers(ApiRouter &router, oEvent *oe) {

  // GET /api/cards -- list all cards.
  router.get("/api/cards", [oe](const ApiRequest &) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");

    std::vector<pCard> cards;
    oe->getCards(cards, false, false);

    nlohmann::json arr = nlohmann::json::array();
    for (pCard c : cards) {
      if (!c) continue;
      nlohmann::json j;
      to_json(j, *c);
      arr.push_back(std::move(j));
    }
    return ApiResponse::ok(arr.dump());
  });

  // GET /api/cards/:id -- return a single card by id.
  router.get("/api/cards/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pCard c = oe->getCard(id);
    if (!c)
      return ApiResponse::notFound("Card not found");
    nlohmann::json j;
    to_json(j, *c);
    return ApiResponse::ok(j.dump());
  });

  // POST /api/cards/read -- simulate a card read-out and store as free punches.
  // Body: {"cardNo": 12345, "punches": [{"type": 0, "time": 3600, "controlId": 31}, ...]}
  router.post("/api/cards/read", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();
    if (!body.contains("cardNo") || !body["cardNo"].is_number_integer())
      return ApiResponse::badRequest("Field 'cardNo' is required");

    int cardNo = body["cardNo"].get<int>();

    // Create an oCard and populate it from the request body.
    oCard newCard(oe);
    newCard.setCardNo(cardNo);

    if (body.contains("punches") && body["punches"].is_array()) {
      for (const auto &p : body["punches"]) {
        int type = p.value("type", 0);
        int time = p.value("time", 0);
        int unit = p.value("controlId", 0);
        // Add each punch as a free punch attached to this card.
        oe->addFreePunch(time, type, unit, cardNo, true, true);
      }
    }

    pCard stored = oe->addCard(newCard);
    if (!stored)
      return ApiResponse::internalError("Failed to store card");

    nlohmann::json j;
    to_json(j, *stored);
    return ApiResponse::created(j.dump());
  });

  // GET /api/punches -- list free punches; optional ?runner=<runnerId> filter.
  router.get("/api/punches", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");

    nlohmann::json arr = nlohmann::json::array();

    auto runnerIt = req.queryParams.find("runner");
    if (runnerIt != req.queryParams.end()) {
      int runnerId = 0;
      try { runnerId = std::stoi(runnerIt->second); } catch (...) {
        return ApiResponse::badRequest("Invalid runner id");
      }
      std::vector<pFreePunch> punches;
      oe->getPunchesForRunner(runnerId, true, punches);
      for (pFreePunch p : punches) {
        if (!p) continue;
        nlohmann::json pj;
        to_json(pj, *p);
        arr.push_back(std::move(pj));
      }
    }
    return ApiResponse::ok(arr.dump());
  });
}
