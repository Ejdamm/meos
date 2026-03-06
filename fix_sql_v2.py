import os
import re

def fix_file(path):
    with open(path, 'r', encoding='latin-1') as f:
        content = f.read()
    
    # Correcting counter, Id, Removed assignments to use (int) instead of (string)
    new_content = re.sub(r'int\s+id\s*=\s*\(string\)([^;]+);', r'int id = (int)\1;', content)
    new_content = re.sub(r'bool\s+removed\s*=\s*\(string\)([^;]+);', r'bool removed = (int)\1;', new_content)
    new_content = re.sub(r'int\s+c\s*=\s*\(string\)([^;]+);', r'int c = (int)\1;', new_content)
    
    if content != new_content:
        with open(path, 'w', encoding='latin-1') as f:
            f.write(new_content)
        return True
    return False

fix_file('code/MeosSQL.cpp')
