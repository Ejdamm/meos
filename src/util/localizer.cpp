#include "localizer.h"

Localizer lang;

const std::wstring& Localizer::tl(const std::string& str) const {
    static std::wstring ws;
    ws.assign(str.begin(), str.end());
    return ws;
}
