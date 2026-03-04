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

#include <map>
#include <set>
#include <string>
#include <vector>
#include <memory>

class LocalizerImpl;

// Stub for oWordList until it is migrated
class oWordList {
public:
    void save(const std::wstring &file) const {}
    void load(const std::wstring &file) {}
    void insert(const wchar_t *s) {}
    bool lookup(const wchar_t *s) const { return false; }
};

class Localizer {
public:
    class LocalizerInternal {
    private:
        std::map<std::wstring, std::wstring> langResource;
        std::unique_ptr<LocalizerImpl> impl;
        std::unique_ptr<LocalizerImpl> implBase;

        bool owning;
        LocalizerInternal* user;

    public:
        LocalizerInternal();
        ~LocalizerInternal();

        void debugDump(const std::wstring& untranslated, const std::wstring& translated) const;

        std::vector<std::wstring> getLangResource() const;
        void loadLangResource(const std::wstring& name);
        void addLangResource(const std::wstring& name, const std::wstring& resource);

        /** Translate string */
        const std::wstring& tl(const std::wstring& str) const;

        void set(Localizer& li);

        /** Get database with given names */
        const oWordList& getGivenNames() const;
    };

private:
    std::unique_ptr<LocalizerInternal> linternal;

public:
    bool capitalizeWords() const;

    LocalizerInternal& get() { return *linternal; }
    const std::wstring& tl(const std::string& str) const;
    const std::wstring& tl(const std::wstring& str) const { return linternal->tl(str); }

    const std::wstring tl(const std::wstring& str, bool cap) const;

    void init() { linternal = std::make_unique<LocalizerInternal>(); }
    void unload() { linternal.reset(); }

    Localizer() : linternal(nullptr) {}
};

extern Localizer lang;
