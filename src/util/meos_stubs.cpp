#include "StdAfx.h"
#include <codecvt>
#include <locale>

// String conversion stubs using standard C++
const wstring& widen(const string& input) {
    static wstring out;
    if (input.empty()) { out = L""; return out; }
    try {
        out = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(input);
    } catch (...) {
        out = L"";
    }
    return out;
}

const string& narrow(const wstring& input) {
    static string out;
    if (input.empty()) { out = ""; return out; }
    try {
        out = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(input);
    } catch (...) {
        out = "";
    }
    return out;
}

const string& toUTF8(const wstring& input) {
    return narrow(input);
}

const wstring& fromUTF8(const string& input) {
    return widen(input);
}

const wstring& recodeToWide(const string& input) {
    return fromUTF8(input);
}

const string& recodeToNarrow(const wstring& input) {
    return toUTF8(input);
}
