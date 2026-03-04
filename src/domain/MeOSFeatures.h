#pragma once
#include <set>
#include <string>

class oEvent;

class MeOSFeatures {
public:
    enum Feature {
        Rogaining = 13
    };
    bool hasFeature(Feature f) const { return false; }
};
