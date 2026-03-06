#include "StdAfx.h"
#include "localizer.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <cwctype>
#include <filesystem>
#include "meos_util.h"
#include "owordlist.h"
#include "meosexception.h"

using namespace std;

namespace fs = std::filesystem;

// Global instance
Localizer lang;

class LocalizerImpl
{
  wstring language;
  map<wstring, wstring> table;
  map<wstring, wstring> unknown;
  void loadTable(const vector<string> &raw, const wstring &language);
  mutable oWordList *givenNames;

  void addUnknown(const wstring& var);

public:
  const oWordList &getGivenNames() const;

  void translateAll(const LocalizerImpl &all);

  const wstring &translate(const wstring &str, bool &found);

  void saveUnknown(const wstring &file);
  void saveTable(const wstring &file);
  void saveTranslation(const wstring &file);

  void loadTable(const wstring &file, const wstring &language);
  void loadTable(int resource, const wstring &language);

  void clear();
  LocalizerImpl(void);
  ~LocalizerImpl(void);
};

Localizer::LocalizerInternal::LocalizerInternal(void)
{
  impl = new LocalizerImpl();
  implBase = nullptr;
  owning = true;
  user = nullptr;
}

Localizer::LocalizerInternal::~LocalizerInternal(void)
{
  if (user) {
    user->owning = true;
    impl = nullptr;
    implBase = nullptr;
  }
  else {
    delete impl;
    delete implBase;
  }
}

void Localizer::LocalizerInternal::set(Localizer &lio) {
  Localizer::LocalizerInternal &li = *lio.linternal;
  if (li.user || user)
    throw std::runtime_error("Runtime error");

  if (owning) {
    delete impl;
    delete implBase;
  }

  implBase = li.implBase;
  impl = li.impl;
  li.user = this;
}

vector<wstring> Localizer::LocalizerInternal::getLangResource() const {
  vector<wstring> v;
  for (auto it = langResource.begin(); it != langResource.end(); ++it)
    v.push_back(it->first);

  return v;
}

const oWordList &Localizer::LocalizerInternal::getGivenNames() const {
  return impl->getGivenNames();
}

LocalizerImpl::LocalizerImpl(void)
{
  givenNames = nullptr;
}

LocalizerImpl::~LocalizerImpl(void)
{
  if (givenNames)
    delete givenNames;
}

const wstring &Localizer::LocalizerInternal::tl(const wstring &str) const {
  bool found;
  const wstring *ret = &impl->translate(str, found);
  if (found || !implBase)
    return *ret;

  ret = &implBase->translate(str, found);
  return *ret;
}

const wstring &LocalizerImpl::translate(const wstring &str, bool &found) {
  found = false;
  static int i = 0;
  const int bsize = 17;
  static wstring value[bsize];
  int len = (int)str.length();

  if (len==0)
    return _EmptyWString;

  if (str[0]=='#') {
    i = (i + 1)%bsize;
    value[i] = str.substr(1);
    found = true;
    return value[i];
  }

  auto isDigit = [](wchar_t c) {
    return c >= '0' && c <= '9';
  };

  if (str[0]==',' || str[0]==' ' || str[0]=='.'
       || str[0]==':'  || str[0]==';' || str[0]=='<' || str[0]=='>' 
       || str[0]=='-' || str[0]==L'–' || str[0]==L'×' || isDigit(str[0])) {
    unsigned k=1;
    while(k < str.length() && (str[k]==' ' || str[k]=='.' || str[k]==':' || str[k]=='<' || str[k]=='>'
           || str[k]=='-' || str[k]==L'–' || str[k] == L'×' || isDigit(str[k])))
      k++;

    if (k<str.length()) {
      wstring sub = str.substr(k);
      i = (i + 1)%bsize;
      value[i] = str.substr(0, k) + translate(sub, found);
      return value[i];
    }
  }

  auto it = table.find(str);
  if (it != table.end()) {
    found = true;
    return it->second;
  }

  size_t subst = str.find_first_of('#');
  if (subst != wstring::npos) {
    wstring s = translate(str.substr(0, subst), found);
    vector<wstring> split_vec;
    split(str.substr(subst+1), L"#", split_vec);
    split_vec.push_back(L"");
    const wchar_t *subsymb = L"XYZW";
    size_t subpos = 0;
    wstring ret;
    size_t lastpos = 0;
    for (size_t k = 0; k<s.size(); k++) {
      if (subpos>=split_vec.size() || subpos>=4)
        break;
      if (s[k] == subsymb[subpos]) {
        if (k>0 && iswalnum(s[k-1]))
          continue;
        if (k+1 < s.size() && iswalnum(s[k+1]))
          continue;
        ret += s.substr(lastpos, k-lastpos);
        ret += split_vec[subpos];
        lastpos = k+1;
        subpos++;
      }
    }
    if (lastpos<s.size())
      ret += s.substr(lastpos);

    i = (i + 1)%bsize;
    value[i] = std::move(ret);
    return value[i];
  }
  else if (str[0] == '@') {
    // Untranslated string with substitution
    i = (i + 1) % bsize;
    value[i] = str.substr(1);
    found = true;
    return value[i];
  }

  wchar_t last = str[len-1];
  if (last != ':' && last != '.' && last != ' ' && last != ',' &&
      last != ';' && last != '<' && last != '>' && last != '-' &&
      last != L'–' && last != L'×' && !isDigit(last)) {
#ifdef _DEBUG
    if (str.length()>1)
      addUnknown(str);
#endif
    found = false;
    i = (i + 1)%bsize;
    value[i] = str;
    return value[i];
  }

  wstring suffix;
  int pos = (int)str.find_last_not_of(last);

  while(pos>0) {
    wchar_t last = str[pos];
    if (last != ':' && last != ' ' && last != ',' && last != '.' &&
        last != ';' && last != '<' && last != '>' && last != '-' && last != L'–' && last != L'×' && !isDigit(last))
      break;

    pos = (int)str.find_last_not_of(last, pos);
  }

  suffix = str.substr(pos+1);

  wstring key = str.substr(0, str.length()-suffix.length());
  it = table.find(key);
  if (it != table.end()) {
    i = (i + 1)%bsize;
    value[i] = it->second + suffix;
    found = true;
    return value[i];
  }
#ifdef _DEBUG
  if (key.length() > 1) {
    try {
      if (stoi(key) == 0) addUnknown(key);
    } catch(...) {}
  }
#endif

  found = false;
  i = (i + 1)%bsize;
  value[i] = str;
  return value[i];
}

void LocalizerImpl::addUnknown(const wstring& key) {
  unknown.emplace(key, L"");
}

void LocalizerImpl::saveUnknown(const wstring &file)
{
  const wstring newline = L"\n";

  if (!unknown.empty()) {
    ofstream fout(narrow(file).c_str(), ios::trunc|ios::out);
    for (auto it = unknown.begin(); it!=unknown.end(); ++it) {
      wstring value = it->second;
      wstring key = it->first;
      if (value.empty()) {
        value = key;

        size_t nl = value.find(newline);
        size_t n2 = value.find(L".");

        if (nl!=wstring::npos || n2!=wstring::npos) {
          while (nl!=wstring::npos) {
            value.replace(nl, newline.length(), L"\\n");
            nl = value.find(newline);
          }
          key = L"help:" + to_wstring(value.length()) + to_wstring(value.find_first_of('.'));
        }
      }
      else {
        size_t nl = value.find(newline);
        while (nl != wstring::npos) {
          value.replace(nl, newline.length(), L"\\n");
          nl = value.find(newline);
        }
      }
      fout << toUTF8(key) << " = " << toUTF8(value) << endl;
    }
  }
}

const oWordList &LocalizerImpl::getGivenNames() const {
  if (givenNames == nullptr) {
    // getUserFile stub/replacement
    wstring path = L"wgiven.mwd"; // Simplified
    givenNames = new oWordList();
    try {
      givenNames->load(path);
    } catch(std::exception &) {}
  }
  return *givenNames;
}

void Localizer::LocalizerInternal::loadLangResource(const wstring &name) {
  auto it = langResource.find(name);
  if (it == langResource.end())
    throw std::runtime_error("Unknown language");

  wstring &res = it->second;

  // Resource mapping for Linux
  static map<wstring, wstring> resMap = {
    {L"103", L"swedish.lng"},
    {L"104", L"english.lng"},
    {L"105", L"german.lng"},
    {L"106", L"danish.lng"},
    {L"107", L"russian.lng"},
    {L"108", L"czech.lng"},
    {L"110", L"french.lng"},
    {L"111", L"spanish.lng"},
    {L"112", L"ukrainian.lng"},
    {L"113", L"portuguese.lng"},
    {L"114", L"bulgarian.lng"}
  };

  wstring filename = res;
  if (resMap.count(res)) {
      filename = resMap[res];
  }

  // Try different locations
  vector<fs::path> searchPaths = {
      fs::path("lang") / filename,
      fs::path("resources/lang") / filename,
      fs::path("../src/app/lang") / filename, // For development/tests
      fs::path(filename)
  };

  bool loaded = false;
  for (const auto& p : searchPaths) {
      if (fs::exists(p)) {
          impl->loadTable(p.wstring(), name);
          loaded = true;
          break;
      }
  }

  if (!loaded) {
      // Fallback to English if file not found
      if (res != L"104" && name != L"English") {
          addLangResource(L"English", L"104");
          loadLangResource(L"English");
      } else {
           throw std::runtime_error("Language file not found: " + narrow(filename));
      }
  }
}

void Localizer::LocalizerInternal::addLangResource(const wstring &name, const wstring &resource) {
  langResource[name] = resource;
  if (implBase == nullptr) {
    implBase = new LocalizerImpl();
    try {
        loadLangResource(name);
        // After loading into impl, we swap it to implBase if it's the first one
        delete implBase;
        implBase = impl;
        impl = new LocalizerImpl();
    } catch(...) {
        // Ignore errors during base load
    }
  }
}

void Localizer::LocalizerInternal::debugDump(const wstring &untranslated, const wstring &translated) const {
  if (implBase) {
    impl->translateAll(*implBase);
  }
  impl->saveUnknown(untranslated);
  impl->saveTable(translated);
  impl->saveTranslation(L"spellcheck.txt");
}

void LocalizerImpl::translateAll(const LocalizerImpl &all) {
  bool f;
  for (auto it = all.table.begin(); it != all.table.end(); ++it) {
    translate(it->first, f);
    if (!f) {
      unknown[it->first] = it->second;
    }
  }
}

void LocalizerImpl::saveTable(const wstring &file) {
  const wstring newline = L"\n";
  wstring filename = language + L"_" + file;
  ofstream fout(narrow(filename).c_str(), ios::trunc|ios::out);
  for (auto it = table.begin(); it!=table.end(); ++it) {
    wstring value = it->second;
    size_t nl = value.find(newline);
    while (nl!=wstring::npos) {
      value.replace(nl, newline.length(), L"\\n");
      nl = value.find(newline);
    }
    fout << toUTF8(it->first) << " = " << toUTF8(value) << endl;
  }
}

void LocalizerImpl::saveTranslation(const wstring &file) {
  wstring filename = language + L"_" + file;
  ofstream fout(narrow(filename).c_str(), ios::trunc | ios::out);
  for (auto it = table.begin(); it != table.end(); ++it) {
    fout << toUTF8(it->second) << endl;
  }
}

void LocalizerImpl::loadTable(int id, const wstring &language)
{
    // Not used on Linux, resource IDs are mapped to files in loadLangResource
    loadTable(to_wstring(id), language);
}

void LocalizerImpl::loadTable(const wstring &file, const wstring &language)
{
  clear();
  ifstream fin(narrow(file).c_str(), ios::in);

  if (!fin.good())
    return;

  string line_str;
  vector<string> raw;
  while (getline(fin, line_str)) {
    if (!line_str.empty() && line_str[0] != '#')
      raw.push_back(line_str);
  }

  loadTable(raw, language);
}

void LocalizerImpl::loadTable(const vector<string> &raw, const wstring &language)
{
  table.clear();
  this->language = language;
  string nline = "\n";
  for (size_t k=0;k<raw.size();k++) {
    const string &s = raw[k];
    size_t pos = s.find_first_of('=');

    if (pos==string::npos)
      continue; // Skip bad lines instead of throwing

    size_t spos = pos;
    size_t epos = pos+1;
    const unsigned char *udata = (const unsigned char*)s.data();

    // Trim spaces
    while (spos > 0) {
      if (isspace(udata[spos - 1]))
        spos--;
      else if (udata[spos - 1] == 0xC2 && spos > 1 && udata[spos - 2] == 0xA0) //NBSP
        spos -= 2;
      else
        break;
    }

    while (epos < s.size()) {
      if (isspace(udata[epos]))
        epos++;
      else if (udata[epos] == 0xC2 && epos + 1 < s.size() && udata[epos + 1] == 0xA0) //NBSP
        epos += 2;
      else
        break;
    }

    string key = s.substr(0, spos);
    string value = s.substr(epos);

    if (value.empty())
      continue;

    // Remove BOM or weird character if present (from original code)
    if (value.size() > 2 && (unsigned char)value[0] == 0xC3 && (unsigned char)value[1] == 0x82) {
       // value = value.substr(2); // In UTF-8, Â is C3 82
    }

    size_t nl = value.find("\\n");
    while (nl!=string::npos) {
      value.replace(nl, 2, nline);
      nl = value.find("\\n");
    }

    table[fromUTF8(key)] = fromUTF8(value);
  }
}

void LocalizerImpl::clear()
{
  table.clear();
  unknown.clear();
  language.clear();
}

bool Localizer::capitalizeWords() const {
  return tl("Lyssna") == L"Listen";
}

const wstring &Localizer::tl(const string &str) const {
  if (str.length() == 0)
    return _EmptyWString;
  wstring key = widen(str);
  return linternal->tl(key);
}

const wstring Localizer::tl(const wstring &str, bool cap) const {
  wstring w = linternal->tl(str);
  if (cap && capitalizeWords())
    ::capitalizeWords(w);

  return w;
}
