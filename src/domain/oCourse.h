#pragma once
#include "domain_header.h"

class oControl;

class oCourse {
public:
    virtual int nControls() const { return 0; }
    virtual oControl* getControl(int i) const { return nullptr; }
    virtual int getStartPunchType() const { return 0; }
    virtual int getFinishPunchType() const { return 0; }
    virtual bool getCommonControl() const { return false; }
    virtual bool hasControl(const oControl* c) const { return false; }
    virtual int getId() const { return 0; }
    virtual bool isRemoved() const { return false; }
};

typedef oCourse * pCourse;
