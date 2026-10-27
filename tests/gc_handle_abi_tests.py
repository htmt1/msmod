"""Run the compiled Windows handle calls against pointer-backed runtime shims.

A 32-bit return, parameter, or widget key truncates the handle and fails the test.
This checks native ABI/data flow, not actual Unity or Windows execution.
"""
import struct
import sys
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_64, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_RSP, UC_X86_REG_RIP, UC_X86_REG_RAX, UC_X86_REG_RCX, UC_X86_REG_RDX

pe = pefile.PE(sys.argv[1] if len(sys.argv) > 1 else 'build/gc_handle_abi_fixture.dll')
image = pe.get_memory_mapped_image()
base = pe.OPTIONAL_HEADER.ImageBase
entry = base + next(e.address for e in pe.DIRECTORY_ENTRY_EXPORT.symbols if e.name == b'cycle_widget')
uc = Uc(UC_ARCH_X86, UC_MODE_64)
uc.mem_map(base, (len(image) + 0xffff) & ~0xffff)
uc.mem_write(base, image)
stack_base, table, runtime, stop = 0x60000000, 0x65000000, 0x70000000, 0x71000000
for address, size in [(stack_base, 0x10000), (table, 0x1000), (runtime, 0x1000), (stop, 0x1000)]:
    uc.mem_map(address, size)
new, get, free = runtime, runtime + 0x100, runtime + 0x200
uc.mem_write(table, struct.pack('<QQQ', new, get, free))
# A tagged 64-bit pointer handle, representative of this game's runtime.
slot, handle, object_address = 0x12345678028, 0x1234567802a, 0x23456789000
uc.mem_map(slot & ~0xfff, 0x1000)
uc.mem_write(slot, struct.pack('<Q', object_address))
uc.mem_write(new, b'\x48\xb8' + struct.pack('<Q', handle) + b'\xc3')
# mov rax,rcx; and rax,-4; mov rax,[rax]; ret
uc.mem_write(get, bytes.fromhex('48 89 c8 48 83 e0 fc 48 8b 00 c3'))
uc.mem_write(free, b'\xc3')
uc.mem_write(stop, b'\xcc')
sp = stack_base + 0x8008
uc.mem_write(sp, struct.pack('<Q', stop))
uc.reg_write(UC_X86_REG_RSP, sp)
uc.reg_write(UC_X86_REG_RCX, table)
uc.reg_write(UC_X86_REG_RDX, object_address)
events = []

def observe(machine, address, size, data):
    if address == new:
        assert machine.reg_read(UC_X86_REG_RCX) == object_address
        assert machine.reg_read(UC_X86_REG_RDX) & 255 == 0
        events.append('retain')
    elif address in (get, free):
        assert machine.reg_read(UC_X86_REG_RCX) == handle, 'truncated GC handle'
        events.append('target' if address == get else 'release')
    elif address == stop:
        assert machine.reg_read(UC_X86_REG_RAX) == object_address
        assert machine.reg_read(UC_X86_REG_RSP) == sp + 8
        machine.emu_stop()

uc.hook_add(UC_HOOK_CODE, observe)
uc.emu_start(entry, stop + 1, count=1000)
assert events == ['retain', 'target', 'release']
assert uc.reg_read(UC_X86_REG_RIP) == stop
print('PASS: compiled Windows x64 UI handle retain/lookup/release preserves all 64 bits of a pointer-backed handle')
