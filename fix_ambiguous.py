import os
import re

def fix_file(path):
    with open(path, 'r', encoding='latin-1') as f:
        content = f.read()
    
    # Matches var = row["field"]; where var is likely string
    # Replace with var = (string)row["field"];
    # Also handles res.at(0)["field"]
    
    # Targeting common patterns in MeosSQL.cpp
    new_content = re.sub(r'([a-zA-Z0-9_\->]+)\s*=\s*(res\.at\(0\)\["[^"]+"\]|row\["[^"]+"\]);', r'\1 = (string)\2;', content)
    
    if content != new_content:
        with open(path, 'w', encoding='latin-1') as f:
            f.write(new_content)
        return True
    return False

fix_file('code/MeosSQL.cpp')
