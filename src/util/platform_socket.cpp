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

#include "platform_socket.h"

// --- Lifecycle ---------------------------------------------------------------

bool platform_socket_init() {
#ifdef _WIN32
  WSADATA wsadata;
  WORD version = MAKEWORD(2, 2);
  return WSAStartup(version, &wsadata) == 0;
#else
  return true;
#endif
}

void platform_socket_cleanup() {
#ifdef _WIN32
  WSACleanup();
#endif
}

// --- Core operations ---------------------------------------------------------

platform_socket_t platform_socket_create(int domain, int type, int protocol) {
  return ::socket(domain, type, protocol);
}

int platform_socket_bind(platform_socket_t sock, const struct sockaddr *addr, int addrlen) {
  return ::bind(sock, addr, static_cast<socklen_platform>(addrlen));
}

int platform_socket_listen(platform_socket_t sock, int backlog) {
  return ::listen(sock, backlog);
}

platform_socket_t platform_socket_accept(platform_socket_t sock,
                                          struct sockaddr *addr, int *addrlen) {
#ifdef _WIN32
  return ::accept(sock, addr, addrlen);
#else
  socklen_t len = addrlen ? static_cast<socklen_t>(*addrlen) : 0;
  platform_socket_t result = ::accept(sock, addr, addrlen ? &len : nullptr);
  if (addrlen) *addrlen = static_cast<int>(len);
  return result;
#endif
}

int platform_socket_connect(platform_socket_t sock,
                             const struct sockaddr *addr, int addrlen) {
  return ::connect(sock, addr, static_cast<socklen_platform>(addrlen));
}

int platform_socket_send(platform_socket_t sock,
                          const char *buf, int len, int flags) {
  return static_cast<int>(::send(sock, buf, static_cast<size_platform>(len), flags));
}

int platform_socket_recv(platform_socket_t sock, char *buf, int len, int flags) {
  return static_cast<int>(::recv(sock, buf, static_cast<size_platform>(len), flags));
}

int platform_socket_sendto(platform_socket_t sock, const char *buf, int len, int flags,
                            const struct sockaddr *dest, int destlen) {
  return static_cast<int>(::sendto(sock, buf, static_cast<size_platform>(len), flags,
                                   dest, static_cast<socklen_platform>(destlen)));
}

int platform_socket_recvfrom(platform_socket_t sock, char *buf, int len, int flags,
                              struct sockaddr *src, int *srclen) {
#ifdef _WIN32
  return static_cast<int>(::recvfrom(sock, buf, len, flags, src, srclen));
#else
  socklen_t slen = srclen ? static_cast<socklen_t>(*srclen) : 0;
  int result = static_cast<int>(::recvfrom(sock, buf, static_cast<size_t>(len), flags,
                                            src, srclen ? &slen : nullptr));
  if (srclen) *srclen = static_cast<int>(slen);
  return result;
#endif
}

int platform_socket_close(platform_socket_t sock) {
#ifdef _WIN32
  return ::closesocket(sock);
#else
  return ::close(sock);
#endif
}

int platform_socket_setsockopt(platform_socket_t sock, int level, int optname,
                                const char *optval, int optlen) {
  return ::setsockopt(sock, level, optname, optval, static_cast<socklen_platform>(optlen));
}

int platform_socket_select(platform_socket_t nfds, fd_set *readfds, fd_set *writefds,
                            fd_set *exceptfds, struct timeval *timeout) {
#ifdef _WIN32
  // On Windows the first argument is ignored by WinSock
  (void)nfds;
  return ::select(0, readfds, writefds, exceptfds, timeout);
#else
  return ::select(static_cast<int>(nfds) + 1, readfds, writefds, exceptfds, timeout);
#endif
}
