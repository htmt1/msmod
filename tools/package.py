from pathlib import Path
import hashlib
import zipfile
root=Path(__file__).resolve().parents[1]
# Runtime package intentionally contains no executable and no fake BepInEx assemblies.
runtime={
    'version.dll':root/'dist/version.dll',
    'MoonWallet/plugins/MoonWallet.dll':root/'dist/MoonWallet.dll',
}
docs=['README.md','THIRD_PARTY_NOTICES.md','docs/TESTING.ja.md','docs/IMPLEMENTATION.md','third_party/minhook/LICENSE.txt']
output=root/'dist/MoonWallet-v0.3.2-ingame-ui-experimental.zip'
with zipfile.ZipFile(output,'w',zipfile.ZIP_DEFLATED) as z:
    for arc,path in runtime.items():z.write(path,arc)
    for path in docs:z.write(root/path,path)
    source_files=[root/'README.md',root/'THIRD_PARTY_NOTICES.md',root/'requirements-dev.txt',root/'.gitignore']
    for folder in ['src','tests','tools','docs','third_party']:
        source_files.extend(p for p in (root/folder).rglob('*') if p.is_file() and '__pycache__' not in p.parts)
    for p in sorted(source_files):z.write(p,'Source/'+str(p.relative_to(root)))
    manifest='\n'.join(hashlib.sha256(p.read_bytes()).hexdigest()+'  '+arc for arc,p in runtime.items())+'\n'
    z.writestr('SHA256SUMS.txt',manifest)
with zipfile.ZipFile(output) as z:
    assert z.testzip() is None
    names=z.namelist()
    assert not any(n.lower().endswith('.exe') for n in names)
    assert not any(n.endswith(('GameAssembly.dll','global-metadata.dat')) for n in names)
    for arc,p in runtime.items():assert z.read(arc)==p.read_bytes()
    for line in z.read('SHA256SUMS.txt').decode().splitlines():
        expected,name=line.split('  ');assert hashlib.sha256(z.read(name)).hexdigest()==expected
print(output)
print(f'{output.stat().st_size:,} bytes; package CRC, SHA-256, runtime paths, no EXEs, no proprietary game binaries: PASS')
