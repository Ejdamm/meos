// SPDX-License-Identifier: GPL-3.0-or-later
// Class REST API endpoints.
// Registers GET/POST/PUT/DELETE handlers for /api/classes[/:id] on an ApiRouter.
#pragma once

#include "api_router.h"
#include "json_serializers.h"
#include "oEvent.h"
#include <string>

// Register class endpoints on the given router.
// `oe` must outlive the router (pointer is captured by handlers).
inline void registerClassHandlers(ApiRouter &router, oEvent *oe) {

  // GET /api/classes – list all classes.
  router.get("/api/classes", [oe](const ApiRequest &) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");

    std::vector<pClass> classes;
    oe->getClasses(classes, false);

    nlohmann::json arr = nlohmann::json::array();
    for (pClass c : classes) {
      if (!c) continue;
      nlohmann::json j;
      to_json(j, *c);
      arr.push_back(std::move(j));
    }
    return ApiResponse::ok(arr.dump());
  });

  // GET /api/classes/:id – return a single class by id.
  router.get("/api/classes/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pClass c = oe->getClass(id);
    if (!c)
      return ApiResponse::notFound("Class not found");
    nlohmann::json j;
    to_json(j, *c);
    return ApiResponse::ok(j.dump());
  });

  // POST /api/classes – create a new class.
  // Body: {"name": "...", "courseId": 1}
  router.post("/api/classes", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();
    if (!body.contains("name") || !body["name"].is_string())
      return ApiResponse::badRequest("Field 'name' is required");

    ClassDTO dto;
    from_json(body, dto);

    pClass c = oe->addClass(dto.name, dto.courseId);
    if (!c)
      return ApiResponse::internalError("Failed to create class");

    nlohmann::json j;
    to_json(j, *c);
    return ApiResponse::created(j.dump());
  });

  // PUT /api/classes/:id – update an existing class.
  router.put("/api/classes/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pClass c = oe->getClass(id);
    if (!c)
      return ApiResponse::notFound("Class not found");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();

    if (body.contains("name") && body["name"].is_string())
      c->setName(gdioutput::fromUTF8(body["name"].get<std::string>()), true);
    if (body.contains("courseId") && body["courseId"].is_number_integer()) {
      int courseId = body["courseId"].get<int>();
      pCourse course = oe->getCourse(courseId);
      c->setCourse(course);
    }
    if (body.contains("type") && body["type"].is_string())
      c->setType(gdioutput::fromUTF8(body["type"].get<std::string>()));

    nlohmann::json j;
    to_json(j, *c);
    return ApiResponse::ok(j.dump());
  });

  // DELETE /api/classes/:id – remove a class.
  router.del("/api/classes/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pClass c = oe->getClass(id);
    if (!c)
      return ApiResponse::notFound("Class not found");
    c->remove();
    return ApiResponse::noContent();
  });
}
