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
- Common enums like `RunnerStatus` and `SubSecond` are moved to `src/util/common_enums.h` to avoid duplicate definitions and circular dependencies between `util` and `domain`.

## Porting Patterns
- When migrating tightly coupled domain entities, use heavy stubbing in `oEvent.h`, `oClass.h`, etc., to allow incremental migration.
- Always use `getId()` instead of direct `Id` access if the base class or stubs might change.
- Use `narrow()` and `widen()` from `meos_util.h` for easy string conversions during migration.

## Dependencies
- Depends on `util` library.
- GUI-related methods in `oDataContainer` are wrapped in `#ifdef _WIN32` and stubbed for Linux.
