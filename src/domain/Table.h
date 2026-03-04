#pragma once
#include <string>
#include <vector>
#include <memory>
#include "TableType.h"

class oEvent;
class oBase;
class oDataDefiner;

class Table {
public:
    enum { CAN_DELETE = 4, CAN_PASTE = 1, CAN_INSERT = 2 };
    Table(oEvent* oe, int rowHeight, const std::wstring& name, const std::string& tname) {}
    void addColumn(const std::string& title, int width, bool isnum, bool formatRight = false) {}
    int addColumn(const std::wstring& title, int width, bool isnum, bool formatRight = false) { return 0; }
    void setTableProp(int prop) {}
    void addRow(int id, oBase* obj) {}
    void set(int col, oBase& owner, int id, const std::wstring& data, bool canEdit, CellType type = cellEdit) {}
};

enum { TID_ID, TID_MODIFIED, TID_CONTROL, TID_STATUS, TID_CODES, TID_RUNNER, TID_CLUB, TID_START, TID_TIME, TID_FINISH, TID_RUNNINGTIME, TID_PLACE, TID_POINTS, TID_CARD, TID_TEAM, TID_LEG, TID_UNIT, TID_FEE, TID_PAID };
