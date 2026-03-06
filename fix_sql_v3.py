import os
import re

def fix_file(path):
    with open(path, 'r', encoding='latin-1') as f:
        content = f.read()
    
    # Matches any int or bool assignment from (string)row["..."]
    # Regex: (int|bool)\s+([a-zA-Z0-9_]+)\s*=\s*\(string\)(row\["[^"]+"\]|res\.at\(0\)\["[^"]+"\]);
    new_content = re.sub(r'(int|bool)\s+([a-zA-Z0-9_]+)\s*=\s*\(string\)(row\["[^"]+"\]|res\.at\(0\)\["[^"]+"\]);', 
                         r'\1 \2 = (int)\3;', content)
    
    # Also handle ob->counter = (string)...
    new_content = re.sub(r'([a-zA-Z0-9_\->]+counter)\s*=\s*\(string\)(row\["[^"]+"\]|res\.at\(0\)\["[^"]+"\]);', 
                         r'\1 = (int)\2;', new_content)
    new_content = re.sub(r'([a-zA-Z0-9_\->]+ZeroTime)\s*=\s*\(string\)(row\["[^"]+"\]|res\.at\(0\)\["[^"]+"\]);', 
                         r'\1 = (int)\2;', new_content)

    if content != new_content:
        with open(path, 'w', encoding='latin-1') as f:
            f.write(new_content)
        return True
    return False

fix_file('code/MeosSQL.cpp')
