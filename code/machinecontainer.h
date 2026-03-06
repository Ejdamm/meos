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
    GNU General Public License fro more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Melin Software HB - software@melin.nu - www.melin.nu
    Eksoppsv�gen 16, SE-75646 UPPSALA, Sweden

************************************************************************/

#include <string>
#include <map>
#include <set>
#include <vector>

class xmlparser;
class xmlobject;

class MachineContainer {

public:

  class AbstractMachine {
    std::map<std::string, std::wstring> props;
  public:
    void clear() {
      props.clear();
    }

    int getInt(const std::string &v) const;
    const std::wstring &getString(const std::string &v) const;
    std::vector<int> getVectorInt(const std::string &v) const;
    std::set<int> getSetInt(const std::string &v) const;
    bool has(const std::string& prop) const;

    void set(const std::string &name, int v);
    void set(const std::string &name, const std::vector<int> &v);
    void set(const std::string &name, const std::wstring &v);
    void set(const std::string& name, const std::string& v) = delete;

    void set(const std::string &name, bool v) {
      set(name, int(v));
    }

    template<typename T>
    void set(const std::string &name, const T &v) {
      std::vector<int> vv;
      for (auto x : v)
        vv.push_back(x);
      set(name, vv);
    }

  protected:
    void load(const xmlobject &data);
    void load(const std::string &data);
    void save(xmlparser &data) const;
    std::string save() const;
    friend class MachineContainer;
  };

private:
  std::map<std::pair<std::string, std::wstring>, AbstractMachine> machines;

public:
  const AbstractMachine *get(const std::string &type, const std::wstring &name) const {
    auto res = machines.find(std::make_pair(type, name));
    if (res != machines.end())
      return &res->second;

    return nullptr;
  }

  void erase(const std::string &type, const std::wstring &name) {
    machines.erase(std::make_pair(type, name));
  }

  void rename(const std::string& type, const std::wstring& oldName, const std::wstring& newName);

  AbstractMachine &set(const std::string &type, const std::wstring &name) {
    auto &m = machines[std::make_pair(type, name)];
    m.clear();
    return m;
  }

  std::vector<std::pair<std::string, std::wstring>> enumerate() const;

  void load(const xmlobject &data);
  void save(xmlparser &data) const;

  void load(const std::string &data);
  std::string save();
};
