#pragma once
#include <string>

// Pure virtual interface for domain-to-UI communication.
// Abstracts user interaction and status reporting away from gdioutput,
// enabling headless/testable domain logic.
class IEventNotifier {
public:
  virtual ~IEventNotifier() = default;

  // Possible answers for multi-choice dialogs.
  enum class Answer { No = 0, Yes = 1, Cancel = 2, OK = 3 };

  // Display a status/informational message.
  virtual void status(const std::wstring &msg) = 0;

  // Display an error or warning message (modal, no return value).
  virtual void alert(const std::wstring &msg) = 0;

  // Ask a yes/no question. Returns true if the user answered Yes/OK.
  virtual bool ask(const std::wstring &msg) = 0;

  // Ask a yes/no/cancel question.
  virtual Answer askOkCancel(const std::wstring &msg) = 0;

  // Update the window/application title shown to the user.
  virtual void setWindowTitle(const std::wstring &title) = 0;

  // Notify the UI that competition data has changed and needs to be refreshed.
  virtual void dataChanged() = 0;
};
