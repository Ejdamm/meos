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

#include <exception>
#include <stdexcept>
#include <string>
#include "meos_util.h"

using std::string;
using std::wstring;

class meosException : public std::runtime_error {
public:
  meosException(const wstring &wmsg) : std::runtime_error(narrow(wmsg)) {}
  meosException(const string &msg) : std::runtime_error(msg) {}
  meosException(const char *msg) : std::runtime_error(msg) {}
  meosException() : std::runtime_error("MeOS Exception") {}

  wstring wwhat() const {
    return widen(what());
  }
};
