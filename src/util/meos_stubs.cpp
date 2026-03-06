#include "StdAfx.h"
#include <codecvt>
#include <locale>

// Localizer and oWordList stubs
#include "localizer.h"
#include "oFreeImport.h"

class LocalizerStub : public Localizer {
public:
    LocalizerStub() {
        // We don't call init() here because it allocates LocalizerInternal
        // which we would also need to stub.
    }
};

Localizer lang;

Localizer::LocalizerInternal::LocalizerInternal() : impl(nullptr), implBase(nullptr), owning(false), user(nullptr) {}
Localizer::LocalizerInternal::~LocalizerInternal() {}
const oWordList& Localizer::LocalizerInternal::getGivenNames() const {
    static oWordList empty;
    return empty;
}
const wstring& Localizer::LocalizerInternal::tl(const wstring& str) const {
    return str;
}

oWordList::oWordList() : wh(false) {}
oWordList::~oWordList() {}
bool oWordList::lookup(const wchar_t* s) const {
    return false;
}
void oWordList::insert(const wchar_t* s) {}
void oWordList::save(const wstring& file) const {}
void oWordList::load(const wstring& file) {}
void oWordList::serialize(vector<char>& serial) const {}
void oWordList::deserialize(const vector<char>& serial) {}

oWordIndexHash::oWordIndexHash(bool hashAll_) : hashAll(hashAll_) {
    for (int i = 0; i < hashTableSize; ++i) hashTable[i] = nullptr;
}
oWordIndexHash::~oWordIndexHash() {}
bool oWordIndexHash::lookup(const wchar_t* s) const {
    return false;
}
void oWordIndexHash::insert(const wchar_t* s) {}
void oWordIndexHash::clear() {}
const char* oWordIndexHash::deserialize(const char* bf, const char* end) { return bf; }
char* oWordIndexHash::serialize(char* bf) const { return bf; }
int oWordIndexHash::serialSize() const { return 0; }
