ENTRY(_start)

SECTIONS
{
    . = 0xC0200000;

    .text : {
        *(.text .text.*)
    }

    .rodata : {
        *(.rodata .rodata.*)
    }
    .data : {
        *(.data .data.*)
    }

    .bss : {
        *(COMMON)
        *(.bss .bss.*)
    }
}

OUTPUT_FORMAT(binary)