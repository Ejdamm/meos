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
#include "meosexception.h"
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cstring>
#include <cwchar>
#include <cmath>
#include <cctype>
#include <cwctype>
#include <thread>
#include <filesystem>
#include <fstream>
#include <cassert>

#ifndef _WIN32
#include <unistd.h>
#include <sys/stat.h>
#endif

using namespace std;

const wstring _EmptyWString = L"";
const string _EmptyString = "";

StringCache globalStringCache;

namespace MeOSUtil {
  int useHourFormat = true;
}

static void GetLocalTime_Internal(SYSTEMTIME *st) {
#ifdef _WIN32
  GetLocalTime(st);
#else
  time_t t = time(NULL);
  struct tm tm_info;
  localtime_r(&t, &tm_info);
  st->wYear = tm_info.tm_year + 1900;
  st->wMonth = tm_info.tm_mon + 1;
  st->wDayOfWeek = tm_info.tm_wday;
  st->wDay = tm_info.tm_mday;
  st->wHour = tm_info.tm_hour;
  st->wMinute = tm_info.tm_min;
  st->wSecond = tm_info.tm_sec;
  st->wMilliseconds = 0; // time() doesn't provide ms
#endif
}

StringCache &StringCache::getInstance() {
  // Simple implementation for now
  return globalStringCache;
}

string getLocalTimeN() {
  SYSTEMTIME st;
  GetLocalTime_Internal(&st);
  return convertSystemTimeN(st);
}

wstring getLocalTime() {
  SYSTEMTIME st;
  GetLocalTime_Internal(&st);
  return convertSystemTime(st);
}

wstring getLocalDate() {
  SYSTEMTIME st;
  GetLocalTime_Internal(&st);
  return convertSystemDate(st);
}

int getLocalAbsTime() {
  return convertAbsoluteTimeHMS(getLocalTimeOnly(), -1);
}

int getThisYear() {
  SYSTEMTIME st;
  GetLocalTime_Internal(&st);
  return st.wYear;
}

int extendYear(int year) {
  if (year < 0) return year;
  if (year >= 100) return year;
  int thisYear = getThisYear();
  int cLast = thisYear % 100;
  if (cLast == 0 && year == 0) return thisYear;
  if (year > thisYear % 100)
    return (thisYear - cLast) - 100 + year;
  else
    return (thisYear - cLast) + year;
}

wstring getLocalTimeFileName() {
  SYSTEMTIME st;
  GetLocalTime_Internal(&st);
  wchar_t bf[32];
  swprintf(bf, 32, L"%d%02d%02d_%02d%02d%02d", st.wYear, st.wMonth, st.wDay,
           st.wHour, st.wMinute, st.wSecond);
  return bf;
}

wstring getLocalTimeOnly() {
  SYSTEMTIME st;
  GetLocalTime_Internal(&st);
  return convertSystemTimeOnly(st);
}

string convertSystemTimeN(const SYSTEMTIME &st) {
  char bf[64];
  snprintf(bf, 64, "%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay,
           st.wHour, st.wMinute, st.wSecond);
  return bf;
}

wstring convertSystemTime(const SYSTEMTIME &st) {
  wchar_t bf[64];
  swprintf(bf, 64, L"%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay,
           st.wHour, st.wMinute, st.wSecond);
  return bf;
}

wstring convertSystemTimeOnly(const SYSTEMTIME &st) {
  wchar_t bf[32];
  swprintf(bf, 32, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
  return bf;
}

wstring convertSystemDate(const SYSTEMTIME &st) {
  wchar_t bf[32];
  swprintf(bf, 32, L"%d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
  return bf;
}

wstring formatDate(int m, bool useIsoFormat) {
  wchar_t bf[24];
  if (m > 0 && m < 30000101) {
    swprintf(bf, 24, L"%d-%02d-%02d", m / (100 * 100), (m / 100) % 100, m % 100);
  } else {
    bf[0] = '-';
    bf[1] = 0;
  }
  return bf;
}

int convertDateYMD(const string &m, SYSTEMTIME &st, bool checkValid) {
  memset(&st, 0, sizeof(st));
  if (m.length() == 0) return -1;
  int dashCount = 0;
  for (char b : m) {
    if (b == 'T') break;
    if (b == '-') dashCount++;
  }
  int year = atoi(m.c_str());
  if (dashCount == 0) {
    int day = year % 100;
    year /= 100;
    int month = year % 100;
    year /= 100;
    if (year < 100) year = extendYear(year);
    if (year < 1900 || year > 3000) return -1;
    if (checkValid && (month < 1 || month > 12 || day < 1 || day > 31)) return -1;
    st.wYear = year; st.wMonth = month; st.wDay = day;
    return year * 10000 + month * 100 + day;
  }
  if (year < 100) year = extendYear(year);
  size_t kp = m.find('-');
  if (kp != string::npos) {
    string mtext = m.substr(kp + 1);
    int month = atoi(mtext.c_str());
    kp = mtext.find('-');
    if (kp != string::npos) {
      int day = atoi(mtext.substr(kp + 1).c_str());
      st.wYear = year; st.wMonth = month; st.wDay = day;
      return year * 10000 + month * 100 + day;
    }
  }
  return -1;
}

int convertDateYMD(const wstring &m, SYSTEMTIME &st, bool checkValid) {
  string sm;
  wide2String(m, sm);
  return convertDateYMD(sm, st, checkValid);
}

int convertDateYMD(const string &m, bool checkValid) {
  SYSTEMTIME st;
  return convertDateYMD(m, st, checkValid);
}

int convertDateYMD(const wstring &m, bool checkValid) {
  SYSTEMTIME st;
  return convertDateYMD(m, st, checkValid);
}

int convertAbsoluteTimeHMS(const string &m, int daysZeroTime) {
  if (m.empty() || m[0] == '-') return -1;
  size_t tix = m.find_first_of("DdTt");
  if (tix != string::npos) {
    if (daysZeroTime < 0) return -1;
    int tpart = convertAbsoluteTimeHMS(m.substr(tix + 1), -1);
    if (tpart != -1) {
      int days = atoi(m.c_str());
      if (tpart < daysZeroTime) days--;
      return days * 24 * timeConstHour + tpart;
    }
    return -1;
  }
  int hour = atoi(m.c_str());
  int minute = 0, second = 0, tenth = 0;
  size_t kp = m.find(':');
  if (kp != string::npos) {
    string mtext = m.substr(kp + 1);
    minute = atoi(mtext.c_str());
    kp = mtext.find(':');
    if (kp != string::npos) {
      second = atoi(mtext.substr(kp + 1).c_str());
      kp = mtext.find('.');
      if (kp != string::npos) tenth = atoi(mtext.substr(kp + 1).c_str());
    }
  }
  return hour * timeConstHour + minute * timeConstMinute + second * timeConstSecond + tenth;
}

int convertAbsoluteTimeHMS(const wstring &m, int daysZeroTime) {
  string sm;
  wide2String(m, sm);
  return convertAbsoluteTimeHMS(sm, daysZeroTime);
}

int convertAbsoluteTimeMS(const string &m) {
  if (m.empty()) return NOTIME;
  int sign = 1;
  string mtext = m;
  if (m[0] == '-') {
    sign = -1;
    mtext = m.substr(1);
  }
  int minute = atoi(mtext.c_str());
  int second = 0, hour = 0, tenth = 0;
  size_t kp = mtext.find(':');
  if (kp != string::npos) {
    string sText = mtext.substr(kp + 1);
    second = atoi(sText.c_str());
    kp = sText.find(':');
    if (kp != string::npos) {
      hour = minute;
      minute = second;
      second = atoi(sText.substr(kp + 1).c_str());
    }
  }
  return sign * (hour * timeConstHour + minute * timeConstMinute + second * timeConstSecond + tenth);
}

int convertAbsoluteTimeMS(const wstring &m) {
  string sm;
  wide2String(m, sm);
  return convertAbsoluteTimeMS(sm);
}

const wstring &formatTimeMS(int m, bool force2digit, SubSecond mode) {
  wchar_t bf[64];
  int am = abs(m);
  if (am < timeConstHour || !MeOSUtil::useHourFormat) {
    swprintf(bf, 64, L"%s%d:%02d", m < 0 ? L"-" : L"", am / timeConstMinute, (am / timeConstSecond) % 60);
  } else {
    swprintf(bf, 64, L"%s%d:%02d:%02d", m < 0 ? L"-" : L"", am / timeConstHour, (am / timeConstMinute) % 60, (am / timeConstSecond) % 60);
  }
  wstring &res = StringCache::getInstance().wget();
  res = bf;
  return res;
}

const wstring &formatTime(int rt, SubSecond mode) {
  wstring &res = StringCache::getInstance().wget();
  if (rt > 0 && rt < timeConstHour * 999) {
    wchar_t bf[64];
    if (rt >= timeConstHour && MeOSUtil::useHourFormat)
      swprintf(bf, 64, L"%d:%02d:%02d", rt / timeConstHour, (rt / timeConstMinute) % 60, (rt / timeConstSecond) % 60);
    else
      swprintf(bf, 64, L"%d:%02d", rt / timeConstMinute, (rt / timeConstSecond) % 60);
    res = bf;
    return res;
  }
  res = L"-";
  return res;
}

const wstring &formatTimeHMS(int rt, SubSecond mode) {
  wstring &res = StringCache::getInstance().wget();
  if (rt >= 0) {
    wchar_t bf[64];
    swprintf(bf, 64, L"%02d:%02d:%02d", rt / timeConstHour, (rt / timeConstMinute) % 60, (rt / timeConstSecond) % 60);
    res = bf;
    return res;
  }
  res = L"-";
  return res;
}

#include <codecvt>
#include <locale>

void string2Wide(const string &in, wstring &out) {
  try {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    out = converter.from_bytes(in);
  } catch (...) {
    out.assign(in.begin(), in.end());
  }
}

void wide2String(const wstring &in, string &out) {
  try {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    out = converter.to_bytes(in);
  } catch (...) {
    out.assign(in.begin(), in.end());
  }
}

bool fileExists(const wstring &file) {
  string sfile;
  wide2String(file, sfile);
  return filesystem::exists(sfile);
}

const string &itos(int i) {
  string &res = StringCache::getInstance().get();
  res = to_string(i);
  return res;
}

string itos(unsigned int i) { return to_string(i); }
string itos(int64_t i) { return to_string(i); }
string itos(uint64_t i) { return to_string(i); }

const wstring &itow(int i) {
  wstring &res = StringCache::getInstance().wget();
  res = to_wstring(i);
  return res;
}

wstring itow(unsigned int i) { return to_wstring(i); }
wstring itow(int64_t i) { return to_wstring(i); }
wstring itow(uint64_t i) { return to_wstring(i); }

const vector<string> &split(const string &line, const string &separators, vector<string> &split_vector) {
  split_vector.clear();
  if (line.empty()) return split_vector;
  size_t start = 0, end;
  while ((end = line.find_first_of(separators, start)) != string::npos) {
    split_vector.push_back(line.substr(start, end - start));
    start = end + 1;
  }
  split_vector.push_back(line.substr(start));
  return split_vector;
}

const vector<wstring> &split(const wstring &line, const wstring &separators, vector<wstring> &split_vector) {
  split_vector.clear();
  if (line.empty()) return split_vector;
  size_t start = 0, end;
  while ((end = line.find_first_of(separators, start)) != wstring::npos) {
    split_vector.push_back(line.substr(start, end - start));
    start = end + 1;
  }
  split_vector.push_back(line.substr(start));
  return split_vector;
}

wstring trim(const wstring &s) {
  if (s.empty()) return s;
  size_t first = s.find_first_not_of(L" \t\r\n");
  if (first == wstring::npos) return L"";
  size_t last = s.find_last_not_of(L" \t\r\n");
  return s.substr(first, last - first + 1);
}

string trim(const string &s) {
  if (s.empty()) return s;
  size_t first = s.find_first_not_of(" \t\r\n");
  if (first == string::npos) return "";
  size_t last = s.find_last_not_of(" \t\r\n");
  return s.substr(first, last - first + 1);
}

const wstring &encodeXML(const wstring &in) {
  wstring &out = StringCache::getInstance().wget();
  out.clear();
  for (wchar_t c : in) {
    switch (c) {
      case L'&': out += L"&amp;"; break;
      case L'<': out += L"&lt;"; break;
      case L'>': out += L"&gt;"; break;
      case L'\"': out += L"&quot;"; break;
      case L'\'': out += L"&apos;"; break;
      default: out += c; break;
    }
  }
  return out;
}

const string &encodeXML(const string &in) {
  string &out = StringCache::getInstance().get();
  out.clear();
  for (char c : in) {
    switch (c) {
      case '&': out += "&amp;"; break;
      case '<': out += "&lt;"; break;
      case '>': out += "&gt;"; break;
      case '\"': out += "&quot;"; break;
      case '\'': out += "&apos;"; break;
      default: out += c; break;
    }
  }
  return out;
}

int toLowerStripped(wchar_t c) {
    if (c >= L'A' && c <= L'Z') return c + (L'a' - L'A');
    // Simplified: just return lowercase for now. 
    // In a real migration we would include the full accent mapping.
    return towlower(c);
}

void MeOSFileLock::lockFile(const wstring &file) {
    // Stub
}

void MeOSFileLock::unlockFile() {
    // Stub
}

void moveFile(const wstring& src, const wstring& dst) {
    string ssrc, sdst;
    wide2String(src, ssrc);
    wide2String(dst, sdst);
    filesystem::rename(ssrc, sdst);
}

int compareStringIgnoreCase(const wstring &a, const wstring &b) {
    // Simplified case-insensitive comparison
    wstring wa = a, wb = b;
    transform(wa.begin(), wa.end(), wa.begin(), ::towlower);
    transform(wb.begin(), wb.end(), wb.begin(), ::towlower);
    if (wa < wb) return -1;
    if (wa > wb) return 1;
    return 0;
}

// Implement other missing functions as stubs or simplified versions
int parseRelativeTime(const char *data) {
  if (data) {
    int ret = atoi(data);
    if (timeConstSecond == 10) {
      int j = 0;
      while (data[j]) {
        if (data[j] == '.') {
          int t = data[j + 1] - '0';
          if (t > 0 && t < 10) {
            if (ret < 0 || data[0] == '-')
              return ret * timeConstSecond - t;
            else
              return ret * timeConstSecond + t;
          }
          break;
        }
        j++;
      }
    }
    else if (timeConstSecond == 100) {
      int j = 0;
      while (data[j]) {
        if (data[j] == '.') {
          int t = data[j + 1] - '0';
          if (t >= 0 && t < 10) {
            t *= 10;
            int t2 = data[j + 2] - '0';
            if (t2 > 0 && t2 < 10)
              t += t2;

            if (ret < 0 || data[0] == '-')
              return ret * timeConstSecond - t;
            else
              return ret * timeConstSecond + t;
          }
          break;
        }
        j++;
      }
    }
    if (ret == -1)
      return ret; // Special value

    return ret * timeConstSecond;
  }
  return 0;
}

int parseRelativeTime(const wchar_t *data) {
  if (data) {
    int ret = (int)wcstol(data, nullptr, 10);
    if (timeConstSecond == 10) {
      int j = 0;
      while (data[j]) {
        if (data[j] == '.') {
          int t = data[j + 1] - '0';
          if (t > 0 && t < 10) {
            if (ret < 0 || data[0] == '-')
              return ret * timeConstSecond - t;
            else
              return ret * timeConstSecond + t;
          }
          break;
        }
        j++;
      }
    }
    else if (timeConstSecond == 100) {
      int j = 0;
      while (data[j]) {
        if (data[j] == '.') {
          int t = data[j + 1] - '0';
          if (t >= 0 && t < 10) {
            t *= 10;
            int t2 = data[j + 2] - '0';
            if (t2 > 0 && t2 < 10)
              t += t2;

            if (ret < 0 || data[0] == '-')
              return ret * timeConstSecond - t;
            else
              return ret * timeConstSecond + t;
          }
          break;
        }
        j++;
      }
    }
    if (ret == -1)
      return ret; // Special value

    return ret * timeConstSecond;
  }
  return 0;
}

const wstring &codeRelativeTimeW(int rt) { static wstring res; return res; }
const string &codeRelativeTime(int rt) { static string res; return res; }
int64_t SystemTimeToInt64TenthSecond(const SYSTEMTIME &st) { return 0; }
SYSTEMTIME Int64TenthSecondToSystemTime(int64_t time) { SYSTEMTIME st = {0}; return st; }
int convertAbsoluteTimeISO(const wstring &m) { return 0; }
wstring addOrSubtractDays(const wstring& m, int days) { return m; }
const wstring &limitText(const wstring& tIn, size_t numChar) { return tIn; }
wstring ensureEndingColon(const wstring &text) { return text; }
const wstring &makeDash(const wstring &t) { return t; }
const wstring &makeDash(const wchar_t *t) { static wstring res; res = t; return res; }
wstring formatRank(int rank) { return L""; }
bool filterMatchString(const wstring &c, const wchar_t *filt_lc, int &score) { return false; }
void prepareMatchString(wchar_t* data_c, int size) {}
bool matchNumber(int number, const wchar_t *key) { return false; }
int getMeosBuild() { return 0; }
wstring getMeosDate() { return L""; }
wstring getMeosFullVersion() { return L""; }
wstring getMajorVersion() { return L""; }
wstring getMeosCompectVersion() { return L""; }
void getSupporters(vector<wstring> &supp, vector<wstring> &developSupp) {}
int countWords(const wchar_t *p) { return 0; }
bool stringMatch(const wstring &a, const wstring &b) { return a == b; }
const char *decodeXML(const char *in)
{
  if (in == nullptr) return nullptr;
  const char *bf = in;
  bool needDecode = false;
  for (int k=0; bf[k] ;k++)
    needDecode |=  (bf[k]=='&');

  if (!needDecode)
    return in;

  string &res = StringCache::getInstance().get();
  res.clear();
  res.reserve(strlen(in));
  for (int k=0;bf[k] ;k++) {
    if (bf[k] != '&')
      res += bf[k];
    else {
      if ( memcmp(&bf[k], "&amp;", 5)==0 )
        res += '&', k+=4;
      else if  ( memcmp(&bf[k], "&lt;", 4)==0 )
        res += '<', k+=3;
      else if  ( memcmp(&bf[k], "&gt;", 4)==0 )
        res += '>', k+=3;
      else if  ( memcmp(&bf[k], "&quot;", 6)==0 )
        res += '"', k+=5;
      else if  ( memcmp(&bf[k], "&apos;", 6)==0 )
        res += '\'', k+=5;
      else if  ( memcmp(&bf[k], "&#10;", 5)==0 )
        res += '\n', k+=4;
      else if  ( memcmp(&bf[k], "&#13;", 5)==0 )
        res += '\r', k+=4;
      else
        res += bf[k];
    }
  }
  return res.c_str();
}

const string &decodeXML(const string &in)
{
  static string res;
  res = decodeXML(in.c_str());
  return res;
}

void inplaceDecodeXML(char *in)
{
  if (in == nullptr) return;
  char *bf = in;
  int outp = 0;

  for (int k=0;bf[k] ;k++) {
    if (bf[k] != '&')
      bf[outp++] = bf[k];
    else {
      if ( memcmp(&bf[k], "&amp;", 5)==0 )
        bf[outp++] = '&', k+=4;
      else if  ( memcmp(&bf[k], "&lt;", 4)==0 )
        bf[outp++] = '<', k+=3;
      else if  ( memcmp(&bf[k], "&gt;", 4)==0 )
        bf[outp++] = '>', k+=3;
      else if  ( memcmp(&bf[k], "&quot;", 6)==0 )
        bf[outp++] = '"', k+=5;
      else if  ( memcmp(&bf[k], "&apos;", 6)==0 )
        bf[outp++] = '\'', k+=5;
      else if  ( memcmp(&bf[k], "&#10;", 5)==0 )
        bf[outp++] = '\n', k+=4;
      else if  ( memcmp(&bf[k], "&#13;", 5)==0 )
        bf[outp++] = '\r', k+=4;
      else
        bf[outp++] = bf[k];
    }
  }
  bf[outp] = 0;
}
const wstring &encodeHTML(const wstring &in) { return in; }
const wchar_t *canonizeName(const wchar_t *name) { return name; }
double stringDistance(const wchar_t *a, const wchar_t *b) { return 0; }
double stringDistanceAssymetric(const wstring &target, const wstring &sample) { return 0; }
int getNumberSuffix(const string &str) { return 0; }
int getNumberSuffix(const wstring &str) { return 0; }
int extractAnyNumber(const wstring &str, wstring &prefix, wstring &suffix) { return 0; }
bool compareClassName(const wstring &a, const wstring &b) { return a == b; }
wstring getErrorMessage(int code) { return L""; }
void HLS::lighten(double f) {}
void HLS::saturate(double s) {}
void HLS::colorDegree(double d) {}
HLS &HLS::RGBtoHLS(uint32_t lRGBColor) { return *this; }
uint32_t HLS::HLStoRGB() const { return 0; }
void unzip(const wchar_t *zipfilename, const char *password, vector<wstring> &extractedFiles) {}
int zip(const wchar_t *zipfilename, const char *password, const vector<wstring> &files) { return 0; }
bool isAscii(const wstring &s) { return true; }
bool isNumber(const wstring &s) { return true; }
bool isAscii(const string &s) { return true; }
bool isNumber(const string &s) { return true; }
int convertDynamicBase(const wstring &s, long long &out) {
  out = 0;
  if (s.empty())
    return 0;

  bool alpha = false;
  bool general = false;
  int len = s.length();
  for (int k = 0; k < len; k++) {
    unsigned c = s[k];
    if (c >= '0' && c <= '9')
      continue;
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
      alpha = true;
      continue;
    }
    general = true;
    if (c<32)
      return 0; // Not a supported character
  }

  int base = general ? 256-32 : (alpha ? 36 : 10);
  long long factor = 1;
  for (int k = len-1; k >= 0; k--) {
    unsigned c = s[k]&0xFF;
    if (general)
      c -= 32;
    else {
      if (c >= '0' && c <= '9')
        c -= '0';
      else if (c >= 'A' && c <= 'Z')
        c -= 'A'-10;
      else if (c >= 'a' && c <= 'z')
        c -= 'a'-10;
    }
    out += factor * c;
    factor *= base;
  }

  return base;
}

void convertDynamicBase(long long val, int base, wchar_t out[16]) {
  int len = 0;
  if (val == 0) {
    out[len++] = '0';
  } else {
    while (val != 0) {
      unsigned int c = val % base;
      val = val / base;
      wchar_t cc;
      if (base == 10)
        cc = '0' + c;
      else if (base == 36) {
        if (c < 10)
          cc = '0' + c;
        else
          cc = 'A' + c - 10;
      }
      else {
        cc = c + 32;
      }
      out[len++] = cc;
    }
  }
  out[len] = 0;
  std::reverse(out, out+len);
}
bool expandDirectory(const wchar_t *dir, const wchar_t *pattern, vector<wstring> &res) { return false; }
PersonSex interpretSex(const wstring &sex) { return sUnknown; }
wstring encodeSex(PersonSex sex) { return L""; }
wstring makeValidFileName(const wstring &input, bool strict) { return input; }
string makeValidFileName(const string& input, bool strict) { return input; }
void capitalize(wstring &str) {}
void capitalizeWords(wstring &str) {}
wstring getTimeZoneString(const wstring &date) { return L""; }
int getTimeZoneInfo(const wstring &date) { return 0; }
bool compareBib(const wstring &b1, const wstring &b2) { return b1 == b2; }
wstring getGivenName(const wstring &name) { return name; }
wstring getFamilyName(const wstring &name) { return name; }
void checkWriteAccess(const wstring &file) {}
wstring formatTimeIOF(int rt, int zeroTime) { return L""; }
void processGeneralTime(const wstring &generalTime, wstring &meosTime, wstring &meosDate) {}
uint16_t HLS::HueToRGB(uint16_t n1, uint16_t n2, uint16_t hue) const { return 0; }
