
#ifndef SPIFFS_PICO_WINBOND_MAIN_H
#define SPIFFS_PICO_WINBOND_MAIN_H

#include "hardware/dma.h"
#include "hardware/spi.h"
#include "hardware/regs/spi.h"

#include "winbond_W25Q_init.h"
#include "winbond_W25Q_core.h"

/**
 * @brief Initialise Pico GPIO (for external chip select), SPI and DMA channel configurations
 * @param pico_spi struct pico_spi_device_t (SPI pins, SPI instance, DMA channels etc.)
 * @note pico_spi_device_t must be defined and configured in the user program - see example
 */
void lib_winbond_init_all(pico_spi_device_t *pico_spi);

/**
 * @brief Program a full 256-byte flash page (must be 256-byte address aligned)
 * @param pico_spi struct pico_spi_device_t (SPI pins, SPI instance, DMA channels etc.)
 * @param full_page_address 32-Bit Flash write address 
 * @param full_page_data Pointer to source byte buffer used to program flash
 * @return Number of flash bytes programmed
 * @note Page Program Time = 0.4 to 3.0 ms (winbond data sheet)
 * @note Target page must have previously been erased or known to contain all 0xff values
 * 
 */
uint32_t lib_winbond_program_full_page(pico_spi_device_t *pico_spi, uint32_t full_page_address, const uint8_t *full_page_data);

/**
 * @brief Program 1-256 bytes in a flash page (256-byte part_page_length must be 256-byte address aligned)  
 * @param pico_spi struct pico_spi_device_t (SPI pins, SPI instance, DMA channels etc.)
 * @param part_page_address 32-Bit Flash write address 
 * @param part_page_data Pointer to source byte buffer used to program flash
 * @param part_page_length
 * @return Number of flash bytes programmed
 * @note Page Program Time = 0.4 to 3.0 ms (winbond data sheet)
 * @note Target page must have previously been erased or target address range known to contain all 0xff values
 */
uint32_t lib_winbond_program_part_page(pico_spi_device_t *pico_spi, uint32_t part_page_address, const uint8_t *part_page_data, uint16_t part_page_length);

/**
 * @brief Convenience function to update part of a 4k sector by reading sector contents to a buffer,
 * @brief updating the sector buffer with new data and reflashing the sector, thus preserving any 
 * @brief sector data not intended to be updated
 * @param pico_spi struct pico_spi_device_t (SPI pins, SPI instance, DMA channels etc.)
 * @param sector_address 32-Bit Flash 4k sector write start address 
 * @param offset_in_sector start offset to above
 * @param new_data byte buffer containing new data to be written
 * @param new_length length in bytes of new data to be written
 * @return Number of flash bytes programmed
 */
uint32_t lib_winbond_update_4k_sector(pico_spi_device_t *pico_spi, uint32_t sector_address, uint32_t offset_in_sector, const uint8_t *new_data, size_t new_length);

/**
 * @brief Erase all 4096 bytes in a 4k flash sector - (erase address must be 4096-byte aligned)
 * @brief Erased bytes are set to 0xff, this is a hardware feature of the NOR flash technology used
 * @param pico_spi struct pico_spi_device_t (SPI pins, SPI instance, DMA channels etc.)
 * @param erase_address 32-Bit Flash erase start address - must be aligned and not exceed (flash size - 4k)
 * @returns number of bytes erased (4k expected)
 * @warning Note that erasing a 4K sector can take between 45 and 400 milliseconds
 */
uint32_t lib_winbond_erase_4k_sector(pico_spi_device_t *pico_spi, uint32_t erase_address);

/**
 * @brief Erase all 32k bytes in a 32k flash block - (erase address must be 32k-byte aligned)
 * @brief Erased bytes are set to 0xff - this is a hardware feature of the NOR flash technology used
 * @param pico_spi struct pico_spi_device_t (SPI pins, SPI instance, DMA channels etc.)
 * @param erase_address 32-Bit Flash erase start address - must be aligned and not exceed (flash size - 32k)
 * @returns number of bytes erased (32k expected)
 * @warning Note that erasing a 32K block can take between 120 and 1600 milliseconds
 */
uint32_t lib_winbond_erase_32k_block(pico_spi_device_t *pico_spi, uint32_t erase_address);

/**
 * @brief Erase all 64k bytes in a flash block - (erase address must be 32k-byte aligned)
 * @brief Erased bytes are set to 0xff, this is a hardware feature of the NOR flash technology used
 * @param pico_spi struct pico_spi_device_t (SPI pins, SPI instance, DMA channels etc.)
 * @param erase_address 32-Bit Flash erase start address - must be aligned and not exceed (flash size - 64k)
 * @returns number of bytes erased (64k expected)
 * @warning Note that erasing a 64K block can take between 150 and 2000 milliseconds
 */
uint32_t lib_winbond_erase_64k_block(pico_spi_device_t *pico_spi, uint32_t erase_address);

/**
 * @brief Erase 4k, 32k or 64k bytes - (erase address must be 4k/32k/64k-byte aligned and within range)
 * @brief Erased bytes are set to 0xff, this is a hardware feature of the NOR flash technology used
 * @param pico_spi struct pico_spi_device_t (SPI pins, SPI instance, DMA channels etc.)
 * @param erase_address 32-Bit Flash erase start address - must be aligned and not exceed (flash size - 64k)
 * @param erase_size values 4k, 32k or 64k only 
 * @returns Number of flash bytes erased (0 if erase failed or erase size invalid)
 * @warning flash erasure can take between 45 and 2000 milliseconds
 * 
 */
uint32_t lib_winbond_erase_size(pico_spi_device_t *pico_spi, uint32_t erase_address, size_t erase_size);

/**
 * @brief Erase Entire Winbond Flash Chip 
 * @brief Erased bytes are set to 0xff - this is a hardware feature of the flash technology used
 * @param pico_spi struct pico_spi_device_t (SPI pins, SPI instance, DMA channels etc.)
 * @warning Note that full chip erase can take between 20 and 100 seconds
 */
int lib_winbond_full_chip_erase(pico_spi_device_t *pico_spi);

/**
 * @brief Read data (SPI direct) from external winbond flash
 * @param pico_spi struct pico_spi_device_t (SPI pins, SPI instance, DMA channels etc.)
 * @param read_address 32-Bit Flash read address
 * @param read_buffer Pointer to byte buffer where flash read data will be written
 * @param read_length Number of flash bytes requested
 * @returns Number of flash bytes read
 */
int lib_winbond_read_data_spi(pico_spi_device_t *pico_spi, uint32_t read_address, uint8_t *read_buffer, size_t read_length);

/** 
 * @brief Read data (DMA-assisted) from external winbond flash
 * @param pico_spi struct pico_spi_device_t (SPI pins, SPI instance, DMA channels etc.)
 * @param read_address 32-Bit Flash read address
 * @param read_buffer Pointer to byte buffer where flash read data will be written
 * @param read_length Number of flash bytes requested
 * @returns Number of flash bytes read
 */
uint32_t lib_winbond_read_data_dma(pico_spi_device_t *pico_spi, uint32_t read_address, uint8_t *read_buffer, size_t read_length);

/** 
 * @brief Query the external Winbond flash device for device ID (2 bytes)
 * @param pico_spi struct pico_spi_device_t (SPI pins, SPI instance, DMA channels etc.)
 * @param query_result min 2-byte buffer to receive device ID from windbond flash query
 * @returns Number of bytes read from winbond flash device (2 bytes expected)
 */
int lib_winbond_query_device_id(pico_spi_device_t *pico_spi, uint8_t *query_result);

/** 
 * @brief Query the external Winbond flash device for unique ID (8 bytes)
 * @param pico_spi struct pico_spi_device_t (SPI pins, SPI instance, DMA channels etc.)
 * @param query_result min 8-byte buffer to receive device ID from windbond flash query
 * @returns Number of bytes read from winbond flash device (8 bytes expected)
 */
int lib_winbond_query_unique_id(pico_spi_device_t *pico_spi, uint8_t *query_result);

/** 
 * @brief Query the external Winbond flash device for JEDEC ID (3 bytes)
 * @param pico_spi struct pico_spi_device_t (SPI pins, SPI instance, DMA channels etc.)
 * @param query_result min 3-byte buffer to receive device ID from windbond flash query
 * @returns Number of bytes read from winbond flash device (3 bytes expected)
 */
int lib_winbond_query_jedec_id(pico_spi_device_t *pico_spi, uint8_t *query_result);

#endif