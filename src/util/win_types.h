#pragma once

#include <cstdint>
#include <cwchar>
#include <cstring>
#include <cstdarg>
#include <string>
#include <cmath>

typedef unsigned char BYTE;
typedef unsigned char * LPBYTE;
typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef int16_t SHORT;
typedef int64_t __int64;
typedef void* LPVOID;
typedef void* HANDLE;

typedef void* HWND;
typedef void* HDC;
typedef void* HBITMAP;
typedef void* HFONT;
typedef void* HBRUSH;
typedef void* HINSTANCE;
typedef void* HMENU;
typedef void* HPEN;
typedef void* HRGN;
typedef void* HCURSOR;
typedef void* HICON;
typedef intptr_t SOCKET;
typedef int32_t LRESULT;
typedef uint32_t WPARAM;
typedef uint64_t LPARAM;
typedef uint32_t UINT;

struct CRITICAL_SECTION {
    void* ptr;
};

struct COMMTIMEOUTS {
    DWORD ReadIntervalTimeout;
    DWORD ReadTotalTimeoutMultiplier;
    DWORD ReadTotalTimeoutConstant;
    DWORD WriteTotalTimeoutMultiplier;
    DWORD WriteTotalTimeoutConstant;
};

struct SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
};

struct FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};

struct WIN32_FIND_DATA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    wchar_t cFileName[260];
};

struct RECT {
  int32_t left;
  int32_t top;
  int32_t right;
  int32_t bottom;
};

#ifndef _WIN32

inline int swprintf_s(wchar_t* buf, size_t size, const wchar_t* format, ...) {
    va_list args;
    va_start(args, format);
    int res = vswprintf(buf, size, format, args);
    va_end(args);
    return res;
}

template<size_t N>
inline int swprintf_s(wchar_t (&buf)[N], const wchar_t* format, ...) {
    va_list args;
    va_start(args, format);
    int res = vswprintf(buf, N, format, args);
    va_end(args);
    return res;
}

#define snprintf_s snprintf
#define sprintf_s snprintf

// Use a more robust way to handle wcscpy_s
#define wcscpy_s(...) wcscpy_s_impl(__VA_ARGS__)

inline wchar_t* wcscpy_s_impl(wchar_t* dst, size_t size, const wchar_t* src) {
    return wcscpy(dst, src);
}

template<size_t N>
inline wchar_t* wcscpy_s_impl(wchar_t (&dst)[N], const wchar_t* src) {
    return wcscpy(dst, src);
}

#define wcscat_s(dst, size, src) wcscat(dst, src)

#define _wtoi(s) std::wcstol(s, nullptr, 10)
#define _wtoi64(s) std::wcstoll(s, nullptr, 10)
#define _wtof(s) std::wcstod(s, nullptr)
#define memset_s(dest, destsz, ch, count) memset(dest, ch, count)

inline void _itow_s(int val, wchar_t* buf, size_t size, int radix) {
    swprintf(buf, size, L"%d", val);
}

template<size_t N>
inline void _itow_s(int val, wchar_t (&buf)[N], int radix) {
    swprintf(buf, N, L"%d", val);
}

#define LOCALE_USER_DEFAULT 0
#define CSTR_EQUAL 2
#define CSTR_LESS_THAN 1

inline int CompareString(int, int, const wchar_t* s1, int len1, const wchar_t* s2, int len2) {
    std::wstring str1(s1, len1 >= 0 ? len1 : wcslen(s1));
    std::wstring str2(s2, len2 >= 0 ? len2 : wcslen(s2));
    if (str1 == str2) return CSTR_EQUAL;
    return str1 < str2 ? CSTR_LESS_THAN : 3;
}

#define lstrcmpi(s1, s2) compareStringIgnoreCase(s1, s2)
#define _wcsicmp wcscasecmp
#define _wcsnicmp wcsncasecmp

inline void OutputDebugStringA(const char* s) {}
inline void OutputDebugString(const wchar_t* s) {}

#define WM_USER 0x0400
inline void PostMessage(HWND, int, int, int) {}

#define MAX_COMPUTERNAME_LENGTH 15
inline void GetComputerName(wchar_t*, uint32_t*) {}

#define MAX_PATH 260

inline HANDLE FindFirstFile(const wchar_t*, WIN32_FIND_DATA*) { return nullptr; }
inline bool FindNextFile(HANDLE, WIN32_FIND_DATA*) { return false; }
inline void FindClose(HANDLE) {}
inline void FileTimeToLocalFileTime(const FILETIME*, FILETIME*) {}
inline void FileTimeToSystemTime(const FILETIME*, SYSTEMTIME*) {}
inline void DeleteFile(const wchar_t*) {}
inline void _wsplitpath_s(const wchar_t*, wchar_t*, wchar_t*, wchar_t*, wchar_t*) {}

#endif
