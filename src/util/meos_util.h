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
#include <map>
#include <string>
#include <cstdint>
#include "timeconstants.hpp"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
struct SYSTEMTIME {
  uint16_t wYear;
  uint16_t wMonth;
  uint16_t wDayOfWeek;
  uint16_t wDay;
  uint16_t wHour;
  uint16_t wMinute;
  uint16_t wSecond;
  uint16_t wMilliseconds;
};

typedef void* HANDLE;
#define INVALID_HANDLE_VALUE (HANDLE)(-1)
#endif

#define NOTIME 0x7FFFFFFF

class StringCache {
private:
  std::vector<std::string> cache;
  size_t ix;

  std::vector<std::wstring> wcache;
  size_t wix;
public:
  static StringCache &getInstance();

  StringCache() : ix(0), wix(0) { init(); }

  void init() { if (cache.empty()) cache.resize(256); if (wcache.empty()) wcache.resize(256); }
  void clear() { cache.clear(); wcache.clear(); }

  std::string &get() {
    if ((++ix) >= cache.size())
      ix = 0;
    size_t lx = ix;
    return cache[lx];
  }

  std::wstring &wget() {
    if ((++wix) >= wcache.size())
      wix = 0;
    size_t lx = wix;
    return wcache[lx];
  }
};

std::string convertSystemTimeN(const SYSTEMTIME &st);
std::string getLocalTimeN();

bool checkValidDate(const std::wstring &date);

std::wstring convertSystemTime(const SYSTEMTIME &st);
std::wstring convertSystemTimeOnly(const SYSTEMTIME &st);
std::wstring convertSystemDate(const SYSTEMTIME &st);
std::wstring getLocalTime();
std::wstring getLocalDate();
std::wstring getLocalTimeOnly();
// Returns time in seconds after midnight
int getLocalAbsTime();

// Get a day number after a fixed day some time ago...
int getRelativeDay();

/// Get time and date in a format that forms a part of a filename
std::wstring getLocalTimeFileName();

enum class SubSecond {
  Off,
  On,
  Auto
};

int parseRelativeTime(const char *data);
int parseRelativeTime(const wchar_t *data);

const std::wstring &codeRelativeTimeW(int rt);
const std::string &codeRelativeTime(int rt);

// Format time MM:SS.t (force2digit=true) or M:SS.t (force2digit=false)
const std::wstring &formatTimeMS(int m, bool force2digit, SubSecond mode = SubSecond::Auto);
const std::wstring &formatTime(int rt, SubSecond mode = SubSecond::Auto);
const std::wstring &formatTimeHMS(int rt, SubSecond mode = SubSecond::Auto);

std::wstring formatTimeIOF(int rt, int zeroTime);

int convertDateYMD(const std::string &m, bool checkValid);
int convertDateYMD(const std::string &m, SYSTEMTIME &st, bool checkValid);

int convertDateYMD(const std::wstring &m, bool checkValid);
int convertDateYMD(const std::wstring &m, SYSTEMTIME &st, bool checkValid);

// Convert a "general" time string to a MeOS compatible time string
void processGeneralTime(const std::wstring &generalTime, std::wstring &meosTime, std::wstring &meosDate);

// Format number date 20160421 -> 2016-04-21 (if iso) or according to a custom format otherwise
//std::string formatDate(int m, bool useIsoFormat);
std::wstring formatDate(int m, bool useIsoFormat);

int64_t SystemTimeToInt64TenthSecond(const SYSTEMTIME &st);
SYSTEMTIME Int64TenthSecondToSystemTime(int64_t time);

//Returns a time converted from +/-MM:SS or NOTIME, in MeOS time unit
int convertAbsoluteTimeMS(const std::wstring &m);
int convertAbsoluteTimeMS(const std::string& m);

// Parses a time on format HH:MM:SS+01:00Z or HHMMSS+0100Z (but ignores time zone)
int convertAbsoluteTimeISO(const std::wstring &m);

/** Returns a time converted from HH:MM:SS or -1, in MeOS time unit 
   @param m time to convert
   @param daysZeroTime -1 do not support days syntax, positive interpret days w.r.t the specified zero time.
*/
int convertAbsoluteTimeHMS(const std::string &m, int daysZeroTime);

/** Returns a time converted from HH:MM:SS or -1, in MeOS time unit
   @param m time to convert
   @param daysZeroTime -1 do not support days syntax, positive interpret days w.r.t the specified zero time.
*/
int convertAbsoluteTimeHMS(const std::wstring &m, int daysZeroTime);

// Add or subtract a number of days from a date in Y-M-D format
std::wstring addOrSubtractDays(const std::wstring& m, int days);

const std::vector<std::string> &split(const std::string &line, const std::string &separators, std::vector<std::string> &split_vector);

const std::vector<std::wstring> &split(const std::wstring &line, const std::wstring &separators, std::vector<std::wstring> &split_vector);

template<typename T>
const T& unsplit(const std::vector<T>& split_vector, const T& separators, T& line);

// Compare two strings, ignore case. 0 = equal, != zero compares as the integers.
int compareStringIgnoreCase(const std::wstring &a, const std::wstring &b);
const std::wstring &limitText(const std::wstring& tIn, size_t numChar);
std::wstring ensureEndingColon(const std::wstring &text);

const std::wstring &makeDash(const std::wstring &t);
const std::wstring &makeDash(const wchar_t *t);

std::wstring formatRank(int rank);
const std::string &itos(int i);
std::string itos(unsigned int i);
std::string itos(int64_t i);
std::string itos(uint64_t i);

const std::wstring &itow(int i);
std::wstring itow(unsigned int i);
std::wstring itow(int64_t i);
std::wstring itow(uint64_t i);


///Lower case match (filt_lc must be lc and stripped of accents)
bool filterMatchString(const std::wstring &c, const wchar_t *filt_lc, int &score);

/** To lower case and strip accants */
void prepareMatchString(wchar_t* data_c, int size);


bool matchNumber(int number, const wchar_t *key);

int getMeosBuild();
std::wstring getMeosDate();
std::wstring getMeosFullVersion();
std::wstring getMajorVersion();
std::wstring getMeosCompectVersion();

void getSupporters(std::vector<std::wstring> &supp, std::vector<std::wstring> &developSupp);

int countWords(const wchar_t *p);

std::wstring trim(const std::wstring &s);
std::string trim(const std::string &s);

bool fileExists(const std::wstring &file);

bool stringMatch(const std::wstring &a, const std::wstring &b);

const char *decodeXML(const char *in);
const std::string &decodeXML(const std::string &in);
void inplaceDecodeXML(char *in);
const std::string &encodeXML(const std::string &in);
const std::wstring &encodeXML(const std::wstring &in);
const std::wstring &encodeHTML(const std::wstring &in);

/** Extend a year from 03 -> 2003, 97 -> 1997 etc */
int extendYear(int year);

/** Get current year, e.g., 2010 */
int getThisYear();

/** Translate a char to lower/stripped of accents etc.*/
int toLowerStripped(wchar_t c);

/** Canonize a person/club name */
//const char *canonizeName(const char *name);
const wchar_t *canonizeName(const wchar_t *name);

/** String distance between 0 and 1. 0 is equal*/
double stringDistance(const wchar_t *a, const wchar_t *b);

/** Return how close sample is to target. 1.0 means equal*/
double stringDistanceAssymetric(const std::wstring &target, const std::wstring &sample);

/** Get a number suffix, Start 1 -> 1. Zero for none*/
int getNumberSuffix(const std::string &str);
int getNumberSuffix(const std::wstring &str);

/// Extract any number from a string and return the number, prefix and suffix
int extractAnyNumber(const std::wstring &str, std::wstring &prefix, std::wstring &suffix);


/** Compare classnames, match H21 Elit with H21E and H21 E */
bool compareClassName(const std::wstring &a, const std::wstring &b);

/** Get WinAPI error from code */
std::wstring getErrorMessage(int code);

class HLS {
private:
  uint16_t HueToRGB(uint16_t n1, uint16_t n2, uint16_t hue) const;
public:

  HLS(uint16_t H, uint16_t L, uint16_t S) : hue(H), lightness(L), saturation(S) {}
  HLS() : hue(0), lightness(0), saturation(1) {}
  short hue;
  short lightness;
  short saturation;
  void lighten(double f);
  void saturate(double s);
  void colorDegree(double d);
  HLS &RGBtoHLS(uint32_t lRGBColor);
  uint32_t HLStoRGB() const;
};

void unzip(const wchar_t *zipfilename, const char *password, std::vector<std::wstring> &extractedFiles);
int zip(const wchar_t *zipfilename, const char *password, const std::vector<std::wstring> &files);

bool isAscii(const std::wstring &s);
bool isNumber(const std::wstring &s);

bool isAscii(const std::string &s);
bool isNumber(const std::string &s);
int convertDynamicBase(const std::wstring &s, long long &out);
void convertDynamicBase(long long val, int base, wchar_t out[16]);

/// Find all files in dir matching given file pattern
bool expandDirectory(const wchar_t *dir, const wchar_t *pattern, std::vector<std::wstring> &res);

enum RunnerStatus {
  StatusOK = 1, StatusDNS = 20, StatusCANCEL = 21, StatusOutOfCompetition = 15, StatusMP = 3,
  StatusDNF = 4, StatusDQ = 5, StatusMAX = 6, StatusNoTiming = 2,
  StatusUnknown = 0, StatusNotCompeting = 99
};

enum class DynamicRunnerStatus {
  StatusInactive,
  StatusActive,
  StatusFinished
};

enum PersonSex {sFemale = 1, sMale, sBoth, sUnknown};

PersonSex interpretSex(const std::wstring &sex);

std::wstring encodeSex(PersonSex sex);

std::wstring makeValidFileName(const std::wstring &input, bool strict);
std::string makeValidFileName(const std::string& input, bool strict);

/** Initial capital letter. */
void capitalize(std::wstring &str);

/** Initial capital letter for each word. */
void capitalizeWords(std::wstring &str);

std::wstring getTimeZoneString(const std::wstring &date);

/** Return bias in seconds. UTC = local time + bias. */
int getTimeZoneInfo(const std::wstring &date);

/** Compare bib numbers (which may contain non-digits, e.g. A-203, or 301a, 301b)*/
bool compareBib(const std::wstring &b1, const std::wstring &b2);

/** Split a name into Given, Family, and return Given.*/
std::wstring getGivenName(const std::wstring &name);

/** Split a name into Given, Family, and return Family.*/
std::wstring getFamilyName(const std::wstring &name);

/** Simple file locking class to prevent opening in different MeOS session. */
class MeOSFileLock {
  HANDLE lockedFile;
  // Not supported
  MeOSFileLock(const MeOSFileLock &);
  const MeOSFileLock &operator=(const MeOSFileLock &);

public:
  MeOSFileLock() {lockedFile = INVALID_HANDLE_VALUE;}
  ~MeOSFileLock() {unlockFile();}

  void unlockFile();
  void lockFile(const std::wstring &file);
};

namespace MeOSUtil {
  extern int useHourFormat;
}

void string2Wide(const std::string &in, std::wstring &out);
void wide2String(const std::wstring &in, std::string &out);

void checkWriteAccess(const std::wstring &file);

void moveFile(const std::wstring& src, const std::wstring& dst);
