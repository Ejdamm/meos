#pragma once
// Cross-platform UTF-8 / wstring conversion utilities.
// Replaces gdioutput::toUTF8 / fromUTF8 / widen / narrow for server builds.

#include <string>

#ifdef _WIN32
#include <windows.h>

inline std::wstring platform_fromUTF8(const std::string &input) {
  if (input.empty()) return {};
  int len = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), (int)input.size(), nullptr, 0);
  std::wstring out(len, 0);
  MultiByteToWideChar(CP_UTF8, 0, input.c_str(), (int)input.size(), &out[0], len);
  return out;
}

inline std::string platform_toUTF8(const std::wstring &input) {
  if (input.empty()) return {};
  int len = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), (int)input.size(), nullptr, 0, nullptr, nullptr);
  std::string out(len, 0);
  WideCharToMultiByte(CP_UTF8, 0, input.c_str(), (int)input.size(), &out[0], len, nullptr, nullptr);
  return out;
}

inline std::wstring platform_widen(const std::string &input, int cp = 1252) {
  if (input.empty()) return {};
  int len = MultiByteToWideChar(cp, MB_PRECOMPOSED, input.c_str(), (int)input.size(), nullptr, 0);
  std::wstring out(len, 0);
  MultiByteToWideChar(cp, MB_PRECOMPOSED, input.c_str(), (int)input.size(), &out[0], len);
  return out;
}

inline std::string platform_narrow(const std::wstring &input, int cp = 1252) {
  if (input.empty()) return {};
  BOOL usedDef = FALSE;
  int len = WideCharToMultiByte(cp, 0, input.c_str(), (int)input.size(), nullptr, 0, "?", &usedDef);
  std::string out(len, 0);
  WideCharToMultiByte(cp, 0, input.c_str(), (int)input.size(), &out[0], len, "?", &usedDef);
  return out;
}

#else
// POSIX: use std::codecvt (deprecated in C++17 but universally available)
#include <locale>
#include <codecvt>

inline std::wstring platform_fromUTF8(const std::string &input) {
  if (input.empty()) return {};
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
  return conv.from_bytes(input);
}

inline std::string platform_toUTF8(const std::wstring &input) {
  if (input.empty()) return {};
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
  return conv.to_bytes(input);
}

inline std::wstring platform_widen(const std::string &input, int /*cp*/ = 1252) {
  // On POSIX, assume input is already UTF-8 or Latin-1; best effort via codecvt
  if (input.empty()) return {};
  std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
  try { return conv.from_bytes(input); } catch (...) { return {}; }
}

inline std::string platform_narrow(const std::wstring &input, int /*cp*/ = 1252) {
  if (input.empty()) return {};
  std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
  try { return conv.to_bytes(input); } catch (...) { return {}; }
}
#endif
