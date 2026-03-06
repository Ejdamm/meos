import os
import re

def fix_file(path):
    with open(path, 'r', encoding='latin-1') as f:
        content = f.read()
    
    # Matches any int or bool assignment from (string)row["..."]
    # Variable can be r.counter, t.Removed, ob->Id, etc.
    # Pattern: [a-zA-Z0-9_\->.]+ (Removed|counter|Id|ZeroTime|inputPlace|InputPlace|TimeAdjustment|PointAdjustment|...)
    
    int_bool_fields = ['Removed', 'counter', 'Id', 'ZeroTime', 'inputPlace', 'InputPlace', 'TimeAdjustment', 'PointAdjustment', 'Status', 'type', 'cardNo', 'totalTime']
    
    new_content = content
    for field in int_bool_fields:
        # Match assignments to these fields from (string)row[...] or (string)res.at(0)[...]
        new_content = re.sub(r'([a-zA-Z0-9_\->.]+)\.' + field + r'\s*=\s*\(string\)(row\["[^"]+"\]|res\.at\(0\)\["[^"]+"\]);', 
                             r'\1.' + field + r' = (int)\2;', new_content)
        new_content = re.sub(r'([a-zA-Z0-9_\->.]+)\->' + field + r'\s*=\s*\(string\)(row\["[^"]+"\]|res\.at\(0\)\["[^"]+"\]);', 
                             r'\1->' + field + r' = (int)\2;', new_content)
        # Also local variables
        new_content = re.sub(r'(int|bool)\s+' + field + r'\s*=\s*\(string\)(row\["[^"]+"\]|res\.at\(0\)\["[^"]+"\]);', 
                             r'\1 ' + field + r' = (int)\2;', new_content)

    if content != new_content:
        with open(path, 'w', encoding='latin-1') as f:
            f.write(new_content)
        return True
    return False

fix_file('code/MeosSQL.cpp')
