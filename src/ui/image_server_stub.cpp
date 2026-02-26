// image_server_stub.cpp
// Provides no-op Image class implementations for the MeOS-server build.
// image.cpp cannot be compiled in the server build because it includes
// "png/png.h" which was removed. This stub satisfies the linker.
#ifdef MEOS_SERVER

#include "stdafx.h"
#include "image.h"

Image image; // Global definition used by domain/io files

Image::Image() = default;
Image::~Image() = default;

HBITMAP Image::loadImage(uint64_t, ImageMethod) { return nullptr; }
HBITMAP Image::getVersion(uint64_t, int, int) { return nullptr; }

vector<uint8_t> Image::loadResourceToMemory(LPCTSTR, LPCTSTR) { return {}; }

int Image::getWidth(uint64_t) { return 0; }
int Image::getHeight(uint64_t) { return 0; }

void Image::drawImage(uint64_t, ImageMethod, HDC, int, int, int, int, int, int, int, int) {}

uint64_t Image::loadFromFile(const wstring &, ImageMethod) { return 0; }
uint64_t Image::loadFromMemory(const wstring &, const vector<uint8_t> &, ImageMethod) { return 0; }
void Image::provideFromMemory(uint64_t, const wstring &, const vector<uint8_t> &) {}
void Image::addImage(uint64_t, const wstring &) {}
void Image::clearLoaded() {}
void Image::enumerateImages(vector<pair<wstring, size_t>> &) const {}
uint64_t Image::getIdFromEnumeration(int) const { return 0; }
int Image::getEnumerationIxFromId(uint64_t) const { return -1; }
bool Image::hasImage(uint64_t) const { return false; }
void Image::reloadImage(uint64_t, ImageMethod) {}

const wstring &Image::getFileName(uint64_t) const {
  static wstring empty;
  return empty;
}

static vector<uint8_t> emptyBytes;
const vector<uint8_t> &Image::getRawData(uint64_t) const {
  return emptyBytes;
}

Image::Bmp::~Bmp() {}
HBITMAP Image::Bmp::getVersion(int &, int &) { return nullptr; }
void Image::Bmp::destroy() {}

#endif // MEOS_SERVER
