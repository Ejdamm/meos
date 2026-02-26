// Standalone portable unit tests - no Win32/gdioutput dependencies.
// Covers IEventNotifier, NullNotifier, MockNotifier, platform_socket,
// std::filesystem, std::thread, std::mutex, and std::condition_variable.
// Built as MeOS-test target; runs on Linux/macOS/Windows without GUI.

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <filesystem>
#include "IEventNotifier.h"
#include "NullNotifier.h"
#include "MockNotifier.h"
#include "platform_socket.h"
#include "api_router.h"

namespace fs = std::filesystem;

static int gPassed = 0;
static int gFailed = 0;

#define CHECK(cond, msg) \
  do { \
    if (cond) { \
      ++gPassed; \
      std::cout << "  PASS: " << (msg) << "\n"; \
    } else { \
      ++gFailed; \
      std::cout << "  FAIL: " << (msg) << "\n"; \
    } \
  } while(0)

// --- NullNotifier tests ---

void testNullNotifierReturnValues() {
  std::cout << "TestNullNotifierReturnValues\n";
  NullNotifier n;
  CHECK(!n.ask(L"?"), "ask returns false");
  CHECK(n.askOkCancel(L"?") == IEventNotifier::Answer::Cancel,
        "askOkCancel returns Cancel");
}

void testNullNotifierNoCrash() {
  std::cout << "TestNullNotifierNoCrash\n";
  NullNotifier n;
  n.status(L"Running");
  n.alert(L"Hello");
  n.ask(L"Continue?");
  n.askOkCancel(L"OK?");
  n.setWindowTitle(L"MeOS");
  n.dataChanged();

  // Empty strings
  n.status(L"");
  n.alert(L"");
  n.ask(L"");
  n.askOkCancel(L"");
  n.setWindowTitle(L"");

  // Long string
  std::wstring longStr(10000, L'x');
  n.status(longStr);
  n.alert(longStr);
  n.ask(longStr);
  n.askOkCancel(longStr);
  n.setWindowTitle(longStr);

  CHECK(true, "NullNotifier survived all calls without crash");
}

// --- MockNotifier tests ---

void testMockNotifierRecordsCalls() {
  std::cout << "TestMockNotifierRecordsCalls\n";
  MockNotifier m;
  CHECK(!m.wasCalled("alert"), "no calls initially");

  m.alert(L"hello");
  CHECK(m.wasCalled("alert"), "alert recorded");
  CHECK(m.callCount("alert") == 1, "alert count == 1");
  CHECK(m.callArg("alert", 0) == L"hello", "alert arg matches");

  m.alert(L"world");
  CHECK(m.callCount("alert") == 2, "alert count == 2");
  CHECK(m.callArg("alert", 1) == L"world", "second alert arg matches");
}

void testMockNotifierAllMethods() {
  std::cout << "TestMockNotifierAllMethods\n";
  MockNotifier m;
  m.askResult = true;
  m.askOkCancelResult = IEventNotifier::Answer::Yes;

  m.status(L"s");
  m.alert(L"a");
  bool askR = m.ask(L"q");
  IEventNotifier::Answer ans = m.askOkCancel(L"q2");
  m.setWindowTitle(L"t");
  m.dataChanged();

  CHECK(m.wasCalled("status"),        "status recorded");
  CHECK(m.wasCalled("alert"),         "alert recorded");
  CHECK(m.wasCalled("ask"),           "ask recorded");
  CHECK(m.wasCalled("askOkCancel"),   "askOkCancel recorded");
  CHECK(m.wasCalled("setWindowTitle"),"setWindowTitle recorded");
  CHECK(m.wasCalled("dataChanged"),   "dataChanged recorded");
  CHECK(askR == true,                 "ask returns configured value true");
  CHECK(ans == IEventNotifier::Answer::Yes, "askOkCancel returns configured Yes");
}

void testMockNotifierReset() {
  std::cout << "TestMockNotifierReset\n";
  MockNotifier m;
  m.alert(L"x");
  CHECK(m.callCount("alert") == 1, "before reset: count 1");
  m.reset();
  CHECK(m.callCount("alert") == 0, "after reset: count 0");
  CHECK(!m.wasCalled("alert"),     "after reset: wasCalled false");
}

// --- platform_socket tests ---

void testSocketInit() {
  std::cout << "TestSocketInit\n";
  CHECK(platform_socket_init(), "platform_socket_init succeeds");
  platform_socket_cleanup();
}

void testSocketCreateClose() {
  std::cout << "TestSocketCreateClose\n";
  platform_socket_init();
  platform_socket_t s = platform_socket_create(AF_INET, SOCK_STREAM, 0);
  CHECK(s != PLATFORM_INVALID_SOCKET, "create TCP socket");
  int r = platform_socket_close(s);
  CHECK(r != PLATFORM_SOCKET_ERROR, "close socket");
  platform_socket_cleanup();
}

void testSocketTCPLoopback() {
  std::cout << "TestSocketTCPLoopback\n";
  platform_socket_init();

  // Create server socket and bind to loopback with auto-assigned port
  platform_socket_t server = platform_socket_create(AF_INET, SOCK_STREAM, 0);
  CHECK(server != PLATFORM_INVALID_SOCKET, "create server socket");

  // Allow fast rebind
  int yes = 1;
  platform_socket_setsockopt(server, SOL_SOCKET, SO_REUSEADDR,
                              reinterpret_cast<const char*>(&yes), sizeof(yes));

  struct sockaddr_in addr;
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port        = 0; // OS assigns port
  int r = platform_socket_bind(server, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
  CHECK(r != PLATFORM_SOCKET_ERROR, "bind server");

  // Discover assigned port
  socklen_platform addrLen = sizeof(addr);
  getsockname(server, reinterpret_cast<struct sockaddr*>(&addr),
              reinterpret_cast<socklen_platform*>(&addrLen));

  r = platform_socket_listen(server, 1);
  CHECK(r != PLATFORM_SOCKET_ERROR, "listen");

  // Connect client
  platform_socket_t client = platform_socket_create(AF_INET, SOCK_STREAM, 0);
  CHECK(client != PLATFORM_INVALID_SOCKET, "create client socket");
  r = platform_socket_connect(client, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
  CHECK(r != PLATFORM_SOCKET_ERROR, "connect");

  // Accept connection (non-blocking check first via select with 2s timeout)
  struct timeval tv;
  tv.tv_sec  = 2;
  tv.tv_usec = 0;
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(server, &fds);
  r = platform_socket_select(server + 1, &fds, nullptr, nullptr, &tv);
  CHECK(r > 0, "select: server readable before accept");

  platform_socket_t conn = platform_socket_accept(server, nullptr, nullptr);
  CHECK(conn != PLATFORM_INVALID_SOCKET, "accept");

  // Send "hello" from client, receive on accepted connection
  const char* msg = "hello";
  int sent = platform_socket_send(client, msg, 5, 0);
  CHECK(sent == 5, "send 5 bytes");

  // Wait for data to arrive
  tv.tv_sec = 2; tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(conn, &fds);
  r = platform_socket_select(conn + 1, &fds, nullptr, nullptr, &tv);
  CHECK(r > 0, "select: conn readable");

  char buf[16] = {};
  int recvd = platform_socket_recv(conn, buf, sizeof(buf), 0);
  CHECK(recvd == 5, "recv 5 bytes");
  CHECK(std::string(buf, 5) == "hello", "received data matches sent data");

  platform_socket_close(conn);
  platform_socket_close(client);
  platform_socket_close(server);
  platform_socket_cleanup();
}

// --- std::filesystem tests (Task 2.24) ---

void testFilesystemCreateWriteReadRemove() {
  std::cout << "TestFilesystemCreateWriteReadRemove\n";
  fs::path tmp = fs::temp_directory_path() / "meos_test_file.txt";
  // Write
  {
    std::ofstream ofs(tmp);
    ofs << "MeOS test content";
  }
  CHECK(fs::exists(tmp), "file exists after write");
  // Read back
  {
    std::ifstream ifs(tmp);
    std::string content((std::istreambuf_iterator<char>(ifs)),
                         std::istreambuf_iterator<char>());
    CHECK(content == "MeOS test content", "file content matches");
  }
  CHECK(fs::file_size(tmp) > 0, "file_size > 0");
  // Remove
  fs::remove(tmp);
  CHECK(!fs::exists(tmp), "file removed");
}

void testFilesystemDirectories() {
  std::cout << "TestFilesystemDirectories\n";
  fs::path dir = fs::temp_directory_path() / "meos_test_dir" / "sub";
  fs::create_directories(dir);
  CHECK(fs::is_directory(dir), "create_directories creates nested dir");
  // Create a file inside
  fs::path f = dir / "x.txt";
  { std::ofstream ofs(f); ofs << "x"; }
  CHECK(fs::exists(f), "file inside nested dir exists");
  // Remove all
  fs::remove_all(fs::temp_directory_path() / "meos_test_dir");
  CHECK(!fs::exists(dir), "remove_all cleans up");
}

void testFilesystemSwedishPaths() {
  std::cout << "TestFilesystemSwedishPaths\n";
  // Path with Swedish characters (UTF-8 on POSIX, UTF-16 on Windows via std::filesystem)
  fs::path dir = fs::temp_directory_path() / u8"meos_\u00e5\u00e4\u00f6_test";
  fs::create_directory(dir);
  CHECK(fs::is_directory(dir), "directory with åäö created");
  fs::path f = dir / u8"fil_\u00e5\u00e4\u00f6.txt";
  { std::ofstream ofs(f); ofs << "swedish"; }
  CHECK(fs::exists(f), "file with åäö exists");
  std::string content;
  {
    std::ifstream ifs(f);
    std::getline(ifs, content);
  }
  CHECK(content == "swedish", "file with åäö readable");
  fs::remove_all(dir);
  CHECK(!fs::exists(dir), "cleaned up åäö dir");
}

// --- std::thread / std::mutex / std::condition_variable tests (Task 2.25) ---

void testThreadStartJoin() {
  std::cout << "TestThreadStartJoin\n";
  std::atomic<int> counter{0};
  std::thread t([&]() { ++counter; });
  t.join();
  CHECK(counter == 1, "thread incremented counter");
}

void testMutexLockUnlock() {
  std::cout << "TestMutexLockUnlock\n";
  std::mutex m;
  int shared = 0;
  auto work = [&]() {
    for (int i = 0; i < 1000; ++i) {
      std::lock_guard<std::mutex> lk(m);
      ++shared;
    }
  };
  std::thread t1(work);
  std::thread t2(work);
  t1.join();
  t2.join();
  CHECK(shared == 2000, "mutex protects counter: value == 2000");
}

void testConditionVariable() {
  std::cout << "TestConditionVariable\n";
  std::mutex m;
  std::condition_variable cv;
  bool ready = false;
  std::atomic<bool> received{false};

  std::thread waiter([&]() {
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [&]{ return ready; });
    received = true;
  });

  {
    std::lock_guard<std::mutex> lk(m);
    ready = true;
  }
  cv.notify_one();
  waiter.join();
  CHECK(received.load(), "condition_variable: waiter received signal");
}

// --- Content negotiation tests ---
#include "../src/net/api_router.h"

void testContentNegotiationDefaultJson() {
  ApiRequest req;
  // No Accept header → default JSON
  CHECK(req.negotiateContentType() == "application/json",
        "content negotiation: no Accept header → application/json");
}

void testContentNegotiationExplicitJson() {
  ApiRequest req;
  req.headers["accept"] = "application/json";
  CHECK(req.negotiateContentType() == "application/json",
        "content negotiation: Accept: application/json");
}

void testContentNegotiationExplicitXml() {
  ApiRequest req;
  req.headers["accept"] = "text/xml";
  CHECK(req.negotiateContentType() == "text/xml",
        "content negotiation: Accept: text/xml");
}

void testContentNegotiationXmlBeforeJson() {
  ApiRequest req;
  req.headers["accept"] = "text/xml, application/json";
  // xml listed first → prefer xml
  CHECK(req.negotiateContentType() == "text/xml",
        "content negotiation: xml before json → text/xml");
}

void testContentNegotiationJsonBeforeXml() {
  ApiRequest req;
  req.headers["accept"] = "application/json, text/xml";
  CHECK(req.negotiateContentType() == "application/json",
        "content negotiation: json before xml → application/json");
}

void testContentNegotiationUnknownType() {
  ApiRequest req;
  req.headers["accept"] = "text/html";
  // Unknown type → default JSON
  CHECK(req.negotiateContentType() == "application/json",
        "content negotiation: unknown Accept type → application/json");
}

void testPostRouteReturns201() {
  ApiRouter router;
  router.post("/api/items", [](const ApiRequest &req) {
    return ApiResponse::created("{\"id\":1}");
  });
  ApiRequest req;
  req.method = "POST";
  req.path = "/api/items";
  req.body = "{\"name\":\"test\"}";
  ApiResponse res = router.dispatch(req);
  CHECK(res.status == 201, "POST route returns 201 Created");
}

void testDeleteRouteReturns204() {
  ApiRouter router;
  router.del("/api/items/:id", [](const ApiRequest &req) {
    return ApiResponse::noContent();
  });
  ApiRequest req;
  req.method = "DELETE";
  req.path = "/api/items/42";
  ApiResponse res = router.dispatch(req);
  CHECK(res.status == 204, "DELETE route returns 204 No Content");
}

void testPutRouteReturns200() {
  ApiRouter router;
  router.put("/api/items/:id", [](const ApiRequest &req) {
    return ApiResponse::ok("{\"id\":" + req.pathParams.at("id") + "}");
  });
  ApiRequest req;
  req.method = "PUT";
  req.path = "/api/items/7";
  req.body = "{\"name\":\"updated\"}";
  ApiResponse res = router.dispatch(req);
  CHECK(res.status == 200, "PUT route returns 200");
  CHECK(res.body.find("7") != std::string::npos, "PUT returns correct id");
}

void testBodyJsonParsing() {
  ApiRequest req;
  req.body = "{\"name\":\"Alice\",\"age\":30}";
  auto j = req.bodyJson();
  CHECK(j["name"] == "Alice", "bodyJson parses string field");
  CHECK(j["age"] == 30, "bodyJson parses int field");
}

void testBodyJsonInvalidReturnsNull() {
  ApiRequest req;
  req.body = "not json{{{";
  auto j = req.bodyJson();
  // Invalid JSON → returned json object is null/empty (no exception)
  CHECK(j.is_null() || j.empty(), "bodyJson returns empty on invalid JSON");
}

void testBodyJsonEmpty() {
  ApiRequest req;
  // empty body
  auto j = req.bodyJson();
  CHECK(!req.hasValidJsonBody(), "empty body → hasValidJsonBody false");
}

void testBadRequestStatus() {
  ApiResponse res = ApiResponse::badRequest("missing field");
  CHECK(res.status == 400, "badRequest returns 400");
}

void testMethodNotAllowed() {
  ApiRouter router;
  router.get("/api/items", [](const ApiRequest &) { return ApiResponse::ok("[]"); });
  ApiRequest req;
  req.method = "DELETE";
  req.path = "/api/items";
  ApiResponse res = router.dispatch(req);
  CHECK(res.status == 405, "wrong method returns 405");
}


int main() {
  std::cout << "=== MeOS Portable Unit Tests ===\n\n";

  testNullNotifierReturnValues();
  testNullNotifierNoCrash();
  testMockNotifierRecordsCalls();
  testMockNotifierAllMethods();
  testMockNotifierReset();
  testSocketInit();
  testSocketCreateClose();
  testSocketTCPLoopback();
  testFilesystemCreateWriteReadRemove();
  testFilesystemDirectories();
  testFilesystemSwedishPaths();
  testThreadStartJoin();
  testMutexLockUnlock();
  testConditionVariable();
  testContentNegotiationDefaultJson();
  testContentNegotiationExplicitJson();
  testContentNegotiationExplicitXml();
  testContentNegotiationXmlBeforeJson();
  testContentNegotiationJsonBeforeXml();
  testContentNegotiationUnknownType();
  testPostRouteReturns201();
  testDeleteRouteReturns204();
  testPutRouteReturns200();
  testBodyJsonParsing();
  testBodyJsonInvalidReturnsNull();
  testBodyJsonEmpty();
  testBadRequestStatus();
  testMethodNotAllowed();

  std::cout << "\nResults: " << gPassed << " passed, " << gFailed << " failed\n";

  return gFailed == 0 ? 0 : 1;
}
