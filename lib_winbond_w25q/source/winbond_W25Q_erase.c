#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/sync.h"
#include "hardware/regs/spi.h"
#include "winbond_W25Q_core.h"
#include "winbond_W25Q_main.h"
#include "winbond_W25Q_read.h"
#include "winbond_W25Q_write.h"
#include "winbond_W25Q_query.h"
#include "winbond_W25Q_erase.h"

// Sector & Block Erase times from Winbond W25Q datasheet
// ------------------------------------------------------
// Winbond W25Q 4k Sector Erase Time: Typical = 45ms,  Maximum = 400 ms
// Winbond W25Q 32k Block Erase Time: Typical = 120ms, Maximum = 1600 ms
// Winbond W25Q 64k Block Erase Time: Typical = 150ms, Maximum = 2000 ms
// Winbond W25Q Full Chip Erase Time: Typical = 20s,   Maximum = 100s (seconds)
 
static uint32_t start_4k_sector_erase(pico_spi_device_t *pico_spi, uint32_t erase_address);
static uint32_t start_32k_block_erase(pico_spi_device_t *pico_spi, uint32_t erase_address);
static uint32_t start_64k_block_erase(pico_spi_device_t *pico_spi, uint32_t erase_address);

static uint32_t start_full_chip_erase(pico_spi_device_t *pico_spi);

static uint32_t start_4k_sector_erase(pico_spi_device_t *pico_spi, uint32_t erase_address) {

    winbond_set_command_and_address(ERASE_4K_SECTOR, erase_address);

    winbond_set_erase_enabled(pico_spi, true);
            
    winbond_chip_select(pico_spi);
    
    int spi_write_bytes = spi_write_blocking(pico_spi->device, command_buffer, 4);

    winbond_chip_release(pico_spi);

    winbond_wait_for_write_complete(pico_spi);

    if (spi_write_bytes == 4) {

        return WINBOND_FLASH_4K_SECTOR_SIZE;

    } else {

        return 0;
    }

}

static uint32_t start_32k_block_erase(pico_spi_device_t *pico_spi, uint32_t erase_address) {

    winbond_set_command_and_address(ERASE_32K_BLOCK, erase_address);

    winbond_set_erase_enabled(pico_spi, true);
            
    winbond_chip_select(pico_spi);
    
    int spi_write_bytes = spi_write_blocking(pico_spi->device, command_buffer, 4);

    winbond_chip_release(pico_spi);

    winbond_wait_for_write_complete(pico_spi); 

    if (spi_write_bytes == 4) {

        return WINBOND_FLASH_32K_BLOCK_SIZE;

    } else {

        return 0;
    }

}

static uint32_t start_64k_block_erase(pico_spi_device_t *pico_spi, uint32_t erase_address) {

    winbond_set_command_and_address(ERASE_64K_BLOCK, erase_address);

    winbond_set_erase_enabled(pico_spi, true);
            
    winbond_chip_select(pico_spi);
    
    int spi_write_bytes = spi_write_blocking(pico_spi->device, command_buffer, 4);

    winbond_chip_release(pico_spi);

    winbond_wait_for_write_complete(pico_spi); 

    if (spi_write_bytes == 4) {

        return WINBOND_FLASH_64K_BLOCK_SIZE;

    } else {

        return 0;
    }

}

static uint32_t start_full_chip_erase(pico_spi_device_t *pico_spi) {

    winbond_set_command_and_address(ERASE_FULL_CHIP, 0);

    winbond_chip_select(pico_spi);
    
    int spi_write_bytes = spi_write_blocking(pico_spi->device, command_buffer, 1);

    winbond_chip_release(pico_spi);

    winbond_wait_for_write_complete(pico_spi);

     if (spi_write_bytes == 1) {

        return WINBOND_FLASH_TOTAL_SIZE;

    } else {

        return 0;
    }

}

uint32_t winbond_full_chip_erase(pico_spi_device_t *pico_spi) {

    if (spi_is_writable(pico_spi->device)) {

        winbond_set_erase_enabled(pico_spi, true);

        bool write_enabled = winbond_get_write_enabled(pico_spi);
    
        if (write_enabled) {

            printf("Starting Winbond Full Flash Chip Erase - allow 20-100 seconds to complete.\n");

            start_full_chip_erase(pico_spi);

        } else {

            printf("ERROR: Full Chip Erase failed, chip not write-enabled\n");

        }            

    }  else {

        printf("ERROR: Full Chip Erase failed, SPI not writable\n");
          
    } 

    return WINBOND_FLASH_TOTAL_SIZE;

}

uint32_t winbond_erase_4k_sector(pico_spi_device_t *pico_spi, uint32_t erase_address) {

    int sector_boundary_address = erase_address % WINBOND_FLASH_4K_SECTOR_SIZE;

    if (sector_boundary_address == 0 && erase_address <= WINBOND_FLASH_LAST_4K_SECTOR) {

        if (spi_is_writable(pico_spi->device)) {

            return start_4k_sector_erase(pico_spi, erase_address);

        }  else {

            printf("ERROR: Erase 4K Sector failed, SPI not writable\n");
          
        } 

    } else {

        if (sector_boundary_address != 0) {

            printf("ERROR: Winbond 4K Sector Erase address (%08X) is not 4K address aligned\n", (uint)erase_address);

        } else {

            printf("ERROR: Winbond 4K Sector Erase address (%08X) is greater than start of last 4k sector\n", (uint)erase_address);

        }

    }
   
    return 0;
}

uint32_t winbond_erase_32k_block(pico_spi_device_t *pico_spi, uint32_t erase_address) {

    int sector_boundary_address = erase_address % WINBOND_FLASH_32K_BLOCK_SIZE;

    if (sector_boundary_address == 0 && erase_address <= WINBOND_FLASH_LAST_32K_BLOCK) {

        if (spi_is_writable(pico_spi->device)) {

            return start_32k_block_erase(pico_spi, erase_address);

        }  else {

            printf("ERROR: Erase 32K Block failed, SPI not writable\n");
          
        } 

    } else {

        if (sector_boundary_address != 0) {

            printf("ERROR: Winbond 32K Block Erase address (%08X) is not 32K address aligned\n", (uint)erase_address);

        } else {

            printf("ERROR: Winbond 32K Block Erase address (%08X) is greater than start of last 32k block\n", (uint)erase_address);

        }

    }
   
    return 0;
}

uint32_t winbond_erase_64k_block(pico_spi_device_t *pico_spi, uint32_t erase_address) {

    int sector_boundary_address = erase_address % WINBOND_FLASH_64K_BLOCK_SIZE;

    if (sector_boundary_address == 0 && erase_address <= WINBOND_FLASH_LAST_64K_BLOCK) {

        if (spi_is_writable(pico_spi->device)) {

            return start_64k_block_erase(pico_spi, erase_address);

        }  else {

            printf("ERROR: Erase 64K Block failed, SPI not writable\n");
          
        } 

    } else {

        if (sector_boundary_address != 0) {

            printf("ERROR: Winbond 64K Block Erase address (%08X) is not 64K address aligned\n", (uint)erase_address);

        } else {

            printf("ERROR: Winbond 64K Block Erase address (%08X) is greater than start of last 64k block\n", (uint)erase_address);

        }

    }
   
    return 0;
}

uint32_t winbond_erase_size(pico_spi_device_t *pico_spi, uint32_t erase_address, size_t erase_size) {

    size_t size_erased = 0;

      switch (erase_size) {

        case WINBOND_FLASH_4K_SECTOR_SIZE:
        lib_winbond_erase_4k_sector(pico_spi, erase_address);
        size_erased = WINBOND_FLASH_4K_SECTOR_SIZE;
        break;

        case WINBOND_FLASH_32K_BLOCK_SIZE:
        lib_winbond_erase_32k_block(pico_spi, erase_address);
        size_erased = WINBOND_FLASH_32K_BLOCK_SIZE;
        break;
       
        case WINBOND_FLASH_64K_BLOCK_SIZE:
        lib_winbond_erase_64k_block(pico_spi, erase_address);
        size_erased = WINBOND_FLASH_64K_BLOCK_SIZE;
        break;

        default:
        printf("ERROR, unsupported Winbond flash erase size\n");
        break;

    } 

    return size_erased;

}
