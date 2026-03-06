import os
import re

def fix_file(path):
    with open(path, 'r', encoding='latin-1') as f:
        content = f.read()
    
    # 1. Matches .open(var.c_str(), ...)
    new_content = re.sub(r'\.open\(([^,)]+)\.c_str\(\)', r'.open(string(\1.begin(), \1.end()).c_str()', content)
    # 2. Matches ifstream var(file.c_str(), ...)
    new_content = re.sub(r'ifstream\s+([^(\s]+)\(([^,)]+)\.c_str\(\)', r'ifstream \1(string(\2.begin(), \2.end()).c_str()', new_content)
    # 3. Matches ofstream var(file.c_str(), ...)
    new_content = re.sub(r'ofstream\s+([^(\s]+)\(([^,)]+)\.c_str\(\)', r'ofstream \1(string(\2.begin(), \2.end()).c_str()', new_content)

    # 4. Matches .open(var) where var is not a string literal and not already fixed
    # This is tricky because var could be a char*. 
    # But in this codebase, most filenames are wstring.
    # We'll target specifically known wstring filename variables or common patterns.
    
    # Files like xmlparser.cpp:528: foutFile.open(file);
    new_content = re.sub(r'foutFile\.open\(file\)', r'foutFile.open(string(file.begin(), file.end()).c_str())', new_content)
    new_content = re.sub(r'fin\.open\(file\)', r'fin.open(string(file.begin(), file.end()).c_str())', new_content)
    new_content = re.sub(r'ifstream\s+fin\(file\)', r'ifstream fin(string(file.begin(), file.end()).c_str())', new_content)
    new_content = re.sub(r'ifstream\s+file\(fn\)', r'ifstream file(string(fn.begin(), fn.end()).c_str())', new_content)
    new_content = re.sub(r'ifstream\s+file\(fileName\)', r'ifstream file(string(fileName.begin(), fileName.end()).c_str())', new_content)
    new_content = re.sub(r'ifstream\s+fin\(fn\)', r'ifstream fin(string(fn.begin(), fn.end()).c_str())', new_content)
    new_content = re.sub(r'fin\.open\(fn,', r'fin.open(string(fn.begin(), fn.end()).c_str(),', new_content)
    new_content = re.sub(r'fin\.open\(filename\)', r'fin.open(string(filename.begin(), filename.end()).c_str())', new_content)
    new_content = re.sub(r'ifstream\s+fin\(filename\)', r'ifstream fin(string(filename.begin(), filename.end()).c_str())', new_content)

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
