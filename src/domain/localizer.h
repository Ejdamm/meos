#pragma once
#include <string>

class Localizer {
public:
    std::wstring tl(const std::string& s, bool b = false) { return std::wstring(s.begin(), s.end()); }
    std::wstring tl(const char* s, bool b = false) { return tl(std::string(s), b); }
    std::wstring tl(const std::wstring& s, bool b = false) { return s; }
};

extern Localizer lang;
