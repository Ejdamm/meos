#pragma once

#include <string>
#include <vector>
#include <memory>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <cstdint>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <cwchar>
#include <cstdarg>

#include "win_types.h"

using std::shared_ptr;
using std::string;
using std::wstring;
using std::vector;
using std::list;
using std::map;
using std::set;
using std::pair;
using std::make_pair;
using std::make_shared;
using std::min;
using std::max;
using std::multimap;
using std::unique_ptr;
using std::make_unique;
using std::unordered_map;
using std::unordered_set;

#ifndef NOTIME
#define NOTIME 0x7FFFFFFF
#endif

#ifndef NOTIMEOUT
#define NOTIMEOUT 0x0AAAAAAA
#endif

typedef void (*GUICALLBACK)();

enum SpecialPunch { PunchUnused = 0, PunchStart = 1, PunchFinish = 2, PunchCheck = 3, HiredCard = 11111 };

#include "common_enums.h"

struct SqlUpdated {
  string updated;
  int counter = 0;
  bool changed = false;
  void reset() {
    updated.clear();
    changed = false;
    counter = 0;
  }
};

enum ClassType {
  oClassIndividual = 1, 
  oClassPatrol = 2, 
  oClassRelay = 3, 
  oClassIndividRelay = 4, 
  oClassKnockout = 5
};

class oCourse;
typedef oCourse* pCourse;
class oControl;
typedef oControl* pControl;
class oClass;
typedef oClass* pClass;
class oClub;
typedef oClub* pClub;
class oCard;
typedef oCard* pCard;
class oRunner;
typedef oRunner* pRunner;
typedef const oRunner* cRunner;
class oTeam;
typedef oTeam* pTeam;
typedef const oTeam* cTeam;
class oPunch;
class oFreePunch;
typedef oFreePunch* pFreePunch;
class oEvent;
class gdioutput;
class oListInfo;

enum SortOrder {
  ClassStartTime,
  ClassTeamLeg,
  ClassResult,
  ClassDefaultResult,
  ClassCourseResult,
  ClassTotalResult,
  ClassTeamLegResult,
  ClassFinishTime,
  ClassStartTimeClub,
  ClassPoints,
  ClassLiveResult,
  ClassKnockoutTotalResult,
  SortByName,
  SortByLastName,
  SortByFinishTime,
  SortByFinishTimeReverse,
  SortByStartTime,
  SortByStartTimeClass,
  CourseResult,
  CourseStartTime,
  SortByEntryTime,
  ClubClassStartTime,
  SortByBib,
  Custom,
  SortEnumLastItem
};

static bool orderByClass(SortOrder so) {
  switch (so) {
  case ClassStartTime:
  case ClassTeamLeg:
  case ClassResult:
  case ClassDefaultResult:
  case ClassCourseResult:
  case ClassTotalResult:
  case ClassTeamLegResult:
  case ClassFinishTime:
  case ClassStartTimeClub:
  case ClassPoints:
  case ClassLiveResult:
  case ClassKnockoutTotalResult:
    return true;
  }
  return false;
}

enum class oListId {oLRunnerId=1, oLClassId=2, oLCourseId=4,
                    oLControlId=8, oLClubId=16, oLCardId=32,
                    oLPunchId=64, oLTeamId=128, oLEventId=256};

enum EStdListType {
  NoneType = 0,
  ClassListType = 1,
  RunnerListType = 2,
  TeamListType = 3,
  ClubListType = 4,
  CourseListType = 5,
  ControlListType = 6,
  CardListType = 7,
  PunchListType = 8,
  EventListType = 9
};

enum EPostType {
  lNone = 0,
  TextPost = 1,
  ValuePost = 2,
  DataPost = 3,
  lRunnerStagePlace,
  lRunnerStageStatus,
  lRunnerStageTime,
  lRunnerStageTimeStatus,
  lRunnerStagePoints,
  lRunnerStageNumber,
  lTeamCourseName,
  lTeamCourseNumber,
  lTeamLegName,
  lTeamRunner,
  lTeamRunnerCard,
  lTeamLegTimeStatus,
  lTeamLegTimeAfter,
  lTeamPlace,
  lTeamStart,
  lResultModuleNumber,
  lResultModuleTime,
  lResultModuleTimeTeam,
  lResultModuleNumberTeam,
  lRunnerAnnotation,
  lTeamAnnotation,
  lRunnerGeneralPlace,
  lRunnerGeneralTimeStatus,
  lRunnerGeneralTimeAfter,
  lAlignNext
};

#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif

inline void getUserFile(wchar_t* path, const wchar_t* name) {
    swprintf(path, _MAX_PATH, L"%ls", name);
}

// SubSecond moved to common_enums.h
