
#include "pico/stdlib.h"

#include "winbond_W25Q_main.h"
#include "winbond_W25Q_init.h"
#include "winbond_W25Q_core.h"
#include "winbond_W25Q_read.h"
#include "winbond_W25Q_write.h"
#include "winbond_W25Q_erase.h"
#include "winbond_W25Q_query.h"

// collate winbond init/read/write/erase/query functions from contributing source files

uint32_t lib_winbond_program_flash_page(pico_spi_device_t *pico_spi, uint32_t flash_page_address, const uint8_t *flash_page_data, uint16_t flash_page_length) {

    if (flash_page_length == 0) {

        return 0;

    } else if (flash_page_length == 256) {

        return lib_winbond_program_full_page(pico_spi, flash_page_address, flash_page_data);

    } else {

        return lib_winbond_program_part_page(pico_spi, flash_page_address, flash_page_data, flash_page_length);
    }

}

uint32_t lib_winbond_program_full_page(pico_spi_device_t *pico_spi, uint32_t full_page_address, const uint8_t *full_page_data) {

    return winbond_program_full_page(pico_spi, full_page_address, full_page_data);
}

uint32_t lib_winbond_program_part_page(pico_spi_device_t *pico_spi, uint32_t part_page_address, const uint8_t *part_page_data, uint16_t part_page_length) {

    return winbond_program_part_page(pico_spi, part_page_address, part_page_data, part_page_length);
}

uint32_t lib_winbond_update_4k_sector(pico_spi_device_t *pico_spi, uint32_t sector_address, uint32_t offset_in_sector, const uint8_t *new_data, size_t new_length) {

    return winbond_update_4k_sector(pico_spi, sector_address, offset_in_sector, new_data, new_length);
}

uint32_t lib_winbond_erase_4k_sector(pico_spi_device_t *pico_spi, uint32_t erase_address) {

    return winbond_erase_4k_sector(pico_spi, erase_address);
}

uint32_t lib_winbond_erase_32k_block(pico_spi_device_t *pico_spi, uint32_t erase_address) {

    return winbond_erase_32k_block(pico_spi, erase_address);
}

uint32_t lib_winbond_erase_64k_block(pico_spi_device_t *pico_spi, uint32_t erase_address) {

    return winbond_erase_64k_block(pico_spi, erase_address);
}

uint32_t lib_winbond_erase_size(pico_spi_device_t *pico_spi, uint32_t erase_address, size_t erase_size) {

    return winbond_erase_size(pico_spi, erase_address, erase_size);
}

int lib_winbond_full_chip_erase(pico_spi_device_t *pico_spi) {

    return winbond_full_chip_erase(pico_spi);
}

int lib_winbond_read_data_spi(pico_spi_device_t *pico_spi, uint32_t read_address, uint8_t *read_buffer, size_t read_length) {

    return winbond_read_data_spi(pico_spi, read_address, read_buffer, read_length);
}

uint32_t lib_winbond_read_data_dma(pico_spi_device_t *pico_spi, uint32_t read_address, uint8_t *read_buffer, size_t read_length) {

    return winbond_read_data_dma(pico_spi, read_address, read_buffer, read_length);
}

int lib_winbond_query_unique_id(pico_spi_device_t *pico_spi, uint8_t *query_result) {

    return winbond_query_unique_id(pico_spi, query_result);
}

int lib_winbond_query_device_id(pico_spi_device_t *pico_spi, uint8_t *query_result) {

    return winbond_query_device_id(pico_spi, query_result);
}

int lib_winbond_query_jedec_id(pico_spi_device_t *pico_spi, uint8_t *query_result) {

    return winbond_query_jedec_id(pico_spi, query_result);
}

void lib_winbond_init_all(pico_spi_device_t *pico_spi) {

    winbond_init_all(pico_spi);
}



