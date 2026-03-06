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

#include <vector>
#include <string>
#include <list>
#include <fstream>

class csvparser
{
protected:
  std::ofstream fout;
  std::ifstream fin;

  int LineNumber;
  std::string ErrorMessage;

  void parseUnicode(const std::wstring &file, std::list<std::vector<std::wstring>> &data);

public:
  static void convertUTF(const std::wstring &file);

  void parse(const std::wstring &file, std::list<std::vector<std::wstring>> &dataOutput);

  bool openOutput(const std::wstring &file, bool writeUTF = false);
  bool closeOutput();

  bool outputRow(const std::vector<std::string> &out);
  bool outputRow(const std::vector<std::wstring>& out);
  bool outputRow(const std::string &row);

  static int split(char *line, std::vector<char *> &split);
  static int split(wchar_t *line, std::vector<wchar_t *> &split);

  enum class CSV {
    NoCSV,
    Unknown,
    RAID,
    OE,
    OS,
  };

  static CSV iscsv(const std::wstring &file);

  csvparser();
  virtual ~csvparser();
};
