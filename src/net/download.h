// Download.h: interface for the Download class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOWNLOAD_H__DEBC6296_9CAE_4FF6_867B_DD896D0B6A7A__INCLUDED_)
#define AFX_DOWNLOAD_H__DEBC6296_9CAE_4FF6_867B_DD896D0B6A7A__INCLUDED_

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


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#ifdef _WIN32
typedef HANDLE HINTERNET;

class dwException : public std::exception {
public:
  int code;
  dwException(const char *msg, int id) : std::exception(msg), code(id) {}
  virtual ~dwException() {}
};

class ProgressWindow;

class Download {
private:
  volatile uintptr_t hThread;
  volatile bool doExit;
  //HWND hProgress;

  HINTERNET hInternet;
  HINTERNET hURL;

  int fileno;

  DWORD bytesLoaded;
  DWORD bytesToLoad;
  bool success;
  void initThread();

  bool httpSendReqEx(HINTERNET hConnect, bool https, const wstring &dest, const vector< pair<wstring, wstring> > &headers,
                     const wstring &upFile, const wstring &outFile, ProgressWindow &pw, int &errroCode) const;

public:

  void postFile(const wstring &url, const wstring &file, const wstring &fileOut,
                const vector< pair<wstring, wstring> > &headers, ProgressWindow &pw);
  int processMessages();
  bool successful();
  bool isWorking();
  void setBytesToDownload(DWORD btd);
  void endDownload();
  void downloadFile(const wstring &url, const wstring &file, const vector< pair<wstring, wstring> > &headers);
  void initInternet();
  void shutDown();
  bool createDownloadThread();
  void downLoadNoThread() {initThread();}

  Download();
  virtual ~Download();

protected:
  bool doDownload();

  friend void SUThread(void *ptr);

};

#else // !_WIN32

// Stub for non-Windows platforms. HTTP download via WinINet is not available;
// methods throw std::runtime_error when called.

#include <stdexcept>

class dwException : public std::exception {
public:
  int code;
  dwException(const char *msg, int id) : std::exception(msg), code(id) {}
  virtual ~dwException() {}
};

class ProgressWindow;

class Download {
public:
  void postFile(const wstring &, const wstring &, const wstring &,
                const vector< pair<wstring, wstring> > &, ProgressWindow &) {
    throw std::runtime_error("Download::postFile not supported on this platform");
  }
  int  processMessages() { return 0; }
  bool successful()      { return false; }
  bool isWorking()       { return false; }
  void setBytesToDownload(unsigned long) {}
  void endDownload()     {}
  void downloadFile(const wstring &, const wstring &,
                    const vector< pair<wstring, wstring> > &) {
    throw std::runtime_error("Download::downloadFile not supported on this platform");
  }
  void initInternet() {}
  void shutDown()     {}
  bool createDownloadThread() { return false; }
  void downLoadNoThread()     {}

  Download()          {}
  virtual ~Download() {}
};

#endif // _WIN32

#endif // !defined(AFX_DOWNLOAD_H__DEBC6296_9CAE_4FF6_867B_DD896D0B6A7A__INCLUDED_)
