// Standalone portable unit tests - no Win32/gdioutput dependencies.
// Covers IEventNotifier, NullNotifier, and MockNotifier.
// Built as MeOS-test target; runs on Linux/macOS/Windows without GUI.

#include <cassert>
#include <iostream>
#include <string>
#include "IEventNotifier.h"
#include "NullNotifier.h"
#include "MockNotifier.h"

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

int main() {
  std::cout << "=== MeOS Portable Unit Tests ===\n\n";

  testNullNotifierReturnValues();
  testNullNotifierNoCrash();
  testMockNotifierRecordsCalls();
  testMockNotifierAllMethods();
  testMockNotifierReset();

  std::cout << "\nResults: " << gPassed << " passed, " << gFailed << " failed\n";

  return gFailed == 0 ? 0 : 1;
}
