// SPDX-License-Identifier: GPL-3.0-or-later
// Import/Export REST API endpoints.
// Registers handlers for /api/import/* and /api/export/* on an ApiRouter.
//
// GET  /api/export/iof?type=startlist|results|event|class|clubs
//   Returns IOF 2.0 XML for the requested data type.
// POST /api/import/iof?updateClass=true&clear=false
//   Body: IOF XML string. Imports entry/start/result data.
// GET  /api/export/csv?includeSplits=false
//   Returns OE-CSV for all runners.
// POST /api/import/csv?clear=false
//   Body: OE-CSV string. Imports competitor data.
#pragma once

#include "api_router.h"
#include "oEvent.h"
#include "xmlparser.h"
#include "gdioutput.h"
#include <filesystem>
#include <fstream>
#include <set>
#include <string>

namespace fs = std::filesystem;

// Write a UTF-8 string to a temporary file.
// Returns the path to the temp file on success, or an empty string on failure.
static inline std::wstring writeTempFile(const std::string &content) {
  extern wstring getTempFile();
  std::wstring path = getTempFile();
  std::ofstream f(fs::path(path), std::ios::binary);
  if (!f.is_open())
    return L"";
  f.write(content.data(), content.size());
  return f.good() ? path : L"";
}

// Read entire file into a std::string. Returns empty string on failure.
static inline std::string readFileContents(const std::wstring &path) {
  std::ifstream f(fs::path(path), std::ios::binary);
  if (!f.is_open())
    return {};
  return std::string(std::istreambuf_iterator<char>(f),
                     std::istreambuf_iterator<char>());
}

// Register import/export endpoints on the given router.
// `oe`  must outlive the router (pointer is captured by handlers).
// `gdi` is used as a sink for progress messages during XML import.
inline void registerImportExportHandlers(ApiRouter &router, oEvent *oe,
                                         gdioutput *gdi) {

  // -----------------------------------------------------------------------
  // GET /api/export/iof  – export IOF 2.0 XML
  // Query params:
  //   type = startlist | results | event | class | clubs   (default: results)
  // -----------------------------------------------------------------------
  router.get("/api/export/iof", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");

    std::string type = "results";
    auto it = req.queryParams.find("type");
    if (it != req.queryParams.end())
      type = it->second;

    xmlparser xml;
    xml.openMemoryOutput(false);

    try {
      if (type == "startlist") {
        oe->exportIOFStartlist(xml);
      } else if (type == "event") {
        oe->exportIOFEvent(xml);
      } else if (type == "class") {
        oe->exportIOFClass(xml);
      } else if (type == "clubs") {
        oe->exportIOFClublist(xml);
      } else {
        // default: results
        std::set<int> allClasses;
        oe->exportIOFResults(xml, true, allClasses, -1, false);
      }
    } catch (const std::exception &ex) {
      return ApiResponse::internalError(std::string("Export failed: ") + ex.what());
    }

    std::string xmlStr;
    xml.getMemoryOutput(xmlStr);

    return ApiResponse::ok(xmlStr, "application/xml; charset=utf-8");
  });

  // -----------------------------------------------------------------------
  // POST /api/import/iof  – import IOF entry/start data from XML body
  // Query params:
  //   updateClass         = true|false  (default: false)
  //   removeNonexisting   = true|false  (default: false)
  // Body: IOF XML string (UTF-8)
  // -----------------------------------------------------------------------
  router.post("/api/import/iof", [oe, gdi](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    if (req.body.empty())
      return ApiResponse::badRequest("Empty request body");

    bool updateClass = false;
    bool removeNonexisting = false;
    {
      auto u = req.queryParams.find("updateClass");
      if (u != req.queryParams.end())
        updateClass = (u->second == "true" || u->second == "1");
      auto r = req.queryParams.find("removeNonexisting");
      if (r != req.queryParams.end())
        removeNonexisting = (r->second == "true" || r->second == "1");
    }

    std::wstring tmpPath = writeTempFile(req.body);
    if (tmpPath.empty())
      return ApiResponse::internalError("Failed to create temporary file");

    // Use a local stub gdioutput if none provided (sink for progress messages).
    gdioutput localGdi("import_sink", 1.0);
    gdioutput &gdiRef = gdi ? *gdi : localGdi;

    try {
      std::set<int> noFilter;
      std::pair<std::string, std::string> noType;
      std::shared_ptr<MapData> readMapData;
      oe->importXML_EntryData(gdiRef, tmpPath, updateClass, removeNonexisting,
                              noFilter, 0, 0, noType, readMapData);
    } catch (const std::exception &ex) {
      std::filesystem::remove(tmpPath);
      return ApiResponse::internalError(std::string("Import failed: ") + ex.what());
    }
    std::filesystem::remove(tmpPath);

    nlohmann::json j;
    j["imported"] = true;
    j["message"] = "IOF XML import completed";
    return ApiResponse::ok(j.dump());
  });

  // -----------------------------------------------------------------------
  // GET /api/export/csv  – export OE-CSV
  // Query params:
  //   includeSplits = true|false  (default: false)
  // -----------------------------------------------------------------------
  router.get("/api/export/csv", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::notFound("No competition loaded");

    bool includeSplits = false;
    auto it = req.queryParams.find("includeSplits");
    if (it != req.queryParams.end())
      includeSplits = (it->second == "true" || it->second == "1");

    extern wstring getTempFile();
    std::wstring tmpPath = getTempFile();
    std::set<int> allClasses;
    bool ok = false;
    try {
      ok = oe->exportOECSV(tmpPath.c_str(), allClasses, 0, includeSplits);
    } catch (const std::exception &ex) {
      std::filesystem::remove(tmpPath);
      return ApiResponse::internalError(std::string("CSV export failed: ") + ex.what());
    }
    if (!ok) {
      std::filesystem::remove(tmpPath);
      return ApiResponse::internalError("CSV export returned false");
    }

    std::string csvStr = readFileContents(tmpPath);
    std::filesystem::remove(tmpPath);

    return ApiResponse::ok(csvStr, "text/csv; charset=utf-8");
  });

  // -----------------------------------------------------------------------
  // POST /api/import/csv  – import OE-CSV competitor data
  // Query params:
  //   clear = true|false  (default: false, appends/updates)
  // Body: OE-CSV string (UTF-8)
  // -----------------------------------------------------------------------
  router.post("/api/import/csv", [oe](const ApiRequest &req) -> ApiResponse {
    if (!oe)
      return ApiResponse::internalError("No competition context");
    if (req.body.empty())
      return ApiResponse::badRequest("Empty request body");

    bool clear = false;
    auto it = req.queryParams.find("clear");
    if (it != req.queryParams.end())
      clear = (it->second == "true" || it->second == "1");

    std::wstring tmpPath = writeTempFile(req.body);
    if (tmpPath.empty())
      return ApiResponse::internalError("Failed to create temporary file");

    try {
      oe->importOECSV_Data(tmpPath, clear);
    } catch (const std::exception &ex) {
      std::filesystem::remove(tmpPath);
      return ApiResponse::internalError(std::string("CSV import failed: ") + ex.what());
    }
    std::filesystem::remove(tmpPath);

    nlohmann::json j;
    j["imported"] = true;
    j["message"] = "CSV import completed";
    return ApiResponse::ok(j.dump());
  });
}
