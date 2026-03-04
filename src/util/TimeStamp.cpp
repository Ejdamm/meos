#include "TimeStamp.h"
#include "meos_util.h"
#include <ctime>

using namespace std;

TimeStamp::TimeStamp()
{
  update();
}

void TimeStamp::update()
{
  Time = (unsigned int)time(NULL);
}

void TimeStamp::update(TimeStamp &ts)
{
  Time = ts.Time;
}

int TimeStamp::getAge() const
{
  return (int)(time(NULL) - Time);
}

const string& TimeStamp::getStamp() const
{
  stampCode = getStampStringN();
  return stampCode;
}

const string& TimeStamp::getStamp(const string &sqlStampIn) const
{
  stampCode = sqlStampIn;
  return stampCode;
}

const wstring TimeStamp::getUpdateTime() const
{
  return getStampString();
}

wstring TimeStamp::getStampString() const
{
  return widen(getStampStringN());
}

string TimeStamp::getStampStringN() const
{
  char bf[32];
  time_t t = Time;
  struct tm *tm_info = localtime(&t);
  if (tm_info) {
    strftime(bf, sizeof(bf), "%Y-%m-%d %H:%M:%S", tm_info);
    return string(bf);
  }
  return "";
}

void TimeStamp::setStamp(const string &s)
{
  stampCode = s;
}

TimeStamp::~TimeStamp()
{
}
