
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

// Winbond W25Q Page Program Time: Typical = 0.4ms, Maximum = 3.0ms

static const uint16_t full_page_size = (uint16_t) WINBOND_FLASH_PAGE_SIZE;

static uint32_t winbond_page_program(pico_spi_device_t *pico_spi, uint32_t page_address, const uint8_t *page_data, uint16_t page_length);
static uint32_t winbond_reflash_4k_sector(pico_spi_device_t *flash_spi, uint32_t sector_address);

uint32_t winbond_program_full_page(pico_spi_device_t *pico_spi, uint32_t full_page_address, const uint8_t *full_page_data) {

    uint32_t bytes_programmed = 0;

    if (full_page_address % full_page_size == 0) {

        bytes_programmed = winbond_page_program(pico_spi, full_page_address, full_page_data, full_page_size);
    
    } else {

        printf("ERROR: Winbond Full Page Program Address (%08X) is not 256-Byte aligned (%08X)\n", 
            (uint)full_page_address, (uint)(full_page_address % full_page_size));

    }

    return bytes_programmed;
  
}

uint32_t winbond_program_part_page(pico_spi_device_t *pico_spi, uint32_t part_page_address, const uint8_t *part_page_data, uint16_t part_page_length) {

    uint32_t bytes_programmed = 0;

    if (part_page_length > 0 && part_page_length < 256) {

        bytes_programmed = winbond_page_program(pico_spi, part_page_address, part_page_data, part_page_length);

    } else if (part_page_length == 256 && (part_page_address % part_page_length == 0)) {

        bytes_programmed = winbond_page_program(pico_spi, part_page_address, part_page_data, part_page_length);

    } else {

        printf("ERROR: Winbond Part Page Program, length is zero or address not 256-Byte aligned (%08X)\n", (uint)part_page_address);
    }

    return bytes_programmed;
  
}

static uint32_t winbond_page_program(pico_spi_device_t *pico_spi, uint32_t page_address, const uint8_t *page_data, uint16_t page_length) {

    int spi_write_bytes = 0;

    winbond_set_command_and_address(PAGE_PROGRAM, page_address);

    winbond_set_write_enabled(pico_spi, true);

    if (winbond_get_write_enabled(pico_spi)) {

        winbond_chip_select(pico_spi);

        spi_write_bytes = spi_write_blocking(pico_spi->device, command_buffer, 4);

        if (spi_write_bytes > 0) {

            winbond_wait_for_spi_idle(pico_spi); // wait for page program command to complete

            spi_write_bytes = spi_write_blocking(pico_spi->device, page_data, page_length);

        }

        winbond_chip_release(pico_spi);

        winbond_wait_for_write_complete(pico_spi);  

        return (uint32_t)spi_write_bytes;

    } else {

        printf("ERROR: Winbond Flash is not write-enabled\n");

        return 0;

    }
}

uint32_t winbond_update_4k_sector(pico_spi_device_t *pico_spi, uint32_t sector_address, uint32_t offset_in_sector, const uint8_t *new_data, size_t new_length) {

    uint32_t spi_read_bytes = 0;
    uint32_t sector_overrun_address = sector_address + new_length;
    uint32_t sector_boundary_address = sector_address % WINBOND_FLASH_4K_SECTOR_SIZE;

    if (sector_boundary_address == 0 && new_length <= WINBOND_FLASH_4K_SECTOR_SIZE) {

        if (sector_overrun_address <= WINBOND_FLASH_LAST_4K_SECTOR) {

            spi_read_bytes = winbond_read_data_dma(pico_spi, sector_address, FLASH_SECTOR_BUFFER, WINBOND_FLASH_4K_SECTOR_SIZE);

            if (spi_read_bytes == WINBOND_FLASH_4K_SECTOR_SIZE) {

                memcpy(&FLASH_SECTOR_BUFFER[offset_in_sector], new_data, new_length);
            
                winbond_reflash_4k_sector(pico_spi, sector_address);

            } else {

                printf("ERROR: Failed to read 4K sector bytes from flash\n");

            }

        } else {

            printf("ERROR: Winbond 4K Sector Update address beyond last 4K sector\n");

        }

    } else {

        printf("ERROR: Winbond 4K Sector Update address is not 4K address aligned\n");

    }

    return spi_read_bytes;
}

static uint32_t winbond_reflash_4k_sector(pico_spi_device_t *pico_spi, uint32_t sector_address) {
    
    uint32_t erase_bytes = winbond_erase_4k_sector(pico_spi, sector_address);

    if (erase_bytes) {

        for (uint32_t offset = 0; offset < WINBOND_FLASH_4K_SECTOR_SIZE; offset += WINBOND_FLASH_PAGE_SIZE) {

            winbond_program_full_page(pico_spi, sector_address + offset, &FLASH_SECTOR_BUFFER[offset]);

        }

    }

    return erase_bytes;

}
