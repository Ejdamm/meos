#pragma once
#include <string>
#include "meos_util.h"

class gdioutput {
public:
    const std::wstring& recodeToWide(const std::string& in) {
        std::wstring& out = StringCache::getInstance().wget();
        string2Wide(in, out);
        return out;
    }
    const std::wstring& widen(const std::string& in) {
        std::wstring& out = StringCache::getInstance().wget();
        string2Wide(in, out);
        return out;
    }
    void setItems(const std::string& id, const std::vector<std::pair<std::wstring, size_t>>& items) {}
};
