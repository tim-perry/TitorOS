ENTRY(kernel)

PHDRS {
    kernel PT_LOAD;
}

SECTIONS {
    . = 0xffffffff80000000;

    .text : {
        *(.text.kernel)
        *(.text .text.*)
    } :kernel

    .data : {
        *(.data .data.*)
        *(.rodata .rodata.*)
    } :kernel

    .bss : {
        *(COMMON)
        *(.bss .bss.*)
    } :kernel

    /DISCARD/ : {
        *(.eh_frame)
        *(.note .note.*)
        *(.comment .comment.*)
    }
}

OUTPUT_ARCH(i386:x86-64)
OUTPUT_FORMAT(elf64-x86-64)