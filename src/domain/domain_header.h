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

typedef unsigned char BYTE;
typedef unsigned char * LPBYTE;

#ifndef NOTIME
#define NOTIME 0x7FFFFFFF
#endif

enum SpecialPunch { PunchUnused = 0, PunchStart = 1, PunchFinish = 2, PunchCheck = 3, HiredCard = 11111 };
