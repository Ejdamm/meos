// Standalone portable unit tests - no Win32/gdioutput dependencies.
// Covers IEventNotifier, NullNotifier, MockNotifier, and platform_socket.
// Built as MeOS-test target; runs on Linux/macOS/Windows without GUI.

#include <cassert>
#include <iostream>
#include <string>
#include "IEventNotifier.h"
#include "NullNotifier.h"
#include "MockNotifier.h"
#include "platform_socket.h"

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

  std::cout << "\nResults: " << gPassed << " passed, " << gFailed << " failed\n";

  return gFailed == 0 ? 0 : 1;
}
