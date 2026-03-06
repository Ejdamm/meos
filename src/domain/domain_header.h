#pragma once

#include <string>
#include <vector>
#include <memory>

// Forward declarations
class oEvent;
class oBase;
class oDataContainer;
class oDataInterface;
class oDataConstInterface;
class gdioutput;
class Table;
class xmlparser;
class xmlobject;

// Common enums
enum SpecialPunch { PunchUnused = 0, PunchStart = 1, PunchFinish = 2, PunchCheck = 3, HiredCard = 11111 };

enum class ChangeType {
  Quiet,
  Update
};

// Typedefs
typedef void* pvoid;
typedef std::vector<std::vector<std::wstring>>* pvectorstr;

struct SqlUpdated {
    std::string updated;
    int counter = 0;
    bool changed = false;
    void reset() {
        updated.clear();
        changed = false;
        counter = 0;
    }
};
