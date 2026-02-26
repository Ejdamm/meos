#pragma once
#include <string>
#include <vector>
#include "IEventNotifier.h"

// Mock implementation of IEventNotifier for unit testing.
// Records all method calls and arguments for later verification.
class MockNotifier : public IEventNotifier {
public:
  struct Call {
    std::string method;
    std::wstring arg;
  };

  std::vector<Call> calls;

  // Configurable return values for interactive methods.
  bool askResult = false;
  Answer askOkCancelResult = Answer::Cancel;

  void status(const std::wstring &msg) override {
    calls.push_back({"status", msg});
  }

  void alert(const std::wstring &msg) override {
    calls.push_back({"alert", msg});
  }

  bool ask(const std::wstring &msg) override {
    calls.push_back({"ask", msg});
    return askResult;
  }

  Answer askOkCancel(const std::wstring &msg) override {
    calls.push_back({"askOkCancel", msg});
    return askOkCancelResult;
  }

  void setWindowTitle(const std::wstring &title) override {
    calls.push_back({"setWindowTitle", title});
  }

  void dataChanged() override {
    calls.push_back({"dataChanged", L""});
  }

  // Returns true if the method was called at least once.
  bool wasCalled(const std::string &method) const {
    for (const auto &c : calls)
      if (c.method == method) return true;
    return false;
  }

  // Returns the number of times the method was called.
  int callCount(const std::string &method) const {
    int n = 0;
    for (const auto &c : calls)
      if (c.method == method) ++n;
    return n;
  }

  // Returns the argument of the nth call to the given method (0-indexed).
  std::wstring callArg(const std::string &method, int index = 0) const {
    int n = 0;
    for (const auto &c : calls) {
      if (c.method == method) {
        if (n == index) return c.arg;
        ++n;
      }
    }
    return L"";
  }

  void reset() { calls.clear(); }
};
