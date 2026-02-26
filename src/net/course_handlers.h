// SPDX-License-Identifier: GPL-3.0-or-later
// Course REST API endpoints.
// Registers GET/POST/PUT/DELETE handlers for /api/courses[/:id] on an ApiRouter.
#pragma once

#include "api_router.h"
#include "json_serializers.h"
#include "oEvent.h"
#include <string>

// Register course endpoints on the given router.
// `oe` must outlive the router (pointer is captured by handlers).
inline void registerCourseHandlers(ApiRouter &router, oEvent *oe) {

  // GET /api/courses – list all courses.
  router.get("/api/courses", [oe](const ApiRequest &) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");

    std::vector<pCourse> courses;
    oe->getCourses(courses);

    nlohmann::json arr = nlohmann::json::array();
    for (pCourse c : courses) {
      if (!c) continue;
      nlohmann::json j;
      to_json(j, *c);
      arr.push_back(std::move(j));
    }
    return ApiResponse::ok(arr.dump());
  });

  // GET /api/courses/:id – return a single course by id.
  router.get("/api/courses/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pCourse c = oe->getCourse(id);
    if (!c)
      return ApiResponse::notFound("Course not found");
    nlohmann::json j;
    to_json(j, *c);
    return ApiResponse::ok(j.dump());
  });

  // POST /api/courses – create a new course.
  // Body: {"name": "...", "length": 5000}
  router.post("/api/courses", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();
    if (!body.contains("name") || !body["name"].is_string())
      return ApiResponse::badRequest("Field 'name' is required");

    CourseDTO dto;
    from_json(body, dto);

    pCourse c = oe->addCourse(gdioutput::fromUTF8(dto.name), dto.length);
    if (!c)
      return ApiResponse::internalError("Failed to create course");

    nlohmann::json j;
    to_json(j, *c);
    return ApiResponse::created(j.dump());
  });

  // PUT /api/courses/:id – update an existing course.
  router.put("/api/courses/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pCourse c = oe->getCourse(id);
    if (!c)
      return ApiResponse::notFound("Course not found");
    if (!req.hasValidJsonBody())
      return ApiResponse::badRequest("Invalid JSON body");
    auto body = req.bodyJson();

    if (body.contains("name") && body["name"].is_string())
      c->setName(gdioutput::fromUTF8(body["name"].get<std::string>()));
    if (body.contains("length") && body["length"].is_number_integer())
      c->setLength(body["length"].get<int>());

    nlohmann::json j;
    to_json(j, *c);
    return ApiResponse::ok(j.dump());
  });

  // DELETE /api/courses/:id – remove a course.
  router.del("/api/courses/:id", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    auto it = req.pathParams.find("id");
    if (it == req.pathParams.end())
      return ApiResponse::badRequest("Missing id");
    int id = 0;
    try { id = std::stoi(it->second); } catch (...) {
      return ApiResponse::badRequest("Invalid id");
    }
    pCourse c = oe->getCourse(id);
    if (!c)
      return ApiResponse::notFound("Course not found");
    c->remove();
    return ApiResponse::noContent();
  });
}
