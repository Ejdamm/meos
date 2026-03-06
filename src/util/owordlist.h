#pragma once

#include <set>
#include <map>
#include <vector>
#include <string>

class oWordDatabase {
public:
  virtual char getType() const = 0;
  virtual const char *deserialize(const char *bf, const char *end) = 0;
  virtual char *serialize(char *bf) const = 0;
  virtual int serialSize() const = 0;
  virtual oWordDatabase *split() {return this;}
  virtual void insert(const wchar_t *s) = 0;
  virtual bool lookup(const wchar_t *s) const = 0;
  virtual ~oWordDatabase() {}
};

class oWordDB : public oWordDatabase {
protected:
  char getType() const override {return 1;}
  std::set<std::wstring> str;
public:
  const char *deserialize(const char *bf, const char *end) override;
  char *serialize(char *bf) const override;
  int serialSize() const override;
  oWordDatabase *split() override;
  void insert(const wchar_t *s) override;
  bool lookup(const wchar_t *s) const override;
  size_t size();
};

const wchar_t indexMapStart='a';
const wchar_t indexMapEnd='z';
const int hashSplitSize=16;
const int hashTableSize=indexMapEnd-indexMapStart+1;

typedef oWordDatabase* pWordDatabase;
typedef std::map<wchar_t, pWordDatabase> MapTable;

class oWordIndexHash : public oWordDatabase {
protected:
  char getType() const override {return 2;}
  pWordDatabase hashTable[hashTableSize];
  MapTable unMapped;
  bool hashAll;
public:
  const char *deserialize(const char *bf, const char *end) override;
  char *serialize(char *bf) const override;
  int serialSize() const override;
  void insert(const wchar_t *s) override;
  bool lookup(const wchar_t *s) const override;
  void clear();
  ~oWordIndexHash() override;
  oWordIndexHash(bool hashAll_);
};

class oWordList {
protected:
  oWordIndexHash wh;
public:
  void serialize(std::vector<char> &serial) const;
  void deserialize(const std::vector<char> &serial);

  void save(const std::wstring &file) const;
  void load(const std::wstring &file);

  void insert(const wchar_t *s);
  bool lookup(const wchar_t *s) const;
  ~oWordList();
  oWordList();
};
