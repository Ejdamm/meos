#pragma once

/************************************************************************
    MeOS - Orienteering Software
    Copyright (C) 2009-2026 Melin Software HB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Melin Software HB - software@melin.nu - www.melin.nu
    Eksoppsvägen 16, SE-75646 UPPSALA, Sweden

************************************************************************/

#include <string>
#include <vector>
#include <map>
#include <set>
#include "domain_header.h"
#include "oListInfo.h"

using std::string;
using std::wstring;
using std::vector;
using std::map;
using std::set;
using std::pair;

class oEvent;
class gdioutput;
struct oListParam;
class oListInfo;
class DynamicResult;

class MetaList {
public:
    virtual ~MetaList() {}
    static EPostType getTypeFromSymbol(const string& s) { return lNone; }
    static EPostType getTypeFromSymbol(const wstring& s) { return lNone; }
    const wstring& getListName() const { static wstring dummy; return dummy; }
    void retagResultModule(const string& oldTag, const string& newTag) {}
};

class MetaListContainer {
public:
  void synchronizeTo(MetaListContainer &dst) const {}
  bool interpret(oEvent *oe, const gdioutput &gdi, const oListParam &par, oListInfo &li) const { return true; }
  void enumerateLists(vector< pair<wstring, pair<string, wstring> > > &out) const {}
  void enumerateLists(vector< pair<wstring, pair<string, wstring> > > &out, bool b1, bool b2, bool b3, bool b4) const {}
  EStdListType getCodeFromUnqiueId(const string& id) { return NoneType; }
  void updateGeneralResult(const string& tag, const shared_ptr<DynamicResult>& res) {}
  const MetaList& getList(EStdListType type) const { static MetaList dummy; return dummy; }
  void getLists(vector<pair<wstring, size_t>>& out, bool b1, bool b2, bool b3, bool b4) const {}
  EStdListType getType(int id) const { return NoneType; }
  EStdListType getType(size_t id) const { return NoneType; }
  string getUniqueId(EStdListType type) const { return ""; }
};

struct DynamicResultRef {
  shared_ptr<DynamicResult> res;
  MetaList *ctr;
  DynamicResultRef(const shared_ptr<DynamicResult> &resIn, MetaList *ctrIn) : res(resIn), ctr(ctrIn) {}
  DynamicResultRef() : ctr(0) {}
};

struct Position {
  struct PosInfo {
    int x, y;
    void operator=(const PosInfo &) {throw std::runtime_error("Unsupported");}
  };
};
