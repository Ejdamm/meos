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


#include "stdafx.h"
#include <fstream>
#include <mutex>
#include <thread>
#include <chrono>
#include "socket.h"
#include "platform_socket.h"
#include "meosexception.h"
#include <iostream>

//#define MEOS_DIRECT_PORT 21338


DirectSocket::DirectSocket(int cmpId, int p) {
  competitionId = cmpId;
  port = p;
  shutDown = false;
  sendSocket = PLATFORM_INVALID_SOCKET;
  hDestinationWindow = 0;
  clearQueue = false;
}

DirectSocket::~DirectSocket() {
  {
    std::lock_guard<std::mutex> lock(syncObj);
    shutDown = true;
  }

  if (sendSocket != PLATFORM_INVALID_SOCKET) {
    platform_socket_close(sendSocket);
    sendSocket = PLATFORM_INVALID_SOCKET;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  shutDown = true;
}

void DirectSocket::addPunchInfo(const SocketPunchInfo &pi) {
  //OutputDebugString("Enter punch in queue\n");
  {
    std::lock_guard<std::mutex> lock(syncObj);
    if (clearQueue)
      messageQueue.clear();
    clearQueue = false;
    messageQueue.push_back(pi);
  }
#ifdef _WIN32
  PostMessage(hDestinationWindow, WM_USER + 3, 0,0);
#endif
}

void DirectSocket::getPunchQueue(vector<SocketPunchInfo> &pq) {
  pq.clear();

  std::lock_guard<std::mutex> lock(syncObj);
  if (!clearQueue)
    pq.insert(pq.begin(), messageQueue.begin(), messageQueue.end());

  clearQueue = true;
  return;
}

void DirectSocket::listenDirectSocket() {

  platform_socket_t clientSocket = platform_socket_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (clientSocket == PLATFORM_INVALID_SOCKET) {
    throw meosException("Socket error");
  }

  struct sockaddr_in UDPserveraddr;
  memset(&UDPserveraddr,0, sizeof(UDPserveraddr));
  UDPserveraddr.sin_family = AF_INET;
  UDPserveraddr.sin_port = htons(port);
  UDPserveraddr.sin_addr.s_addr = INADDR_ANY;

  if (platform_socket_bind(clientSocket, (struct sockaddr*)&UDPserveraddr, sizeof(struct sockaddr_in)) < 0) {
    throw meosException("Socket error");
  }

  fd_set fds;
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 200000;

  while (!shutDown) {
    FD_ZERO(&fds);
    FD_SET(clientSocket, &fds);

    int rc = platform_socket_select(clientSocket, &fds, NULL, NULL, &timeout);

    if (shutDown) {
      platform_socket_close(clientSocket);
      return;
    }

    if (rc > 0) {
      ExtPunchInfo pi;
      struct sockaddr_in clientaddr;
      int len = sizeof(clientaddr);
      if (platform_socket_recvfrom(clientSocket, (char*)&pi, sizeof(pi), 0, (struct sockaddr*)&clientaddr, &len) > 0) {
        if (pi.cmpId == competitionId)
          addPunchInfo(pi.punch);
      }
    }
  }
  platform_socket_close(clientSocket);
}

#ifdef _WIN32
extern HWND hWndMain;
#endif

void startListeningDirectSocket(void *p) {
  wstring error;
  try {
    ((DirectSocket*)p)->listenDirectSocket();
  }
  catch (const meosException &ex) {
    error = ex.wwhat();
  }
  catch (std::exception &ex) {
    string ne = ex.what();
    error.insert(error.begin(), ne.begin(), ne.end());
  }
  catch (...) {
    error = L"Unknown error";
  }
  if (!error.empty()) {
#ifdef _WIN32
    PostMessage(hWndMain, WM_USER + 5, 0, 0);
#endif
  }
}

void DirectSocket::startUDPSocketThread(HWND targetWindow) {
  hDestinationWindow = targetWindow;
  std::thread(startListeningDirectSocket, this).detach();
}

void DirectSocket::sendPunch(SocketPunchInfo &pi) {

  if (sendSocket == PLATFORM_INVALID_SOCKET) {
    platform_socket_init();

    sendSocket = platform_socket_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sendSocket == PLATFORM_INVALID_SOCKET) {
      throw meosException("Socket error");
    }
    char opt = 1;
    platform_socket_setsockopt(sendSocket, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(char));
  }

  struct sockaddr_in brdcastaddr;
  memset(&brdcastaddr,0, sizeof(brdcastaddr));
  brdcastaddr.sin_family = AF_INET;
  brdcastaddr.sin_port = htons(port);
  brdcastaddr.sin_addr.s_addr = INADDR_BROADCAST;

  int len = sizeof(brdcastaddr);

  ExtPunchInfo epi;
  epi.cmpId = competitionId;
  epi.punch = pi;

  int ret = platform_socket_sendto(sendSocket, (char*)&epi, sizeof(epi), 0, (struct sockaddr*)&brdcastaddr, len);

  if (ret < 0) {
#ifdef _WIN32
    OutputDebugStringA("Error broadcasting to the clients");
#endif
  }
}
