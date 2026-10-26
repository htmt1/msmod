.text
.globl wallet_proxy_GetFileVersionInfoA
.def wallet_proxy_GetFileVersionInfoA; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_GetFileVersionInfoA
wallet_proxy_GetFileVersionInfoA:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $0, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_GetFileVersionInfoByHandle
.def wallet_proxy_GetFileVersionInfoByHandle; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_GetFileVersionInfoByHandle
wallet_proxy_GetFileVersionInfoByHandle:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $1, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_GetFileVersionInfoExA
.def wallet_proxy_GetFileVersionInfoExA; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_GetFileVersionInfoExA
wallet_proxy_GetFileVersionInfoExA:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $2, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_GetFileVersionInfoExW
.def wallet_proxy_GetFileVersionInfoExW; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_GetFileVersionInfoExW
wallet_proxy_GetFileVersionInfoExW:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $3, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_GetFileVersionInfoSizeA
.def wallet_proxy_GetFileVersionInfoSizeA; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_GetFileVersionInfoSizeA
wallet_proxy_GetFileVersionInfoSizeA:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $4, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_GetFileVersionInfoSizeExA
.def wallet_proxy_GetFileVersionInfoSizeExA; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_GetFileVersionInfoSizeExA
wallet_proxy_GetFileVersionInfoSizeExA:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $5, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_GetFileVersionInfoSizeExW
.def wallet_proxy_GetFileVersionInfoSizeExW; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_GetFileVersionInfoSizeExW
wallet_proxy_GetFileVersionInfoSizeExW:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $6, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_GetFileVersionInfoSizeW
.def wallet_proxy_GetFileVersionInfoSizeW; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_GetFileVersionInfoSizeW
wallet_proxy_GetFileVersionInfoSizeW:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $7, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_GetFileVersionInfoW
.def wallet_proxy_GetFileVersionInfoW; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_GetFileVersionInfoW
wallet_proxy_GetFileVersionInfoW:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $8, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_VerFindFileA
.def wallet_proxy_VerFindFileA; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_VerFindFileA
wallet_proxy_VerFindFileA:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $9, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_VerFindFileW
.def wallet_proxy_VerFindFileW; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_VerFindFileW
wallet_proxy_VerFindFileW:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $10, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_VerInstallFileA
.def wallet_proxy_VerInstallFileA; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_VerInstallFileA
wallet_proxy_VerInstallFileA:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $11, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_VerInstallFileW
.def wallet_proxy_VerInstallFileW; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_VerInstallFileW
wallet_proxy_VerInstallFileW:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $12, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_VerLanguageNameA
.def wallet_proxy_VerLanguageNameA; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_VerLanguageNameA
wallet_proxy_VerLanguageNameA:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $13, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_VerLanguageNameW
.def wallet_proxy_VerLanguageNameW; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_VerLanguageNameW
wallet_proxy_VerLanguageNameW:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $14, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_VerQueryValueA
.def wallet_proxy_VerQueryValueA; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_VerQueryValueA
wallet_proxy_VerQueryValueA:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $15, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
.globl wallet_proxy_VerQueryValueW
.def wallet_proxy_VerQueryValueW; .scl 2; .type 32; .endef
.seh_proc wallet_proxy_VerQueryValueW
wallet_proxy_VerQueryValueW:
subq $136, %rsp
.seh_stackalloc 136
.seh_endprologue
movq %rcx, 32(%rsp)
movq %rdx, 40(%rsp)
movq %r8, 48(%rsp)
movq %r9, 56(%rsp)
movdqu %xmm0, 64(%rsp)
movdqu %xmm1, 80(%rsp)
movdqu %xmm2, 96(%rsp)
movdqu %xmm3, 112(%rsp)
movl $16, %ecx
call wallet_resolve_version
movq %rax, %r11
movq 32(%rsp), %rcx
movq 40(%rsp), %rdx
movq 48(%rsp), %r8
movq 56(%rsp), %r9
movdqu 64(%rsp), %xmm0
movdqu 80(%rsp), %xmm1
movdqu 96(%rsp), %xmm2
movdqu 112(%rsp), %xmm3
addq $136, %rsp
jmp *%r11
.seh_endproc
