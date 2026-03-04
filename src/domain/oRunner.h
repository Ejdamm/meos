#pragma once
#include "domain_header.h"
#include <vector>

class oCourse;

class oRunner {
public:
    virtual bool isRemoved() const { return false; }
    virtual oCourse* getCourse(bool update) const { return nullptr; }
    virtual void getSplitAnalysis(std::vector<int>& out) const {}
    virtual bool isVacant() const { return false; }
    virtual int getStatus() const { return 0; }
    virtual int getPunchTime(int i, bool adjust, bool manual, bool radio) const { return 0; }
    virtual std::wstring getClub() const { return L""; }
    virtual std::wstring getClass(bool update) const { return L""; }
};

typedef oRunner * pRunner;
