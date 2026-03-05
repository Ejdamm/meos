import os
import re

# Get all files in the repository
all_files = []
for root, dirs, files in os.walk('.'):
    # Ignore hidden directories like .git
    if '.git' in dirs:
        dirs.remove('.git')
    for f in files:
        all_files.append(os.path.join(root, f))

# Create a map for case-insensitive lookup, storing the actual path
file_map = {os.path.basename(f).lower(): os.path.basename(f) for f in all_files}

# Also need a map for full paths if includes use subdirectories
full_path_map = {f.replace('\\', '/').lower().lstrip('./'): f.replace('\\', '/').lstrip('./') for f in all_files}

include_pattern = re.compile(r'#include\s*"([^"]+)"')

mismatches = []

for filepath in all_files:
    if not (filepath.endswith('.h') or filepath.endswith('.cpp') or filepath.endswith('.hpp') or filepath.endswith('.rc')):
        continue

    try:
        with open(filepath, 'r', encoding='latin-1') as f:
            for i, line in enumerate(f, 1):
                match = include_pattern.search(line)
                if match:
                    included_file_original = match.group(1)
                    
                    included_file_normalized = included_file_original.replace('\\', '/')
                    
                    # Case 1: Basename only
                    included_basename = os.path.basename(included_file_normalized)
                    if included_basename.lower() in file_map:
                        actual_basename = file_map[included_basename.lower()]
                        if included_basename != actual_basename:
                            mismatches.append({
                                'file': filepath,
                                'line': i,
                                'included': included_file_original,
                                'actual': actual_basename
                            })
                    
                    # Case 2: Relative or full path within the project
                    elif included_file_normalized.lower() in full_path_map:
                        actual_path = full_path_map[included_file_normalized.lower()]
                        if included_file_normalized != actual_path:
                             mismatches.append({
                                'file': filepath,
                                'line': i,
                                'included': included_file_original,
                                'actual': actual_path
                            })
    except Exception as e:
        print(f"Error reading {filepath}: {e}")

if mismatches:
    print(f"Found {len(mismatches)} mismatches:")
    for m in mismatches:
        print(f"{m['file']}:{m['line']}: {m['included']} -> {m['actual']}")
else:
    print("No mismatches found.")
