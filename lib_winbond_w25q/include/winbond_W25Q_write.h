
#ifndef SPIFFS_PICO_WINBOND_WRITE_H
#define SPIFFS_PICO_WINBOND_WRITE_H

extern uint8_t FLASH_SECTOR_BUFFER[WINBOND_FLASH_4K_SECTOR_SIZE];

uint32_t winbond_update_4k_sector(pico_spi_device_t *flash_spi, uint32_t sector_addr, uint32_t offset_in_sector, const uint8_t *new_data, size_t new_len);
uint32_t winbond_program_full_page(pico_spi_device_t *flash_spi, uint32_t full_page_address, const uint8_t *full_page_data);
uint32_t winbond_program_part_page(pico_spi_device_t *pico_spi, uint32_t part_page_address, const uint8_t *part_page_data, uint16_t part_page_length);

#endif