import os
import re

def fix_file(path):
    with open(path, 'r', encoding='latin-1') as f:
        content = f.read()
    
    # Matches snprintf(bf, "format", ...) where bf is the first arg.
    # We want to insert sizeof(bf) as second arg.
    # Regex: snprintf\(([^,]+),\s*("[^"]*")
    new_content = re.sub(r'snprintf\(([^,]+),\s*("[^"]*")', r'snprintf(\1, sizeof(\1), \2', content)
    
    if content != new_content:
        with open(path, 'w', encoding='latin-1') as f:
            f.write(new_content)
        return True
    return False

for root, dirs, files in os.walk('code'):
    for file in files:
        if file.endswith(('.cpp', '.h')):
            if fix_file(os.path.join(root, file)):
                print(f"Fixed {file}")
