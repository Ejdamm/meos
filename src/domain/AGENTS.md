# Domain Module

## Overview
This module contains the domain base classes and logic for MeOS.

## Domain Base Classes
- `oBase`: The base class for all domain objects. Handles change tracking and synchronization.
- `oDataContainer`: Handles metadata and data access for domain objects via `oDataInterface`.

## Porting Notes
- Win32-specific functions like `strcpy_s` and `swprintf_s` are handled via macros in `oDataContainer.cpp` or replaced by standard functions.
- `oEvent` is currently a stub in `oEvent.h` to allow compilation of base classes. It will be fully migrated in US-014.
- `long long` is used for 64-bit identifiers for compatibility with `meos_util.h`'s `convertDynamicBase`.
- `NOTIME` is defined as `0x7FFFFFFF`.

## Dependencies
- Depends on `util` library.
- GUI-related methods in `oDataContainer` are wrapped in `#ifdef _WIN32` and stubbed for Linux.
