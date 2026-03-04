#include "localizer.h"
#include "meos_util.h"
#include <fstream>
#include <algorithm>
#include <random>
#include <sstream>
#include <numeric>
#include <cwctype>

using namespace std;

// Forward declaration of functions from meos_util.cpp if needed
// Actually we already have meos_util.h included.

class LocalizerImpl {
    wstring language;
    map<wstring, wstring> table;
    map<wstring, wstring> unknown;
    mutable unique_ptr<oWordList> givenNames;

    void addUnknown(const wstring& key) {
        if (unknown.emplace(key, L"").second) {
            // Log missing resource
        }
    }

public:
    LocalizerImpl() : givenNames(nullptr) {}
    ~LocalizerImpl() {}

    const oWordList& getGivenNames() const {
        if (!givenNames) {
            givenNames = make_unique<oWordList>();
            // In a real migration, we would load the word list here
        }
        return *givenNames;
    }

    const wstring& translate(const wstring& str, bool& found) {
        found = false;
        static int i = 0;
        const int bsize = 17;
        static wstring value[bsize];
        int len = str.length();

        if (len == 0)
            return _EmptyWString;

        if (str[0] == L'#') {
            i = (i + 1) % bsize;
            value[i] = str.substr(1);
            found = true;
            return value[i];
        }

        auto isDigit = [](wchar_t c) {
            return c >= L'0' && c <= L'9';
        };

        if (str[0] == L',' || str[0] == L' ' || str[0] == L'.' ||
            str[0] == L':' || str[0] == L';' || str[0] == L'<' || str[0] == L'>' ||
            str[0] == L'-' || str[0] == (wchar_t)0x96 || str[0] == L'×' || isDigit(str[0])) {
            unsigned k = 1;
            while (k < str.length() && (str[k] == L' ' || str[k] == L'.' || str[k] == L':' || str[k] == L'<' || str[k] == L'>' ||
                   str[k] == L'-' || str[k] == (wchar_t)0x96 || str[k] == L'×' || isDigit(str[k])))
                k++;

            if (k < str.length()) {
                wstring sub = str.substr(k);
                i = (i + 1) % bsize;
                value[i] = str.substr(0, k) + translate(sub, found);
                return value[i];
            }
        }

        auto it = table.find(str);
        if (it != table.end()) {
            found = true;
            return it->second;
        }

        size_t subst = str.find_first_of(L'#');
        if (subst != wstring::npos) {
            wstring s = translate(str.substr(0, subst), found);
            vector<wstring> split_vec;
            split(str.substr(subst + 1), L"#", split_vec);
            split_vec.push_back(L"");
            const wchar_t* subsymb = L"XYZW";
            size_t subpos = 0;
            wstring ret;
            size_t lastpos = 0;
            for (size_t k = 0; k < s.size(); k++) {
                if (subpos >= split_vec.size() || subpos >= 4)
                    break;
                if (s[k] == subsymb[subpos]) {
                    if (k > 0 && iswalnum(s[k - 1]))
                        continue;
                    if (k + 1 < s.size() && iswalnum(s[k + 1]))
                        continue;
                    ret += s.substr(lastpos, k - lastpos);
                    ret += split_vec[subpos];
                    lastpos = k + 1;
                    subpos++;
                }
            }
            if (lastpos < s.size())
                ret += s.substr(lastpos);

            i = (i + 1) % bsize;
            value[i] = ret;
            return value[i];
        } else if (str[0] == L'@') {
            i = (i + 1) % bsize;
            value[i] = str.substr(1);
            found = true;
            return value[i];
        }

        wchar_t last = str[len - 1];
        if (last != L':' && last != L'.' && last != L' ' && last != L',' &&
            last != L';' && last != L'<' && last != L'>' && last != L'-' &&
            last != (wchar_t)0x96 && last != (wchar_t)215 && !isDigit(last)) {
            addUnknown(str);
            found = false;
            i = (i + 1) % bsize;
            value[i] = str;
            return value[i];
        }

        wstring suffix;
        int pos = str.find_last_not_of(last);

        while (pos > 0) {
            wchar_t l = str[pos];
            if (l != L':' && l != L' ' && l != L',' && l != L'.' &&
                l != L';' && l != L'<' && l != L'>' && l != L'-' && l != (wchar_t)0x96 && l != (wchar_t)215 && !isDigit(l))
                break;

            pos = str.find_last_not_of(l, pos);
        }

        suffix = str.substr(pos + 1);
        wstring key = str.substr(0, str.length() - suffix.length());
        it = table.find(key);
        if (it != table.end()) {
            i = (i + 1) % bsize;
            value[i] = it->second + suffix;
            found = true;
            return value[i];
        }

        addUnknown(key);
        found = false;
        i = (i + 1) % bsize;
        value[i] = str;
        return value[i];
    }

    void loadTable(const wstring& file, const wstring& language) {
        clear();
        this->language = language;
        string sfile;
        wide2String(file, sfile);
        ifstream fin(sfile, ios::in);
        if (!fin.good()) return;

        vector<string> raw;
        string line;
        while (getline(fin, line)) {
            if (!line.empty() && line[0] != '#') {
                raw.push_back(line);
            }
        }
        loadTableFromRaw(raw);
    }

    void loadTableFromRaw(const vector<string>& raw) {
        vector<int> order(raw.size());
        iota(order.begin(), order.end(), 0);
        
        random_device rd;
        mt19937 g(rd());
        shuffle(order.begin(), order.end(), g);

        table.clear();
        wstring nline = L"\n";
        for (size_t k = 0; k < raw.size(); k++) {
            const string& s = raw[order[k]];
            size_t pos = s.find_first_of('=');
            if (pos == string::npos) continue;

            string key_s = s.substr(0, pos);
            string val_s = s.substr(pos + 1);

            // Simple trim
            auto trim = [](string s) {
                size_t first = s.find_first_not_of(" \t\r\n");
                if (first == string::npos) return string("");
                size_t last = s.find_last_not_of(" \t\r\n");
                return s.substr(first, last - first + 1);
            };

            key_s = trim(key_s);
            val_s = trim(val_s);

            if (val_s.empty()) continue;

            size_t nl = val_s.find("\\n");
            while (nl != string::npos) {
                val_s.replace(nl, 2, "\n");
                nl = val_s.find("\\n");
            }

            wstring wkey, wval;
            string2Wide(key_s, wkey);
            string2Wide(val_s, wval);
            table[wkey] = wval;
        }
    }

    void clear() {
        table.clear();
        unknown.clear();
        language.clear();
    }

    void translateAll(const LocalizerImpl& all) {
        bool f;
        for (auto const& [key, val] : all.table) {
            translate(key, f);
            if (!f) {
                unknown[key] = val;
            }
        }
    }

    void saveUnknown(const wstring& file) {
        // Implementation for saving unknown strings
    }

    void saveTable(const wstring& file) {
        // Implementation for saving table
    }
};

Localizer::LocalizerInternal::LocalizerInternal() : owning(true), user(nullptr) {
    impl = make_unique<LocalizerImpl>();
}

Localizer::LocalizerInternal::~LocalizerInternal() {
    if (user) {
        // In the original, this part was weirdly handling ownership
        // But with unique_ptr, it's safer.
    }
}

void Localizer::LocalizerInternal::set(Localizer& li) {
    // Original implementation was sharing internals
    // For now we just copy the table if needed, or keep it simple.
}

vector<wstring> Localizer::LocalizerInternal::getLangResource() const {
    vector<wstring> v;
    for (auto const& [key, val] : langResource)
        v.push_back(key);
    return v;
}

const oWordList& Localizer::LocalizerInternal::getGivenNames() const {
    return impl->getGivenNames();
}

const wstring& Localizer::LocalizerInternal::tl(const wstring& str) const {
    bool found;
    const wstring& ret = impl->translate(str, found);
    if (found || !implBase)
        return ret;
    return implBase->translate(str, found);
}

void Localizer::LocalizerInternal::loadLangResource(const wstring& name) {
    auto it = langResource.find(name);
    if (it == langResource.end()) return;
    impl->loadTable(it->second, name);
}

void Localizer::LocalizerInternal::addLangResource(const wstring& name, const wstring& resource) {
    langResource[name] = resource;
    if (!implBase) {
        implBase = make_unique<LocalizerImpl>();
        implBase->loadTable(resource, name);
    }
}

bool Localizer::capitalizeWords() const {
    return tl(L"Lyssna") == L"Listen";
}

const wstring& Localizer::tl(const string& str) const {
    if (str.length() == 0) return _EmptyWString;
    wstring wstr;
    string2Wide(str, wstr);
    return tl(wstr);
}

const wstring Localizer::tl(const wstring& str, bool cap) const {
    wstring w = tl(str);
    if (cap && capitalizeWords()) {
        ::capitalizeWords(w);
    }
    return w;
}

Localizer lang;
