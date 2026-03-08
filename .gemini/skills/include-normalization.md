# Include Normalization Skill

This skill provides a procedure and scripts for normalizing `#include` directives to match file casing on disk, which is essential for cross-platform builds (e.g., Windows to Linux).

## Procedure

1.  **Identify Mismatches**: Use a script to compare `#include "..."` paths against the actual files on disk.
2.  **Apply Fixes**: Update source files to use the correct casing.
3.  **Verify**: Re-run the identification script to ensure no mismatches remain.

## Scripts

### `fix_casing.py`

This script recursively scans the `code/` directory, builds a casing database, and updates includes in `.h` and `.cpp` files.

```python
import os
import re

# Get all files in code/ recursively and build a map of lowercase to actual
code_dir = 'code'
all_files = {}

for root, dirs, files in os.walk(code_dir):
    for f in files:
        rel_dir = os.path.relpath(root, code_dir)
        if rel_dir == '.':
            path = f
        else:
            path = os.path.join(rel_dir, f)
        
        path = path.replace('\\', '/')
        all_files[path.lower()] = path

# Now iterate through each file and check includes
for root, dirs, files in os.walk(code_dir):
    for f in files:
        if f.endswith(('.cpp', '.h', '.hpp')):
            file_path = os.path.join(root, f)
            
            with open(file_path, 'r', encoding='latin-1') as fr:
                lines = fr.readlines()
            
            changed = False
            new_lines = []
            for line in lines:
                match = re.match(r'(^#include\s+")([^"]+)(")', line)
                if match:
                    prefix, included_file, suffix = match.groups()
                    included_file_normalized = included_file.replace('\\', '/')
                    
                    file_dir = os.path.relpath(root, code_dir)
                    search_path = included_file_normalized.lower()
                    if file_dir != '.':
                        search_path = os.path.join(file_dir, included_file_normalized).lower()
                    
                    direct_search_path = included_file_normalized.lower()
                    
                    actual_path = None
                    if search_path in all_files:
                        actual_path = all_files[search_path]
                        if file_dir != '.':
                            actual_path = os.path.relpath(os.path.join(code_dir, actual_path), root)
                    elif direct_search_path in all_files:
                        actual_path = all_files[direct_search_path]
                    
                    if actual_path:
                        actual_path = actual_path.replace('\\', '/')
                        if actual_path != included_file:
                            new_lines.append(f"{prefix}{actual_path}{suffix}\n")
                            changed = True
                        else:
                            new_lines.append(line)
                    else:
                        new_lines.append(line)
                else:
                    new_lines.append(line)
            
            if changed:
                with open(file_path, 'w', encoding='latin-1') as fw:
                    fw.writelines(new_lines)
```

## Common Mismatches in MeOS

- `stdafx.h` -> `StdAfx.h`
- `localizer.h` -> `localizer.h` (included as `Localizer.h`)
- `meosexception.h` -> `meosexception.h` (included as `meosException.h`)
- `TabBase.h` -> `TabBase.h` (included as `tabbase.h`)
