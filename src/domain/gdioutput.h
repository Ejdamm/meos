#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "gdifonts.h"
#include "win_types.h"

using std::string;
using std::wstring;
using std::vector;
using std::pair;

enum Color { colorBlack, colorWhite, colorRed, colorGreen, colorBlue, colorYellow, colorGray, colorGreyBlue, colorDarkRed, colorLightGreen, colorDarkGreen };

const int textRight = 1024;
const int textLeft = 0;
const int boldText = 2048;
const int italicSmall = 4096;
const int italicText = 4096;
const int textCenter = 8192;
const int timeWithTenth = 16384;
const int timerCanBeNegative = 32768;
const int timeHHMM = 65536;
const int pageNewChapter = 131072;
const int Capitalize = 262144;

enum class BoxStyle { Header };

struct TextInfo {
    int xp;
    TextInfo& setExtra(int id) { return *this; }
    string id;
    RECT textRect;
};

class GuiHandler { public: virtual ~GuiHandler() {} };

class gdioutput {
public:
    virtual ~gdioutput() {}
    
    virtual void drawLine(int x1, int y1, int x2, int y2) {}
    virtual void drawRect(int x1, int y1, int x2, int y2) {}
    virtual void drawText(int x, int y, const wstring& text) {}
    
    gdioutput& addString(const string& id, int font, const string& s) { return *this; }
    gdioutput& addString(const string& id, int font, const wstring& s) { return *this; }
    gdioutput& addString(const string& id, int font, const char* s) { return *this; }
    gdioutput& addString(const string& id, int font, const char* s, void (*cb)()) { return *this; }
    gdioutput& addString(const string& id, int font, const wstring& s, void (*cb)()) { return *this; }
    gdioutput& addString(const string& id, int y, int x, int font, const string& s) { return *this; }
    gdioutput& addString(const string& id, int y, int x, int font, const wstring& s, int dx) { return *this; }
    gdioutput& addString(const string& id, int y, int x, int font, const string& s, int dx) { return *this; }
    gdioutput& addString(const string& id, int y, int x, int font, const char* s, int dx) { return *this; }
    gdioutput& addString(const string& id, int font, const wstring& s, Color c) { return *this; }
    gdioutput& setExtra(int id) { return *this; }
    
    TextInfo addStringUT(int font, const wstring& s) { return TextInfo(); }
    TextInfo addStringUT(int y, int x, int font, const wstring& s) { return TextInfo(); }
    TextInfo addStringUT(int y, int x, int font, const string& s) { return TextInfo(); }
    TextInfo addStringUT(int y, int x, int font, const char* s) { return TextInfo(); }
    TextInfo& addStringUT(int y, int x, int font, const wstring& s, int limit) { static TextInfo ti; return ti; }
    TextInfo& addStringUT(int y, int x, int font, const wstring& s, int limit, void (*cb)()) { static TextInfo ti; return ti; }
    TextInfo& addStringUT(int y, int x, int font, const wstring& s, int limit, int (*cb)(gdioutput*, int, int*)) { static TextInfo ti; return ti; }
    
    gdioutput& setColor(Color c) { return *this; }
    void refreshFast() {}
    void refresh() {}
    void scrollToBottom() {}
    bool isTest() const { return false; }
    
    void dropLine() {}
    void dropLine(double f) {}
    void fillDown() {}
    void fillRight() {}
    void addItem(const string& name, const wstring& text, int data) {}
    void clearList(const string& name) {}
    void setItems(const string& id, const vector<pair<wstring, size_t>>& items) {}
    void makeEvent(const string& type, const string& subType, int x, int y, bool b) {}
    bool ask(const wstring& msg) { return true; }
    void addInfoBox(const string& id, const wstring& msg, const wstring& title, BoxStyle style, int timeout) {}
    void alert(const string& s) {}
    void alert(const wstring& s) {}

    int getCY() { return 0; }
    int getCX() { return 0; }
    void setCX(int x) {}
    int getLineHeight() { return 10; }
    void addRectangle(struct RECT rc, Color c, bool b = false) {}
    void pushX() {}
    void popX() {}
    void pushY() {}
    void popY() {}
    int scaleLength(int l) { return l; }
    wstring recodeToWide(const string& s) { return widen_s(s); }

    static string narrow(const wstring& s) { return string(s.begin(), s.end()); }
    static wstring widen(const string& s) { return wstring(s.begin(), s.end()); }
    // wstring widen { return wstring(s.begin(), s.end()); }
    // string narrow { return string(s.begin(), s.end()); }

    static string toUTF8(const wstring& s) { return narrow_s(s); }
    static wstring fromUTF8(const string& s) { return widen_s(s); }

    void addTimer(int y, int x, int font, int time, const wstring& s, int limit) {}
    void addTimer(int y, int x, int font, int time, const wstring& s, int limit, int (*cb)(gdioutput*, int, int*)) {}
    static wstring getTimerText(int time, int flag, bool b, const wstring& s) { return L""; }

    bool hasData(const string& id) { return false; }
    void getData(const string& id, int& val) {}
    void getData(const string& id, uint32_t& val) {}
    void getData(const string& id, vector<int>& val) {}
    void setData(const string& id, int val) {}
    void setData(const string& id, const vector<int>& val) {}

    void takeShownStringsSnapshot() {}
    int getOffsetY() { return 0; }
    void setOffsetY(int y) {}
    void restoreNoUpdate(const string& s) {}
    void restore(const string& s, bool b) {}
    void setRestorePoint(const string& s) {}
    void updatePos(int x, int y, int w, int h) {}
    void registerEvent(const string& s, void (*cb)()) {}
    void setWindowTitle(const wstring& s) {}
    HWND getHWNDTarget() { return nullptr; }
    HWND getHWNDMain() { return nullptr; }
    void calcStringSize(TextInfo& ti, HDC hdc) {}
    void refreshSmartFromSnapshot(bool b) {}
    void updateScrollbars() {}
    void addSelection(const string& id, int x, int y) {}
    void addSelection(const string& id, int x, int y, void* p, const wstring& s) {}
    void setHandler(shared_ptr<GuiHandler> h) {}
    void selectItemByData(const string& id, int data) {}
    void selectFirstItem(const string& id) {}
    void addToolTip(const string& id, const wstring& s, int i, RECT* r) {}
};
