#pragma once
#include <vector>
#include <list>
#include "domain_header.h"

class oFreeImport {
public:
    bool isLoaded() const { return true; }
    void load() {}
    void init(oRunnerList& r, oClubList& c, oClassList& cl) {}
};
