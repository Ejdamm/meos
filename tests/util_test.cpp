#include <gtest/gtest.h>
#include "meos_util.h"
#include "meosexception.h"
#include "xmlparser.h"
#include "csvparser.h"
#include "TimeStamp.h"

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

TEST(XMLParserTest, BasicReadWrite) {
    xmlparser parser;
    parser.openMemoryOutput(false);
    parser.startTag("root");
    parser.write("child", "value");
    parser.endTag();
    
    std::string output;
    parser.getMemoryOutput(output);
    
    EXPECT_NE(output.find("<root>"), std::string::npos);
    EXPECT_NE(output.find("<child>value</child>"), std::string::npos);
    
    xmlparser readParser;
    readParser.readMemory(output, 0);
    xmlobject root = readParser.getObject("root");
    EXPECT_TRUE(root);
    EXPECT_EQ(root.getObjectString("child", output), "value");
}

TEST(CSVParserTest, BasicSplit) {
    char line[] = "val1;val2;\"val3;with;semi\";val4";
    std::vector<char*> sp;
    csvparser::split(line, sp);
    ASSERT_EQ(sp.size(), 4);
    EXPECT_STREQ(sp[0], "val1");
    EXPECT_STREQ(sp[1], "val2");
    EXPECT_STREQ(sp[2], "val3;with;semi");
    EXPECT_STREQ(sp[3], "val4");
}

TEST(TimeStampTest, BasicUpdate) {
    TimeStamp ts;
    ts.update();
    EXPECT_GT(ts.getModificationTime(), 0);
    
    std::string s = ts.getStamp();
    EXPECT_EQ(s.length(), 14); // YYYYMMDDHHMMSS
}
