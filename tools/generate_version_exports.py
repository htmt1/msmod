"""Windows x64 tail forwarding: preserve caller arguments without guessing API signatures."""
from pathlib import Path
names='GetFileVersionInfoA GetFileVersionInfoByHandle GetFileVersionInfoExA GetFileVersionInfoExW GetFileVersionInfoSizeA GetFileVersionInfoSizeExA GetFileVersionInfoSizeExW GetFileVersionInfoSizeW GetFileVersionInfoW VerFindFileA VerFindFileW VerInstallFileA VerInstallFileW VerLanguageNameA VerLanguageNameW VerQueryValueA VerQueryValueW'.split()
root=Path(__file__).resolve().parents[1]
# Initial RSP is 8 mod 16; 136 bytes gives 32-byte shadow space + argument saves + 16-byte alignment.
asm=['.text']
for index,name in enumerate(names):
    label='wallet_proxy_'+name
    asm += [f'.globl {label}',f'.def {label}; .scl 2; .type 32; .endef',f'.seh_proc {label}',label+':',
            'subq $136, %rsp','.seh_stackalloc 136','.seh_endprologue',
            'movq %rcx, 32(%rsp)','movq %rdx, 40(%rsp)','movq %r8, 48(%rsp)','movq %r9, 56(%rsp)',
            'movdqu %xmm0, 64(%rsp)','movdqu %xmm1, 80(%rsp)','movdqu %xmm2, 96(%rsp)','movdqu %xmm3, 112(%rsp)',
            f'movl ${index}, %ecx','call wallet_resolve_version','movq %rax, %r11',
            'movq 32(%rsp), %rcx','movq 40(%rsp), %rdx','movq 48(%rsp), %r8','movq 56(%rsp), %r9',
            'movdqu 64(%rsp), %xmm0','movdqu 80(%rsp), %xmm1','movdqu 96(%rsp), %xmm2','movdqu 112(%rsp), %xmm3',
            'addq $136, %rsp','jmp *%r11','.seh_endproc']
(root/'src/version_exports.S').write_text('\n'.join(asm)+'\n')
(root/'src/version_exports.def').write_text('LIBRARY version\nEXPORTS\n'+''.join(f'  {n}=wallet_proxy_{n}\n' for n in names))
