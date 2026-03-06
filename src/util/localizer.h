#pragma once

#include <map>
#include <string>
#include <vector>
#include "owordlist.h"

class LocalizerImpl;

class Localizer {
public:
  class LocalizerInternal {
  private:
    std::map<std::wstring, std::wstring> langResource;
    LocalizerImpl *impl;
    LocalizerImpl *implBase;

    bool owning;
    LocalizerInternal *user;

  public:
    void debugDump(const std::wstring &untranslated, const std::wstring &translated) const;

    std::vector<std::wstring> getLangResource() const;
    void loadLangResource(const std::wstring &name);
    void addLangResource(const std::wstring &name, const std::wstring &resource);

    /** Translate string */
    const std::wstring &tl(const std::wstring &str) const;

    void set(Localizer &li);

    /** Get database with given names */
    const oWordList &getGivenNames() const;

    LocalizerInternal();
    ~LocalizerInternal();
  };

private:
  LocalizerInternal *linternal;

public:
  bool capitalizeWords() const;

  LocalizerInternal &get() {return *linternal;}
  const std::wstring &tl(const std::string &str) const;
  const std::wstring &tl(const std::wstring &str) const {return linternal->tl(str);}
  
  const std::wstring tl(const std::wstring &str, bool cap) const;

  void init() {linternal = new LocalizerInternal();}
  void unload() {
      if (linternal) {
          delete linternal;
          linternal = nullptr;
      }
  }

  Localizer() : linternal(nullptr) {}
  ~Localizer() {unload();}
};

extern Localizer lang;
