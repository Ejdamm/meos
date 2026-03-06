# Util Module (`src/util/`)

Foundation utilities, string conversions, and platform shims for MeOS.

## Localization (`localizer.h/cpp`)

The `Localizer` handles internationalization (i18n).

- **Data format:** Uses `.lng` files (plain text `Key = Value` format).
- **Paths:** Searches for `.lng` files in `resources/lang/`, `lang/`, and `../src/app/lang/`.
- **Substitution:**
  - Uses `#` as a delimiter in the source string.
  - Substitutes `X`, `Y`, `Z`, `W` in the translated string with parts separated by `#`.
  - Example: `L"Lista av typ 'X'#Resultat"` -> `X` in translated "Lista av typ 'X'" is replaced by "Resultat".
- **Suffix preservation:** Automatically preserves trailing punctuation ( `:`, `.`, ` `, `,`, etc.) from the source string.

## Word Lists (`owordlist.h/cpp`)

Generic word set and database classes (`oWordList`, `oWordDB`, `oWordIndexHash`).

- **Case-insensitivity:** `oWordList` converts strings to lowercase using `towlower` before storage/lookup.
- **Serialization:** Custom binary format (`WWDB` header). Uses `std::ifstream`/`std::ofstream` in binary mode.
- **Dependency:** `Localizer` uses `oWordList` for name databases (e.g., `wgiven.mwd`).

## Conventions

- **Encoding:** All file I/O for text files should use UTF-8. `toUTF8`/`fromUTF8` and `widen`/`narrow` are available in `meos_util.h`.
- **Wide Characters:** Use `L'character'` literals (e.g., `L'×'`, `L'–'`) for comparison with `wchar_t` or `wstring` elements to avoid multi-character constant warnings.
- **File I/O:** Prefer `std::ifstream`/`std::ofstream` over low-level `_read`/`_write` or `_wsopen_s`.
