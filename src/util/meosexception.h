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
#include "meos_util.h"
#include <string>
#include <exception>

class meosException : public std::exception {
  std::wstring wideMessage;
  static const char *narrow(const std::wstring& msg) {
    static std::string n;
    wide2String(msg, n);
    return n.c_str();
  }
public:
  meosException(const std::wstring &wmsg) : std::exception(), wideMessage(wmsg) {
    
  }
  meosException(const std::string &msg) : std::exception() {
    string2Wide(msg, wideMessage);
  }
  meosException(const char *msg) : std::exception() {
    string2Wide(std::string(msg), wideMessage);
  }
  meosException() : std::exception() {}

  std::wstring wwhat() const {
    return wideMessage;
  }

  virtual const char* what() const noexcept override {
    static std::string n;
    wide2String(wideMessage, n);
    return n.c_str();
  }
};

class meosCancel : public meosException {
public:
  using meosException::meosException;
};
