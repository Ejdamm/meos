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
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>

class xmlobject;
class xmlparser;

typedef std::vector<xmlobject> xmlList;

const int buff_pre_alloc = 1024 * 10;

struct xmldata
{
  xmldata(const char *t, char *d);
  const char *tag;
  char *data;
  int parent;
  int next;
};

struct xmlattrib {
  xmlattrib(const char *t, char *d, const xmlparser *parser);
  const char *tag;
  char *data;
  operator bool() const { return data != nullptr; }

  int getInt() const {if (data) return atoi(data); else return 0;}

  std::int64_t getInt64() const;

  std::uint64_t getInt64u() const {
    return data ? strtoull(data, nullptr, 10) : 0;
  }

  double getDouble() const {
    return data ? strtod(data, nullptr) : 0.0;
  }

  const char *getPtr() const;
  const wchar_t *getWPtr() const;

  std::string getStr() const {
    const char* ptr = getPtr();
    if (ptr == nullptr)
      ptr = "";
    return ptr;
  }

  std::wstring getWStr() const {
    const wchar_t* ptr = getWPtr();
    if (ptr == nullptr)
      ptr = L"";
    return ptr;
  }

private:
  const xmlparser *parser;
};

class xmlparser
{
protected:
  static char *ltrim(char *s);
  static const char *ltrim(const char *s);

  std::string tagStack[32];
  int tagStackPointer = 0;

  bool toString = false;
  std::ofstream foutFile;
  std::ostringstream foutString;

  std::ifstream fin;

  std::ostream &fOut() {
    if (toString)
      return foutString;
    else
      return foutFile;
  }

  int lineNumber = 0;
  std::string doctype;

  std::vector<int> parseStack;
  std::vector<xmldata> xmlinfo;
  std::vector<char> xbf;

  bool processTag(char *start, char *end);

  bool checkUTF(const char *ptr) const;
  bool parse(int maxobj);

  void convertString(const char *in, char *out, int maxlen) const;
  void convertString(const char *in, wchar_t *out, int maxlen) const;

  // True if empty/zero values are excluded when writing
  bool cutMode;

  bool isUTF;
  std::vector<char> strbuff; // Temporary buffer for processing (no threading allowed)
  std::vector<wchar_t> strbuffw; // Temporary buffer for processing (no threading allowed)

  int lastIndex;

  mutable std::wstring encodeString;
public:
  void access(int index);

  void setProgress(void* hWnd); // Changed from HWND to void*

  const xmlobject getObject(const char *pname) const;

  void read(const std::wstring &file, int maxobj = 0);
  void readMemory(const std::string &mem, int maxobj);

  void write(const char *tag, const std::string &value);
  void write(const char *tag, const std::wstring &value);
  void write(const char *tag); // Empty case
  void write(const char *tag, const char *prop,
              const std::string &value);
  void write(const char *tag, const char *prop,
              const std::wstring &value);
  void write(const char *tag, const char *prop,
             const wchar_t *value);
  void writeBool(const char *tag, const char *prop,
             const bool value);

  void writeAscii(const char *tag, const std::vector<std::pair<std::string, std::wstring>> &propValue, const std::string &valueAscii);
  void write(const char *tag, const char *prop,
             const std::wstring &propValue, const std::wstring &value);

  void writeBool(const char *tag, const char *prop,
                 bool propValue, const std::wstring &value);

  void write(const char *tag, const char *prop,
             const wchar_t *propValue, const std::wstring &value);
  void write(const char *tag, const std::vector< std::pair<std::string, std::wstring> > &propValue, const std::wstring &value);

  void write(const char *tag, const char *value);
  
  void write(const char *tag, double value);
  void write(const char *tag, int value);
  void write(const char* tag, size_t value) {
    write(tag, int(value));
  }
  
  void writeTime(const char *tag, int relativeTime);

  void writeBool(const char *tag, bool value);
  void write64(const char *tag, std::int64_t);
  void write64u(const char* tag, std::uint64_t);

  void startTag(const char *tag);
  void startTag(const char *tag, const char *Property,
                const std::string &Value);

  void startTag(const char *tag, const char *Property,
                const std::wstring &Value);
  void startTag(const char *tag, const std::vector<std::wstring> &propvalue);


  void endTag();
  int closeOut();
  void openOutput(const wchar_t *file, bool useCutMode);
  void openOutputT(const wchar_t *file, bool useCutMode, const std::string &type);

  void openMemoryOutput(bool useCutMode);
  void getMemoryOutput(std::string &res);


  const std::string &encodeXML(const std::string &input);
  const std::string &encodeXML(const std::wstring &input);

  xmlparser();
  virtual ~xmlparser();

  bool skipDefault() const {
    return cutMode;
  }

  friend class xmlobject;
  friend struct xmlattrib;
};

class xmlobject
{
protected:
  xmlobject(xmlparser *p) {parser = p; index = 0;}
  xmlobject(xmlparser *p, int i) {parser = p; index = i;}

  xmlparser *parser;
  int index;
public:
  const char *getName() const;
  xmlobject getObject(const char *pname) const;
  xmlattrib getAttrib(const char *pname) const;

  int getObjectInt(const char *pname) const {
    xmlobject x(getObject(pname));
    if (x)
      return x.getInt();
    else {
      xmlattrib xa(getAttrib(pname));
      if (xa)
        return xa.getInt();
    }
    return 0;
  }

  std::int64_t getObjectInt64(const char* pname) const;
  std::uint64_t getObjectInt64u(const char* pname) const;
  double getObjectDouble(const char* pname) const;

  bool got(const char *pname) const {
    xmlobject x(getObject(pname));
    if (x)
      return true;
    else {
      xmlattrib xa(getAttrib(pname));
      if (xa)
        return true;
    }
    return false;
  }

  bool getObjectBool(const char *pname) const;

  std::string &getObjectString(const char *pname, std::string &out) const;
  char *getObjectString(const char *pname, char *out, int maxlen) const;

  std::wstring &getObjectString(const char *pname, std::wstring &out) const;
  wchar_t *getObjectString(const char *pname, wchar_t *out, int maxlen) const;


  void getObjects(xmlList &objects) const;
  void getObjects(const char *tag, xmlList &objects) const;

  bool is(const char *pname) const;

  const char *getRawPtr() const;
  
  std::string getRawStr() const {
    const char* ptr = getRawPtr();
    if (ptr == nullptr)
      ptr = "";
    return ptr;
  }

  const char *getPtr() const;
  const wchar_t *getWPtr() const;

  std::string getStr() const {
    const char* ptr = getPtr();
    if (ptr == nullptr)
      ptr = "";
    return ptr;
  }

  std::wstring getWStr() const {
    const wchar_t* ptr = getWPtr();
    if (ptr == nullptr)
      ptr = L"";
    return ptr;
  }

  int getInt() const;

  int getRelativeTime() const;

  std::int64_t getInt64() const;

  std::uint64_t getInt64u() const;

  double getDouble() const;

  bool isnull() const {return parser==nullptr;}

  operator bool() const {return parser!=nullptr;}

  xmlobject();
  virtual ~xmlobject();
  friend class xmlparser;
};
