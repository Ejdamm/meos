import os
import re

def fix_file(path):
    with open(path, 'r', encoding='latin-1') as f:
        content = f.read()
    
    # Int/Bool fields that should NOT use (string) cast
    int_bool_fields = [
        'Removed', 'counter', 'Id', 'ZeroTime', 'inputPlace', 'InputPlace', 
        'TimeAdjustment', 'PointAdjustment', 'Status', 'type', 'cardNo', 'totalTime',
        'Leg', 'leg', 'Order', 'order', 'StartNo', 'Bib', 'CardNo', 'Time', 'time', 
        'Place', 'place', 'Result', 'result', 'Score', 'score', 'Factor', 'factor',
        'nLeg', 'nRunner', 'nStart', 'number', 'timeConstSecond', 'zeroTime', 'id',
        'origin', 'punchUnit', 'punchTime', 'autoAssignStartNo', 'matchCard', 
        'updateFromDatabase', 'updateStartNo', 'isOriginal', 'isAutoSave', 
        'internalFormat', 'doImport', 'forMerge', 'forceNew', 'toString', 'cutMode'
    ]
    
    new_content = content
    for field in int_bool_fields:
        # Match assignments to these fields from (string)row[...] or (string)res.at(0)[...]
        # ob->field = (string)row[...]
        new_content = re.sub(r'([a-zA-Z0-9_\->.]+)' + field + r'\s*=\s*\(string\)(row\["[^"]+"\]|res\.at\(0\)\["[^"]+"\]);', 
                             r'\1' + field + r' = (int)\2;', new_content)
        # Type field = (string)row[...]
        new_content = re.sub(r'([a-zA-Z0-9_]+)\s+' + field + r'\s*=\s*\(string\)(row\["[^"]+"\]|res\.at\(0\)\["[^"]+"\]);', 
                             r'\1 ' + field + r' = (int)\2;', new_content)

    if content != new_content:
        with open(path, 'w', encoding='latin-1') as f:
            f.write(new_content)
        return True
    return False

fix_file('code/MeosSQL.cpp')
