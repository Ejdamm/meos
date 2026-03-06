#include <gtest/gtest.h>
#include "TimeStamp.h"
#include "meos_util.h"

class UtilTest : public ::testing::Test {
protected:
    void SetUp() override {
        setlocale(LC_ALL, "C.UTF-8"); // Try a common UTF-8 locale on Linux
        StringCache::getInstance().init();
    }
};

TEST_F(UtilTest, TimeStampBasic) {
    TimeStamp ts;
    ts.update();
    // Basic check that it doesn't crash
    EXPECT_GT(ts.getAge(), -10); // Allowance for small time diffs
}

TEST_F(UtilTest, StringConversion) {
    // Test UTF-8 conversion
    wstring original = L"Hello, \u00E5\u00E4\u00F6!"; // Åäö
    string utf8 = toUTF8(original);
    wstring back = fromUTF8(utf8);
    EXPECT_EQ(original, back);

    // Test widen/narrow (Windows-1252)
    string n = "Normal text";
    wstring w = widen(n);
    EXPECT_EQ(L"Normal text", w);
    EXPECT_EQ(n, narrow(w));
}

TEST_F(UtilTest, TimeFormatting) {
    SYSTEMTIME st;
    st.wYear = 2026;
    st.wMonth = 3;
    st.wDay = 6;
    st.wHour = 14;
    st.wMinute = 30;
    st.wSecond = 45;
    st.wMilliseconds = 123;

    wstring formatted = convertSystemTime(st);
    EXPECT_EQ(L"2026-03-06 14:30:45", formatted);

    wstring dateOnly = convertSystemDate(st);
    EXPECT_EQ(L"2026-03-06", dateOnly);

    wstring timeOnly = convertSystemTimeOnly(st);
    EXPECT_EQ(L"14:30:45", timeOnly);
}

TEST_F(UtilTest, CompareStringIgnoreCase) {
    EXPECT_EQ(0, compareStringIgnoreCase(L"Hello", L"hello"));
    EXPECT_EQ(0, compareStringIgnoreCase(L"ÅÄÖ", L"åäö"));
    EXPECT_NE(0, compareStringIgnoreCase(L"Hello", L"World"));
}


