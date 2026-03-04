#include <gtest/gtest.h>
#include "meos_util.h"
#include "meosexception.h"

TEST(UtilTest, StringConversion) {
    std::string n = "Hello World";
    std::wstring w;
    string2Wide(n, w);
    EXPECT_EQ(w, L"Hello World");

    std::string n2;
    wide2String(w, n2);
    EXPECT_EQ(n2, "Hello World");
}

TEST(UtilTest, TimeFormatting) {
    SYSTEMTIME st;
    st.wYear = 2026;
    st.wMonth = 3;
    st.wDay = 4;
    st.wHour = 10;
    st.wMinute = 30;
    st.wSecond = 45;

    std::string nt = convertSystemTimeN(st);
    EXPECT_EQ(nt, "2026-03-04 10:30:45");

    std::wstring wt = convertSystemTime(st);
    EXPECT_EQ(wt, L"2026-03-04 10:30:45");
}

TEST(UtilTest, Exception) {
    try {
        throw meosException(L"Wide Error Message");
    } catch (const meosException& e) {
        EXPECT_EQ(e.wwhat(), L"Wide Error Message");
        EXPECT_STREQ(e.what(), "Wide Error Message");
    }
}
