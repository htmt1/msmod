"""Validate the produced native Windows binaries; does not claim in-game execution."""
from pathlib import Path
import pefile
for name,isdll in [("MoonWallet.dll",True),("version.dll",True)]:
    path=Path('dist')/name
    pe=pefile.PE(str(path))
    assert pe.FILE_HEADER.Machine==0x8664,(name,'not AMD64')
    assert bool(pe.FILE_HEADER.Characteristics&0x2000)==isdll,(name,'incorrect file kind')
    assert pe.OPTIONAL_HEADER.Subsystem==2,(name,'not Windows GUI')
    assert 0<pe.OPTIONAL_HEADER.AddressOfEntryPoint<pe.OPTIONAL_HEADER.SizeOfImage
    imports={d.dll.decode().lower() for d in pe.DIRECTORY_ENTRY_IMPORT}
    assert all(n in {'user32.dll','gdi32.dll','bcrypt.dll','kernel32.dll','ntdll.dll','ucrtbase.dll'} or n.startswith('api-ms-win-crt-') for n in imports),(name,imports)
    assert b'GameAssembly.dll' not in {d.dll for d in pe.DIRECTORY_ENTRY_IMPORT},'must bind game API at runtime'
    print(f'PASS: {name}: Windows x64, GUI, native entrypoint, system-only imports ({path.stat().st_size:,} bytes)')

proxy=pefile.PE('dist/version.dll')
exports={s.name.decode() for s in proxy.DIRECTORY_ENTRY_EXPORT.symbols if s.name}
assert len(exports)==17
assert {'GetFileVersionInfoW','GetFileVersionInfoSizeW','VerQueryValueW'}<=exports
assert all(d.dll.decode().lower()!='version.dll' for d in proxy.DIRECTORY_ENTRY_IMPORT),'proxy must not import itself'
print('PASS: version.dll exposes 17 named APIs and has no recursive version.dll import')

plugin=pefile.PE('dist/MoonWallet.dll')
imports={item.name.decode() for descriptor in plugin.DIRECTORY_ENTRY_IMPORT for item in descriptor.imports if item.name}
forbidden={'CreateWindowExW','RegisterClassW','MessageBoxW','GetAsyncKeyState','GetMessageW','DispatchMessageW','SetWindowTextW','SendMessageW'}
assert not (imports&forbidden),('unexpected external UI dependency',imports&forbidden)
print('PASS: in-game plugin has no OS-window/dialog/message-loop or global keyboard-polling imports')
