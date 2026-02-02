section .text
global ft_strdup
extern malloc
extern ft_strlen
extern ft_strcpy

ft_strdup:
    push rdi
    call ft_strlen
    inc rax
    mov rdi, rax
    call malloc wrt ..plt        ; rax -> adresse de l'endroit malloc
    cmp rax, 0
    je .error
    mov rdi, rax
    pop rsi
    call ft_strcpy
    ret
    
.error:
    pop rdi
    xor rax, rax        ; = mov rax, 0 mais coute 3oct au lieu de 7pour mov
    ret

section .note.GNU-stack noalloc noexec nowrite progbits