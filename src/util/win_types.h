#pragma once

#include <sys/stat.h>
#include <unistd.h>

#ifndef _WIN32

#include <cstdint>
#include <cwchar>
#include <string>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cstdarg>
#include <cmath>
#include <unistd.h>
#include <fcntl.h>

typedef uint32_t DWORD;
typedef uint32_t* LPDWORD;
typedef uint16_t WORD;
typedef uint16_t SHORT;
typedef uint8_t BYTE;
typedef uint8_t* LPBYTE;
typedef int BOOL;
typedef uint32_t UINT;
typedef long LRESULT;
typedef intptr_t WPARAM;
typedef intptr_t LPARAM;
typedef intptr_t UINT_PTR;
typedef void* WNDPROC;
typedef int64_t __int64;

typedef uint32_t COLORREF; // Moved here
typedef struct _RGBQUAD {
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
} RGBQUAD;
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GetRValue(rgb) ((BYTE)(rgb))
#define GetGValue(rgb) ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb) ((BYTE)((rgb)>>16))

// Added for GDI stubs
typedef unsigned long ULONG;
typedef long LONG;
typedef intptr_t LONG_PTR; // Corrected: Added LONG_PTR

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef void* HANDLE;
typedef void* HWND;
typedef void* HINSTANCE;
typedef void* HDC;
typedef void* HGDIOBJ;
typedef void* HFONT;
typedef void* HBRUSH;
typedef void* HPEN;
typedef void* HBITMAP;
typedef void* HMENU;
typedef void* LPVOID;
typedef void* HRSRC;
typedef void* HGLOBAL;

#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)

typedef struct _RECT {
    int left;
    int top;
    int right;
    int bottom;
} RECT, *PRECT;

typedef struct _POINT {
    int x;
    int y;
} POINT, *PPOINT;

typedef RECT* LPRECT;
typedef POINT* LPPOINT;

// GDI Structs (Moved here)
typedef struct _GRADIENT_RECT {
    ULONG UpperLeft;
    ULONG LowerRight;
} GRADIENT_RECT, *PGRADIENT_RECT;

typedef struct _TRIVERTEX {
    COLORREF      Red;
    COLORREF      Green;
    COLORREF      Blue;
    COLORREF      Alpha;
    LONG        x;
    LONG        y;
} TRIVERTEX, *PTRIVERTEX;

typedef struct _SIZE {
    int cx;
    int cy;
} SIZE, *PSIZE;

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;
typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;



typedef char* LPSTR;
typedef char* LPTSTR;
typedef wchar_t* LPWSTR;
typedef const char* LPCSTR;
typedef const wchar_t* LPCWSTR;
typedef char TCHAR;
typedef const TCHAR* LPCTSTR;

#define _T(x) x
#define TEXT(x) x

#define MAX_PATH 260
#define _MAX_PATH MAX_PATH
#define MAX_COMPUTERNAME_LENGTH 15

typedef struct _WIN32_FIND_DATAW {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    wchar_t cFileName[MAX_PATH];
    wchar_t cAlternateFileName[14];
} WIN32_FIND_DATAW, *PWIN32_FIND_DATAW;
typedef WIN32_FIND_DATAW WIN32_FIND_DATA;

#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_ATTRIBUTE_NORMAL 0x00000080

// Some Win32 constants
#define APIENTRY
#define CALLBACK
#define CP_UTF8 65001
#define CP_ACP 0
#define MB_PRECOMPOSED 0x00000001

#define GENERIC_READ 0x80000000
#define GENERIC_WRITE 0x40000000
#define FILE_SHARE_READ 0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5

#define ERROR_ACCESS_DENIED 5
#define ERROR_SHARING_VIOLATION 32
#define FORMAT_MESSAGE_FROM_SYSTEM 0x00001000
#define FORMAT_MESSAGE_ALLOCATE_BUFFER 0x00000100
#define FORMAT_MESSAGE_IGNORE_INSERTS 0x00000200
#define LANG_NEUTRAL 0x00
#define SUBLANG_DEFAULT 0x01
#define MAKELANGID(p, s) ((((uint16_t)(s)) << 10) | (uint16_t)(p))

#define MB_OK 0x00000000
#define WM_USER 0x0400

#define MB_ICONWARNING 0x00000030
#define MB_ERR_INVALID_CHARS 0x00000008
#define NORM_IGNORECASE 0x00000001
#define ERROR_INTERNET_TIMEOUT 12002

#ifndef FIXED_TABS_DEFINED
#define FIXED_TABS_DEFINED
class FixedTabs {
public:
    void* get(int i) { return nullptr; }
    void clearCompetitionData() {}
};
#endif
struct TOOLINFOW {};
// For toolbar.h
typedef void* HIMAGELIST;
typedef intptr_t INT_PTR;
typedef uintptr_t DWORD_PTR;

typedef struct _TBBUTTON {
    int iBitmap;
    int idCommand;
    BYTE fsState;
    BYTE fsStyle;
    BYTE bReserved[2];
    DWORD_PTR dwData;
    INT_PTR iString;
} TBBUTTON, *PTBBUTTON;

typedef void* CRITICAL_SECTION;
typedef struct _COMMTIMEOUTS {
    DWORD ReadIntervalTimeout;
    DWORD ReadTotalTimeoutMultiplier;
    DWORD ReadTotalTimeoutConstant;
    DWORD WriteTotalTimeoutMultiplier;
    DWORD WriteTotalTimeoutConstant;
} COMMTIMEOUTS;

// Win32 API stubs for Linux
inline void InvalidateRect(HWND hWnd, const RECT* lpRect, BOOL bErase) {}
inline uint32_t GetTickCount() { return 0; }
inline uint64_t GetTickCount64() { return 0; }
inline void GetCurrentDirectory(DWORD nBufferLength, LPTSTR lpBuffer) { if (lpBuffer) lpBuffer[0] = 0; }
inline DWORD GetModuleFileName(HINSTANCE hModule, LPTSTR lpFilename, DWORD nSize) { if (lpFilename) lpFilename[0] = 0; return 0; }
inline void GetLocalTime(SYSTEMTIME* lpSystemTime) { if (lpSystemTime) memset(lpSystemTime, 0, sizeof(SYSTEMTIME)); }
inline void GetSystemTime(SYSTEMTIME* lpSystemTime) { if (lpSystemTime) memset(lpSystemTime, 0, sizeof(SYSTEMTIME)); }
inline HWND CreateDialog(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, void* lpDialogFunc) { return nullptr; }
inline void UpdateWindow(HWND hWnd) {}
inline void* MAKEINTRESOURCE(int i) { return (void*)(intptr_t)i; }
inline BOOL SystemTimeToFileTime(const SYSTEMTIME* lpSystemTime, LPFILETIME lpFileTime) { return TRUE; }
inline BOOL FileTimeToSystemTime(const FILETIME* lpFileTime, LPSYSTEMTIME lpSystemTime) { return TRUE; }
inline BOOL FileTimeToLocalFileTime(const FILETIME* lpFileTime, LPFILETIME lpLocalFileTime) { return TRUE; }
inline int MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar) { return 0; }
inline int WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, BOOL* lpUsedDefaultChar) { return 0; }
inline int CompareString(DWORD Locale, DWORD dwCmpFlags, LPCWSTR lpString1, int cchCount1, LPCWSTR lpString2, int cchCount2) { return 2; }
inline int CompareString(DWORD Locale, DWORD dwCmpFlags, LPCSTR lpString1, int cchCount1, LPCSTR lpString2, int cchCount2) { return 2; }
inline BOOL DeleteFile(LPCWSTR lpFileName) { return TRUE; }
inline BOOL CopyFile(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists) { return TRUE; }
inline HANDLE CreateFile(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, void* lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) { return nullptr; }
inline BOOL CloseHandle(HANDLE hObject) { return TRUE; }
inline uint32_t GetLastError() { return 0; }
inline DWORD FormatMessage(DWORD dwFlags, const void* lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPTSTR lpBuffer, DWORD nSize, va_list* Arguments) { return 0; }
inline void LocalFree(void* h) {}
inline int iswascii(wchar_t c) { return (c >= 0 && c <= 127); }
inline HANDLE FindFirstFile(LPCWSTR lpFileName, WIN32_FIND_DATAW* lpFindFileData) { return INVALID_HANDLE_VALUE; }
inline BOOL FindNextFile(HANDLE hFindFile, WIN32_FIND_DATAW* lpFindFileData) { return FALSE; }
inline BOOL FindClose(HANDLE hFindFile) { return TRUE; }
inline void CharUpperBuff(LPWSTR lpsz, DWORD cchLength) {}
inline BOOL CharLowerBuff(LPWSTR lpsz, DWORD cchLength) { return TRUE; }
inline BOOL TzSpecificLocalTimeToSystemTime(const void* lpTimeZoneInformation, const SYSTEMTIME* lpLocalTime, LPSYSTEMTIME lpUniversalTime) { return TRUE; }
inline BOOL SystemTimeToTzSpecificLocalTime(const void* lpTimeZoneInformation, const SYSTEMTIME* lpUniversalTime, LPSYSTEMTIME lpLocalTime) { return TRUE; }
inline int MessageBox(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) { return 1; }
inline BOOL PostMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return TRUE; }
inline BOOL GetComputerName(LPWSTR lpBuffer, LPDWORD nSize) { if (nSize && *nSize > 0) lpBuffer[0] = 0; return TRUE; }
inline void OutputDebugString(LPCWSTR lpOutputString) {}
inline void OutputDebugStringA(LPCSTR lpOutputString) {}
inline void OutputDebugStringW(LPCWSTR lpOutputString) {}
inline DWORD GetCurrentThreadId() { return 0; }
inline HRSRC FindResource(HINSTANCE hModule, LPCWSTR lpName, LPCWSTR lpType) { return nullptr; }
inline HGLOBAL LoadResource(HINSTANCE hModule, HRSRC hResInfo) { return nullptr; }
inline void* LockResource(HGLOBAL hResData) { return nullptr; }
inline DWORD SizeofResource(HINSTANCE hModule, HRSRC hResInfo) { return 0; }

#define LOCALE_USER_DEFAULT 0
#define CSTR_LESS_THAN 1
#define CSTR_EQUAL 2
#define CSTR_GREATER_THAN 3

typedef union _ULARGE_INTEGER {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } u;
    uint64_t QuadPart;
} ULARGE_INTEGER;
#define LowPart u.LowPart
#define HighPart u.HighPart

#ifndef IDD_SPLASH
#define IDD_SPLASH 101
#endif
#define SW_SHOW 5

class GDIImplFontEnum {};
class GDIImplFontSet {};

// MSVC CRT shims
#define _atoi64(s) std::stoll(s)
#define _wtoi(s) std::stoi(s)
#define _wtof(s) std::stod(s)
#define _waccess(f, m) 0
#define _wfullpath(a, f, l) nullptr
#define _stricmp strcasecmp
#define _strcmpi strcasecmp
#define _wcsicmp wcscasecmp
#define _wcstoui64 std::wcstoull
#define _memicmp strncasecmp

inline void wcscpy_s_impl(wchar_t* d, const wchar_t* s) { wcscpy(d, s); }
inline void wcscpy_s_impl(wchar_t* d, size_t n, const wchar_t* s) { wcscpy(d, s); }
#define wcscpy_s(d, ...) wcscpy_s_impl(d, __VA_ARGS__)

inline void wcscat_s_impl(wchar_t* d, const wchar_t* s) { wcscat(d, s); }
inline void wcscat_s_impl(wchar_t* d, size_t n, const wchar_t* s) { wcscat(d, s); }
#define wcscat_s(d, ...) wcscat_s_impl(d, __VA_ARGS__)

inline void wcsncpy_s_impl(wchar_t* d, const wchar_t* s, size_t n) { wcsncpy(d, s, n); }
inline void wcsncpy_s_impl(wchar_t* d, size_t size, const wchar_t* s, size_t n) { wcsncpy(d, s, n); }
#define wcsncpy_s(d, ...) wcsncpy_s_impl(d, __VA_ARGS__)

inline void strncpy_s_impl(char* d, const char* s, size_t n) { strncpy(d, s, n); }
inline void strncpy_s_impl(char* d, size_t size, const char* s, size_t n) { strncpy(d, s, n); }
#define strncpy_s(d, ...) strncpy_s_impl(d, __VA_ARGS__)

inline void strcpy_s_impl(char* d, const char* s) { strcpy(d, s); }
inline void strcpy_s_impl(char* d, size_t n, const char* s) { strcpy(d, s); }
#define strcpy_s(d, ...) strcpy_s_impl(d, __VA_ARGS__)

inline std::string path2str(const std::wstring& w) { return std::string(w.begin(), w.end()); }
inline std::string path2str(const wchar_t* w) { std::wstring ws(w); return std::string(ws.begin(), ws.end()); }

inline int _wfopen_s(FILE** pFile, const wchar_t* filename, const wchar_t* mode) {
    std::string fn = path2str(filename);
    std::string m = path2str(mode);
    *pFile = fopen(fn.c_str(), m.c_str());
    return (*pFile != nullptr) ? 0 : -1;
}

inline FILE* _wfopen(const wchar_t* filename, const wchar_t* mode) {
    std::string fn = path2str(filename);
    std::string m = path2str(mode);
    return fopen(fn.c_str(), m.c_str());
}

inline size_t fread_s(void* buffer, size_t bufferSize, size_t elementSize, size_t count, FILE* stream) {
    return fread(buffer, elementSize, count, stream);
}

#define _close close
#define _read read
#define _write write
#define _lseek lseek
#define _O_RDONLY O_RDONLY
#define _O_WRONLY O_WRONLY
#define _O_RDWR O_RDWR
#define _O_CREAT O_CREAT
#define _O_TRUNC O_TRUNC
#define _O_BINARY 0
#define _SH_DENYNO 0
#define _SH_DENYWR 0
#define _S_IREAD 0
#define _S_IWRITE 0

inline int _filelength(int fd) {
    struct stat st;
    if (fstat(fd, &st) == 0)
        return st.st_size;
    return -1;
}



inline int _wsopen_s(int* pfd, const wchar_t* filename, int oflag, int shflag, int pmode) {
    std::string fn = path2str(filename);
    *pfd = open(fn.c_str(), oflag);
    return (*pfd != -1) ? 0 : -1;
}

// On Linux, use standard stat function and struct
inline int _wstat(const wchar_t* f, struct stat* s) {
    std::string fn = path2str(f); // Convert wchar_t* to char*
    return stat(fn.c_str(), s); // Call standard stat
}
inline void DestroyWindow(HWND hWnd) {}

template<size_t N>
inline int swprintf_s(wchar_t (&buf)[N], const wchar_t* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vswprintf(buf, N, fmt, args);
    va_end(args);
    return ret;
}

inline int swprintf_s(wchar_t* buf, size_t size, const wchar_t* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vswprintf(buf, size, fmt, args);
    va_end(args);
    return ret;
}

template<size_t N>
inline int sprintf_s(char (&buf)[N], const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vsnprintf(buf, N, fmt, args);
    va_end(args);
    return ret;
}

inline int sprintf_s(char* buf, size_t size, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vsnprintf(buf, size, fmt, args);
    va_end(args);
    return ret;
}

#define _itoa_s(val, buf, size, radix) snprintf(buf, size, "%d", val)
#define _i64toa_s(val, buf, size, radix) snprintf(buf, size, "%lld", (long long)val)
#define itoa(val, buf, radix) sprintf(buf, "%d", val)
#define _wrename(o, n) rename(path2str(o).c_str(), path2str(n).c_str())
#define _wremove(f) remove(path2str(f).c_str())

// GDI Stubs
inline HDC GetDC(HWND hWnd) { return nullptr; }
inline int ReleaseDC(HWND hWnd, HDC hDC) { return 1; }
inline HDC CreateCompatibleDC(HDC hdc) { return nullptr; }
inline HBITMAP CreateCompatibleBitmap(HDC hdc, int cx, int cy) { return nullptr; }
inline HGDIOBJ SelectObject(HDC hdc, HGDIOBJ h) { return nullptr; }
inline BOOL BitBlt(HDC hdcDest, int xDest, int yDest, int w, int h, HDC hdcSrc, int xSrc, int ySrc, DWORD rop) { return FALSE; }
inline BOOL GradientFill(HDC hdc, PTRIVERTEX pVertex, ULONG nVertex, PGRADIENT_RECT pMesh, ULONG nMesh, DWORD dwMode) { return FALSE; }
inline HGDIOBJ GetStockObject(int i) { return nullptr; }
inline COLORREF SetDCPenColor(HDC hdc, COLORREF color) { return 0; }
inline COLORREF SetDCBrushColor(HDC hdc, COLORREF color) { return 0; }
inline BOOL Rectangle(HDC hdc, int left, int top, int right, int bottom) { return FALSE; }
inline int SetBkMode(HDC hdc, int mode) { return 0; }
inline int DrawText(HDC hdc, LPCWSTR lpString, int cchText, LPRECT lpRect, UINT format) { return 0; }
inline COLORREF GetSysColor(int nIndex) { return 0; }
inline int CharLower(LPWSTR lpsz) { /* Not implemented for simplicity */ return 0; }
inline COLORREF GetDCPenColor(HDC hdc) { return 0; }
inline BOOL Polygon(HDC hdc, const POINT* lpPoints, int nCount) { return FALSE; }
inline int GetWindowText(HWND hWnd, LPWSTR lpString, int nMaxCount) { return 0; }
inline BOOL OpenClipboard(HWND hWndNewOwner) { return FALSE; }
inline HANDLE GetClipboardData(UINT uFormat) { return nullptr; }
inline LPVOID GlobalLock(HGLOBAL hMem) { return nullptr; }
inline BOOL GlobalUnlock(HGLOBAL hMem) { return FALSE; }
inline BOOL CloseClipboard() { return FALSE; }
inline BOOL DeleteDC(HDC hdc) { return FALSE; }
inline BOOL DeleteObject(HGDIOBJ ho) { return FALSE; }
inline BOOL MoveToEx(HDC hdc, int x, int y, LPPOINT lppt) { return FALSE; }
inline BOOL LineTo(HDC hdc, int x, int y) { return FALSE; }

// GDI Macros/Constants
#define SRCCOPY 0x00CC0020
#define GRADIENT_FILL_RECT_H 0x00000000
#define GRADIENT_FILL_RECT_V 0x00000001
#define DC_PEN 18
#define NULL_PEN 8
#define DC_BRUSH 19
#define NULL_BRUSH 5
#define TRANSPARENT 1
#define DT_RIGHT 0x00000002
#define DT_NOPREFIX 0x00000800
#define DT_LEFT 0x00000000
#define COLOR_ACTIVECAPTION 2
#define DT_CALCRECT 0x00000400
#define DT_CENTER 0x00000001
#define CF_UNICODETEXT 13
#define CF_TEXT 1

// GDI Stubs (continued)
inline BOOL ClientToScreen(HWND hWnd, LPPOINT lpPoint) { if (lpPoint) { /* No-op, just to compile */ } return TRUE; }
inline HWND WindowFromPoint(POINT Point) { return nullptr; }
inline HWND SetCapture(HWND hWnd) { return nullptr; }
inline BOOL ReleaseCapture() { return TRUE; }
inline HWND SetFocus(HWND hWnd) { return nullptr; }
inline HWND CreateWindowEx(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) { return nullptr; }
inline LRESULT SendMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0; }
inline BOOL MessageBeep(UINT uType) { return FALSE; }
inline COLORREF SetTextColor(HDC hdc, COLORREF color) { return 0; }
inline BOOL GetClientRect(HWND hWnd, LPRECT lpRect) { if (lpRect) { lpRect->left = 0; lpRect->top = 0; lpRect->right = 1024; lpRect->bottom = 768; } return TRUE; }

// GDI Macros/Constants (continued)
#define WS_TABSTOP 0x00010000L
#define WS_VISIBLE 0x10000000L
#define WS_CHILD 0x40000000L
#define ES_AUTOHSCROLL 0x00000080L
#define WS_BORDER 0x00800000L
#define EM_SETSEL 0x000000B1
#define WM_SETFONT 0x0030

// GDI Stubs (continued)
inline HWND GetDesktopWindow() { return nullptr; }

// --- New GDI stubs and definitions ---

// Basic types and structs
typedef size_t SIZE_T; // Make SIZE_T a size_t
typedef struct tagLOGFONTW {
    LONG  lfHeight;
    LONG  lfWidth;
    LONG  lfEscapement;
    LONG  lfOrientation;
    LONG  lfWeight;
    BYTE  lfItalic;
    BYTE  lfUnderline;
    BYTE  lfStrikeOut;
    BYTE  lfCharSet;
    BYTE  lfOutPrecision;
    BYTE  lfClipPrecision;
    BYTE  lfQuality;
    BYTE  lfPitchAndFamily;
    wchar_t lfFaceName[32]; // Corrected WCHAR to wchar_t
} LOGFONTW, *PLOGFONTW, *LPLOGFONTW;
typedef LOGFONTW LOGFONT;
typedef LOGFONT* LPLOGFONT; // Added for LPLOGFONT

typedef struct tagTEXTMETRICW {
    LONG        tmHeight;
    LONG        tmAscent;
    LONG        tmDescent;
    LONG        tmInternalLeading;
    LONG        tmExternalLeading;
    LONG        tmAveCharWidth;
    LONG        tmMaxCharWidth;
    LONG        tmWeight;
    LONG        tmOverhang;
    LONG        tmDigitizedAspectX;
    LONG        tmDigitizedAspectY;
    wchar_t       tmFirstChar; // Corrected WCHAR to wchar_t
    wchar_t       tmLastChar;  // Corrected WCHAR to wchar_t
    wchar_t       tmDefaultChar; // Corrected WCHAR to wchar_t
    wchar_t       tmBreakChar; // Corrected WCHAR to wchar_t
    BYTE        tmItalic;
    BYTE        tmUnderlined;
    BYTE        tmStruckOut;
    BYTE        tmPitchAndFamily;
    BYTE        tmCharSet;
} TEXTMETRICW, *PTEXTMETRICW, *LPTEXTMETRICW;
typedef TEXTMETRICW TEXTMETRIC;
typedef TEXTMETRIC* LPTEXTMETRIC; // Added for LPTEXTMETRIC

typedef struct tagWINDOWPLACEMENT {
  UINT  length;
  UINT  flags;
  UINT  showCmd;
  POINT ptMinPosition;
  POINT ptMaxPosition;
  RECT  rcNormalPosition;
} WINDOWPLACEMENT, *PWINDOWPLACEMENT, *LPWINDOWPLACEMENT;

typedef struct tagCHOOSECOLORW {
  DWORD        lStructSize;
  HWND         hwndOwner;
  HWND         hInstance;
  COLORREF     rgbResult;
  COLORREF*    lpCustColors;
  DWORD        Flags;
  LPARAM       lCustData;
  LPCWSTR      lpfnHook; // Placeholder, actually a func pointer
  LPCWSTR      lpTemplateName;
} CHOOSECOLORW, *LPCHOOSECOLORW;
typedef CHOOSECOLORW CHOOSECOLOR;
typedef CHOOSECOLOR* LPCHOOSECOLOR;

typedef struct tagTPMPARAMS {
  UINT cbSize;
  RECT rcExclude;
} TPMPARAMS, *LPTPMPARAMS;

typedef void* HMONITOR;

// Function pointer types
typedef int (CALLBACK *GDI_FONTOUTENUMPROC)(const LOGFONT*, const TEXTMETRIC*, DWORD, LPARAM);
typedef BOOL (CALLBACK *MONITORENUMPROC)(HMONITOR, HDC, LPRECT, LPARAM);

// Macros (constants)
#define WM_COPY 0x0301
#define WM_CUT 0x0300
#define WM_PASTE 0x0302
#define WM_CLOSE 0x0010
#define GWL_STYLE -16
#define ES_PASSWORD 0x0020
#define EM_SETPASSWORDCHAR 0x00CC
#define SW_MAXIMIZE 3
#define SW_NORMAL 1
#define WS_POPUP 0x80000000L
#define WS_CAPTION 0x00C00000L
#define WS_SYSMENU 0x00080000L
#define WS_THICKFRAME 0x00040000L
#define WS_MINIMIZEBOX 0x00020000L
#define WS_MAXIMIZEBOX 0x00010000L
#define WS_OVERLAPPED (0x00000000L | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_CLIPCHILDREN 0x02000000L
#define WS_CLIPSIBLINGS 0x04000000L
#define DEFAULT_CHARSET 1
#define FW_BOLD 700
#define OUT_TT_ONLY_PRECIS 4
#define CLIP_DEFAULT_PRECIS 0
#define PROOF_QUALITY 2
#define DEFAULT_PITCH 0
#define FF_ROMAN 48
#define FW_NORMAL 400
#define FF_MODERN 16
#define DEFAULT_GUI_FONT 17
#define GWLP_HINSTANCE -6
#define MF_SEPARATOR 0x00000800L
#define MF_STRING 0x00000000L
#define TPM_LEFTALIGN 0x0000
#define TPM_TOPALIGN 0x0000
#define TPM_RETURNCMD 0x0100L
#define TPM_NONOTIFY 0x0080L
#define LB_SETSEL 0x0185
#define CB_SETCURSEL 0x014E
#define SM_CXVIRTUALSCREEN 76
#define SM_CYVIRTUALSCREEN 77
#define SM_CXSCREEN 0
#define SM_CYSCREEN 1
#define CC_RGBINIT 0x00000001
#define SWP_NOZORDER 0x0004
#define WS_EX_CLIENTEDGE 0x00000200L

// New Function Stubs
inline BOOL GetWindowRect(HWND hWnd, LPRECT lpRect) { return FALSE; }
inline BOOL MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint) { return FALSE; }
inline HBRUSH CreateSolidBrush(COLORREF color) { return nullptr; }
inline UINT_PTR SetTimer(HWND hWnd, UINT_PTR nIDEvent, UINT uElapse, void (CALLBACK *lpTimerFunc)(HWND, UINT, UINT_PTR, DWORD)) { return 0; }
inline BOOL KillTimer(HWND hWnd, UINT_PTR nIDEvent) { return FALSE; }
inline LONG SetWindowLong(HWND hWnd, int nIndex, LONG dwNewLong) { return 0; }
inline BOOL ShowWindow(HWND hWnd, int nCmdShow) { return FALSE; }
inline HFONT CreateFont(int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCWSTR pszFaceName) { return nullptr; }
inline BOOL EnumFontFamiliesEx(HDC hdc, LPLOGFONT lpLogfont, GDI_FONTOUTENUMPROC lpEnumFontFamExProc, LPARAM lParam, DWORD dwFlags) { return FALSE; }
inline BOOL EmptyClipboard() { return FALSE; }
inline HGLOBAL GlobalAlloc(UINT uFlags, SIZE_T dwBytes) { return nullptr; }
inline UINT RegisterClipboardFormat(LPCWSTR lpszFormat) { return 0; }
inline BOOL SetClipboardData(UINT uFormat, HANDLE hMem) { return FALSE; }
inline BOOL IsWindowEnabled(HWND hWnd) { return TRUE; }
inline LONG GetWindowLong(HWND hWnd, int nIndex) { return 0; }
inline BOOL SetWindowText(HWND hWnd, LPCWSTR lpString) { return FALSE; }
inline BOOL GetWindowPlacement(HWND hWnd, LPWINDOWPLACEMENT lpwpl) { return FALSE; }
inline BOOL SetWindowPlacement(HWND hWnd, const WINDOWPLACEMENT* lpwpl) { return FALSE; }
inline int GetSystemMetrics(int nIndex) { return 0; }
inline BOOL ChooseColor(LPCHOOSECOLOR lpcc) { return FALSE; }
inline HMENU CreatePopupMenu() { return nullptr; }
inline BOOL AppendMenu(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem) { return FALSE; }
inline BOOL TrackPopupMenuEx(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm) { return FALSE; }
inline BOOL DestroyMenu(HMENU hMenu) { return FALSE; }
inline BOOL EnumDisplayMonitors(HDC hdc, LPRECT lprcClip, MONITORENUMPROC lpfnEnum, LPARAM dwData) { return FALSE; }
inline BOOL IntersectRect(LPRECT lprcDst, const RECT* lprcSrc1, const RECT* lprcSrc2) { return FALSE; }
inline BOOL SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) { return FALSE; }
inline BOOL ScreenToClient(HWND hWnd, LPPOINT lpPoint) { return FALSE; }
inline LONG_PTR GetWindowLongPtr(HWND hWnd, int nIndex) { return 0; }

// --- More New GDI stubs and definitions ---

// Types (structs/enums/typedefs)
typedef struct tagSCROLLINFO {
    UINT cbSize;
    UINT fMask;
    int  nMin;
    int  nMax;
    UINT nPage;
    int  nPos;
    int  nTrackPos;
} SCROLLINFO, *LPCSCROLLINFO;

typedef struct tagOFNW {
    DWORD        lStructSize;
    HWND         hwndOwner;
    HINSTANCE    hInstance;
    LPCWSTR      lpstrFilter;
    LPWSTR       lpstrCustomFilter;
    DWORD        nMaxCustFilter;
    LPWSTR       lpstrFilterIndex; // should be LPDWORD
    LPWSTR       lpstrFile;
    DWORD        nMaxFile;
    LPWSTR       lpstrFileTitle;
    DWORD        nMaxFileTitle;
    LPCWSTR      lpstrInitialDir;
    LPCWSTR      lpstrTitle;
    DWORD        Flags;
    WORD         nFileOffset;
    WORD         nFileExtension;
    LPCWSTR      lpstrDefExt;
    LPARAM       lCustData;
    LPCWSTR      lpfnHook; // Placeholder
    LPCWSTR      lpTemplateName;
    void*        pvReserved;
    DWORD        dwReserved;
    DWORD        FlagsEx;
} OPENFILENAMEW, *LPOPENFILENAMEW;
typedef OPENFILENAMEW OPENFILENAME, *LPOPENFILENAME;

typedef struct _SHITEMID {
    WORD cb;
    BYTE abID[1];
} SHITEMID, *LPSHITEMID;

typedef struct _ITEMIDLIST {
    SHITEMID mkid;
} ITEMIDLIST, *LPITEMIDLIST, *PCITEMIDLIST, *LPCITEMIDLIST;

typedef struct _browseinfoW {
    HWND        hwndOwner;
    PCITEMIDLIST pidlRoot; // Not a standard Windows type, should be an ITEMIDLIST
    LPWSTR      pszDisplayName;
    LPCWSTR     lpszTitle;
    UINT        ulFlags;
    void* lpfn; // Placeholder for BFFCALLBACK
    LPARAM      lParam;
    int         iImage;
} BROWSEINFOW, *PBROWSEINFOW, *LPBROWSEINFOW;
typedef BROWSEINFOW BROWSEINFO, *LPBROWSEINFO;
typedef int INT;
typedef SIZE *LPSIZE;

typedef void* LPMALLOC; // Placeholder for IMalloc interface pointer

#define CCHDEVICENAME 32
#define CCHFORMNAME   32
typedef struct _devicemodeW {
    wchar_t dmDeviceName[CCHDEVICENAME];
    WORD dmSpecVersion;
    WORD dmDriverVersion;
    WORD dmSize;
    WORD dmDriverExtra;
    DWORD dmFields;
    short dmOrientation;
    short dmPaperSize;
    short dmPaperLength;
    short dmPaperWidth;
    short dmScale;
    short dmCopies;
    short dmDefaultSource;
    short dmPrintQuality;
    short dmColor;
    short dmDuplex;
    short dmYResolution;
    short dmTTOption;
    short dmCollate;
    wchar_t dmFormName[CCHFORMNAME];
    WORD dmLogPixels;
    DWORD dmBitsPerPel;
    DWORD dmPelsWidth;
    DWORD dmPelsHeight;
    union {
        DWORD dmDisplayFlags;
        DWORD dmNup;
    } DUMMYUNIONNAME;
    DWORD dmDisplayFrequency;
    DWORD dmICMMethod;
    DWORD dmICMIntent;
    DWORD dmMediaType;
    DWORD dmDitherType;
    DWORD dmReserved1;
    DWORD dmReserved2;
    DWORD dmPanningWidth;
    DWORD dmPanningHeight;
} DEVMODEW, *PDEVMODEW, *LPDEVMODEW;
typedef DEVMODEW DEVMODE;

typedef void* HRGN; // Handle to region
typedef int HRESULT_INT; // Use int as placeholder for HRESULT
#define S_OK 0 // Standard value for S_OK

// Functions
inline HPEN CreatePen(int iStyle, int cWidth, COLORREF color) { return nullptr; }
inline void InitCommonControls() {}
inline BOOL GetSaveFileName(LPOPENFILENAME lpofn) { return FALSE; }
inline BOOL GetOpenFileName(LPOPENFILENAME lpofn) { return FALSE; }
inline HRESULT_INT CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit) { return S_OK; }
inline LPITEMIDLIST SHBrowseForFolder(LPBROWSEINFO lpbi) { return nullptr; }
inline BOOL SHGetPathFromIDList(LPCITEMIDLIST pidl, LPWSTR pszPath) { return FALSE; }
inline HRESULT_INT SHGetMalloc(LPMALLOC* ppMalloc) { return S_OK; }
inline HINSTANCE ShellExecute(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd) { return nullptr; }
inline HWND CreateWindow(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) { return nullptr; }
inline int TabCtrl_SetCurSel(HWND hwnd, int iItem) { return 0; }
inline BOOL EnableWindow(HWND hWnd, BOOL bEnable) { return TRUE; }
inline int ScrollWindowEx(HWND hWnd, int dx, int dy, const RECT* prcScroll, const RECT* prcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT flags) { return 0; }
inline BOOL SetScrollInfo(HWND hwnd, int nBar, LPCSCROLLINFO lpsi, BOOL bRedraw) { return TRUE; }
inline BOOL GetTextExtentPoint32(HDC hdc, LPCWSTR lpString, int c, LPSIZE lpsz) { if (lpsz) { lpsz->cx = c * 8; lpsz->cy = 16; } return TRUE; }
inline HINSTANCE GetModuleHandle(LPCWSTR lpModuleName) { return nullptr; }
inline HBITMAP LoadBitmap(HINSTANCE hInstance, LPCWSTR lpBitmapName) { return nullptr; }


// Macros (constants)
#define MAKELPARAM(l,h) ((LPARAM)(((WORD)(l)) | ((DWORD)((WORD)(h))) << 16))
#define MAKELONG(l,h) ((LONG)(((WORD)(l)) | ((DWORD)((WORD)(h))) << 16))
#define COLOR_WINDOW 1
#define FW_LIGHT 300
#define CLEARTYPE_QUALITY 5
#define DT_NOCLIP 0x0100
#define ANSI_FIXED_FONT 11
inline BOOL OffsetRect(LPRECT lprc, int dx, int dy) { if (lprc) { lprc->left += dx; lprc->right += dx; lprc->top += dy; lprc->bottom += dy; return TRUE; } return FALSE; }
#define MB_ICONEXCLAMATION MB_ICONWARNING // Re-use existing
#define BM_SETIMAGE 0x00F5
#define IMAGE_BITMAP 0
#define BS_CHECKBOX 0x00000002
#define BS_PUSHLIKE 0x00000004
#define BS_PUSHBUTTON 0x00000000
#define BS_BITMAP 0x00000080
#define BS_MULTILINE 0x00002000
#define BS_NOTIFY 0x00004000
#define SWP_NOSIZE 0x0001
#define SWP_NOCOPYBITS 0x00000400L
#define DT_SINGLELINE 0x00000020
#define DT_WORDBREAK 0x00000010
#define COLOR_INFOBK 24
#define BLACK_PEN 6
#define PS_SOLID 0
#define WHITE_BRUSH 0
#define LTGRAY_BRUSH 1
#define COLOR_INFOTEXT 20
#define SW_HIDE 0
#define TTM_DELTOOL (WM_USER + 4)
#define COLOR_3DFACE 15
#define SIF_POS 0x0004
#define SB_VERT 1
#define SB_HORZ 0
#define OFN_OVERWRITEPROMPT 0x00000002
#define OFN_HIDEREADONLY 0x00000004
#define OFN_PATHMUSTEXIST 0x00000800
#define OFN_FILEMUSTEXIST 0x00001000
#define COINIT_APARTMENTTHREADED 0x2
#define BIF_RETURNONLYFSDIRS 0x00000001
#define BIF_EDITBOX 0x00000010
#define BIF_NEWDIALOGSTYLE 0x00000040
#define TOOLTIPS_CLASS L"tooltips_class32"
#define TTS_ALWAYSTIP 0x01
#define CW_USEDEFAULT ((int)0x80000000)
#define TTF_IDISHWND 0x0001
#define TTF_SUBCLASS 0x0010
#define TTM_ADDTOOLW (WM_USER + 50)
#define TTM_SETMAXTIPWIDTH (WM_USER + 30)
#define TTM_UPDATETIPTEXTW (WM_USER + 59)
#define TabCtrl_SetCurSel(hwnd, iItem) 0
#define STD_PRINT 100 // Arbitrary value
#define STD_COPY 101 // Arbitrary value
#define STD_PASTE 102 // Arbitrary value
#define SW_SHOWNORMAL 1

#endif