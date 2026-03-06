#include "StdAfx.h"
#include "gdioutput.h"
#include "meos_util.h"
#include "Table.h"

gdioutput::gdioutput(const string& tag, double _scale) : tag(tag), scale(_scale) {
    CurrentX = 0; CurrentY = 0; MaxX = 0; MaxY = 0; lineHeight = 15;
}
gdioutput::gdioutput(double _scale, HWND hWndTarget, const PrinterObject& defprn) : scale(_scale), hWndTarget(hWndTarget) {
    CurrentX = 0; CurrentY = 0; MaxX = 0; MaxY = 0; lineHeight = 15;
}
gdioutput::~gdioutput() {}

// Minimal essential stubs
void gdioutput::alert(const string& msg) const {}
void gdioutput::alert(const wstring& msg) const {}
bool gdioutput::ask(const wstring& s, const char *yesButton, const char *noButton) { return true; }
void gdioutput::refresh() const {}
void gdioutput::refreshFast() const {}
void gdioutput::clearPage(bool autoRefresh, bool keepToolbar) {}

// TimerInfo stubs
int TimerInfo::globalTimerId = 0;
TimerInfo::~TimerInfo() {}

// Static members
const wstring& gdioutput::widen(const string& input) { return ::widen(input); }
const string& gdioutput::narrow(const wstring& input) { return ::narrow(input); }
const string& gdioutput::toUTF8(const wstring& input) { return ::toUTF8(input); }
const wstring& gdioutput::fromUTF8(const string& input) { return ::fromUTF8(input); }
const wstring& gdioutput::recodeToWide(const string& input) { return ::recodeToWide(input); }
const string& gdioutput::recodeToNarrow(const wstring& input) { return ::recodeToNarrow(input); }
