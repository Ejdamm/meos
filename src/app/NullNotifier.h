#pragma once
#include "IEventNotifier.h"

// No-op IEventNotifier for headless operation.
// All methods are silent; ask/askOkCancel return safe defaults.
class NullNotifier : public IEventNotifier {
public:
  void status(const std::wstring &) override {}
  void alert(const std::wstring &) override {}
  bool ask(const std::wstring &) override { return false; }
  Answer askOkCancel(const std::wstring &) override { return Answer::Cancel; }
  void setWindowTitle(const std::wstring &) override {}
  void dataChanged() override {}
};
