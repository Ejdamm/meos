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

#pragma once

// Cross-platform socket abstraction.
// Provides a unified API over WinSock2 (Windows) and POSIX sockets (Linux/macOS).

#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
   typedef SOCKET platform_socket_t;
#  define PLATFORM_INVALID_SOCKET  INVALID_SOCKET
#  define PLATFORM_SOCKET_ERROR    SOCKET_ERROR
#else
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <sys/select.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <unistd.h>
#  include <poll.h>
   typedef int platform_socket_t;
#  define PLATFORM_INVALID_SOCKET  (-1)
#  define PLATFORM_SOCKET_ERROR    (-1)
#endif

#include <cstdint>

// Platform-specific length types used internally by platform_socket.cpp.
#ifdef _WIN32
  typedef int    socklen_platform;
  typedef int    size_platform;
#else
  typedef socklen_t socklen_platform;
  typedef size_t    size_platform;
#endif

// --- Lifecycle ---------------------------------------------------------------

/// Call once at application startup before any socket operations (no-op on POSIX).
bool platform_socket_init();

/// Call once at application shutdown (no-op on POSIX).
void platform_socket_cleanup();

// --- Core operations ---------------------------------------------------------

/// Create a socket. Returns PLATFORM_INVALID_SOCKET on failure.
platform_socket_t platform_socket_create(int domain, int type, int protocol);

/// Bind a socket to an address.
int platform_socket_bind(platform_socket_t sock, const struct sockaddr *addr, int addrlen);

/// Put a socket into listening state.
int platform_socket_listen(platform_socket_t sock, int backlog);

/// Accept an incoming connection.
platform_socket_t platform_socket_accept(platform_socket_t sock, struct sockaddr *addr, int *addrlen);

/// Initiate a connection.
int platform_socket_connect(platform_socket_t sock, const struct sockaddr *addr, int addrlen);

/// Send data on a connected socket.
int platform_socket_send(platform_socket_t sock, const char *buf, int len, int flags);

/// Receive data from a connected socket.
int platform_socket_recv(platform_socket_t sock, char *buf, int len, int flags);

/// Send a datagram.
int platform_socket_sendto(platform_socket_t sock, const char *buf, int len, int flags,
                            const struct sockaddr *dest, int destlen);

/// Receive a datagram.
int platform_socket_recvfrom(platform_socket_t sock, char *buf, int len, int flags,
                              struct sockaddr *src, int *srclen);

/// Close a socket.
int platform_socket_close(platform_socket_t sock);

/// Set socket option.
int platform_socket_setsockopt(platform_socket_t sock, int level, int optname,
                                const char *optval, int optlen);

/// Synchronous I/O multiplexing (thin wrapper around select(2)).
/// On Windows the first argument is ignored; this wrapper handles that transparently.
int platform_socket_select(platform_socket_t nfds, fd_set *readfds, fd_set *writefds,
                            fd_set *exceptfds, struct timeval *timeout);
