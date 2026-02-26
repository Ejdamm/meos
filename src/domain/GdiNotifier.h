#pragma once
#include "IEventNotifier.h"
#include "gdioutput.h"

// Adapter: routes IEventNotifier calls to a gdioutput instance.
class GdiNotifier : public IEventNotifier {
public:
  explicit GdiNotifier(gdioutput &gdi) : gdi_(gdi) {}

  void status(const std::wstring & /*msg*/) override {}

  void alert(const std::wstring &msg) override {
    gdi_.alert(msg);
  }

  bool ask(const std::wstring &msg) override {
    return gdi_.ask(msg);
  }

  Answer askOkCancel(const std::wstring &msg) override {
    return static_cast<Answer>(static_cast<int>(gdi_.askOkCancel(msg)));
  }

  void setWindowTitle(const std::wstring &title) override {
    gdi_.setWindowTitle(title);
  }

  void dataChanged() override {}

private:
  gdioutput &gdi_;
};
