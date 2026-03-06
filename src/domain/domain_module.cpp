#include "StdAfx.h"
#include "oEvent.h"
#include "gdioutput.h"
#include "Table.h"

// oEvent stubs for domain foundation
bool oEvent::msSynchronize(oBase* ob) { return false; }
void oEvent::updateFreeId(oBase* ob) {}
wstring oEvent::formatCurrency(int c, bool includeSymbol) const { return L""; }
int oEvent::interpretCurrency(const wstring& c) const { return 0; }

// gdioutput stubs for domain foundation 
BaseInfo* gdioutput::setText(const char* id, const wstring& text, bool update, int requireExtraMatch, bool updateOriginal) { return nullptr; }
bool gdioutput::hasWidget(const string& id) const { return false; }
pair<int, bool> gdioutput::getSelectedItem(const string& id) { return make_pair(-1, false); }
pair<int, bool> gdioutput::getSelectedItem(const char* id) { return make_pair(-1, false); }

ListBoxInfo& gdioutput::addSelection(const string& id, int width, int height, GUICALLBACK cb, const wstring& explanation, const wstring& tooltip) { 
    static ListBoxInfo lbi; return lbi; 
}
bool gdioutput::setItems(const string& id, const vector< pair<wstring, size_t> >& items) { return false; }
InputInfo& gdioutput::addInput(const string& id, const wstring& text, int length, GUICALLBACK cb, const wstring& explanation, const wstring& tooltip) {
    static InputInfo ii; return ii;
}
gdioutput::AskAnswer gdioutput::askOkCancel(const wstring& s) { return gdioutput::AskAnswer::AnswerCancel; }
bool gdioutput::selectItemByData(const char* id, int data) { return false; }
int gdioutput::getTextNo(const char* id, bool acceptMissing) const { return 0; }
const wstring& gdioutput::getText(const char* id, bool acceptMissing, int requireExtraMatch) const { static wstring empty; return empty; }

// Table stubs for domain foundation
void Table::addDataDefiner(const string& key, const oDataDefiner* definer) {}
TableColSpec Table::addColumn(const string& Title, int width, bool isnum, bool formatRight) { return TableColSpec(); }
int Table::addColumnPaddedSort(const string& title, int width, int padding, bool formatRight) { return 0; }
void Table::set(int column, oBase& owner, int id, const wstring& data, bool canEdit, CellType type) {}

// InputInfo stub
#include "gdistructures.h"
InputInfo::InputInfo() : hWnd(0), callBack(nullptr), updateLastData(nullptr), xp(0), yp(0), width(0), height(0), bgColor(GDICOLOR(0)), fgColor(GDICOLOR(0)), isEditControl(false), writeLock(false), ignoreCheck(false) {}
