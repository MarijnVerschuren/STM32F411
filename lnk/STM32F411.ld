MEMORY {
    FLASH(rx) : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM(rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

INCLUDE "lnk/flash.ld"
INCLUDE "lnk/sram.ld"