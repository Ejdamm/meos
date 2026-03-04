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

#include <vector>
#include <string>
#include <cstring>
#include <cstdint>

typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef uint8_t BYTE;

enum class ConvertedTimeStatus {
  Unknown = 0,
  Hour12,
  Hour24,
  Done,
};

struct SIPunch {
  DWORD Code;
  DWORD Time;
};

struct SICard
{
  SICard(ConvertedTimeStatus status) {
    clear(nullptr);
    convertedTime = status;
  }
  
  void clear(const SICard *condition) {
    if (this==condition || condition==nullptr)
      memset(this, 0, sizeof(SICard));
  }

  bool empty() const {return CardNumber==0;}
  DWORD CardNumber;
  SIPunch StartPunch;
  SIPunch FinishPunch;
  SIPunch CheckPunch;
  DWORD nPunch;
  SIPunch Punch[192];
  wchar_t firstName[21];
  wchar_t lastName[21];
  wchar_t club[41];
  int miliVolt; // SIAC voltage
  char readOutTime[32];
  bool punchOnly;
  ConvertedTimeStatus convertedTime;
  
  int runnerId;
  int relativeFinishTime;
  bool statusOK;
  bool statusDNF;
  bool isDebugCard = false;
};
