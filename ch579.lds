/* CH579M 257 KiB FLASH, 32 KiB RAM */

MEMORY
{
    FLASH (rx) : ORIGIN = 0x0, LENGTH = 250K
    FLASH_DATA (rx) : ORIGIN = 0x0003E800, LENGTH = 2K
    FLASH_BOOTLOADER (rx) : ORIGIN = 0x0003F000, LENGTH = 4K
    FLASH_INFO (rx) : ORIGIN = 0x00040000, LENGTH = 1K
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 32K
}

_minimum_stack_size = 2048;

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(4);
        _etext = .;
        _sidata = _etext;
    } >FLASH =0xff

    .data : AT(_sidata)
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        _edata = .;
    } >RAM =0

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    /* Dodane, żeby się kompilowało za pomocą arm-none-eabi-gcc. */
    /DISCARD/ :
    {
        *(.ARM.exidx*)
    }

    _estack = (ORIGIN(RAM) + LENGTH(RAM)) & 0xFFFFFFFC;

    ASSERT(_ebss + _minimum_stack_size <= _estack,
           "There is not enough space in RAM for stack.")

    PROVIDE(end = _ebss);

    DISCARD :
    {
        libc.a(*)
        libm.a(*)
        libgcc.a(*)
    }

    .stab            0 : {*(.stab)}
    .stabstr         0 : {*(.stabstr)}
    .stab.excl       0 : {*(.stab.excl)}
    .stab.exclstr    0 : {*(.stab.exclstr)}
    .stab.index      0 : {*(.stab.index)}
    .stab.indexstr   0 : {*(.stab.indexstr)}
    .comment         0 : {*(.comment)}
    .debug           0 : {*(.debug)}
    .line            0 : {*(.line)}
    .debug_srcinfo   0 : {*(.debug_srcinfo)}
    .debug_sfnames   0 : {*(.debug_sfnames)}
    .debug_aranges   0 : {*(.debug_aranges)}
    .debug_pubnames  0 : {*(.debug_pubnames)}
    .debug_info      0 : {*(.debug_info .gnu.linkonce.wi.*)}
    .debug_abbrev    0 : {*(.debug_abbrev)}
    .debug_line      0 : {*(.debug_line)}
    .debug_frame     0 : {*(.debug_frame)}
    .debug_str       0 : {*(.debug_str)}
    .debug_loc       0 : {*(.debug_loc)}
    .debug_macinfo   0 : {*(.debug_macinfo)}
    .debug_weaknames 0 : {*(.debug_weaknames)}
    .debug_funcnames 0 : {*(.debug_funcnames)}
    .debug_typenames 0 : {*(.debug_typenames)}
    .debug_varnames  0 : {*(.debug_varnames)}
}
