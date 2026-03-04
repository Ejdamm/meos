#pragma once
#include <set>
#include <string>

class oEvent;

class MeOSFeatures {
public:
    enum Feature {
        Economy,
        Service,
        Network,
        Speaker,
        Auto,
        CoursePool,
        MultiDay,
        Team,
        Relay,
        Rogaining,
        QualificationFinal,
        Points,
        Live,
        Knockout,
        API,
        Map,
        ForkedIndividual,
        MultipleRaces,
        EditClub,
        RunnerDb,
        Patrol
    };
    bool hasFeature(Feature f) const { return true; }
    void useAll(oEvent& oe) {}
    void useFeature(Feature f, bool b, oEvent& oe) {}
    bool withoutCourses(const oEvent& oe) const { return false; }
};
