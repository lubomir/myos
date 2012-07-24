[GLOBAL gdt_flush]      ; Allows the C code to call gdt_flush()

gdt_flush:
    mov eax, [esp+4]    ; Get the pointer to the GDT, passed as parameter.
    lgdt [eax]          ; Load the new GDT pointer

    mov ax, 0x10        ; 0x10 is the offset in the GDT to our data segment.
    mov ds, ax          ; Load all data segment selectors
    mov es, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush     ; 0x08 is the offset to our code segment. Far jump!
.flush:
    ret

[GLOBAL idt_flush]      ; Allows the C code to call idt_flush()

idt_flush:
    mov eax, [esp+4]    ; Get the pointer to the IDT, passed as parameter
    lidt [eax]          ; Load the IDT pointer
    ret

[GLOBAL tss_flush]      ; Allows C code to call tss_flush().
tss_flush:
    mov ax, 0x2B        ; Load the index of our TSS structure - the index
                        ; is 0x2B, as it is the 5th selector and each is
                        ; 8 bytes long, but we set the bottom two bits so
                        ; that it has an RPL of 3, not zero.
    ltr ax              ; Load the 0x2B into the task state register.
    ret
