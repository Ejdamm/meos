// gdioutput_server_stubs.cpp
// Provides stub implementations of FixedTabs and TabObject::loadPage for the
// MeOS-server headless build. This replaces TabBase.cpp (which transitively
// requires all Tab*.cpp files) for the server target.
#ifdef MEOS_SERVER

#include "stdafx.h"
#include "TabBase.h"

FixedTabs::FixedTabs() {
  runnerTab = nullptr; teamTab = nullptr; classTab = nullptr;
  courseTab = nullptr; controlTab = nullptr; siTab = nullptr;
  listTab = nullptr; cmpTab = nullptr; speakerTab = nullptr;
  clubTab = nullptr; autoTab = nullptr;
}

FixedTabs::~FixedTabs() {
  tabs.clear();
}

TabBase *FixedTabs::get(TabType /*tab*/) {
  return nullptr; // No UI tabs in server mode
}

void FixedTabs::clearCompetitionData() {
  // No tabs created in server mode; nothing to clear
}

bool TabObject::loadPage(gdioutput & /*gdi*/) {
  return false;
}

#endif // MEOS_SERVER
