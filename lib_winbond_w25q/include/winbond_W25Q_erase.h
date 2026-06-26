
#ifndef SPIFFS_PICO_WINBOND_ERASE_H
#define SPIFFS_PICO_WINBOND_ERASE_H

uint32_t winbond_full_chip_erase(pico_spi_device_t *pico_spi);

uint32_t winbond_erase_4k_sector(pico_spi_device_t *flash_spi, uint32_t erase_address);
uint32_t winbond_erase_32k_block(pico_spi_device_t *flash_spi, uint32_t erase_address);
uint32_t winbond_erase_64k_block(pico_spi_device_t *flash_spi, uint32_t erase_address);

uint32_t winbond_erase_size(pico_spi_device_t *pico_spi, uint32_t erase_address, size_t erase_size);

#endif