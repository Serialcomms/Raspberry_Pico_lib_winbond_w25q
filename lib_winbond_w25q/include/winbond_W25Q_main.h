
#ifndef SPIFFS_PICO_WINBOND_MAIN_H
#define SPIFFS_PICO_WINBOND_MAIN_H

#include "hardware/dma.h"
#include "hardware/spi.h"
#include "hardware/regs/spi.h"

#include "winbond_W25Q_init.h"
#include "winbond_W25Q_core.h"

extern uint8_t FLASH_SECTOR_BUFFER[WINBOND_FLASH_4K_SECTOR_SIZE];

extern uint8_t command_buffer[12] __attribute__((aligned(4))); 

extern void winbond_chip_select(pico_spi_device_t *pico_spi);
extern void winbond_chip_release(pico_spi_device_t *pico_spi);

void lib_winbond_init_all(pico_spi_device_t *pico_spi);

/**
 * Program a full or part 256-byte winbond flash page 
 * 
 * Target page must have previously been erased or known to contain all 0xff values
 * @param spi SPI Device Number (spi0 or spi1)
 * @param address 32-Bit Flash write address 
 * @param buffer Pointer to source byte buffer used to program flash
 * @return Number of flash bytes programmed
 */
uint32_t lib_winbond_program_flash_page(pico_spi_device_t *pico_spi, uint32_t flash_page_address, const uint8_t *flash_page_data, uint16_t flash_page_length);

/**
 * Program a full 256-byte flash page (must be 256-byte address aligned)
 * 
 * Target page must have previously been erased or known to contain all 0xff values
 * @param spi SPI Device Number (spi0 or spi1)
 * @param address 32-Bit Flash write address 
 * @param buffer Pointer to source byte buffer used to program flash
 * @return Number of flash bytes programmed
 */
uint32_t lib_winbond_program_full_page(pico_spi_device_t *pico_spi, uint32_t full_page_address, const uint8_t *full_page_data);

uint32_t lib_winbond_program_part_page(pico_spi_device_t *pico_spi, uint32_t part_page_address, const uint8_t *part_page_data, uint16_t part_page_length);

uint32_t lib_winbond_update_4k_sector(pico_spi_device_t *pico_spi, uint32_t sector_address, uint32_t offset_in_sector, const uint8_t *new_data, size_t new_length);

/**
 * Erase all 4096 bytes in a flash sector - (erase address must be 4096-byte aligned)
 * 
 * Erased bytes are set to 0xff - this is a hardware feature of the flash technology used
 * 
 * \warning Note that erasing a 4K sector can take between 40 and 400 milliseconds
 *
 * @param spi SPI Device Number (spi0 or spi1)
 * @param address 32-Bit Flash erase start address - must be aligned and not exceed (flash size - 4096)
 */
uint32_t lib_winbond_erase_4k_sector(pico_spi_device_t *pico_spi, uint32_t erase_address);

uint32_t lib_winbond_erase_32k_block(pico_spi_device_t *pico_spi, uint32_t erase_address);

uint32_t lib_winbond_erase_64k_block(pico_spi_device_t *pico_spi, uint32_t erase_address);

uint32_t lib_winbond_erase_size(pico_spi_device_t *pico_spi, uint32_t erase_address, size_t erase_size);

/**
 * Erase Entire Winbond Flash Chip 
 * 
 * Erased bytes are set to 0xff - this is a hardware feature of the flash technology used
 * 
 * \warning Note that full chip erase can take between 20 and 100 seconds.
 *
 * @param spi SPI Device Number (spi0 or spi1)
 */
int lib_winbond_full_chip_erase(pico_spi_device_t *pico_spi);

/**
 * Read data directly from flash via the SPI Interface
 *
 * @param spi SPI Device Number (spi0 or spi1)
 * @param address 32-Bit Flash read address
 * @param buffer Pointer to byte buffer where SPI data will be written
 * @param bytes Number of flash bytes requested
 * @return Number of flash bytes read
 */
// pico_spi_device_t PICO_SPI
 //int winbond_read_data_spi(spi_inst_t *spi_number, uint32_t read_address, uint8_t *read_buffer, size_t read_length);

int lib_winbond_read_data_spi(pico_spi_device_t *pico_spi, uint32_t read_address, uint8_t *read_buffer, size_t read_length);

/** 
* Read data directly from flash via the DMA Interface
 *
 * @param spi SPI Device Number (spi0 or spi1)
 * @param address 32-Bit Flash read address
 * @param buffer Pointer to byte buffer where SPI data will be written
 * @param bytes Number of flash bytes requested
 * @return Number of flash bytes read
 */
uint32_t lib_winbond_read_data_dma(pico_spi_device_t *pico_spi, uint32_t read_address, uint8_t *read_buffer, size_t read_length);

int lib_winbond_query_device_id(pico_spi_device_t *pico_spi, uint8_t *query_result);

int lib_winbond_query_unique_id(pico_spi_device_t *pico_spi, uint8_t *query_result);

int lib_winbond_query_jedec_id(pico_spi_device_t *pico_spi, uint8_t *query_result);


#endif