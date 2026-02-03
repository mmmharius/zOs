section .text
global ft_read
extern __errno_location

ft_read:
    xor rax, rax
    syscall
    cmp rax, 0
    jl .error
    ret

.error:
    neg rax
    push rax
    call __errno_location wrt ..plt
    pop rdx
    mov [rax], rdx
    mov rax, -1
    ret

section .note.GNU-stack noalloc noexec nowrite progbits