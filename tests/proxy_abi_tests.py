"""Emulate each compiled proxy export with real x64 machine code.
Checks register/stack argument forwarding; does not emulate Windows DLL loading.
"""
from pathlib import Path
import struct
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from unicorn import Uc, UC_ARCH_X86, UC_MODE_64, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_RSP, UC_X86_REG_RIP, UC_X86_REG_RAX, UC_X86_REG_RCX, UC_X86_REG_RDX, UC_X86_REG_R8, UC_X86_REG_R9, UC_X86_REG_XMM0, UC_X86_REG_XMM1, UC_X86_REG_XMM2, UC_X86_REG_XMM3

pe=pefile.PE(str(Path('dist/version.dll')))
base=pe.OPTIONAL_HEADER.ImageBase
image=pe.get_memory_mapped_image()
exports=sorted((s.name.decode(),s.address) for s in pe.DIRECTORY_ENTRY_EXPORT.symbols if s.name)
expected='GetFileVersionInfoA GetFileVersionInfoByHandle GetFileVersionInfoExA GetFileVersionInfoExW GetFileVersionInfoSizeA GetFileVersionInfoSizeExA GetFileVersionInfoSizeExW GetFileVersionInfoSizeW GetFileVersionInfoW VerFindFileA VerFindFileW VerInstallFileA VerInstallFileW VerLanguageNameA VerLanguageNameW VerQueryValueA VerQueryValueW'.split()
assert [name for name,_ in exports]==expected
cs=Cs(CS_ARCH_X86,CS_MODE_64)
regs=[UC_X86_REG_RCX,UC_X86_REG_RDX,UC_X86_REG_R8,UC_X86_REG_R9]
xmm=[UC_X86_REG_XMM0,UC_X86_REG_XMM1,UC_X86_REG_XMM2,UC_X86_REG_XMM3]
for index,(name,rva) in enumerate(exports):
    entry=base+rva
    instructions=list(cs.disasm(pe.get_data(rva,160),entry))
    call=next(i for i in instructions if i.mnemonic=='call')
    resolver=int(call.op_str,16)
    uc=Uc(UC_ARCH_X86,UC_MODE_64)
    uc.mem_map(base,(len(image)+0xffff)&~0xffff);uc.mem_write(base,image)
    stackBase=0x60000000;uc.mem_map(stackBase,0x10000)
    rsp=stackBase+0x8008;stack=bytes((i*17+3)&255 for i in range(128))
    uc.mem_write(rsp,stack);uc.reg_write(UC_X86_REG_RSP,rsp)
    values=[0x1234567800000000+n for n in range(4)]
    floats=[0x112233445566778899aabbccddeeff00+n for n in range(4)]
    for reg,value in zip(regs,values):uc.reg_write(reg,value)
    for reg,value in zip(xmm,floats):uc.reg_write(reg,value)
    original=0x70000000;uc.mem_map(original,0x1000);uc.mem_write(original,b'\xcc')
    reached=[]
    def hook(machine,address,size,data):
        if address==resolver:
            assert machine.reg_read(UC_X86_REG_RSP)%16==8,'resolver stack alignment'
            assert machine.reg_read(UC_X86_REG_RCX)==index,'wrong API lookup index'
            sp=machine.reg_read(UC_X86_REG_RSP)
            resume=struct.unpack('<Q',machine.mem_read(sp,8))[0]
            # Model a resolver that clobbers every argument register and XMM0-3.
            for reg in regs+xmm:machine.reg_write(reg,0xdeadbeef)
            machine.reg_write(UC_X86_REG_RAX,original)
            machine.reg_write(UC_X86_REG_RSP,sp+8);machine.reg_write(UC_X86_REG_RIP,resume)
        elif address==original:
            assert machine.reg_read(UC_X86_REG_RSP)==rsp,'caller stack not restored'
            assert bytes(machine.mem_read(rsp,len(stack)))==stack,'return address or stack arguments corrupted'
            for reg,value in zip(regs,values):assert machine.reg_read(reg)==value,'integer/pointer argument corrupted'
            for reg,value in zip(xmm,floats):assert machine.reg_read(reg)==value,'XMM argument corrupted'
            reached.append(True);machine.emu_stop()
    uc.hook_add(UC_HOOK_CODE,hook)
    uc.emu_start(entry,original+1,count=200)
    assert reached,name
print('PASS: all 17 compiled version.dll exports preserve Windows x64 argument registers, XMM0-3, stack alignment and stack arguments')
