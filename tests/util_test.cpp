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
    
    // Check that getAge returns something reasonable (around 0)
    EXPECT_GE(ts.getAge(), -1);
    EXPECT_LE(ts.getAge(), 5);

    // Check that getStamp returns a 14-character string (YYYYMMDDHHMMSS)
    string stamp = ts.getStamp();
    EXPECT_EQ(14, stamp.length());
    for (char c : stamp) {
        EXPECT_TRUE(isdigit(c));
    }

    // Check getStampString
    wstring stampStr = ts.getStampString();
    EXPECT_EQ(19, stampStr.length()); // YYYY-MM-DD HH:MM:SS

    // Check setStamp round-trip
    TimeStamp ts2;
    string sampleStamp = "20260306143045";
    ts2.setStamp(sampleStamp);
    EXPECT_EQ(sampleStamp, ts2.getStamp());
    EXPECT_EQ(L"2026-03-06 14:30:45", ts2.getStampString());
    EXPECT_EQ(L"14:30", ts2.getUpdateTime());
}

TEST_F(UtilTest, TimeStampModification) {
    TimeStamp ts1;
    ts1.setStamp("20260306100000");
    
    TimeStamp ts2;
    ts2.setStamp("20260306110000");
    
    EXPECT_GT(ts2.getModificationTime(), ts1.getModificationTime());
    
    TimeStamp ts3;
    ts3.update(ts1);
    EXPECT_EQ(ts3.getModificationTime(), ts1.getModificationTime());
    
    ts3.update(ts2);
    EXPECT_EQ(ts3.getModificationTime(), ts2.getModificationTime());
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

#include "xmlparser.h"
#include "csvparser.h"

TEST_F(UtilTest, XMLParserBasic) {
    xmlparser parser;
    string xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><root><item id=\"1\">Value 1</item><item id=\"2\">Value 2</item></root>";
    parser.readMemory(xml, 0);

    xmlobject root = parser.getObject("root");
    EXPECT_TRUE(root);

    xmlList items;
    root.getObjects("item", items);
    EXPECT_EQ(2, items.size());

    EXPECT_EQ("1", items[0].getAttrib("id").getStr());
    EXPECT_EQ("Value 1", items[0].getStr());
    EXPECT_EQ("2", items[1].getAttrib("id").getStr());
    EXPECT_EQ("Value 2", items[1].getStr());
}

TEST_F(UtilTest, CSVParserBasic) {
    // Create a temporary CSV file
    wstring filename = L"test.csv";
    {
        csvparser writer;
        writer.openOutput(filename);
        writer.outputRow(vector<string>{"Col1", "Col2"});
        writer.outputRow(vector<string>{"Val1", "Val2"});
        writer.closeOutput();
    }

    csvparser reader;
    list<vector<wstring>> data;
    reader.parse(filename, data);

    EXPECT_EQ(2, data.size());
    auto it = data.begin();
    EXPECT_EQ(L"Col1", (*it)[0]);
    EXPECT_EQ(L"Col2", (*it)[1]);
    ++it;
    EXPECT_EQ(L"Val1", (*it)[0]);
    EXPECT_EQ(L"Val2", (*it)[1]);

    std::filesystem::remove(path2str(filename));
}


