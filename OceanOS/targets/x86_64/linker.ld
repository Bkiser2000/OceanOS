ENTRY(start)

SECTIONS
{
    . = 1M;

    /* Place multiboot header at the very beginning */
    .multiboot_header ALIGN(8) : {
        KEEP(*(.multiboot_header))
    }

    .text ALIGN(4096) : {
        *(.text*)
    }

    .rodata ALIGN(4096) : {
        *(.rodata*)
    }

    .data ALIGN(4096) : {
        *(.data*)
    }

    .bss ALIGN(4096) : {
        *(.bss*)
        *(COMMON)
    }
}
