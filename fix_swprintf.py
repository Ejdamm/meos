import os
import re

def fix_file(path):
    with open(path, 'r', encoding='latin-1') as f:
        content = f.read()
    
    # Matches swprintf(bf, L"format", ...) where bf is the first arg.
    # We want to insert sizeof(bf)/sizeof(wchar_t) as second arg.
    # Regex: swprintf\(([^,]+),\s*(L"[^"]*")
    # This only handles cases where second arg is a literal.
    new_content = re.sub(r'swprintf\(([^,]+),\s*(L"[^"]*")', r'swprintf(\1, sizeof(\1)/sizeof(wchar_t), \2', content)
    
    # Also handle cases where second arg is a variable (like 'pattern' in oTeam.cpp)
    # but ONLY if it's NOT already sizeof or a number.
    # Matches swprintf(var1, var2, ...
    # This is risky, so let's be more specific for oTeam.cpp pattern.
    new_content = re.sub(r'swprintf\((bf|ch|bf2|num|sname),\s*(pattern|fmt|lang.tl\([^)]+\).c_str\(\)|it->second.c_str\(\))', 
                         r'swprintf(\1, sizeof(\1)/sizeof(wchar_t), \2', new_content)

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
