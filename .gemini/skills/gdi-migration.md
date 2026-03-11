# GDI Output Migration Skill

## Context
MeOS legacy code uses a Win32-heavy `gdioutput` class for all layout and rendering. For the headless migration, we need a cross-platform version that provides layout metrics without requiring a GUI.

## Patterns

### 1. Headless gdioutput
When migrating classes that depend on `gdioutput` (like `MetaList`, `HTMLWriter`, `oBase`), use the abstract `gdioutput` base class defined in `src/util/gdioutput.h`.

### 2. Colors and Fonts
- Use `GDICOLOR` enum and `RGB(r,g,b)` macro from `src/util/gdioutput.h`.
- Use `gdiFonts` enum for font styles.
- When calculating background colors for zebra-striped tables, use the `MeOSUtil::HLS` class to manipulate lightness and saturation.

### 3. Layout Metrics
- `gdioutput::getLineHeight()`: Returns the default line height.
- `PageInfo::renderPages()`: Use this method to split a list of `TextInfo` objects into pages, especially for multi-column layouts. It handles page breaks and basic overflow logic.

### 4. String Handling
- Always use `MeOSUtil::toUTF8()` when writing to HTML or XML files.
- Use `MeOSUtil::encodeXML()` or `MeOSUtil::encodeHTML()` to escape special characters.

## Gotchas
- `domain_header.h` should NOT define its own `GDICOLOR` or `gdiFonts`. It should include `../util/gdioutput.h` instead.
- `TextInfo` uses `RECT`, which is defined as a simple struct in `gdioutput.h` to replace the Win32 `RECT`.
