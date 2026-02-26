// SPDX-License-Identifier: GPL-3.0-or-later
// Static file serving with SPA fallback for the MeOS REST server.
// dispatchWithStatic() wraps ApiRouter: API routes take priority, then static
// files from webRoot are served, unknown paths fallback to index.html.
#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include "api_router.h"

namespace fs = std::filesystem;

// Returns the MIME type for a given file extension (with leading dot).
inline std::string guessMimeType(const std::string &ext) {
  if (ext == ".html" || ext == ".htm") return "text/html";
  if (ext == ".js" || ext == ".mjs")   return "application/javascript";
  if (ext == ".css")                   return "text/css";
  if (ext == ".json")                  return "application/json";
  if (ext == ".png")                   return "image/png";
  if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
  if (ext == ".gif")                   return "image/gif";
  if (ext == ".svg")                   return "image/svg+xml";
  if (ext == ".ico")                   return "image/x-icon";
  if (ext == ".woff")                  return "font/woff";
  if (ext == ".woff2")                 return "font/woff2";
  if (ext == ".ttf")                   return "font/ttf";
  if (ext == ".txt")                   return "text/plain";
  if (ext == ".xml")                   return "application/xml";
  if (ext == ".webmanifest")           return "application/manifest+json";
  return "application/octet-stream";
}

// Reads a file into `contents`. Returns true on success.
inline bool readFileContents(const fs::path &filePath, std::string &contents) {
  std::ifstream f(filePath, std::ios::binary);
  if (!f) return false;
  contents.assign(std::istreambuf_iterator<char>(f),
                  std::istreambuf_iterator<char>());
  return true;
}

// Dispatch a request: API router has priority. On 404, serve a static file
// from webRoot. Any path not matching a file falls back to index.html (SPA).
// Pass webRoot="" to disable static serving (returns the router's 404).
inline ApiResponse dispatchWithStatic(const ApiRouter &router,
                                      ApiRequest req,
                                      const std::string &webRoot) {
  // Let the API router handle /api/* routes first.
  ApiResponse resp = router.dispatch(req);
  if (resp.status != 404)
    return resp;

  // Only GET is served for static assets.
  if (req.method != "GET")
    return resp;

  if (webRoot.empty())
    return resp;

  // Resolve the file path inside webRoot, guarding against traversal.
  fs::path root = fs::path(webRoot).lexically_normal();
  std::string urlPath = req.path;
  if (urlPath.empty() || urlPath == "/")
    urlPath = "/index.html";

  // Strip leading slash to build a relative path.
  fs::path relPath = fs::path(urlPath.substr(1)).lexically_normal();
  // Reject any path that would escape the root.
  if (!relPath.empty() && relPath.begin()->string() == "..")
    relPath = fs::path("index.html");

  fs::path filePath = (root / relPath).lexically_normal();

  std::string body;
  if (readFileContents(filePath, body)) {
    std::string ext = filePath.extension().string();
    return ApiResponse::ok(std::move(body), guessMimeType(ext));
  }

  // SPA fallback: serve index.html for any unresolved path.
  if (readFileContents(root / "index.html", body))
    return ApiResponse::ok(std::move(body), "text/html");

  return ApiResponse::notFound();
}
