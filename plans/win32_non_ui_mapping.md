# Task 2.10: Win32 API Mapping Outside UI Layer

## Sockets (src/net/)

### socket.h / socket.cpp
- `#include <winsock2.h>` — WinSock2 types
- `SOCKET sendSocket` — WinSock SOCKET type
- `CRITICAL_SECTION syncObj` — Windows mutex primitive
- `WSAStartup()` / `WSACleanup()` (implied by winsock2 init in socket.cpp:162–163)
- `socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)` — create UDP socket
- `closesocket()` — close socket (not POSIX `close()`)
- `htons()` — byte order
- `setsockopt()` — socket options
- `InitializeCriticalSection` / `EnterCriticalSection` / `LeaveCriticalSection` / `DeleteCriticalSection`
- **Migration**: platform_socket.h abstraction (task 2.11), CRITICAL_SECTION → std::mutex (task 2.13)

### download.h / download.cpp
- `#include <Wininet.h>` — WinInet HTTP/FTP library (Windows-only)
- `#include <winsock2.h>`, `#include <iphlpapi.h>` — network interface info
- `#include <io.h>`, `#include <process.h>` — low-level I/O and process
- `HINTERNET` handle type (typedef'd to HANDLE in download.h for non-Windows)
- `InternetOpen()` — initialize WinInet session
- `InternetOpenUrl()` — open URL
- `InternetConnect()` — connect to host
- `HttpOpenRequest()` — create HTTP request
- **Migration**: Replace with libcurl or std::net / Asio (task 2.14)

## File System (src/util/, src/util/zip.cpp)

### meos_util.h / meos_util.cpp
- `HANDLE lockedFile` / `INVALID_HANDLE_VALUE` — file handle
- `GetFileAttributes()` — check file existence
- `FindFirstFile()` / `FindNextFile()` / `FindClose()` — directory enumeration
- `CreateFile()` — file locking (MeOSFileLock class, line 2303, 2470)
- `MultiByteToWideChar(CP_UTF8, ...)` — string encoding (line 2456)
- **Migration**: `std::filesystem::exists`, `std::filesystem::directory_iterator`, `std::fopen`+`flock`/`LockFile` abstraction (task 2.12)

### zip.cpp
- `#include <direct.h>`, `#include <io.h>` — Windows low-level I/O
- `#include "minizip/iowin32.h"` — WinAPI file I/O backend for minizip
- `HANDLE hFile` — Windows file handle
- `CreateFile()` — open file for minizip
- `CreateDirectory()` — create directory
- `FindFirstFile()` / `FindNextFile()` — enumerate files
- **Migration**: Use minizip POSIX backend (`ioapi.h`) via `#ifdef`, `std::filesystem::create_directory` (task 2.12)

## String Encoding (src/util/, src/io/)

### localizer.cpp
- `MultiByteToWideChar(CP_UTF8, ...)` — UTF-8 to wide (lines 38, 46, 530, 534)
- `WideCharToMultiByte(CP_UTF8, ...)` — wide to UTF-8 (line 38)
- **Migration**: Use `std::codecvt` or `iconv` / platform abstraction (task 2.12)

### xmlparser.cpp
- `MultiByteToWideChar(CP_UTF8, ...)` — multiple locations (lines 932–1031)
- `WideCharToMultiByte(CP_ACP, ...)` — wide to ANSI (line 998)
- **Migration**: Platform abstraction for string conversion (task 2.12)

### csvparser.cpp
- `MultiByteToWideChar(CP_UTF8, ...)` — lines 1243, 1258
- **Migration**: Platform abstraction (task 2.12)

## Threads & Synchronization (src/db/, src/net/)

### mysqldaemon.cpp
- `#include <process.h>` — Windows CRT thread functions
- `static CRITICAL_SECTION CS_MySQL` — mutex for MySQL operations
- `_beginthreadex()` — start reconnect thread (line 134)
- `HANDLE hThread` — thread handle
- `InitializeCriticalSection` / `EnterCriticalSection` / `LeaveCriticalSection` / `DeleteCriticalSection`
- **Migration**: `std::thread` + `std::mutex` (task 2.13)

## Summary of Migration Tasks

| Category | Files | Migration Task |
|---|---|---|
| WinSock sockets | socket.h/cpp | 2.11 |
| WinInet HTTP | download.h/cpp | 2.14 |
| File system / directory ops | meos_util.h/cpp, zip.cpp | 2.12 |
| String encoding (MBCS↔wide) | localizer.cpp, xmlparser.cpp, csvparser.cpp | 2.12 |
| Threads (CRITICAL_SECTION, _beginthreadex) | mysqldaemon.cpp, socket.h/cpp | 2.13 |
