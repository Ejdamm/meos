#pragma once
#include <string>
#include <vector>
#include "TableType.h"

class oBase;

enum {TID_CLASSNAME, TID_COURSE, TID_NUM, TID_ID, TID_MODIFIED,
TID_RUNNER, TID_CLUB, TID_START, TID_TIME,
TID_FINISH, TID_STATUS, TID_RUNNINGTIME, TID_PLACE, TID_POINTS,
TID_CARD, TID_TEAM, TID_LEG, TID_CONTROL, TID_UNIT, TID_CODES, TID_FEE, TID_PAID,
TID_INPUTTIME, TID_INPUTSTATUS, TID_INPUTPOINTS, TID_INPUTPLACE,
TID_NAME, TID_NATIONAL, TID_SEX, TID_YEAR, TID_INDEX, 
TID_ENTER, TID_STARTNO, TID_VOLTAGE, TID_BATTERYDATE, TID_CARDTYPE, TID_FINISHCONTROL, TID_STARTCONTROL};

struct TableUpdateInfo {
    bool doRefresh = false;
    bool doAdd = false;
    void* object = nullptr;
};

class oEvent;

class Table {
    std::string internalName;
    oEvent* oe;
public:
    enum { CAN_DELETE = 1, CAN_PASTE = 2, CAN_INSERT = 4 };
    Table(oEvent* oe, int type, const std::wstring& name, const std::string& internalName) : oe(oe), internalName(internalName) {}
    TableColSpec addColumn(const std::string& name, int width, bool b1, bool b2 = false) { return TableColSpec(); }
    TableColSpec addColumn(const std::wstring& name, int width, bool b1, bool b2 = false) { return TableColSpec(); }
    void addRow(int id, oBase* obj) {}
    void set(int col, oBase& obj, int tid, const std::wstring& val, bool canEdit = true, CellType type = cellEdit) {}
    const std::string& getInternalName() const { return internalName; }
    void setTableProp(uint32_t prop) {}
    void reserve(size_t n) {}
    oEvent* getEvent() const { return oe; }
};
