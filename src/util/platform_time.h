#pragma once
#include <cstdint>
#ifdef _WIN32
#include <windows.h>
inline uint32_t getTickMs() { return static_cast<uint32_t>(GetTickCount()); }
inline uint64_t getTickMs64() { return GetTickCount64(); }
#else
#include <chrono>
inline uint32_t getTickMs() {
  return static_cast<uint32_t>(
    std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now().time_since_epoch()).count());
}
inline uint64_t getTickMs64() {
  return static_cast<uint64_t>(
    std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now().time_since_epoch()).count());
}
#endif
