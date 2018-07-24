MEMORY {
    VECTORS (rx) : ORIGIN = 0x00102000, LENGTH = 0x00000090
        FLASH (rx) : ORIGIN = 0x00102090, LENGTH = 320K - 4K - 0x90
        RAM (rwx) : ORIGIN = 0x3FFF4000, LENGTH = 48K
    }
    ENTRY(Reset_Handler)
    SECTIONS {
.isr_vector :
    {
        __vector_table = .;
        KEEP(*(.vector_table))
        . = ALIGN(4);
    } > VECTORS
    __UVISOR_SRAM_OFFSET = 0x90;
    __UVISOR_BSS_START = ORIGIN(RAM) + __UVISOR_SRAM_OFFSET;
    .uvisor.bss __UVISOR_BSS_START (NOLOAD):
    {
        . = ALIGN(32);
        __uvisor_bss_start = .;
        . = ALIGN(32);
        __uvisor_bss_main_start = .;
        KEEP(*(.keep.uvisor.bss.main))
        . = ALIGN(32);
        __uvisor_bss_main_end = .;
        . = ALIGN(32);
        __uvisor_bss_boxes_start = .;
        KEEP(*(.keep.uvisor.bss.boxes))
        . = ALIGN(32);
        __uvisor_bss_boxes_end = .;
        . = ALIGN(32);
        __uvisor_bss_end = .;
    } > RAM
.text :
    {
        . = ALIGN(4);
        __uvisor_main_start = .;
        *(.uvisor.main)
        __uvisor_main_end = .;
        *(.text*)
        KEEP(*(.init))
        KEEP(*(.fini))
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
        *(.rodata*)
        KEEP(*(.eh_frame*))
    } > FLASH
.ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH
.ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FLASH
.data :
    {
        PROVIDE( __etext = LOADADDR(.data) );
        __data_start__ = .;
        *(vtable)
        *(.data*)
        . = ALIGN(4);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(4);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(32);
        __data_end__ = .;
    } >RAM AT>FLASH
.uvisor.secure :
    {
        . = ALIGN(32);
        __uvisor_secure_start = .;
        . = ALIGN(32);
        __uvisor_cfgtbl_start = .;
        KEEP(*(.keep.uvisor.cfgtbl))
        . = ALIGN(32);
        __uvisor_cfgtbl_end = .;
        __uvisor_cfgtbl_ptr_start = .;
        KEEP(*(.keep.uvisor.cfgtbl_ptr_first))
        KEEP(*(.keep.uvisor.cfgtbl_ptr))
        __uvisor_cfgtbl_ptr_end = .;
        __uvisor_data_src = .;
        __uvisor_data_start = .;
        __uvisor_data_end = .;
        . = ALIGN(32);
        __uvisor_secure_end = .;
    } >FLASH
    .uninitialized (NOLOAD):
    {
        . = ALIGN(32);
        __uninitialized_start = .;
        *(.uninitialized)
        KEEP(*(.keep.uninitialized))
        . = ALIGN(32);
        __uninitialized_end = .;
    } > RAM
    .bss (NOLOAD):
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > RAM
    .heap (NOLOAD):
    {
        __end__ = .;
        end = __end__;
        *(.heap*);
        . += 0x800;
        __HeapLimit = .;
    } > RAM
    PROVIDE(__heap_size = SIZEOF(.heap));
    PROVIDE(__mbed_sbrk_start = ADDR(.heap));
    PROVIDE(__mbed_krbs_start = ADDR(.heap) + SIZEOF(.heap));
    .stack (NOLOAD):
    {
        __StackLimit = .;
        *(.stack*);
        . += 0x800 - (. - __StackLimit);
    } > RAM
    __StackTop = ORIGIN(RAM) + LENGTH(RAM);
    __StackLimit = __StackTop - SIZEOF(.stack);
    PROVIDE(__stack = __StackTop);
}
