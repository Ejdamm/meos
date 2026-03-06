#include "StdAfx.h"
#include "Table.h"

int Table::uniqueId = 1;
const Table* TableSortIndex::table = nullptr;

Table::Table(oEvent *oe_, int rowHeight, const wstring &name, const string &tname) 
    : oe(oe_), baseRowHeight(rowHeight), tableName(name), internalName(tname) {
    id = uniqueId++;
}
Table::~Table() {}
void Table::clear() {}
void Table::addRow(int rowId, oBase *object) {}
void Table::set(int column, oBase &owner, int id, const wstring &data, bool canEdit, CellType type) {}
void Table::reserve(size_t siz) {}
void Table::update() {}
void Table::autoAdjust(gdioutput &gdi) {}
TableColSpec Table::addColumn(const string &Title, int width, bool isnum, bool formatRight) { return TableColSpec(); }
int Table::addColumn(const wstring &translatedTitle, int width, bool isnum, bool formatRight) { return 0; }
int Table::addColumnPaddedSort(const string &title, int width, int padding, bool formatRight) { return 0; }
void Table::resetColumns() {}
void Table::autoSelectColumns() {}
void Table::reloadRow(int rowId) {}
void Table::markAll(bool doSelect) {}
void Table::sort(int col, bool forceDirection) {}
void Table::filter(int col, const wstring &filt, bool forceFilter) {}
bool Table::upDown(gdioutput &gdi, int direction) { return false; }
bool Table::tabFocus(gdioutput &gdi, int direction) { return false; }
bool Table::enter(gdioutput &gdi) { return false; }
void Table::escape(gdioutput &gdi) {}
bool Table::inputChange(gdioutput &gdi, HWND hEdit) { return false; }
void Table::clearCellSelection(gdioutput *gdi) {}
void Table::hide(gdioutput &gdi) {}
void Table::selection(gdioutput &gdi, const wstring &text, int data) {}
void Table::updateDimension(gdioutput &gdi) {}
void Table::draw(gdioutput &gdi, HDC hDC, int dx, int dy, const RECT &screen) {}
void Table::print(gdioutput &gdi, HDC hDC, int dx, int dy) {}
bool Table::mouseMove(gdioutput &gdi, int x, int y) { return false; }
bool Table::mouseLeftDown(gdioutput &gdi, int x, int y) { return false; }
bool Table::mouseLeftUp(gdioutput &gdi, int x, int y) { return false; }
bool Table::mouseLeftDblClick(gdioutput &gdi, int x, int y) { return false; }
bool Table::mouseRightDown(gdioutput &gdi, int x, int y) { return false; }
bool Table::mouseRightUp(gdioutput &gdi, int x, int y) { return false; }
bool Table::mouseMidDown(gdioutput &gdi, int x, int y) { return false; }
bool Table::mouseMidUp(gdioutput &gdi, int x, int y) { return false; }
bool Table::editCell(gdioutput &gdi, int row, int col) { return false; }
bool Table::keyCommand(gdioutput &gdi, KeyCommandCode code) { return false; }
void Table::showFilter(gdioutput &gdi) {}
vector< Table::ColSelection > Table::getColumns() const { return vector<ColSelection>(); }
void Table::selectColumns(const std::set<int> &sel) {}
void Table::getDimension(gdioutput &gdi, int &dx, int &dy, bool filteredResult) const {}
int Table::getNumDataRows() const { return 0; }
TableRow *Table::getRowById(int rowId) { return nullptr; }
void Table::setTableText(gdioutput &gdi, int editRow, int editCol, const wstring &bf) {}
const wstring &Table::getTableText(gdioutput &gdi, int editRow, int editCol) { static wstring s; return s; }
void Table::addDataDefiner(const string &key, const oDataDefiner *definer) {}
void Table::insertRow(gdioutput &gdi) {}
bool Table::deleteSelection(gdioutput &gdi) { return false; }
void Table::exportClipboard(gdioutput &gdi) {}
void Table::importClipboard(gdioutput &gdi) {}
void TableRow::setObject(oBase &obj) { ob = &obj; }
