default rel
section .text
global ft_write
extern __errno_location

ft_write:
    mov rax, 1
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
