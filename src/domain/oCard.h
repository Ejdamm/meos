#pragma once
#include "domain_header.h"
#include <vector>
#include "oPunch.h"

class oCard {
public:
    virtual bool isRemoved() const { return false; }
    std::vector<oPunch> punches;
};

typedef oCard * pCard;
