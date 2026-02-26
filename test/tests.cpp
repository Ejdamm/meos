/************************************************************************
    MeOS - Orienteering Software
    Copyright (C) 2009-2023 Melin Software HB

    Melin Software HB - software@melin.nu - www.melin.nu
    Eksoppsv�gen 16, SE-75646 Uppsala, Sweden

************************************************************************/
#include "stdafx.h"

#include "testmeos.h"
#include "oEvent.h"
#include "MockNotifier.h"
#include "NullNotifier.h"

// Test: oEvent::getNotifier() returns the notifier wired in the constructor,
// and checkDB() delegates to notifier_.setWindowTitle().
class TestOEventDelegatesNotifier : public TestMeOS {
public:
  TestOEventDelegatesNotifier(TestMeOS &parent)
      : TestMeOS(parent, "oEvent delegates to IEventNotifier") {}

  TestMeOS *newInstance() const override {
    return new TestOEventDelegatesNotifier(
        const_cast<TestOEventDelegatesNotifier &>(*this));
  }

  void run() const override {
    MockNotifier mock;
    {
      oEvent testEvent(gdi(), mock);
      // checkDB() is the simplest public method that calls notifier_.setWindowTitle().
      testEvent.checkDB();
      assertTrue("setWindowTitle was called via notifier", mock.wasCalled("setWindowTitle"));
    }
    // On destruction, clear() calls checkDB() again -- notifier must still be valid.
    assertTrue("setWindowTitle called at least twice (once in run, once in dtor clear)",
               mock.callCount("setWindowTitle") >= 2);
  }
};

// Test: oEvent::getNotifier() accessor returns the same instance passed to ctor.
class TestOEventGetNotifier : public TestMeOS {
public:
  TestOEventGetNotifier(TestMeOS &parent)
      : TestMeOS(parent, "oEvent::getNotifier returns wired notifier") {}

  TestMeOS *newInstance() const override {
    return new TestOEventGetNotifier(
        const_cast<TestOEventGetNotifier &>(*this));
  }

  void run() const override {
    MockNotifier mock;
    {
      oEvent testEvent(gdi(), mock);
      IEventNotifier &ref = testEvent.getNotifier();
      ref.alert(L"probe");
    }
    assertTrue("alert delegated through getNotifier()", mock.wasCalled("alert"));
    assertEquals(std::wstring(L"probe"), mock.callArg("alert", 0));
  }
};

// Test: NullNotifier does not crash or throw on any method call.
class TestNullNotifierNoCrash : public TestMeOS {
public:
  TestNullNotifierNoCrash(TestMeOS &parent)
      : TestMeOS(parent, "NullNotifier does not crash") {}

  TestMeOS *newInstance() const override {
    return new TestNullNotifierNoCrash(const_cast<TestNullNotifierNoCrash &>(*this));
  }

  void run() const override {
    NullNotifier n;

    // Normal values
    n.status(L"Running");
    n.alert(L"Hello");
    bool askResult = n.ask(L"Continue?");
    IEventNotifier::Answer ans = n.askOkCancel(L"OK or Cancel?");
    n.setWindowTitle(L"MeOS Test");
    n.dataChanged();

    // Empty strings
    n.status(L"");
    n.alert(L"");
    n.ask(L"");
    n.askOkCancel(L"");
    n.setWindowTitle(L"");

    // Extreme: very long string
    std::wstring longStr(10000, L'x');
    n.status(longStr);
    n.alert(longStr);
    n.ask(longStr);
    n.askOkCancel(longStr);
    n.setWindowTitle(longStr);

    // Verify safe return values
    assertTrue("NullNotifier::ask returns false", !askResult);
    assertTrue("NullNotifier::askOkCancel returns Cancel",
               ans == IEventNotifier::Answer::Cancel);
    assertTrue("NullNotifier::ask(empty) returns false", !n.ask(L""));
    assertTrue("NullNotifier::askOkCancel(empty) returns Cancel",
               n.askOkCancel(L"") == IEventNotifier::Answer::Cancel);
  }
};

// Integration test: headless startup and competition loading.
// Creates an oEvent with NullNotifier (simulating --headless mode),
// sets up minimal competition data, saves to a temp file, and reloads it.
class TestHeadlessStartupAndLoad : public TestMeOS {
public:
  TestHeadlessStartupAndLoad(TestMeOS &parent)
      : TestMeOS(parent, "Headless startup and competition loading") {}

  TestMeOS *newInstance() const override {
    return new TestHeadlessStartupAndLoad(
        const_cast<TestHeadlessStartupAndLoad &>(*this));
  }

  void run() const override {
    NullNotifier nullNotifier;
    wstring tmpFile = getTempFile();

    // Step 1: create a competition and populate it with NullNotifier
    {
      oEvent evt(gdi(), nullNotifier);
      evt.setName(L"HeadlessTestComp", true);
      evt.addClass(L"Elite");
      evt.addRunner(L"TestRunner", L"TestClub", 0, 0, 0, false);

      // Save to file (internal .meos format)
      bool saved = evt.save(tmpFile, true, false);
      assertTrue("Competition saved without crash", saved);
    }

    // Step 2: reload the competition in a fresh oEvent with NullNotifier
    {
      oEvent evt2(gdi(), nullNotifier);
      bool opened = evt2.open(tmpFile, false, false, false);
      assertTrue("Competition loaded without crash", opened);
      assertTrue("Competition name restored", evt2.getName() == L"HeadlessTestComp");
      assertTrue("Classes loaded (>= 1)", evt2.getNumClasses() >= 1);
      assertTrue("Runners loaded (>= 1)", evt2.getNumRunners() >= 1);
    }
  }
};

void registerTests(TestMeOS &tm) {
  tm.registerTest(TestOEventDelegatesNotifier(tm));
  tm.registerTest(TestOEventGetNotifier(tm));
  tm.registerTest(TestNullNotifierNoCrash(tm));
  tm.registerTest(TestHeadlessStartupAndLoad(tm));
}
