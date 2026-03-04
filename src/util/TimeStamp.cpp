#include "TimeStamp.h"
#include "meos_util.h"
#include "timeconstants.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cwchar>

using namespace std;

constexpr int64_t minYearConstant = 2014 - 1601;
// Jan 1, 1970 is 11644473600 seconds after Jan 1, 1601
constexpr int64_t unixEpochOffset = 11644473600LL;
constexpr int64_t meosEpochConstant = minYearConstant * 365 * 24 * 3600LL;

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
  st->wMilliseconds = 0;
#endif
}

static time_t SystemTimeToTimeT(const SYSTEMTIME &st) {
    struct tm tm_info = {0};
    tm_info.tm_year = st.wYear - 1900;
    tm_info.tm_mon = st.wMonth - 1;
    tm_info.tm_mday = st.wDay;
    tm_info.tm_hour = st.wHour;
    tm_info.tm_min = st.wMinute;
    tm_info.tm_sec = st.wSecond;
    tm_info.tm_isdst = -1;
    return mktime(&tm_info);
}

static void TimeTToSystemTime(time_t t, SYSTEMTIME &st) {
    struct tm tm_info;
    localtime_r(&t, &tm_info);
    st.wYear = tm_info.tm_year + 1900;
    st.wMonth = tm_info.tm_mon + 1;
    st.wDayOfWeek = tm_info.tm_wday;
    st.wDay = tm_info.tm_mday;
    st.wHour = tm_info.tm_hour;
    st.wMinute = tm_info.tm_min;
    st.wSecond = tm_info.tm_sec;
    st.wMilliseconds = 0;
}

TimeStamp::TimeStamp()
{
  Time=0;
}

TimeStamp::~TimeStamp()
{
}

void TimeStamp::update(TimeStamp &ts)
{
  Time=max(Time, ts.Time);
}

void TimeStamp::update()
{
  SYSTEMTIME st;
  GetLocalTime_Internal(&st);
  time_t t = SystemTimeToTimeT(st);
  int64_t currenttime_s = (int64_t)t + unixEpochOffset;
  Time = unsigned(currenttime_s - meosEpochConstant);
}

int TimeStamp::getAge() const
{
  SYSTEMTIME st;
  GetLocalTime_Internal(&st);
  time_t t = SystemTimeToTimeT(st);
  int64_t currenttime_s = (int64_t)t + unixEpochOffset;
  int CTime = int(currenttime_s - meosEpochConstant);
  return CTime-Time;
}

const string &TimeStamp::getStamp() const
{
  if (stampCodeTime == (int)Time)
    return stampCode;
  
  stampCodeTime = Time;
  int64_t currenttime_s = (int64_t)Time + meosEpochConstant;
  time_t t = (time_t)(currenttime_s - unixEpochOffset);
  
  SYSTEMTIME st;
  TimeTToSystemTime(t, st);

  char bf[64];
  snprintf(bf, 64, "%d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
  stampCode = bf;

  return stampCode;
}

const string &TimeStamp::getStamp(const string &sqlStampIn) const {
  stampCode.clear();
  stampCode.reserve(15);
  int outIx = 0;
  for (char c : sqlStampIn) {
    if (c >= '0' && c <= '9' && outIx < 14) {
      stampCode += c;
      outIx++;
    }
  }
  return stampCode;
}

const wstring TimeStamp::getUpdateTime() const {
  int64_t currenttime_s = (int64_t)Time + meosEpochConstant;
  time_t t = (time_t)(currenttime_s - unixEpochOffset);
  
  SYSTEMTIME st;
  TimeTToSystemTime(t, st);

  wchar_t bf[32];
  swprintf(bf, 32, L"%02d:%02d", st.wHour, st.wMinute);
  return bf;
}

wstring TimeStamp::getStampString() const
{
  int64_t currenttime_s = (int64_t)Time + meosEpochConstant;
  time_t t = (time_t)(currenttime_s - unixEpochOffset);
  
  SYSTEMTIME st;
  TimeTToSystemTime(t, st);

  wchar_t bf[32];
  swprintf(bf, 32, L"%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

  return bf;
}

string TimeStamp::getStampStringN() const
{
  int64_t currenttime_s = (int64_t)Time + meosEpochConstant;
  time_t t = (time_t)(currenttime_s - unixEpochOffset);
  
  SYSTEMTIME st;
  TimeTToSystemTime(t, st);

  int y = getThisYear();
  if (st.wYear > y || st.wYear < 2009) {
    st.wYear = y;
    st.wDay = 1;
    st.wMonth = 1;
    st.wHour = 2;
    st.wMinute = 0;
    st.wSecond = 0;
  }
  
  char bf[32];
  snprintf(bf, 32, "%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

  return bf;
}

void TimeStamp::setStamp(const string &s)
{
  if (s.size()<14)
    return;
  SYSTEMTIME st;
  memset(&st, 0, sizeof(st));

  auto parse = [](const char* data, int size, const char*& next) -> int {
    int ix = 0;
    int out = 0;
    while (ix < size && data[ix] >= '0' && data[ix] <= '9') {
      out = (out * 10) + data[ix] - '0';
      ix++;
    }
    while (data[ix] && (data[ix] == ' ' || data[ix] == '-' || data[ix] == ':')) {
      ix++;
    }
    next = data + ix;
    return out;
  };

  const char* ptr = s.data();

  st.wYear = parse(ptr, 4, ptr);
  st.wMonth = parse(ptr, 2, ptr);
  st.wDay = parse(ptr, 2, ptr);
  st.wHour = parse(ptr, 2, ptr);
  st.wMinute = parse(ptr, 2, ptr);
  st.wSecond = parse(ptr, 2, ptr);

  time_t t = SystemTimeToTimeT(st);
  int64_t currenttime_s = (int64_t)t + unixEpochOffset;

  Time = unsigned(currenttime_s - meosEpochConstant);
}
