#pragma once
#include "domain_header.h"
#include <set>

class oClass {
public:
    virtual bool isRemoved() const { return false; }
    virtual bool hasAnyCourse(const std::set<int>& cid) const { return false; }
    virtual int getId() const { return 0; }
};

typedef oClass * pClass;
