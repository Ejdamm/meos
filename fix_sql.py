import os
import re

def fix_file(path):
    with open(path, 'r', encoding='latin-1') as f:
        content = f.read()
    
    # Correcting counter assignments to use (int)
    new_content = re.sub(r'([a-zA-Z0-9_\->]+counter)\s*=\s*\(string\)([^;]+);', r'\1 = (int)\2;', content)
    new_content = re.sub(r'([a-zA-Z0-9_\->]+ZeroTime)\s*=\s*\(string\)([^;]+);', r'\1 = (int)\2;', new_content)
    
    # Also handle the cases where they were NOT yet fixed by my previous broken script
    new_content = re.sub(r'([a-zA-Z0-9_\->]+counter)\s*=\s*(row\["[^"]+"\]|res\.at\(0\)\["[^"]+"\]);', r'\1 = (int)\2;', new_content)
    new_content = re.sub(r'([a-zA-Z0-9_\->]+ZeroTime)\s*=\s*(row\["[^"]+"\]|res\.at\(0\)\["[^"]+"\]);', r'\1 = (int)\2;', new_content)
    
    if content != new_content:
        with open(path, 'w', encoding='latin-1') as f:
            f.write(new_content)
        return True
    return False

fix_file('code/MeosSQL.cpp')
