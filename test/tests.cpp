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

void registerTests(TestMeOS &tm) {
  tm.registerTest(TestOEventDelegatesNotifier(tm));
  tm.registerTest(TestOEventGetNotifier(tm));
}
