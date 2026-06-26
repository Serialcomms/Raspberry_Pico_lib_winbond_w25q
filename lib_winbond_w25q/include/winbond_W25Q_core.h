#ifndef SPIFFS_PICO_WINBOND_CORE_H
#define SPIFFS_PICO_WINBOND_CORE_H

#define W25Q_FLASH_STATUS_BUSY_MASK 0x01 
#define W25Q_FLASH_WRITE_ENABLE_MASK 0x02  

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "winbond_W25Q_init.h"

extern uint8_t FLASH_SECTOR_BUFFER[];
extern uint8_t command_buffer[];

static const uint8_t repeat_data_ff = 0xFF;
static const uint8_t repeat_data_00 = 0x00;

typedef enum W25Q_FLASH_DIMENSIONS {

    WINBOND_FLASH_PAGE_SIZE = 256u,
    WINBOND_FLASH_4K_SECTOR_SIZE = 4096u,
    WINBOND_FLASH_32K_BLOCK_SIZE = 32768u,
    WINBOND_FLASH_64K_BLOCK_SIZE = 65536u,
    WINBOND_FLASH_TOTAL_SIZE = (WINBOND_FLASH_4K_SECTOR_SIZE * 2048u),                         // = 8,388,608 bytes = 0x4000000 = 2^23
    WINBOND_FLASH_LAST_4K_SECTOR = (WINBOND_FLASH_TOTAL_SIZE - WINBOND_FLASH_4K_SECTOR_SIZE),  // last eraseable 4K sector start address 
    WINBOND_FLASH_LAST_32K_BLOCK = (WINBOND_FLASH_TOTAL_SIZE - WINBOND_FLASH_32K_BLOCK_SIZE),  // last eraseable 32K block start address 
    WINBOND_FLASH_LAST_64K_BLOCK = (WINBOND_FLASH_TOTAL_SIZE - WINBOND_FLASH_64K_BLOCK_SIZE),  // last eraseable 64K block start address 
    WINBOND_FLASH_ADDRESS_BITS = 23,

} w25q_dimensions_t;

void clear_command_buffer();

void winbond_chip_select(pico_spi_device_t *pico_spi);
void winbond_chip_release(pico_spi_device_t *pico_spi);

void winbond_wait_for_spi_idle(pico_spi_device_t *pico_spi);
bool winbond_get_write_busy(pico_spi_device_t *pico_spi);
bool winbond_get_write_enabled(pico_spi_device_t *pico_spi);
bool winbond_set_write_enabled(pico_spi_device_t *pico_spi, bool enabled);
bool winbond_set_erase_enabled(pico_spi_device_t *pico_spi, bool enabled);
void winbond_wait_for_write_complete(pico_spi_device_t *pico_spi);
void winbond_set_command_and_address(uint8_t winbond_command, uint32_t address);
uint32_t winbond_set_dma_transfer_count_tx_rx(pico_spi_device_t *pico_spi, uint32_t dma_bytes_tx_rx);

#endif