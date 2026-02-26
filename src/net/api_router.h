// SPDX-License-Identifier: GPL-3.0-or-later
// API router: maps URL patterns like /api/runners/:id to handler functions.
// Supports path parameters (:name segments) and query parameters.
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <stdexcept>
#include <nlohmann/json.hpp>

struct ApiRequest {
  std::string method;                              // "GET", "POST", etc.
  std::string path;                                // e.g. "/api/runners/42"
  std::map<std::string, std::string> pathParams;   // e.g. {"id": "42"}
  std::multimap<std::string, std::string> queryParams; // from ?key=val&...
  std::map<std::string, std::string> headers;      // request headers (lowercase keys)
  std::string body;                                // request body (for POST/PUT)

  // Parses the request body as JSON. Returns a null JSON value on parse error.
  nlohmann::json bodyJson() const {
    if (body.empty())
      return nlohmann::json{};
    try {
      return nlohmann::json::parse(body);
    } catch (...) {
      return nlohmann::json{};
    }
  }

  // Returns true if the body is valid JSON (non-empty).
  bool hasValidJsonBody() const {
    if (body.empty()) return false;
    try {
      nlohmann::json::parse(body);
      return true;
    } catch (...) {
      return false;
    }
  }

  // Returns the preferred response content type based on the Accept header.
  // Supports "application/json" and "text/xml". Defaults to "application/json".
  std::string negotiateContentType() const {
    auto it = headers.find("accept");
    if (it == headers.end())
      return "application/json";
    const std::string &accept = it->second;
    // Check if text/xml is explicitly preferred (appears before application/json or json absent)
    auto jsonPos = accept.find("application/json");
    auto xmlPos  = accept.find("text/xml");
    bool hasJson = (jsonPos != std::string::npos);
    bool hasXml  = (xmlPos  != std::string::npos);
    bool hasAny  = (accept.find("*/*") != std::string::npos);
    if (!hasJson && !hasXml) {
      // No recognised type; default to JSON unless */* is present
      return "application/json";
    }
    if (hasXml && !hasJson)
      return "text/xml";
    if (hasJson && !hasXml)
      return "application/json";
    // Both present: prefer the one listed first
    return (xmlPos < jsonPos) ? "text/xml" : "application/json";
  }
};

struct ApiResponse {
  int status = 200;
  std::string contentType = "application/json";
  std::string body;

  static ApiResponse ok(std::string body, std::string ct = "application/json") {
    return {200, std::move(ct), std::move(body)};
  }
  static ApiResponse notFound(std::string msg = "Not found") {
    return {404, "application/json", "{\"error\":\"" + msg + "\"}"};
  }
  static ApiResponse badRequest(std::string msg = "Bad request") {
    return {400, "application/json", "{\"error\":\"" + msg + "\"}"};
  }
  static ApiResponse internalError(std::string msg = "Internal error") {
    return {500, "application/json", "{\"error\":\"" + msg + "\"}"};
  }
  static ApiResponse methodNotAllowed() {
    return {405, "application/json", "{\"error\":\"Method not allowed\"}"};
  }
  static ApiResponse created(std::string body, std::string ct = "application/json") {
    return {201, std::move(ct), std::move(body)};
  }
  static ApiResponse noContent() {
    return {204, "application/json", ""};
  }
};

using ApiHandler = std::function<ApiResponse(const ApiRequest &)>;

// Splits a path like "/api/runners/42" into ["api","runners","42"].
static inline std::vector<std::string> splitPath(const std::string &path) {
  std::vector<std::string> parts;
  std::string::size_type start = 0;
  if (!path.empty() && path[0] == '/')
    start = 1;
  while (start < path.size()) {
    auto end = path.find('/', start);
    if (end == std::string::npos) end = path.size();
    parts.push_back(path.substr(start, end - start));
    start = end + 1;
  }
  return parts;
}

class ApiRouter {
  struct Route {
    std::string method;                     // uppercase, empty = any
    std::vector<std::string> patternParts;  // e.g. ["api","runners",":id"]
    ApiHandler handler;
  };

  std::vector<Route> routes_;

  // Tries to match actualParts against patternParts.
  // On success, fills pathParams and returns true.
  static bool matchParts(const std::vector<std::string> &pattern,
                         const std::vector<std::string> &actual,
                         std::map<std::string, std::string> &params) {
    if (pattern.size() != actual.size())
      return false;
    for (size_t i = 0; i < pattern.size(); ++i) {
      if (!pattern[i].empty() && pattern[i][0] == ':') {
        params[pattern[i].substr(1)] = actual[i];
      } else if (pattern[i] != actual[i]) {
        return false;
      }
    }
    return true;
  }

public:
  // Register a route for a specific method, e.g. addRoute("GET", "/api/runners/:id", handler)
  void addRoute(const std::string &method, const std::string &pattern, ApiHandler handler) {
    Route r;
    r.method = method;
    r.patternParts = splitPath(pattern);
    r.handler = std::move(handler);
    routes_.push_back(std::move(r));
  }

  // Convenience helpers
  void get(const std::string &pattern, ApiHandler h) { addRoute("GET", pattern, std::move(h)); }
  void post(const std::string &pattern, ApiHandler h) { addRoute("POST", pattern, std::move(h)); }
  void put(const std::string &pattern, ApiHandler h) { addRoute("PUT", pattern, std::move(h)); }
  void del(const std::string &pattern, ApiHandler h) { addRoute("DELETE", pattern, std::move(h)); }

  // Dispatch a request. Returns 404 if no route matches, 405 if path matches but method does not.
  ApiResponse dispatch(ApiRequest req) const {
    auto actualParts = splitPath(req.path);
    std::map<std::string, std::string> params;
    bool pathMatched = false;

    for (const auto &route : routes_) {
      params.clear();
      if (!matchParts(route.patternParts, actualParts, params))
        continue;
      // Path matched
      pathMatched = true;
      if (!route.method.empty() && route.method != req.method)
        continue;
      // Method matched
      req.pathParams = std::move(params);
      return route.handler(req);
    }

    if (pathMatched)
      return ApiResponse::methodNotAllowed();
    return ApiResponse::notFound();
  }

  bool handles(const std::string &path) const {
    auto actualParts = splitPath(path);
    std::map<std::string, std::string> params;
    for (const auto &route : routes_) {
      params.clear();
      if (matchParts(route.patternParts, actualParts, params))
        return true;
    }
    return false;
  }
};
