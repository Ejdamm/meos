# Win32 Type Migration Skill

## Purpose
Automate the replacement of Win32-specific type aliases with standard C++ types in domain code.

## Key Types
- `DWORD` -> `uint32_t` (requires `#include <cstdint>`)
- `BOOL` -> `bool`
- `TRUE` -> `true`
- `FALSE` -> `false`

## Contextual Replacements
- Only replace in domain/utility files (oEvent*, oRunner*, meos_util*, etc.).
- Skip UI files (Tab*, gdioutput*, Table*).
- **CRITICAL:** Skip `BOOL` inside SQL query strings (e.g., in `MeosSQL.cpp`).

## Reusable Script Pattern
```python
import re
import os

def migrate_types(content, is_sql_file=False):
    # Standard DWORD replacement
    content = re.sub(r'\bDWORD\b', 'uint32_t', content)
    
    if not is_sql_file:
        content = re.sub(r'\bBOOL\b', 'bool', content)
        content = re.sub(r'\bTRUE\b', 'true', content)
        content = re.sub(r'\bFALSE\b', 'false', content)
    else:
        # Specialized SQL-safe replacement (avoiding typical SQL column definitions)
        # In MeosSQL.cpp, BOOL often appears in CREATE TABLE strings
        pass

    # Ensure cstdint header if uint32_t is added
    if 'uint32_t' in content and '<cstdint>' not in content:
        if '#pragma once' in content:
            content = content.replace('#pragma once', '#pragma once\n\n#include <cstdint>', 1)
        # ... more insertion logic ...
    
    return content
```
