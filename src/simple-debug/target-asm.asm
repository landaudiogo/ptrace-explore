section    .text
    ; The _start symbol must be declared for the linker (ld)
    global _start

_start:
    ; syscall write
    mov    rdi, 1     ; stdout fd
    mov    rsi, msg
    mov    rdx, len   ; msg length
    mov    rax, 1     ; write syscall
    syscall

    ; syscall exit
    xor    rdi, rdi
    mov    rax, 60
    syscall

section   .data
msg db    'Hello, World!', 0xa
len equ    $ - msg
