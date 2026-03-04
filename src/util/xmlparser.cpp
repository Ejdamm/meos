#include "xmlparser.h"
#include "meos_util.h"
#include "meosexception.h"
#include <algorithm>
#include <cstring>
#include <cwchar>
#include <iostream>

using namespace std;

xmlparser::xmlparser()
{
  lastIndex = 0;
  tagStackPointer = 0;
  isUTF = false;
  cutMode = false;
  toString = false;
  strbuff.resize(buff_pre_alloc);
  strbuffw.resize(buff_pre_alloc);
}

xmlparser::~xmlparser()
{
  fin.close();
  foutFile.close();
}

inline bool isBlankSpace(char b) {
  return b == ' ' || b == '\t' || b == '\n' || b == '\r';
}

void xmlparser::setProgress(void* hWnd)
{
  // Progress not supported on Linux CLI
}

void xmlparser::access(int index) {
  if ((index-lastIndex)>1000 ) {
    lastIndex = index;
  }
}

xmlobject::xmlobject()
{
  parser = nullptr;
  index = 0;
}

xmlobject::~xmlobject()
{
}

const string &xmlparser::encodeXML(const wstring &input) {
  string s;
  wide2String(input, s);
  return ::encodeXML(s);
}

const string &xmlparser::encodeXML(const string &input) {
  return ::encodeXML(input);
}

void xmlparser::write(const char *tag, const wstring &Value)
{
  if (!cutMode || !Value.empty()) {
    auto &valEnc = encodeXML(Value);
    if (valEnc.length() > 400) {
      fOut() << "<" << tag << ">"
        << valEnc
        << "</" << tag << ">" << endl;
    }
    else {
      char bf[512];
      snprintf(bf, 512, "<%s>%s</%s>\n", tag, valEnc.c_str(), tag);
      fOut() << bf;
    }
  }
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::write(const char *tag, const string &Value)
{
  if (!cutMode || !Value.empty()) {
    auto &valEnc = encodeXML(Value);
    if (valEnc.length() > 400) {
      fOut() << "<" << tag << ">"
        << valEnc
        << "</" << tag << ">" << endl;
    }
    else {
      char bf[512];
      snprintf(bf, 512, "<%s>%s</%s>\n", tag, valEnc.c_str(), tag);
      fOut() << bf;
    }
  }
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::write(const char *tag, const char *Value)
{
  if (!cutMode || (Value && Value[0])) {
    auto &valEnc = encodeXML(Value ? string(Value) : string());
    if (valEnc.length() > 400) {
      fOut() << "<" << tag << ">"
        << valEnc
        << "</" << tag << ">" << endl;
    }
    else {
      char bf[512];
      snprintf(bf, 512, "<%s>%s</%s>\n", tag, valEnc.c_str(), tag);
      fOut() << bf;
    }
  }
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::write(const char *tag)
{
  char bf[128];
  snprintf(bf, 128, "<%s/>\n", tag);
  fOut() << bf;
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::write(const char *tag, const char *Property, const string &Value)
{
  if (!cutMode || !Value.empty()) {
    fOut() << "<" << tag << " " << Property << "=\""
           << encodeXML(Value) << "\"/>\n";
  }
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::write(const char *tag, const char *Property, const wstring &Value)
{
  if (!cutMode || !Value.empty()) {
    fOut() << "<" << tag << " " << Property << "=\""
           << encodeXML(Value) << "\"/>\n";
  }
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::write(const char *tag, const char *prop, const wchar_t *value)
{
  encodeString = value ? value : L"";
  write(tag, prop, encodeString);
}

void xmlparser::writeBool(const char *tag, const char *prop, bool value)
{
  if (!cutMode || value)
    write(tag, prop, value ? L"true" : L"false");
}

void xmlparser::writeAscii(const char *tag, const vector<pair<string, wstring>>& propValue,
                           const string &valueAscii) {
  if (!cutMode || !valueAscii.empty()) {
    fOut() << "<" << tag;
    for (size_t k = 0; k < propValue.size(); k++) {
      fOut() << " " << propValue[k].first << "=\"" << encodeXML(propValue[k].second) << "\"";
    }
    fOut() << ">" << ::encodeXML(valueAscii)
           << "</" << tag << ">" << endl;
  }
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::write(const char *tag, const char *Property, const wstring &PropValue, const wstring &Value)
{
  if (!cutMode || !Value.empty() || !PropValue.empty()) {
    fOut() << "<" << tag << " " << Property << "=\""
           << encodeXML(PropValue) << "\">" << encodeXML(Value)
           << "</" << tag << ">\n";
  }
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::write(const char *tag, const vector< pair<string, wstring> > &propValue, const wstring &value) {
  if (!cutMode || !value.empty() || !propValue.empty()) {
    fOut() << "<" << tag;
    for (size_t k = 0; k < propValue.size(); k++) {
      fOut() << " " << propValue[k].first << "=\"" << encodeXML(propValue[k].second) << "\"";
    }
    if (!value.empty()) {
      fOut() << ">" << encodeXML(value)
             << "</" << tag << ">\n";
    }
    else
      fOut() << "/>\n";
  }
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::writeBool(const char *tag, const char *prop,
                      bool propValue, const wstring &value) {
  static const wstring wTrue = L"true";
  static const wstring wFalse = L"false";
  write(tag, prop, propValue ? wTrue : wFalse, value);
}

void xmlparser::write(const char *tag, const char *prop,
                      const wchar_t *propValue, const wstring &value) {
  write(tag, prop, wstring(propValue ? propValue : L""), value);
}

void xmlparser::write(const char *tag, int Value) {
  if (!cutMode || Value!=0) {
    char bf[256];
    snprintf(bf, 256, "<%s>%d</%s>\n", tag, Value, tag);
    fOut() << bf;
  }
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::write(const char* tag, double value) {
  if (!cutMode || value != 0.0) {
    char bf[256];
    snprintf(bf, 256, "<%s>%g</%s>\n", tag, value, tag);
    fOut() << bf;
  }
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::writeTime(const char *tag, int relativeTime) {
  if (!cutMode || relativeTime != 0) {
    char bf[256];
    int subSec = timeConstSecond == 1 ? 0 : relativeTime % timeConstSecond;

    if (timeConstSecond == 1 || relativeTime == -1)
      snprintf(bf, 256, "<%s>%d</%s>\n", tag, relativeTime, tag);
    else if (subSec == 0 && relativeTime != -10) 
      snprintf(bf, 256, "<%s>%d</%s>\n", tag, relativeTime / timeConstSecond, tag);
    else if (relativeTime >= 0)
      if (timeConstSecond == 10)
        snprintf(bf, 256, "<%s>%d.%d</%s>\n", tag, (relativeTime / timeConstSecond), 
                                               (relativeTime % timeConstSecond), tag);
      else
        snprintf(bf, 256, "<%s>%d.%02d</%s>\n", tag, (relativeTime / timeConstSecond),
                                                 (relativeTime % timeConstSecond), tag);
    else {
      int at = std::abs(relativeTime);
      if (timeConstSecond == 10)
        snprintf(bf, 256, "<%s>-%d.%d</%s>\n", tag, (at / timeConstSecond),
                                                (at % timeConstSecond), tag);
      else
        snprintf(bf, 256, "<%s>-%d.%02d</%s>\n", tag, (at / timeConstSecond),
                                                  (at % timeConstSecond), tag);
    }
    fOut() << bf;
  }
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::writeBool(const char *tag, bool value)
{
  if (!cutMode || value) {
    fOut() << "<" << tag << ">" << (value ? "true" : "false") << "</" << tag << ">\n";
  }
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::write64(const char* tag, std::int64_t value) {
  if (!cutMode || value!=0) {
    fOut() << "<" << tag << ">"
           << value
           << "</" << tag << ">\n";
  }
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::write64u(const char* tag, std::uint64_t value) {
  if (!cutMode || value != 0) {
    fOut() << "<" << tag << ">"
      << value
      << "</" << tag << ">\n";
  }
  if (!fOut().good())
    throw meosException("Writing to XML file failed.");
}

void xmlparser::startTag(const char *tag, const char *prop, const wstring &Value)
{
  if (tagStackPointer<32) {
    fOut() << "<" << tag << " " << prop << "=\"" << encodeXML(Value) << "\">\n";
    tagStack[tagStackPointer++]=tag;
    if (!fOut().good())
      throw meosException("Writing to XML file failed.");
  }
  else
    throw meosException("Tag depth too large.");
}


void xmlparser::startTag(const char *tag, const char *prop, const string &Value)
{
  if (tagStackPointer<32) {
    fOut() << "<" << tag << " " << prop << "=\"" << encodeXML(Value) << "\">\n";
    tagStack[tagStackPointer++]=tag;
    if (!fOut().good())
      throw meosException("Writing to XML file failed.");
  }
  else
    throw meosException("Tag depth too large.");
}

void xmlparser::startTag(const char *tag, const vector<wstring> &propvalue)
{
  if (tagStackPointer<32) {
    fOut() << "<" << tag ;
    for (size_t k=0;k+1<propvalue.size(); k+=2) {
      fOut() << " " << encodeXML(propvalue[k]) << "=\"" << encodeXML(propvalue[k+1]) << "\"";
    }
    fOut() << ">\n";
    tagStack[tagStackPointer++]=tag;
    if (!fOut().good())
      throw meosException("Writing to XML file failed.");
  }
  else
    throw meosException("Tag depth too large.");
}

void xmlparser::startTag(const char *tag)
{
  if (tagStackPointer<32) {
    fOut() << "<" << tag << ">\n";
    tagStack[tagStackPointer++]=tag;
    if (!fOut().good())
      throw meosException("Writing to XML file failed.");
  }
  else
    throw meosException("Tag depth too large.");
}

void xmlparser::endTag()
{
  if (tagStackPointer>0) {
    fOut() << "</" << tagStack[--tagStackPointer] << ">\n";
    if (!fOut().good())
      throw meosException("Writing to XML file failed.");
  }
  else throw std::runtime_error("BAD XML CODE");
}

void xmlparser::openMemoryOutput(bool useCutMode) {
  cutMode = useCutMode;
  toString = true;
  foutString.str("");
  foutString.clear();
  fOut() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n\n";
}

void xmlparser::getMemoryOutput(string &res) {
  res = foutString.str();
  foutString.str("");
}

void xmlparser::openOutput(const wchar_t *file, bool useCutMode)
{
  openOutputT(file, useCutMode, "");
}

void xmlparser::openOutputT(const wchar_t *file, bool useCutMode, const string &type) {
  toString = false;
  cutMode = useCutMode;
  wstring wfile(file);
  string sfile;
  wide2String(wfile, sfile);
  foutFile.open(sfile);
  checkWriteAccess(wfile);
  tagStackPointer=0;

  if (foutFile.bad())
    throw meosException(L"Kunde inte skriva till '" + wfile + L"'");

  fOut() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n\n";

  if (!type.empty()) {
    startTag(type.c_str());
  }
}

int xmlparser::closeOut()
{
  while(tagStackPointer>0)
    endTag();

  int len = (int)foutFile.tellp();
  foutFile.close();

  return len;
}

xmldata::xmldata(const char *t, char *d) : tag(t), data(d)
{
  parent = -1;
  next = 0;
}

xmlattrib::xmlattrib(const char *t, char *d, const xmlparser *p) : tag(t), data(d), parser(p) {}

void xmlparser::read(const wstring &file, int maxobj)
{
  string sfile;
  wide2String(file, sfile);
  fin.open(sfile, ios::binary);

  if (!fin.good())
    throw meosException(L"Failed to open for reading: " + file);

  char bf[1024];
  bf[0]=0;

  do {
    fin.getline(bf, 1024, '>');
    lineNumber++;
  }
  while(fin.good() && bf[0]==0);

  char *ptr=ltrim(bf);
  isUTF = checkUTF(ptr);
  int p1 = (int)fin.tellg();

  fin.seekg(0, ios::end);
  int p2 = (int)fin.tellg();
  fin.seekg(p1, ios::beg);

  int asize = p2-p1;
  if (maxobj>0)
    asize = min(asize, maxobj*256);

  xbf.resize(asize+1);
  xmlinfo.clear();
  xmlinfo.reserve(xbf.size() / 30);

  parseStack.clear();

  fin.read(&xbf[0], xbf.size());
  xbf[asize] = 0;

  fin.close();

  parse(maxobj);
}

void xmlparser::readMemory(const string &mem, int maxobj)
{
  if (mem.empty())
    return;

  char bf[1024];
  bf[0] = mem[0];
  int i = 1;
  int stop = min<int>(1020, mem.length());
  while (i < stop && mem[i-1] != '>'){
    bf[i] = mem[i];
    i++;
  }
  bf[i] = 0;

  char *ptr=ltrim(bf);
  isUTF = checkUTF(ptr);
  int p1 = i;
  int p2 = mem.size();

  int asize = p2-p1;
  if (maxobj>0)
    asize = min(asize, maxobj*256);

  xbf.resize(asize+1);
  xmlinfo.clear();
  xmlinfo.reserve(xbf.size() / 30);

  parseStack.clear();

  memcpy(&xbf[0], mem.c_str() + p1, xbf.size());
  xbf[asize] = 0;

  parse(maxobj);
}

bool xmlparser::checkUTF(const char *ptr) const {
  bool utf = false;

  if (uint8_t(ptr[0]) == 0xEF && uint8_t(ptr[1])==0xBB && uint8_t(ptr[2])==0xBF) {
    utf = true;
    ptr+=3;
  }

  if (memcmp(ptr, "<?xml", 5) == 0) {
    int i = 5;
    bool hasEncode = false;
    while (ptr[i]) {
      if ((ptr[i] == 'U' || ptr[i] == 'u') && strncasecmp(ptr+i, "UTF-8", 5)==0) {
        utf = true;
        break;
      }
      if (ptr[i] == 'e' && memcmp(ptr+i, "encoding", 8)==0) {
        hasEncode = true;
      }
      i++;
    }
    if (!hasEncode)
      utf = true;
  }
  else if (ptr[0] == '<' && ptr[1] == '?') {
    utf = true;
  }
  return utf;
}

bool xmlparser::parse(int maxobj) {
  lineNumber=0;
  int pp = 0;
  const int size = (int)xbf.size()-2;
  while (pp < size) {
    while (pp < size && xbf[pp] != '<') pp++;

    if (xbf[pp] == '<') {
      xbf[pp] = 0;
      char *start = &xbf[pp+1];
      while (pp < size && xbf[pp] != '>') pp++;

      if (xbf[pp] == '>') {
        xbf[pp] = 0;
      }
      if (*start=='!')
        continue;

      processTag(start, &xbf[pp-1]);
    }

    if (maxobj>0 && int(xmlinfo.size()) >= maxobj) {
      xbf[pp+1] = 0;
      return true;
    }
    pp++;
  }

  lastIndex = 0;
  return true;
}

bool xmlparser::processTag(char *start, char *end) {
  bool onlyAttrib = *end == '/';
  bool isEndTag = *start == '/';

  char *tag = start;

  if (isEndTag)
    tag++;

  while (start<=end && !isBlankSpace(*start))
    start++;

  *start = 0;

  if (!isEndTag && !onlyAttrib) {
    parseStack.push_back(xmlinfo.size());
    xmlinfo.push_back(xmldata(tag, end+2));
    int p = (int)parseStack.size()-2;
    xmlinfo.back().parent = p>=0 ? parseStack[p] : -1;
  }
  else if (isEndTag) {
    if (!parseStack.empty()){
      xmldata &xd = xmlinfo[parseStack.back()];
      inplaceDecodeXML(xd.data);
      if (strcmp(tag, xd.tag)== 0) {
        parseStack.pop_back();
        xd.next = xmlinfo.size();
      }
      else {
        throw std::runtime_error("Unmatched XML tag");
      }
    }
    else
    {
       throw std::runtime_error("Unmatched XML tag");
    }
  }
  else if (onlyAttrib) {
    *end = 0;
    xmlinfo.push_back(xmldata(tag, nullptr));
    int p = (int)parseStack.size() - 1;
    xmlinfo.back().parent = p>=0 ? parseStack[p] : -1;
    xmlinfo.back().next = (int)xmlinfo.size();
  }
  return true;
}

char * xmlparser::ltrim(char *s)
{
  while(*s && isspace((unsigned char)(*s)))
    s++;
  return s;
}

const char * xmlparser::ltrim(const char *s)
{
  while(*s && isspace((unsigned char)(*s)))
    s++;
  return s;
}

xmlobject xmlobject::getObject(const char *pname) const
{
  if (pname == nullptr)
    return *this;
  if (isnull())
     throw std::runtime_error("Null pointer exception");

  vector<xmldata> &xmlinfo = parser->xmlinfo;

  parser->access(index);

  unsigned child = index+1;
  while (child < xmlinfo.size() && xmlinfo[child].parent == index) {
    if (strcmp(xmlinfo[child].tag, pname)==0)
      return xmlobject(parser, (int)child);
    else
      child = xmlinfo[child].next;
  }
  return xmlobject(nullptr);
}


void xmlobject::getObjects(xmlList &obj) const
{
  obj.clear();

  if (isnull())
    throw std::runtime_error("Null pointer exception");

  vector<xmldata> &xmlinfo = parser->xmlinfo;
  unsigned child = index+1;
  parser->access(index);

  while (child < xmlinfo.size() && xmlinfo[child].parent == index) {
    obj.push_back(xmlobject(parser, (int)child));
    child = xmlinfo[child].next;
  }
}

void xmlobject::getObjects(const char *tag, xmlList &obj) const
{
  obj.clear();

  if (isnull())
    throw std::runtime_error("Null pointer exception");

  vector<xmldata> &xmlinfo = parser->xmlinfo;
  unsigned child = index+1;
  parser->access(index);

  while (child < xmlinfo.size() && xmlinfo[child].parent == index) {
    if (strcmp(tag, xmlinfo[child].tag) == 0)
      obj.push_back(xmlobject(parser, (int)child));
    child = xmlinfo[child].next;
  }
}


const xmlobject xmlparser::getObject(const char *pname) const
{
  if (!xmlinfo.empty()){
    if (pname == nullptr || strcmp(xmlinfo[0].tag, pname) == 0)
      return xmlobject(const_cast<xmlparser *>(this), 0);
    else return xmlobject(const_cast<xmlparser *>(this), 0).getObject(pname);
  }
  else return xmlobject(nullptr);
}

xmlattrib xmlobject::getAttrib(const char *pname) const
{
  if (pname != nullptr) {
    char *start = const_cast<char *>(parser->xmlinfo[index].tag);
    const char *end = parser->xmlinfo[index].data;

    if (end)
      end-=2;
    else {
      if (size_t(index + 1) < parser->xmlinfo.size())
        end = parser->xmlinfo[index+1].tag - 1;
      else
        end = &parser->xbf.back();
    }

    while (start<end && *start != 0)
      start++;
    start++;

    char *oldStart = start;
    while (start<end) {
      while(start<end && isBlankSpace(*start))
        start++;

      char *tag = start;

      while(start<end && *start!='=' && *start!=0)
        start++;

      if (start<end && (start[1]=='"' || start[1] == 0)) {
        *start = 0;
        ++start;
        char *value = ++start;

        while(start<end && (*start!='"' && *start != 0))
          start++;

        if (start<=end) {
          *start = 0;
          if (strcmp(pname, tag) == 0)
            return xmlattrib(tag, value, parser);
          start++;
        }
      }

      if (oldStart == start)
        break;
      else
        oldStart = start;
    }
  }
  return xmlattrib(nullptr,nullptr, parser);
}

const wchar_t *xmlobject::getWPtr() const
{
  const char *ptr = getRawPtr();
  if (ptr == nullptr)
    return nullptr;
  wstring w;
  string2Wide(ptr, w);
  static wchar_t buff[buff_pre_alloc];
  wcsncpy(buff, w.c_str(), buff_pre_alloc - 1);
  buff[buff_pre_alloc - 1] = 0;
  return buff;
}

const char *xmlobject::getPtr() const {
  const char *ptr = getRawPtr();
  if (ptr == nullptr)
    return nullptr;
  return ptr;
}

int xmlobject::getRelativeTime() const {
  const char *d = parser->xmlinfo[index].data;
  return parseRelativeTime(d);
}

void xmlparser::convertString(const char *in, char *out, int maxlen) const
{
  if (in == nullptr)
    throw std::runtime_error("Null pointer exception");
  strncpy(out, in, maxlen - 1);
  out[maxlen - 1] = 0;
}

void xmlparser::convertString(const char *in, wchar_t *out, int maxlen) const
{
  if (in==nullptr)
     throw std::runtime_error("Null pointer exception");
  wstring w;
  string2Wide(in, w);
  wcsncpy(out, w.c_str(), maxlen - 1);
  out[maxlen - 1] = 0;
}

bool xmlobject::getObjectBool(const char *pname) const
{
  string tmp;
  getObjectString(pname, tmp);
  return tmp=="true" || atoi(tmp.c_str()) > 0 || strcasecmp(trim(tmp).c_str(), "true") == 0;
}

string &xmlobject::getObjectString(const char *pname, string &out) const
{
  xmlobject x=getObject(pname);
  if (x) {
    const char *bf = x.getRawPtr();
    if (bf) {
      out = bf;
      return out;
    }
  }
  xmlattrib xa(getAttrib(pname));
  if (xa && xa.data) {
    out = xa.getPtr();
  }
  else
    out = "";
  return out;
}

wstring &xmlobject::getObjectString(const char *pname, wstring &out) const
{
  xmlobject x=getObject(pname);
  if (x) {
    const wchar_t *bf = x.getWPtr();
    if (bf) {
      out = bf;
      return out;
    }
  }
  xmlattrib xa(getAttrib(pname));
  if (xa && xa.data) {
     string2Wide(xa.getPtr(), out);
  }
  else
    out = L"";
  return out;
}

char *xmlobject::getObjectString(const char *pname, char *out, int maxlen) const
{
  xmlobject x=getObject(pname);
  if (x) {
    const char *bf = x.getRawPtr();
    if (bf) {
      strncpy(out, bf, maxlen - 1);
      out[maxlen - 1] = 0;
      return out;
    }
    else
      out[0] = 0;
  }
  else {
    xmlattrib xa(getAttrib(pname));
    if (xa && xa.data) {
      strncpy(out, xa.getPtr(), maxlen - 1);
      out[maxlen - 1] = 0;
    } else
       out[0] = 0;
  }
  return out;
}

wchar_t *xmlobject::getObjectString(const char *pname, wchar_t *out, int maxlen) const
{
  xmlobject x=getObject(pname);
  if (x) {
    const char *bf = x.getRawPtr();
    if (bf) {
      wstring w;
      string2Wide(bf, w);
      wcsncpy(out, w.c_str(), maxlen - 1);
      out[maxlen - 1] = 0;
      return out;
    }
    else 
      out[0] = 0;
  }
  else {
    xmlattrib xa(getAttrib(pname));
    if (xa && xa.data) {
      wstring w;
      string2Wide(xa.getPtr(), w);
      wcsncpy(out, w.c_str(), maxlen - 1);
      out[maxlen - 1] = 0;
    } else
       out[0] = 0;
  }
  return out;
}

const char *xmlattrib::getPtr() const
{
  if (data)
    return ::decodeXML(data);
  else
    return nullptr;
}

const wchar_t *xmlattrib::getWPtr() const
{
  if (data) {
    const char *dec = ::decodeXML(data);
    static wchar_t xbf[buff_pre_alloc];
    wstring w;
    string2Wide(dec, w);
    wcsncpy(xbf, w.c_str(), buff_pre_alloc - 1);
    xbf[buff_pre_alloc - 1] = 0;
    return xbf;
  }
  else
    return nullptr;
}

std::int64_t xmlobject::getObjectInt64(const char* pname) const {
  xmlobject x(getObject(pname));
  if (x)
    return x.getInt64();
  else {
    xmlattrib xa(getAttrib(pname));
    if (xa)
      return xa.getInt64();
  }
  return 0;
}

std::uint64_t xmlobject::getObjectInt64u(const char* pname) const {
  xmlobject x(getObject(pname));
  if (x)
    return x.getInt64u();
  else {
    xmlattrib xa(getAttrib(pname));
    if (xa)
      return xa.getInt64u();
  }
  return 0;
}

double xmlobject::getObjectDouble(const char* pname) const {
  xmlobject x(getObject(pname));
  if (x)
    return x.getDouble();
  else {
    xmlattrib xa(getAttrib(pname));
    if (xa)
      return xa.getDouble();
  }
  return 0;
}

const char *xmlobject::getName() const {return parser->xmlinfo[index].tag;}
int xmlobject::getInt() const {const char *d = parser->xmlinfo[index].data;
                      return d ? atoi(d) : 0;}
std::int64_t xmlobject::getInt64() const {
    const char* d = parser->xmlinfo[index].data;
    return d ? atoll(d) : 0;
}
std::uint64_t xmlobject::getInt64u() const {
    const char* d = parser->xmlinfo[index].data;
    return d ? strtoull(d, nullptr, 10) : 0;
}
double xmlobject::getDouble() const {
    const char* d = parser->xmlinfo[index].data;
    return d ? strtod(d, nullptr) : 0.0;
}
bool xmlobject::is(const char *pname) const {
    const char *n = getName();
    return n[0] == pname[0] && strcmp(n, pname)==0;
}
const char *xmlobject::getRawPtr() const {return parser->xmlinfo[index].data;}
std::int64_t xmlattrib::getInt64() const {
    return data ? atoll(data) : 0;
}
