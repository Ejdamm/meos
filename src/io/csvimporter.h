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

#pragma once

#include "csvparser.h"
#include <vector>
#include <map>
#include <list>
#include <string>

class oEvent;
struct SICard;

struct PunchInfo {
  int code;
  int card;
  int time;
  char date[28];
};

struct TeamLineup {
  struct TeamMember {
    std::wstring name;
    std::wstring club;
    int cardNo;
    std::wstring course;
    std::wstring cls;
  };

  std::wstring teamName;
  std::wstring teamClass;
  std::wstring teamClub;
  std::vector<TeamMember> members;
};

class CSVLineWrapper;

class csvimporter : public csvparser
{
public:
  int nimport;
  
  void importTeamLineup(const std::wstring &file,
                        const std::map<std::wstring, int> &classNameToNumber,
                        std::vector<TeamLineup> &teams);

  bool importOCAD_CSV(oEvent &oe, const std::wstring &file, bool addClasses);
  bool importOS_CSV(oEvent &oe, const std::wstring &file);
  bool importRAID(oEvent &oe, const std::wstring &file);
  bool importOE_CSV(oEvent &oe, const std::wstring &file);

  bool importPunches(const oEvent &oe, const std::wstring &file,
                     std::vector<PunchInfo> &punches);

  bool importCards(const oEvent &oe, const std::wstring &file,
                   std::vector<SICard> &cards);

  int importRanking(oEvent &oe, const std::wstring &file, std::vector<std::wstring> & problems);

  csvimporter();

protected:
  enum SIConfigFields {
    sicSIID,
    sicCheck,
    sicCheckTime,
    sicCheckDOW,
    sicStart,
    sicStartTime,
    sicStartDOW,
    sicFinish,
    sicFinishTime,
    sicFinishDOW,
    sicNumPunch,
    sicRecordStart,
    sicFirstName,
    sicLastName,
  };

  std::map<SIConfigFields, int> siconfigmap;
  const wchar_t *getSIC(SIConfigFields sic, const CSVLineWrapper&sp) const;

  bool checkSimanLine(const oEvent &oe, const CSVLineWrapper &sp, SICard &card);
  void checkSIConfigHeader(const CSVLineWrapper &sp);
  bool checkSIConfigLine(const oEvent &oe, const CSVLineWrapper &sp, SICard &card);

  static int selectPunchIndex(const std::wstring &competitionDate, const CSVLineWrapper &sp,
                              int &cardIndex, int &timeIndex, int &dateIndex,
                              std::wstring &processedTime, std::wstring &processedDate);
};
