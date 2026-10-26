"""Check required UI APIs against a real LibCpp2IL dump; optional private analysis input."""
import json,sys
from pathlib import Path
contract=json.loads(Path('src/ui_contract.json').read_text())
classes={t['name']:t for t in json.load(open(sys.argv[1]))}
def arity(signature):
    text=signature[signature.index('(')+1:signature.rindex(')')]
    if not text:return 0
    depth=0;count=1
    for c in text:
        if c=='<':depth+=1
        if c=='>':depth-=1
        if c==',' and depth==0:count+=1
    return count
def find(full,name,count):
    while full:
        t=classes.get(full)
        if not t:return []
        result=[m for m in t['methods'] if m['name']==name and arity(m['signature'])==count]
        if result:return result
        full=t.get('parent')
    return []
total=0
for full,methods in contract['methods'].items():
    assert full in classes,('missing class',full)
    for method,count in methods.items():
        matches=find(full,method,count)
        assert len(matches)==1,('missing/ambiguous method',full,method,count,[m['signature'] for m in matches])
        total+=1
for full,fields in contract['fields'].items():
    for field in fields:assert any(f['name']==field for f in classes[full]['fields']),(full,field)
print(f'PASS: {total} UI methods, {sum(map(len,contract["fields"].values()))} fields and {len(contract["methods"])} classes match supplied game metadata (including inherited methods)')

if len(sys.argv)>2:
    import pefile,re
    pe=pefile.PE(sys.argv[2],fast_load=True)
    pe.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY['IMAGE_DIRECTORY_ENTRY_EXPORT']])
    exports={x.name.decode() for x in pe.DIRECTORY_ENTRY_EXPORT.symbols if x.name}
    required=set(re.findall(r'bind\(module,[^,]+,"(il2cpp_[^"]+)"\)',Path('src/il2cpp_ui.hpp').read_text()))
    assert required and required<=exports,('missing native IL2CPP UI exports',required-exports)
    print(f'PASS: all {len(required)} IL2CPP bridge exports exist in supplied native game DLL')
