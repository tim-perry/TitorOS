ENTRY(stage2)

SECTIONS
{
    . = 0x7e00;

    .text : {
        *(.text.stage2)
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