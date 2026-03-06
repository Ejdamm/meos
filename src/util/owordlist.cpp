#include "StdAfx.h"
#include "owordlist.h"
#include <fstream>
#include <algorithm>
#include <cstring>
#include <cwctype>
#include <cassert>
#include "meos_util.h"
#include "meosexception.h"

using namespace std;

void oWordDB::insert(const wchar_t *s)
{
  str.insert(s);
}

bool oWordDB::lookup(const wchar_t *s) const
{
  return str.count(s)==1;
}

const char *oWordDB::deserialize(const char *bf, const char *end)
{
  uint8_t s = (uint8_t)bf[0];
  bf++;
  str.clear();
  for (int k=0;k<s;k++) {
    wstring ns((wchar_t*)bf);
    bf+=(ns.size()+1)*sizeof(wchar_t);

    if (bf>end)
      throw std::runtime_error("Internal error deserializing wordlist.");

    str.insert(ns);
  }
  return bf;
}

char *oWordDB::serialize(char *bf) const
{
  uint8_t s=uint8_t(str.size());
  vector<wstring> rnd(s);
  int k=0;
  for (set<wstring>::const_iterator it=str.begin(); it!=str.end(); ++it)
    rnd[k++]=*it;

  bf[0]=(char)s;
  bf++;
  // Using a simpler serialization order than the original randomizing one for now,
  // or we can keep the randomization if rand() is acceptable.
  for (k=0;k<s;k++) {
    int i=rand() % rnd.size();
    int byteSize = (rnd[i].size()+1) * sizeof(wchar_t);
    memcpy(bf, rnd[i].c_str(), byteSize);
    bf+=byteSize;
    swap(rnd[i], rnd.back());
    rnd.pop_back();
  }
  return bf;
}

int oWordDB::serialSize() const
{
  int s=1;
  for(set<wstring>::const_iterator it=str.begin(); it!=str.end(); ++it)
    s+=(it->size()+1)*sizeof(wchar_t);

  return s;
}

oWordDatabase *oWordDB::split()
{
  if (str.size()>=hashSplitSize) {
    oWordIndexHash *db=new oWordIndexHash(false);

    for(auto it=str.begin();it!=str.end();++it)
      db->insert(it->c_str());

    delete this;
    return db;
  }
  else return this;
}

size_t oWordDB::size()
{
  return str.size();
}

void oWordIndexHash::clear()
{
  for(auto it=unMapped.begin(); it!=unMapped.end(); ++it)
    if (it->second)
      delete it->second;

  unMapped.clear();

  for(int k=0;k<hashTableSize;k++)
    if (hashTable[k]) {
      delete hashTable[k];
      hashTable[k]=0;
    }
}

const char *oWordIndexHash::deserialize(const char *bf, const char *end)
{
  clear();
  unsigned short s=*((unsigned short *)&bf[0]);
  bf+=2;

  for (int k=0;k<s;k++) {
    unsigned short a=*((unsigned short *)&bf[0]);
    unsigned short b=*((unsigned short *)&bf[2]);
    pWordDatabase db=0;
    bf+=4;
    if (a) {
      if (b==1)
        db = new oWordDB;
      else if (b==2)
        db = new oWordIndexHash(false);
      else
        throw std::runtime_error("Internal error deserilizing wordlist.");

      bf = db->deserialize(bf, end);
    }

    uint32_t i=a-indexMapStart;
    if (i<=(indexMapEnd-indexMapStart)) {
      if (hashTable[i])
        throw std::runtime_error("Internal error deserilizing wordlist.");
      else hashTable[i]=db;
    }
    else
      unMapped[a]=db;
  }
  return bf;
}

char *oWordIndexHash::serialize(char *bf) const
{
  unsigned short s = uint16_t(unMapped.size());
  for (int k=0;k<hashTableSize;k++)
    if (hashTable[k])
      s++;

  *((unsigned short *)&bf[0])=s;
  bf+=2;

  for(auto it=unMapped.begin(); it!=unMapped.end(); ++it) {
    *((unsigned short *)&bf[0]) = it->first;
    if (it->first) {
      *((unsigned short *)&bf[2]) = it->second->getType();
      bf = it->second->serialize(bf+4);
    }
    else { //Empty string.
      *((unsigned short *)&bf[2]) = 0;
      bf+=4;
    }
  }

  for (int k=0;k<hashTableSize;k++) {
    if (hashTable[k]) {
      *((unsigned short *)&bf[0]) = k + indexMapStart ;
      *((unsigned short *)&bf[2]) = hashTable[k]->getType();
      bf = hashTable[k]->serialize(bf+4);
    }
  }
  return bf;
}

int oWordIndexHash::serialSize() const
{
  int s=2;
  for (int k=0;k<hashTableSize;k++) {
    if (hashTable[k])
      s+=hashTable[k]->serialSize()+4;
  }
  for(auto it=unMapped.begin(); it!=unMapped.end(); ++it)
    s+=it->second ? it->second->serialSize()+4 : 4;

  return s;
}

void oWordIndexHash::insert(const wchar_t *s)
{
  uint32_t i=s[0]-indexMapStart;

  if (i<=(indexMapEnd-indexMapStart)) {
    if (!hashTable[i]) {
      hashTable[i]=new oWordDB();
      hashTable[i]->insert(s+1);
    }
    else {
      hashTable[i]=hashTable[i]->split();
      hashTable[i]->insert(s+1);
    }
  }
  else {
    if (s[0]==0)
      unMapped[0]=0; //Empty string
    else {
      auto it=unMapped.find(s[0]);
      if (it==unMapped.end()) {
        pWordDatabase db(hashAll ? pWordDatabase(new oWordIndexHash(false)):
                                   pWordDatabase(new oWordDB()));
        db->insert(s+1);
        unMapped[s[0]]=db;
      }
      else {
        if (it->second) {
          it->second=it->second->split();
          it->second->insert(s+1);
        }
        else
          assert(s[0]==0);
      }
    }
  }
}

bool oWordIndexHash::lookup(const wchar_t *s) const
{
  uint32_t i = s[0] - indexMapStart;

  if (i<=(indexMapEnd-indexMapStart)) {
    if (!hashTable[i])
      return false;
    else
      return hashTable[i]->lookup(s+1);
  }
  else {
    auto it=unMapped.find(s[0]);
    if (it==unMapped.end())
      return false;
    else if (s[0]==0)
      return true;
    else
      return it->second->lookup(s+1);
  }
}

oWordIndexHash::oWordIndexHash(bool hashAll_) : hashAll(hashAll_)
{
  for(int k=0;k<hashTableSize;k++)
    hashTable[k]=0;
}

oWordIndexHash::~oWordIndexHash()
{
  clear();
}

oWordList::oWordList() : wh(true) {}

oWordList::~oWordList() {}

void oWordList::insert(const wchar_t *s)
{
  size_t len=wcslen(s);
  if (len<511) {
    wchar_t bf[512];
    wcscpy(bf, s);
    for (size_t i = 0; i < len; ++i) bf[i] = towlower(bf[i]);
    wh.insert(bf);
  }
  else {
    wstring ws(s);
    for (size_t i = 0; i < ws.length(); ++i) ws[i] = towlower(ws[i]);
    wh.insert(ws.c_str());
  }
}

bool oWordList::lookup(const wchar_t *s) const
{
  size_t len=wcslen(s);
  if (len<511) {
    wchar_t bf[512];
    wcscpy(bf, s);
    for (size_t i = 0; i < len; ++i) bf[i] = towlower(bf[i]);
    return wh.lookup(bf);
  }
  else {
    wstring ws(s);
    for (size_t i = 0; i < ws.length(); ++i) ws[i] = towlower(ws[i]);
    return wh.lookup(ws.c_str());
  }
}

void oWordList::serialize(vector<char> &serial) const
{
  int s=wh.serialSize();
  serial.resize(s);
  wh.serialize(&serial[0]);
}

void oWordList::deserialize(const vector<char> &serial)
{
  wh.clear();
  wh.deserialize(&serial[0], &serial[0]+serial.size());
}

void oWordList::save(const wstring &file) const
{
  ofstream fout(string(file.begin(), file.end()), ios::binary | ios::trunc);

  if (fout.is_open()) {
    vector<char> serial;
    serialize(serial);
    const char *hdr="WWDB";

    fout.write(hdr, 4);
    uint32_t s=(uint32_t)serial.size();
    fout.write((char*)&s, 4);
    fout.write(&serial[0], s);
  }
  else throw std::runtime_error("Could not save word database.");
}

void oWordList::load(const wstring &file)
{
  wstring ex = L"Bad word database. " +file;
  ifstream fin(string(file.begin(), file.end()), ios::binary);

  if (fin.is_open()) {
    char hdr[5]={0,0,0,0,0};
    fin.read(hdr, 4);

    if ( strcmp(hdr, "WWDB")!=0 )
      throw meosException(ex);

    uint32_t s=0;
    fin.read((char*)&s, 4);

    vector<char> serial(s);

    if (!fin.read(&serial[0], s))
      throw meosException(ex);

    deserialize(serial);
  }
  else throw meosException(ex);
}
