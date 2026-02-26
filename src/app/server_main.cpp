// server_main.cpp
// Entry point for the MeOS-server headless build.
// Starts the REST server without any Win32 UI dependencies.
#ifdef MEOS_SERVER

#include <cstdio>
#include <cstdlib>
#include <string>
#include <csignal>
#include <thread>
#include <chrono>

#include "stdafx.h"
#include "meos_util.h"
#include "oEvent.h"
#include "NullNotifier.h"
#include "gdioutput.h"
#include "restserver.h"
#include "platform_string.h"
#include "localizer.h"

// -----------------------------------------------------------------------
// Global variables required by domain/io/net code
// -----------------------------------------------------------------------
int defaultCodePage = 1252;
Localizer lang;
gdioutput *gdi_main = nullptr;
oEvent *gEvent = nullptr;

#ifdef _WIN32
wchar_t exePath[MAX_PATH] = {};
wchar_t programPath[MAX_PATH] = {};
HWND hWndMain = nullptr;
HWND hWndWorkspace = nullptr;
#endif

// -----------------------------------------------------------------------
// getUserFile / getTempFile stubs
// -----------------------------------------------------------------------
bool getUserFile(wchar_t *fileNamePath, const wchar_t *fileName) {
  if (fileName && *fileName)
    wcsncpy(fileNamePath, fileName, MAX_PATH);
  else
    fileNamePath[0] = L'\0';
  return true;
}

wstring getTempFile() {
  return L"meos_server_tmp.tmp";
}

// -----------------------------------------------------------------------
// Shutdown signal handler
// -----------------------------------------------------------------------
static volatile bool g_running = true;
static void handleSignal(int) {
  g_running = false;
}

// -----------------------------------------------------------------------
// main
// -----------------------------------------------------------------------
int main(int argc, char *argv[]) {
  int port = 2009;
  std::string competitionFile;

  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);
    if ((arg == "--port" || arg == "-p") && i + 1 < argc)
      port = std::atoi(argv[++i]);
    else if ((arg == "--competition" || arg == "-c") && i + 1 < argc)
      competitionFile = argv[++i];
    else if (arg == "--help" || arg == "-h") {
      printf("Usage: MeOS-server [--port PORT] [--competition FILE]\n");
      return 0;
    }
  }

  signal(SIGINT, handleSignal);
  signal(SIGTERM, handleSignal);

  // Create a minimal gdioutput stub (no window, no GDI calls)
  static gdioutput serverGdi("server", 1.0);
  gdi_main = &serverGdi;

  NullNotifier notifier;
  oEvent oe(serverGdi, notifier);
  gEvent = &oe;

  if (!competitionFile.empty()) {
    wstring wfile = platform_fromUTF8(competitionFile);
    if (!oe.open(wfile, false, false, false)) {
      fprintf(stderr, "Failed to open competition: %s\n", competitionFile.c_str());
      return 1;
    }
    printf("Loaded competition: %s\n", competitionFile.c_str());
  }

  RestServer server;
  server.startService(port);
  printf("MeOS server started on port %d\n", port);

  while (g_running) {
    RestServer::computeRequested(oe);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  server.stop();
  printf("MeOS server stopped.\n");
  return 0;
}

#endif // MEOS_SERVER
